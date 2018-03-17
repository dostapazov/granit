//---------------------------------------------------------------------------

#pragma warn -8058

#include <vcl.h>
#include <rx_buffer.hpp>
#include <memory>
#include <string>
#include <otd.h>
#include <modem_proto.h>
#include <otdm_proto.h>
#include <registry.hpp>

#pragma hdrstop

#include "opc_config_form.h"


//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TConfigForm *ConfigForm;
//---------------------------------------------------------------------------
__fastcall TConfigForm::TConfigForm(TComponent* Owner)
	: TForm(Owner)
{
 ProgID->Text   = OPC_PROG_ID;
 GuidText->Text = OPC_GUID;
 bUnregister->Enabled = is_srv_registered();
 bRegister->Enabled   = !bUnregister->Enabled;

 config_data.read();
 setup_controls();

}
//---------------------------------------------------------------------------
void __fastcall TConfigForm::setup_controls        ()
{
   OpcSrvName->Text             = config_data.opc_server_name;
   otd_srv_addr->Text           = config_data.otd_server_addr;
   otd_srv_port->Text           = config_data.otd_server_port;
   RefreshRate->Text            = config_data.RefreshRate;
   MinRefreshRate->Text         = config_data.RefreshRateMin;
   MaxQueueCount->Text          = config_data.MaxQueueCount;
   BranchSep->Text              = UnicodeString(config_data.BranchSep);
   TagsCount->Text              = config_data.TagsCount;
   NoClientsWorkTime->Text      = config_data.no_clients_terminate_delay ;
   TagNameMode->ItemIndex       = (int) config_data.tn_mode;
   UseConsole->Checked          = (bool)config_data.use_console;
}

void __fastcall TConfigForm::SpeedButton1Click(TObject *Sender)
{
/*
      if(ClSock->Active)
      {
       ClSock->Active  = false;
      }
      else

*/  {
   ClSock->Active  = false;
   ClSock->Address = otd_srv_addr->Text;
   ClSock->Port    = otd_srv_port->Text.ToInt();
   ClSock->Active  = true;
  }
}
//---------------------------------------------------------------------------
void __fastcall TConfigForm::ClSockConnect(TObject *Sender, TCustomWinSocket *Socket)

{
  Socket->Data = new modem_proto_buffer(4096);
  pkt_num = 0;
  send_request_enum_kadr(Socket);
  UnicodeString s;
  Socket->Addr.sin_addr.S_un.S_un_b.s_b1;
  s.printf(L"Подключен к %d.%d.%d.%d порт %d"
  ,int(Socket->Addr.sin_addr.S_un.S_un_b.s_b1)
  ,int(Socket->Addr.sin_addr.S_un.S_un_b.s_b2)
  ,int(Socket->Addr.sin_addr.S_un.S_un_b.s_b3)
  ,int(Socket->Addr.sin_addr.S_un.S_un_b.s_b4)
  ,int(Socket->Addr.sin_port)
  );
  StatusBar1->Panels->Items[0]->Text = s;
}
//---------------------------------------------------------------------------

void __fastcall TConfigForm::send_request_enum_kadr(TCustomWinSocket * Socket)
{
    char tmp[4096];
    memset(tmp,0,sizeof(tmp));
    LPMPROTO_HEADER mph = (LPMPROTO_HEADER)tmp;
    mph->fa       = FA_OTD_MEDIUM;
    mph->internal = MP_INTERNAL_DEFAULT;
    mph->pkt_num = ++pkt_num;
    LPOTDM_PROTO_HEADER omph = (LPOTDM_PROTO_HEADER)mph->data;
    omph->command = OTDMPROTO_CMD_ENUM_KADRS;
    omph->data_size = sizeof(DWORD);
    omph->data[0] = -1;
    mph->data_size = sizeof(*omph);

    Socket->SendBuf(mph,MPROTO_SIZE(mph));
}

void __fastcall TConfigForm::ClSockDisconnect(TObject *Sender, TCustomWinSocket *Socket)

{
 AvailList->Clear();
 if(Socket->Data)
    delete reinterpret_cast<modem_proto_buffer *>(Socket->Data);
  StatusBar1->Panels->Items[0]->Text = L"Нет соединения с сервером";
}
//---------------------------------------------------------------------------
void __fastcall TConfigForm::ClSockRead(TObject *Sender, TCustomWinSocket *Socket)

{
   char tmp[4096];
   modem_proto_buffer * rxb = reinterpret_cast<modem_proto_buffer *>(Socket->Data);
   if(rxb)
   {

    DWORD len;
    do
    {
     len = Socket->ReceiveLength();
     if(len)
     {
     if(len > rxb->get_free_size())
       rxb->grow(len-rxb->get_free_size());
       Socket->ReceiveBuf(rxb->begin()+rxb->get_data_len(),len);
       rxb->reserve(len);
     }
    }while(len);
    handle_receive(Socket);
   }
}
//---------------------------------------------------------------------------

void __fastcall TConfigForm::handle_receive        (TCustomWinSocket * Socket)
{
 modem_proto_buffer * rxb = reinterpret_cast<modem_proto_buffer *>(Socket->Data);

 if(rxb && rxb->get_data_len())
 {
  rxb->sync_internal();
  while(rxb->get_mproto_len())
  {
      LPMPROTO_HEADER      mph = (LPMPROTO_HEADER)rxb->begin();
      if(mph->fa == FA_OTD_MEDIUM)
      {
       LPOTDM_PROTO_HEADER omph = (LPOTDM_PROTO_HEADER)mph->data;
       if(omph->command&OTDM_RESPOND)
        {
           switch(omph->command&OTDM_COMMAND_MASK)
           {
             case OTDMPROTO_CMD_ENUM_KADRS:
                  handle_enum_kadrs(omph->data,omph->data_size);
             break;
           }
        }
     }
     rxb->move_mproto();
  }
 }
}

void __fastcall TConfigForm::handle_enum_kadrs(void * data,DWORD data_size)
{
 LPOTDM_PROTO_KADR kdr = (LPOTDM_PROTO_KADR)data;
 TListItem * li =  AvailList->Items->Add();
 li->Caption = kdr->kadr.kadr_id;
 wchar_t * short_name = kdr->names;
 int sn_len = wcslen(short_name);// Длина короткого имени
 wchar_t * long_name = short_name+sn_len+1;
 UnicodeString kadr_name;

 if(*short_name)
   kadr_name = short_name;
 if(*long_name)
   kadr_name.cat_printf(L"[%s]",long_name);
 if(kadr_name.IsEmpty())
    kadr_name.printf(L"Frame № %d",int(kdr->kadr.kadr_id));

 li->SubItems->Add(kadr_name);
 li->Checked  = std::binary_search(config_data.order_kadrs.begin(),config_data.order_kadrs.end(),int(kdr->kadr.kadr_id),std::less<int>());
}

void __fastcall TConfigForm::bGUIDGenClick(TObject *Sender)
{
   _GUID guid;
   if(S_OK == CreateGUID(guid))
   {
     GuidText->Text = GUIDToString(guid);
     TGUID guid1    = StringToGUID(GuidText->Text);
     IsEqualGUID(guid,guid1);
   }

}
//---------------------------------------------------------------------------






void __fastcall TConfigForm::bRegisterClick(TObject *Sender)
{
  GUID _guid   = StringToGUID(this->GuidText->Text);
  AnsiString prog_id  = ProgID->Text;
  AnsiString srv_name = OpcSrvName->Text;

  AnsiString exe_name = Application->ExeName;
  exe_name.cat_printf(" %s",AnsiString(CMD_RUN_OPC,int(KERTL_ARRAY_COUNT(CMD_RUN_OPC))).c_str());
  loServerRegister(&_guid,prog_id.c_str(),srv_name.c_str(),exe_name.c_str(),NULL);
}
//---------------------------------------------------------------------------



void __fastcall TConfigForm::bUnregisterClick(TObject *Sender)
{
  GUID _guid   = StringToGUID(this->GuidText->Text);
  AnsiString prog_id  = ProgID->Text;
  loServerUnregister(&_guid,prog_id.c_str());
}
//---------------------------------------------------------------------------


bool __fastcall TConfigForm::is_srv_registered ()
{
  std::auto_ptr<TRegistry> reg(new TRegistry(KEY_READ));
  reg->RootKey = HKEY_CLASSES_ROOT;
  AnsiString key_text;
  key_text.printf ("AppID\\%s",OPC_GUID);
  bool res = reg->KeyExists(key_text);
  return res;
}




void __fastcall TConfigForm::FormClose(TObject *Sender, TCloseAction &Action)
{
  ClSock->Close();
}
//---------------------------------------------------------------------------

void __fastcall TConfigForm::bWriteConfigClick(TObject *Sender)
{
    strcpy(config_data.otd_server_addr,AnsiString(otd_srv_addr->Text).c_str());
    config_data.otd_server_port = otd_srv_port->Text.ToInt();

    strcpy(config_data.opc_server_name,AnsiString(OpcSrvName->Text).c_str());

    config_data.RefreshRate    = this->RefreshRate->Text.ToInt();
    config_data.RefreshRateMin = this->MinRefreshRate->Text.ToInt();
    config_data.MaxQueueCount  = this->MaxQueueCount->Text.ToInt();
    config_data.TagsCount      = this->TagsCount->Text.ToInt();

    AnsiString _br_sep(this->BranchSep->Text.c_str());
    config_data.BranchSep[0]   = _br_sep.Length() ? *_br_sep.c_str() : 0;

    config_data.no_clients_terminate_delay = NoClientsWorkTime->Text.ToInt();
    config_data.tn_mode   = (TOpcSrvConfig::tag_name_mode) TagNameMode->ItemIndex;
    config_data.use_console = (int)UseConsole->Checked;

    if(AvailList->Items->Count)
    {
     config_data.order_kadrs.clear();
        for(int i = 0;i< this->AvailList->Items->Count;i++)
        {
          TListItem * li = AvailList->Items->Item[i];
          if(li->Checked)
             config_data.order_kadrs.push_back(li->Caption.ToInt());
        }

        std::sort(config_data.order_kadrs.begin(),config_data.order_kadrs.end(),std::less<int>());
    }

    if(config_data.write())
    {
       KeRTL::TEvent evt(EVENT_CHANGE_SETTINGS, EVENT_ALL_ACCESS );
       evt.PulseEvent();
    }
}
//---------------------------------------------------------------------------

CONFIG_DLL_EXPORT BOOL WINAPI show_config(GUID * guid)
{
  TConfigForm * cf = new TConfigForm(NULL);
  cf->ShowModal();
  delete cf;
  return false;
}


void __fastcall TConfigForm::ClSockError(TObject *Sender, TCustomWinSocket *Socket,
          TErrorEvent ErrorEvent, int &ErrorCode)
{
  AnsiString str;
  str.printf("Network error %d",ErrorCode);
  StatusBar1->Panels->Items[0]->Text = str;
  ErrorCode = 0;
}
//---------------------------------------------------------------------------

