//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include <gkio_base.h>
#include "grantablet_wnd.h"
//#include <modemmer.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "gkmodule_form"
#pragma resource "*.dfm"
TGranTabledFrm *GranTabledFrm;
//---------------------------------------------------------------------------
__fastcall TGranTabledFrm::TGranTabledFrm(TComponent* Owner,HWND parent,GKHANDLE _handle )
        : TGKModuleForm(Owner,parent,_handle)
{
 root = 0;
 ZeroMemory(&settings,sizeof(settings));
 settings.dw_size = sizeof(settings);
 in_setup = false;
 create_mode = false;
 Fconfig_changes = 0;
 txrx_mon = 0;
 #ifdef COMERCIAL_BUILD
 Label30->Caption = "ООО \"Компания промышленной автоматизации\"";
 #endif
 
}

//---------------------------------------------------------------------------
int WINAPI node_comparer(LPARAM p1,LPARAM p2,LPARAM sort_param)
{
  TTreeNode * node1 = (TTreeNode *)p1;
  TTreeNode * node2 = (TTreeNode *)p2;
  int floor1 = HIWORD(node1->Data);
  int pos1   = LOWORD(node1->Data);
  int floor2 = HIWORD(node2->Data);
  int pos2   = LOWORD(node2->Data);
  if(floor1 == floor2)
    return pos1-pos2;
  return floor1-floor2;  
}

void     __fastcall TGranTabledFrm::on_module_state(GKHANDLE mod,DWORD state)
{
  bool running = ((state & MODULE_STATE_RUNNING_MASK )== MODULE_STATE_RUNNING) ? true:false;
  bool stopped  = (state & MODULE_STATE_RUNNING_MASK ) ? false:true;
  StartBtn->Enabled = stopped;
  StopBtn->Enabled = running;
  AnsiString str;
  if(stopped)
     str =    "Вывод остановлен";
  if(running)
     str =  "Вывод работает";
   StatusBar1->Panels->Items[0]->Text = str;  
}

void     __fastcall TGranTabledFrm::on_module_config_change(GKHANDLE mod,LPMODULE_CONFIG_DATA mcd)
{
 LPGRTBL_SETTINGS p_set = (LPGRTBL_SETTINGS)mcd->data;
 config_changes&=~mcd->mask;
    if(mcd->mask&CCFL_MODEM_NUMBER)
    {
     settings.modem_number = p_set->modem_number;
     ModemNumber->Text     = p_set->modem_number;
     ModemNumber->Font->Color = clDefault;
    }

    if(mcd->mask&CCFL_IOMODULE)
     {
      safe_strcpy(settings.io_module,p_set->io_module);
      IoModule->Text =p_set->io_module;
      IoModule->Font->Color = clDefault;
     }
    if(mcd->mask&CCFL_IODEVICE)
    {
     safe_strcpy(settings.io_device,p_set->io_device);
     DeviceName->Text = p_set->io_device;
     DeviceName->Font->Color = clDefault;
    }

    if(mcd->mask&CCFL_DBFILE)
     {
      safe_strcpy(settings.db_file,p_set->db_file);
      DbFile->Text = p_set->db_file;
      DbFile->Font->Color = clDefault;
     }
    if(mcd->mask & CCFL_IOSPEED)
    {
      settings.io_speed = p_set->io_speed;
      DeviceSpeed->Text = p_set->io_speed;
      DeviceSpeed->Font->Color = clDefault;
    }
    if(mcd->mask & CCFL_IOWATCHDOG)
    {
      settings.io_watchdog = p_set->io_watchdog;
      IoWatchDog->Checked  = p_set->io_watchdog;
      IoWatchDog->Font->Color = clDefault;
    }
}

LRESULT  __fastcall TGranTabledFrm::on_gkhandle_notify (GKHANDLE from,LPNOTIFY_CODE nc,DWORD mask)
{
 LRESULT ret;
 switch(mask)
 {
  case  GRTBL_NM_TXRX:
         on_txrx_notify((LPMIO_REQUEST)nc->notify_data,nc->dw_size - sizeof(*nc)+sizeof(nc->notify_data[0]), nc->notify_code == GRTBL_NC_TX ? true:false);
         break;
  case GRTBL_NM_KADRDATA:
         if(nc->notify_code == GRTBL_NC_KADRDATA_END)
         on_enum_kadr(NULL,true);
         else
         on_enum_kadr((LPOTDM_PROTO_KADR)nc->notify_data,false);
         break;
  default : TGKModuleForm::on_gkhandle_notify(from,nc,mask);
 }
 return ret;
}



bool     __fastcall TGranTabledFrm::get_config_param(LPGRTBL_SETTINGS psett)
{
 BYTE buffer[sizeof(*psett)+sizeof(MODULE_CONFIG_DATA)];
 ZeroMemory(buffer,sizeof(buffer));
 LPMODULE_CONFIG_DATA mcd = (LPMODULE_CONFIG_DATA)buffer;
 mcd->dw_sz = sizeof(buffer);
 LPGRTBL_SETTINGS  mod_config =(LPGRTBL_SETTINGS) mcd->data;
 mod_config->dw_size = sizeof(*mod_config);
 if(psett && mod_iface.get_config_data(mcd,sizeof(buffer)))
   {
    memcpy(psett,mod_config,sizeof(*psett));
    return true;
   }  
 return false;
}

void     __fastcall TGranTabledFrm::set_config_changes(DWORD value)
{
  if(!in_setup)
  { 
    if(Fconfig_changes!=value)
    {
     Fconfig_changes=value;
    }
     ApplyBtn->Enabled = value;
     UndoBtn->Enabled  = value;
   }
}




void     __fastcall TGranTabledFrm::after_set_gkhandle()
{
 TGKModuleForm::after_set_gkhandle();
 if(root)
  root->DeleteChildren();
 else
 {
  root = OutFloorsTree->Items->Add(0,"Этажи вывода");
  root->Data = (LPVOID)-1;
  enum_floors();
 }
 root->Selected = true;
 get_config_param(&settings);
 setup_settings_panel();
 config_changes = 0;
 on_module_state(mod_iface(),mod_iface.get_module_state());
 set_notify(mod_iface(),MNF_COMMON|GRTBL_NM_TXRX);
 root->Expand(false);
}

void __fastcall TGranTabledFrm::enum_floors()
{
  //Перечисление этажей
  DWORD index = 0;
  MODEM_LINE_STATS mls;
  mls.dw_size      = sizeof(mls);

  while(GKH_RET_SUCCESS==mod_iface.call(MODEMCM_ENUM_LINES,index++,(LPARAM)&mls))
  {
   add_floor_node(root,mls.addr.line);
  }

}

TTreeNode *   __fastcall TGranTabledFrm::add_floor_node(TTreeNode * root,DWORD num)
{
  BYTE  buf[1024];
  LPMODEM_LINE_PROPS mlp =(LPMODEM_LINE_PROPS)buf;
  mlp->dw_size = sizeof(buf);
  mlp->line_text_sz  =(mlp->dw_size-sizeof(*mlp))/sizeof(wchar_t);
  AnsiString text;
  if(GKH_RET_SUCCESS == mod_iface.call(MODEMCM_GET_LINE_PROPS,num,(LPARAM)mlp))
     text = mlp->line_text;
   else
     text.printf("Этаж вывода № %d",num);
  TTreeNode * floor = OutFloorsTree->Items->AddChild(root,text);
  floor->Data =(LPVOID) MAKELONG(0,num);
  //set_node_image(floor);
  enum_output_kadrs(floor);
   root->CustomSort(node_comparer,0,false);
  return floor;
}

void         __fastcall TGranTabledFrm::enum_output_kadrs(TTreeNode * floor)
{
 BYTE buffer[2048];
 LPOUTPUT_KADR_PARAM  okp = (LPOUTPUT_KADR_PARAM) buffer;
 ZeroMemory(okp,sizeof(*okp));
 okp->dw_size = sizeof(buffer);
 okp->kadr_name_size = okp->dw_size-sizeof(*okp);
 okp->floor_number   = HIWORD(floor->Data);
 DWORD index = 0;
 while(GKH_RET_SUCCESS == mod_iface.call(GRTBL_CMD_ENUM_OUTPUT_KADR,(LPARAM)okp,index++) )
    add_output_kadr_node(floor,okp);
}

TTreeNode *  __fastcall TGranTabledFrm::add_output_kadr_node(TTreeNode * floor,LPOUTPUT_KADR_PARAM okp)
{
 AnsiString str;
 str.printf("Место вывода № - %d ",okp->output_position);
 TTreeNode * kadr_node = this->OutFloorsTree->Items->AddChild(floor,str);
 kadr_node->Data = (LPVOID)MAKELONG(okp->output_position,okp->floor_number);
 floor->CustomSort(node_comparer,0,false);
 return kadr_node;
}




void __fastcall TGranTabledFrm::OutFloorsTreeChange(TObject *Sender,
      TTreeNode *Node)
{



 if(Node == root)
 {
  SettingsPanel->Visible = true;
  KadrPanel ->Visible = false;
  FloorPanel->Visible = false;
  DeleteBtn->Enabled = false;
  OutParamsSheet->Visible = false;
  get_config_param(&settings);
  setup_settings_panel();

 }
 else
 {
  DeleteBtn->Enabled = true;
  if( LOWORD(Node->Data))
    {
     clear_output_param_grid();
     mod_iface.call(MODEMCM_REFRESH_LINE,HIWORD(Node->Data),LOWORD(Node->Data));
     KadrPanel ->Visible    = true;
     SettingsPanel->Visible = false;
     FloorPanel->Visible    = false;
     setup_kadr_panel();
     OutParamsSheet->Visible = true;


    }
    else
    {
     FloorPanel->Visible = true;
     SettingsPanel->Visible = false;
     KadrPanel ->Visible    = false;
     setup_floor_panel();
     OutParamsSheet->Visible = false;

    }
 }
 config_changes = 0;
}
//---------------------------------------------------------------------------

void     __fastcall TGranTabledFrm::setup_floor_panel()
{
  in_setup = true;
  FloorNumber->Text = HIWORD(OutFloorsTree->Selected->Data);
  in_setup = false;
}

void     __fastcall TGranTabledFrm::setup_kadr_panel ()
{
 in_setup = true;
 KadrDescript->Items->Clear();
 mod_iface.call(GRTBL_CMD_ENUM_DBKADRS,(LPARAM)form_gkhandle,0);
 DWORD floor_data = (DWORD )OutFloorsTree->Selected->Data;
 BYTE buffer[2048];
 LPOUTPUT_KADR_PARAM  okp = (LPOUTPUT_KADR_PARAM) buffer;
 ZeroMemory(okp,sizeof(*okp));
 okp->dw_size = sizeof(buffer);
 okp->kadr_name_size  = okp->dw_size-sizeof(*okp);
 okp->floor_number    = HIWORD(floor_data);
 okp->output_position = LOWORD(floor_data);
 mod_iface.call(GRTBL_CMD_GET_OUTPUT_KADR_PARAM,(LPARAM)okp,0);
 AnsiString str;
 str = okp->output_kadr_id;
 str+=' ';
 str+=okp->kadr_name;
 KadrDescript->Text    = str;
 OutputPosition->Text  = okp->output_position;
 OutputMode->ItemIndex = okp->output_mode;
 in_setup = false;
}


void     __fastcall TGranTabledFrm::setup_settings_panel()
{
 in_setup = true;
 ModemNumber->Text = settings.modem_number;
 IoModule->Text    = settings.io_module;
 DeviceName->Text  = settings.io_device;
 DeviceSpeed->Text = settings.io_speed;
 IoWatchDog->Checked = settings.io_watchdog;
 IoNotReset->Checked = settings.io_not_reset;
 DbFile->Text        = settings.db_file;
 in_setup = false;
}

void __fastcall TGranTabledFrm::DeviceNameDropDown(TObject *Sender)
{
   DeviceName->Items->Clear();
   TGKIoInterface  io;
   if(io.load(mod_iface(),IoModule->Text.c_str(),NULL))
   {
    MIO_ENUM_DEVICES ed;
    ed.dw_size  = sizeof(ed);
    ed.index = 0;
    while(io.enum_devices(&ed) == GKH_RET_SUCCESS)
    {
     DeviceName->Items->Add(ed.dev_name);
     ed.index++;
    }
    DeviceName->DropDownCount = KeRTL::MAX(1,DeviceName->Items->Count);
   }

}
//---------------------------------------------------------------------------

void __fastcall TGranTabledFrm::ModemNumberChange(TObject *Sender)
{
  DWORD modem_number = atoi(ModemNumber->Text.c_str());
  if(modem_number!=this->settings.modem_number)
   {
    ModemNumber->Font->Color = clBlue;
    config_changes|=CCFL_MODEM_NUMBER;
   }
   else
   {
    ModemNumber->Font->Color = clDefault;
    config_changes&=~CCFL_MODEM_NUMBER;
   }
}
//---------------------------------------------------------------------------

void __fastcall TGranTabledFrm::IoModuleChange(TObject *Sender)
{
 AnsiString io_mod = settings.io_module;
 if(io_mod.AnsiCompareIC(IoModule->Text))
 {
  IoModule->Font->Color = clBlue;
    config_changes|=CCFL_IOMODULE;
 }
 else
 {
  IoModule->Font->Color = clDefault;
    config_changes&=~CCFL_IOMODULE;
 }
}
//---------------------------------------------------------------------------

void __fastcall TGranTabledFrm::DeviceNameChange(TObject *Sender)
{

 AnsiString io_dev = settings.io_device;
 if(io_dev.AnsiCompareIC(DeviceName->Text))
 {
  DeviceName->Font->Color = clBlue;
    config_changes |= CCFL_IODEVICE;
 }
 else
 {
    DeviceName->Font->Color = clDefault;
    config_changes&=~CCFL_IODEVICE;
 }
}
//---------------------------------------------------------------------------

void __fastcall TGranTabledFrm::DeviceSpeedChange(TObject *Sender)
{
 DWORD io_speed  = atoi(DeviceSpeed->Text.c_str());
 if(io_speed!=settings.io_speed)
 {
  DeviceSpeed->Font->Color = clBlue;
    config_changes|=CCFL_IOSPEED;
 }
 else
 {
  DeviceSpeed->Font->Color = clDefault;
    config_changes&=~CCFL_IOSPEED;
 }
}
//---------------------------------------------------------------------------

void __fastcall TGranTabledFrm::IoWatchDogClick(TObject *Sender)
{
  if(IoWatchDog->Checked!=bool(settings.io_watchdog))
    {
     config_changes|= CCFL_IOWATCHDOG;
     IoWatchDog->Font->Color = clBlue;
    }
    else
    {
     config_changes&= ~CCFL_IOWATCHDOG;
     IoWatchDog->Font->Color = clDefault;
    }

}

//---------------------------------------------------------------------------
void __fastcall TGranTabledFrm::DbFileChange(TObject *Sender)
{
 AnsiString dbf = settings.db_file;
 if(dbf.AnsiCompareIC(DbFile->Text))
 {
   DbFile->Font->Color = clBlue;
    config_changes|=CCFL_DBFILE;
 }
 else
 {
    DbFile->Font->Color = clDefault;
    config_changes&=~CCFL_DBFILE;
 }

}
//---------------------------------------------------------------------------

void __fastcall TGranTabledFrm::ApplyBtnClick(TObject *Sender)
{
  TTreeNode * node = this->OutFloorsTree->Selected;
  bool res ;
  if(node == root)
    res = apply_module_settings();
   else
   {
    if(LOWORD(node->Data))
      res = apply_kadr_param();
      else
      res = apply_floor_param();
   } 
  wchar_t err_text[MAX_PATH];
  if(!res)
  mod_iface.get_error_text(GetLastError(),err_text,sizeof(err_text)/sizeof(wchar_t));
  else
  safe_strcpy(err_text,L"OK");
  StatusBar1->Panels->Items[1]->Text = err_text;

}
//---------------------------------------------------------------------------

bool __fastcall TGranTabledFrm::apply_module_settings()
{
 settings.modem_number = atoi(ModemNumber->Text.c_str());
 WideString w_str;
 w_str = IoModule->Text;
 *settings.io_module = 0;
 safe_strcpy(settings.io_module,w_str.c_bstr());
 *settings.io_device = 0;
 w_str = DeviceName->Text;
 safe_strcpy(settings.io_device,w_str.c_bstr());

 *settings.db_file = 0;
 w_str = DbFile->Text;
 safe_strcpy(settings.db_file,w_str.c_bstr());
 settings.io_speed = atoi(DeviceSpeed->Text.c_str());
 settings.io_watchdog  = IoWatchDog->Checked;
 settings.io_not_reset = IoNotReset->Checked;

 BYTE buffer [sizeof(MODULE_CONFIG_DATA)+sizeof(settings)];
 LPMODULE_CONFIG_DATA mcd = (LPMODULE_CONFIG_DATA)buffer;
 mcd->dw_sz = sizeof(buffer)-sizeof(mcd->data[0]);
 mcd->mask= config_changes;
 LPGRTBL_SETTINGS pset = (LPGRTBL_SETTINGS) mcd->data;
 memcpy(pset,&settings,sizeof(*pset));
 if(!mod_iface.set_config_data(mcd,mcd->dw_sz))
 {
  return false;
 }
 return true;
}

bool         __fastcall TGranTabledFrm::apply_floor_param()
{
  bool ret = false;
  TTreeNode * node = OutFloorsTree->Selected;
  DWORD       floor_number = FloorNum->Position;
  if(create_mode)
  {
   create_mode = false;
   if(GKH_RET_SUCCESS==mod_iface.call(GRTBL_CMD_CREATE_FLOOR,floor_number,0))
     {
       ret = true;
       OutFloorsTree->Selected = add_floor_node(node->Parent,floor_number);
       node->Delete();
     }
  }
  else
  {
   DWORD old_num = HIWORD(node->Data);
   if(GKH_RET_SUCCESS==mod_iface.call(GRTBL_CMD_SET_FLOOR_NUMBER,old_num,floor_number))
   {
       ret = true;
       OutFloorsTree->Selected = add_floor_node(node->Parent,floor_number);
       node->Delete();
   }
  }

  if(ret)
    config_changes = 0;
  return ret;
}

void         __fastcall TGranTabledFrm::get_kadr_param(OUTPUT_KADR_PARAM &kp )
{
 ZeroMemory(&kp,sizeof(kp));
 kp.dw_size         = sizeof(kp);
 kp.floor_number    = HIWORD(OutFloorsTree->Selected->Data);
 kp.output_position = OutputPositionNum->Position;
 kp.output_mode     = OutputMode->ItemIndex;
 kp.output_kadr_id  = atoi(KadrDescript->Text.c_str());
}

bool         __fastcall TGranTabledFrm::apply_kadr_param()
{
 bool ret = false;
 TTreeNode * node   = OutFloorsTree->Selected;
 DWORD old_position = LOWORD(node->Data);
 OUTPUT_KADR_PARAM kp;
 get_kadr_param(kp);
 if(create_mode)
  {

   if(GKH_RET_SUCCESS==mod_iface.call(GRTBL_CMD_CREATE_FLOOR_OUTPUT,(LPARAM)&kp,old_position))
   {
    create_mode = false;
    OutFloorsTree->Selected = add_output_kadr_node(node->Parent,&kp);
    node->Delete();
    config_changes = 0;
    ret = true;
   }

  }
  else
  {
   if(GKH_RET_SUCCESS==mod_iface.call(GRTBL_CMD_SET_FLOOR_OUTPUT_PARAM,(LPARAM)&kp,old_position))
     {
      node->Data = (LPVOID)MAKELONG(kp.output_position,kp.floor_number);
      AnsiString str;
      str.printf("Место вывода № - %d ",kp.output_position);
      node->Text =  str;
      config_changes = 0;
      node->Parent->CustomSort(node_comparer,0,false);
      ret = true;
     }
  }
  return ret;
}



void __fastcall TGranTabledFrm::StartBtnClick(TObject *Sender)
{
  mod_iface.start(MODULE_START_REQUEST,0);
}
//---------------------------------------------------------------------------

void __fastcall TGranTabledFrm::StopBtnClick(TObject *Sender)
{
 mod_iface.stop(MODULE_STOP_REQUEST);
}
//---------------------------------------------------------------------------

void         __fastcall TGranTabledFrm::on_txrx_notify(LPMIO_REQUEST io_req,DWORD len,bool tx)
{
 if(txrx_mon)
 {
  if(TxRxMonWork->Down)
  {
  AnsiString str = tx ? "Передача" : "Приём   ";
  DWORD color    = tx ? clBlue: clGreen;
  str.cat_printf(" : канал %d, размер данных %d",io_req->channel,io_req->data_len);
  txrx_mon->add_line(str.c_str(),color);
  str.Delete(1,str.Length());
  if(io_req->data_len)
  {
  LPBYTE io_bytes = (LPBYTE) io_req->data;
  for(DWORD i = 0;i<io_req->data_len;++i,++io_bytes)
    str.cat_printf("%02X ",(DWORD)*io_bytes);
   txrx_mon->add_line(str.c_str(),color);
  }
  }
 }
 if(tx)
 {
   LPBYTE floor_afb =(LPBYTE)io_req->data;
   WORD  floor = (WORD)(floor_afb[0]>>4);
   WORD  afb   = (WORD)(floor_afb[1]&0x0F);
   DWORD node_data = MAKELONG(afb,floor);
   if(OutFloorsTree->Selected && OutFloorsTree->Selected->Data == (LPVOID)node_data)
   {
   switch(io_req->data_len)
   {
    case sizeof(gr_proto_out_ti):
      setup_out_params_ti((lpgr_proto_out_ti)io_req->data);
     break;
    case sizeof(gr_proto_out_ts):
      {
       OUTPUT_KADR_PARAM okp;
       okp.dw_size = sizeof(okp);
       okp.floor_number    = floor;
       okp.output_position = afb;
       get_kadr_param(okp);
       if(okp.output_mode == OUTPUT_MODE_READY)
         setup_out_params_ready((lpgr_proto_out_ready)io_req->data);
         else
         setup_out_params_ts((lpgr_proto_out_ts)io_req->data);

      } 
     break;
   }
  }
 }
}

void         __fastcall TGranTabledFrm::clear_output_param_grid()
{
 AnsiString str;
 Label9->Caption = str;
 str = '?';
  for(int col = 0;col<8;col++)
  {
    OutputParams->Cells[col][0] = col;

   for(int row = 1;row<9;row++)
   {
     OutputParams->Cells[col][row] = str;
   }
  } 

}

void __fastcall TGranTabledFrm::setup_out_params_ts( lpgr_proto_out_ts data)
{
  int param = 0;
  for( ;param <64;++param)
   {
     div_t dt = div(param,8);
     OutputParams->Cells[dt.rem][dt.quot+1 ] = gr_get_ts(data->bits,param);
   }
}

void __fastcall get_out_ti_data(LPBYTE data, DWORD & param,char  &digit0,char  &digit1,char  &digit2,DWORD &slaking,DWORD & reverse)
{
 digit0 = '0'+ (data[2]&0x0F);
 digit1 = '0'+ ((data[2]>>4)&0x0F);
 digit2 = '0'+ (data[1]&0x0F);

 slaking = data[1]&0x20 ? TRUE:FALSE;
 reverse = data[1]&0x10 ? TRUE:FALSE;
 param = 0;
 param   |= (data[1]>>6)&3;
 param   |= (data[0]<<2)&~(0xC3);
}

void __fastcall TGranTabledFrm::setup_out_params_ti( lpgr_proto_out_ti data)
{
  DWORD param;
  char  digit0,digit1,digit2;
  DWORD slaking;
  DWORD reverse;
  get_out_ti_data(data->data,param,digit0,digit1,digit2,slaking,reverse);
  div_t dt = div(param,8);
  AnsiString str;
  if(slaking)
   str+="[";
   str+=reverse ? '-':' ';
   str+=digit2;
   str+=digit1;
   str+=digit0;
  if(slaking)
   str+=" ]";
  OutputParams->Cells[dt.rem][dt.quot+1] = str;
}

void __fastcall TGranTabledFrm::setup_out_params_ready(lpgr_proto_out_ready data)
{

  AnsiString str;
  int param = 0;
  for(;param<64; ++param)
  {
   div_t dt = div(param,8);
   if(param<30)
     {

      DWORD ready_on = gr_get_ts(data->ready_state,param);
      DWORD flash    = gr_get_ts(data->ready_flash,param);
      DWORD slaking  = gr_get_ts(data->ready_slaking,param+2);
      str.printf("%d %c %c",ready_on ? 1:0,flash ? 'F':' ',slaking ? 'S':' ');
     }
     else
     str.Delete(1,str.Length());
     OutputParams->Cells[dt.rem][dt.quot+1] = str;
     
   }
}


void __fastcall TGranTabledFrm::FormShow(TObject *Sender)
{
 if(!txrx_mon)
 {
  txrx_mon = new txrx_monitor;
  KeWin::TRect r(0,0,MonitorBox->Width,MonitorBox->Height);
  r.Grow(-10,-10);
  r.Move(4,8);
  if(txrx_mon)
  {
      char txtv_name [MAX_PATH];
      safe_strcpy(txtv_name,"GrantabletMonitor");
      int name_len = lstrlen(txtv_name);
      int create_limit = 128;
      int create_cntr = 0;

      while(create_cntr<create_limit &&
            !txrx_mon->DoCreate(MonitorBox->Handle,r,-1,256,txtv_name) )
      {
        wsprintf(txtv_name+name_len,"%d",create_cntr++);
      }
    txrx_mon->set_lines_limit(8192);
    txrx_mon->SetFont(MonitorBox->Font->Handle,true);
  }
 }
}
//---------------------------------------------------------------------------

void __fastcall TGranTabledFrm::MonSheetResize(TObject *Sender)
{
if(txrx_mon)
  txrx_mon->SetSize(MonitorBox->Width-10,MonitorBox->Height-10);
}
//---------------------------------------------------------------------------

void __fastcall TGranTabledFrm::FormDestroy(TObject *Sender)
{
if(txrx_mon)
  {
   txrx_mon->Destroy();
   delete txrx_mon;
   txrx_mon = NULL;
  }

}
//---------------------------------------------------------------------------

void __fastcall TGranTabledFrm::CreateBtnClick(TObject *Sender)
{
  TTreeNode * parent = OutFloorsTree->Selected;
  if(parent)
  {
   if(parent!=root && LOWORD(parent->Data))
      parent = parent->Parent;
   AnsiString str = parent == root ? "Создание этажа": "Создание элемента вывода";
   TTreeNode * node = OutFloorsTree->Items->AddChild(parent,str);
   if(parent == root)
     {
      FloorNum->Position = FloorNum->Min;
      FloorNumber->Text  = FloorNum->Min;
      node->Data = (LPVOID) MAKELONG(0,-1) ;
     }
     else
     {
      node ->Data = (LPVOID)(0x000000FF|(DWORD)parent->Data&0xFFFF0000);
     }
   OutFloorsTree->Selected = node;
   PageControl1->ActivePageIndex = 0;
   create_mode = true;
  }
}
//---------------------------------------------------------------------------

void __fastcall TGranTabledFrm::OutFloorsTreeChanging(TObject *Sender,
      TTreeNode *Node, bool &AllowChange)
{
  AllowChange = !create_mode;
}
//---------------------------------------------------------------------------

void __fastcall TGranTabledFrm::DeleteBtnClick(TObject *Sender)
{
  TTreeNode * node = OutFloorsTree->Selected;
  if(node)
  {
  if(create_mode)
    {
     create_mode = false;
     OutFloorsTree->Selected = node->GetPrev();
     node->Delete();
    }
    else
    {
     OutFloorsTree->Selected = node->GetPrev();
     if(0 == LOWORD(node->Data))
        mod_iface.call(GRTBL_CMD_DELETE_FLOOR,HIWORD(node->Data),0);
        else
        {
          OUTPUT_KADR_PARAM kp;
          get_kadr_param(kp);
          kp.floor_number    = HIWORD(node->Data);
          kp.output_position = LOWORD(node->Data);
          mod_iface.call(GRTBL_CMD_DELETE_FLOOR_OUTPUT,(LPARAM)&kp,0);
        }
     node->Delete();
    }

   }

}
//---------------------------------------------------------------------------

void __fastcall TGranTabledFrm::UndoBtnClick(TObject *Sender)
{
 if(create_mode)
  DeleteBtnClick(Sender);
  else
  this->OutFloorsTreeChange(this,OutFloorsTree->Selected) ;
  StatusBar1->Panels->Items[1]->Text = ' ';
}
//---------------------------------------------------------------------------

void         __fastcall TGranTabledFrm::on_enum_kadr( LPOTDM_PROTO_KADR kadr_data,bool end)
{
 if(end || !kadr_data)
    reset_notify(mod_iface(),GRTBL_NM_KADRDATA);
  else
  {
   AnsiString  str;
   str.printf("%03d ",kadr_data->kadr.kadr_id);
   str += kadr_data->names;
   KadrDescript->Items->Add(str);
   KadrDescript->DropDownCount = KeRTL::MIN(10,KadrDescript->Items->Count);
  }
}



void __fastcall TGranTabledFrm::OutputPositionChange(TObject *Sender)
{
 config_changes|=CCFL_OUT_FLOORS;
}
//---------------------------------------------------------------------------


void __fastcall TGranTabledFrm::FloorNumberChange(TObject *Sender)
{
  TColor color = clDefault;
  if(OutFloorsTree->Selected && (FloorNum->Position!=(int)HIWORD(OutFloorsTree->Selected->Data)))
      {
       color =  clBlue;
       config_changes|=CCFL_OUT_FLOORS;
      }
      else
      config_changes&=~CCFL_OUT_FLOORS;
  FloorNumber->Font->Color = color;
}
//---------------------------------------------------------------------------

void __fastcall TGranTabledFrm::TxRxMonClearClick(TObject *Sender)
{
  if(txrx_mon)
     txrx_mon->clear();
}
//---------------------------------------------------------------------------



void __fastcall TGranTabledFrm::OutputParamsMouseMove(TObject *Sender,
      TShiftState Shift, int X, int Y)
{
  int ACol,ARow;
  OutputParams->MouseToCell(X,Y,ACol,ARow);
  AnsiString str;
  int p_num = (ARow-1)*8+ACol;
  if(p_num>=0)
    str.printf("Номер параметра %d",p_num);
  Label9->Caption = str;
}
//---------------------------------------------------------------------------


void __fastcall TGranTabledFrm::PageControl1Changing(TObject *Sender,
      bool &AllowChange)
{
 AllowChange = !create_mode;

}
//---------------------------------------------------------------------------


void __fastcall TGranTabledFrm::IoNotResetClick(TObject *Sender)
{
  if(IoNotReset->Checked!=bool(settings.io_not_reset))
    {
     config_changes|= CCFL_IONOTRESET;
     IoNotReset->Font->Color = clBlue;
    }
    else
    {
     config_changes&= ~CCFL_IONOTRESET;
     IoNotReset->Font->Color = clDefault;
    }

}
//---------------------------------------------------------------------------

