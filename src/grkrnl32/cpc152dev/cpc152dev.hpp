#ifndef __CPC152DEV_MODEM__
#define __CPC152DEV_MODEM__

#define BOOST_THREAD_USE_LIB

#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <wsockdef.h>

#include <cpc152dev.h>
#include <modem.hpp>
#include <kertlio.hpp>
#include <kedatas.hpp>
#include <rx_buffer.hpp>
#include <tregstry.hpp>
#include <otd_storage.hpp>
#include <kestrm.hpp>
#include <cpc152proto.h>

#define CPC152_LINE_BUFFSZ   (8192)
class  Tcpc152controller ;
struct cpc152dev_t
{
  sotd_proto groups [CPC152_MAX_GROUPS];
  LPVOID     dev_ext[CPC152_MAX_GROUPS];
};




#define SCANINPUT_MULTYPLIER 10
struct analog_ext_t
{
 TFASTMutex           * locker;
 int                    dev_num,grp_num;
 //double                 freq;
 //double                 sample_freq;
 //int                    dft_frame_size;
 int                    scan_counter;
 int                    scan_count;
 int                    scan_to_recalc;
 int                    scan_capacity;
 int                    recalc_number;
 int                    recalc_done;
 sotd_proto             ext_result;
 int                    param_count;
 int                    overtime;
 int                    overtime_count;
 int                    avg_scan_time_lenght;
 int                    avg_scan_time_param;
 int                    enable_phase_correct;
 int                    scan_time_param;
 DWORD                  dft_enabled;

 LPCPC152_ANALOG_PARAM  params[CPC152_MAX_PARAM_ANALOG];
 analog_ext_t (int _dev_num,int _grp_num,DWORD _dft_enabled);
 ~analog_ext_t();
 void __fastcall free();
 void __fastcall init               (int _scan_count);
 void __fastcall alloc_params       (int scan_count);
 void __fastcall add_param_scan     (int param_num,__int16 val);
 void __fastcall prepare_params_calc();
 void __fastcall calc_param         (LPCPC152_ANALOG_PARAM ap);
 TFASTMutex & __fastcall get_locker (){return *locker;}
 double __fastcall get_phase_correction(double freq);
 bool   __fastcall is_dft_enable(DWORD param_num)
 {
   return (dft_enabled & (1<<param_num)) ? true : false;
 }

};

typedef analog_ext_t * lpanalog_ext_t;



class dft_t
{
  protected:
  TFASTMutex locker;
  double  freq;
  double  sample_freq;
  double  gate;
  double  percent;//Процент от максимальной амплитуды для усреднения
  int     dft_frame;
  int     dft_limit;
  double  dF;
  int         index_min;
  int         index_count;
  double    * dft_input;
  complex_t * dft_result;
  fftw_plan   dft_plan;
  int       * index;
  public :
   dft_t(){dft_input = NULL; dft_result = NULL; dft_plan = NULL; index = NULL; dft_frame = index_count = 0; }
  ~dft_t(){release();}
  void __fastcall release();
  void __fastcall setup  (int dft_frame,double sample_freq,double freq,double gate,double percent);
  int  __fastcall do_dft (double * input,int input_size,double * res_freq,double * res_phase,double *offset);
  int  __fastcall get_dft_frame(){TLockHelper l(locker); return dft_frame;}
double __fastcall get_freq(){return freq;}
static  double get_phase_quadrant(complex_t* cv);


};

#define CPC152DEV_LINE_HANDLE_SUCCESS 0
#define CPC152DEV_LINE_HANDLE_ERROR_CHECKSUM 1



class cpc152dev_line : public modem_line,public TGKThread
{
  protected:
     CPC152_LINE_CONFIG line_config;
     TEventSimpleDataQueue  send_queue;
     modem_proto_buffer     mpb;
     TIPAddress             host_addr;
     KeRTL::TEvent          terminate_event;
     TWaitableTimer         timer;
     WSAEVENT               network_event;
     TSynchroSet            ss;
     TIPSocket  *           socket;
     TFASTMutex *           otd_locker;

     DWORD                  no_rx_timer;
     DWORD                  no_tx_timer;
     BOOL                   sending;
     WORD                   last_pkt_num;
     WORD                   pkt_num;
     DWORD                  connect_error_log;
     int                    analog_sample_size;
     int                    discrete_seq;
     int                    analog_seq;
     cpc152dev_t            discrete_devs[CPC152_MAX_SLOTS];
     cpc152dev_t            analog_devs  [CPC152_MAX_SLOTS];
     QWORD                  last_scan_time;
     typedef std::vector<QWORD> alarms_list_t;
     alarms_list_t              alarms_list;
     QWORD                      current_alarm;
     QWORD                      alarm_times[2];
     QWORD                      local_alarm_times[2];
     int                        alarm_recv_timer ;
     typedef std::vector<CPC152_ALARM_START>  alarm_start_t;
     alarm_start_t              alarm_starts;
     dft_t                      dft;
     TIoCompletionPort          icp;
     boost::thread            * analog_params_calc_thread;
     CPC152_DFT_ENABLED     dft_enabled;

	 DWORD __fastcall get_max_frame_size() {return CPC152_LINE_BUFFSZ;}
	 DWORD __fastcall get_tx_queue_size () {return 0;}
	 DWORD __fastcall get_line_flags    () {return MLPF_TXABLE|MLPF_RXABLE|MLPF_DATASOURCE;}
	 DWORD __fastcall get_line_text     (wchar_t * text,DWORD text_sz);
	 bool  __fastcall BeforeExecute     ();
	 void  __fastcall BeforeTerminate   ();
	 int   __fastcall Execute           ();
	 bool  __fastcall do_stop           ();
	 bool  __fastcall do_start          ();
	 void  __fastcall do_connect        ();
	 bool  __fastcall do_send           (LPMPROTO_HEADER mph);
	 void  __fastcall on_disconnect     ();
	 void  __fastcall on_connect        ();
	 bool  __fastcall send        (LPMPROTO_HEADER mph,DWORD sz);
	 void  __fastcall send_processing   ();
	 void  __fastcall send_keep_alive   ();
	 void  __fastcall on_timer();
	 void  __fastcall handle_network_events();
	 void  __fastcall handle_connect    (DWORD err);
	 void  __fastcall handle_disconnect (DWORD err);
	 void  __fastcall handle_recv       (DWORD err);
	 int   __fastcall read_from_socket  ();
	 WORD  __fastcall get_packet_num    ()  { while(0 == (++pkt_num)); return pkt_num;}
	 void  __fastcall request_discrete  ();
	 void  __fastcall request_alarm_list();
	 int   __fastcall do_handle_recv   (LPMPROTO_HEADER mph);
	 int   __fastcall handle_cpc152_scans  (lpcpc152proto_hdr phdr,bool is_end,bool is_respond);
	 int   __fastcall handle_discrete_scan (lpcpc152scan_data sd,bool is_respond);
	 int   __fastcall handle_analog_scan   (lpcpc152scan_data sd);
	 void  __fastcall recalc_analog_params (lpanalog_ext_t  aext,sotd_proto & sop,DWORD &ch_base,DWORD &ch_ext);
        double __fastcall correct_analog_value(double otd_value);
	 void  __fastcall check_analog_seq_number(int sqn);
     void  __fastcall alarm_starts_send_all();
     void  __fastcall handle_otd_query(sotd_proto & sop);
     void  __fastcall message_end_alarm_recv(QWORD * alarm_times);

	 int   __fastcall handle_time_sync     (LPMPROTO_HEADER mph,lpcpc152proto_hdr phdr);
	 int   __fastcall handle_alarms_list   (lpcpc152proto_hdr phdr,bool is_end,bool is_new);
	 int   __fastcall handle_alarm_file    (lpcpc152proto_hdr phdr,bool is_end);
	 sotd_proto*  __fastcall handle_alarm_discrete_scan (QWORD alarm_id,lpcpc152scan_data sd,bool end);
	 sotd_proto*  __fastcall handle_alarm_analog_scan   (QWORD alarm_id,lpcpc152scan_data sd,bool end);
			void  __fastcall request_alarm_archive      (QWORD alarm_id,bool erase);
			void  __fastcall handle_alarm_params(int count,lpotd_alarm_param ap);
			void  __fastcall handle_alarm_param (lpotd_alarm_param ap);
            void  __fastcall alarm_start_send   (LPCPC152_ALARM_START src,bool erase);
            bool  __fastcall analog_calc_start_thread         ();
            void  __fastcall analog_calc_stop_thread          ();

            int              analog_params_calc_thread_proc();
            void  __fastcall __exec_dft(LPCPC152_ANALOG_PARAM ap,int scan_to_recalc,double phase_correct,bool make_dft);
            const char  * __fastcall get_gkthread_name(){return "cpc152dev_line thread";}

  public   :
  cpc152dev_line(DWORD ln);
  DWORD    __fastcall config_get(LPCPC152_LINE_CONFIG lc);
  DWORD    __fastcall config_set(LPCPC152_LINE_CONFIG lc);
  Tcpc152controller * __fastcall get_cpc152controller();
  void      __fastcall do_report(DWORD type,char    * descr );
  void      __fastcall do_report(DWORD type,wchar_t * descr );
  void      __fastcall Terminate(){terminate_event.SetEvent(true);KeRTL::TThread::Terminate();}

  void      __fastcall setup_analog_calc_params();

  void      __fastcall alarm_start_write(KeRTL::TStream & s);
  void      __fastcall alarm_start_read (KeRTL::TStream & s);
  bool      __fastcall alarm_start_enum (DWORD idx,LPCPC152_ALARM_START dst);
  bool      __fastcall alarm_start_add  (LPCPC152_ALARM_START src);
  bool      __fastcall alarm_start_del  (LPCPC152_ALARM_START src);
  void      __fastcall alarm_start_clear_external(LPCPC152_ALARM_START src);

  void      __fastcall dft_enabled_write(KeRTL::TStream & s);
  void      __fastcall dft_enabled_read (KeRTL::TStream & s);


  static int __fastcall devgrp2otdgroup(bool discrete ,int dev_num,int grp_num);
  static int __fastcall otdgroup2devgrp(bool discrete ,int otd_group,int & dev_num,int & grp_num);
        void __fastcall start_write_oscilogramm(DWORD len);
        bool __fastcall get_analog_stats       (LPCPC152_ANALOG_STATS stats);
        bool __fastcall enum_analogs           (LPCPC152_ANALOG_STATS stats);
        bool __fastcall get_dft_enabled(LPCPC152_DFT_ENABLED data);
        bool __fastcall set_dft_enabled(LPCPC152_DFT_ENABLED data);

};

class Tcpc152controller : public TModemBase
{
  protected:
  DWORD              rep_id;
  CPC152DEV_CONFIG   dev_cfg;
  TFASTMutex         otd_locker;
  otd_storage        otd_data;
  void             __fastcall free_line          (modem_line * ml) ;
  DWORD            __fastcall get_modem_flags()  {return  MPF_DATA_SOURCE;}
  int              __fastcall convert_rx_data    (LPWORD fa,LPBYTE in,int in_len,LPBYTE out,DWORD out_sz) ;
  DWORD            __fastcall get_lines_limit()  {return MODEM_ADDR_MAXVALUE;}
  DWORD            __fastcall get_config_data    (DWORD mask,LPVOID buf,DWORD bsz);
  bool             __fastcall set_config_data    (DWORD mask,LPVOID buf,DWORD bsz);
  bool             __fastcall compare_config_data(DWORD mask,LPVOID buf,DWORD bsz,LPDWORD ch_mask,BOOL * restart);
  bool             __fastcall read_settings();
  bool             __fastcall write_settings();
  cpc152dev_line * __fastcall get_cpc152controller_line(DWORD dev_num);

  BOOL    __fastcall can_start(DWORD reason,LPARAM p2);
  DWORD   __fastcall start    (DWORD reason,LPARAM p2);
  DWORD   __fastcall stop     (DWORD reason);
  LRESULT __fastcall processing         (DWORD cmd,LPARAM p1,LPARAM p2);
  DWORD   __fastcall get_window_module  (wchar_t * buf,DWORD bsz);

  void    __fastcall do_recv_puname();
  DWORD   __fastcall check_license(){return MOD_LICENSE_REGISTERED; }
  void    __fastcall read_lines (KeRTL::TMemoryStream & ms);
  void    __fastcall write_lines(KeRTL::TMemoryStream & ms);

  DWORD   __fastcall line_add(LPCPC152_LINE_CONFIG lc);
  DWORD   __fastcall line_get(LPCPC152_LINE_CONFIG lc);
  DWORD   __fastcall line_upd(LPCPC152_LINE_CONFIG lc,DWORD old_num);
  DWORD   __fastcall line_del(DWORD line_num);
  LRESULT __fastcall alarm_start_enum(DWORD ln,LPCPC152_ALARM_START ast);
  LRESULT __fastcall alarm_start_add (DWORD ln,LPCPC152_ALARM_START ast);
  LRESULT __fastcall alarm_start_del (DWORD ln,LPCPC152_ALARM_START ast);
  LRESULT __fastcall get_analog_stats(DWORD ln,LPCPC152_ANALOG_STATS  sts);
  LRESULT __fastcall enum_analogs    (DWORD ln,LPCPC152_ANALOG_STATS  sts);
  LRESULT __fastcall get_line_dft_enabled(DWORD ln ,LPCPC152_DFT_ENABLED data);
  LRESULT __fastcall set_line_dft_enabled(DWORD ln ,LPCPC152_DFT_ENABLED data);
  void    __fastcall reg_reports();
  LRESULT __fastcall send        (LPMPROTO_HEADER mph,DWORD sz);
  public:
  Tcpc152controller ();
  void        __fastcall do_report(DWORD evt_type,wchar_t * rep_text,LPVOID data,DWORD dt_sz);
  WORD        __fastcall get_type ()  {return MT_MODEM_CPC152;}
  DWORD       __fastcall get_mem_used();
  void        __fastcall do_report    (DWORD type,char    * descr);
  void        __fastcall do_report    (DWORD type,wchar_t * descr);
  int         __fastcall create_group (sotd_proto & sop,bool discrete,int cp_num,int group_num,int param_count,int ext_size = 0,bool float_type = false);
  TFASTMutex& __fastcall otd_get_locker(){return otd_locker;}
  void        __fastcall otd_lock      (){otd_locker.Lock  ();}
  void        __fastcall otd_unlock    (){otd_locker.Unlock();}
  void        __fastcall otd_set_diag  (BYTE cp_number,DWORD diag);
  WORD        __fastcall get_pu_number (){TLockHelper l(locker); return dev_cfg.pu_number;}
  int         __fastcall get_otd_range (otd_addr & addr, proto_pointer & ptr_beg,proto_pointer & ptr_end);
  LRESULT     __fastcall start_write_oscilogramm(int lnum , int lenght);

};

inline   Tcpc152controller * __fastcall cpc152dev_line::get_cpc152controller()
{
    return dynamic_cast<Tcpc152controller *>(owner);
}

inline  void       __fastcall Tcpc152controller::do_report(DWORD type,char   * descr)
{
   report(rep_id,type,descr);
}

inline  void       __fastcall Tcpc152controller::do_report(DWORD type,wchar_t * descr )
{
   report(rep_id,type,descr);
}

inline int __fastcall cpc152dev_line::devgrp2otdgroup(bool discrete ,int dev_num,int grp_num)
{
   if(discrete)
      return dev_num*CPC152_MAX_GROUP_DISCRETE + grp_num;
   return  dev_num*CPC152_MAX_GROUP_ANALOG + grp_num;
}

inline int __fastcall cpc152dev_line::otdgroup2devgrp(bool discrete ,int otd_group,int & dev_num,int & grp_num)
{
   int divider = discrete ? CPC152_MAX_GROUP_DISCRETE : CPC152_MAX_GROUP_ANALOG;
   dev_num  = otd_group/divider;
   grp_num  = otd_group%divider;
   return 0;
}

#endif
