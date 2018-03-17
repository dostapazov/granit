//---------------------------------------------------------------------------
#pragma hdrstop
#include <vcl.h>
#include "modemmer_frm.h"
#include "modemmer_wnd.rh"
#include <otd.h>
#include <stdlib.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "gkmodule_form"
#pragma link "about_common_frame"
#pragma resource "*.dfm"
TModemmerFrm *ModemmerFrm;
//---------------------------------------------------------------------------
__fastcall TModemmerFrm::TModemmerFrm(TComponent* Owner,HWND owner_hwnd,GKHANDLE handle)
        : TGKModuleForm(Owner,owner_hwnd,handle)
{

 filter_enabled = false;
 worked_modems = free_modems = NULL;
 selected_addr.addr    = -1;
 selected_modem_handle = NULL;
 monitor = new TModemmerMonitor;
  KeWin::TRect r(0,0,MonitorBox->Width,MonitorBox->Height);
  r.Grow(-4,-10);
  r.Move(4,8);
  monitor->DoCreate(MonitorBox->Handle,r,-1,256,"ModemmerMonitor");
  HFONT font = CreateFont(-12,0,0,0,FW_NORMAL,0,0,0,
               RUSSIAN_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH,
               "Courier New");
  font = monitor->SetFont(font,false);
  monitor->SetBkColor(GetSysColor(COLOR_INFOBK));
  DeleteObject(font);
  rx_mon_enabled    = tx_mon_enabled = true;
  redir_mon_enabled = false;
  MonLinesCountChange(this);
 #ifdef COMERCIAL_BUILD
 //Label30->Caption = "ООО \"Компания промышленной автоматизации\"";
 #endif
 DoubleBuffered = true;
 //this->RedirectSheet->Visible = false;

}
//---------------------------------------------------------------------------

void       __fastcall  set_node_image_index(TTreeNode * node,int idx)
{
  node->ImageIndex    = idx;
  node->StateIndex    = idx;
  node->SelectedIndex = idx;
}


void        __fastcall TModemmerFrm::end_module_command(GKHANDLE mod,DWORD ret)
{
 wchar_t err_text[1024];
 *err_text = 0;
 if(ret == (DWORD)GKH_RET_ERROR)
 {
  TGKModuleInterface(mod,true).get_error_text(GetLastError(),err_text,sizeof(err_text)/sizeof(wchar_t));
  StatusBar1->Panels->Items[1]->Text = err_text;
 }
}

int __stdcall modem_addr_sort(LPARAM p1,LPARAM p2,LPARAM param)
{
 TTreeNode * n1 = (TTreeNode *)p1;
 TTreeNode * n2 = (TTreeNode *)p2;
 modem_data * md1,*md2;
 md1 = (modem_data *)n1->Data;md2 =(modem_data *)n2->Data;
 return  modem_addr_cmp(md1->addr,md2->addr);
}

void        __fastcall TModemmerFrm::on_module_state(GKHANDLE mod,DWORD state)
{
  bool running = ((state & MODULE_STATE_RUNNING_MASK )== MODULE_STATE_RUNNING) ? true:false;
  bool stopped  = (state & MODULE_STATE_RUNNING_MASK ) ? false:true;
  TTreeNode * modem_node = find_modem(mod);
  if(modem_node)
     {
      set_node_image_index(modem_node,(running && !stopped) ? 0:1);
      if(modem_node->Selected )
        update_modem_state(mod,state);
      if(ModemsTree->Selected && ModemsTree->Selected->Parent == modem_node)
        update_modem_state(mod,state);
         ModemsTree->Update();  
     }
     else
     {
      if(is_same_gkhandle(mod,mod_iface()))
      {
        on_modem_change();
      }
     }
}

void        __fastcall TModemmerFrm::on_module_config_change(GKHANDLE mod,LPMODULE_CONFIG_DATA mcd)
{

}

LRESULT __fastcall TModemmerFrm::on_gkhandle_notify(GKHANDLE from,LPNOTIFY_CODE nc,DWORD mask)
{

  if(mask==MNF_MODEM)
  {
   switch(nc->notify_code)
   {
    case MNCODE_LINE_STATE  : on_modem_line_state((LPMODEM_LINE_STATE)nc->notify_data);
                             break;
    case MNCODE_MODEM_NUMBER: on_modem_change_number((LPMODEM_CHANGE_NUMBER)nc->notify_data);
                              break;
   }
   return GKH_RET_SUCCESS;
  }

  if(mask == MNF_MODEMMER)
  {
   on_modemmer_txrx((LPMPROTO_HEADER)nc->notify_data,(nc->dw_size - sizeof(*nc)+sizeof(nc->notify_data)),nc->notify_code );
   return GKH_RET_SUCCESS;
  }
  if(mask == MNF_MODEMMER_REDIRECT)
  {
   switch(nc->notify_code)
   {
    case MNCODE_REDIRECT_ADD   : on_redir_change(nc->notify_data[0],true);break;
    case MNCODE_REDIRECT_REMOVE: on_redir_change(nc->notify_data[0],false);break;
   }
   return GKH_RET_SUCCESS;
  }
  return TGKModuleForm::on_gkhandle_notify(from,nc,mask);
}

TTreeNode * __fastcall TModemmerFrm::find_modem(DWORD number,GKHANDLE handle)
{
 /*Поиск ветки модема*/
 bool wm = number <MAX_MODEM_COUNT ? true:false;
 TTreeNode * root =  wm ? worked_modems:free_modems;
 for(int i = 0;i < root->Count;i++)
 {
  TTreeNode * node = root->Item[i];
  modem_data md = get_modem_data(node);
  if(wm)
  {
   if((DWORD)md.addr.modem == number)
     return node;
  }
  else
  {
   if(is_same_gkhandle(md.hModem,handle))
      return node;
  }
 }
 return NULL;
}

TTreeNode * __fastcall TModemmerFrm::find_modem(GKHANDLE mod)
{
 /*Поиск ветки модема*/
 TTreeNode * root =   worked_modems;
 for(int i = 0;i < root->Count;i++)
 {
  TTreeNode * node = root->Item[i];
  modem_data md = get_modem_data(node);
   if(is_same_gkhandle(md.hModem,mod))
      return node;
  }
root =   free_modems;
 for(int i = 0;i < root->Count;i++)
 {
  TTreeNode * node = root->Item[i];
  modem_data md = get_modem_data(node);
   if(is_same_gkhandle(md.hModem,mod))
      return node;
  }
 return NULL;  

}

TTreeNode * __fastcall TModemmerFrm::find_modem_line(TTreeNode * modem_node,DWORD line)
{
 TTreeNode * ret = NULL;

 for(int i = 0;modem_node && i<modem_node->Count;i++)
    {
      ret = modem_node->Item[i];
      if((DWORD)get_modem_data(ret).addr.line == line)
        break;
      ret = NULL;  
    }
  return ret;
}

TTreeNode * __fastcall TModemmerFrm::find_node (MODEM_ADDR & addr)
{
  TTreeNode * node = find_modem(addr.modem,NULL);
  if(node && addr.line< MODEM_ADDR_MAXVALUE)
      node = find_modem_line(node,addr.line);
  return node;
}



void    __fastcall TModemmerFrm::on_added_module  (GKHANDLE owner,GKHANDLE mod,DWORD mod_type)
{
 /*Добавление модема*/

 if(mod_type & MT_MODEM)
 {
  TGKModuleInterface  mi(mod,true);
  MODEM_ADDR addr;addr.addr = -1;
  addr.modem =(BYTE) mi.call(MODEMCM_GET_NUMBER,0,0);
  TTreeNode * modem_node = find_modem(DWORD(addr.modem),mod);
  if(!modem_node)
    {
     add_modem_node(mod,addr);
    }
 }

}

void    __fastcall TModemmerFrm::on_removed_module(GKHANDLE owner,GKHANDLE mod,DWORD mod_type)
{
 /*Удаление модема*/

 if(mod_type & MT_MODEM)
 {
  TTreeNode * modem_node = find_modem(mod);
  if(modem_node)
     modem_node->Delete();
 }
}




void __fastcall TModemmerFrm::FormShow(TObject *Sender)
{
 set_notify(mod_iface(),MNF_COMMON|MNF_MODEM|( PageControl1->TabIndex==1 ? MNF_MODEMMER:0));
}
//---------------------------------------------------------------------------
void __fastcall TModemmerFrm::FormHide(TObject *Sender)
{
  reset_notify(mod_iface(),-1);
}
//---------------------------------------------------------------------------

void    __fastcall TModemmerFrm::after_set_gkhandle()
{
 TGKModuleForm::after_set_gkhandle();
 worked_modems =  ModemsTree->Items->Add(0,load_rcstring(ModuleInstance,IDS_MODEMS_LIST));
 free_modems = ModemsTree->Items->Add(0,load_rcstring(ModuleInstance,IDS_MODEMS_LIST_NONUMBER)/*"Модемы без номера"*/);
 set_node_image_index(worked_modems,12);
 set_node_image_index(free_modems,12);
 Tree_setup_modems();
 Tree_setup_freemodems();
 worked_modems->Expand(false);
 worked_modems->Selected = true;
 selected_addr.addr = 0;
 ModemsTreeChange(this,worked_modems);
}

TTreeNode * __fastcall TModemmerFrm::add_modem_node(GKHANDLE modem,MODEM_ADDR &addr)
{
 AnsiString str;
 str=load_rcstring(ModuleInstance,IDS_MODEM);
 TTreeNode * root;
 char  buf[MAX_PATH<<1];
 LPMODEM_PROPS mp  = this->get_modem_props(modem,buf,sizeof(buf));
 
 TGKModuleInterface mi(modem,true);
 set_notify(modem,MNF_COMMON);
 if(mp)
   str = mp->modem_info;
 if((DWORD)addr.modem<MAX_MODEM_COUNT)
 {
  root = worked_modems;
  str.cat_printf(" № %d",DWORD(addr.modem));
 }
 else
 {
  root = free_modems;
 }
 if(root)
    {
    TGKModuleInterface (modem,true).set_notify(form_gkhandle,MNF_COMMON,TRUE);
    root = this->ModemsTree->Items->AddChild(root,str);
    set_node_image_index(root,mi.is_running() ? 0:1);
    root->Data       = new modem_data(modem,addr);
    
    }
 return root;
}

void __fastcall TModemmerFrm::Tree_setup_modems()
{
  //Формирование списка модемов
  MODEM_ADDR addr;
  DWORD idx = 0;

  while(mod_iface.call(MDMCM_ENUM_MODEMS,idx++,(LPARAM)&addr) == GKH_RET_SUCCESS)
  {
    TTreeNode * modem_node = add_modem_node((GKHANDLE)mod_iface.call(MDMCM_GET_MODEM_HANDLE,(LPARAM)addr.modem,0),addr);
    Tree_setup_modem_lines(modem_node,addr);
  }
}

void __fastcall TModemmerFrm::Tree_setup_freemodems()
{
 GKHANDLE modem_handle;
 MODEM_ADDR addr;
 addr.addr = -1;
 DWORD idx = 0;
 while(mod_iface.call(MDMCM_ENUM_FREEMODEMS,idx++,(LPARAM)&modem_handle) == GKH_RET_SUCCESS)
      {
       add_modem_node(modem_handle,addr);
      }

}

TTreeNode * __fastcall TModemmerFrm::add_line_node(TTreeNode * modem,MODEM_ADDR & addr)
{
  if(!modem)
      modem =  find_modem(addr.modem,NULL);
  if(modem)
  {
   char buff[MAX_PATH<<1];
   modem_data md = get_modem_data(modem);

   LPMODEM_LINE_PROPS mlp = this->get_line_props(md.hModem,addr.line,buff,sizeof(buff));
   if(mlp)
     return add_line_node(modem,mlp);
  }
  return NULL;
}

TTreeNode * __fastcall TModemmerFrm::add_line_node(TTreeNode * modem,LPMODEM_LINE_PROPS mlp)
{
  if(!modem)
      modem =  find_modem(mlp->addr.modem,NULL);
  if(modem)
  {
   UnicodeString s ;
   UnicodeString line_info;
   if(mlp->line_text_len)
      s = UnicodeString(mlp->line_text,mlp->line_text_len);
      else
      {
      s = load_rcstring(ModuleInstance,IDS_LINE);
      s.cat_printf(L" № %d",line_info.c_str(),(DWORD)mlp->addr.line);
      }
   TTreeNode * line = ModemsTree->Items->AddChild(modem,s);
   set_node_image_index(line,(mlp->line_state&MODEM_LINE_STATE_CONNECT) ? 14:15);
   modem_data * md = new modem_data(get_modem_data(modem).hModem,mlp->addr);
   line->Data = md;
   return line;
  }
 return NULL;
}

void __fastcall TModemmerFrm::Tree_setup_modem_lines(TTreeNode * node,MODEM_ADDR & addr)
{
  DWORD idx    = 0;
  char buffer[MAX_PATH<<1];
  ZeroMemory(buffer,sizeof(buffer));
  LPMODEM_LINE_PROPS mlp = (LPMODEM_LINE_PROPS)buffer;
  mlp->dw_size      = sizeof(buffer);
  mlp->addr.addr    = addr.addr;
  mlp->line_text_sz = (sizeof(buffer)-sizeof(*mlp))/sizeof(wchar_t);
  while(mod_iface.call(MDMCM_ENUM_MODEM_LINES,idx++,(LPARAM)mlp) == GKH_RET_SUCCESS)
  {
    mlp->addr.modem = addr.modem;
    add_line_node(node,mlp);
  }
}

void        __fastcall TModemmerFrm::on_modem_change()
{
 bool enable_controls = false;
 StatusBar1->Panels->Items[0]->Text = "";
 StatusBar1->Panels->Items[1]->Text = "";
 if(selected_modem_handle && selected_modem_handle !=(GKHANDLE)-1)
 {
   TGKModuleInterface mi(selected_modem_handle,true);
   update_modem_state(selected_modem_handle,mi.get_module_state());

   enable_controls = true;
   if(selected_addr.modem)
   {
    ModemNumber->Position      = (DWORD)selected_addr.modem;
    SetMNBtn->Enabled = false;
    MiRemoveModem->Enabled     = true;
    RemoveModemBtn->Enabled     = true;
   }
   set_notify(selected_modem_handle,MNF_COMMON);
 }
 else
 {
  bool is_run = mod_iface.is_running();
  MiRemoveModem->Enabled     = false;
  MiStopModem  ->Enabled     = is_run;
  MiStartModem ->Enabled     = !is_run;
  StartBtn->Enabled          = !is_run;
  StopBtn-> Enabled          = is_run;
  MiRemoveModem->Enabled     = false;
  RemoveModemBtn->Enabled = false;
 }

 MiSettings->Enabled = enable_controls && selected_addr.modem;
 PropBtn-> Enabled   = enable_controls && selected_addr.modem;
 MiView->Enabled     = enable_controls && selected_addr.modem;
 ViewBtn-> Enabled   = enable_controls && selected_addr.modem;
 RefreshBtn->Enabled = enable_controls && selected_addr.modem;
 MiRefresh->Enabled  = enable_controls && selected_addr.modem;
 MnPanel->Visible    = enable_controls && selected_addr.modem;
 SetMNBtn->Visible   = enable_controls && selected_addr.modem;
 ToolBar1->Update();

 if(PageControl1->ActivePageIndex)
 {

 }
 else
 {
  AnsiString str;
  ModemBox->Visible = enable_controls;
  if(enable_controls)
  {
    /*Просмотр свойств модема / линии*/
    MODEM_PROPS mp ;
    get_modem_props(selected_modem_handle,(LPBYTE)&mp,sizeof(mp));
    ModemLinesCount->Caption = mp.lines_count;
    ModemMaxLinesCount->Caption = mp.lines_limit;
    if(mp.flags&MPF_DATA_SOURCE)
       str+= "Источник ";
    if(mp.flags&MPF_DATA_ACCEPTOR)
       str+= " Потребитель";
    ModemFlags->Caption = str;   
  }
 }
}


void        __fastcall TModemmerFrm::on_line_change ()
{
  if(selected_addr.line< MAX_MODEM_LINES_COUNT)
  {
  char buffer[MAX_PATH];
  LPMODEM_LINE_PROPS mlp = this->get_line_props(selected_modem_handle,selected_addr.line,buffer,sizeof(buffer));
  if(mlp)
  {
  AnsiString str ;//= "Неопределено";
  str = load_rcstring(ModuleInstance,IDS_UNDEF_SPEED);
  if(mlp->rx_speed == DWORD(-1))
     RxSpeed->Caption = str;
     else
     RxSpeed->Caption        = mlp->rx_speed;
  if(mlp->tx_speed == DWORD(-1))
     TxSpeed->Caption = str;
   else
    TxSpeed->Caption        = mlp->tx_speed;
  if(mlp->line_text_len)
      ModemsTree->Selected->Text = mlp->line_text;
    this->MaxFrameSize->Caption  = (mlp->max_frame_size == (DWORD)-1) ? AnsiString("Неограничено") : AnsiString(mlp->max_frame_size);
  }    
      line_connect_time = 0;
      update_line_stats();
      LineBox->Visible = true;
  }
  else
    LineBox->Visible = false;

  Timer1->Enabled  = (LineBox->Visible && this->PageControl1->ActivePageIndex == 0) ? true:false;
}

void        __fastcall TModemmerFrm::update_modem_state(GKHANDLE  h_modem, DWORD state)
{
 AnsiString text ;
 if(h_modem)
 {
  bool running = ((state & MODULE_STATE_RUNNING_MASK )== MODULE_STATE_RUNNING) ? true:false;
  bool stopped  = (state & MODULE_STATE_RUNNING_MASK ) ? false:true;
 if(is_same_gkhandle(selected_modem_handle , h_modem))
 {
  MiStopModem->Enabled = running;
  StopBtn->Enabled  = running;
  MiStartModem->Enabled = stopped;
  StartBtn->Enabled = stopped;
  if(selected_addr.line==MODEM_ADDR_MAXVALUE)
    set_node_image_index(ModemsTree->Selected,(running && !stopped) ? 0:1);
    
  text = load_rcstring(ModuleInstance,IDS_MODEM);
  if(running)
   text+= load_rcstring(ModuleInstance,IDS_WORK);
  if(stopped)
   text+= load_rcstring(ModuleInstance,IDS_STOP);
  if((state & MODULE_STATE_RUNNING_MASK )== MODULE_STATE_START_PENDING)
      text+= load_rcstring(ModuleInstance,IDS_START_PENDING);
  if((state & MODULE_STATE_RUNNING_MASK )== MODULE_STATE_STOP_PENDING)
      text+= load_rcstring(ModuleInstance,IDS_STOP_PENDING);

 }
 }
 StatusBar1->Panels->Items[0]->Text = text;
}

void        __fastcall TModemmerFrm::update_line_state(MODEM_ADDR & ma ,DWORD line_state)
{
  AnsiString str;
  if(ma.line < MAX_MODEM_LINES_COUNT)
  {
   AnsiString tmpl = load_rcstring(ModuleInstance,IDS_LINE_STATE);
   str.printf(tmpl.c_str(),ma.line);
   if(line_state & MODEM_LINE_STATE_WORKED)
     {
       if(!(line_state & MODEM_LINE_STATE_CONNECT))
          str+=load_rcstring(ModuleInstance,IDS_LOST_CONNECT) ;
          else
       if(line_state & MODEM_LINE_STATE_TXREADY)
          str+=load_rcstring(ModuleInstance,IDS_TX_READY);
       if(line_state & MODEM_LINE_STATE_RXDATA)
          str+=load_rcstring(ModuleInstance,IDS_RX_DATA );
       if(line_state&MODEM_LINE_STATE_CONNECTING)
          str+=load_rcstring(ModuleInstance,IDS_LINE_CONNECTING );
       if(line_state&MODEM_LINE_STATE_DISCONNECTING)
          str+=load_rcstring(ModuleInstance,IDS_LINE_DISCONNECTING );

     }
     else
     str=load_rcstring(ModuleInstance,IDS_LINE_NOTACTIVE);
   }
     StatusBar1->Panels->Items[1]->Text = str;
}

void        __fastcall TModemmerFrm::update_line_stats()
{

  MODEM_LINE_STATS mls;
  AnsiString str;
  if(selected_addr.line<MAX_MODEM_LINES_COUNT)
  {
  mls.dw_size   = sizeof(mls);
  mls.addr.addr = selected_addr.addr;
  if(call_gkhandle(selected_modem_handle,MODEMCM_GET_LINE_STATS,(LPARAM)selected_addr.line,(LPARAM)&mls)==GKH_RET_SUCCESS)
  {
   if(mls.line_state&MODEM_LINE_STATE_CONNECT)
    {
     line_connect_time = NS100_MSEC(GetTime(false)-mls.begin_connect_time.QuadPart);
     char  buf[32] ;
     secunds2HMS(line_connect_time/1000,buf,sizeof(buf));
     LineWorkTime->Caption =buf;
    }
   else
   LineWorkTime->Caption ="";;


   float f_value;
   TColor color ;
   LostConnection->Caption = mls.connect_lost;
   RxBytesCount->Caption   = mls.rx_bytes;
   RxBytesCount->Hint      = mls.rx_total_bytes;
   RxKadrsCount->Caption   = mls.rx_kadrs;
   RxKadrsCount->Hint      = mls.rx_total_kadrs;
   RxErrorsCount->Caption  = mls.rx_errors;
   RxErrorsCount->Hint     = mls.rx_total_errors;


   str.printf("%.2f",(float)(((float)mls.rx_middle_speed)/100.0));
   RxSpeedMiddle->Caption  = str;
   str.printf("%.2f",(float)(((float)mls.rx_max_speed)/100.0));
   RxSpeedMax->Caption = str;
   if(mls.rx_min_speed!=(DWORD)-1)
   str.printf("%.2f",(float)(((float)mls.rx_min_speed)/100.0));
   else
   str = 0;
   RxSpeedMin->Caption = str;

   f_value = (float)(((float)mls.rx_quality));
   str.printf("%.2f",f_value);
   RxQuality->Caption = str;
   if(f_value < 50)
      color = clRed;
      else
   if(f_value < 90)
      color = clYellow;
      else
      color = clGreen;
    RxQuality->Font->Color = color;


   TxBytesCount->Caption   = mls.tx_bytes;
   TxKadrsCount->Caption   = mls.tx_kadrs;
   TxErrorsCount->Caption  = mls.tx_errors;
   TxBytesCount->Hint      = mls.tx_total_bytes;
   TxKadrsCount->Hint      = mls.tx_total_kadrs;
   TxErrorsCount->Hint     = mls.tx_total_errors;
   str.printf("%.2f",(float)(((float)mls.tx_middle_speed)/100.0));
   TxSpeedMiddle->Caption  = str;
   str.printf("%.2f",(float)(((float)mls.tx_max_speed)/100.0));
   TxSpeedMax->Caption = str;
   if(mls.tx_min_speed!=(DWORD)-1)
    str.printf("%.2f",(float)(((float)mls.tx_min_speed)/100.0));
    else
    str = 0;
   TxSpeedMin->Caption = str;

   f_value = (float)(((float)mls.tx_quality));
   str.printf("%.2f",f_value);
   TxQuality->Caption = str;
   if(f_value < 50)
      color = clRed;
     else 
   if(f_value < 90)
      color = clYellow;
      else
      color = clGreen;
    TxQuality->Font->Color = color;
   this->TxQueueSize->Caption = mls.tx_queue_size; 
   ModemmerRxKadrs->Caption = mls.modemmer_rx_kadrs;
   ModemmerTxKadrs->Caption = mls.modemmer_tx_kadrs;
   ModemmerRxBytes->Caption = mls.modemmer_rx_bytes;
   ModemmerTxBytes->Caption = mls.modemmer_tx_bytes;
   LineBox->Visible = true;
   }
   update_line_state(selected_addr,mls.line_state);
  }
  else
  LineBox->Visible = false;


}



void __fastcall TModemmerFrm::ModemsTreeChange(TObject *Sender,
      TTreeNode *Node)
{
 /*Смена активного элемента ModemsTree*/

 TTreeNode * parent = Node->Parent;

 modem_data md = get_modem_data(Node);
 if(md.addr.addr  != selected_addr.addr || selected_modem_handle != md.hModem)
 {
   bool ch_modem  = md.addr.modem!=selected_addr.modem ? true:false || selected_modem_handle!=md.hModem;
   bool ch_line  = (ch_modem || (md.addr.line != selected_addr.line)) ? true:false;
   selected_addr.addr = md.hModem ? md.addr.addr:-1;
   if(ch_modem)
   {
     if(parent)
     selected_modem_handle = md.hModem; 
     else
     selected_modem_handle = (GKHANDLE)-1;
     on_modem_change();
   }
   if(ch_line)
      on_line_change();
 }
 if(!parent)
  {
   on_module_state(mod_iface(),mod_iface.get_module_state());
  }
}
//---------------------------------------------------------------------------


void __fastcall TModemmerFrm::PageControl1Change(TObject *Sender)
{
  reset_notify(mod_iface(),~(MNF_COMMON|MNF_MODEM));
  Timer1->Enabled = false;
  switch(PageControl1->TabIndex)
  {
   case 0:  //Окно просмотра
          Timer1Timer(this);
          Timer1->Enabled = LineBox->Visible;
          on_modem_change();
          on_line_change();

          break;
   case 1: // Просмотр
         set_notify(mod_iface(),MNF_MODEMMER);
        break;
   case 2: // Просмотр редиректора
        setup_redirect_list();
        set_notify(mod_iface(),MNF_MODEMMER_REDIRECT);
        break;
   case 3: // Просмотр DFA;
        setup_dfa();
        break;
  }

}
//---------------------------------------------------------------------------

bool __fastcall filter_addr(LPBYTE b1,LPBYTE b2,DWORD len)
{
  bool ret = true;

    for(DWORD i = 0;i<len && ret;i++)
            {
             if((*b1 != *b2) && (*b1) != 0xFF)
              ret = false;
              b1++,b2++;
            }
  return ret;
}
bool __fastcall TModemmerFrm::check_monitor_filter(LPBYTE b2,DWORD len,DWORD fa)
{
 bool ret = true;
 switch(MonitorParam->ItemIndex)
 {
  case 0: ret = true;break;
  case 1:
         {
          if(ModemsTree->Selected)
          {
            MODEM_ADDR addr;
            addr.addr =get_modem_data(ModemsTree->Selected).addr.addr;
            LPBYTE b1 = (LPBYTE)&addr;
            len = KeRTL::MIN((DWORD)3,len);
            ret = filter_addr(b1+1,b2+1,len);
          }
          else
          ret = false;
         }break;
  case 2:
         {
          MODEM_ADDR addr;
          addr.addr = -1;
          addr.sta  = 0;
          addr.modem = filter_modem;
          addr.line  = filter_line;
          if(fa==filter_fa || filter_fa>=255)
            ret = filter_addr((LPBYTE)&addr,b2,KeRTL::MIN((DWORD)2,len));
           else
           ret = false;
         }
         break;
 }
 return ret;
}

void __fastcall TModemmerFrm::on_modemmer_txrx(LPMPROTO_HEADER mph,DWORD sz,DWORD code)
{
        //TODO: Add your source code here
        MODEM_ADDR addr;
        addr.addr = selected_addr.addr;
        bool is_rx = (code == MNCODE_MODEMMER_RX) ? true : false;
        bool is_tx = (code == MNCODE_MODEMMER_TX) ? true : false;
        if((rx_mon_enabled && is_rx)||(tx_mon_enabled && is_tx) || (redir_mon_enabled && code == MNCODE_MODEMMER_REDIRECT))
        {
        if(mph)
        {
         DWORD color;         
         switch(code)
         {
          case MNCODE_MODEMMER_TX : color = RGB(0,0,255);break;
          case MNCODE_MODEMMER_RX : color = RGB(41,141,58);break;
          case MNCODE_MODEMMER_REDIRECT : color = RGB(100,0,100);break;
          case MNCODE_FA_HANDLER  : color = RGB(255,0,0);break;
         }
         LPMODEM_ADDR     ma = code != MNCODE_MODEMMER_RX ?  &mph->addr_to:&mph->addr_from;
         if(check_monitor_filter((LPBYTE)&ma->addr,sizeof(DWORD),mph->fa))
         {
         UnicodeString s;
         s.sprintf(load_rcstring(ModuleInstance,IDS_RX_TEMPLATE).c_str(),
                   load_rcstring(ModuleInstance,IDS_RX+code).c_str(),
                   (DWORD)mph->addr_from.sta,(DWORD)mph->addr_from.modem,(DWORD)mph->addr_from.line,
                   (DWORD)mph->addr_to.sta,(DWORD)mph->addr_to.modem,(DWORD)mph->addr_to.line,(DWORD)mph->pkt_num,(DWORD)mph->fa,mph->data_size);
         monitor_string(s,color);
           if(MonitorData->Checked && code <MNCODE_MODEMMER_REDIRECT)
              monitor_data(mph->fa,(LPBYTE)mph->data,mph->data_size);
         }
        }
       }
}

#define MON_LIMIT 256

void __fastcall TModemmerFrm::monitor_string(UnicodeString & s,DWORD color)
{
 monitor->add_line(s.c_str(),color);
}

void  __fastcall TModemmerFrm::notify_handler()
{
  if(monitor)
     monitor->set_lock_draw(true);
   TGKModuleForm::notify_handler();
   monitor->set_lock_draw(false);
}

void __fastcall TModemmerFrm::on_modem_line_state(LPMODEM_LINE_STATE mls)
{
        if(mls)
        {
         /*Изменение состояния линии*/
         TTreeNode * modem_node = this->find_modem(mls->addr.modem,NULL);
         if(modem_node)
            {
             TTreeNode * line_node = find_modem_line(modem_node,mls->addr.line);
             
             if(mls->line_state&MODEM_LINE_STATE_EXIST)
             {
              /*Удаление / добавление линии */
               if(!line_node)
                  {
                   line_node = add_line_node(modem_node,mls->addr);
                   //TODO пересортировать элементы дерева
                   modem_node->CustomSort(modem_addr_sort,0,false);
                  }

             }
             else
             {
                if(line_node)
                   line_node->Delete();
                   line_node = NULL;
             }
             if(line_node && mls->state_changes&(MODEM_LINE_STATE_WORKED|MODEM_LINE_STATE_CONNECT))
             {
               set_node_image_index(line_node,(mls->line_state&(MODEM_LINE_STATE_CONNECT)) ? 14:15);
               modem_data md = get_modem_data(line_node);
               char buff[MAX_PATH<<1];
               LPMODEM_LINE_PROPS mlp = this->get_line_props(md.hModem,md.addr.line,buff,sizeof(buff));
               if(mlp && mlp->line_text_len)
                  line_node->Text = mlp->line_text;
               if(line_node->Selected)
                   on_line_change();
             }
            }
        }
}

void        __fastcall TModemmerFrm::on_modem_change_number(LPMODEM_CHANGE_NUMBER mcn)
{
 /*Обработка изменения номера модема*/
 AnsiString str;
 str.printf("Изменение номера модема %d -> %d",mcn->old_number,mcn->new_number);
 this->monitor->add_line(str.c_str());
 TTreeNode * modem_node = find_modem(mcn->old_number,mcn->modem);
 MODEM_ADDR addr;
 addr.addr = -1;
 addr.sta  =  0;
 addr.modem = mcn->new_number;
 ModemsTree->Items->BeginUpdate();
 char res_str[MAX_PATH];
 LoadString(ModuleInstance,IDS_MODEM,res_str,sizeof(res_str));
 str = res_str;

 if(modem_node)
 {
  modem_data * md = (modem_data*) modem_node->Data;
  char  buf[MAX_PATH<<1];
  LPMODEM_PROPS mp  = this->get_modem_props(md->hModem,buf,sizeof(buf));
  if(mp)
    str = mp->modem_info;
  //Такой модем уже есть
  if(modem_node->Parent == this->free_modems)
   {
    if(mcn->new_number<MAX_MODEM_COUNT)
      {
       str.cat_printf(" №% d",mcn->new_number);
       modem_node->MoveTo(worked_modems,naAddChild);
       ((modem_data*)modem_node->Data)->addr.modem = mcn->new_number;
       Tree_setup_modem_lines(modem_node,addr);
       worked_modems->CustomSort(modem_addr_sort,0,false);
      }
   }
   else
   {
   if(modem_node->Parent == this->worked_modems)
   {
     if(mcn->new_number<MAX_MODEM_COUNT)
        {
         str.cat_printf(" №% d",mcn->new_number);
         addr.modem = mcn->new_number;
         ((modem_data*)modem_node->Data)->addr.addr = addr.addr;
         if(modem_node->Selected)
           {
            selected_addr.addr = addr.addr;
            ModemNumber->Position = addr.modem;
           }
         for(int i = 0;i<modem_node->Count;i++)
         {
          TTreeNode * line_node = modem_node->Item[i];
            ((modem_data*)line_node->Data)->addr.modem = mcn->new_number;
          if(line_node->Selected)
             {
              selected_addr.addr = addr.addr;
              ModemNumber->Position = addr.modem;
             }

         }
         worked_modems->CustomSort(modem_addr_sort,0,false);
        }
      else
      {
       modem_node->DeleteChildren();
       modem_node->MoveTo(free_modems,naAddChild);
      if(modem_node->Selected)
      {
      selected_addr.addr = addr.addr;
      ModemNumber->Position = addr.modem;
      }
       ((modem_data*)modem_node->Data)->addr.modem = mcn->new_number;
      }
    }
   }
   modem_node->Text = str;
   if(modem_node->Selected)
      ModemsTreeChange(this,modem_node);


 }
 //else
  //add_modem_node(mcn->modem,addr);
 ModemsTree->Items->EndUpdate();

}


void __fastcall TModemmerFrm::Timer1Timer(TObject *Sender)
{
 update_line_stats();       
}
//---------------------------------------------------------------------------

#pragma warn -8004
void __fastcall TModemmerFrm::SetModemNumberBtnClick(TObject *Sender)
{
 /*Установка номера модема*/
  DWORD   old_num , new_num = ModemNumber->Position;
  TGKModuleInterface mi(selected_modem_handle,true);
  bool running = mi.is_running();
  //if(running)
  //   mi.stop(MODULE_STOP_RECONFIGURE);
  //Application->ProcessMessages();
  LRESULT ret = mi.call(MODEMCM_SET_NUMBER,new_num,(LPARAM)&old_num);
  if(ret == GKH_RET_ERROR)
     end_module_command(selected_modem_handle,ret);
   else
     ;//if(running)
      //mi.start(MODULE_START_RECONFIGURE,0);

  SetMNBtn->Enabled = false;

}
#pragma warn .8004
//---------------------------------------------------------------------------

void __fastcall TModemmerFrm::StartBtnClick(TObject *Sender)
{

  TGKModuleInterface mi(selected_modem_handle == GKHANDLE(-1) ? mod_iface():selected_modem_handle,true);
  DWORD ret = mi.start(MODULE_START_REQUEST,0);
  end_module_command(mi(),ret);      
}
//---------------------------------------------------------------------------

void __fastcall TModemmerFrm::StopBtnClick(TObject *Sender)
{
  TGKModuleInterface mi(selected_modem_handle == GKHANDLE(-1) ? mod_iface():selected_modem_handle,true);
  DWORD ret = mi.stop(MODULE_STOP_REQUEST);
  end_module_command(mi(),ret);

}
//---------------------------------------------------------------------------

void __fastcall TModemmerFrm::ModemNumberChanging(TObject *Sender,
      bool &AllowChange)
{
 SetMNBtn->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TModemmerFrm::Edit1Change(TObject *Sender)
{
 ModemNumber->Enabled       = true;
 SetMNBtn->Enabled = true;
 if(Edit1->Text.Length())
 {
 short number = atoi(Edit1->Text.c_str());
 if(number< ModemNumber->Min || number > ModemNumber->Max)
    Edit1->Text = ModemNumber->Max;
 }   
}
//---------------------------------------------------------------------------

HWND __fastcall TModemmerFrm::get_owner_window()
{
  HWND ret = owner_window;
  if(!ret)
   {
     TModemmerFrm * frm = dynamic_cast<TModemmerFrm *>(Owner);
     ret = frm->get_owner_window();
   }
   return ret;
}



void __fastcall TModemmerFrm::PropBtnClick(TObject *Sender)
{
    PostMessage(get_owner_window(),WGKM_SHOW_PROPWND,selected_addr.addr,(LPARAM)selected_modem_handle);
}
//---------------------------------------------------------------------------

void __fastcall TModemmerFrm::ViewBtnClick(TObject *Sender)
{
  PostMessage(get_owner_window(),WGKM_SHOW_VIEWWND,selected_addr.addr,(LPARAM)selected_modem_handle);
}
//---------------------------------------------------------------------------


void __fastcall TModemmerFrm::RefreshBtnClick(TObject *Sender)
{
 call_gkhandle(selected_modem_handle,MODEMCM_REFRESH_LINE,LPARAM(selected_addr.line),0);       
}
//---------------------------------------------------------------------------

void __fastcall TModemmerFrm::AddModemBtnClick(TObject *Sender)
{
 /*Добавление модема*/
 if(OpenDialog1->Execute())
 {
  WideString ws = OpenDialog1->FileName;
  GKHANDLE new_modem = (GKHANDLE)mod_iface.call(MCMD_APPEND_MODULE,LPARAM(ws.c_bstr()),TRUE);
  if(IsValidGKHandle(new_modem))
  {
   Application->ProcessMessages();/*Убедиться , что все напоминания сделаны*/
   TTreeNode * modem_node = find_modem(-1,new_modem);
   if(modem_node)
   {
     modem_node->Parent->Expand(false);
     modem_node->Selected = true;
     PropBtnClick(this);
   }  
  }
  else
   end_module_command(mod_iface(),-1);
 }
}
//---------------------------------------------------------------------------


void __fastcall TModemmerFrm::RemoveModemBtnClick(TObject *Sender)
{
  LRESULT ret = GKH_RET_SUCCESS;
  TGKModuleInterface mi(selected_modem_handle,true);
  wchar_t module_name[MAX_PATH];
  mi.get_module_name(module_name,sizeof(module_name)/sizeof(wchar_t));
  WideString query = L"Отключть модем ";
  query+=module_name;
  query+= " ?";
  if(MessageBoxW(Handle,query.c_bstr(),L"Отключение модема",MB_YESNO|MB_ICONQUESTION)==ID_YES)
  {
  if(mi.is_running())
      ret = mi.stop(MODULE_STOP_REQUEST);
  if(ret == GKH_RET_SUCCESS)
  {
   ret = mod_iface.call(MCMD_REMOVE_MODULE,LPARAM(selected_modem_handle),0);
  }
  }
  else
  ret = GKH_RET_CANCEL; 
  end_module_command(mod_iface(),ret);
}
//---------------------------------------------------------------------------

void __fastcall TModemmerFrm::FormDestroy(TObject *Sender)
{
  worked_modems->Delete();
  free_modems->Delete();
  if(monitor)
     delete monitor;
     monitor = NULL;   
}
//---------------------------------------------------------------------------

void __fastcall TModemmerFrm::FormResize(TObject *Sender)
{
  if(monitor)
     monitor->SetSize(MonitorBox->Width-8,MonitorBox->Height-12);
}
//---------------------------------------------------------------------------


void __fastcall TModemmerFrm::MonitorParamClick(TObject *Sender)
{
  filter_enabled =  MonitorParam->ItemIndex == 2 ? true:false;

  /*
  if(FilterBox->Visible != filter_enabled)
  {
  if(filter_enabled)
    {
      MonitorBox->Height-= FilterBox->Height;
      MonitorBox->Top    = FilterBox->Height+FilterBox->Top;
    }
    else
    {
      MonitorBox->Top    = FilterBox->Top;
      MonitorBox->Height+= FilterBox->Height;

    }
     OnResize(this);
   }
  */
  FilterBox->Visible = filter_enabled;
  if(filter_enabled)
  {
   filter_modem =  atoi(FilterModem->Text.c_str());
   filter_line  =  atoi(FilterLine ->Text.c_str());
   filter_fa    =  atoi(FilterFA   ->Text.c_str());
  }
}
//---------------------------------------------------------------------------

void __fastcall TModemmerFrm::FilterModemChange(TObject *Sender)
{
   filter_modem =  atoi(FilterModem->Text.c_str());
   filter_line  =  atoi(FilterLine ->Text.c_str());
   filter_fa    =  atoi(FilterFA   ->Text.c_str());
}
//---------------------------------------------------------------------------

void __fastcall TModemmerFrm::NewWindowBtnClick(TObject *Sender)
{
  TModemmerFrm * mfrm =
    new TModemmerFrm(this,NULL,mod_iface());
  mfrm->PageControl1->ActivePageIndex = PageControl1->ActivePageIndex;
  mfrm->Show();
}
//---------------------------------------------------------------------------

void        __fastcall TModemmerFrm::monitor_data(DWORD fa, LPBYTE data,DWORD data_size)
{
 switch(fa)
 {
  case FA_OTD                     :  monitor_otd(monitor,data,data_size);
       break;
  case FA_OTD_ALARM_ARCHIVE       :  monitor_otd_archieve(monitor,data,data_size);
       break;
  case FA_OTD_MEDIUM              : monitor_otd_medium(monitor,data,data_size);
      break;
  case FA_OTD_MEDIUM_ALARM_ARCHIVE: monitor_otd_medium_alarm_archive(monitor,data,data_size);
     break;

 }
}




void __fastcall TModemmerFrm::MiCloseClick(TObject *Sender)
{
  Close();      
}
//---------------------------------------------------------------------------

void __fastcall TModemmerFrm::MiAboutClick(TObject *Sender)
{
 PageControl1->ActivePage = pgAbout ;
}
//---------------------------------------------------------------------------

void __fastcall TModemmerFrm::RxMonEnableClick(TObject *Sender)
{
  rx_mon_enabled = (!PausedMon->Checked && RxMonEnable->Checked) ? true:false;
}
//---------------------------------------------------------------------------

void __fastcall TModemmerFrm::TxMonEnableClick(TObject *Sender)
{
  tx_mon_enabled = (!PausedMon->Checked && TxMonEnable->Checked) ? true:false;
}
//---------------------------------------------------------------------------

void __fastcall TModemmerFrm::ClearMonitorClick(TObject *Sender)
{
  monitor->clear();
}
//---------------------------------------------------------------------------

void __fastcall TModemmerFrm::PausedMonClick(TObject *Sender)
{
  TxMonEnableClick(this);
  RxMonEnableClick(this);
  RedirMonEnableClick(this);
}
//---------------------------------------------------------------------------


void __fastcall TModemmerFrm::AddRedirectItem(DWORD idx,LPREDIRECT_ENTRY re)
{
     AnsiString str;
     TListItem * item = idx<(DWORD)RedirectList->Items->Count ?  RedirectList->Items->Insert(idx) : RedirectList->Items->Add();
     ;
     item->Data    = (LPVOID)re->options;
     char * templ1 =  "%03d.%03d.%03d";
     str.printf(templ1,(DWORD)re->from_addr.sta,(DWORD)re->from_addr.modem,(DWORD)re->from_addr.line);
     item->Caption =str;
     str.printf("%03d",(DWORD)re->fa);
     item->SubItems->Add(str);
     str.printf(templ1,(DWORD)re->to_addr.sta,(DWORD)re->to_addr.modem,(DWORD)re->to_addr.line);
     item->SubItems->Add(str);

}

void __fastcall TModemmerFrm::setup_redirect_list()
{
  if(!RedirectSheet->Tag)
  {
   BOOL enable = FALSE;
   mod_iface.call(MDMCM_ISREDIRECT_ENABLED,(LPARAM)&enable,0);
   RedirectEnable->Checked = enable;
   RedirectList->Items->Clear();
   RedirFrom->Text.Delete(1,RedirFrom->Text.Length());
   RedirFA->Text.Delete(1,RedirFA->Text.Length());
   RedirTo->Text.Delete(1,RedirTo->Text.Length());
   RedirDisabled->Checked = false;
   RedirOnly->Checked = false;
   DWORD index = 0;
   REDIRECT_ENTRY re;
   re.dw_size   = sizeof(re);
   while(mod_iface.call(MDMCM_ENUM_REDIRENTRY,index,(LPARAM)&re)==GKH_RET_SUCCESS)
    {
     AddRedirectItem(index++,&re);
    }
   if(RedirectList->Items->Count)
      RedirectList->Items->Item[0]->Selected = true;
  }
}

//---------------------------------------------------------------------------


void __fastcall TModemmerFrm::RedirectListSelectItem(TObject *Sender,
      TListItem *Item, bool Selected)
{
  if(Selected)
  {
   RedirFrom->Text    = Item->Caption;
   RedirFA->Text      = Item->SubItems->Strings[0];
   RedirTo->Text      = Item->SubItems->Strings[1];
   RedirOnly->Checked =  (((DWORD)Item->Data)&MDM_REDIR_REDIRONLY) ? true:false;
   RedirDisabled->Checked = (((DWORD)Item->Data)&MDM_REDIR_DISABLED) ? true:false;
   RedirDeleteBtn->Enabled = true;
  }
  else
  RedirDeleteBtn->Enabled = false;
  RedirNewBtn->Enabled  = false;
  RedirEditBtn->Enabled = false;

}
//---------------------------------------------------------------------------

void __fastcall TModemmerFrm::RedirDeleteBtnClick(TObject *Sender)
{
  TListItem * item = RedirectList->Selected;
  if(item)
  {
   mod_iface.call(MDMCM_REDIRENTRY_REMOVE,item->Index,0);
  }
}
//---------------------------------------------------------------------------

void __fastcall TModemmerFrm::RedirectEnableClick(TObject *Sender)
{
  mod_iface.call(MDMCM_ENABLE_REDIRECT,this->RedirectEnable->Checked ? TRUE:FALSE,0);
}
//---------------------------------------------------------------------------




bool        __fastcall TModemmerFrm::make_redirect_entry(LPREDIRECT_ENTRY re)
{
 re->dw_size  =  sizeof(*re);
 if(str2addr(RedirFrom->Text.c_str(),(LPBYTE)&re->from_addr,sizeof(re->from_addr),'.') &&
    str2addr(RedirTo->Text.c_str(),(LPBYTE)&re->to_addr,sizeof(re->to_addr),'.')
   )
   {
    if(re->from_addr.modem && re->from_addr.modem<MODEM_ADDR_MAXVALUE && re->to_addr.modem && re->to_addr.modem<MODEM_ADDR_MAXVALUE)
      {
       re->fa = RedirFA ->Text.Length() ? (BYTE)atoi(RedirFA->Text.c_str()): FA_ALL;
       DWORD opt = 0;
       if(RedirOnly->Checked)
         opt|=MDM_REDIR_REDIRONLY;
       if(RedirDisabled->Checked)
         opt|=MDM_REDIR_DISABLED;  
       re->options = opt;
       return true;
      }
   }
  return false;
}

void __fastcall TModemmerFrm::RedirFromChange(TObject *Sender)
{
   REDIRECT_ENTRY re;
   bool enabled = make_redirect_entry(&re);
   RedirEditBtn->Enabled = RedirectList->Selected &&  enabled;
   RedirNewBtn->Enabled  = enabled;

}
//---------------------------------------------------------------------------

void __fastcall TModemmerFrm::RedirNewBtnClick(TObject *Sender)
{
 //Добавление новой записи
   REDIRECT_ENTRY re;
   if(make_redirect_entry(&re))
   {
    int idx =(int) mod_iface.call(MDMCM_REDIRENTRY_ADD,(LPARAM)&re,0);
    if(idx>=0)
      {
       Application->ProcessMessages();
       if(idx<RedirectList->Items->Count)
         RedirectList->Items->Item[idx]->Selected = true;
      }

   }

}
//---------------------------------------------------------------------------

void        __fastcall TModemmerFrm::on_redir_change(DWORD index,bool add)
{
 if(add)
  {
   REDIRECT_ENTRY re;
   re.dw_size   = sizeof(re);
   if(mod_iface.call(MDMCM_ENUM_REDIRENTRY,index,(LPARAM)&re)==GKH_RET_SUCCESS)
      AddRedirectItem(index,&re);

  }
  else
  {
   if((int)index<RedirectList->Items->Count)
        RedirectList->Items->Item[index]->Delete();
  }
}


void __fastcall TModemmerFrm::RedirEditBtnClick(TObject *Sender)
{
   REDIRECT_ENTRY re;
   if(make_redirect_entry(&re))
   {
    int idx = RedirectList->Selected->Index;
    if(mod_iface.call(MDMCM_REDIRENTRY_UPDATE,(LPARAM)&re,(LPARAM)&idx)==GKH_RET_SUCCESS)
      {
       Application->ProcessMessages();
       if(idx<RedirectList->Items->Count)
         RedirectList->Items->Item[idx]->Selected = true;
      }

   }
}
//---------------------------------------------------------------------------

void __fastcall TModemmerFrm::RedirOnlyClick(TObject *Sender)
{
 if(RedirDeleteBtn->Enabled)
    RedirEditBtn->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TModemmerFrm::RedirMonEnableClick(TObject *Sender)
{
  redir_mon_enabled = (!PausedMon->Checked && RedirMonEnable->Checked) ? true:false;
}
//---------------------------------------------------------------------------


void __fastcall TModemmerFrm::ModemsTreeDeletion(TObject *Sender,
      TTreeNode *Node)
{
  if(Node->Data)
    {
     modem_data * md = (modem_data *)Node->Data;
     if(Node->Parent == worked_modems || Node->Parent == free_modems)
      {
      reset_notify(md->hModem,-1);
      }
      delete md;
      Node->Data = NULL;
    } 
}
//---------------------------------------------------------------------------

LPMODEM_PROPS      __fastcall TModemmerFrm::get_modem_props(GKHANDLE hModem,LPBYTE buffer,DWORD bsz)
{
 if(buffer && bsz>=sizeof(MODEM_PROPS))
 {
  LPMODEM_PROPS mp =(LPMODEM_PROPS)buffer;
  mp->dw_size = bsz;
  mp->modem_info_size = (bsz-sizeof(*mp))/sizeof(wchar_t);
  if(call_gkhandle(hModem,MODEMCM_GET_MODEM_PROPS,(LPARAM)mp,bsz)==GKH_RET_SUCCESS)
     return mp;
 }
 return NULL;
}

LPMODEM_LINE_PROPS __fastcall TModemmerFrm::get_line_props(GKHANDLE hModem,DWORD line,LPBYTE buffer,DWORD bsz)
{
 if(buffer && bsz>=sizeof(MODEM_LINE_PROPS))
 {
  LPMODEM_LINE_PROPS mlp =(LPMODEM_LINE_PROPS)buffer;
  mlp->dw_size = bsz;
  mlp->line_text_sz = (bsz-sizeof(*mlp))/sizeof(wchar_t);
  if(call_gkhandle(hModem,MODEMCM_GET_LINE_PROPS,line,(LPARAM)mlp)==GKH_RET_SUCCESS)
     return mlp;
 }
 return NULL;

}


void __fastcall TModemmerFrm::Splitter1Moved(TObject *Sender)
{
 this->OnResize(this);       
}
//---------------------------------------------------------------------------





void __fastcall TModemmerFrm::ModemsTreeDblClick(TObject *Sender)
{
  if(PageControl1->ActivePageIndex == 1)
      ViewBtnClick(this);
      else
      PropBtnClick(this);
}
//---------------------------------------------------------------------------

void __fastcall TModemmerFrm::N1Click(TObject *Sender)
{
  TRect r;
  GetMenuItemRect(Handle,N1->Parent->Handle,N1->MenuIndex,&r);
  PopupMenu1->Popup(r.left,r.top+r.Height());      
}
//---------------------------------------------------------------------------


void __fastcall TModemmerFrm::ModemsTreeKeyPress(TObject *Sender,
      char &Key)
{
  if(Key == 0x0D)
     PropBtnClick(this);   
}
//---------------------------------------------------------------------------

void __fastcall TModemmerFrm::FormClose(TObject *Sender,
      TCloseAction &Action)
{
  if(Action == caHide && !owner_window)
     Action =  caFree;
}
//---------------------------------------------------------------------------

void __fastcall TModemmerFrm::ToolButton1Click(TObject *Sender)
{
 mod_iface.flush_config();       
}
//---------------------------------------------------------------------------

void __fastcall TModemmerFrm::MonLinesCountChange(TObject *Sender)
{
  int value = atoi(MonLinesCount->Text.c_str());
  this->monitor->set_lines_limit(KeRTL::MIN(KeRTL::MAX(1024,value),32000));
}
//---------------------------------------------------------------------------

void __fastcall TModemmerFrm::before_show(LPMODULE_WINDOW mw)
{
 PageControl1->ActivePageIndex  = mw->window_type-1;
}

void        __fastcall TModemmerFrm::setup_dfa()
{
  DfaTree->Items->Clear();
  FA_HANDLER fah;
  fah.dw_size  = sizeof(fah);


  for(int i = 0;i<MAX_FA_COUNT;i++)
  {
    fah.fa = i;
    TTreeNode * fa_node = NULL;
    DWORD idx = 0;

    while(GKH_RET_SUCCESS == mod_iface.call(MDMCM_ENUM_HANDLER,idx,(LPARAM)&fah))
    {
      if(!fa_node)
        {
          UnicodeString text;
          text.printf(L"FA %s",fah.fa_text);
          fa_node = DfaTree->Items->Add(NULL,text);
        }
     TGKModuleInterface handler(fah.module,true);
     wchar_t mod_name[MAX_PATH];
     handler.get_module_name(mod_name,KERTL_ARRAY_COUNT(mod_name));
     TTreeNode * item = fa_node->Owner->AddChild(fa_node,mod_name);
     item->Data   =(void*) idx;
     idx++;
    }
  }
}


