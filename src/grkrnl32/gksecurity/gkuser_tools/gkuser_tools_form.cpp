//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "gkuser_tools_form.h"
#include <KeRTL.hpp>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm2 *Form2;
//---------------------------------------------------------------------------
__fastcall TForm2::TForm2(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TForm2::conn_btnClick(TObject *Sender)
{
  if(!csocket->Active)
  {
   csocket->Host   = this->gkServer->Text;
   csocket->Port   = this->gkSrvPort->Text.ToInt() ;
   csocket->Active = true;
  }
  else
  {
    csocket->Close();
  }

}
//---------------------------------------------------------------------------
void __fastcall TForm2::csocketConnect(TObject *Sender, TCustomWinSocket *Socket)

{
    conn_btn->Down = true;
    conn_btn->AllowAllUp = true;
    conn_btn->Caption = L"Отключиться";
    grp2->Visible = true;
}
//---------------------------------------------------------------------------
void __fastcall TForm2::csocketDisconnect(TObject *Sender, TCustomWinSocket *Socket)

{
    conn_btn->Down = false;
    conn_btn->AllowAllUp = false;
    conn_btn->Caption = L"Подключиться";
    grp2->Visible = false;
    txt1->Caption = L"?*?";
}
//---------------------------------------------------------------------------



void __fastcall TForm2::login_btnClick(TObject *Sender)
{
  //Отправить команду подключения
  char buf[8192];
  LPMPROTO_HEADER mph      = (LPMPROTO_HEADER)buf;
  memset(mph,0,sizeof(*mph));

  mph->fa             = FA_SECURITY;
  mph->internal       = MP_INTERNAL_DEFAULT;

  local_ssid = KeRTL::GetTime(true);
  mph->data_size  = gksecur_login_format(mph->data,sizeof(buf)-sizeof(*mph),local_ssid,user_name->Text.c_str(),this->user_password->Text.c_str());
  wchar_t _usr[128],_psw[64];

  *((LPWORD)(((LPBYTE)mph->data)+mph->data_size)) = otd_calc_checksum(mph->data,mph->data_size,OTD_DEF_POLINOM);
  mph->data_size+=sizeof(WORD);
  mph->header_cs  = otd_calc_checksum(mph,sizeof(*mph)-sizeof(mph->header_cs)-sizeof(mph->data),OTD_DEF_POLINOM);
  csocket->Socket->SendBuf(buf,MPROTO_SIZE(mph));
}
//---------------------------------------------------------------------------


void __fastcall TForm2::csocketError(TObject *Sender, TCustomWinSocket *Socket, TErrorEvent ErrorEvent,
          int &ErrorCode)
{
   ErrorCode = 0;
}
//---------------------------------------------------------------------------

void __fastcall TForm2::csocketRead(TObject *Sender, TCustomWinSocket *Socket)
{
  DWORD len;
  do{
     len = Socket->ReceiveLength();
     if(len)
       {
        if(rxm.get_free_size()<len)
               rxm.grow(len);
        Socket->ReceiveBuf(rxm.end(),len);
        rxm.reserve(len);
       }
    }while(len);
    rxm.sync_internal(MP_INTERNAL_DEFAULT);


   do{
       len = rxm.get_mproto_len();
       if(len)
       {

        handle_recv((LPMPROTO_HEADER)rxm.begin(),len);
        rxm.move_mproto();
       }
     }while(len);

}
//---------------------------------------------------------------------------

void __fastcall TForm2::handle_recv(LPMPROTO_HEADER mph,int len)
{
    if(mph->fa == FA_SECURITY)
    {

     LPOTDM_PROTO_HEADER omph = (LPOTDM_PROTO_HEADER)mph->data;

     switch(omph->command&OTDM_COMMAND_MASK)
     {
      case GKSECUR_PROTO_CMD_LOGIN:
      if(omph->command&OTDM_RESPOND)
      {
       if(gksecur_login_extract(omph,&remote_ssid,NULL,0,NULL,0))
          txt1->Caption = remote_ssid;

      }
      break;
      default : break;
     }
    }

}

