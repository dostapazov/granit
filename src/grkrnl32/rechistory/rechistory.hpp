#ifndef _RECHISTORY_INC_HPP_
#define _RECHISTORY_INC_HPP_

#define BOOST_THREAD_USE_LIB
#include <boost/thread/locks.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>

#include <ibclass.h>

#include <gkmodules.hpp>
#include <rechistory.h>
#include <modemmer.h>
#include <modem_proto.h>
#include <kedatas.hpp>
#include <kertlio.hpp>
#include <stack>


class TRecHistory:public TGKModule//,public TGKThread
{
 protected:
 typedef std::map<DWORD,DWORD> tkadr_orders;

 /*
 typedef boost::recursive_mutex mutex_type;
 typedef boost::lock_guard<mutex_type> guarder;
 */
 typedef TFASTMutex  mutex_type;
 typedef TLockHelper guarder;
 typedef std::stack<TIBDatabase*>   Tdb_cached;


 mutex_type locker;

 REC_HIST_CONFIG    config;
 tkadr_orders       orders;
 TIBDatabase        ibdb;
 TIBTransaction     wr_trans;
 TIBTransaction     rd_trans;
 TIBSqlQuery        wr_query_recs;
 TIBSqlQuery        wr_ins_records;
 TIBSqlQuery        wr_ins_string;
 TIBSqlQuery        wr_query_arch;
 TIBSqlQuery        rd_query;

 Tdb_cached         db_cached;
 mutex_type         dbc_locker;

 DWORD              rep_id;

 TGKModuleInterface  modemmer;
 int   int_line;
 LONG  pkt_num;

 KeRTL::TEvent                 term_event;
 KeRTL::TEventSimpleDataQueue  data_queue;
 KeRTL::TSimpleDataQueue       not_commited_data;
 boost::thread                 main_thread;
 boost::thread                 maintance_thread ;
 boost::thread                 statistic_thread;
 KeRTL::TIoCompletionPort      maintance_icp;
 #define  MAINTANCE_TERM_KEY   0
 #define  MAINTANCE_START_KEY  1


 void    __fastcall reg_reports();

 WORD    __fastcall get_type       () {return MT_MEDIUMDB_HISTORY;}
 DWORD   __fastcall get_mem_used   ();
 DWORD   __fastcall get_config_data(DWORD mask,LPVOID buf,DWORD bsz);
 bool    __fastcall set_config_data(DWORD mask,LPVOID buf,DWORD bsz);
 bool    __fastcall compare_config_data(DWORD mask,LPVOID buf,DWORD bsz,LPDWORD ch_mask,BOOL * restart);
 bool    __fastcall write_settings();
 bool    __fastcall read_settings ();

 BOOL    __fastcall can_start(DWORD reason,LPARAM p2);
 DWORD   __fastcall start(DWORD reason,LPARAM p2) ;
 DWORD   __fastcall stop (DWORD reason);
 bool    __fastcall check_data_base       (LPREC_HIST_CONFIG cfg);
 void    __fastcall init_trans_query(TIBDatabase * ibdb,TIBTransaction * trans,TIBSqlQuery * rd_query,bool write);
 bool    __fastcall open_data_base   (TIBDatabase & ibdb,REC_HIST_CONFIG & config,TIBTransaction * wr_trans,TIBTransaction * rd_trans,TIBSqlQuery * wr_query,TIBSqlQuery * rd_query);
 bool    __fastcall reopen_data_base (TIBDatabase & ibdb);

 void    __fastcall send_order(bool order);
 LRESULT __fastcall send(LPMPROTO_HEADER mph);
 DWORD   __fastcall send_command(DWORD fa,bool begin,bool finish,DWORD cmd,DWORD err,LPVOID data,DWORD data_size );

 LRESULT __fastcall handle_query_archive  (LPMPROTO_HEADER mph);
 LRESULT __fastcall handle_otdm_proto     (LPMPROTO_HEADER mph);
 bool    __fastcall handle_records_changes(LPOTDM_PROTO_HEADER omph,bool archive);
 void    __fastcall handle_record_names   (LPOTDM_PROTO_HEADER omph,bool short_name);
 bool    __fastcall handle_get_kadr       (LPOTDM_PROTO_HEADER omph,bool archive,bool end);
 bool    __fastcall handle_records        (LPOTDM_PROTO_HEADER omph);
 bool    __fastcall store_record          (mdb_record & rec,DWORD mask,bool archive,bool upd_rec_table = false);
 bool    __fastcall store_archive_times   (DWORD kakdr_id,bool alarm,__int64 beg_tm,__int64 end_tm);

 void    __fastcall request_record_name  (DWORD rec_id, bool short_name);


 DWORD   __fastcall get_window_module     (wchar_t * buf,DWORD bsz);
 LRESULT __fastcall check_query           (char * query);
 LRESULT __fastcall check_query           (TIBTransaction * tr,char * query);
   int              write_sql_thread      ();
   void  __fastcall write_sql_proc        (int & time_count,int & cycle_count,int commit_after);
   void  __fastcall write_sql_timeout     (int &time_count, int & cycle_count,REC_HIST_CONFIG & cfg);
   void  __fastcall report_connect(TIBDatabase & ibdb,const wchar_t * dbpath);
   void  __fastcall report_sql_error      (TIBStatus * ibs);
    int             maintance_thread_proc ();
    void __fastcall do_maintance_db       ();
     int            statistic_thread_proc ();

     int            update_archive_thread (DWORD cmd,LPBYTE request);
     int            query_archive_set_props(TIBSqlQuery & query,LPOTDM_ARCHIVE_HEADER arch_hdr);
     int            read_archive_thread   (DWORD cmd,LPBYTE request);
     bool           query_records_archive (TIBSqlQuery & query,LPOTDM_ARCHIVE_QUERY arch_query,char * sql_rd_history);
     bool           query_kadr_alarms_archives (TIBSqlQuery & query,LPOTDM_ARCHIVE_QUERY arch_query);

    void __fastcall send_archive_query_records_results  (DWORD cmd,TIBSqlQuery & query,MODEM_ADDR &to_addr,QWORD hist_id);
    void __fastcall send_archive_query_alarms_results   (DWORD cmd,TIBSqlQuery & query,MODEM_ADDR &to_addr,QWORD hist_id);

           void __fastcall close_cached_db();
           bool __fastcall end_transaction(TIBTransaction * tr,bool commit);
           LRESULT __fastcall restore_database (LPRECHIST_RESTORE rhr,char * cl_lib);
 virtual   DWORD   __fastcall check_license(){return MOD_LICENSE_REGISTERED; }
           DWORD   __fastcall get_modem_line_frame_size(modem_addr & to_addr);
 public :
 TRecHistory();
 virtual ~TRecHistory();
 virtual LRESULT __fastcall processing(DWORD cmd,LPARAM p1,LPARAM p2);
 static  char *  __fastcall new_str(const char * src);
 static  void    __fastcall replace_str(char ** dst,const char * src);
};




#endif

