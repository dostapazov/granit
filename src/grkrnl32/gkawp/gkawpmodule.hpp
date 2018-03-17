#ifndef __GKAWP_MODULE__
#define __GKAWP_MODULE__

#define BOOST_THREAD_USE_LIB
#include <boost/thread.hpp>
#include <boost/bind.hpp>

#ifdef BOOST_LOCKER
#include <boost/thread/locks.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/recursive_mutex.hpp>
#endif


#include <windows.h>
#include <gkmodules.hpp>
#include <modemmer.h>
#include <modem_proto.h>
#include <otdm_proto.h>
#include <vcl.h>



#include <map>
#include <kadrs_table.hpp>
#include <stdlib_utils.hpp>
#include <kedatas.hpp>
#include <gksecur_proto.h>


#define REGDW_MODEM_SOURCE        L"modem_source"
#define REGSTR_DATA_DIR           L"data_dir"
#define REGSTR_COMTRADE_DIR       L"comtrade_data"
#define REGSTR_COMTRADE_VIEWER    L"comtrade_viewer"
#define REGDW_HOTKEY_RECEIPT_ALL  L"hot_key_receipt"
#define REGDW_HOTKEY_CLEAR_ALL    L"hot_key_clear"
#define REGDW_PERIOD_RECALL       L"period_recall"
#define REGSTR_LAYOUT_NAME        L"layout"
#define REGSTR_SOUND_ALARM        L"snd_alarm"
#define REGSTR_SOUND_DANGER       L"snd_danger"
#define REGSTR_SOUND_DISPARITY    L"snd_disparity"
#define REGSTR_SOUND_BACK         L"snd_back"

#define REGDW_READY_MONITOR       L"ready_monitor"
#define REGDW_READY_STABLE_TIMER  L"ready_stable_timer"
#define REGDW_READY_SPACE         L"ready_space"
#define REGDW_READY_EQUAL_SIZE    L"ready_equal_size"
#define REGBIN_READY_COLORS       L"ready_colors"
#define REGBIN_READY_FONT         L"ready_font"
#define REGBIN_READY_STATE_FONT   L"ready_state_font"
#define REGDW_FLASH_PERIOD        L"flash_period"

#define CFGM_MODEM_SOURCE         0x000001
#define CFGM_DATA_DIR             0x000002
#define CFGM_COMTRADE_DIR         0x000004
#define CFGM_COMTRADE_VIEWER      0x000008
#define CFGM_HOTKEY_RECEIPT_ALL   0x000010
#define CFGM_HOTKEY_CLEAR_ALL     0x000020
#define CFGM_PERIOD_RECALL        0x000040
#define CFGM_LAYOUT_NAME          0x000080
#define CFGM_SOUND_ALARM          0x000100
#define CFGM_SOUND_DANGER         0x000200
#define CFGM_SOUND_DISPARITY      0x000400
#define CFGM_SOUND_BACK           0x000800

#define CFGM_READY_MONITOR        0x001000
#define CFGM_READY_STABLE_TIMER   0x002000
#define CFGM_READY_SPACE          0x004000
#define CFGM_READY_EQUAL_SIZE     0x008000
#define CFGM_READY_COLORS         0x010000
#define CFGM_READY_FONT           0x020000
#define CFGM_READY_STATE_FONT     0x040000
#define CFGM_FLASH_PERIOD         0x080000

#define KADRS_FILE_NAME       L"kadrs.data"
#define NAMES_FILE_NAME       L"names.data"
#define RECORDS_FILE_NAME     L"records.data"
#define PAINTVARS_FILE_NAME   L"paintvars.data"

#define KADR_IMAGES_PATH      L"kadr_images"
#define PAINTER_IMAGES_PATH   L"painter_images"


#define CCFL_PARAMS    0x0001000
#define CCFL_KADRS     0x0002000
#define CCFL_NAMES     0x0004000
#define CCFL_RECORDS   0x0008000
#define CCFL_PAINTVARS 0x0010000


#define WNDCM_HANDLE_ENUM_KADRS  (WM_USER+1000)

#pragma pack(push,1)

struct TAWParameters
{
  modem_addr      modem_source;
  UnicodeString   data_folder;
  UnicodeString   comtrade_export_folder;
  UnicodeString   comtrade_viewer_app;
  DWORD           hot_key_receipt_all;
  DWORD           hot_key_clear_all;
  DWORD           recall_period;
  UnicodeString   layout_name;

  UnicodeString   sound_alarm;
  UnicodeString   sound_danger;
  UnicodeString   sound_disparity;
  UnicodeString   sound_back;

  DWORD           ready_monitor;
  DWORD           ready_stable_timer;
  DWORD           ready_space;
  DWORD           ready_equal_size;
  READY_COLORS    ready_colors;
  LOGFONTW        ready_font;
  LOGFONTW        ready_state_font;
  int             flash_period;
};

#pragma pack(pop)



class TAwpModuleParams;
class TAwpMainForm;
class TTlmKadrForm;

struct Thist_times:public OTDM_KADR_ARCHIVE
{
 Thist_times(bool _alarm,__int64  _hist_beg_time,__int64  _hist_end_time,bool _protect = false,char * aname = NULL)
 {
  alarms = (_alarm);
  beg_time = (_hist_beg_time);
  end_time = (_hist_end_time);
  protect  = (_protect);
  if(aname && *aname)
   strncpy(arch_name,aname,sizeof(arch_name));
   else
   memset(arch_name,0,sizeof(arch_name));
 }
 Thist_times(const OTDM_KADR_ARCHIVE& src){*this = src;}
 Thist_times & operator = (const Thist_times& src)
 {
  memcpy(this,&src,sizeof(*this));
  return * this;
 }
};

typedef std::vector<Thist_times> hist_times_t;



class kadr_t:public single_kadr
{
  private:
  KeRTL::TFile   file;
  int            stable_timer;
  int            recall_timer;
  hist_times_t   hist_times;
  public:
  DWORD          old_changes_count;
  DWORD          new_changes_count;
  kadr_t():single_kadr()        {kadr.kadr_id = 0;kadr.kadr_diag = 0;stable_timer = 0;recall_timer = 0;old_changes_count = 0;new_changes_count = 0;}
  kadr_t(const MDB_KADR & _kadr){kadr = _kadr;stable_timer = 0;recall_timer = 0;old_changes_count = 0;new_changes_count = 0;}
  ~kadr_t(){if(user_data) delete get_kadr_form();}
  TTlmKadrForm * __fastcall get_kadr_form() {return reinterpret_cast<TTlmKadrForm *>(user_data);}
  KeRTL::TFile  & get_file(){return file;}

  void __fastcall stable_timer_set(int msec){stable_timer = msec; ready_state &=~ rds_stable_ready;}
  bool __fastcall stable_timer_dec(int msec){if(stable_timer>0){ stable_timer-=abs(msec);if(stable_timer<=0){ready_state |=rds_stable_ready; return true;}  } return false; }
  void __fastcall recall_timer_set(int msec){recall_timer = msec;}
  bool __fastcall recall_timer_dec(int msec){if(recall_timer >0 ) {recall_timer -= abs(msec); if(recall_timer<=0)return  true;} return false;}

  hist_times_t & __fastcall get_hist_times(){return hist_times;}
  void           __fastcall push_hist_times(bool alarm,__int64 beg_tm,__int64 end_tm)
  {   hist_times.push_back(Thist_times(alarm,beg_tm,end_tm));}

};


typedef mdb_record  record_t;
typedef std::vector<record_t>          records_t;
//typedef single_kadr kadr_t;
typedef std::vector<kadr_t*>           kadrs_t;
typedef std::map<DWORD,UnicodeString>  names_t;
typedef std::map<DWORD,DWORD>          changes_map_t;
typedef std::vector<mdb_paint_variant> paint_variants_t;



class TGkAwpModule:public TGKModule
{
  public :
  #ifdef BOOST_LOCKER
  typedef boost::recursive_mutex        mutex_type;
  typedef boost::lock_guard<mutex_type> locker;
  #else
  typedef KeRTL::TFASTMutex             mutex_type;
  typedef KeRTL::TLockHelper            locker;
 #endif

  private:
  DWORD     paint_context ;
  static    KeRTL::THeap  mod_heap;
  static    DWORD         mem_used;
  TAwpModuleParams  *     mod_params_wnd;
  TAwpMainForm      *     main_form;
  LONG                    pkt_num;
  int                     int_line;
  TGKModuleInterface      modemmer;
  TAWParameters           params;
  BOOL                    is_connected;

  mutex_type              mut;
  names_t                 names;
  records_t               records;
  kadrs_t                 kadrs;
  kadr_t                  main_kadr;
  paint_variants_t        paint_vars;

  DWORD                   ready_state;
  changes_map_t           record_changes;//Список изменений записей
  changes_map_t           kadr_changes;  //Список изменений кадров
  int                     flash_timer;
  DWORD                   flash_counter;

  BOOL                    history_present; //Наличие на сервере модуля архивных данных
  BOOL                    paint_vars_requested;

  KeRTL::TEvent    *      thread_term_event;
  DWORD   __fastcall       thread_term_event_create();


  LONG                    thread_term_request;
  boost::thread           timer_thread;

  TEventSimpleDataQueue  *rx_queue;
  boost::thread           rx_thread;
  int                     rx_thread_proc   ();

  int                     timer_thread_proc();
  void    __fastcall      do_timer_work    (int period);
  void                    do_timer_kadr    (int period);

  DWORD                   disparity_count;
  DWORD                   back_count     ;
  DWORD                   alarms_count   ;
  DWORD                   danger_count   ;

  SESSION_ID              local_ssid;
  SESSION_ID              remote_ssid;

  int                     sound_thread_proc     ();
  bool         __fastcall do_play_alarm_sound   ();
  bool         __fastcall do_play_danger_sound  ();
  bool         __fastcall do_play_disparty_sound();
  bool         __fastcall do_play_back_sound    ();

  boost::thread           sound_thread;



  protected:
            void    __fastcall load_paint_dll();
  virtual   void    __fastcall release      (LPVOID);
  virtual   DWORD   __fastcall check_license(){return MOD_LICENSE_REGISTERED; }
  virtual   WORD    __fastcall get_type(){return MT_TLM_WORK_PLACE;}
  virtual   DWORD   __fastcall get_config_data(DWORD mask,LPVOID buf,DWORD bsz);
  virtual   bool    __fastcall set_config_data(DWORD mask,LPVOID buf,DWORD bsz);
  virtual   bool    __fastcall compare_config_data(DWORD mask,LPVOID buf,DWORD bsz,LPDWORD ch_mask,BOOL * restart);
            void    __fastcall clear_all_tables();
            void    __fastcall _clear_kadrs    ();
            void    __fastcall create_folders  ();

  virtual   bool    __fastcall read_settings  ();
            bool    __fastcall _read_params   ();
            bool    __fastcall _read_kadrs    ();
            bool    __fastcall _read_names    ();
            bool    __fastcall _read_records  ();
            bool    __fastcall _read_paintvars();

  virtual   bool    __fastcall write_settings  ();
            bool    __fastcall _write_params   ();
            bool    __fastcall _write_kadrs    ();
            bool    __fastcall _write_names    ();
            bool    __fastcall _write_records  ();
            bool    __fastcall _write_paintvars();

            void    __fastcall init_ready_colors();

  virtual   LRESULT __fastcall processing            (DWORD cmd,LPARAM p1,LPARAM p2);
  virtual   LRESULT __fastcall on_notify_code        (LPNOTIFY_CODE nc,DWORD mask);

            void    __fastcall _increment_sounds     (record_t & dst_rec);
            DWORD   __fastcall _copy_record_compare  (record_t & dst_rec,const LPMDB_RECORD  src_rec,DWORD mask);
            LRESULT __fastcall handle_recv           ( LPMPROTO_HEADER mph     );
            LRESULT __fastcall handle_security       (MODEM_ADDR & from, LPOTDM_PROTO_HEADER omph, DWORD data_size);
            LRESULT __fastcall handle_otdm           (MODEM_ADDR & from, LPOTDM_PROTO_HEADER omph, DWORD data_size);
            LRESULT __fastcall handle_history        (MODEM_ADDR & from, LPOTDM_PROTO_HEADER omph, DWORD data_size);
            LRESULT __fastcall handle_enum_kadrs     (MODEM_ADDR & from, OTDM_PROTO_STATE & ps);
            void    __fastcall handle_name           (OTDM_PROTO_STATE & ps,bool long_name);
            void    __fastcall handle_get_records    (OTDM_PROTO_STATE & ps);
            void    __fastcall handle_records        (OTDM_PROTO_STATE & ps);
            void    __fastcall handle_get_kadr       (OTDM_PROTO_STATE & ps);
            void    __fastcall handle_kadr_state     (kadr_t * kadr ,LPOTDM_PROTO_KADR  pkadr);
            void    __fastcall handle_kadr_entryes   (OTDM_PROTO_STATE & ps);
            void    __fastcall handle_kadr_image     (OTDM_PROTO_STATE & ps);
            void    __fastcall handle_paint_image    (OTDM_PROTO_STATE & ps);
            void    __fastcall handle_paint_variants (OTDM_PROTO_STATE & ps);
            void    __fastcall handle_tu_operation   (OTDM_PROTO_STATE & ps);

            bool    __fastcall file_image_begin      (KeRTL::TFile & file,bool paint_image,LPOTDM_PROTO_IMAGE_BEGIN image_begin);
            void    __fastcall painter_dll_setup     ();

            void    __fastcall handle_db_change      ();
            void    __fastcall handle_record_changes ();
            void    __fastcall handle_record_depended(DWORD rec_id,DWORD mask);
            void    __fastcall handle_kadrs_changes  ();

            void    __fastcall request_record_names   (DWORD rec_id,bool short_name,bool long_name);
            void    __fastcall request_kadr_image     (DWORD kadr_id);
            void    __fastcall request_paint_variants ();

            DWORD   __fastcall kadr_scan_ready    (kadr_t * kadr,bool update);
            void    __fastcall _kadr_set_bad_ready(kadr_t * kadr);

            void    __fastcall show_main_form();
  virtual   DWORD   __fastcall start         (DWORD reason,LPARAM p2);
  virtual   DWORD   __fastcall stop          (DWORD reason);
  virtual   DWORD   __fastcall get_window_module  (wchar_t * buf,DWORD bsz);
            void    __fastcall param_window_close (TObject * sender,TCloseAction & ca);
            void    __fastcall param_change_orders();
            void    __fastcall remove_kadr_order  (DWORD kadr_id);


            void    __fastcall kadr_selected      (TObject * sender,bool active);
            void    __fastcall kadr_activate      (TObject * sender){kadr_selected(sender,true) ;}
            void    __fastcall kadr_deactivate    (TObject * sender){kadr_selected(sender,false);}
            void    __fastcall kadr_close         (TObject * sender,TCloseAction & ca);


            bool    __fastcall search_kadr_t      (const DWORD kadr_id,kadrs_t::iterator   * kptr);
            bool    __fastcall search_record_t    (const DWORD rec_id ,records_t::iterator * rptr);
            void    __fastcall _clear_record      (record_t & rec,bool single);
            void    __fastcall _receipt_record    (record_t & rec,bool single);

            void    __fastcall on_source_connect      (bool connect);
            bool    __fastcall check_source_connected ();

            /*layouts*/
    UnicodeString   __fastcall get_layout_file(UnicodeString layout_name );
            void    __fastcall do_save_layout (UnicodeString layout_name );
            void    __fastcall do_open_layout (UnicodeString layout_name );
            void    __fastcall enum_layouts   (TStrings * strs);


  public:
    TGkAwpModule();
    mutex_type & __fastcall get_mutex     () {return mut;}
          bool   __fastcall open_modemmer ();
          bool   __fastcall close_modemmer();
         bool    __fastcall is_source_connected(){return is_connected;}

          HICON  __fastcall get_module_icon    (bool small = false);
          void   __fastcall send_order_command (DWORD kadr_id,bool order);
          DWORD  __fastcall send_command       (modem_addr addr_to,DWORD fa,bool begin,bool finish,DWORD cmd,DWORD err,LPVOID data,DWORD data_size );
          DWORD  __fastcall send_command       (DWORD fa,bool begin,bool finish,DWORD cmd,DWORD err,LPVOID data,DWORD data_size );
          DWORD  __fastcall send_data          (LPMPROTO_HEADER mph);
          DWORD  __fastcall send_login_request (UnicodeString user_name,UnicodeString passw);
          void   __fastcall param_window_show  ();
          bool   __fastcall get_string   (const DWORD str_id,      UnicodeString & str) const;
 UnicodeString   __fastcall _get_string  (const DWORD str_id) const;
          int    __fastcall set_string   (const DWORD str_id,const UnicodeString  str );
          bool   __fastcall get_record   (const DWORD rec_id,      record_t      & rec) const;
 UnicodeString   __fastcall get_kadr_image_file(single_kadr * sk,bool no_request = false);
 UnicodeString   __fastcall get_kadr_image_file   (UnicodeString file_name);
 UnicodeString   __fastcall get_painter_image_file(UnicodeString file_name);
 UnicodeString   __fastcall make_module_file_name (UnicodeString fname,UnicodeString folder);
 TTlmKadrForm *  __fastcall open_kadr_form        (DWORD kadr_id,bool show);
          void   __fastcall kadr_reset_changes   (kadr_t * kadr);
          void   __fastcall kadrs_reset_changes  ();
          void   __fastcall close_all_kadrs       (bool force);


          void   __fastcall kadr_order   (kadr_t * kadr,bool order);
          void   __fastcall kadrs_order  (bool order);
          void   __fastcall kadrs_repaint();

          DWORD  __fastcall get_kadr_recall_period();
          bool   __fastcall is_kadr_ordered (LPMODEM_ADDR ma,DWORD kadr_id);
  single_kadr  * __fastcall get_kadr        (DWORD kadr_id);
          void   __fastcall show_record_info(const DWORD rec_id,bool _set = true);
          void   __fastcall clear_all       ();
          void   __fastcall receipt_all     ();
          void   __fastcall clear_record    (DWORD rec_id,bool single);
          void   __fastcall receipt_record  (DWORD rec_id,bool single);
 UnicodeString   __fastcall get_comtrade_export_folder();
 UnicodeString   __fastcall get_comtrade_export_viewer();
          void   __fastcall get_ready_colors(READY_COLORS & rdy_colors);
     modem_addr  __fastcall get_modem_source();

          DWORD  __fastcall make_kadr_selected_rec_ids(single_kadr * kadr,records_ids_t & rlist);

          void   __fastcall history_check_present();
          bool   __fastcall history_is_present   (){return history_present;}
          void   __fastcall history_set_present  (bool present);

          bool   __fastcall get_opened_kadrs (kadrs_t & kadrs);
          void   __fastcall request_history  (QWORD hist_id,QWORD tm_beg,QWORD tm_end,DWORD alarm,records_ids_t &  rids_list);
          void   __fastcall set_config_changes(DWORD mask);
          void   __fastcall show_all_windows(bool show);
          DWORD  __fastcall kadrs_scan_ready   ();
          void   __fastcall kadrs_set_bad_ready();

          bool   __fastcall is_source_addr(MODEM_ADDR & from );

/*Отладочные функции*/
         void   __fastcall dbg_set_all_records_state(DWORD state,bool set);


  static  void * __fastcall alloc_mem  (size_t sz );
  static  void   __fastcall free_mem   (void * ptr);
  static  void   __fastcall get_kadr_names(LPOTDM_PROTO_KADR pkadr,UnicodeString & short_name,UnicodeString & long_name);
  static  UnicodeString __fastcall get_time_text(__int64 tmval,char time_sep,bool msec);
  static  UnicodeString   __fastcall get_state_tu_text      (DWORD state);
  static  UnicodeString   __fastcall get_state_text         (DWORD state);
  static  UnicodeString   __fastcall get_state_view_text    (DWORD state);
  static  DWORD           __fastcall get_record_ready_state (const mdb_record &rec,LPDWORD  changes_count);

};

inline UnicodeString   __fastcall TGkAwpModule::get_painter_image_file(UnicodeString fname)
 {
   return make_module_file_name(fname,PAINTER_IMAGES_PATH);
 }

inline UnicodeString   __fastcall TGkAwpModule::get_kadr_image_file(UnicodeString fname)
 {
   return make_module_file_name(fname,KADR_IMAGES_PATH);
 }

 inline UnicodeString   __fastcall TGkAwpModule::get_comtrade_export_folder()
 {
   UnicodeString str;
   {
   locker l(mut);
   str = params.comtrade_export_folder;
   }
   if(!str.IsEmpty()) str+=L'\\';
   return str;
 }

 inline UnicodeString   __fastcall TGkAwpModule::get_comtrade_export_viewer()
 {
   locker l(mut);
   return params.comtrade_viewer_app;
 }

inline   void   __fastcall TGkAwpModule::get_ready_colors(READY_COLORS & rdy_colors)
{
   locker l(mut);
   memcpy(&rdy_colors,&params.ready_colors,sizeof(READY_COLORS));
}

inline DWORD  __fastcall TGkAwpModule::send_command (DWORD fa,bool begin,bool finish,DWORD cmd,DWORD err,LPVOID data,DWORD data_size )
 {
    modem_addr ma = get_modem_source();
    return send_command(ma,fa,begin,finish,cmd,err,data,data_size);
 }

inline DWORD  __fastcall TGkAwpModule::make_kadr_selected_rec_ids(single_kadr * kadr,records_ids_t & rlist)
 {
   if(kadr)
   {
      locker l(mut);
      return kadr->make_selected_rec_ids(rlist);
   }
   return 0;
 }

inline DWORD  __fastcall TGkAwpModule::get_kadr_recall_period()
{
   locker l(mut);
   return params.recall_period*1000;
}

inline modem_addr  __fastcall TGkAwpModule::get_modem_source()
{
   locker l(mut);
   modem_addr ret = params.modem_source;
   if(!ret.modem) ret.line = 0;
   return ret;
}

inline void   __fastcall TGkAwpModule::set_config_changes(DWORD mask)
{
   locker l(mut);
   cc_flags|=mask;
}

inline bool __fastcall TGkAwpModule::is_source_addr(MODEM_ADDR & from )
{
  locker l(mut);
  return (from.modem == params.modem_source.modem && (!from.modem || from.line == params.modem_source.line)) ? true : false;
}




#endif
