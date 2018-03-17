/*
 Интерфейсы классов для  реализации системной централи "Гранит"
*/

#ifndef _GRANIT_HPP_INC
#define _GRANIT_HPP_INC
#include "granit_modem.hpp"
#include <kestrm.hpp>
#include <vector>
#include <map>


#define GRSC_TIMER_RESOLUTION  200

using namespace KeRTL;
class gr_cp;

class sc_floor :public modem_line,protected TGKThread
{
 protected:

    DWORD             floor_flags;

    TEvent            term_event;
    TEvent            floor_rx_event;
    TWaitableTimer    floor_timer;
    TEvent            send_event;
    TSynchroSet       synchro_set;
    TSimpleDataQueue  floor_rx_queue; // Очередь данных на обработку

    TModemDataQueue   send_queue;    // Очередь входящих данных

    gr_cp *           cp_list[GRSCFLOOR_MAX_CPCOUNT];



    DWORD     __fastcall  get_max_frame_size() {return 21;}
    DWORD     __fastcall  get_tx_queue_size () {return 0 ;}
    DWORD     __fastcall  get_line_flags    () {return MLPF_RXABLE|MLPF_TXABLE;}

    DWORD     __fastcall  get_line_text     (wchar_t * text,DWORD text_sz) ;
    bool      __fastcall  do_start ();
    bool      __fastcall  do_stop  ();
    void      __fastcall  on_connect();
    void      __fastcall  on_disconnect();
    bool      __fastcall  send     (LPMPROTO_HEADER mph,DWORD sz);
    DWORD     __fastcall  refresh(DWORD sbl);

    void      __fastcall  set_diag(DWORD df,bool set);
    void      __fastcall  Terminate();
    int       __fastcall  Execute();
    void      __fastcall  BeforeTerminate();
    bool      __fastcall  BeforeExecute  ();
    const char  * __fastcall get_gkthread_name(){return "SCFloor Thread";}

    void      __fastcall  handle_floor_timer();
    void      __fastcall  handle_floor_recv ();
    void      __fastcall  handle_floor_send ();
    void      __fastcall  handle_cp_recv(gr_proto & gp,DWORD proto_error);
    gr_cp *   __fastcall  find_cp(DWORD number,bool create);
    bool      __fastcall  add_cp(gr_cp * cp);

    virtual
    void      __fastcall  release();
    DWORD     __fastcall  OnException(DWORD ExceptionCode,LPEXCEPTION_POINTERS);


 public:
    sc_floor   (DWORD number);
    virtual ~sc_floor(){release();}
    void     __fastcall floor_rx     (LPBYTE data,DWORD len);
    bool     __fastcall floor_tx     (LPBYTE data,DWORD len);
    void     __fastcall modem_notify (DWORD mask,DWORD code,LPVOID data,DWORD len);
    void     __fastcall modem_notify (DWORD code,DWORD addr,DWORD parts,DWORD lo = 0,DWORD hi=-1,DWORD what = 0);
    void     __fastcall event_report (DWORD evt_type,wchar_t * rep_text,LPVOID data = 0,DWORD sz = 0);
    void     __fastcall tutr_report  (DWORD evt_type,wchar_t * rep_text);
    DWORD    __fastcall get_flags    (){DWORD ret; lock();ret = floor_flags;unlock();return ret;}
    DWORD    __fastcall get_cp_count (bool dyn = false);
    DWORD    __fastcall get_props    (DWORD addr,LPVOID ptr);
    DWORD    __fastcall set_props    (DWORD addr,LPVOID ptr);
    DWORD    __fastcall set_cp_props (DWORD addr,LPVOID ptr);
    DWORD    __fastcall set_grp_props(DWORD addr,LPGR_GRP_PROPS ptr);
    DWORD    __fastcall set_props    (LPGR_FLOOR_PROPS  prp);
    DWORD    __fastcall enum_cp      (DWORD index,LPGR_CP_PROPS  cpp);
    DWORD    __fastcall enum_grp     (DWORD index,LPGR_GRP_PROPS grp);
    DWORD    __fastcall create_cp    (DWORD num,LPGR_CP_PROPS cpp);
    DWORD    __fastcall create_group (DWORD cp,LPGR_GRP_PROPS grp);
    DWORD    __fastcall remove       (DWORD addr);
    DWORD    __fastcall get_otd_data (DWORD addr,LPSC_OTDDATA scod);
    DWORD    __fastcall get_otd_data (LPBYTE buffer,DWORD bsz,DWORD parts,DWORD lo,DWORD hi);
    DWORD    __fastcall get_dynamic  (DWORD addr,LPVOID dyn_data);
    DWORD    __fastcall floor_refresh(DWORD cp,DWORD fa);
    DWORD    __fastcall test_tutr    (LPSC_TEST_TUTR tt);
    DWORD    __fastcall reset_tu_error(otd_addr & addr,DWORD obj);

    LRESULT  __fastcall get_tu_entry (LPGRSC_TU_ENTRY tue);
    LRESULT  __fastcall add_tu_entry (LPGRSC_TU_ENTRY tue);
    LRESULT  __fastcall set_tu_entry (LPGRSC_TU_ENTRY tue);
    LRESULT  __fastcall del_tu_entry (LPGRSC_TU_ENTRY tue);
    LRESULT  __fastcall enum_tu_entry(LPGRSC_TU_ENTRY tue,DWORD idx);



    void      __fastcall set_line_number(DWORD number);
    void      __fastcall config_change();
    DWORD     __fastcall get_stream_ver(){return 0;}
    void      __fastcall write(TStream & strm);
    void      __fastcall read (TStream & strm,DWORD ver);

};


typedef std::vector<LPGRSC_TU_ENTRY> tu_entry_list;

struct  tu_entry_comparer
{
 int operator()(const LPGRSC_TU_ENTRY  tue1, const  LPGRSC_TU_ENTRY  tue2);
 int operator()(const   GRSC_TU_ENTRY &tue1, const    GRSC_TU_ENTRY  &tue2);
};

inline int tu_entry_comparer::operator()(const LPGRSC_TU_ENTRY tue1,const LPGRSC_TU_ENTRY tue2)
{
 return (*this)(*tue1,*tue2);
}

inline int tu_entry_comparer::operator()(const GRSC_TU_ENTRY & tue1,const GRSC_TU_ENTRY  &tue2)
{
 int res = otd_addr_cmp((lpotd_addr)&tue1.addr,(lpotd_addr)&tue2.addr);
 if(!res)
    res = (int)tue1.object < (int)tue2.object;
 return res;
}

inline LPGRSC_TU_ENTRY __fastcall tue_init(LPGRSC_TU_ENTRY dest,LPGRSC_TU_ENTRY src)
{
 if(dest && src && src->dw_size == sizeof(*dest))
 {
  memcpy(dest,src,sizeof(*dest));
  dest->dw_size = sizeof(*dest);
 }
 return dest;
}

inline bool  __fastcall is_tu_active(LPGRSC_TU_ENTRY tue)
{
 return (tue->tu_state&GRSC_TUTR_STATE_ACTIVE) ? true:false;
}

inline bool  __fastcall is_tu_queued(LPGRSC_TU_ENTRY tue)
{
 return (!is_tu_active(tue) && tue->tu_state&GRSC_TUTR_STATE_QUEUED) ? true:false;
}

inline bool __fastcall is_tu_selected(LPGRSC_TU_ENTRY tue)
{
 return (tue->tu_state&GRSC_TUTR_STATE_SELECTED) ? true:false;
}

inline bool __fastcall is_RA(LPGRSC_TU_ENTRY tue)
{
 return ((tue->tu_flags&GRSC_TUFL_RAviaRC) || (tue->addr.fa&OTD_FA_DATA_MASK)) ?  true:false;
}

inline DWORD __fastcall get_tu_command(LPGRSC_TU_ENTRY tue)
{
 return (is_tu_queued(tue) && (tue->tu_state&(GRSC_TUTR_STATE_CMDON|GRSC_TUTR_STATE_CMDOFF))) ?
 ((tue->tu_state&GRSC_TUTR_STATE_CMDON) ? tue->cmd_on:tue->cmd_off)
 :
 GRKTAFB_TRCANCEL ;
}

class gr_group;
class gr_cp
{
 protected:
 sc_floor *        owner;
 DWORD             number;
 DWORD             flags;
 wchar_t           name[GRSC_TEXT_MAXSIZE];
 int               send_wait_time;

 typedef std::vector<gr_group*> gr_group_list;

 gr_group_list     grp_list;                    // список групп
 tu_entry_list     tu_list;                     // Список    ТУ

 DWORD             diag;
 __int64           time_stamp;
 __int64           tx_time;         //Готовность к передаче
 DWORD             tx_deliver_time; //Время от начала передачи до готовности
 DWORD             tx_byte_time;    //Среднее время передачи 1 байта
 DWORD             tx_current_len;  //Количество передаваемых байт
 TSimpleDataQueue  tx_queue;        //Очередь на передачу

 DWORD             rx_timer;      // Таймер приёма
 DWORD             rx_timer_value;// Значение таймера приёма определяющее неработоспособность КП
 LPGRSC_TU_ENTRY   tu_active[GRCP_MAX_MODCOUNT];     // Активные ТУ;  


 void       __fastcall release  ();
 void       __fastcall on_timer  (__int64 sys_time);

 void       __fastcall continue_floor_tx(__int64 sys_time);
 bool       __fastcall do_floor_tx(LPBYTE data,DWORD len);

 DWORD      __fastcall handle       (gr_proto & gp,DWORD proto_error,__int64 h_time);
 DWORD      __fastcall handle_data  (gr_proto & gp,DWORD proto_error,__int64 h_time);
 DWORD      __fastcall handle_ready (gr_proto & gp,__int64 h_time);
 DWORD      __fastcall handle_error (gr_proto & gp,__int64 h_time);
 DWORD      __fastcall handle_group (gr_proto & gp,__int64 h_time);

 void       __fastcall receipt      (gr_proto & grp);
 void       __fastcall group_timer  (__int64 sys_time);

 void       __fastcall update_rxtimer_value();
 void       __fastcall queue_group_data  (gr_group * grp,DWORD ma = 0,bool _call = false);
 DWORD      __fastcall modem_send        (LPMPROTO_HEADER mph,lpotd_proto op);
 DWORD      __fastcall modem_handle_query(LPMPROTO_HEADER mph,lpotd_proto op);

 void       __fastcall tutr_autoreset   (__int64 sys_time);
 void       __fastcall tutr_timer       (__int64 sys_time);
 bool       __fastcall tutr_is_ready    (DWORD mod_number);
 void       __fastcall tutr_next        (DWORD mod_number);
 DWORD      __fastcall tutr_activate    (DWORD tutr_idx);
 DWORD      __fastcall tutr_deactivate  (DWORD tutr_idx);
 DWORD      __fastcall tutr_command(DWORD modem_addr,DWORD  addr,DWORD obj,otd_tutr& cmd,LPGRSC_TU_ENTRY tu_entry);
 DWORD      __fastcall tutr_select      (DWORD modem_addr,DWORD  addr,DWORD obj,LPGRSC_TU_ENTRY tu_entry);
 DWORD      __fastcall tutr_deselect    (DWORD modem_addr,DWORD  addr,DWORD obj,LPGRSC_TU_ENTRY tu_entry);

 DWORD      __fastcall tutr_send        (LPMPROTO_HEADER mph,lpotd_proto op);
 void       __fastcall tutr_log         (DWORD modem_addr,DWORD  addr ,DWORD obj,otd_tutr & tu_tr,LPGRSC_TU_ENTRY tue,DWORD error);

 tu_entry_list::iterator
            __fastcall tutr_lower_bound(DWORD _otd_addr,DWORD obj);
 DWORD      __fastcall tutr_find_entry (DWORD _otd_addr,DWORD obj);
 BOOL       __fastcall tutr_remove     (DWORD idx);
 DWORD      __fastcall tutr_add        (gr_group * grp,LPGRSC_TU_ENTRY entry,DWORD modem_addr);

 void       __fastcall tutr_check_success(gr_group * grp,DWORD obj);


 public   :
              explicit gr_cp(sc_floor * own,DWORD number);
                      ~gr_cp();
 sc_floor *   __fastcall get_owner(){return owner;}
 DWORD        __fastcall get_otd_addr(){return sotd_addr(0,number,/*OTD_FA_ALL*/OTD_FA_DATA_MASK,-1).addr;}
 DWORD        __fastcall get_diag(bool rescan = false){return rescan ? scan_diag():diag;}
 DWORD        __fastcall get_number(){return number;}
 DWORD        __fastcall get_flags (){return flags ;}

 bool         __fastcall is_tx_ready(){return (!tx_time) ? true:false ;}
 bool         __fastcall floor_tx     (LPBYTE data,DWORD len,bool  first = false);
 void         __fastcall send_call(DWORD call_afb, bool no_check = false);

 DWORD        __fastcall set_diag  (DWORD df,bool set);
 bool         __fastcall check_diag(DWORD df,bool all);
 DWORD        __fastcall scan_diag ();
 void         __fastcall update_diag();

 void         __fastcall set_number(DWORD num);


 DWORD        __fastcall get_props (DWORD addr,LPVOID ptr);
 DWORD        __fastcall set_props (DWORD addr,LPVOID ptr);
 DWORD        __fastcall get_props (LPGR_CP_PROPS prp);
 DWORD        __fastcall set_props (LPGR_CP_PROPS prp);
 DWORD        __fastcall add_group (gr_group * grp,gr_group_list::iterator ptr = NULL);

 gr_group_list::iterator   __fastcall lower_group(otd_addr & addr);
 gr_group*    __fastcall find_group(DWORD addr);
 gr_group*    __fastcall find_group(otd_addr & addr);
 gr_group*    __fastcall find_group(DWORD inf,DWORD group,DWORD mod,bool create = false);
 DWORD        __fastcall enum_grp  (DWORD index,LPGR_GRP_PROPS cpp);
 void         __fastcall refresh (DWORD  fa);
 void         __fastcall queue_rxdata(DWORD fa,LPVOID data,DWORD len,bool first = false,DWORD maddr_to = 0);
 DWORD        __fastcall get_group_count(bool dyn);
 DWORD        __fastcall create_group(LPGR_GRP_PROPS grp);
 DWORD        __fastcall remove      (DWORD addr);


 DWORD        __fastcall get_stream_ver(){return 1;}
 void         __fastcall write       (TStream & strm);
 void         __fastcall read        (TStream & strm,DWORD ver);

 __int64      __fastcall get_timestamp();
 DWORD        __fastcall get_otd_data(DWORD addr,LPSC_OTDDATA scod);
 DWORD        __fastcall get_otd_data(DWORD addr,DWORD parts,LPBYTE buff,DWORD bsz,lpotd_proto op);
 DWORD        __fastcall get_dynamic (DWORD addr,LPVOID dyn_data);

 void         __fastcall modem_notify(DWORD mask,DWORD code,LPVOID data,DWORD len);
 void         __fastcall modem_notify(DWORD code,DWORD addr,DWORD parts,DWORD lo = 0,DWORD hi=-1,DWORD what = 0);
 DWORD        __fastcall test_tutr   (LPSC_TEST_TUTR tt);

 LRESULT      __fastcall get_tu_entry(LPGRSC_TU_ENTRY tue);
 LRESULT      __fastcall add_tu_entry(LPGRSC_TU_ENTRY tue);
 LRESULT      __fastcall set_tu_entry(LPGRSC_TU_ENTRY tue);
 LRESULT      __fastcall del_tu_entry(LPGRSC_TU_ENTRY tue);
 LRESULT      __fastcall enum_tu_entry(LPGRSC_TU_ENTRY tue,DWORD idx);


 void         __fastcall log_diag();
 void         __fastcall event_report(DWORD evt_type,wchar_t * rep_text,LPVOID data = 0,DWORD sz = 0);
 void         __fastcall tutr_report (DWORD evt_type,wchar_t * rep_text);
 void         __fastcall reset_tu_error();
 DWORD        __fastcall reset_tu_error(otd_addr & sa,DWORD obj);

 friend     class sc_floor;
};





#define GRSC_GROUP_STREM_VER        1


class gr_group
{
  protected:
  gr_cp    *    owner;                 //КП - владелец
  DWORD         number;                //Номер группы
  DWORD         flags;                 //Флаги
  DWORD         inf;                   //Тип группы
  DWORD         mod_number;            //Номер модуля
  DWORD         work_timer_value;      //Значения таймаута вызова работоспособной группы
  DWORD         nowork_timer_value;    //Значение таймаута неработоспособной группы
  DWORD         respond_timer_value;   //таймаут - ответа
  DWORD         tm_counter;            //Счетчик таймера
  __int64       call_send_time;        //Состояние группы
  DWORD         respond_time  ;        //Время ответа группы
  DWORD         nchd_counter;          //Счётчик данных пришедших без изменения
  DWORD         chdbc_counter;         //Счетчик изменений по вызову
  DWORD         respond_smooth;        //Значение Norespond_counter для установки неработоспособности группы
  DWORD         norespond_counter;     //Количество неотвеченных вызовов
  lpotd_proto   grp_data;              //Данные группы
  tu_entry_list tu_list;

  bool        __fastcall create();
  bool        __fastcall check_tu_duplicate(LPGRSC_TU_ENTRY tue);
  public:
            explicit   gr_group(gr_cp * owner);
            explicit   gr_group(gr_cp * owner,LPGR_GRP_PROPS);
            explicit   gr_group(gr_cp * owner,DWORD number,DWORD inf,DWORD mod_number);
                      ~gr_group();

  void      __fastcall release();
  void      __fastcall set_cp_number(DWORD num);
  DWORD     __fastcall set_diag(DWORD dfl,bool set,bool send_changes = true);
  bool      __fastcall check_diag(DWORD df,bool all = false);
  DWORD     __fastcall scan_diag();
  DWORD     __fastcall get_diag(bool rescan = false);

  bool      __fastcall on_timer(__int64 sys_time);
  DWORD     __fastcall get_otd_addr();
  DWORD     __fastcall handle  (gr_proto & gp,__int64 h_time);


  DWORD     __fastcall get_respond_time()      {return respond_time;}
  DWORD     __fastcall get_respond_time_value(){return respond_timer_value;}
  DWORD     __fastcall get_norespond_counter_value() {return respond_smooth;}
  DWORD     __fastcall get_work_timer_value(){return work_timer_value;}
  DWORD     __fastcall get_props(LPGR_GRP_PROPS);
  DWORD     __fastcall set_props(LPGR_GRP_PROPS);
  DWORD     __fastcall get_call_afb();
  DWORD     __fastcall get_number(){return number;}
  DWORD     __fastcall get_flags (){return flags ;}
  DWORD     __fastcall get_mod_number(){return mod_number;}

  DWORD     __fastcall get_stream_ver(){return GRSC_GROUP_STREM_VER;}
  void      __fastcall write(TStream & strm);
  void      __fastcall read (TStream & strm,DWORD ver);

  __int64   __fastcall get_timestamp(){return (grp_data && grp_data->time_stamp) ? *((__int64*)grp_data->time_stamp):(__int64)0;}
  DWORD     __fastcall get_otd_data(LPSC_OTDDATA scod);
  DWORD     __fastcall get_otd_data(DWORD parts,DWORD lo_num,DWORD hi_num,LPBYTE buf,DWORD bsz,lpotd_proto op);
  DWORD     __fastcall get_dynamic (LPVOID dyn_data);
  DWORD     __fastcall get_object_value(DWORD obj,bool inverse);
  void      __fastcall modem_send_changes(DWORD parts,DWORD low = 0,DWORD hi = -1,DWORD modem_addr = 0);
  bool      __fastcall call_sended(__int64 c_time);
  void      __fastcall queue_rxdata(LPBYTE data,DWORD len,bool first,DWORD maddr_to = 0);
  void      __fastcall modem_notify(DWORD mask,DWORD code,LPVOID data,DWORD len);
  void      __fastcall modem_notify(DWORD code,DWORD addr,DWORD parts,DWORD lo = 0,DWORD hi=-1,DWORD what = 0);
  void      __fastcall log_diag();
  void      __fastcall event_report(DWORD evt_type,wchar_t * rep_text,LPVOID data = 0,DWORD sz = 0);

  tu_entry_list::iterator __fastcall lower_bound(DWORD obj);
  LPGRSC_TU_ENTRY         __fastcall find_tu_entry(DWORD obj);
  BOOL                    __fastcall get_tu_entry(DWORD obj,LPGRSC_TU_ENTRY);

  LRESULT __fastcall get_tu_entry (LPGRSC_TU_ENTRY tue);
  LRESULT __fastcall add_tu_entry (LPGRSC_TU_ENTRY tue);
  LRESULT __fastcall set_tu_entry (LPGRSC_TU_ENTRY tue);
  LRESULT __fastcall del_tu_entry (LPGRSC_TU_ENTRY tue);
  LRESULT __fastcall enum_tu_entry(LPGRSC_TU_ENTRY tue,DWORD idx);

  DWORD   __fastcall set_personal_diag(DWORD lo_obj,DWORD hi_obj,DWORD pd,bool set,bool send = false,DWORD modem_addr = 0);
  BOOL    __fastcall get_first_change(LPDWORD chnum);
  BOOL    __fastcall get_next_change (DWORD prev_ch,LPDWORD chnum);
  void    __fastcall reset_tu_error(DWORD lo,DWORD hi);
  


};

struct gr_group_less
{
  DWORD addr;
  gr_group_less(DWORD _addr = -1):addr(_addr){}
  gr_group_less(gr_group_less & gl):addr(gl.addr) {}
  bool  operator()(gr_group* g1 ,gr_group* g2)
  {
   sotd_addr a1(g1? g1->get_otd_addr():addr),a2(g2 ? g2->get_otd_addr():addr);
   a1.pu = a2.pu = 0;
   return (otd_addr_cmp(&a1,&a2)<0) ? true:false;
  }
};

inline void    __fastcall sc_floor::modem_notify(DWORD mask,DWORD code,LPVOID data,DWORD len)
{
 if(owner)
   owner->notify(mask,code,data,len);
}


inline void         __fastcall gr_cp::queue_rxdata(DWORD fa,LPVOID data,DWORD len,bool first,DWORD maddr_to )
{

 if(owner)
    owner->queue_rxdata(fa,(LPBYTE)data,len,first,maddr_to);
}

inline void      __fastcall gr_cp::modem_notify(DWORD mask,DWORD code,LPVOID data,DWORD len)
{
 if(owner)
    owner->modem_notify(mask,code,data,len);
}

inline void      __fastcall gr_cp::modem_notify(DWORD mask,DWORD addr,DWORD parts,DWORD lo,DWORD hi,DWORD what )
{
 if(owner)
    owner->modem_notify(mask,addr,parts,lo,hi,what);
}

inline void      __fastcall gr_cp::event_report(DWORD evt_type,wchar_t * rep_text,LPVOID data ,DWORD sz )
{
 if(owner)
    owner->event_report(evt_type,rep_text,data,sz);
}

inline void      __fastcall gr_cp::tutr_report(DWORD evt_type,wchar_t * rep_text)
{
 if(owner)
    owner->tutr_report(evt_type,rep_text);
}

inline gr_group*    __fastcall gr_cp::find_group(DWORD addr)
{
 sotd_addr sa(addr);
 return find_group(sa);
}



inline  void      __fastcall gr_group::queue_rxdata(LPBYTE data,DWORD len,bool first,DWORD maddr_to )
{
 if(owner)
    owner->queue_rxdata(FA_OTD,data,len,first,maddr_to);

}

inline void      __fastcall gr_group::modem_notify(DWORD mask,DWORD code,LPVOID data,DWORD len)
{
 if(owner)
    owner->modem_notify(mask,code,data,len);
}

inline void      __fastcall gr_group::modem_notify(DWORD mask,DWORD addr,DWORD parts,DWORD lo,DWORD hi,DWORD what )
{
 if(owner)
    owner->modem_notify(mask,addr,parts,lo,hi,what);
}

inline void      __fastcall gr_group::event_report(DWORD evt_type,wchar_t * rep_text,LPVOID data ,DWORD sz )
{
 if(owner)
    owner->event_report(evt_type,rep_text,data,sz);
}






#endif


 