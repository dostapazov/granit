//---------------------------------------------------------------------------

#include "test_mdm_frm.h"

#pragma hdrstop

#include <KeRTL.hpp>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TAppMdm *AppMdm;
//---------------------------------------------------------------------------
__fastcall TAppMdm::TAppMdm(TComponent* Owner)
        : TForm(Owner)
{
 call_parts = 0;
 CheckBox1->Checked = true;
 CheckBox2->Checked = true;
 CheckBox3->Checked = true;
 CheckBox4->Checked = true;
 CheckBox5->Checked = true;
 CheckBox8->Checked = true;
 CheckBox9->Checked = true;
 last_error = recv_kadrs =  send_kadrs =   recv_bytes = send_bytes =0;
}
//---------------------------------------------------------------------------
void  __fastcall TAppMdm::create_recvmon()
{
    KeWin::TRect r(0,0,MonitorBox->Width-10,MonitorBox->Height-14);
    r.Move(4,10);
    recv_mon.DoCreate(MonitorBox->Handle,r,-1,8192);
    recv_mon.SetFont(MonitorBox->Font->Handle,true);
}

void __fastcall TAppMdm::FormResize(TObject *Sender)
{
  recv_mon.SetSize(MonitorBox->Width-10,MonitorBox->Height-14);
}
//---------------------------------------------------------------------------

void __fastcall TAppMdm::FormShow(TObject *Sender)
{
 create_recvmon();
}
//---------------------------------------------------------------------------

void __fastcall TAppMdm::FormCreate(TObject *Sender)
{
  std_input        = GetStdHandle(STD_INPUT_HANDLE);
  std_output       = GetStdHandle(STD_OUTPUT_HANDLE);
  terminate_event  = GetStdHandle(STD_ERROR_HANDLE);
  AnsiString str;
  str.printf("StdInput - %08X, StdOutput - %08X, Event - %08X ",std_input,std_output,terminate_event);
  recv_mon.add_line(str.c_str());
  DWORD tid;
  recv_thread = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)recv_thread_func,this,0,&tid);
  Application->Title = Caption;
}
//---------------------------------------------------------------------------

void __fastcall TAppMdm::FormDestroy(TObject *Sender)
{
  if(IS_VALID_HANDLE(recv_thread))
    {
     Application->Terminate();
     WaitForSingleObject(recv_thread,500);
     CloseHandle(recv_thread);
    }
}
//---------------------------------------------------------------------------

bool __fastcall TAppMdm::check_for_terminate()
{
  if(!Application->Terminated && IS_VALID_HANDLE(terminate_event))
  {
      if(WaitForSingleObject(terminate_event,0) == WAIT_OBJECT_0 )  return  true;
      if(last_error == ERROR_BROKEN_PIPE || last_error == ERROR_BAD_PIPE)
        return true;
     return false;
  }
  return true;
}


void __fastcall TAppMdm::CheckBox1Click(TObject *Sender)
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

void __fastcall TAppMdm::Button1Click(TObject *Sender)
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
  mph->data_size = otd_proto_format_query((LPBYTE)mph->data,sizeof(buffer)-sizeof(*mph),&addr,0,_call_parts,NULL);
  send(buffer,MPROTO_SIZE(mph));

}
//---------------------------------------------------------------------------

void  __fastcall    TAppMdm::send(LPBYTE buff,DWORD len)
{
 DWORD wrb = 0;
 if(!WriteFile(std_output,buff,len,&wrb,NULL))
   {
    last_error = GetLastError();
    if(check_for_terminate())
       Application->Terminate();
   }
   else
   {
    ++send_kadrs, send_bytes+=len;
    update_send_stat();
   }
}

void __fastcall monitor_otd(KeWin::TTextViewer<char> * monitor, LPBYTE data, DWORD data_size);

void  __fastcall    TAppMdm::recv()
{
  BYTE   buffer[8192];
  DWORD  rdb  = 0;
  DWORD  avb  = 0;
  DWORD  msgb = 0;

  do{
     if(PeekNamedPipe(std_input,buffer,sizeof(buffer),&rdb,&avb,&msgb) && rdb)
      {
       if(ReadFile(std_input,buffer,rdb,&rdb,NULL))
       {
       rxbuff.add(buffer,rdb);
       recv_bytes+=rdb;
       rdb = 0;
       }
       else
       {last_error = GetLastError();break;}
      }
      else
      last_error = GetLastError();
    }while(rdb);


  if(rxbuff.get_data_len())
    {
     AnsiString str;
     LPMPROTO_HEADER mph  = (LPMPROTO_HEADER)buffer;
     while(rxbuff.peek_mproto(buffer,sizeof(buffer),true))
     {
      recv_kadrs++;
      if(!CheckBox6->Checked)
      {
       str.printf("Приём кадр №%d  модем %d линия %d FA %d размер данных %d",mph->pkt_num,(DWORD)mph->addr_from.modem,(DWORD)mph->addr_from.line,mph->fa,mph->data_size);
       recv_mon.add_line(str.c_str());
       if(mph->fa == FA_OTD)
        monitor_otd(&recv_mon,(LPBYTE)mph->data,mph->data_size);
      }  
     }
    }
 update_recv_stat();   
}

DWORD WINAPI TAppMdm::recv_thread_func(TAppMdm * frm)
{
   volatile BOOL need_terminate = FALSE;
   while(!need_terminate )
   {
     SendMessage(frm->Handle,MSG_RECV,0,(LPARAM)&need_terminate);
     Sleep(100);
   }
  return 0;
}




void __fastcall TAppMdm::OnMsgRecv(TMessage & msg)
{
        //TODO: Add your source code here
        LPBOOL term = 0;//(LPBOOL) msg.LParam;
        if(check_for_terminate())
        {
          Application->Terminate();
          if(term)    *term = TRUE;
        }  
         recv();
}

void __fastcall TAppMdm::Set_last_error(DWORD value)
{
        //TODO: Add your source code here
    if(Flast_error!=value)
    {
     Flast_error = value;
     if(value)
     {
      char err_text[MAX_PATH];
      int len = wsprintf(err_text,"Error %08X ",value);
      GetErrorMessage(value,err_text+len,sizeof(err_text)-len,(char*)0);
      this->recv_mon.add_line(err_text,clRed);
     }
    }
}
void __fastcall TAppMdm::CheckBox7Click(TObject *Sender)
{
  CallPeriod->Visible = CheckBox7->Checked;
  CallPeriodChange(this);
  Timer1->Enabled     = CheckBox7->Checked;
}
//---------------------------------------------------------------------------

void __fastcall TAppMdm::Timer1Timer(TObject *Sender)
{
 Button1Click(Button1);       
}
//---------------------------------------------------------------------------

void __fastcall TAppMdm::CallPeriodChange(TObject *Sender)
{
   Timer1->Interval = KeRTL::MAX(1000*atoi(CallPeriod->Text.c_str()),2000);      
}
//---------------------------------------------------------------------------


void __fastcall TAppMdm::Button3Click(TObject *Sender)
{
  recv_mon.clear();
}
//---------------------------------------------------------------------------
void    __fastcall TAppMdm::update_recv_stat()
{
 AnsiString str;
 str.printf("Принято байт %d  кадров %d", recv_bytes,recv_kadrs);
 StatusBar1->Panels->Items[0]->Text = str;
}
void    __fastcall TAppMdm::update_send_stat()
{
 AnsiString str;
 str.printf("Отправлено байт %d  кадров %d", send_bytes,send_kadrs);
 StatusBar1->Panels->Items[1]->Text = str;
}




