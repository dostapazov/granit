#ifndef _GKTLM_VIEWER_MOD_HPP_
#define _GKTLM_VIEWER_MOD_HPP_

#include <windows.h>
#include "tlmpainter.hpp"
#include <vcl.h>

#pragma hdrstop

#ifdef BOOST_LOCKER
#define BOOST_THREAD_USE_LIB
#include <boost/thread/locks.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/recursive_mutex.hpp>
#endif

#include <gkmodules.hpp>
#include <kedatas.hpp>


#include <otdm_proto.h>
#include <modem_proto.h>
#include <modemmer.h>
#include <mediumdb.h>
#include <vector>
#include <map>
#include <string>

#include "gktlm_viewer.h"
#include "gktlm_viewer_form.h"
#include "tlm_viewer_main_form.h"




class TTlmViewerMainForm;
class TTlmViewerParams;

class TGKTlmViewer:public TGKModule,public TGKThread
{
 public :
 #ifdef BOOST_LOCKER
 typedef boost::recursive_mutex        mutex_type;
 typedef boost::lock_guard<mutex_type> locker;
 #else
 typedef KeRTL::TFASTMutex             mutex_type;
 typedef KeRTL::TLockHelper            locker;
 #endif

 protected:
  static LONG        total_alloc;
  mutex_type         mut;

  Tviewer_parameters parameters;

  Trecords        records;
  Tnames          names;
  Tkadrs          kadrs_ordered;
  Tkadrs          kadrs_avail;
  //Tkadr_entryes   kadr_entryes;
  Tpaint_variants paint_vars;
  Trecord_changes changed_records;

  LONG            discrete_disparity_count;
  LONG            discrete_back_count;

  TTlmViewerMainForm  * main_form;
  TTlmViewerParams    * params_wnd;

  TGKModuleInterface    modemmer;
  int                   int_line;
  LONG                  pkt_num;
  TEventSimpleDataQueue data_queue;
  KeRTL::TEvent         term_event;

  virtual   WORD    __fastcall get_type       () {return MT_TLMVIEWER;}
  virtual   void    __fastcall release(LPVOID);

  virtual   DWORD   __fastcall get_config_data(DWORD mask,LPVOID buf,DWORD bsz);
  virtual   bool    __fastcall set_config_data(DWORD mask,LPVOID buf,DWORD bsz);
  virtual   bool    __fastcall read_settings  ();
  virtual   bool    __fastcall write_settings ();
  virtual   bool    __fastcall compare_config_data(DWORD mask,LPVOID buf,DWORD bsz,LPDWORD ch_mask,BOOL * restart);
  virtual   DWORD   __fastcall get_mem_used();
  virtual   BOOL    __fastcall can_start(DWORD reason,LPARAM p2);
  virtual   BOOL    __fastcall can_stop (DWORD reason);
  virtual   DWORD   __fastcall start(DWORD reason,LPARAM p2);
  virtual   DWORD   __fastcall stop (DWORD reason);
  virtual   LRESULT __fastcall processing    (DWORD cmd,LPARAM p1,LPARAM p2);
            LRESULT __fastcall TGKTlmViewer::processing_input_proto(LPMPROTO_HEADER mph);



  virtual   bool    __fastcall BeforeExecute  ();
  virtual   int     __fastcall Execute        ();
  virtual   void    __fastcall BeforeTerminate();
            LRESULT __fastcall on_notify_code     (LPNOTIFY_CODE nc,DWORD mask);
            void    __fastcall on_modem_line_state(LPMODEM_LINE_STATE mls     );
  virtual   DWORD   __fastcall check_license(){return MOD_LICENSE_REGISTERED; }
            void    __fastcall release_tables();

 public:
          TGKTlmViewer();
 virtual ~TGKTlmViewer();
 virtual DWORD  __fastcall get_window_module  (wchar_t * buf,DWORD bsz);
         DWORD  __fastcall send_data (LPMPROTO_HEADER mph);
         DWORD  __fastcall send_command(modem_addr ma,DWORD fa,bool begin,bool finish,DWORD cmd,DWORD err,LPVOID data,DWORD data_size );

 TTlmViewerMainForm * __fastcall get_main_form();


  virtual const char  * __fastcall get_gkthread_name(){return "TlmViewer::Thread";}
  void  __fastcall Terminate(){term_event.SetEvent(true); TGKThread::Terminate();};
  void  __fastcall config_change(DWORD mask){ this->cc_flags |= mask;}
  Tviewer_parameters & __fastcall get_params(){return parameters;}
  mutex_type & get_mutex() {return mut;}
  Trecords &   __fastcall   get_records() {return records;}
  Tnames   &   __fastcall   get_names  () {return names; }
  Tkadrs   &   __fastcall   get_kadrs_ordered() {return kadrs_ordered;}
  Tkadrs   &   __fastcall   get_kadrs_avail  () {return kadrs_avail;  }
  DWORD    &   __fastcall   get_ready_color  () {return parameters.ready_color;  }
  Tpaint_variants    &   __fastcall   get_paint_vars(){return  paint_vars;}
  Trecord_changes    &   __fastcall   get_changed_records() {return changed_records;}

  BOOL          __fastcall get_enable_double_buffered() {return parameters.enable_double_buffered;}
  void          __fastcall set_enable_double_buffered(BOOL set) { parameters.enable_double_buffered = set;}
  UnicodeString __fastcall get_data_dir();
           void __fastcall set_data_dir(UnicodeString _ddir) { parameters.data_dir = _ddir.c_str();}

  void   __fastcall clear_kadrs      (Tkadrs & kadrs);
  bool   __fastcall __get_record     (DWORD id ,bool offset, mdb_record_ex & rec);
  bool   __fastcall __get_text       (DWORD id,UnicodeString & name);
  void   __fastcall set_text         (DWORD str_id,UnicodeString s);
  void   __fastcall get_record_text  (DWORD rec_id,UnicodeString & str);
  bool   __fastcall find_record      (DWORD rec_id,Trecords::iterator & ptr);

  TTlmViewerMainForm * __fastcall get_ready_painter() {return main_form;}

   WORD __fastcall get_hot_key_clear_all  ()        {return parameters.hot_key_clear_all;}
   void __fastcall set_hot_key_clear_all  (WORD val){parameters.hot_key_clear_all = val;}
   WORD __fastcall get_hot_key_receipt_all()        {return parameters.hot_key_receipt_all;}
   void __fastcall set_hot_key_receipt_all(WORD val){parameters.hot_key_receipt_all = val;}

   LONG       __fastcall get_discrete_disparity_count();
   LONG       __fastcall get_discrete_back_count();
   void       __fastcall inc_discrete_disparity_count();
   void       __fastcall inc_discrete_back_count();
   modem_addr __fastcall get_modem_source() {return modem_addr(parameters.modem_source);}
   void       __fastcall set_modem_source (MODEM_ADDR & ma){parameters.modem_source.addr = ma.addr;}

   void  __fastcall reset_diag();
   void  __fastcall set_kadr_diag        (Tmdb_kadr_ex * kadr,DWORD diag);

   BOOL __fastcall get_external_source   ()                {return parameters.external_source;}
   void __fastcall set_external_source   (BOOL val)        {parameters.external_source = val; parameters.modem_source.addr = val ? -1 : 0;}

   int  __fastcall get_order_period      ()                {return parameters.order_period;}
   void __fastcall set_order_period      (int val)         {parameters.order_period = val; }

   int  __fastcall get_stable_ready_timer()                {return  parameters.stable_ready_timer;}
   void __fastcall set_stable_ready_timer(int val)         { parameters.stable_ready_timer = val;}
   void __fastcall set_auto_open_layout  (UnicodeString val) {parameters.auto_open_layout = val.c_str();}

   int  __fastcall get_main_form_monitor ()        {return parameters.main_form_monitor;}
   void __fastcall set_main_form_monitor (int val) ;

   int  __fastcall get_ready_spacer      ()        {return parameters.ready_spacer;}
   void __fastcall set_ready_spacer      (int val) {parameters.ready_spacer = val; }


  UnicodeString   __fastcall   get_auto_open_layout()      {return parameters.auto_open_layout.c_str();}
  READY_COLORS &  __fastcall   get_ready_colors ()         {return parameters.ready_colors;}

  UnicodeString  __fastcall   get_snd_back     ()          {return UnicodeString(parameters.snd_back.c_str());     }
  UnicodeString  __fastcall   get_snd_disparity()          {return UnicodeString(parameters.snd_disparity.c_str());}



  bool  __fastcall     read_config      (DWORD mask,HKEY reg_key,wchar_t * config_folder);
  void  __fastcall     read_params (HKEY reg);
  void  __fastcall     read_kadrs  ();
  void  __fastcall     read_names  ();
  void  __fastcall     read_records();

  bool  __fastcall     write_config     (DWORD mask,HKEY reg_key,wchar_t * config_folder);
  void  __fastcall     write_params (HKEY reg);
  void  __fastcall     write_kadrs    ();
  void  __fastcall     write_names    ();
  void  __fastcall     write_records  ();

  void  __fastcall     show_parameters();
  void  __fastcall     params_close   (TObject * Sender,TCloseAction &Action);
  void  __fastcall     request_kadr_list(modem_addr ma );
};

inline LONG __fastcall TGKTlmViewer::get_discrete_disparity_count()
{
  return InterlockedExchange(&discrete_disparity_count,0);
}

inline LONG __fastcall TGKTlmViewer::get_discrete_back_count()
{
  return InterlockedExchange(&discrete_back_count,0);
}

inline void __fastcall TGKTlmViewer::inc_discrete_disparity_count()
{
  InterlockedIncrement(&discrete_disparity_count);
}

inline void __fastcall TGKTlmViewer::inc_discrete_back_count()
{
  InterlockedIncrement(&discrete_back_count);
}


#endif

