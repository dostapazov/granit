//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "modbus_emul_form.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm3 *Form3;
//---------------------------------------------------------------------------
__fastcall TForm3::TForm3(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TForm3::StartBtnClick(TObject *Sender)
{
   SrvSocket->Port   = TcpPort->Text.ToInt();
   SrvSocket->Active = true;
   StartBtn->Enabled = false;
   StopBtn->Enabled  = true;
   this->setup_dataClick(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TForm3::StopBtnClick(TObject *Sender)
{
   SrvSocket->Close();
   SrvSocket->Active = false;
   StartBtn->Enabled = true;
   StopBtn->Enabled  = false;
}
//---------------------------------------------------------------------------

void __fastcall TForm3::on_destroy_socket(TObject * sender)
{
  TCustomWinSocket * s = dynamic_cast<TCustomWinSocket *>(sender);

  if(s )
  {
	if( s->Data)
	{
	 delete get_rx_buffer(s);
	 s->Data = NULL;
    }
	s->Close();
  }
}

rx_buffer *  __fastcall TForm3::get_rx_buffer(TCustomWinSocket * s)
{
  rx_buffer * ret = NULL;
  if(s && s->Data)
     ret = reinterpret_cast<rx_buffer*>(s->Data);
  return ret;
}


//---------------------------------------------------------------------------

void  __fastcall TForm3::handle_rx(TCustomWinSocket * sender,rx_buffer * rx_buf)
{
  LPMODBUS_TCP_FRAME mtf  = (LPMODBUS_TCP_FRAME) rx_buf->begin();
  if( rx_buf->get_data_len() >= sizeof(*mtf))
  {
   int len = modbus_get_tcp_length(mtf);
   len = len;
   AnsiString s;
   s.printf("To addr %d command %d framr no %d",int(mtf->modbus_frame.addr),int(mtf->modbus_frame.data.command),int(mtf->hdr.frame_number));
   Memo1->Lines->Add(s);
   if(int(mtf->modbus_frame.addr) == DevNum->Text.ToInt())
   {
	 switch(mtf->modbus_frame.data.command)
	 {
	   case MODBUS_READ_DISCRETE_OUTPUT:
	   case MODBUS_READ_DISCRETE_INPUT:
			s.printf("Read discrete \n\r");
			Memo1->Lines->Add(s);
			read_discrete(mtf,sender);
	   break;
	   case MODBUS_READ_HOLDING_REGISTERS :
	   case MODBUS_READ_INPUT_REGISTERS   :
			s.printf("Read analogs\n\r");
			Memo1->Lines->Add(s);
			read_input(mtf,sender);

	   break;

	 }
   }
   rx_buf->move_to_begin(0,rx_buf->get_data_len());
  }
}

void __fastcall TForm3::read_discrete(LPMODBUS_TCP_FRAME mtf,TCustomWinSocket * socket)
{
 AnsiString s;
 LPMODBUS_STD_REQUEST rq = (LPMODBUS_STD_REQUEST)mtf->modbus_frame.data.data;
 int req_from = modbus_word(rq->req_from.val) , req_count = modbus_word(rq->req_count.val);
 s.printf("request from %d count %d",req_from,req_count);
 Memo1->Lines->Add(s);
 if(req_from < ParamList->Items->Count)
 {
	 int top  = KeRTL::MIN(ParamList->Items->Count,req_from+req_count);
	 int cnt  = top-req_from;
	 BYTE buf [256] = {0};
	 LPMODBUS_TCP_FRAME send_frame = (LPMODBUS_TCP_FRAME)buf;
	 send_frame->hdr               = mtf->hdr;
	 int data_len = 0;
	 modbus_format_respond(&send_frame->modbus_frame,sizeof(buf)-sizeof(MODBUS_TCP_HEADER),mtf->modbus_frame.addr,mtf->modbus_frame.data.command,cnt,MODBUS_DISCRETE_BITS,&data_len);

	 for(int i = req_from,pn = 0;pn<cnt;i++,pn++ )
	 {
	   TListItem * li = ParamList->Items->Item[i];
	   modbus_set_discrete(&send_frame->modbus_frame.data,pn,li->Caption.ToInt() ? 1 : 0);
	 }
	 send_frame->hdr.frame_len = modbus_word(sizeof(send_frame->modbus_frame)+modbus_get_respond_length(&send_frame->modbus_frame));
	 int s_len = modbus_get_tcp_length(send_frame);
	 socket->SendBuf(buf,s_len);
 }

}

void __fastcall TForm3::read_input (LPMODBUS_TCP_FRAME mtf,TCustomWinSocket * socket)
{
 AnsiString s;
 LPMODBUS_STD_REQUEST rq = (LPMODBUS_STD_REQUEST)mtf->modbus_frame.data.data;
 int req_from = modbus_word(rq->req_from.val) , req_count = modbus_word(rq->req_count.val);
 s.printf("request from %d count %d",req_from,req_count);
 Memo1->Lines->Add(s);
 if(req_from < ParamList->Items->Count)
 {
	 int top  = KeRTL::MIN(ParamList->Items->Count,req_from+req_count);
	 int cnt  = KeRTL::MAX(top-req_from,0);
	 BYTE buf [256] = {0};
	 LPMODBUS_TCP_FRAME send_frame = (LPMODBUS_TCP_FRAME)buf;
	 send_frame->hdr               = mtf->hdr;
	 int data_len = 0;
	 modbus_format_respond(&send_frame->modbus_frame,sizeof(buf)-sizeof(MODBUS_TCP_HEADER),mtf->modbus_frame.addr,mtf->modbus_frame.data.command,cnt,MODBUS_ANALOG_BITS,&data_len);

	 for(int i = req_from,pn = 0;pn<cnt;i++,pn++ )
	 {
	   TListItem * li = ParamList->Items->Item[i];
	   modbus_set_analog(&send_frame->modbus_frame.data,pn,li->Caption.ToInt());
	 }
	 send_frame->hdr.frame_len = modbus_word(sizeof(send_frame->modbus_frame)+modbus_get_respond_length(&send_frame->modbus_frame));
	 int s_len = modbus_get_tcp_length(send_frame);
	 socket->SendBuf(buf,s_len);

 }
}




void __fastcall TForm3::setup_dataClick(TObject *Sender)
{
   int cnt = ParamCount->Text.ToInt();
   ParamList->Clear();
   for(int i = 0;i< cnt;i++)
   {
	 TListItem * li = ParamList->Items->Add();
	 int val = random(255);
	 li->Caption = setup_data->ItemIndex ? val : (val&1);
	 AnsiString s; s.printf("Параметр № %d",i);
	 li->SubItems->Add(s);
   }
}
//---------------------------------------------------------------------------

void __fastcall TForm3::SrvSocketClientRead(TObject *Sender, TCustomWinSocket *Socket)

{
 if(!Socket->Data)
	 {
	  Socket->Data =  new rx_buffer ;
	 }
 rx_buffer * rx_buf = get_rx_buffer(Socket);
 if(rx_buf)
 {
 BYTE buf [32000];
 int len;
 do{

	 len = Socket->ReceiveBuf(buf,sizeof(buf));
	 if(len>0)
	 {
	  rx_buf->add(buf,DWORD(len));
	  AnsiString s;
	  s.printf("Read from client %d bytes",len);
	  Memo1->Lines->Add(s);
	  handle_rx(Socket,rx_buf);
	 }

   }while(len>0);
 }

}
//---------------------------------------------------------------------------

void __fastcall TForm3::SrvSocketClientDisconnect(TObject *Sender, TCustomWinSocket *Socket)

{
   rx_buffer * rx_buf = this->get_rx_buffer(Socket);
   if(rx_buf)
      delete rx_buf;
  Socket->Data = NULL;
}
//---------------------------------------------------------------------------

