#include <kethreads.hpp>
#include <algorithm>

#ifndef _NO_NAMESPACE
 namespace KeRTL {
#endif

	 using namespace std;

  DWORD     __fastcall TThreadsList::add(TThread * th)
  {
    DWORD ret;
    lock();
	std::vector<TThread*>::iterator b = list.begin(),ptr = list.end();
    ptr = lower_bound(b,ptr,th,thread_less());
    if(!list.size() || (*ptr)!=th)
       {
        ptr = list.insert(ptr,th);
       }
    ret = std::distance((b < list.end() ? b:list.begin()) ,ptr);
    unlock();
    return ret;
  }

  TThread * __fastcall TThreadsList::remove(DWORD idx)
  {
   TThread * ret = NULL;
   std::vector<TThread*>::iterator ptr = list.begin();
   lock();
   if(idx<list.size())
      {
       advance(ptr,idx);
       ret = (*ptr);
       list.erase(ptr);
      }
   unlock();
   return ret;
  }

  bool       __fastcall TThreadsList::remove(TThread * th)
  {
   bool ret =false;
   lock();
   if(list.size())
   {
    vector<TThread*>::iterator ptr = list.begin();
    ptr = lower_bound(ptr,list.end(),th,thread_less());
    if(*ptr == th)
      {
       list.erase(ptr);
       ret = true;
      }
   }
   unlock();
   return ret;
  }

  bool      __fastcall TThreadsList::terminate_all(bool wait ,DWORD wto)
  {
   TSynchroSet ss;
   DWORD beg_idx = 0;
   vector<TThread*>::iterator ptr,end;
   while(beg_idx<this->get_count())
   {
    lock();
    ptr = list.begin();
    end = list.end();
    advance(ptr,beg_idx);
    for(int i = 0;i<MAXIMUM_WAIT_OBJECTS && ptr<end;i++,beg_idx++)
      {
       (*ptr)->Terminate();
       if(wait)
         ss+=(**ptr)();
       ptr++;  
      }
    unlock();
    if(wait)
       {
        ss.Wait(wto == INFINITE ? wto:wto*ss.GetWaitingCount(),true);
        ss.Clear();
       }
   }
return true;
  }

  void      __fastcall std_del_thread(TThread * th)
  {
   if(th) delete th;
  }

  void      __fastcall TThreadsList::clear(bool del,pfn_thread_del _df )
  {
   if(del && !_df) _df = std_del_thread;
   if(del)
   {
   terminate_all(true,1000);
   TThread * th  ;
     while(NULL!=(th = this->remove(DWORD(0))))
     {
        th->Terminate();
        if(th->WaitFor(2000))
        {
        _df(th);
        }
        else
         TerminateThread((*th)(),-1);
      }
    lock();
    list.resize(0);
    unlock();

   }
   else
   {
    lock();
    list.erase(list.begin(),list.end());
    list.reserve(0);
    unlock();
   }
  }



#ifndef _NO_NAMESPACE
}
#endif

 
