
#ifndef __LIGHTOPC_CLASS__
#define __LIGHTOPC_CLASS__

#ifndef STRICT
 #define STRICT
#endif

#define _WIN32_DCOM
#include <windows.h>

#include <KeRTL.hpp>
//#include <dll_procs.hpp>


//#pragma pack(push,1)

#include <lightopc/lightopc.h>
#include <vector>
#include <functional>


namespace lightopc
{
 using namespace KeRTL;
 typedef void    (*release_handler)(void *, loService *, loClient *);
 typedef HRESULT (*qi_chain)       (void *rha, loService *, loClient *,const IID *, LPVOID *);

 struct loClientComparer
 {
  int compare(const loClient * const & c1,const loClient *const & c2)
  {return  c1 == c2 ? (int)0 : (c1<c2 ? 1:-1); }
 };

 struct loClientLess:public std::unary_function<loClient*,bool>
 {
  bool operator()(const loClient  * const &c1,const loClient  * const & c2)
  {
  return loClientComparer().compare(c1,c2)<0 ? true:false; }
 };

 class client_vector:protected std::vector<loClient*>
 {
   protected:
   TFASTMutex locker;
   public:
   client_vector () {}
   ~client_vector() {}
   iterator  __fastcall find_client(const loClient * cl);
   bool      __fastcall is_found   (const loClient * cl,iterator ptr);
   iterator  __fastcall add_client (loClient * cl);
   bool      __fastcall lock  () {return locker.Lock()  ;}
   bool      __fastcall unlock() {return locker.Unlock();}
   int       __fastcall get_size();
   loClient* __fastcall get_client(int idx);
   bool      __fastcall remove_client(loClient* cl);

 };

//Статические функции int_XXX являются интерфейсами для вызова виртуальных
 class TOpcDriver
 {
  protected:
   loDriver   lo_driver;
   loService  *service;
   int        tag_count;
   client_vector  clients;
   loTagValue *cache;

   static  void            int_client_release(void *, loService * svc, loClient * cli);
   virtual void __fastcall client_release    (loClient * cli);

   static  void int_subscribe(const loCaller *ca, int count, loTagPair *til);
   virtual void __fastcall subscribe(const loCaller *ca, int count, loTagPair *til){};

  public:
  TOpcDriver();
  virtual ~TOpcDriver(){}

  static inline TOpcDriver * get_driver(const loCaller * cd) { if(cd && cd->ca_se_arg) return reinterpret_cast<TOpcDriver *>(cd->ca_se_arg); else return NULL; };


   public:

           bool __fastcall is_service_valid(){return service ? true :false;}
   virtual int  __fastcall service_create   (int tag_count,unsigned Flags,int QueueMax,loMilliSec RefreshRate = 1000,loMilliSec RefreshRateMin = 100, char BranchSep = 0);
   virtual int  __fastcall service_destroy  ();
   virtual int  __fastcall create_client_agg(loClient **cli, IUnknown *outer, IUnknown **inner, int ldFlags, const loVendorInfo *vi = NULL);
   virtual int  __fastcall create_client    (loClient **cli, int ldFlags ,const loVendorInfo *vi = NULL);

   virtual int  __fastcall set_state        ( loClient *cli, int oper, int state, const loWchar *reason);
   virtual int  __fastcall set_state        ( loClient *cli, int oper, int state, const char *reason);

           bool __fastcall has_clients       ()  { return clients.get_size() ;}
   virtual void __fastcall clients_shutdown  (bool shutdown,char * reason);
           bool __fastcall wait_clients_shutdown(int timeout);


   virtual loVendorInfo const * const __fastcall get_vendor();
   virtual loTrid       __fastcall cache_unlock(bool wait);
   virtual bool         __fastcall cache_lock(bool flush = false);
   virtual int          __fastcall trans_wait(loTrid id){ return loTridWait(service,id);}
   virtual loTrid       __fastcall cache_update(int count,loTagValue * tv);

 };





}

#endif // __LIGHTOPC_CLASS__


