//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop


#include "TIpClientForm.h"
#include <algorithm>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TIPClient *IPClient;
//---------------------------------------------------------------------------
__fastcall TIPClient::TIPClient(TComponent* Owner)
        : TForm(Owner)
{
 client = NULL;
}


void   __fastcall  TIPClient::update_rx_status()
{
 AnsiString str;
 if(client)
 {

  if(client->IsRunning())
  {
   switch(client->is_connected())
   {
    case CS_CONNECTING:
     connect_state->Caption = "Поиск сервера";
     if(!Visible)
        {
         Visible = true;
         SetFocus();
        }

     break;

    case CS_CONNECTED:
    connect_state->Caption = "соединен";
    RxBytes->Caption = client->get_rx_bytes_count();
    RxKadrs->Caption = client->get_rx_kadrs_count();
    TxBytes->Caption = client->get_tx_bytes_count();
    TxKadrs->Caption = client->get_tx_kadrs_count();
    break;

    default :
    connect_state->Caption = "Обрыв соединения";
     if(!Visible)
        {
         Visible = true;
         SetFocus();
        }
    break;
   }
  }
  else
  connect_state->Caption = "Остановлен";

 }
}

bool   __fastcall TIPClient::is_connected()
{
 return (client && client->IsRunning() && client->is_connected()) ? true:false;
}



void   __fastcall TIPClient::connect(bool act)
{
 if(act!= is_connected())
 {
   Timer1->Enabled = act;
 }
}

//---------------------------------------------------------------------------

void __fastcall TIPClient::Timer1Timer(TObject *Sender)
{
 update_rx_status();
}
//---------------------------------------------------------------------------

void __fastcall TIPClient::ServerAddrDropDown(TObject *Sender)
{
  //Поиск записей о серверах
    ServerAddr->Clear();
    WIN32_FIND_DATA find_data = {0};
    find_data.dwFileAttributes = FILE_ATTRIBUTE_DIRECTORY;
    AnsiString file_name;
    file_name.sprintf("%s*.*",TLMV_DATA_FOLDER);
    HANDLE hFind = FindFirstFile(file_name.c_str(),&find_data);
    if(hFind!=INVALID_HANDLE_VALUE)
    {
     do{
        if((find_data.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY) && *find_data.cFileName!='.')
        {
         std::replace(find_data.cFileName,find_data.cFileName+lstrlen(find_data.cFileName),'_','.');
         ServerAddr->Items->Add(find_data.cFileName);
        }
       }while(FindNextFile(hFind,&find_data));
      FindClose(hFind); 
    }
}
//---------------------------------------------------------------------------


