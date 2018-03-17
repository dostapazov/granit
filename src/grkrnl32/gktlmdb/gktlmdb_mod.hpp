/*-----------------------------------------------------------------------------
 Unit Name: gktlmdb
 Author   : Остапенко Д.В.
 Purpose  :
 History  :
-----------------------------------------------------------------------------*/

#ifndef   _GKTLMDB_HPP
#define   _GKTLMDB_HPP


#include <gkmodules.hpp>
#include <gktlmdb.h>
#include <modemmer.h>
#include "otd_dblow.hpp"
#include "orders.hpp"
#include <map>
#include <kedatas.hpp>
#include <otd_arch_proto.h>



using namespace KeRTL;
using namespace std;
typedef std::multimap<DWORD,DWORD>             mpu_map; // С каких ПУ   модем поставляет данные
typedef std::multimap<modem_addr,sotd_addr>    modem_otd_t; // С каких ПУ   модем поставляет данные
typedef std::map<sotd_addr  ,modem_addr>       otd_modem_t; // С каких ПУ   модем поставляет данные
typedef std::pair<modem_otd_t::iterator,modem_otd_t::iterator> modem_range_t;


class modem2pu
{
 protected:
 modem_otd_t  modem_otd;
 otd_modem_t  otd_modem;

 DWORD      pu_m     [OTD_ADDR_MAXVALUE];
 BOOL       conflict [OTD_ADDR_MAXVALUE];
 public   :
 modem2pu();
 //bool       __fastcall  set(DWORD pu_num,modem_addr & addr);
 //DWORD      __fastcall  get_modem(DWORD pu);
 //DWORD      __fastcall  get_pu   (modem_addr & ma,DWORD cnt);
 BOOL          __fastcall  get_conflict(DWORD pu);
 void          __fastcall  set_conflict(DWORD pu,BOOL conf);
 bool          __fastcall  set(modem_addr ma,otd_addr addr);
 void          __fastcall  erase(modem_addr ma);
 modem_addr    __fastcall  get_modem(otd_addr addr);
 otd_addr      __fastcall  get_otd  (modem_addr & ma);
 void          __fastcall  clear();
 modem_range_t __fastcall get_modem_range(modem_addr ma){return modem_otd.equal_range(ma);}
};



class TGKTlmDB;

class main_dblow:public otd_dblow
{
 protected:
 modem2pu    modem_pu;
 TGKTlmDB *  module;

 proto_pointer __fastcall create      (otd_addr & addr,otd_proto_param & src_opp);
 proto_pointer __fastcall create_group(otd_addr & addr);
 DWORD         __fastcall item_changed(proto_pointer  ptr,DWORD ch_parts,DWORD lo_obj,DWORD hi_obj);
 void          __fastcall report_tu_change(otd_addr & addr,DWORD obj,DWORD param);
 void          __fastcall report_conflict(DWORD pu, DWORD modem, DWORD conflict_modem);
 void          __fastcall report_tutr_sended(otd_addr & addr,DWORD obj , otd_tutr & tutr,modem_addr & src);

 public:
  main_dblow(){};
  DWORD  __fastcall handle(otd_proto & op, modem_addr & from,LPDWORD,LPDWORD );
  DWORD  __fastcall handle_ctrl          (otd_proto & op,modem_addr & from,LPDWORD,LPDWORD );
  void   __fastcall set_module(TGKTlmDB * _mod){module = _mod;};
  //DWORD  __fastcall get_punumber (modem_addr & ma);
  otd_addr  __fastcall get_otd_addr (modem_addr & ma);
  //DWORD  __fastcall get_modemaddr(DWORD pu){return modem_pu.get_modem(pu);}
  modem_addr  __fastcall get_modemaddr(otd_addr addr){return modem_pu.get_modem(addr);}
  DWORD  __fastcall modem_connect(modem_addr & ma,bool connect,LPMODEM_PROPS mp);
  DWORD  __fastcall get_index_range(otd_addr & addr,DWORD & lo,DWORD & hi);
  void   __fastcall handle_query (modem_addr & query,otd_addr &  query_mask,
                                      DWORD parts = OTD_PROTO_PART_ALL,
                                      DWORD lo = 0,DWORD hi = -1);
  TGKTlmDB * __fastcall get_module(){return module;}
  void       __fastcall send_tu  (DWORD addr,DWORD obj,BOOL on);
  void       __fastcall send_undo(DWORD addr,DWORD obj);
  DWORD      __fastcall remove_invalid_dynamic(DWORD addr);
  DWORD      __fastcall get_puname(DWORD pu,wchar_t * text,DWORD bsz);
  DWORD      __fastcall get_cpname(DWORD pu,DWORD cp,wchar_t * text,DWORD bsz);
  DWORD      __fastcall get_name  (otd_addr & addr,wchar_t * text,DWORD bsz);
  DWORD      __fastcall recreate  (otd_addr & addr,LPTLMDB_LOW_RECREATE rcs);
  DWORD      __fastcall make_static(otd_addr & addr,BOOL st);
  void       __fastcall on_start(){modem_pu.clear();}
  void       __fastcall on_stop (){modem_pu.clear();}
};

class TRespond:public TPooledThread
{
 protected:
 modem_addr   dest;//Адрес модема куда отправлять данные
 main_dblow * src; //База данных
 otd_addr     addr;
 DWORD        parts;

 bool           __fastcall begin_work(DWORD p,LPVOID arg1,LPVOID arg2);
 void           __fastcall do_work();
 DWORD          __fastcall OnException(DWORD ExceptionCode,LPEXCEPTION_POINTERS);
 public   :
 TRespond(TThreadsPool * _own):TPooledThread(_own),src(NULL){};
 static TPooledThread * WINAPI creator(TThreadsPool * own){ return new TRespond(own);};
};





class TGKTlmDB:public TGKModule
{
 protected:
 DWORD               rep_id;
 DWORD               turep_id;
 DWORD               arch_rep_id;
 TLMDB_SETTINGS      settings;
 TGKModuleInterface  modemmer;
 main_dblow          lowdb;
 TThreadsPool        responds;
 TOrders             orders;
 TOrders             archive_orders;
 //TOtdArchiever       otd_archiever;


 DWORD  __fastcall get_mem_used();
 WORD   __fastcall get_type(){return MT_TLMDB;}
 DWORD  __fastcall get_window_module (wchar_t * buf,DWORD bsz);
 void   __fastcall release(LPVOID);

 bool    __fastcall read_settings  ();
 bool    __fastcall write_settings ();
// void    __fastcall start_archives();
// void    __fastcall stop_archives ();
 DWORD   __fastcall start(DWORD reason,LPARAM p2);
 DWORD   __fastcall stop (DWORD reason);
 LRESULT __fastcall processing    (DWORD cmd,LPARAM p1,LPARAM p2);

 DWORD   __fastcall get_config_data(DWORD mask,LPVOID buf,DWORD bsz);
 bool    __fastcall set_config_data(DWORD mask,LPVOID buf,DWORD bsz);
 bool    __fastcall compare_config_data(DWORD mask,LPVOID buf,DWORD bsz,LPDWORD ch_mask,BOOL * restart);

 void     __fastcall oncm_settings (LPTLMDB_SETTINGS s,BOOL set);
 LRESULT  __fastcall handle_data   (LPMPROTO_HEADER mph,GKHANDLE src);
 void     __fastcall handle_orders (MODEM_ADDR & addr_form,DWORD fa,otd_proto & op,GKHANDLE src) ;
 LRESULT  __fastcall handle_otd    (LPMPROTO_HEADER mph,GKHANDLE src);
 LRESULT  __fastcall handle_otd_archive    (LPMPROTO_HEADER mph,GKHANDLE src);
 LRESULT  __fastcall on_notify_code(LPNOTIFY_CODE,DWORD mask);
 void     __fastcall on_modem_line_state(LPMODEM_LINE_STATE mls);
 void     __fastcall check_source_lines ();

 LRESULT  __fastcall lowdb_get_entry (DWORD addr ,LPTLMDB_LOW_GET_ENTRY);
 LRESULT  __fastcall lowdb_enum_entry(DWORD index,LPTLMDB_LOW_GET_ENTRY);
 DWORD    __fastcall on_exception(lpexcept_record er);
 void     __fastcall read_db();
 void     __fastcall write_db();
 void     __fastcall send_archieve_data(sotd_proto & sop);

 static   THeap heap;
 static   LONG  total_alloc;
 virtual  DWORD   __fastcall check_license(){return MOD_LICENSE_REGISTERED; }

 public:
 TGKTlmDB();
 ~TGKTlmDB();
 void     __fastcall refresh_addr  (sotd_addr & addr,BOOL order);
 void     __fastcall refresh_pu    (DWORD modem_addr,DWORD pu_num);

 LRESULT  __fastcall send(LPMPROTO_HEADER mph);
 void     __fastcall send_otd_changes(otd_addr & addr,DWORD parts,DWORD lo_obj,DWORD hi_obj);
 void     __fastcall db_report(DWORD evt_type,wchar_t * rep_text,LPVOID data = 0,DWORD dt_sz = 0);
 DWORD    __fastcall get_modem_line_text(modem_addr & ma,wchar_t * text,DWORD sz);
 DWORD    __fastcall get_modem_props(modem_addr & ma,LPMODEM_PROPS mp,DWORD sz);
 void     __fastcall report_dbtutr(DWORD type,wchar_t *  text);
 void     __fastcall config_change(DWORD cf){lock();cc_flags|=cf;unlock();}
 void     __fastcall archive_report(DWORD evt_type,wchar_t * rep_text);
 TOrders& __fastcall get_orders          ();
 TOrders& __fastcall get_archive_orders  ();
 static   void * __stdcall mem_alloc(size_t sz );
 static   void   __stdcall mem_free (void * ptr);
};

inline TOrders& __fastcall TGKTlmDB::get_orders()
{
  return orders;
}

inline TOrders& __fastcall TGKTlmDB::get_archive_orders()
{
  return this->archive_orders;
}


inline modem2pu::modem2pu()
{
 FillMemory(pu_m,sizeof(pu_m),-1);
 FillMemory(conflict,sizeof(conflict),0);
}

#endif

