//---------------------------------------------------------------------------
#pragma warn -8058
#include <vcl.h>
#pragma warn .8058
#pragma hdrstop

#include "gkipmodem_frm.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "gkmodule_form"
#pragma link "about_common_frame"
#pragma resource "*.dfm"
TGkIPModemFrm *GkIPModemFrm;
//---------------------------------------------------------------------------
__fastcall TGkIPModemFrm::TGkIPModemFrm(TComponent* Owner,HWND owner_hwnd,GKHANDLE handle )
        : TGKModuleForm(Owner,owner_hwnd,handle )
{
 in_setup = 0;
 #ifdef COMERCIAL_BUILD
 //Label30->Caption = "ООО \"Компания промышленной автоматизации\"";
 #endif

}
//---------------------------------------------------------------------------

void __fastcall  TGkIPModemFrm::set_ch_mask(DWORD chm)
{
 if(Fch_mask^chm)
 {
  Fch_mask = chm;
 }
 ApplyBtn->Enabled = Fch_mask ? true:false;
 UndoBtn-> Enabled = Fch_mask ? true:false;
}

void     __fastcall TGkIPModemFrm::after_set_gkhandle()
{
 TGKModuleForm::after_set_gkhandle();
 on_module_state(mod_iface(),mod_iface.get_module_state());
 in_setup++;
 if(get_module_settings())
    setup_settings_controls();
 ClientList->Clear();
 setup_lines_list();
 ch_mask = 0;
 set_notify(mod_iface(),MNF_COMMON|MNF_MODEM);
 PageControl1Resize(this);
  in_setup--;
}

LRESULT __fastcall TGkIPModemFrm::on_gkhandle_notify (GKHANDLE from,LPNOTIFY_CODE nc,DWORD mask)
{
 if(mask&MNF_MODEM)
 {
  switch(nc->notify_code)
  {
   case MNCODE_MODEM_NUMBER: on_modem_number((LPMODEM_CHANGE_NUMBER)nc->notify_data);break;
   case MNCODE_LINE_STATE  : on_line_state((LPMODEM_LINE_STATE)nc->notify_data);break;
  }
 }
 return TGKModuleForm::on_gkhandle_notify(from,nc,mask);
}

void __fastcall  TGkIPModemFrm::on_modem_number(LPMODEM_CHANGE_NUMBER mcn)
{
 settings.modem_number = mcn->new_number;
 ModemNumberChange(ModemNumber);
}

void __fastcall  TGkIPModemFrm::on_line_state  (LPMODEM_LINE_STATE mls)
{
  if(!(mls->line_state&MODEM_LINE_STATE_EXIST) )
  {
	TListItem * item;
	item  = find_line(LinesList,mls->addr.line);
	if(item)
	   item->Delete();
	item  = find_line(ClientList,mls->addr.line);
	if(item)
	   item->Delete();
  }
  else
  {
   GKIPM_LINE_ADDRS la;
   if(this->get_line_addr(mls->addr.line,la))
      add_line(mls->addr.line,la);
  }
}


void     __fastcall TGkIPModemFrm::on_module_state(GKHANDLE mod,DWORD state)
{
  bool running = ((state & MODULE_STATE_RUNNING_MASK )== MODULE_STATE_RUNNING) ? true:false;
  bool stopped  = (state & MODULE_STATE_RUNNING_MASK ) ? false:true;
  StopBtn->Enabled  = running;
  StartBtn->Enabled = stopped;
  if(running)
     StatusBar1->Panels->Items[0]->Text = "Модем работает";
  if(stopped)
     StatusBar1->Panels->Items[0]->Text = "Модем остановлен";
}

void     __fastcall TGkIPModemFrm::on_module_config_change(GKHANDLE mod,LPMODULE_CONFIG_DATA mcd)
{
 LPGKIPM_SETTINGS     ipms = (LPGKIPM_SETTINGS)mcd->data;
 if(mcd->mask&CCFL_IPM_FLAGS)
   settings.flags = ipms->flags;
 if(mcd->mask&CCFL_IPM_PORT)
   settings.port = ipms->port;
 if(mcd->mask& CCFL_IPM_BINDPARAMS)
 {
  memcpy(settings.bind_addrs,ipms->bind_addrs,sizeof(settings.bind_addrs));
  settings.bind_count = ipms->bind_count;
 }
 if(mcd->mask&CCFL_IPM_SERVERADDR)
   {
    strcpy(settings.server_addr,ipms->server_addr);
   }
 setup_settings_controls();
 ch_mask&=~mcd->mask;
}

#define MCD_SIZE ( sizeof(MODULE_CONFIG_DATA)+ sizeof(GKIPM_SETTINGS)-sizeof(BYTE))

bool __fastcall  TGkIPModemFrm::get_module_settings()
{
 BYTE buf[MCD_SIZE];
 LPMODULE_CONFIG_DATA mcd  = (LPMODULE_CONFIG_DATA)buf;
 mcd->dw_sz = MCD_SIZE;
 LPGKIPM_SETTINGS     ipms = (LPGKIPM_SETTINGS)mcd->data;
 ipms->dw_size = sizeof(*ipms);
 mcd->mask = -1;
 if(mod_iface.get_module_config_data(mcd,MCD_SIZE))
 {
  memcpy(&settings,ipms,sizeof(settings));
  return true;
 }
 return false;
}

void __fastcall  TGkIPModemFrm::setup_settings_controls()
{
 if(settings.modem_number>=1  && settings.modem_number<=250)
   ModemNumber->Text   = settings.modem_number;
   else
   ModemNumber->Text = AnsiString();
 ModemNumber->Font->Color = clDefault;  
   
 PortNumber->Text    = settings.port;
 PortNumber->Font->Color = clDefault;

 ServerAddr->Text    = "";//settings.server_addr;
 ServerAddr->Font->Color = clDefault;
 if(settings.flags&IPM_FLAG_ASCLIENT)
    AsClient->Checked = true;
    else
    {
      AsServer->Checked = true;
    }
    AllAddress->Checked = settings.bind_count == 0 ? true : false;
   OptCompatibleMode->Checked = settings.client_options & IPM_CLOPT_COMPATIBLE_MODE;
}

bool __fastcall  TGkIPModemFrm::IsBindingAddr(DWORD addr)
{
 bool ret = false;
 if(settings.bind_count)
 {
  for(DWORD i= 0;!ret && i<settings.bind_count;i++)
  {
    ret = settings.bind_addrs[i] == addr ? true:false;
  }
 }
 else
  ret = true;
 return ret; 
}

void __fastcall TGkIPModemFrm::setup_addr_list()
{
 AddressList->Clear();
 char name[MAX_PATH];
 gethostname(name,sizeof(name));


 AnsiString str("127.0.0.1");
 TIPAddress ipa(str.c_str(),0);
 TListItem * item = AddressList->Items->Add();
 item->Caption = str;
 ipa.sin_addr.S_un.S_addr;
 item->Data    = (LPVOID)ipa.sin_addr.S_un.S_addr;
 item->Checked = IsBindingAddr(ipa.sin_addr.S_un.S_addr);
 hostent * he = gethostbyname(name);
 if(he)
 {
  LPBYTE* addrs = (LPBYTE*)he->h_addr_list;
  while(*addrs)
  {
   str.printf("%d.%d.%d.%d",(DWORD)(*addrs)[0],(DWORD)(*addrs)[1],(DWORD)(*addrs)[2],(DWORD)(*addrs)[3]);
   item = AddressList->Items->Add();
   item->Caption = str;
   DWORD lAddr   = *((LPDWORD)*addrs);
   item->Data    = (LPVOID)lAddr;
   item->Checked = IsBindingAddr(lAddr);
   ++addrs;
  }

 }

}



void __fastcall TGkIPModemFrm::AllAddressClick(TObject *Sender)
{
  bool box_visible   = !AllAddress->Checked;
  if(box_visible)
     setup_addr_list();
     else
     AddressList->Clear();
     AddressBox->Visible = box_visible;
     ch_mask|=CCFL_IPM_BINDPARAMS;
}
//---------------------------------------------------------------------------
int _cdecl atoi( const wchar_t * text)
{
 return _wtoi(text);
}


void __fastcall TGkIPModemFrm::PortNumberChange(TObject *Sender)
{
  DWORD  pn = atoi(PortNumber->Text.c_str());
  TColor cl = clDefault;
  if(pn!=settings.port)
     {
      cl = clBlue;
      if(AsServer->Checked)
      ch_mask|=CCFL_IPM_PORT;
      else
      {
       bChangeLine->Enabled = true;
       bNewLine->Enabled = true;
      }
     }
     else
     {
      ch_mask&=~CCFL_IPM_PORT;
      bChangeLine->Enabled = false;
     }
     PortNumber->Font->Color = cl;
}
//---------------------------------------------------------------------------

void __fastcall TGkIPModemFrm::UndoBtnClick(TObject *Sender)
{
  //Отмена изменений
  this->setup_settings_controls();
  ch_mask = 0;
          
}
//---------------------------------------------------------------------------

void __fastcall TGkIPModemFrm::get_settings_from_control(GKIPM_SETTINGS & ipms)
{
 ipms.dw_size = sizeof(GKIPM_SETTINGS);
 ipms.modem_number = atoi(ModemNumber->Text.c_str());
 ipms.port         = atoi(PortNumber->Text.c_str());
 ipms.bind_count = 0;
 for(int i = 0;i<AddressList->Items->Count;i++)
 {
  TListItem * item = AddressList->Items->Item[i];
  if(item->Checked) ipms.bind_addrs[ipms.bind_count++] = (DWORD)item->Data;
 }
 if( AllAddress->Checked)
   ipms.bind_count = 0;

 strcpy(ipms.server_addr,AnsiString(ServerAddr->Text).c_str());

 ipms.flags&=~IPM_FLAG_ASCLIENT;
 if(AsClient->Checked)
    ipms.flags|=IPM_FLAG_ASCLIENT;
 ipms.client_options = 0;
 if(OptCompatibleMode->Checked)
    ipms.client_options |= IPM_CLOPT_COMPATIBLE_MODE;
}

void __fastcall TGkIPModemFrm::ApplyBtnClick(TObject *Sender)
{
 //Применение изменений
 BYTE buf[MCD_SIZE];
 ZeroMemory(buf,sizeof(buf));
 LPMODULE_CONFIG_DATA mcd  = (LPMODULE_CONFIG_DATA)buf;
 mcd->dw_sz = MCD_SIZE;
 LPGKIPM_SETTINGS     ipms = (LPGKIPM_SETTINGS)mcd->data;
 mcd->mask = ch_mask;
 ipms->dw_size = sizeof(*ipms);
 this->get_settings_from_control(*ipms);
 begin_command("Изменение параметров");
 bool ret = mod_iface.set_module_config_data(mcd,MCD_SIZE);
 end_command(ret ? GKH_RET_SUCCESS : GKH_RET_ERROR);
}
//---------------------------------------------------------------------------

void __fastcall TGkIPModemFrm::AsClientClick(TObject *Sender)
{
 AddressBox->Visible =  false;
 AllAddress->Visible =  false;
 ClientBox->Visible  =  true;
 bChangeLine->Visible = true;
 bNewLine->Visible    = true;
 bDelLine->Visible   =  true;
 Label3->Visible     =  true;
 ServerAddr->Visible =  true;
 ch_mask|=CCFL_IPM_FLAGS;
 ClOpts->Visible = true;
 //Constraints->MaxWidth = 586;
 //Width  = Constraints->MaxWidth;
 if(!in_setup)
 {
  //MessageBox(Handle,"Перезапустить модуль для обновления списка клиентов ?");
 }
}
//---------------------------------------------------------------------------

void __fastcall TGkIPModemFrm::AsServerClick(TObject *Sender)
{
 AddressBox->Visible  = !AllAddress->Checked;
 AllAddress->Visible  = true;
 Label3->Visible      = false;
 ServerAddr->Visible  = false;
 ClientBox->Visible   = false;
 bChangeLine->Visible = false;
 bNewLine->Visible    = false;
 bDelLine->Visible    = false;
 ch_mask|=CCFL_IPM_FLAGS;
 ClOpts->Visible = false;
 //Constraints->MaxWidth = Constraints->MinWidth;
 //Width  = Constraints->MaxWidth;

}
//---------------------------------------------------------------------------

void __fastcall TGkIPModemFrm::ServerAddrChange(TObject *Sender)
{
 TColor cl = clDefault;
 if(AnsiString(settings.server_addr).AnsiCompareIC(ServerAddr->Text))
 {
  cl = clBlue;
  if(this->AsServer->Checked)
  ch_mask|=CCFL_IPM_SERVERADDR;
  else
  {
   bChangeLine->Enabled = true;
   bNewLine->Enabled = true;
  }
 }
 else
 {
   ch_mask&=~CCFL_IPM_SERVERADDR;
   bChangeLine->Enabled = false;
 }

 ServerAddr->Font->Color = cl;
}
//---------------------------------------------------------------------------

void __fastcall TGkIPModemFrm::StartBtnClick(TObject *Sender)
{
  begin_command("Запуск модема");
  DWORD ret = mod_iface.start(MODULE_START_REQUEST,0);
  end_command(ret);
}
//---------------------------------------------------------------------------

void __fastcall TGkIPModemFrm::StopBtnClick(TObject *Sender)
{
  begin_command("Останов модема");
  DWORD ret = mod_iface.stop(MODULE_STOP_REQUEST);
  end_command(ret);

}
//---------------------------------------------------------------------------

TListItem *   __fastcall  TGkIPModemFrm::find_line(TListView * lst,DWORD num)
{
 TListItem * line =  lst->FindData(0,(LPVOID)(num),true,false);
 return line;
}




bool          __fastcall  TGkIPModemFrm::get_line_addr(DWORD num,GKIPM_LINE_ADDRS & la)
{
  la.dw_size     = sizeof(la);
  la.line_number = num;
  return mod_iface.call(CMIPM_GETLINE_ADDRS,(LPARAM)&la,0)==GKH_RET_SUCCESS ? true:false;
}

TListItem *   __fastcall  TGkIPModemFrm::add_line(DWORD num,GKIPM_LINE_ADDRS & la)
{
  TListItem * line = find_line(LinesList,num);
  AnsiString str;
  if(!line)
	{
	 line = LinesList->Items->Add();
	 line->Data    = (LPVOID)num;
	 line->Caption = num;
	 line->SubItems->Add(str);
	 line->SubItems->Add(str);
	}

   str.printf("%d.%d.%d.%d:%d",  (DWORD)la.local_addr.sin_addr.S_un.S_un_b.s_b1,
								 (DWORD)la.local_addr.sin_addr.S_un.S_un_b.s_b2,
								 (DWORD)la.local_addr.sin_addr.S_un.S_un_b.s_b3,
								 (DWORD)la.local_addr.sin_addr.S_un.S_un_b.s_b4,
								 (DWORD)htons(la.local_addr.sin_port) );
   line->SubItems->Strings[0] = str;//Локальный адрес
   str.printf("%s:%d",la.host_name,la.host_port);
   line->SubItems->Strings[1] = str;// адрес host'a

   if(settings.flags&IPM_FLAG_ASCLIENT)
	 {
	  line = find_line(ClientList,num);
	  if(!line)
		{
		 line = ClientList->Items->Add();
		 line->Data    = (LPVOID)num;
		 line->SubItems->Add(str);
		}
	  line->SubItems->Strings[0] = la.host_name;
	  line->Caption = la.host_port;
	 }
  return line;
}

void __fastcall  TGkIPModemFrm::setup_lines_list()
{
  ClientList->Items->Clear();
  DWORD idx    = 0;
  char buffer[MAX_PATH<<1];
  ZeroMemory(buffer,sizeof(buffer));
  LPMODEM_LINE_PROPS mlp = (LPMODEM_LINE_PROPS)buffer;
  mlp->dw_size      = sizeof(buffer);
  mlp->line_text_sz = (sizeof(buffer)-sizeof(*mlp))/sizeof(wchar_t);
  while(mod_iface.call(MODEMCM_ENUM_LINES,idx++,(LPARAM)mlp) == GKH_RET_SUCCESS)
  {
	GKIPM_LINE_ADDRS la;
	if(this->get_line_addr(mlp->addr.line,la))
	{
	 add_line(mlp->addr.line,la);
	}
  }

}


void __fastcall TGkIPModemFrm::PageControl1Resize(TObject *Sender)
{
  int wd = LinesList->Columns->Items[0]->Width + LinesList->Columns->Items[1]->Width; 
  LinesList->Columns->Items[2]->Width = LinesList->Width - wd - 8;
}
//---------------------------------------------------------------------------

void __fastcall TGkIPModemFrm::ModemNumberChange(TObject *Sender)
{
  DWORD mn = atoi(ModemNumber->Text.c_str());
  TColor cl = clDefault;
  if(settings.modem_number!=mn)
  {
   cl = clBlue;
   ch_mask|=CCFL_MODEM_NUMBER;
  }
  else
  ch_mask&=~CCFL_MODEM_NUMBER;
  ModemNumber->Font->Color = cl;
}
//---------------------------------------------------------------------------

void __fastcall  TGkIPModemFrm::begin_command(char * cmd)
{
  StatusBar1->Panels->Items[0]->Text = cmd;
  StatusBar1->Panels->Items[1]->Text = AnsiString();
}

void __fastcall  TGkIPModemFrm::end_command  (DWORD ret_code)
{
 wchar_t err_text[1024];
 *err_text = 0; AnsiString str;
 DWORD error = GetLastError();
 switch(ret_code)
 {
  case GKH_RET_ERROR    :
  mod_iface.get_error_text(error,err_text,sizeof(err_text)/sizeof(wchar_t));
  str =err_text;
  break;
  case GKH_RET_EXCEPTION:
  {
   except_record  er;
   mod_iface.get_except_record(&er);/*"командa вызвалa исключение %X"*/
   str.sprintf("Исключение %s",er.except_rec.ExceptionCode);
  }
  break;
  case GKH_RET_CANCEL :
  str ="Отмена"; //str.LoadString(ModuleInstance,IDS_ENDCMD_CANCEL);//"команда отмененa";
  break;
  default:
  str = "команда завершена успешно";//str.LoadString(ModuleInstance,IDS_ENDCMD_SUCCESS);//"выполненo успешно";
  break;
 }
 StatusBar1->Panels->Items[1]->Text = str;
}


void __fastcall TGkIPModemFrm::ClientListSelectItem(TObject *Sender, TListItem *Item,
          bool Selected)
{

  PortNumber->Text = Item  ? Item->Caption              : UnicodeString();
  ServerAddr->Text = Item  ? Item->SubItems->Strings[0] : UnicodeString();
  DWORD client_opts = 0;
  mod_iface.call(CMIPM_GET_LINE_OPTS,(DWORD)Item->Data,(LPARAM)&client_opts);
  setup_client_opts(client_opts);
}
//---------------------------------------------------------------------------

void __fastcall TGkIPModemFrm::setup_client_opts(DWORD opts)
{

   AnsiString s = ServerAddr->Text;
   strncpy(settings.server_addr,s.c_str(),sizeof(settings.server_addr));
   settings.port   = _wtoi(PortNumber->Text.c_str());
   OptCompatibleMode->Checked    =  (opts&IPM_CLOPT_COMPATIBLE_MODE          )? true : false;
   bChangeLine->Enabled = false;
   bNewLine->Enabled = false;

}

DWORD           __fastcall  TGkIPModemFrm::get_client_opts()
{
  DWORD opts = 0;
  if(OptCompatibleMode->Checked ) opts|=IPM_CLOPT_COMPATIBLE_MODE;
  return opts;
}

void __fastcall TGkIPModemFrm::bChangeLineClick(TObject *Sender)
{
 TListItem * item =  ClientList->Selected;
 if(item)
 {
  GKIPM_LINE_ADDRS la;
  la.dw_size = sizeof(la);
  la.line_number  = (DWORD)item->Data;
  AnsiString text = ServerAddr->Text;
  strncpy(la.host_name,text.c_str() ,sizeof(la.host_name));
  la.host_port    = atoi(PortNumber->Text.c_str());
  if(mod_iface.call(CMIPM_SETLINE_ADDRS,(LPARAM)&la,0))
      text = "Ошибка изменения параметров клиента";
      else
      {
       item->Caption = PortNumber->Text;
       item->SubItems->Strings[0] = ServerAddr->Text;
      }
 }
}
//---------------------------------------------------------------------------

void __fastcall TGkIPModemFrm::bDelLineClick(TObject *Sender)
{

 TListItem * item =  ClientList->Selected;
 if(item)
 {  if(ID_YES==MessageBoxW(Handle,L"Вы действительно желаете удалить линию?",Caption.c_str(),MB_YESNOCANCEL))
    {
     if(mod_iface.call(CMIPM_DELETE_LINE,(LPARAM)item->Data,0))
       {
        StatusBar1->Panels->Items[1]->Text = "Ошибка удаления линии";
       }
       else
       item->Delete();
    }
 }

}
//---------------------------------------------------------------------------

void __fastcall TGkIPModemFrm::bNewLineClick(TObject *Sender)
{

  GKIPM_LINE_ADDRS la;
  la.dw_size = sizeof(la);
  la.line_number  = (DWORD)-1;
  AnsiString text = ServerAddr->Text;
  strncpy(la.host_name,text.c_str() ,sizeof(la.host_name));
  la.host_port    = atoi(PortNumber->Text.c_str());
  if(la.host_port && *la.host_name)
  {
   if(mod_iface.call(CMIPM_CREATE_LINE,(LPARAM)&la,0))
      text = "Ошибка создания клиента";
      else
       {
        text.printf("Клиентская линия № %d  добавлена",la.line_number);
        bNewLine->Enabled = false;
       }

  }
  else
   text = "Задайте правильно параметры клиентской линии";
   StatusBar1->Panels->Items[1]->Text = text;


}
//---------------------------------------------------------------------------




void __fastcall TGkIPModemFrm::OptCompatibleModeClick(TObject *Sender)
{
  if(!in_setup)
     ch_mask|=CCFL_IPM_CLIENT_OPTS;

}
//---------------------------------------------------------------------------


