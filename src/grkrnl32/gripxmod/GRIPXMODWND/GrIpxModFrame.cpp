//---------------------------------------------------------------------------

#pragma hdrstop
#include <vcl.h>
#include "GrIpxModFrame.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "gkmodule_form"
#pragma link "about_common_frame"
#pragma resource "*.dfm"
TGrIpxModuleWnd *GrIpxModuleWnd;
//---------------------------------------------------------------------------
__fastcall TGrIpxModuleWnd::TGrIpxModuleWnd(TComponent* Owner,HWND p_wnd,GKHANDLE handle)
        : TGKModuleForm(Owner,p_wnd,handle)
{
  RootNode = ClientsTree->Items->Add(NULL,"Список клиентов");
  RootNode->Data = (LPVOID)-1;
  selected_line_number = selected_client_id = -1;

  settings_up = FALSE;
  settings_change_mask = 0;
  FillMemory(&current_settings,sizeof(current_settings),-1);
  current_settings.server_name[0] = current_settings.client_name[0] = 0;

  monitor_switch = RadioGroup1->ItemIndex;
  monitor = new KeWin::TTextViewer<char>;
  KeWin::TRect r(0,0,MonitorBox->Width,MonitorBox->Height);
  r.Grow(-4,-10);
  r.Move(4,8);
  monitor->DoCreate(MonitorBox->Handle,r,-1,1024,L"GrNetMonitor");
  HFONT font = CreateFont(-12,0,0,0,FW_NORMAL,0,0,0,
               RUSSIAN_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH,
               _T("Courier New"));
  font = monitor->SetFont(font,false);
  DeleteObject(font);
 #ifdef COMERCIAL_BUILD
 Label30->Caption = "ООО \"Компания промышленной автоматизации\"";
 #endif

}
//---------------------------------------------------------------------------

void __fastcall TGrIpxModuleWnd::after_set_gkhandle()
{
 TGKModuleForm::after_set_gkhandle();
 setup_settings();
 setup_tu_clients();
 set_form_caption();
 this->PageControl1Change(this);
}

void     __fastcall TGrIpxModuleWnd::set_form_caption()
{
  AnsiString str  =  "Настройка ";
  AnsiString str1 =  "модуля GRNET IPX ";
  if(RadioButton1->Checked)
  {
   str1.printf("GRNET IPX сервера %s",ServerName->Text.c_str());
  }
  if(RadioButton2->Checked)
  {
   str1.printf("GRNET IPX клиента %s [ сервер %s ]",ClientName->Text.c_str(),ServerName->Text.c_str());
  }
  Caption = str+str1;
}


void     __fastcall TGrIpxModuleWnd::on_module_state(GKHANDLE mod,DWORD state)
{
  bool running = (state&MODULE_STATE_RUNNING) ? true:false;
  StatusBar1->Panels->Items[0]->Text =  running ? "Модем работает":"Модем остановлен";
  StartBtn->Enabled = !running;
  StopBtn->Enabled  =  running;
}

void     __fastcall TGrIpxModuleWnd::on_module_config_change(GKHANDLE mod,LPMODULE_CONFIG_DATA mcd)
{
  set_settings((LPGRNET_SETTINGS)mcd->data);
}




void __fastcall TGrIpxModuleWnd::setup_settings()
{
 settings_up = TRUE;
 char buffer[sizeof(MODULE_CONFIG_DATA)+sizeof(GRNET_SETTINGS)];
 LPMODULE_CONFIG_DATA mcd = (LPMODULE_CONFIG_DATA)buffer;
 mcd->dw_sz = sizeof(MODULE_CONFIG_DATA)+sizeof(GRNET_SETTINGS)-sizeof(mcd->data);
 mcd->mask  = -1;
 if(mod_iface.call(MCMD_GET_MODULECONFIG_DATA,(LPARAM)mcd,sizeof(GRNET_SETTINGS))==GKH_RET_SUCCESS)
 {
   set_settings((LPGRNET_SETTINGS)mcd->data);
 }

}

void     __fastcall TGrIpxModuleWnd::setup_tu_clients()
{
 GRNET_TU_CLIENT gtc;
 gtc.dw_size  = sizeof(gtc);
 TuClientsList->Clear();
 DWORD idx = 0;
 while(mod_iface.call(GRNET_GET_TUCLIENT,(LPARAM)&gtc,idx)==GKH_RET_SUCCESS)
 {
   TListItem * item = TuClientsList->Items->Add();
   item->Caption = idx;
   item->SubItems->Add(gtc.name);
   if(*gtc.name)
     item->SubItems->Add( gtc.sock_num);
     else
     item->SubItems->Add("");
   idx++;
 }
 TuClientsList->Items->Item[0]->Selected = true;

}


void __fastcall TGrIpxModuleWnd::set_settings(LPGRNET_SETTINGS gs)
{
   settings_up = TRUE;
   memcpy(&current_settings,gs,sizeof(current_settings));
   settings_change_mask = 0;
   dummy->Checked = true;
   Server->Visible = false; Client->Visible = false;
     {
         PageControl1->Pages[PageControl1->TabIndex]->Tag = TRUE;
         ModemNumberEdit->Text  = KeRTL::MIN((DWORD)255,gs->modem_number);
         ModemNumberEdit->Font->Color = clBlack;
         ServerName->Text       = gs->server_name;
         ServerName->Font->Color = clBlack;
         ServerSocket->Text     = gs->server_socket;
         ServerSocket->Font->Color = clBlack;
         ClientName->Text       = gs->client_name;
         ClientName->Font->Color = clBlack;
         ClientSocket->Text     = gs->client_socket;
         ClientSocket->Font->Color = clBlack;
         PUNumber->Position     = KeRTL::MIN((DWORD)255,gs->pu_number);
         PUNumberEdit->Font->Color = clBlack;
         RetryCount->Position   = gs->tx_retry_count;
         RetryCountEdit->Font->Color = clBlack;
         RetryTimeoutEdit->Text = gs->tx_retry_delay;
         RetryTimeoutEdit->Font->Color = clBlack;
         PuName->Text  = gs->pu_name;
         PuName->Font->Color = clBlack;
      if(gs->work_mode == GRNET_WM_AS_SERVER)
         {RadioButton1->Checked = true;Server->Visible = true;Client->Visible = false;}
      if(gs->work_mode == GRNET_WM_AS_CLIENT)
        {
         RadioButton2->Checked = true;Server->Visible = true;Client->Visible = true;
        }
        PageControl1->Pages[0]->Tag = 1;
     }

   Application->ProcessMessages();
   ApplyBtn->Enabled = false;
   UndoBtn->Enabled  = false;
   settings_up = FALSE;
}


void __fastcall TGrIpxModuleWnd::apply_settings()
{
    if(!dummy->Checked)
    {
       char buffer[sizeof(MODULE_CONFIG_DATA)+sizeof(GRNET_SETTINGS)];
       ZeroMemory(buffer,sizeof(buffer));
       LPMODULE_CONFIG_DATA mcd = (LPMODULE_CONFIG_DATA)buffer;
       mcd->dw_sz = sizeof(MODULE_CONFIG_DATA)+sizeof(GRNET_SETTINGS)-sizeof(mcd->data);
       mcd->mask  = -1;
       GRNET_SETTINGS & gs = *(LPGRNET_SETTINGS)mcd->data;



      KeRTL::Unicode2Ansi(gs.server_name,sizeof(gs.server_name),ServerName->Text.c_str());
      //strncpy(gs.server_name  ,ServerName->Text.c_str(),sizeof(gs.server_name)-1);
      gs.server_socket =_wtoi(ServerSocket->Text.c_str());
      KeRTL::Unicode2Ansi(gs.client_name,sizeof(gs.server_name),ClientName->Text.c_str());
      //strncpy(gs.client_name,ClientName->Text.c_str(),sizeof(gs.client_name)-1);
      gs.client_socket = atoi(ClientSocket->Text.c_str());
      gs.modem_number  = atoi(ModemNumberEdit->Text.c_str());
      gs.pu_number     =PUNumber->Position     ;
      gs.tx_retry_count=KeRTL::MAX(RetryCount->Position,(int)1);
      gs.tx_retry_delay=KeRTL::MAX(atoi(RetryTimeoutEdit->Text.c_str()),100);
      WideString w_str = PuName->Text;
      safe_strcpyn(gs.pu_name,w_str.c_bstr(),sizeof(gs.pu_name)/sizeof(wchar_t));
      gs.work_mode = GRNET_WM_UNDEFINED;
      if(RadioButton1->Checked)
        gs.work_mode = GRNET_WM_AS_SERVER;
      if(RadioButton2->Checked)
        gs.work_mode = GRNET_WM_AS_CLIENT;
      begin_command("Применение изменений");
      DWORD cmd_ret =mod_iface.call(MCMD_SET_MODULECONFIG_DATA,LPARAM(mcd),sizeof(GRNET_SETTINGS));
      end_command(cmd_ret);
      if(cmd_ret == GKH_RET_SUCCESS)
      {
          ApplyBtn->Enabled = false;
          UndoBtn->Enabled  = false;
      }
    }
}




void __fastcall TGrIpxModuleWnd::UndoBtnClick(TObject *Sender)
{
  setup_settings();
}
//---------------------------------------------------------------------------

void __fastcall TGrIpxModuleWnd::ApplyBtnClick(TObject *Sender)
{
  apply_settings();
}
//---------------------------------------------------------------------------




void __fastcall TGrIpxModuleWnd::PageControl1Change(TObject *Sender)
{
  reset_notify(mod_iface(),~(MNF_COMMON|MNF_MODEM));
  AnsiString str;
  StatusBar1->Panels->Items[1]->Text = str;
  StatusBar1->Panels->Items[2]->Text = str;
  TuEnable->Visible = (PageControl1->TabIndex == GRNET_MONITOR_PAGE ) ? true:false;

  switch(PageControl1->TabIndex)
  {
  case GRNET_PARAM_PAGE://Параметры
       if(PageControl1->Pages[0]->Tag == 0)
          setup_settings();
          break;

  case GRNET_MONITOR_PAGE://Монитор
        scan_lines();
      if(ClientsTree->Selected)
          ClientsTreeChange(this,ClientsTree->Selected);
        RxEnableMonitorClick(this);
        TxEnableMonitorClick(this);
        set_notify(mod_iface(),GRNET_NOTIFY_TXDATA|GRNET_NOTIFY_RXDATA|GRNET_NOTIFY_CLIENT_STATE);
      break;

   case GRNET_DIAG_PAGE:
      scan_diag();
      set_notify(mod_iface(),GRNET_NOTIFY_READY);
   break;
  }
}
//---------------------------------------------------------------------------

void __fastcall TGrIpxModuleWnd::FormShow(TObject *Sender)
{
  on_module_state(mod_iface(),mod_iface.is_running() ? MODULE_STATE_RUNNING:0);
  DWORD notify_mask = MNF_COMMON|MNF_MODEM;
  if(PageControl1->TabIndex == GRNET_MONITOR_PAGE)
     notify_mask|=(GRNET_NOTIFY_TXDATA|GRNET_NOTIFY_RXDATA|GRNET_NOTIFY_CLIENT_STATE);
  if(PageControl1->TabIndex == GRNET_DIAG_PAGE)
     notify_mask|= GRNET_NOTIFY_READY;
     set_notify(mod_iface(),notify_mask);
}
//---------------------------------------------------------------------------

void __fastcall TGrIpxModuleWnd::FormHide(TObject *Sender)
{
  reset_notify(mod_iface(),-1);
}
//---------------------------------------------------------------------------

void __fastcall TGrIpxModuleWnd::OnMsgGrnetNotify(TMessage & msg)
{
        //TODO: Add your source code here
        LPNOTIFY_CODE nc = (LPNOTIFY_CODE)msg.LParam;
        OnGrnetNotify(nc->from_module,nc,msg.WParam);
        delete [](char*)nc;
}


LRESULT  __fastcall TGrIpxModuleWnd::on_gkhandle_notify (GKHANDLE from,LPNOTIFY_CODE nc,DWORD mask)
{
 /*Обработка кодов напоминания*/

 if(mask == MNF_MODEM)
 {
  if(nc->notify_code == MNCODE_LINE_STATE)
     {
      on_line_state((LPMODEM_LINE_STATE)nc->notify_data);
     }
  if(nc->notify_code == MNCODE_MODEM_NUMBER)
    {
     current_settings.modem_number = ((LPMODEM_CHANGE_NUMBER)nc->notify_data)->new_number;
     ModemNumberEdit->Text   = KeRTL::MIN((DWORD)255,current_settings.modem_number);
    }
  if(nc->notify_code == GRNET_NCODE_TUCLIENTS)
  {
   update_tu_client(nc->notify_data[0]);
  }
  return GKH_RET_SUCCESS;
 }

 if(mask == GRNET_NOTIFY_CLIENT_STATE)
 {
  on_client_state((LPMODEM_LINE_STATE)nc->notify_data);
  return GKH_RET_SUCCESS;
 }


 if((mask == GRNET_NOTIFY_RXDATA && RxEnableMonitor->Checked) ||
    (mask == GRNET_NOTIFY_TXDATA && TxEnableMonitor->Checked)   )
 {
   OnGrnetNotify(from,nc,mask);
   return GKH_RET_SUCCESS;
 }

 if(mask==GRNET_NOTIFY_READY)
   {
     lp_grdiag_item item = (lp_grdiag_item)nc->notify_data;
     DWORD cp    = LOWORD(nc->notify_code);
     DWORD count = HIWORD(nc->notify_code);
     for(DWORD i = 0;i<count;i++,cp++,item++)
        handle_diag(cp,item->state,item->mask);
     return GKH_RET_SUCCESS;
   }

 return TGKModuleForm::on_gkhandle_notify(from,nc,mask);
}

TTreeNode * __fastcall TGrIpxModuleWnd::find_line_node(DWORD num)
{
 TTreeNode * root = RootNode;

 TTreeNode * nline = NULL ;
 if(num<MODEM_ADDR_MAXVALUE)
 {
 if(root && root->Count)
 {
  nline = root->getFirstChild();
  do{
     if((DWORD)LOWORD((DWORD)nline->Data) == num)
        break;
     nline = nline->getNextSibling();
    }while(nline);
 }
 }
 else
 nline = root;
 return nline;
}

char * client_text = "Клиент ID %04X %s ";

void __fastcall TGrIpxModuleWnd::scan_lines()
{

  DWORD index = 0;
  MODEM_LINE_PROPS mlp;
  ZeroMemory(&mlp,sizeof(mlp));
  mlp.dw_size = sizeof(mlp);
  AnsiString s;
  RootNode->DeleteChildren();
  while(this->mod_iface.call(MODEMCM_ENUM_LINES,index,(LPARAM)&mlp) == GKH_RET_SUCCESS)
  {
    if(!this->find_line_node((DWORD)mlp.addr.line))
       add_line(mlp.addr.line);
    index++;   
  }
  RootNode->Expand(false);
  RootNode->Selected = true;
  PageControl1->Pages[1]->Tag =1;
}

TTreeNode * __fastcall TGrIpxModuleWnd::add_line(DWORD ln)
{
   AnsiString s;
   GRNET_CLIENT_INFO cli;
   cli.dw_size = sizeof(cli);
   if(mod_iface.call(GRNETCM_GET_CLIENT_INFO,ln,(LPARAM)&cli) == GKH_RET_SUCCESS)
   {
   s.printf(client_text,cli.client_id,cli.client_name);
   TTreeNode * line = ClientsTree->Items->AddChild(RootNode,s);
   line->Data = (LPVOID)MAKELONG((WORD)ln,cli.client_id);
   return line;
   }
   return NULL;

}

int __stdcall lines_sort(LPARAM p1,LPARAM p2,LPARAM param)
{
 TTreeNode * n1 = (TTreeNode *)p1;
 TTreeNode * n2 = (TTreeNode *)p2;
 int ln1 =  LOWORD((DWORD)n1->Data),ln2=LOWORD((DWORD)n2->Data);
 return ln1-ln2;
}

 WORD __fastcall _htons(WORD src)
 {
  return ((src>>8)&0xFF) | ((src<<8)&0xFF00);
 }

void     __fastcall TGrIpxModuleWnd::on_line_state  (LPMODEM_LINE_STATE mls)
{
 TTreeNode * nline = find_line_node(mls->addr.line);
 if(!nline && mls->line_state&MODEM_LINE_STATE_EXIST)
 {
   nline = add_line(mls->addr.line);
   RootNode->CustomSort(lines_sort,0,false);
 }


 if(nline && nline!= RootNode)
 {
  if(!(mls->line_state&MODEM_LINE_STATE_EXIST))
    nline->Delete();
  else
  {
  TuEnable->Enabled = (mls->line_state&MODEM_LINE_STATE_CONNECT) ? RadioButton1->Checked:false;
  if(mls->state_changes&(MODEM_LINE_STATE_CONNECT|MODEM_LINE_STATE_WORKED))
  {
  GRNET_CLIENT_INFO cli;
  cli.dw_size = sizeof(cli);
  mod_iface.call(GRNETCM_GET_CLIENT_INFO,(LPARAM)mls->addr.line,(LPARAM)&cli);
  AnsiString s;
  s.printf(client_text,cli.client_id,cli.client_name);
  nline->Text = s;
  nline->Data = (LPVOID)MAKELONG((WORD)mls->addr.line,cli.client_id);
  if(nline == this->ClientsTree->Selected)
      selected_client_id   = cli.client_id;
  char net_num [128];
  bytes2hexstr(cli.host_addr.sa_netnum,sizeof(cli.host_addr.sa_netnum),net_num,sizeof(net_num),0);
  char net_node[128];
  int len = bytes2hexstr(cli.host_addr.sa_nodenum,sizeof(cli.host_addr.sa_nodenum),net_node,sizeof(net_node),'-');
  net_node[len]=0;
  s  = net_num;
  s+=':';
  s+=net_node;
  s.cat_printf("гнездо %d",_htons(cli.host_addr.sa_socket));
  StatusBar1->Panels->Items[2]->Text = s;
  mls->line_state = cli.client_state;
  mls->state_changes = -1;
  on_client_state(mls);
  bool tu_enabled  = is_tu_enabled(cli.client_name,_htons(cli.host_addr.sa_socket));
  TuEnable->Checked = tu_enabled && TuEnable->Enabled;
  }
  }
 }
}

char *cl_states_text[] =
{
 "Поиск сервера",
 "Отключён",
 "Отключение..."
};
void     __fastcall TGrIpxModuleWnd::on_client_state(LPMODEM_LINE_STATE mls)
{

 if(selected_line_number == (DWORD)mls->addr.line)
 {
  AnsiString str;
  if(mls->line_state&CLIENT_STATE_FIND_SERVER)
     str+=cl_states_text[0];
     else
     {
      if(mls->line_state&CLIENT_STATE_DISCONNECTED)
         str+=cl_states_text[1];
         else
      if(mls->line_state&CLIENT_STATE_DISCONNECTING)
         str+=cl_states_text[2];
     }
  StatusBar1->Panels->Items[1]->Text = str;
 }
}

void __fastcall TGrIpxModuleWnd::Flash(DWORD st)
{
   PaintBox1->Canvas->Brush->Color = st ? clLime:clBtnFace;
   PaintBox1->Canvas->Ellipse(0,0,PaintBox1->Height,PaintBox1->Width);

}

void __fastcall TGrIpxModuleWnd::OnGrnetNotify(GKHANDLE from,LPNOTIFY_CODE nc,DWORD mask)
{
   LPGRNET_KADR grnet_kadr = (LPGRNET_KADR)nc->notify_data;
   Flash(grnet_kadr->kadr_num&1);
   if(!PausedMonitor->Checked)
   {
     DWORD len =  nc->dw_size-sizeof(*nc)+sizeof(nc->notify_data);

     if(monitor_switch)
     {
      monitor_grnet_kadr (grnet_kadr,len,mask == GRNET_NOTIFY_TXDATA ? TRUE:FALSE);
     }
   }
}

bool     __fastcall TGrIpxModuleWnd::check_monitor_filtr(LPGRNET_KADR grnet_kadr,DWORD len)
{
 bool ret = false;
 switch(monitor_switch)
 {
  case 1: ret = true;break;
  case 2: if((WORD)selected_client_id == (WORD)-1 || (WORD)selected_client_id == (WORD)grnet_kadr->id) ret = true;break;
  case 3: {
           lp_gratony_kadr atony_kadr = (lp_gratony_kadr) grnet_kadr->data;
           if(   (filtrCp  == 255  || filtrCp  == (DWORD)atony_kadr->numCp   )
              && (filtrGrp == 255  || filtrGrp == (DWORD)atony_kadr->numGrp  )
              && (filtrFa  == 255  || filtrFa  == (DWORD)atony_kadr->InfoType)
              )
              ret = true;
          }break;
 }
 return ret;
}

void     __fastcall TGrIpxModuleWnd::monitor_grnet_kadr (LPGRNET_KADR kadr,DWORD len, BOOL tx)
{
   if(check_monitor_filtr(kadr,len) && kadr->command!= CmCheckConnect && !kadr->receipt)
   {
    DWORD color = tx ? RGB(40,40,130):RGB(40,130,40);
    SYSTEMTIME st;
    GetLocalTime(&st);
    UnicodeString str;
    str.printf(L"%02d:%02d:%02d:%03d %s",st.wHour,st.wMinute,st.wSecond,st.wMilliseconds,    tx ? L"Передача : ":L"Прием    : ");
    monitor_string(str.c_str(),0,color);
    str.printf(L"кадр №%05d ID-%03X net_cmd=%05d попытка %02d ",kadr->kadr_num,kadr->id,kadr->command,(DWORD)kadr->retry_num);
    monitor_string(str.c_str(),0,color);
    lpobject_tu otu = (lpobject_tu) (kadr->data);
    switch(kadr->command)
    {
     case CmConnect      : monitor_string(L"Подключение...",NULL,RGB(78,120,146));
          break;
     case  CmDisconnect  : monitor_string(L"Отключение",NULL,RGB(255,0,0));
          break;
     case  CmBeginTU     :
                           str.printf(L"%s ТУ/ТР %03d.%02d.%02d - %02d ",L"Начало",otu->kp,otu->tlm_type,otu->sb,otu->obj);
                           monitor_string(str.c_str(),NULL,color);
          break;
     case  CmResetTU     :
                           str.printf(L"%s ТУ/ТР %03d.%02d.%02d - %02d ",L"Отмена выбора",otu->kp,otu->tlm_type,otu->sb,otu->obj);
                           monitor_string(str.c_str(),NULL,color);
          break;
     case  CmQueryTU     : str.printf(L"%s ТУ/ТР %03d.%02d.%02d - %02d ",L"Выбор",otu->kp,otu->tlm_type,otu->sb,otu->obj);
                           monitor_string(str.c_str(),NULL,color);
          break;
     case  CmTuFalse     :
                           str.printf(L"%s ТУ/ТР %03d.%02d.%02d - %02d ",L"Неуспешное",otu->kp,otu->tlm_type,otu->sb,otu->obj);
                           monitor_string(str.c_str(),NULL,RGB(255,0,0));
          break;
     case  CmTuOn        :
                           str.printf(L"%s  %03d.%02d.%02d - %02d ",L"Команда ВКЛЮЧИТЬ/БОЛЬШЕ",otu->kp,otu->tlm_type,otu->sb,otu->obj);
                           monitor_string(str.c_str(),NULL,color);
          break;
     case  CmTuOff       :
                           str.printf(L"%s  %03d.%02d.%02d - %02d ",L"Команда ОТКЛЮЧИТЬ/МЕНЬШЕ",otu->kp,otu->tlm_type,otu->sb,otu->obj);
                           monitor_string(str.c_str(),NULL,color);
          break;
     case  CmTrNorm      : monitor_string(L"");
          break;
     case  CmTuCancel    :
                           str.printf(L"%s  %03d.%02d.%02d - %02d ",L"Команда ОТМЕНА",otu->kp,otu->tlm_type,otu->sb,otu->obj);
                           monitor_string(str.c_str(),NULL,color);
          break;
     case  CmEnableTU    :
                           str.printf(L"%s ТУ/ТР %03d.%02d.%02d - %02d ",L"РАЗРЕШЕНИЕ",otu->kp,otu->tlm_type,otu->sb,otu->obj);
                           monitor_string(str.c_str(),NULL,color);
          break;
     case  CmDisableTU   :
                           str.printf(L"%s ТУ/ТР %03d.%02d.%02d - %02d ",L"ЗАПРЕЩЕНИЕ",otu->kp,otu->tlm_type,otu->sb,otu->obj);
                           monitor_string(str.c_str(),NULL,color);
          break;
     case  CmErrorTU     : monitor_string(L"ОШИБКА ТУ",NULL,RGB(255,0,0));
          break;
     case  CmActivateTR  : str.printf(L"%s ТУ/ТР %03d.%02d.%02d - %02d ",L"Активизация",otu->kp,otu->tlm_type,otu->sb,otu->obj);
                           monitor_string(str.c_str(),NULL,color);

          break;
     case  CmQuery       :
     case  CmQueryCp     :
     case  CmQueryFa     :
     case  CmQueryGroup  : monitor_string(L"Вызов данных");
          break;
     case  CmCheckConnect: //monitor_string("Проверка связи");
          break;

     case CmRetranslation:
         monitor_gratony_kadr((lp_gratony_kadr) kadr->data,color);
     break;
   }
 }
}

void     __fastcall make_data_str(TCHAR * text,DWORD text_sz,lp_gratony_kadr kadr)
{
  LPBYTE data = kadr->data;
  DWORD  data_len = (kadr->len - sizeof(*kadr)+sizeof(kadr->data)+sizeof(kadr->kadr_len));
  DWORD  text_len = 0;
  *text = 0;
  if(data_len<256)
  {
  LPBYTE data_end = data+data_len;
  while(text_len+4<text_sz && data<data_end)
  {
   text_len+=wsprintf(text+text_len,_T("%02X "),(DWORD)*data);
   data++;
  }
 }
}

void     __fastcall make_data_str_word(TCHAR * text,DWORD text_sz,lp_gratony_kadr kadr)
{
  LPWORD data = (LPWORD)kadr->data;
  DWORD  data_len = (kadr->len - sizeof(*kadr)+sizeof(kadr->data)+sizeof(kadr->kadr_len))/sizeof(WORD);
  DWORD  text_len = 0;
  *text = 0;
  if(data_len<256)
  {
  LPWORD data_end = data+data_len;
  while(text_len+4<text_sz && data<data_end)
  {
   text_len+=wsprintf(text+text_len,_T("%04X "),(DWORD)*data);
   data++;
  }
  }
}

void     __fastcall TGrIpxModuleWnd::monitor_gratony_kadr( lp_gratony_kadr  kadr,DWORD color)
{
 UnicodeString str;
 TCHAR       mon_text[1024];
 str.printf(L"КП-%03d ГРУППА-%03d ",(DWORD)kadr->numCp,(DWORD)kadr->numGrp);
 
 switch((DWORD)kadr->InfoType)
 {
  case tpTs    : str+= L" сигнализация";
                 make_data_str(mon_text,KERTL_ARRAY_COUNT(mon_text),kadr);break;
  case tpTit   : str+= L" измерения";
                 make_data_str(mon_text,KERTL_ARRAY_COUNT(mon_text),kadr);break;
  case tpReady : str+= L" готовность";
                 make_data_str_word(mon_text,KERTL_ARRAY_COUNT(mon_text),kadr);break;
 }
 if(!kadr->P)
   str+=L" ** Ответ на вызов ** ";
   
 monitor_string(str.c_str(),mon_text,color);
}

void     __fastcall TGrIpxModuleWnd::monitor_string(TCHAR * text1,TCHAR * text2,DWORD color)
{
  monitor->add_line(text1,color);
  if(text2)
     monitor->add_line(text2,color);
  

}

void __fastcall TGrIpxModuleWnd::RadioGroup1Click(TObject *Sender)
{
  monitor_switch = RadioGroup1->ItemIndex;
  if(monitor_switch == 3)
    {
     GroupBox4->Visible = true;
     filtrCp  =  atoi(filtrCpEdit->Text.c_str());
     filtrGrp =  atoi(filtrGrpEdit->Text.c_str());
     filterFaEditChange(this);
    }
    else
    GroupBox4->Visible = false;
}
//---------------------------------------------------------------------------



void __fastcall TGrIpxModuleWnd::filtrCpEditChange(TObject *Sender)
{
  filtrCp  =  atoi(filtrCpEdit->Text.c_str());
}
//---------------------------------------------------------------------------

void __fastcall TGrIpxModuleWnd::filtrGrpEditChange(TObject *Sender)
{
     filtrGrp =  atoi(filtrGrpEdit->Text.c_str());
}
//---------------------------------------------------------------------------


void __fastcall TGrIpxModuleWnd::ClientsTreeChange(TObject *Sender,
      TTreeNode *Node)
{
   DWORD  val = (DWORD)Node->Data;
   selected_client_id   = HIWORD(val);
   selected_line_number = LOWORD(val);
   StatusBar1->Panels->Items[1]->Text = "";
   StatusBar1->Panels->Items[2]->Text = "";
   MODEM_LINE_STATE mls;
   mls.dw_size = sizeof(mls);

   if(GKH_RET_SUCCESS==mod_iface.call(MODEMCM_GET_LINE_STATE,selected_line_number,(LPARAM)&mls))
   {
    mls.state_changes = -1;
    on_line_state(&mls);
   }
}
//------------------------------------------------------------------------------





void __fastcall TGrIpxModuleWnd::FormDestroy(TObject *Sender)
{
  if(monitor)
    delete monitor;
}
//---------------------------------------------------------------------------

void __fastcall TGrIpxModuleWnd::FormResize(TObject *Sender)
{
  monitor->SetSize(MonitorBox->Width-8,MonitorBox->Height-12);
}
//---------------------------------------------------------------------------


void __fastcall TGrIpxModuleWnd::RadioButton1Click(TObject *Sender)
{
  DWORD work_mode = GRNET_WM_UNDEFINED;
  if(RadioButton1->Checked)
     work_mode = GRNET_WM_AS_SERVER;
  if(RadioButton2->Checked)
     work_mode = GRNET_WM_AS_CLIENT;
  Server->Visible =  (work_mode!=(DWORD)GRNET_WM_UNDEFINED) ? true:false;
  ServerTUTR->Visible =  (work_mode==(DWORD)GRNET_WM_AS_SERVER) ? true:false;
  Client->Visible =  (work_mode==(DWORD)GRNET_WM_AS_CLIENT) ? true:false;
  if(work_mode != current_settings.work_mode)
  {
    settings_change_mask|=GRNET_CFG_WORKMODE;
  }
  else
    settings_change_mask&=~GRNET_CFG_WORKMODE;

   bool enabled = settings_change_mask ? true:false;
   ApplyBtn->Enabled = enabled;
   UndoBtn->Enabled  = enabled;
   set_form_caption();
}
//---------------------------------------------------------------------------

void __fastcall TGrIpxModuleWnd::PUNumberEditChange(TObject *Sender)
{
 DWORD value = atoi(this->PUNumberEdit->Text.c_str());
 if(current_settings.pu_number == (DWORD)value)
   {
    settings_change_mask &=~GRNET_CFG_PUNUMBER;
    PUNumberEdit->Font->Color = clBlack;
   }
   else
   {
    settings_change_mask |=GRNET_CFG_PUNUMBER;
    PUNumberEdit->Font->Color = clBlue;
   }
   bool enabled = settings_change_mask ? true:false;
   ApplyBtn->Enabled = enabled;
   UndoBtn->Enabled = enabled;
}
//---------------------------------------------------------------------------
void __fastcall TGrIpxModuleWnd::RetryTimeoutEditChange(TObject *Sender)
{
 int value1 = atoi(this->RetryCountEdit->Text.c_str());
 int value2 = atoi(this->RetryTimeoutEdit->Text.c_str());
 settings_change_mask &=~GRNET_CFG_TXPARAM;
 if(current_settings.tx_retry_count == (DWORD)value1)
   {
    RetryCountEdit->Font->Color = clBlack;
   }
   else
   {
    settings_change_mask |=GRNET_CFG_TXPARAM;
    RetryCountEdit->Font->Color = clBlue;
   }
 if(current_settings.tx_retry_delay == (DWORD)value2)
   {
    RetryTimeoutEdit->Font->Color = clBlack;
   }
   else
   {
    settings_change_mask |=GRNET_CFG_TXPARAM;
    RetryTimeoutEdit->Font->Color = clBlue;
   }
   bool enabled = settings_change_mask ? true:false;
   ApplyBtn->Enabled = enabled;
   UndoBtn->Enabled = enabled;
}
//---------------------------------------------------------------------------



void __fastcall TGrIpxModuleWnd::ServerNameChange(TObject *Sender)
{
  DWORD socket_num =  atoi(ServerSocket->Text.c_str());
  AnsiString name   =  ServerName->Text;
  settings_change_mask &=~GRNET_CFG_SERVERPARAM;
  if(current_settings.server_socket!=socket_num)
   {
    settings_change_mask |=GRNET_CFG_SERVERPARAM;
    ServerSocket->Font->Color = clBlue;
   }
   else
    ServerSocket->Font->Color = clBlack;

   if(strnicmp(name.c_str(),current_settings.server_name,sizeof(current_settings.server_name)-1))
   {
    settings_change_mask |=GRNET_CFG_SERVERPARAM;
    ServerName->Font->Color = clBlue;
   }
   else
    ServerName->Font->Color = clBlack;
   set_form_caption();
   bool enabled = settings_change_mask ? true:false;
   ApplyBtn->Enabled = enabled;
   UndoBtn->Enabled = enabled;

}
//---------------------------------------------------------------------------


void __fastcall TGrIpxModuleWnd::ClientSocketChange(TObject *Sender)
{
  DWORD socket_num =  atoi(ClientSocket->Text.c_str());
  AnsiString name   =  ClientName->Text;
    settings_change_mask &=~GRNET_CFG_CLIENTPARAM;
 if(current_settings.server_socket!=socket_num)
   {
    ClientSocket->Font->Color = clBlack;
   }
   else
   {
    settings_change_mask |=GRNET_CFG_CLIENTPARAM;
    ClientSocket->Font->Color = clBlue;
   }

   if(0==strnicmp(name.c_str(),current_settings.client_name,sizeof(current_settings.client_name)-1))
   {
    ClientName->Font->Color = clBlack;
   }
   else
   {
    settings_change_mask |=GRNET_CFG_CLIENTPARAM;
    ClientName->Font->Color = clBlue;
   }
   set_form_caption();

   bool enabled = settings_change_mask ? true:false;
   ApplyBtn->Enabled = enabled;
   UndoBtn->Enabled = enabled;


}
//---------------------------------------------------------------------------

void __fastcall TGrIpxModuleWnd::ModemNumberEditChange(TObject *Sender)
{
 DWORD value = atoi(this->ModemNumberEdit->Text.c_str());
 if(current_settings.modem_number == (DWORD)value)
   {
    settings_change_mask &=~GRNET_CFG_MODEMNUMBER;
    ModemNumberEdit->Font->Color = clBlack;
   }
   else
   {
    settings_change_mask |=GRNET_CFG_MODEMNUMBER;
    ModemNumberEdit->Font->Color = clBlue;
   }
   bool enabled = settings_change_mask ? true:false;
   ApplyBtn->Enabled = enabled;
   UndoBtn->Enabled  = enabled;
}
//---------------------------------------------------------------------------

void __fastcall TGrIpxModuleWnd::StartBtnClick(TObject *Sender)
{
  begin_command("Запуск модуля");
  end_command(mod_iface.start(MODULE_START_REQUEST,0));
}
//---------------------------------------------------------------------------

void __fastcall TGrIpxModuleWnd::StopBtnClick(TObject *Sender)
{
  begin_command("Останов модуля");
  end_command(mod_iface.stop(MODULE_STOP_REQUEST));
}
//---------------------------------------------------------------------------



void     __fastcall TGrIpxModuleWnd::scan_diag()
{
  DiagView->Items->Clear();
  grdiag_map gd;

  mod_iface.call(GRNETCM_GETDIAG_TABLE,(LPARAM)&gd,MAX_CP_COUNT);

  lp_grdiag_item item = gd.items;
  for(DWORD i = 0;i<MAX_CP_COUNT;i++)
  {
     handle_diag(i,item->state,item->mask);
     item++;
  }
}

void     __fastcall TGrIpxModuleWnd::handle_diag(DWORD cp_num,WORD diag,WORD exist)
{
  AnsiString  str = int(cp_num);
  TListItem * item = DiagView->FindCaption(1,str,false,true,true);
  if(exist&GRDIAG_CP)
  {
  if(!item)
     {
      
      item = DiagView->Items->Add();
      item->Caption = str;
      item->Data = (LPVOID)0xFFFF;
      str.Delete(1,str.Length());
      item->SubItems->Add(str);
      item->SubItems->Add(str);
      item->SubItems->Add(str);
     }
     DWORD new_diag = MAKELONG(diag,exist);
     if(new_diag^(DWORD)item->Data)
     {
     item->Data = (LPVOID)new_diag;
     if(item->Selected)
        DiagViewChange(this,item,ctState);
     diag&=exist;
     str.Delete(1,str.Length());
     if(diag&GRDIAG_CP)
        str = "** Не";
        str+= "работает";
     item->SubItems->Strings[0] = str;
     str.Delete(1,str.Length());
     DWORD mask = 1;
     for(int i =0;i<8;i++)
     {
      if(diag&mask)
        {
         if(str.Length())
           str+=',';
           str+= i;
        }
      mask<<=1;
     }
     
     item->SubItems->Strings[1] = str;
     str.Delete(1,str.Length());
     mask = 0x100;
     for(int i =0;i<7;i++)
     {
      if(diag&mask)
        {
         if(str.Length())
           str+=',';
         str+= i;
        }
      mask<<=1;
     }
     
     item->SubItems->Strings[2] = str;
     if((diag&exist)==exist)
       item->StateIndex = diag  = 2;
       else
       item->StateIndex = diag ? 1:0;
    }
  }
  else
  if(item) item->Delete();

}



void __fastcall TGrIpxModuleWnd::DiagViewChange(TObject *Sender,
      TListItem *Item, TItemChange Change)
{
  AnsiString str;
  if(Item)
  {
   DWORD val   = (DWORD)Item->Data;
   WORD  diag  = LOWORD(val);
   WORD  exist = HIWORD(val);
   str.printf("Диагностика %04X  Маска %04X",diag,exist);
   WORD  ts_mask  = 1;
   WORD  tit_mask = 0x100;
   diag&=exist;
   AnsiString ts_exist,ts_nowork,tit_exist,tit_nowork;
   for(int i = 0;i<8;i++ )
   {
    ts_mask &=GRDIAG_TS_MASK;
    tit_mask&=GRDIAG_TIT_MASK;
    ts_exist. cat_printf ("%c ",(exist&ts_mask)? '*':' ');
    ts_nowork.cat_printf ("%c ",(diag &ts_mask)? '*':' ');
    if(i<7)
    {
    tit_exist. cat_printf("%c ",(exist&tit_mask)? '*':' ');
    tit_nowork.cat_printf("%c ",(diag &tit_mask)?'*':' ');
    }
    ts_mask <<=1;
    tit_mask<<=1;
   }
   this->ts_exist->Caption  = ts_exist;
   this->ts_nowork->Caption = ts_nowork;
   this->tit_exist->Caption  = tit_exist;
   this->tit_nowork->Caption = tit_nowork;
  }
  StatusBar1->Panels->Items[1]->Text = str;
}
//---------------------------------------------------------------------------

void __fastcall TGrIpxModuleWnd::DiagViewCompare(TObject *Sender,
      TListItem *Item1, TListItem *Item2, int Data, int &Compare)
{
   Compare =  Item1->Caption.ToInt()-Item2->Caption.ToInt();
}
//---------------------------------------------------------------------------




void __fastcall TGrIpxModuleWnd::filterFaEditChange(TObject *Sender)
{

 switch(filterFaEdit->ItemIndex)
 {
  case 0: filtrFa = tpTs;break;
  case 1: filtrFa = tpTit;break;
  case 2: filtrFa = tpUr;break;
  case 3: filtrFa = tpReady;break;
  case 4: filtrFa = 255;break;
 }
}
//---------------------------------------------------------------------------


void __fastcall TGrIpxModuleWnd::RefreshBtnClick(TObject *Sender)
{
  mod_iface.call(MODEMCM_REFRESH_LINE,LPARAM(selected_line_number),0);
}
//---------------------------------------------------------------------------

void __fastcall TGrIpxModuleWnd::RxEnableMonitorClick(TObject *Sender)
{
  if(RxEnableMonitor->Checked)
   set_notify(mod_iface(),GRNET_NOTIFY_RXDATA);
   else
   reset_notify(mod_iface(),GRNET_NOTIFY_RXDATA);
   Flash(0);
}
//---------------------------------------------------------------------------

void __fastcall TGrIpxModuleWnd::TxEnableMonitorClick(TObject *Sender)
{
  if(TxEnableMonitor->Checked)
   set_notify(mod_iface(),GRNET_NOTIFY_TXDATA);
   else
   reset_notify(mod_iface(),GRNET_NOTIFY_TXDATA);
   Flash(0);
}
//---------------------------------------------------------------------------


void __fastcall TGrIpxModuleWnd::ClearMonBtnClick(TObject *Sender)
{
 monitor->clear();
 Flash(0);       
}
//---------------------------------------------------------------------------

void __fastcall TGrIpxModuleWnd::CopyWndBtnClick(TObject *Sender)
{
 TGrIpxModuleWnd * mwnd = 
 new TGrIpxModuleWnd(this,NULL,mod_iface());
 mwnd->PageControl1->ActivePageIndex = PageControl1->ActivePageIndex;
 mwnd->Show();
}
//---------------------------------------------------------------------------

void __fastcall TGrIpxModuleWnd::FormClose(TObject *Sender,
      TCloseAction &Action)
{
  if(Action == caHide && !owner_window)
     Action =  caFree;
}
//---------------------------------------------------------------------------

void __fastcall TGrIpxModuleWnd::TuClientsListSelectItem(TObject *Sender,
      TListItem *item, bool Selected)
{
 if(item && Selected)
 {
   TuClientName->Text = item->SubItems->Strings[0];
   TuClientSocket->Text = item->SubItems->Strings[1];
 }
}
//---------------------------------------------------------------------------


void __fastcall TGrIpxModuleWnd::SpeedButton2Click(TObject *Sender)
{
   if(RadioButton1->Checked && TuClientsList->Selected)
   {
    char text[128];
    KeRTL::Unicode2Ansi(text,sizeof(text),TuClientName->Text.c_str());
    grnet_tu_client gtc(text,_wtoi(TuClientSocket->Text.c_str()));
    mod_iface.call(GRNET_SET_TUCLIENT,(LPARAM)&gtc,TuClientsList->Selected->Index);
   }


}
//---------------------------------------------------------------------------

void     __fastcall TGrIpxModuleWnd::update_tu_client(DWORD idx)
{
 if(idx< (DWORD)TuClientsList->Items->Count)
  {
 GRNET_TU_CLIENT gtc;
 gtc.dw_size  = sizeof(gtc);
 if(mod_iface.call(GRNET_GET_TUCLIENT,(LPARAM)&gtc,idx)==GKH_RET_SUCCESS)
 {
   TListItem * item = TuClientsList->Items->Item[idx];
   item->SubItems->Strings[0]  =  gtc.name;
   if(*gtc.name)
      item->SubItems->Strings[1] = gtc.sock_num;
      else
      item->SubItems->Strings[1] = '\0';
  }
 }
}

void __fastcall TGrIpxModuleWnd::SpeedButton3Click(TObject *Sender)
{
   if(RadioButton1->Checked && TuClientsList->Selected)
   {

    mod_iface.call(GRNET_DEL_TUCLIENT,TuClientsList->Selected->Index,0);
   }

}
//---------------------------------------------------------------------------

bool     __fastcall TGrIpxModuleWnd::is_tu_enabled(char * name,WORD sock_num)
{
 bool ret = false;
 for(int i = 0;!ret && ServerTUTR->Visible && i< TuClientsList->Items->Count;i++)
 {
  TListItem * item = TuClientsList->Items->Item[i];
  if(item->SubItems->Strings[0] == UnicodeString(name)  &&
     _wtoi(item->SubItems->Strings[1].c_str()) == sock_num)
     ret = true;
 }
 return ret;
}

void __fastcall TGrIpxModuleWnd::TuEnableBtnClick(TObject *Sender)
{
 ;//Разрешение ТУ/ТР Клиенту
 bool enable = TuEnable->Checked;
 GRNET_CLIENT_INFO cli;
 cli.dw_size  = sizeof(cli);
 if(ClientsTree->Selected)
 {
  DWORD  line_num = (DWORD)ClientsTree->Selected->Data;
  line_num &= 0xFFFF;
  if(GKH_RET_SUCCESS == mod_iface.call(GRNETCM_GET_CLIENT_INFO,line_num,(LPARAM)&cli))
  {
    enable_tu_client(cli.client_name,_htons(cli.host_addr.sa_socket),enable);
  }
 }
}
//---------------------------------------------------------------------------

bool     __fastcall TGrIpxModuleWnd::enable_tu_client(char * name,WORD sock_num,bool enable)
{
 bool ret = false;
 if(enable)
   {
    //Разрешение ТУ
    //1) Ищем свободное место
    for(int i = 0;!ret && i<TuClientsList->Items->Count;i++)
    {
       TListItem * item =TuClientsList->Items->Item[i];
         if(!item->SubItems->Strings[0].Length())
         {
          item->Selected = true;
          TuClientName->Text   = name;
          TuClientSocket->Text = sock_num;
          SpeedButton2Click(this);
          ret = true;
         }
    }
   }
   else
   {
    for(int i = 0;!ret && i<TuClientsList->Items->Count;i++)
    {
       TListItem * item =TuClientsList->Items->Item[i];
         if(item->SubItems->Strings[0] == UnicodeString(name) &&
             _wtoi(item->SubItems->Strings[1].c_str()) == sock_num)
          {
           item->Selected   = true;
           SpeedButton3Click(this);
           ret = true;
          }

    }
   }

  return ret;
}


void     __fastcall TGrIpxModuleWnd::begin_command(char * text)
{
 StatusBar1->Panels->Items[1]->Text = text;
}

void     __fastcall TGrIpxModuleWnd::end_command  (DWORD cmd_ret)
{
 wchar_t err_text[MAX_PATH];
 switch(cmd_ret)
 {
  case GKH_RET_ERROR:
       mod_iface.get_error_text(GetLastError(),err_text,sizeof(err_text)/sizeof(wchar_t));
  break;
  case GKH_RET_EXCEPTION:
   safe_strcpy(err_text,L"Исключение при выполнении...");
   break;
  case GKH_RET_CANCEL:
   safe_strcpy(err_text,L"Команда отменена");
   break;
  case GKH_RET_SUCCESS:
   safe_strcpy(err_text,L"OK");
   break;
 }
 StatusBar1->Panels->Items[2]->Text = err_text;
}




void __fastcall TGrIpxModuleWnd::PuNameChange(TObject *Sender)
{
 WideString w_str = PuName->Text.Length() ? PuName->Text.c_str():L"";
 if(w_str.Length() && lstrcmpiW(w_str.c_bstr(),current_settings.pu_name))
 {
    settings_change_mask |=GRNET_CFG_PUNAME;
    PuName->Font->Color = clBlue;
 }
 else
 {
    settings_change_mask &=~GRNET_CFG_PUNAME;
    PuName->Font->Color = clBlack;
 }
   bool enabled = settings_change_mask ? true:false;
   ApplyBtn->Enabled = enabled;
   UndoBtn->Enabled  = enabled;

}
//---------------------------------------------------------------------------

