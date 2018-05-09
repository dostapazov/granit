#ifndef _MEDIUMDB_INC_HPP
#include "mediumdb.h"
#include <gkmodules.hpp>
#include <modemmer.h>
#include <otd.h>
#include <record_table.hpp>
#include <text_table.hpp>
#include <kadrs_table.hpp>
#include <calcf.hpp>
#include <kedatas.hpp>
#include <rx_buffer.hpp>
#include <scale.h>
#include "orders_table.hpp"
#include "comb_table.hpp"
#include  <lua/lua_iface.hpp>



using namespace std;

typedef map<DWORD,DWORD> TChangesMap;/*ID -> маска изменений*/
typedef vector<DWORD>    record_idlist;
typedef record_idlist    sended_recs;//Список отправленных кадров
typedef vector<mdb_paint_variant> Tmdb_paint_variants;



struct tutr_record_active
{
   modem_addr ma;
   int       timeout;
   tutr_record_active():ma(-1),timeout(0){};
   tutr_record_active(const MODEM_ADDR & _ma,DWORD _to): ma(_ma),timeout(_to){};
   tutr_record_active(const tutr_record_active & _src):ma(-1),timeout(0){*this = _src;}
   tutr_record_active& operator = (const tutr_record_active & _src)
   {    ma = _src.ma;this->timeout = _src.timeout;   return *this; }
};

// Список выбранных ТУ/ТР
typedef std::map<DWORD,tutr_record_active>  tutr_actives_t;

#ifndef FASTMUTEX_LOCKER
#define FASTMUTEX_LOCKER 1
#endif



class TMediumDB;

class med_db
{
 protected:
 TMediumDB         * module;
 #if( FASTMUTEX_LOCKER != 0)
 TFASTMutex          locker;
 #else
 TMrSw               locker;
 #endif

 otd_addr            addr_mask; // для механизма добавления записей по маске
 int                 addr_mask_timeout;
 records_table       records;   //Список записей
 combine_table       combine;   //комбинированные объектов
 Tmdb_paint_variants paint_vars;//Список настройки вариантов отрисовки
 TCalculator      *  calculator;
 lua::LUAInstance    lua;
 BOOL                lua_prepared;

 //TFASTMutex       text_locker;
 text_table_t       txt_table; //Хранилище имён

 //TFASTMutex       kadrs_locker;
 kadrs_table      kadrs;     // кадров;
 orders_table     orders;    // Список заказов

 TChangesMap      record_changes;//Изменения в записях
 TChangesMap      kadr_changes;  // Маски изменения кадров
 tutr_actives_t   tutr_actives;

 void         __fastcall handle_kadr_changes();
 DWORD        __fastcall handle_record (mdb_record & rec,otd_proto & op,__int64 &time);
 //DWORD        __fastcall calc_record_kvants(mdb_record & rec,DWORD kvants,float & result);
 DWORD        __fastcall calc_record_kvants(mdb_record & rec,float kvants,float & result);
 float        __fastcall get_record_min_value(mdb_record & rec);
 float        __fastcall get_record_max_value(mdb_record & rec);
 DWORD        __fastcall update_minmax_value (mdb_record & rec);
 DWORD        __fastcall handle_sb     (otd_proto & op,bool second_pass = false);
 DWORD        __fastcall handle_group  (otd_proto & op);
 DWORD        __fastcall record_changed(mdb_record & rec,DWORD chmask,__int64 time = 0);
 DWORD        __fastcall record_changed(DWORD id,DWORD chmask,__int64 time = 0);
 void         __fastcall auto_create_owners(MDB_RECORD & rec);
 DWORD        __fastcall calc_combine(mdb_record &rec,__int64 time);
 DWORD        __fastcall calc_combine(mdb_record &rec ,float & value,DWORD opt);//OPT = 0 текущее значение; 1 min_value ; 2 - max_value
 DWORD        __fastcall calc_combine(DWORD id);
 void         __fastcall recalc_combined(DWORD id);
 bool         __fastcall _lua_script_exec(const std::string & formula,int count,long double * result,DWORD * ids,long double * args,LPWORD diags,LPDWORD error);
 void         __fastcall _lua_prepare();

 static  int   _lua_get_record_limits    (lua_State * l, int lim_type);
 static  int   _lua_get_record_min_danger(lua_State * l);
 static  int   _lua_get_record_max_danger(lua_State * l);
 static  int   _lua_get_record_min_alarm (lua_State * l);
 static  int   _lua_get_record_max_alarm (lua_State * l);

 void         __fastcall update_owner_diag(DWORD id,__int64 time);
 void         __fastcall update_owner_diag(mdb_record & rec,__int64 time);
 bool         __fastcall send_alarm_condition(mdb_record & rec);
 DWORD        __fastcall get_count_opts(DWORD opt_mask,trecords_index::iterator low,trecords_index::iterator hi);
 DWORD        __fastcall get_count_alarms(trecords_index::iterator low,trecords_index::iterator hi);

 DWORD        __fastcall record_change_addr   (mdb_record & rec,LPMDB_RECORD src_rec);

 public:
 med_db ():module(NULL),calculator(new TCalculator),addr_mask_timeout(0),lua_prepared(FALSE) {}
 ~med_db(){if(calculator) delete calculator;}

 DWORD        __fastcall get_record_changes_size(){return record_changes.size();}
 void         __fastcall handle_record_changes();

 void       __fastcall set_module           (TMediumDB * _module){lock();module = _module;unlock();}
 void       __fastcall config_change        (DWORD mask);
 void       __fastcall set_add_addr_mask    (otd_addr & addr){lock();addr_mask.addr = addr.addr;unlock();}
 DWORD      __fastcall calc_record          (mdb_record & rec,__int64 time);
 DWORD      __fastcall handle               (otd_proto & op);
 DWORD      __fastcall create               (LPMDB_RECORD rec,char * formula = NULL,bool no_call_data = false);
 DWORD      __fastcall update               (LPMDB_RECORD rec,DWORD mask);
 DWORD      __fastcall remove               (DWORD id);
 DWORD      __fastcall remove_group         (DWORD group_rec_id,otd_param_addr & pa);
 DWORD      __fastcall get_count            (otd_addr & addr);
 DWORD      __fastcall enum_objects         (otd_addr & addr,DWORD idx);
 DWORD      __fastcall enum_records         (DWORD idx,BOOL by_addr,LPMDB_RECORD rec);
 DWORD      __fastcall get_record           (DWORD id,LPMDB_RECORD rec);
 DWORD      __fastcall get_name             (DWORD id,LPMDB_RECORD_TEXT,bool long_name);
 DWORD      __fastcall get_name             (DWORD rec_id,bool long_name,wchar_t * buf,DWORD bsz);
 DWORD      __fastcall get_full_name        (DWORD id,LPMDB_RECORD_TEXT,bool long_name);
 DWORD      __fastcall get_full_name        (DWORD rec_id,bool long_name,wchar_t * buf,DWORD bsz);
 DWORD      __fastcall get_record_parents_id(DWORD rec_id,LPDWORD buf,DWORD count);


 DWORD      __fastcall set_name             (DWORD id,LPMDB_RECORD_TEXT,bool long_name);
 DWORD      __fastcall set_name             (DWORD id,wchar_t * text,bool long_name);
 DWORD      __fastcall add_to_combine       (DWORD id_comb,DWORD id_obj);
 DWORD      __fastcall remove_from_combine  (DWORD id_comb,DWORD id_obj);
 DWORD      __fastcall record_scale_changed (DWORD scal_id);

 DWORD      __fastcall get_combine_formula  (DWORD comb_id,LPMDB_RECORD_TEXT text);
 DWORD      __fastcall set_combine_formula  (DWORD comb_id,LPMDB_RECORD_TEXT text);
 DWORD      __fastcall get_combine_list     (DWORD comb_id, LPMDB_COMBINE_LIST comb_list);
 DWORD      __fastcall set_combine_list     (DWORD comb_id, LPMDB_COMBINE_LIST comb_list);

 DWORD      __fastcall update_text          (DWORD text_id,LPMDB_RECORD_TEXT text);
 DWORD      __fastcall get_text             (DWORD text_id,LPMDB_RECORD_TEXT text);
 DWORD      __fastcall enum_text            (DWORD text_id,LPMDB_RECORD_TEXT text);
 DWORD      __fastcall get_info             (LPMDB_DBINFO dbinfo);

 DWORD      __fastcall get_combine_count    (DWORD id);
 DWORD      __fastcall enum_combine         (DWORD idx,LPMDB_RECORD rec);
 DWORD      __fastcall rebuild_combine_index();

 DWORD      __fastcall kadr_enum            (DWORD idx,LPMDB_KADR  kdr);
 DWORD      __fastcall kadr_get             (DWORD kadr_id,LPMDB_KADR  kdr);
 DWORD      __fastcall kadr_create          (LPMDB_KADR);
 DWORD      __fastcall kadr_update          (LPMDB_KADR kadr,DWORD mask);
 DWORD      __fastcall kadr_remove          (DWORD id);
 DWORD      __fastcall kadr_get_entry_list  (DWORD kadr_id,LPMDB_KADR_ENTRY_LIST);
 DWORD      __fastcall kadr_set_entry_list  (DWORD kadr_id,LPMDB_KADR_ENTRY_LIST);
 DWORD      __fastcall kadr_make_record_list(DWORD kadr_id,record_idlist & rec);
 DWORD      __fastcall _kadr_update_diag    (kadrs_list::iterator kadr);
 DWORD      __fastcall kadr_update_diag     (DWORD kadr_id);
 __int64    __fastcall get_kadr_image_time  (DWORD kadr_id);
 __int64    __fastcall get_kadr_image_time  (kadrs_list::iterator kadr );
 DWORD      __fastcall kadr_find_byname     (wchar_t * long_name , LPDWORD kadr_id);
 DWORD      __fastcall kadr_find_record     (DWORD kadr_id      , DWORD rec_id);
 DWORD      __fastcall kadr_add_record      (DWORD kadr_id      , DWORD rec_id);
 bool       __fastcall open                 (wchar_t*);
 bool       __fastcall read_records         (wchar_t* folder);
 bool       __fastcall read_kadrs           (wchar_t* folder);
 bool       __fastcall read_combines        (wchar_t* folder);
 bool       __fastcall read_paintvars       (wchar_t* folder);

 void       __fastcall paint_vars_read (KeRTL::TStream & strm,  DWORD ver ,DWORD count);
 DWORD      __fastcall paint_vars_write(KeRTL::TStream & strm);
 DWORD      __fastcall paint_vars_stream_ver   ();
 DWORD      __fastcall paint_vars_enum         (DWORD idx,LPMDB_PAINT_VARIANT pv);
 DWORD      __fastcall paint_vars_add          (LPMDB_PAINT_VARIANT pv);
 DWORD      __fastcall paint_vars_update       (LPMDB_PAINT_VARIANT pv,DWORD  notify_code = MDB_NOTIFY_CODE_PAINTVAR_UPD);
 DWORD      __fastcall paint_vars_clear        ();
 DWORD      __fastcall paint_vars_end_update   (DWORD pv_count);
 bool       __fastcall paint_vars_search       (DWORD idx,Tmdb_paint_variants::iterator * ptr);
 void       __fastcall paint_vars_handle_change(mdb_paint_variant & paint_var,DWORD ch_mask);

 void       __fastcall store          (wchar_t * folder,DWORD mask);
 void       __fastcall store_records  (wchar_t * folder);
 void       __fastcall store_kadrs    (wchar_t * folder);
 void       __fastcall store_combines (wchar_t * folder);
 void       __fastcall store_paintvars(wchar_t * folder);

 bool       __fastcall lock  ();
 bool       __fastcall unlock();
 bool       __fastcall full_unlock();
 DWORD      __fastcall handle_otdm_proto  (LPMPROTO_HEADER mph);

 DWORD      __fastcall record_operation   (DWORD rec_id,DWORD op_code,float new_value );
 DWORD      __fastcall record_operation   (const LPMODEM_ADDR ma,const LPOTDM_PROTO_TUOPERATION tuop);
 DWORD      __fastcall tutr_select        (const LPMODEM_ADDR ma,DWORD rec_id,bool select);

 DWORD      __fastcall record_request_oscillogramm(DWORD rec_id,DWORD osc_lenght);
 void       __fastcall record_work_inout( LPOTDM_PROTO_WORK_INOUT wio);
 void       __fastcall record_clear_tu_error(DWORD rec_id);


 DWORD      __fastcall remove_medium_order(modem_addr & ma,DWORD kadr);
 void       __fastcall report_alarm_danger(mdb_record & rec);
 void       __fastcall report_tu_operation(mdb_record & rec,DWORD op_code);
 DWORD      __fastcall check_db           (LPMDB_CHECK_PARAM);
 void       __fastcall recalc_combines    ();
 DWORD      __fastcall calc_change_percent(mdb_record & rec,float new_value);
 bool       __fastcall kadr_exists        (DWORD kadr_id);
 static bool __fastcall check_not_send_record(sended_recs * sr,DWORD rec_id);
 DWORD      __fastcall kadrs_get_range(DWORD rec_id,entry_index & kadr_list);
 void       __fastcall archives_increment( lpotd_addr addr,bool alarm
                                          ,__int64 hist_beg_time,__int64 hist_end_time
                                          ,trecords & recs,trecords_index & alarms
                                          );
 __int64    __fastcall get_file_time(wchar_t * file_name);
 __int64    __fastcall get_file_time(DWORD _id);
 void       __fastcall send_database_reopen  ();

 DWORD      __fastcall enum_orders(DWORD idx,LPORDER_ITEM oi);
 void       __fastcall do_timer   (int msec);

 protected:
 DWORD      __fastcall mproto_send(order_vector::iterator modem_low,order_vector::iterator modem_hi,DWORD mask,DWORD cmd,DWORD err,LPVOID data,DWORD data_size,bool begin,bool finish = false);
 DWORD      __fastcall mproto_enum_kadrs(MODEM_ADDR & to,DWORD mask);
 DWORD      __fastcall mproto_send_kadr_removed  (DWORD kadr_id);
 DWORD      __fastcall mproto_get_kadr  (MODEM_ADDR & to,DWORD cmd,DWORD kadr_id,DWORD mask,bool begin = true,bool finish = true);
 DWORD      __fastcall mproto_get_kadr_image  (MODEM_ADDR & to,DWORD kadr_id);
 DWORD      __fastcall mproto_get_image       (DWORD cmd,MODEM_ADDR & to,DWORD image_id ,__int64 image_time, DWORD kadr_id = -1);
 DWORD      __fastcall mproto_get_kadr_entryes(MODEM_ADDR & to,DWORD kadr_id);
 DWORD      __fastcall mproto_get_kadr_records(MODEM_ADDR & to,DWORD kadr_id);
 DWORD      __fastcall mproto_send_kadr_changes(DWORD kid,DWORD mask,order_vector::iterator modem_low,order_vector::iterator modem_hi,sended_recs * sr );
 DWORD      __fastcall mproto_get_record_name(DWORD cmd,MODEM_ADDR & to,DWORD rec_id,bool short_name,bool full_name);
 DWORD      __fastcall mproto_send_name_changes(DWORD rec_id,DWORD mask,order_vector::iterator modem_low,order_vector::iterator modem_hi);
 DWORD      __fastcall mproto_send(MODEM_ADDR & to,DWORD cmd,DWORD err,LPVOID data,DWORD data_size,bool begin,bool finish = false);
 DWORD      __fastcall mproto_get_paint_variants(MODEM_ADDR & to,DWORD cmd,LPVOID data,DWORD data_size);
};

inline  bool       __fastcall med_db::lock()
{
 #if( FASTMUTEX_LOCKER != 0)
  return locker.Lock();
 #else
  return locker.LockForWrite();
 #endif
}

inline  bool       __fastcall med_db::unlock()
{
 #if( FASTMUTEX_LOCKER != 0)
  return locker.Unlock();
 #else
  return locker.WriteUnlock();
 #endif
}

inline  bool      __fastcall med_db::full_unlock()
{
 #if( FASTMUTEX_LOCKER != 0)
 return  locker.FullUnlock();
 #else
 while(!locker.WriteUnlock());
 while(!locker.ReadUnlock());
 return true;
 #endif
}

inline DWORD      __fastcall med_db::rebuild_combine_index()
{
 DWORD ret ;
 lock();
 ret = combine.rebuild_index();
 unlock();
 return ret;
}

class TMediumDB;

class TArchiveThread:public TGKThread
{
 TMediumDB * owner;
 HANDLE    term_event;
 TEventSimpleDataQueue   input_queue;
 orders_table      orders;//Список подписки на архивы аварий
 trecords          recs;
 trecords_index    alarm_recs;
 bool __fastcall BeforeExecute  ();
 void __fastcall BeforeTerminate();
 int  __fastcall Execute();
 TArchiveThread(){};
 void __fastcall handle_data        ();
 void __fastcall handle_otd_arch    (bool alarm,otd_proto & op);
 void __fastcall handle_otd_arch_cmd(bool alarm,otd_proto & op);
 void __fastcall handle_medium_query(LPMPROTO_HEADER mph);
 void __fastcall handle_order       (MODEM_ADDR & to,DWORD cmd,DWORD kadr_id,DWORD mask,bool finish);
 void __fastcall send_order         (trecords & recs);
 void __fastcall do_send_order      (order_vector::iterator lo,order_vector::iterator hi,trecords & recs,record_idlist & kadr_recs);
 void __fastcall mproto_send        (order_vector::iterator lo,order_vector::iterator hi,DWORD cmd,DWORD err,LPVOID data,DWORD data_sz,bool start,bool finish);
 void __fastcall erase_records      (lpotd_addr  addr);
 public:
 TArchiveThread(TMediumDB * own ):owner(own){};
 void __fastcall put_data(LPVOID data,DWORD data_size){input_queue.PutIntoQueue(data,data_size);}
};

class TMediumDB:public TGKModule,protected TGKThread
{
 protected:
 MDB_SETTINGS          settings;
 med_db                mdb;
 TGKModuleInterface    modemmer;
 TGKModuleInterface    scale;

 TEventSimpleDataQueue input_queue;
 rx_buffer             handle_buffer;
 TEvent                terminate_event;
 TWaitableTimer        waitable_timer;
 DWORD                 int_line;
 DWORD                 report_id;
 DWORD                 report_tu_id;
 DWORD                 report_scripts;
 DWORD                 report_alarm_danger_id;
 TArchiveThread        archvie_thread;

 void    __fastcall reg_reports();

 WORD    __fastcall get_type(){return  MT_MEDIUMDB ;}
 DWORD   __fastcall get_config_data(DWORD mask,LPVOID buf,DWORD bsz);
 bool    __fastcall set_config_data(DWORD mask,LPVOID buf,DWORD bsz);
 bool    __fastcall compare_config_data(DWORD mask,LPVOID buf,DWORD bsz,LPDWORD ch_mask,BOOL * restart);
 DWORD   __fastcall get_window_module  (wchar_t * buf,DWORD bsz);
 LRESULT __fastcall processing(DWORD cmd,LPARAM p1,LPARAM p2);
 DWORD   __fastcall start(DWORD reason,LPARAM sp);
 DWORD   __fastcall stop (DWORD reason);
 bool    __fastcall read_settings  ();
 bool    __fastcall write_settings ();
 void    __fastcall open_mdb();
 LRESULT __fastcall make_order(otd_addr &addr,DWORD parts);
 LRESULT __fastcall recv_handler(LPMPROTO_HEADER mph,DWORD sz);
 DWORD   __fastcall get_mem_used();
 LRESULT __fastcall get_modem_props(LPMODEM_PROPS mp);

 LRESULT __fastcall get_name(DWORD id,LPMDB_RECORD_TEXT rt,bool long_name);
 LRESULT __fastcall set_name(DWORD id,LPMDB_RECORD_TEXT rt,bool long_name);
 LRESULT __fastcall add_record_bymask(DWORD addr_mask);

 const char  * __fastcall get_gkthread_name();
 DWORD   __fastcall OnException(DWORD ExceptionCode,LPEXCEPTION_POINTERS);
 DWORD   __fastcall on_exception  (lpexcept_record er);
 int     __fastcall Execute();
 void    __fastcall BeforeTerminate();
 bool    __fastcall BeforeExecute();
 void    __fastcall do_handle_recv();
 void    __fastcall Terminate(){terminate_event.SetEvent(true);TGKThread::Terminate();}
 LRESULT __fastcall on_notify_code(LPNOTIFY_CODE,DWORD mask);
 void    __fastcall on_modem_line_state(LPMODEM_LINE_STATE mls);
 LRESULT __fastcall fa_handler(LPMPROTO_HEADER mph);
 virtual   DWORD   __fastcall check_license(){return MOD_LICENSE_REGISTERED; }
 public:
  TMediumDB();
  void    __fastcall set_ccflags(DWORD fl,bool set);
  LRESULT __fastcall send_call (otd_addr & addr,DWORD parts = -1);
  LRESULT __fastcall send(LPMPROTO_HEADER mph);


  DWORD   __fastcall scale_calc(DWORD scale_id,DWORD  & kvants,float & result ,bool direct);
  DWORD   __fastcall scale_minmax(LPSC_MINMAX mm){return scale.call(CM_SC_MINMAX,(LPARAM)mm,0);}
  void    __fastcall mdb_report             (DWORD rep_id,DWORD rep_type,wchar_t * rep_text,LPVOID data = 0,DWORD dtsz = 0);
  void    __fastcall mdb_report             (DWORD rep_type,wchar_t * rep_text,LPVOID data = 0,DWORD dtsz = 0);
  void    __fastcall mdb_report_alarm_danger(DWORD rep_type,wchar_t * rep_text,LPVOID data = 0,DWORD dtsz = 0);
  void    __fastcall mdb_report_tutr        (DWORD rep_type,wchar_t * rep_text,LPVOID data = 0,DWORD dtsz = 0);
  void    __fastcall mdb_report_script      (DWORD rep_type,const wchar_t * rep_text,LPVOID data = 0,DWORD dtsz = 0)
  {
    mdb_report(report_scripts,rep_type,(wchar_t*)rep_text,data,dtsz);
  }
  TEvent & __fastcall get_terminate_event()   {return terminate_event;}
  med_db & __fastcall get_med_db()            {return mdb;}
  DWORD    __fastcall mproto_send(int fa,MODEM_ADDR & to,DWORD cmd,DWORD err,LPVOID data,DWORD data_size,bool begin,bool finish );
  static THeap heap;
  static DWORD total_mem_alloc;
  static void * __stdcall mem_alloc(size_t sz );
  static void   __stdcall mem_free (void * ptr );


};

inline  void       __fastcall TMediumDB::mdb_report(DWORD rep_type,wchar_t * rep_text,LPVOID data ,DWORD dtsz )
 {
  mdb_report(this->report_id,rep_type,rep_text,data,dtsz);
 }

inline  void       __fastcall TMediumDB::mdb_report_tutr(DWORD rep_type,wchar_t * rep_text,LPVOID data ,DWORD dtsz )
 {
  mdb_report(this->report_tu_id,rep_type,rep_text,data,dtsz);
 }

 inline  void       __fastcall TMediumDB::mdb_report_alarm_danger(DWORD rep_type,wchar_t * rep_text,LPVOID data ,DWORD dtsz )
 {
  mdb_report(this->report_alarm_danger_id,rep_type,rep_text,data,dtsz);
 }


#endif
