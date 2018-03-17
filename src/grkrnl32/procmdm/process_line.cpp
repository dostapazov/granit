#include "processmdm.hpp"

 process_line::process_line(DWORD number):modem_line(number),term_event(true,false)
 {
  ZeroMemory(&settings,sizeof(settings));
  settings.dw_size = sizeof(settings);
  write_pipe = stdin_pipe = read_pipe = stdout_pipe = NULL;
  ZeroMemory(&proc_sa,sizeof(proc_sa));
  proc_sa.nLength = sizeof(proc_sa);
  proc_sa.bInheritHandle = TRUE;
  ZeroMemory(&sd,sizeof(sd));
  if(InitializeSecurityDescriptor(&sd,SECURITY_DESCRIPTOR_REVISION) &&
     SetSecurityDescriptorDacl(&sd,TRUE,(PACL)NULL,FALSE))
     proc_sa.lpSecurityDescriptor = &sd;

 }

 DWORD __fastcall process_line::get_line_text  (wchar_t * text,DWORD text_sz)
 {
  DWORD ret = 0;
  wchar_t name[MAX_PATH];
  wchar_t ext [MAX_PATH];
  *ext = *name = 0;
  _wsplitpath(settings.proc_name,0,0,name,ext);
  if(!*name)
     safe_strcpy(name,L"Модем не настроен");
  if(text && text_sz>=DWORD(2+lstrlenW(name)+lstrlenW(ext)) )
     ret =  wsprintfW(text,L"%s.%s",name,ext);



  return ret;
 }

 void  __fastcall process_line::on_disconnect()
 {
  send_queue.DropData();
  modem_line::on_disconnect();
 }

 bool  __fastcall process_line::do_start ()
 {
  bool   ret = true;
  Start(0);
  return ret;
 }

 bool  __fastcall process_line::do_stop  ()
 {
  bool   ret = true;
  TerminateAndWait(2000);
  return ret;
 }

 bool  __fastcall process_line::send     (LPMPROTO_HEADER mph,DWORD sz)
 {
  DWORD wr_bytes = MPROTO_SIZE(mph);
  if(((int)wr_bytes)>=0)
  {
   if(write_pipe)
    {
     if(wr_bytes)
     {
      send_queue.PutIntoQueue(mph,wr_bytes);
      update_modemmer_tx(wr_bytes,1);
     } 
    }
  }
  else
  wr_bytes = 0;
  return wr_bytes ? true:false;
 }

 void  __fastcall process_line::send_processing()
 {
   rx_buffer rxb;
   while(send_queue.QueueCount() && !CheckTerminateRequest())
   {
    DWORD wr_bytes = send_queue.TopDataSize();
    if((int)wr_bytes>0)
    {
    if(wr_bytes> rxb.get_free_size())
       rxb.set_size(wr_bytes);
    send_queue.GetFromQueue(rxb.begin(),rxb.get_free_size(),NULL);
    WriteFile(this->write_pipe,rxb.begin(),wr_bytes,&wr_bytes,NULL);
    if(wr_bytes)
       update_stats_tx(wr_bytes,1,wr_bytes ? 0:1);
       else
       {
        GetLastError();
        TerminateProcess(process(),-1);
        DebugBreak();
       }
   }
   else
   {
    send_queue.DropData();
    DebugBreak();
   }
  } 
 }


 void  __fastcall process_line::recv_processing()
 {
   BYTE buffer[2048];
   DWORD rd_bytes = 0;
   DWORD left = 0;
   DWORD total = 0;
   SetLastError(0);
   while(!CheckTerminateRequest() && PeekNamedPipe(this->read_pipe,buffer,sizeof(buffer),&rd_bytes,&total,&left) && rd_bytes)
   {
     ReadFile(read_pipe,buffer,rd_bytes,&rd_bytes,NULL);
     update_stats_rx(rd_bytes,0,0);
     mpb.add(buffer,rd_bytes);
     handle_recv();
   }
 }

 void __fastcall process_line::handle_recv()
 {
  BYTE buffer[4096];
  LPMPROTO_HEADER mph = (LPMPROTO_HEADER)buffer;
  otd_proto op;
  op.dw_size = sizeof(op);
  while(mpb.peek_mproto(buffer,sizeof(buffer),true))
  {
     if(mph->fa || mph->data_size)
        {
          queue_rxdata(mph->fa,(LPBYTE)mph->data,mph->data_size,false);
          update_stats_rx(0,1,0);
        }

  }
 }

 bool          __fastcall process_line::run_process()
 {
  ZeroMemory(&startup_info,sizeof(startup_info));
  startup_info.cb = sizeof(startup_info);
  startup_info.dwFlags     =  STARTF_USESTDHANDLES|STARTF_USESHOWWINDOW;
  startup_info.wShowWindow = (WORD)settings.cmd_show;
  startup_info.hStdError = DupHandle(term_event(),TRUE);
  CreatePipe(&stdin_pipe ,&write_pipe,&proc_sa,0);
  CreatePipe(&read_pipe  ,&stdout_pipe,&proc_sa,0);
  startup_info.hStdInput  = DupHandle(stdin_pipe,TRUE);
  startup_info.hStdOutput = DupHandle(stdout_pipe,TRUE);

  if(IS_VALID_HANDLE(write_pipe) && IS_VALID_HANDLE(stdout_pipe))
  {
  if(process.Create(settings.proc_name,settings.cmd_line,NULL,
                    settings.work_dir[0] ? settings.work_dir:NULL ,
                    CREATE_DEFAULT_ERROR_MODE,TRUE,&startup_info,NULL,NULL))
  {
   process.GetProcessInfo(&this->proccess_info);
   process.WaitStartup(2000);
   
   connect(true);
   return true;
  }
  }
  else
  GetLastError();
  return false;
 }

 void          __fastcall process_line::cleanup_process()
 {
   if(write_pipe)
      CloseHandle(write_pipe);
   if(stdin_pipe)
      CloseHandle(stdin_pipe);
   if(this->read_pipe)
      CloseHandle(read_pipe);
   if(this->stdout_pipe)
      CloseHandle(stdout_pipe);
   if(startup_info.hStdError)
      CloseHandle(startup_info.hStdError);
   if(startup_info.hStdInput)
      CloseHandle(startup_info.hStdInput);
   if(startup_info.hStdOutput)
      CloseHandle(startup_info.hStdOutput);
    write_pipe = stdin_pipe = read_pipe = stdout_pipe = NULL;
 }


 int   __fastcall process_line::Execute  ()
 {
  connect(true);
  TSynchroSet  ss;
  ss+=term_event();
  ss+=send_queue.GetEvent()();
  ss+=process();
  DWORD wr = -1;
  while(wr)
  {
   wr =  ss.Wait(10);
   switch(wr)
   {
    case 1   : send_processing();
               recv_processing();
               break;

    case 2   :
               connect(false);
               cleanup_process();
               if(!process.WaitForTerminate(1000));
                  TerminateProcess(proccess_info.hProcess,-1);
               ss-=process();
               process.Close();
               if(!settings.auto_restart || !run_process())//Автозапуск по завершению
                  wr = 0;
                  else
                  ss+=process();
               break;
    case WAIT_TIMEOUT  : recv_processing();break;
   }
  }
  connect(false);
  return 0;
 }

 bool  __fastcall process_line::BeforeExecute()
 {
  term_event.SetEvent(false);
  if(this->run_process())
    {
     return TGKThread::BeforeExecute();
    }
    else
    GetLastError();
    return false;
 }

 void  __fastcall process_line::BeforeTerminate()
 {
  cleanup_process();
  this->set_state(MODEM_LINE_STATE_WORKED,false);
  TGKThread::BeforeTerminate();
 }

 DWORD __fastcall process_line::OnException(DWORD ExceptionCode,LPEXCEPTION_POINTERS exp)
 {
   DWORD ret = TGKThread::OnException(ExceptionCode,exp);
   TerminateProcess(proccess_info.hProcess ,-1);
   connect(false);
   locker.FullUnlock();
   send_queue.Unlock();
   return ret;

 }




 
