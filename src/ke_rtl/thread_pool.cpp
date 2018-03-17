#include <kethreads.hpp>
#include <algorithm>


#ifndef _NO_NAMESPACE
namespace KeRTL {
#endif

 void __fastcall TQueuedPoolData::release()
 {
  DWORD p;LPVOID a1,a2;
  while(this->pop_data(p,a1,a2))
      this->free_data(p,a1,a2);
 }

 void   __fastcall TPooledThread::set_term_event(HANDLE te)
 {
  if(term_event)
    CloseHandle(term_event);
  if(te)
     te = DupHandle(te);
  term_event = te;     
 }

 DWORD __fastcall TPooledThread::OnException(DWORD ExceptionCode,LPEXCEPTION_POINTERS ep)
 {
   if(owner)
   {
     owner->make_free(this);
   }
   return TThread::OnException(ExceptionCode,ep);
 }

 bool           __fastcall TPooledThread::need_terminate()/*Проверка необходимости завершения*/
 {
  bool ret = CheckTerminateRequest();
  if(!ret && term_event && WaitForSingleObject(term_event,0)==WAIT_OBJECT_0)
       {Terminate();ret = true;}
   return ret;
 }

 int            __fastcall TPooledThread::Execute()
 {
 if(owner)
     set_term_event(owner->get_term_event());
  while(!need_terminate())
  {
   do_work();
   done_work();
   if(owner)
      {
       if(owner->make_free(this) && !need_terminate())
           Suspend();
      }
      else
      break;
  }
  set_term_event(NULL);
  return 0;
 }


TPooledThread * __fastcall TThreadsPool::get_free_thread()
{
 TPooledThread * ret ;
 if(ready_threads.get_count())
     {
      ret = (TPooledThread*)ready_threads.get_front(true);
      if(ret && ret->GetState() == TThread::tsTerminated)
         {
          ret->Start(0,true,sa);
         } 
     }
     else
     {
     
     ret = (TPooledThread*)_free_threads.get_front(true);
     if(ret)
      {
       ret->TerminateAndWait(1000);
       ret->Start(0,true,sa);
       _free_threads.clear(true,pfn_del_thread);
       return ret;
      }
     }
  if(!ret && pfn_new_thread)
  {
    DWORD tc =  worked_threads.get_count()+ready_threads.get_count();
    for(DWORD i = 0;i<grow_size && tc<limit;i++,tc++)
    {
      ret = pfn_new_thread(this);
     if(ret)
     {
      ret->Start(0,true,sa);
      ready_threads.push_back(ret);
     }
     else
     return ret;
    }
    ret =  get_free_thread();
  }

 return ret;
}

TThreadsPool::start_result __fastcall TThreadsPool::start(DWORD p,LPVOID arg1,LPVOID arg2)
{
  start_result      sr = sr_fail;
  TPooledThread   * pt = get_free_thread();
  if(pt && pt->begin_work(p,arg1,arg2))
    {
     worked_threads.add(pt);
     pt->Resume();
     sr = sr_success;
    }
    else
    {
     if(queue)
        {
         queue->push_data(p,arg1,arg2);
         sr = sr_pending;
        }
    }
  return sr;
}

bool      __fastcall TThreadsPool::make_free(TPooledThread* pd)
{
 /*Нить завершила работу*/
 bool ret = true;
 if(!terminating)
 {
 if(queue)
 {
   DWORD p; LPVOID arg1,arg2;
   if(queue->pop_data(p,arg1,arg2))
     {
      ret = false;
      pd->begin_work(p,arg1,arg2);
     }
 }
 else
 {
  worked_threads.remove (pd);
  if(ready_threads.get_count()<this->grow_size)
     ready_threads.push_back(pd);
     else
     {
      pd->Terminate();/*Освобождаем лишние*/
      _free_threads.push_back(pd);
      ret = false;
     }
 }
 }
 return ret;
}

 bool            __fastcall TThreadsPool::terminate(DWORD wto)
 {
  bool ret;
  terminating = true;
  this->common_term_event.SetEvent(true);
  if(queue)
     queue->release();
            worked_threads.terminate_all(true,wto);
  ret =     ready_threads.terminate_all(true,wto);
  this->common_term_event.SetEvent(false);
  terminating = false;
  return ret;
 }

 void            __fastcall TThreadsPool::release()
 {
  terminate(INFINITE);
  ready_threads.clear (true,pfn_del_thread);
  worked_threads.clear(true,pfn_del_thread);
  _free_threads.clear (true,pfn_del_thread);
 }


#ifndef _NO_NAMESPACE
}
#endif
