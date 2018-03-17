//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "GrIpxModFrame.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "gkmodule_form"
#pragma resource "*.dfm"
TGrIpxModuleWnd *GrIpxModuleWnd;
//---------------------------------------------------------------------------
__fastcall TGrIpxModuleWnd::TGrIpxModuleWnd(TComponent* Owner,HWND p_wnd,GKHANDLE handle)
        : TGKModuleForm(Owner,p_wnd,handle)
{
}
//---------------------------------------------------------------------------

void __fastcall TGrIpxModuleWnd::after_set_gkhandle()
{
 switch(PageControl1->TabIndex)
 {
  case 0 : set_settings();break;
  case 1 : /*scan_lines*/;break;
 }
}

void __fastcall TGrIpxModuleWnd::set_settings()
{
   GRNET_SETTINGS gs;
   gs.dw_size  = sizeof(gs);
   RadioButton3->Checked = true;
   GroupBox1->Visible= true;GroupBox1->Visible = false; GroupBox3->Visible = false;

   if(mod_iface.call(GRNETCM_SETTINGS,(LPARAM)&gs,FALSE)==GKH_RET_SUCCESS)
     {
         PageControl1->Pages[PageControl1->TabIndex]->Tag = TRUE;
         ServerName->Text   = gs.server_name;
         ServerSocket->Text = gs.server_socket;
         ClientName->Text   = gs.client_name;
         ClientSocket->Text = gs.client_socket;
         PUNumber->Position = gs.pu_number;
         RetryCount->Position = gs.tx_retry_count;
         RetryTimeoutEdit->Text = gs.tx_retry_delay;

      if(gs.work_mode == GRNET_WM_AS_SERVER)
         {RadioButton1->Checked = true;GroupBox1->Visible = true;GroupBox3->Visible = false;}
      if(gs.work_mode == GRNET_WM_AS_CLIENT)
        {
         RadioButton2->Checked = true;GroupBox1->Visible = true;GroupBox3->Visible = true;
        }

     }
   ApplyBtn->Enabled = false;
   UndoBtn->Enabled  = false;

}

void __fastcall TGrIpxModuleWnd::apply_settings()
{
    if(!RadioButton3->Checked)
    {
      GRNET_SETTINGS gs;
      ZeroMemory(&gs,sizeof(gs));
      gs.dw_size  = sizeof(gs);
      strncpy(gs.server_name  ,ServerName->Text.c_str(),sizeof(gs.server_name)-1);
      gs.server_socket =atoi(ServerSocket->Text.c_str());
      strncpy(gs.client_name,ClientName->Text.c_str(),sizeof(gs.client_name)-1);
      gs.client_socket = atoi(ClientSocket->Text.c_str());
      gs.pu_number     =PUNumber->Position     ;
      gs.tx_retry_count=KeRTL::MAX(RetryCount->Position,(short)1);
      gs.tx_retry_delay=KeRTL::MAX(atoi(RetryTimeoutEdit->Text.c_str()),100);
      gs.work_mode = GRNET_WM_UNDEFINED;
      if(RadioButton1->Checked)
        gs.work_mode = GRNET_WM_AS_SERVER;
      if(RadioButton2->Checked)
        gs.work_mode = GRNET_WM_AS_CLIENT;
      mod_iface.call(GRNETCM_SETTINGS,(LPARAM)&gs,TRUE);
      ApplyBtn->Enabled = false;
      UndoBtn->Enabled  = false;
    }
}          


void __fastcall TGrIpxModuleWnd::RadioButton1Click(TObject *Sender)
{
  GroupBox1->Visible =  RadioButton1->Checked || RadioButton2->Checked;
  GroupBox3->Visible = !RadioButton1->Checked;
  ApplyBtn-> Enabled = true;UndoBtn->Enabled = true;
}
//---------------------------------------------------------------------------


void __fastcall TGrIpxModuleWnd::UndoBtnClick(TObject *Sender)
{
  set_settings();      
}
//---------------------------------------------------------------------------

void __fastcall TGrIpxModuleWnd::ApplyBtnClick(TObject *Sender)
{
  apply_settings();      
}
//---------------------------------------------------------------------------

void __fastcall TGrIpxModuleWnd::RetryTimeoutEditChange(TObject *Sender)
{
  ApplyBtn->Enabled = true;
  UndoBtn->Enabled  = true;
}
//---------------------------------------------------------------------------


void __fastcall TGrIpxModuleWnd::PageControl1Change(TObject *Sender)
{
  if(PageControl1->TabIndex == 1)
    set_notify(mod_iface(),GRNET_NOTIFY_TXDATA|GRNET_NOTIFY_RXDATA );
    else
    reset_notify(mod_iface(),GRNET_NOTIFY_TXDATA|GRNET_NOTIFY_RXDATA);
}
//---------------------------------------------------------------------------

void __fastcall TGrIpxModuleWnd::FormShow(TObject *Sender)
{
  if(PageControl1->TabIndex == 1)
     set_notify(mod_iface(),GRNET_NOTIFY_TXDATA|GRNET_NOTIFY_RXDATA );
}
//---------------------------------------------------------------------------

void __fastcall TGrIpxModuleWnd::FormHide(TObject *Sender)
{
    if(PageControl1->TabIndex == 1)
     reset_notify(mod_iface(),GRNET_NOTIFY_TXDATA|GRNET_NOTIFY_RXDATA );
}
//---------------------------------------------------------------------------

LRESULT  __fastcall TGrIpxModuleWnd::on_gkhandle_notify (GKHANDLE from,LPNOTIFY_CODE nc,DWORD mask)
{
 /*Обработка кодов напоминания*/
 if(mask == GRNET_NOTIFY_RXDATA)
 {
   DWORD len =  nc->dw_size-sizeof(*nc)+sizeof(nc->notify_data);
   LPGRNET_KADR grnet_kadr = (LPGRNET_KADR)nc->notify_data;
   monitor_grnet_kadr (grnet_kadr,len,FALSE); 
 }

 return GKH_RET_SUCCESS;
}

void     __fastcall TGrIpxModuleWnd::monitor_grnet_kadr (LPGRNET_KADR kadr,DWORD len, BOOL tx)
{
 AnsiString str;
 str = tx ? "Передача : ":
            "Прием    : ";
 str.cat_printf(" %s № %d клиент ID %X net_cmd = %d", kadr->receipt ? "квитанции":"данных", kadr->kadr_num,kadr->id,kadr->command);
 
 monitor_string(str);
 switch(kadr->command)
 {
  case CmRetranslation:
       monitor_gratony_kadr((lp_gratony_kadr) kadr->data);
  break;
 }

}

void     __fastcall make_data_str(AnsiString & str,lp_gratony_kadr kadr)
{

  LPBYTE data = kadr->data;
  LPBYTE data_end = data+(kadr->len - sizeof(*kadr)+sizeof(kadr->data));
  while(data<data_end)
  {
   str.cat_printf(" %2F",(DWORD)*data++);
  }

}

void     __fastcall TGrIpxModuleWnd::monitor_gratony_kadr( lp_gratony_kadr  kadr)
{
 AnsiString str;
 AnsiString str1;
 str.printf("КП-%d ГРУППА-%d ",(DWORD)kadr->numCp,(DWORD)kadr->numGrp);
 switch((DWORD)kadr->InfoType)
 {
  case tpTs    : str+= " сигнализация";
                 make_data_str(str1,kadr);break;
  case tpTit   : str+= " измерения";
                 make_data_str(str1,kadr);break;
 }

 monitor_string(str);
 monitor_string(str1);
}

void     __fastcall TGrIpxModuleWnd::monitor_string(AnsiString & s)
{
  int ln_cnt =Memo1->Lines->Count;
  if(ln_cnt>=256)
  {
   Memo1->Lines->BeginUpdate();
    while(ln_cnt-->=256)
     Memo1->Lines->Delete(0);
   Memo1->Lines->EndUpdate();
  }
  Memo1->Lines->Add(s);
}
