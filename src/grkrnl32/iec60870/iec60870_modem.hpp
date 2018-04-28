#ifndef __IEC60870_MODEM_INC_HPP__
#define __IEC60870_MODEM_INC_HPP__

#include <modem.hpp>
#include <kertlio.hpp>
#include <kedatas.hpp>
#include <rx_buffer.hpp>
#include <tregstry.hpp>
#include <otd_storage.hpp>
#include <kestrm.hpp>
#include <iec60870_modem.h>
#include <kestrm.hpp>
#include <wsockdef.h>
#include <vector>
#include <map>
#include <vector_index.hpp>

using namespace vector_index;

class iec60870_buffer_t:protected rx_buffer
{
 lpiec60870_proto_header_t current_hdr;
 KeRTL::TFASTMutex locker;
 DWORD  __fastcall get_frame_offset (lpiec60870_proto_header_t phdr);
 bool   __fastcall is_frame_complete(lpiec60870_proto_header_t phdr);
 public:
 iec60870_buffer_t(DWORD sz = 0):rx_buffer(sz),current_hdr(NULL){}
 int  __fastcall add_bytes(LPBYTE ptr,DWORD length);
 lpiec60870_proto_header_t __fastcall get_current_frame(){ return current_hdr; }
 bool __fastcall is_frame_complete();
 bool __fastcall next_frame       (bool move);
 void __fastcall move_to_begin    ();
 void __fastcall clear() {TLockHelper l(locker); current_hdr = NULL; rx_buffer::clear();}
 void __fastcall insert_first    (lpiec60870_proto_header_t hptr);
 bool __fastcall grow    (DWORD grow_size);
 int  __fastcall get_frames_count();
 bool __fastcall set_current_frame(DWORD number);
 bool __fastcall erase_frames(DWORD count);
 KeRTL::TFASTMutex & get_locker(){return locker;}
};

inline DWORD  __fastcall iec60870_buffer_t::get_frame_offset(lpiec60870_proto_header_t phdr)
  {
    TLockHelper l(locker);
    return ((LPBYTE)phdr) - begin();
  }



typedef vector_index::indexed_vector<iec60870_record,iec60870_record_less,iec60870_record_otd_less> iec60870_records_t;

class Tiec60870line:public modem_line,protected TGKThread
{
  protected:
   iec60870_buffer_t  rxbuf;
   iec60870_buffer_t  txbuf;
   KeRTL::TEvent    * term_event;
   TWaitableTimer   * wtimer;
   HANDLE             socket_event;
   TIPSocket        * socket;
   WSAEVENT           net_events;
   TSynchroSet        sset;
   DWORD              last_timer_ticks;

   DWORD              timer_counter;
   DWORD              timer_count;
   void __fastcall    countered_timer_start(DWORD cnt){timer_counter = 0;timer_count = cnt;}
   bool __fastcall    is_countered_timer_fired();

   iec60870_seq_num_t  tx_number;
   DWORD K_counter;

   iec60870_seq_num_t  rx_number;
   DWORD W_counter;
   int WT[MAX_WT_TIMERS];
   IEC60870_LINE_CONFIG            line_config;

   TFASTMutex                      storage_locker;
   otd_storage                     storage;
   iec60870_records_t::container_t record_changes;

   iec60870_records_t              ts_recs;
   iec60870_records_t              tit_recs;
   iec60870_records_t              tii_recs;
   iec60870_records_t::container_t tutr_active; //Список активных ТУ/ ТР

   DWORD __fastcall get_W_count();
   DWORD __fastcall get_K_count();
   DWORD __fastcall get_dev_addr();
   DWORD __fastcall get_common_addr();

   void  __fastcall WTimer_start(int idx);
   void  __fastcall WTimer_stop (int idx);
   bool  __fastcall WTimer_check(int idx);

   void  __fastcall __check_tx_number(DWORD number);
   void  __fastcall __check_rx_number(DWORD number);
   void  __fastcall __do_receipt_rx(bool force = false);
   void  __fastcall __send_next_frame();



   enum   {max_frame_size = 8192};
   virtual DWORD __fastcall get_line_text  (wchar_t * text,DWORD text_sz);
   virtual DWORD __fastcall get_max_frame_size(){return max_frame_size;}
   virtual DWORD __fastcall get_tx_queue_size ();
   virtual DWORD __fastcall get_line_flags    (){return MLPF_TXABLE|MLPF_RXABLE|MLPF_DATASOURCE;}
   virtual bool  __fastcall send              (LPMPROTO_HEADER mph,DWORD sz) ;
   virtual bool  __fastcall do_start ();
   virtual bool  __fastcall do_stop  ();

   virtual bool  __fastcall BeforeExecute     ();
   virtual void  __fastcall BeforeTerminate   ();
   virtual int   __fastcall Execute           ();
   virtual void  __fastcall Terminate         ();
           DWORD __fastcall get_wait_ticks    (DWORD ms = 1000);
   virtual void  __fastcall on_connect        ();
   virtual void  __fastcall on_disconnect     ();
   virtual DWORD __fastcall refresh(DWORD sbl);
           void  __fastcall send(lpiec60870_proto_header_t phdr);


           void __fastcall handle_recv        ();
           void __fastcall handle_frame      (lpiec60870_proto_header_t phdr);
           void __fastcall handle_frame_typeS(lpiec60870_proto_header_t phdr);
           void __fastcall handle_frame_typeU(lpiec60870_proto_header_t phdr);
           void __fastcall handle_frame_typeI(lpiec60870_proto_header_t phdr);
           void __fastcall handle_asdu       (lpiec60870_proto_header_t phdr);
           template <typename obj_t>
           void  __fastcall handle_objects(lpiec60870_asdu_header asdu);
           void  __fastcall __update_record ( iec60870_record & rec);
           void  __fastcall __record_changed( iec60870_record & rec,bool bad_diag = false);
           void  __fastcall __notify_record_changes      (DWORD line_num
                                                          ,iec60870_records_t::container_t::iterator cbeg
                                                          ,iec60870_records_t::container_t::iterator cend);
           void  __fastcall __handle_record_changes_range(DWORD line_num
                                                          ,iec60870_records_t::container_t::iterator cbeg
							  ,iec60870_records_t::container_t::iterator cend);

           void  __fastcall handle_remote_control(lpiec60870_proto_header_t phdr);
           bool  __fastcall tutr_find_record(BYTE command,DWORD obj_num,iec60870_records_t::iterator & ptr );
           void  __fastcall tutr_start_timer (      iec60870_record & rec );
           bool  __fastcall tutr_need_check  (const iec60870_record & rec );
           bool  __fastcall is_tu_success    (const iec60870_record & rec );
           void  __fastcall remove_tutr_timer(const iec60870_record & rec );




           void  __fastcall __setup_group_values (otd_proto * op);
           void  __fastcall __queue_rxdata(otd_proto * op);

           void  __fastcall __set_bad_diag      ();

           void  __fastcall __handle_otd_query  (otd_proto * op);
           void  __fastcall  __do_recv_cp_data  (bool inc_name );

            void  __fastcall __handle_otd_control(otd_proto * op);


           int   __fastcall socket_execute       ();
           bool  __fastcall create_socket        ();
           void  __fastcall destroy_socket       ();
           void  __fastcall handle_socket_events ();

   void  __fastcall connect_socket   ();
   void  __fastcall tutr_timer       ();
   void  __fastcall on_waitable_timer();
   void  __fastcall on_socket_event  ();
   void  __fastcall on_socket_timeout();
   void  __fastcall on_socket_connect   (DWORD error);
   void  __fastcall on_socket_recv      (DWORD error);
   void  __fastcall on_socket_disconnect(DWORD error);
   bool  __fastcall do_socket_send      (LPBYTE ptr,DWORD len);


   //OTD storage section
   DWORD __fastcall otd_create_group   (DWORD otd_fa,DWORD otd_group,DWORD first,DWORD count,DWORD otd_type);
   void  __fastcall otd_create_groups  (iec60870_records_t & recs);
   void  __fastcall handle_record_changes ();


   virtual const char  * __fastcall get_gkthread_name(){return "IEC60870 Line thread";}
   bool __fastcall is_need_restart(const IEC60870_LINE_CONFIG & ln_conf);
   Tiec60870line():modem_line(-1)
   ,term_event(NULL),wtimer(NULL)
   ,socket(NULL),net_events(INVALID_HANDLE_VALUE)
   {}

   friend class Tiec60870modem;


  public:
   Tiec60870line(const IEC60870_LINE_CONFIG & cfg):modem_line(cfg.cp_number)
   ,term_event(NULL),wtimer(NULL)
   ,socket(NULL),net_events(INVALID_HANDLE_VALUE)
   {
    memcpy(&line_config,&cfg,sizeof(line_config));
   }

           bool     __fastcall set_config(const IEC60870_LINE_CONFIG & ln_conf);
           bool     __fastcall get_config(      IEC60870_LINE_CONFIG & ln_conf);
           int      __fastcall read         (KeRTL::TStream & strm,DWORD stream_ver);
           int      __fastcall read_records (KeRTL::TStream & strm,iec60870_records_t & recs,DWORD stream_ver);
           int      __fastcall write        (KeRTL::TStream & strm);
           int      __fastcall write_records(KeRTL::TStream & strm,iec60870_records_t & recs);
           void     __fastcall do_report(DWORD evt_type,wchar_t * rep_text,LPVOID data,DWORD dt_sz);
           DWORD    __fastcall get_addr_size  (){TLockHelper l(locker); return line_config.obj_addr_size;}
           iec60870_records_t * __fastcall get_records_for(DWORD otd_fa);
           DWORD    __fastcall get_asdu_records_count(DWORD otd_fa);
           DWORD    __fastcall enum_asdu_records     (lpiec60870_record rec);
           DWORD    __fastcall get_asdu_record       (lpiec60870_record rec);
           LRESULT  __fastcall otd_create_group      (LPIEC60870_LINE_GROUP_PARAM lgc);
           LRESULT  __fastcall otd_delete_group      (BYTE otd_fa,BYTE otd_group);
           LRESULT  __fastcall record_bind           (lpiec60870_record rec);
           LRESULT  __fastcall record_bind_rccmd     (lpiec60870_record rec);
           DWORD    __fastcall get_group_count       (DWORD otd_fa,DWORD otd_group);
           DWORD    __fastcall get_group_min_max_numbers(DWORD otd_fa,DWORD otd_group,LPDWORD min_num,LPDWORD max_num);
           DWORD    __fastcall get_line_diag(bool rescan);


   static  __int64  __fastcall get_timestamp(lpiec60870_cp24time2a tm);
   static  __int64  __fastcall get_timestamp(lpiec60870_cp56time2a tm);
   static  void     __fastcall set_timestamp(__int64 tmstamp,lpiec60870_cp56time2a tm);
};

class Tiec60870modem : public TModemBase
{
 static THeap         mod_heap;
 static DWORD         total_mem_alloc;
 DWORD                rep_id;
 IEC60870_MOD_CONFIG  mod_config;
 DWORD                pu_diag;

         void     __fastcall reg_reports();
 virtual void     __fastcall free_line          (modem_line * line);
 virtual DWORD    __fastcall get_modem_flags    (){return  MPF_DATA_SOURCE;}
 virtual int      __fastcall convert_rx_data    (LPWORD fa,LPBYTE in,int in_len,LPBYTE out,DWORD out_sz) ;
 virtual DWORD    __fastcall get_lines_limit    () {return MODEM_ADDR_MAXVALUE;}
 virtual DWORD    __fastcall get_config_data    (DWORD mask,LPVOID buf,DWORD bsz);
 virtual bool     __fastcall set_config_data    (DWORD mask,LPVOID buf,DWORD bsz);
 virtual bool     __fastcall compare_config_data(DWORD mask,LPVOID buf,DWORD bsz,LPDWORD ch_mask,BOOL * restart);
         bool     __fastcall read_settings      ();
         bool     __fastcall write_settings     ();
         void     __fastcall read_lines         (TRegsReader & rd);
         void     __fastcall write_lines        (TRegsWriter & rw);
         BOOL     __fastcall check_pu_number    ();
         BOOL     __fastcall check_tcp_proto    ();
 virtual BOOL     __fastcall can_start          (DWORD reason,LPARAM p2);
 virtual DWORD    __fastcall start              (DWORD reason,LPARAM p2);
 virtual DWORD    __fastcall stop               (DWORD reason);

 virtual LRESULT  __fastcall processing         (DWORD cmd,LPARAM p1,LPARAM p2);
 virtual DWORD    __fastcall get_window_module  (wchar_t * buf,DWORD bsz);
         BYTE     __fastcall get_pu_number      ();
 virtual DWORD   __fastcall check_license(){return MOD_LICENSE_REGISTERED; }
         void    __fastcall  do_recv_pu_data(bool inc_name );

         DWORD   __fastcall get_pu_diag    (bool rescan);
         DWORD   __fastcall scan_pu_diag   ();
         LRESULT __fastcall _line_get(LPIEC60870_LINE_CONFIG ln_conf,DWORD line_num);
         LRESULT __fastcall _line_add(LPIEC60870_LINE_CONFIG ln_conf);
         LRESULT __fastcall _line_upd(LPIEC60870_LINE_CONFIG ln_conf,DWORD line_num);
         LRESULT __fastcall _line_del(DWORD line_num);
 Tiec60870line * __fastcall get_iec60870_line(DWORD line_num);
         DWORD   __fastcall get_mem_used();
         LRESULT __fastcall send        (LPMPROTO_HEADER mph,DWORD sz);

 public:
 Tiec60870modem();
 void    __fastcall  do_recv_cp_data(DWORD cp_number,DWORD diag,wchar_t * cp_name );
 void    __fastcall  do_report(DWORD evt_type,wchar_t * rep_text,LPVOID data,DWORD dt_sz);

 LRESULT __fastcall  get_asdu_types_count  (DWORD line_number);
 LRESULT __fastcall  enum_asdu_types       (DWORD line_number,DWORD idx );
 LRESULT __fastcall  get_asdu_records_count(DWORD line_number,DWORD type);
 LRESULT __fastcall  enum_asdu_records     (DWORD line_number,LPARAM p2 );
 LRESULT __fastcall  get_asdu_records      (DWORD line_number,LPARAM p2 );
 LRESULT __fastcall  otd_create_group      (DWORD line_number,LPARAM p2 );
 LRESULT __fastcall  bind_record_otd       (DWORD line_number,LPARAM p2 );
 LRESULT __fastcall  bind_record_otd_rccmd (DWORD line_number,LPARAM p2 );
 LRESULT __fastcall  get_group_max_number  (DWORD line_number,LPARAM p2 );


 static void * __fastcall mem_alloc(size_t size);
 static void   __fastcall mem_free (void * ptr );

};

inline    Tiec60870line * __fastcall Tiec60870modem::get_iec60870_line(DWORD line_num)
{
     return dynamic_cast<Tiec60870line * >(get_line(line_num));
}


#endif
