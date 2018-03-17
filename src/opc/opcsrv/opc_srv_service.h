//---------------------------------------------------------------------------

#ifndef opc_srv_serviceH
#define opc_srv_serviceH
//---------------------------------------------------------------------------
#include <lightopc.hpp>
#include <KeThreads.hpp>
#include "opc_srv_config.h"

#include <KeDatas.hpp>
#include <rx_buffer.hpp>
#include <wsockdef.h>
#include <otd.h>
#include <otdm_proto.h>
#include <modem_proto.h>
#include <vector>
#include <map>

#include <algorithm>
#include <iterator>

//#include <boost/multi_index_container.hpp>

#define CACHE_SIZE 128



using namespace lightopc;
using namespace KeRTL;
using namespace std;

class Tmdb_records
{
  public :
  typedef  std::vector<mdb_record> Trecords;
  protected:
  TFASTMutex locker;
  Trecords records;
  public:
  Trecords::iterator __fastcall begin(){return records.begin();}
  Trecords::iterator __fastcall end  (){return records.end  ();}
  Tmdb_records(){ }
  void __fastcall clear(){lock(); records.clear();unlock();}
  Trecords::iterator __fastcall find_record(DWORD rec_id);
  Trecords::iterator __fastcall add_record (mdb_short_record & rec);
                void __fastcall remove(Trecords::iterator ptr){lock(); records.erase(ptr); if(!records.size()) {records.clear();records.resize(0);} unlock();}
  bool __fastcall lock  (){return locker.Lock  ();}
  bool __fastcall unlock(){return locker.Unlock();}
  int  __fastcall size() {return (int)records.size();}
};


class TGkOpcDriver : public TOpcDriver,public TThread
{
 protected:
 TFASTMutex config_locker;
 TOpcSrvConfig  config;
 int            cache_count;
 int            cache_size;

 public:
 TGkOpcDriver():TOpcDriver()
 {
  term_request = NULL;change_settings = NULL;socket_event = INVALID_HANDLE_VALUE;socket = NULL;send_queue = NULL;
  no_clients_timer = 0;
  cache       = NULL;
  cache_count = 0;
  cache_size  = 0;

  //cache_size  = 0; cache_count = 0;
 }

 #define SEND_BUFFSZ 4096
 TOpcSrvConfig::tag_name_mode __fastcall get_tag_name_mode();
                          int __fastcall get_no_clients_work_time();
                          int __fastcall get_use_console();
 void __fastcall check_use_console();
 bool __fastcall config_lock()  {return config_locker.Lock()  ;}
 bool __fastcall config_unlock(){return config_locker.Unlock();}

 int  __fastcall service_create   (int tag_count,unsigned Flags,int QueueMax,loMilliSec RefreshRate = 1000,loMilliSec RefreshRateMin = 100, char BranchSep = 0);
 int  __fastcall service_destroy  ();



 void __fastcall write_console(const char *text,bool time = false);
 void __fastcall readconfig() {config.read();}
 protected:

        void __fastcall subscribe(const loCaller *ca, int count, loTagPair *til);
        void __fastcall client_release    (loClient * cli);
 static void   convert_tags(const loCaller *ca,
                 unsigned count, const loTagPair taglist[],
                 VARIANT *values, WORD *qualities, HRESULT *errs,
                 HRESULT *master_err, HRESULT *master_qual,
                 const VARIANT src[], const VARTYPE vtypes[], LCID lcid);
        void   do_convert_tags(const loCaller *ca,
                 unsigned count, const loTagPair taglist[],
                 VARIANT *values, WORD *qualities, HRESULT *errs,
                 HRESULT *master_err, HRESULT *master_qual,
                 const VARIANT src[], const VARTYPE vtypes[], LCID lcid);
static loTrid read_tags(const loCaller *ca,
                unsigned count, loTagPair taglist[],
                VARIANT *values, WORD *qualities,
                FILETIME *stamps, HRESULT *errs,
                HRESULT *master_err, HRESULT *master_qual,
                const VARTYPE vtypes[], LCID lcid)
{
  return loDR_CACHED; /* perform actual reading from cache */
}

static  int write_tags(const loCaller *,
                       unsigned count, loTagPair taglist[], VARIANT values[],
                       HRESULT error[], HRESULT *master_err, LCID);
        int do_write_tags(const loCaller *,
                       unsigned count, loTagPair taglist[], VARIANT values[],
                       HRESULT error[], HRESULT *master_err, LCID);


   virtual loTrid       __fastcall cache_unlock(bool wait);
   virtual bool         __fastcall cache_lock(bool flush = false);
   virtual int          __fastcall trans_wait(loTrid id);
   virtual loTrid       __fastcall cache_update(int count,loTagValue * tv);
           loTagValue * __fastcall get_cache_pointer(loTagId tagId);


   KeRTL::TEvent   * term_request;
   KeRTL::TEvent   * change_settings;
   KeRTL::TEventSimpleDataQueue * send_queue;
   HANDLE            socket_event;
   TIPSocket       * socket;
   KeRTL::TSynchroSet ss;
   int               is_connected;

   modem_proto_buffer    mp_buff;
   DWORD send_pkt_num;
   DWORD recv_pkt_num;
   DWORD kpa_timer;

   Tmdb_records      records;
   KeRTL::TFASTMutex data_locker;
   typedef std::map<DWORD,loTagId> maptags;
   maptags mtags;

   int no_clients_timer;
   int no_data_timer;

 DWORD __fastcall OnException(DWORD ExceptionCode,LPEXCEPTION_POINTERS);
 bool __fastcall data_lock()  {return data_locker.Lock()  ;}
 bool __fastcall data_unlock(){return data_locker.Unlock();}

 bool __fastcall  BeforeExecute  ();
 void __fastcall  BeforeTerminate();
 int  __fastcall  Execute();

 void __fastcall  Terminate() { KeRTL::TThread::Terminate();term_request->SetEvent(true);}
 void __fastcall  on_change_config();
 void __fastcall  on_timer();
 void __fastcall  otd_connect   ();
 void __fastcall  otd_disconnect();

 void __fastcall handle_socket_events();
 void __fastcall handle_disconnect(DWORD err);
 void __fastcall handle_connect   (DWORD err);

 void __fastcall handle_recv      (DWORD err);

 loTagId __fastcall add_tag(LPMDB_RECORD sr,char * name);
 void __fastcall handle_recv_mproto     ( LPMPROTO_HEADER mph);
 void __fastcall handle_record(LPMDB_RECORD ks_rec,DWORD mask);
 void __fastcall handle_record_name     (LPOTDM_PROTO_RECORD_NAME rn);
 void __fastcall handle_records         ( LPMDB_RECORD ks_rec,LPMDB_RECORD ks_rec_end );

 void __fastcall handle_get_kadr        ( LPOTDM_PROTO_KADR pk,DWORD data_size);
 void __fastcall handle_get_kadr_entryes( LPOTDM_PROTO_KADR_ENTRYES ke);

 void __fastcall handle_get_kadr_records( LPOTDM_PROTO_KADR_RECORDS kadr_records);
 void __fastcall handle_changed_record  ( LPOTDM_PROTO_RECORDS recs,int data_size);

 void __fastcall set_all_quality(int quality);
 void __fastcall set_rec_quality(DWORD rec_id,int quality);

 void __fastcall do_send          ();

 void __fastcall send_otd_subscribe  (int kadr_id, bool order);
 void __fastcall send_otd_subscribes (bool subscr);
 void __fastcall send_record_query_name(DWORD rec_id,bool long_name);
 void __fastcall send_keep_alive ();
 bool __fastcall send            (LPMPROTO_HEADER mph,DWORD sz);


};

int run_as_service(GUID & guid);

#endif
