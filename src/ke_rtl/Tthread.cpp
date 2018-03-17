#include <KeThreads.HPP>

#ifndef _NO_NAMESPACE
namespace KeRTL
{
#endif

  void  __fastcall TThread::Terminate()
  {
   InterlockedIncrement(&TerminateRequest);
   if(GetState() == tsSuspended)
      Resume();
  }

  bool  __fastcall TThread::CheckTerminateRequest()
  {
   #ifdef __BORLANDC__
	  return InterlockedCompareExchange(&TerminateRequest,0,0) ? true:false;
   #else
	  return InterlockedCompareExchange((LPVOID*)&TerminateRequest,0,0) ? true:false;
   #endif
  }

  bool  __fastcall TThread::TerminateAndWait(DWORD to,bool Term)
  {
   bool ret(State>=tsRunning && State<tsTerminating);
   if(ret)
   {
   Terminate();
   State = tsTerminating;
   if(WaitFor(to))
   {
    ret = true;
   }
   else
     if(Term)
       {
        HANDLE hTemp = Dup(true);
        BeforeTerminate();
        Handle = INVALID_HANDLE_VALUE;
        State = tsTerminated;
        ::TerminateThread(hTemp,-1);
        return false;
       }

   Handle = INVALID_HANDLE_VALUE;
   State = tsTerminated;
   }
   return ret;
  }

  bool  __fastcall TThread::WaitFor(DWORD time_out)
  {
   return (GetCurrentThreadId()!=Id && IsValidHandle(Handle) && WaitForSingleObjectEx(Handle,time_out,false)==WAIT_OBJECT_0) ? true:false;
  }

  DWORD WINAPI TThread::ThreadStarter(TThread* thread)
  {
    DWORD ret(0);
    thread->TerminateRequest = 0;
    thread->Id = GetCurrentThreadId();
    thread->State = tsRunning;
    __try{
    __try{
       if(thread->BeforeExecute())
          ret = (DWORD)thread->Execute();
          thread->BeforeTerminate();
          thread->State  = tsTerminated;
         }

  __except(thread->OnException(::GetExceptionCode(),GetExceptionInformation()))
  {
	  thread->BeforeTerminate();
	  if(thread->IsFlagsSet(KERTL_THREAD_ENABLERESTART))
	  {
           thread->State = tsTerminated;thread->Start();return -1;
          }

  }

       }
       __finally{
    if(thread->IsFlagsSet(KERTL_THREAD_SELFDESTRUCT))
    {
     //__try{
           delete thread;
       //   }
      //    __except(EXCEPTION_EXECUTE_HANDLER)
      //    {
      //     ExitThread(-1);
      //    }
      ret = -1;
    }


   }
   return ret;
  }


    bool  __fastcall TThread::BeforeExecute ()
    { return true;}

    void  __fastcall TThread::BeforeTerminate()
    {}

 bool  __fastcall TThread::PostMessage(UINT msg,WPARAM wp,LPARAM lp)
 {

   for(int i = 0;i<256;i++)
   {
    if(PostThreadMessage(Id,msg,wp,lp))
      return true;
      else
      Sleep(0,false);
   }   
   return false;
 }


DWORD  __fastcall TThread::OnException(DWORD ExceptionCode,LPEXCEPTION_POINTERS /*epp*/)
{
 //epp = epp;
 this->ExceptionCode = ExceptionCode;
 return EXCEPTION_EXECUTE_HANDLER;
}


#ifndef _NO_NAMESPACE
}
#endif


