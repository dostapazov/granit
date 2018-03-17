//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ipmodem_wrk.h"


//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TIpModemSampleFrm *IpModemSampleFrm;

#define IPLINE_BUFFSZ  (16384<<1)
//---------------------------------------------------------------------------
__fastcall TIpModemSampleFrm::TIpModemSampleFrm(TComponent* Owner)
        : TForm(Owner)
{
 current_proto_id = FA_OTD;
 call_parts = 0;
 CheckBox1->Checked = true;
 CheckBox2->Checked = true;
 CheckBox3->Checked = true;
 CheckBox4->Checked = true;
 CheckBox5->Checked = true;
 CheckBox8->Checked = true;
 CheckBox9->Checked = true;
 PageControl1Change(this);

}
//---------------------------------------------------------------------------
void __fastcall TIpModemSampleFrm::CheckBox8Click(TObject *Sender)
{
 TCheckBox * cb = dynamic_cast<TCheckBox*>(Sender);
 if(cb)
 {
   if(cb->Checked)
     call_parts|=cb->Tag;
     else
     call_parts&=~cb->Tag;
 }

}
//---------------------------------------------------------------------------
void __fastcall TIpModemSampleFrm::StopBtnClick(TObject *Sender)
{
 StartBtn->Enabled = true;
 StopBtn->Enabled  = false;
 ClientSocket1->Active = false;
}
//---------------------------------------------------------------------------
void __fastcall TIpModemSampleFrm::StartBtnClick(TObject *Sender)
{
 rxbuff.move_to_begin(rxbuff.get_data_len());
 ClientSocket1->Host = ServerName->Text;
 ClientSocket1->Port = atoi(AnsiString(ServerPort->Text).c_str());
 StartBtn->Enabled = false;
 StopBtn->Enabled  = true;
 //ClientSocket1->Socket->ClientType = ctNonBlocking;
 ClientSocket1->Active = true;

}
//-------------------------------------------------------------------------
void  __fastcall TIpModemSampleFrm::create_recvmon()
{
    if(recv_mon() == NULL)
    {
     KeWin::TRect r(0,0,MonitorBox->Width-10,MonitorBox->Height-14);
     r.Move(4,10);
     recv_mon.DoCreate(MonitorBox->Handle,r,-1,8192);
     recv_mon.SetFont(MonitorBox->Font->Handle,true);
    }
}

void __fastcall TIpModemSampleFrm::FormShow(TObject *Sender)
{
 create_recvmon();
}
//---------------------------------------------------------------------------

void __fastcall TIpModemSampleFrm::FormResize(TObject *Sender)
{
 recv_mon.SetSize(MonitorBox->Width-10,MonitorBox->Height-14);
}
//---------------------------------------------------------------------------
void __fastcall TIpModemSampleFrm::Button3Click(TObject *Sender)
{
 recv_mon.clear();       
}
//---------------------------------------------------------------------------

void __fastcall TIpModemSampleFrm::update_status_line()
{
   AnsiString str;
   str.printf("Принято кадров %u байт %u",kadrs_count,bytes_count);
   StatusBar1->Panels->Items[0]->Text = str;

}

void __fastcall monitor_otd(KeWin::TTextViewer<char> * monitor, LPBYTE data, DWORD data_size);
void __fastcall monitor_otd_medium( KeWin::TTextViewer<char>* monitor, LPBYTE data, DWORD data_size);

void __fastcall TIpModemSampleFrm::handle_recv()
{

 TCHAR text[256];
 DWORD sync_offset = rxbuff.sync_internal(0xAAAA5555);
  if(sync_offset)
    {
     wsprintf(text,_T("Рассинхронизация на %d байт"), sync_offset);
     recv_mon.add_line(text);
    }

 while(rxbuff.get_mproto_len())
 {
  kadrs_count++;
  LPMPROTO_HEADER mph = (LPMPROTO_HEADER) rxbuff.begin();

  if(!CheckBox6->Checked)
  {
  sync_offset = rxbuff.sync_internal(0xAAAA5555);
  if(sync_offset)
    {
     wsprintf(text,_T("Рассинхронизация на %d байт"), sync_offset);
     recv_mon.add_line(text);
    }
    if(mproto_check(mph))
    {
        if(current_proto_id == (DWORD)mph->fa )
        {
            wsprintf(text,_T("Номер кадра %d длиной %d КС %04X"),mph->pkt_num,mph->data_size,(DWORD)mph->header_cs );
            recv_mon.add_line(text);
            if(mph->fa == FA_OTD)
               monitor_otd(&recv_mon,(LPBYTE)mph->data,mph->data_size);
            if(mph->fa == FA_OTD_MEDIUM)
               monitor_otd_medium(&recv_mon,(LPBYTE)mph->data,mph->data_size);
         }
    }
    else
    {
      recv_mon.add_line("Error mproto check");
    }
  }
   rxbuff.move_mproto();
  }
}

void  __fastcall TIpModemSampleFrm::do_send(LPMPROTO_HEADER mph)
{
  mph->pkt_num  = ++pkt_num;
  mph->internal = MP_INTERNAL_DEFAULT;
  mproto_protect(mph);
  ClientSocket1->Socket->SendBuf(mph,MPROTO_SIZE(mph));
  no_tx_timer = 0;
}

void __fastcall TIpModemSampleFrm::Button1Click(TObject *Sender)
{
  DWORD _call_parts = call_parts;
  BYTE buffer[MAX_PATH];
  sotd_addr addr(-1);
  str2addr(AddrMask->Text.c_str(),(LPBYTE)&addr,sizeof(addr),'.');
  addr.fa&=OTD_FA_ALL;
  if(Sender == Button1 )
    addr.fa|= OTD_FA_CALL;
  else
    addr.fa|= OTD_FA_ORDER;
  if(Sender == Button4 )
    _call_parts = 0;
  LPMPROTO_HEADER mph = (LPMPROTO_HEADER)buffer;
  ZeroMemory(mph,sizeof(*mph));
  mph->fa = FA_OTD;
  mph->internal = 0xAAAA5555;
  mph->data_size = otd_proto_format_query((LPBYTE)mph->data,sizeof(buffer)-sizeof(*mph),&addr,0,_call_parts,NULL);
  do_send(mph);


}
//---------------------------------------------------------------------------
void __fastcall TIpModemSampleFrm::CheckBox7Click(TObject *Sender)
{
  CallPeriod->Visible = CheckBox7->Checked;
  CallPeriodChange(this);
   
}//---------------------------------------------------------------------------
void __fastcall TIpModemSampleFrm::CallPeriodChange(TObject *Sender)
{
   AnsiString s(CallPeriod->Text);
   Timer1->Interval = KeRTL::MAX(1000*atoi(s.c_str()),2000);
}
//---------------------------------------------------------------------------

void __fastcall TIpModemSampleFrm::Timer1Timer(TObject *Sender)
{
 if(CheckBox7->Checked)
    Button1Click(Button1);
 ClientSocket1Read(ClientSocket1,ClientSocket1->Socket);
 if(no_tx_timer++>30)
    send_synchro();
 if(no_rx_timer++>100)
    StopBtnClick(this);
 update_status_line();
}
//---------------------------------------------------------------------------

void __fastcall TIpModemSampleFrm::send_synchro()
{
 MPROTO_HEADER mph;
 ZeroMemory(&mph,sizeof(mph));
 do_send(&mph);
}

void __fastcall TIpModemSampleFrm::ClientSocket1Read(TObject *Sender,
      TCustomWinSocket *Socket)
{
   int len;
   do{
   len = Socket->ReceiveLength();
   if(len)
   {
    no_rx_timer = 0;
    if(rxbuff.get_free_size()<(DWORD)len)
       if(!rxbuff.grow(len))
          DebugBreak();

    char * b = rxbuff.begin();
    b+= rxbuff.get_data_len();
    len = Socket->ReceiveBuf(b,len);

    bytes_count+=len;
    rxbuff.add(b,len);
   }
   }while(len);
   handle_recv();
   
}
//---------------------------------------------------------------------------
void __fastcall TIpModemSampleFrm::ClientSocket1Connect(TObject *Sender,
      TCustomWinSocket *Socket)
{

  if(Socket->Connected )
  {
   //DWORD bsz = IPLINE_BUFFSZ;
   //int opt_len = sizeof(DWORD);
   recv_mon.add_line("Соединение установлено");
   kadrs_count = 0;
   bytes_count = 0;
   pkt_num     = 0;
   update_status_line();
   rxbuff.move_to_begin(rxbuff.get_data_len());
   Button1->Enabled = true ;Button2->Enabled = true ;Button4->Enabled = true ;
   kadrs_count =  bytes_count = 0;
   if(CheckBox10->Checked)
     Button1Click(Button2);
   no_tx_timer = 0;
   no_rx_timer = 0;
   Timer1->Enabled = true;
  }



}
//---------------------------------------------------------------------------
void __fastcall TIpModemSampleFrm::ClientSocket1Disconnect(TObject *Sender,
      TCustomWinSocket *Socket)
{
  this->Timer1->Enabled = false;
  recv_mon.add_line("Соединение разорвано",clRed);
  rxbuff.move_to_begin(rxbuff.get_data_len());
  Button1->Enabled = false;Button2->Enabled = false;Button4->Enabled = false;
  StopBtnClick(this);

}
//---------------------------------------------------------------------------
void __fastcall TIpModemSampleFrm::Timer2Timer(TObject *Sender)
{
 ClientSocket1->Active = true; 
}
//---------------------------------------------------------------------------
void __fastcall TIpModemSampleFrm::ClientSocket1Error(TObject *Sender,
      TCustomWinSocket *Socket, TErrorEvent ErrorEvent, int &ErrorCode)
{
  AnsiString str;
  str.printf("Ошибка %d",ErrorCode); 
  recv_mon.add_line(str.c_str(),clRed);
  ErrorCode = 0;
}
//---------------------------------------------------------------------------
void __fastcall TIpModemSampleFrm::PageControl1Change(TObject *Sender)
{
 switch(PageControl1->ActivePageIndex)
  {
   case 0 :    current_proto_id = FA_OTD;break;
   case 1 :    current_proto_id = FA_OTD_MEDIUM ;break;
   case 2 :    current_proto_id = FA_OTD_ALARM_ARCHIVE;break;
  }
}
//---------------------------------------------------------------------------


void __fastcall TIpModemSampleFrm::Button5Click(TObject *Sender)
{
 BYTE buffer[MAX_PATH];
 LPMPROTO_HEADER mph =(LPMPROTO_HEADER)buffer;
 LPOTDM_PROTO_HEADER omph;
 omph = (LPOTDM_PROTO_HEADER)mph->data;
 ZeroMemory(mph,sizeof(*mph)+sizeof(*omph));
 mph->fa = FA_OTD_MEDIUM;
 mph->internal = 0xAAAA5555;
 omph->command = OTDMPROTO_CMD_ENUM_KADRS;
 omph->data_size = sizeof(DWORD);
 omph->data[0] = -1;
 mph->data_size = sizeof(*omph);
 do_send(mph);
}
//---------------------------------------------------------------------------

void __fastcall TIpModemSampleFrm::Button6Click(TObject *Sender)
{
 if(KadrID->Text.Length())
 {

  BYTE buffer[MAX_PATH];
  LPMPROTO_HEADER mph =(LPMPROTO_HEADER)buffer;
  LPOTDM_PROTO_HEADER omph;
  omph = (LPOTDM_PROTO_HEADER)mph->data;
  ZeroMemory(mph,sizeof(*mph)+sizeof(*omph));
  mph->fa = FA_OTD_MEDIUM;
  mph->internal = 0xAAAA5555;
  omph->command = OTDMPROTO_CMD_GETKADR_IMAGE;
  AnsiString s(KadrID->Text);
  omph->data[0] = atoi(s.c_str());
  mph->data_size = sizeof(*omph);
  do_send(mph);
 }
}
//---------------------------------------------------------------------------




void __fastcall TIpModemSampleFrm::Button7Click(TObject *Sender)
{

  BYTE buffer[MAX_PATH];
  LPMPROTO_HEADER mph =(LPMPROTO_HEADER)buffer;
  LPOTDM_PROTO_HEADER omph;
  omph = (LPOTDM_PROTO_HEADER)mph->data;
  ZeroMemory(mph,sizeof(*mph)+sizeof(*omph));
  mph->fa = FA_OTD_MEDIUM;
  mph->internal = 0xAAAA5555;
  omph->command = OTDMPROTO_CMD_GETKADR_ENTRYES;
  AnsiString s(KadrID->Text);
  omph->data[0] = atoi(s.c_str());
  mph->data_size = sizeof(*omph);
  do_send(mph);
}
//---------------------------------------------------------------------------

void __fastcall TIpModemSampleFrm::Button8Click(TObject *Sender)
{

  BYTE buffer[MAX_PATH];
  LPMPROTO_HEADER mph =(LPMPROTO_HEADER)buffer;
  LPOTDM_PROTO_HEADER omph;
  omph = (LPOTDM_PROTO_HEADER)mph->data;
  ZeroMemory(mph,sizeof(*mph)+sizeof(*omph));
  mph->fa = FA_OTD_MEDIUM;
  mph->internal = 0xAAAA5555;
  omph->command = OTDMPROTO_CMD_GETKADR_RECORDS;
  omph->data[0] = atoi(AnsiString(KadrID->Text).c_str());
  mph->data_size = sizeof(*omph);
  do_send(mph);
}
//---------------------------------------------------------------------------

void __fastcall TIpModemSampleFrm::Button9Click(TObject *Sender)
{

  BYTE buffer[MAX_PATH];
  LPMPROTO_HEADER mph =(LPMPROTO_HEADER)buffer;
  LPOTDM_PROTO_HEADER omph;
  omph = (LPOTDM_PROTO_HEADER)mph->data;
  ZeroMemory(mph,sizeof(*mph)+sizeof(*omph));
  mph->fa = FA_OTD_MEDIUM;
  mph->internal   = MP_INTERNAL_DEFAULT;
  omph->command   = OTDMPROTO_CMD_GETKADR|OTDM_ORDER;
  mph->data_size  = sizeof(*omph)+sizeof(DWORD);
  omph->data[0]   = atoi(AnsiString(KadrID->Text).c_str());//Номер кадра
  omph->data[1]   = Sender == Button9 ? -1 : 0;
  omph->data_size = sizeof(DWORD)<<1;
  do_send(mph);
}
//---------------------------------------------------------------------------


