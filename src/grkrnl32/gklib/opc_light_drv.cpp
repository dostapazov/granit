#ifndef _WIN64

#include <lightopc.hpp>
#include <algorithm>

namespace lightopc
{

TOpcDriver::TOpcDriver()
{
 memset(&lo_driver,0,sizeof(lo_driver));
 service      = NULL;
 tag_count    = 0;

 lo_driver.ldDriverArg = this;
 cache        = NULL;
}

 loVendorInfo const * const __fastcall TOpcDriver::get_vendor()
 {
  static const loVendorInfo vendor = {
  1 /*Major */ , 1 /*Minor */ , 1 /*Build */ , 0 /*Reserv */ ,
  "OPC-DRIVER BASE CLASS (Ostapenko D.V. 2015) "
   };
   return & vendor;
 }


int  __fastcall   TOpcDriver::service_create(int _tag_count,unsigned Flags,int QueueMax,loMilliSec RefreshRate ,loMilliSec RefreshRateMin , char BranchSep )
{
 int res ;
 lo_driver.ldQueueMax         = QueueMax;
 lo_driver.ldRefreshRate      = RefreshRate;
 lo_driver.ldRefreshRate_min  = RefreshRateMin;
 lo_driver.ldBranchSep        = BranchSep;
 lo_driver.ldFlags            = Flags;
 tag_count = _tag_count;

 lo_driver.ldSubscribe = int_subscribe;
 cache = NULL;
  __try{
        res = loServiceCreate(&service,&lo_driver,tag_count);
       }
  __except (EXCEPTION_EXECUTE_HANDLER)
  {
   res =  -1  ;
  }
 return res;
}

int __fastcall    TOpcDriver::service_destroy( )
{
 int res = 0;
 if(this->service )
  {
   if(cache) cache_unlock(true);
   res = loServiceDestroy(service);
   service = NULL;
  }
 return res;
}

 void  TOpcDriver::int_subscribe(const loCaller *ca, int count, loTagPair *til)
 {
  TOpcDriver * drv = get_driver(ca );
  if(drv)
     drv->subscribe(ca,count,til);

 }


 void TOpcDriver::int_client_release(void * arg, loService * svc, loClient * cli)
 {
     TOpcDriver * drv = reinterpret_cast<TOpcDriver *>(arg);
     if(drv && drv->service == svc)
        drv->client_release(cli);
 }

 void __fastcall TOpcDriver::client_release    (loClient * cli)
 {
   if(cli)
      {
        clients.remove_client(cli);
      }
 }



 int  __fastcall   TOpcDriver::create_client_agg(loClient **cli,IUnknown *outer, IUnknown **inner,int ldFlags,const loVendorInfo *vi)
 {
   if(!vi) vi = get_vendor();
   int ret = loClientCreate_agg(this->service,cli,outer,inner,ldFlags,vi,&int_client_release, this );
   if(S_OK == ret)
        clients.add_client(*cli);

   return ret;
 }

 int  __fastcall   TOpcDriver::create_client(loClient **cli, int ldFlags, const loVendorInfo *vi)
 {
   if(!vi) vi = get_vendor();
   int ret = loClientCreate(service,cli,ldFlags,vi,int_client_release, this);
   if(S_OK == ret)
      clients.add_client(*cli);

   return ret;
 }

   int  __fastcall   TOpcDriver::set_state( loClient *cli, int oper, int state, const loWchar *reason)
   {
     return loSetStateW(service,cli,oper,state,reason);
   }

   int  __fastcall   TOpcDriver::set_state( loClient *cli, int oper, int state, const char *reason)
   {
    return loSetState(service,cli,oper,state,reason);
   }

   bool            __fastcall TOpcDriver::cache_lock(bool flush)
   {
    if(cache && flush)        cache_unlock(true);
    if(!cache   )       cache = loCacheLock(service);
    return cache ? true : false;
   }

   loTrid          __fastcall TOpcDriver::cache_update(int count,loTagValue * tv)
   {
    FILETIME ft;
    GetSystemTimeAsFileTime(&ft);
    return loCacheUpdate(service,count,tv,&ft);
    }

   loTrid       __fastcall TOpcDriver::cache_unlock(bool wait)
   {
    loTrid tid = 0;
    if(cache)
    {
    FILETIME ft;
    GetSystemTimeAsFileTime(&ft);
    tid = loCacheUnlock(service,&ft);
    if(wait)
      {
       trans_wait(tid);
       tid = -1;
      }
   }
    return tid;
  }

   void __fastcall TOpcDriver::clients_shutdown(bool shutdown,char * reason)
   {
       clients.lock();
       for(int i = 0;i<clients.get_size();i++)
             set_state(clients.get_client(i),shutdown ? loOP_SHUTDOWN : loOP_DISCONNECT,0,reason);
       clients.unlock();
   }

   bool __fastcall TOpcDriver::wait_clients_shutdown(int timeout)
   {

     while(timeout>0 && clients.get_size())
       {
        Sleep(100);
        timeout-=100;
       }
    return clients.get_size() ? false: true;
   }


   client_vector::iterator __fastcall client_vector::find_client(const loClient * cl)
   {
      KeRTL::TLockHelper l(locker);
      iterator ptr = std::lower_bound(begin(),end(),cl,loClientLess());
      return ptr;
   }

   bool     __fastcall client_vector::is_found   (const loClient * cl,client_vector::iterator ptr)
   {
     KeRTL::TLockHelper l(locker);
     return (ptr<end() && cl == *ptr) ? true : false;
   }

   client_vector::iterator __fastcall client_vector::add_client (loClient * cl)
   {
     KeRTL::TLockHelper l(locker);
     iterator ptr = find_client(cl);
     if(!is_found(cl,ptr))
        ptr = insert(ptr,cl);
      return ptr;
   }

   int       __fastcall client_vector::get_size()
   {
     TLockHelper l(locker);
     return size();
   }

   loClient* __fastcall client_vector::get_client(int idx)
   {
    TLockHelper l(locker);
     if(idx<(int)size())
       return  at(idx);

     return NULL_PTR;
   }

   bool      __fastcall client_vector::remove_client(loClient* cl)
   {
    TLockHelper l(locker);
    iterator ptr = find_client(cl);
    if(is_found(cl,ptr))
      {
       this->erase(ptr);
       return true;
      }
    return false;
   }



} // end of namespace

#endif

