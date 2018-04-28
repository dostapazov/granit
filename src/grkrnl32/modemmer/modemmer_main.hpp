/*-----------------------------------------------------------------------------
 15.03.2004 13:53:23  
 Модуль:   modemmer
 Автор:    Остапенко Д.В.
 Назначение:  Менеджер устройств и дипетчер ФА(ДФА)

-----------------------------------------------------------------------------*/



#ifndef _GKMODEMMER_HPP
#define _GKMODEMMER_HPP 1

#include <modemmer.h>
#include <modem.hpp>
#include <gkmodules.hpp>
#include <KeRTLio.hpp>
#include <kethreads.hpp>
#include <kedatas.hpp>
#include <vector>
#include <map>
#include <list>
#include <string>

using namespace KeRTL;
using namespace std;

class  TModemmer;
class  TDfa;
class  TRedirectTable;
class  TRouterTable;


class TFaHandler:public TGKThread
{
 protected:
 TEvent            terminate_event;//Событие для завершения
 TEvent            handle_event;   //Событие для сигнализации о том, что есть данные для обработки
 TSimpleDataQueue  data_queue;     //Очередь данных для обработки
 TFASTMutex        reg_locker;
 vector<GKHANDLE>  registered_handlers;//Список зарегистрированных обработчиков фа
 DWORD             wait_time;
 TModemmer         * modemmer;
 DWORD          pkt_num;
 bool         __fastcall BeforeExecute();
 int          __fastcall Execute();
 void         __fastcall BeforeTerminate();
 void         __fastcall do_handle();
 void         __fastcall call_handlers(LPBYTE data,DWORD len);
 static char  * thread_name;
 const  char* __fastcall get_gkthread_name(){return thread_name;}

 public:
 TFaHandler(TModemmer* _modemmer,const wchar_t * stop_event_name,GKHANDLE first_handle)
 :terminate_event(stop_event_name,SYNCHRONIZE)
 ,handle_event(false,false)
 {registered_handlers.push_back(first_handle);};
 int    __fastcall reg_handler  ( GKHANDLE h  );
 int    __fastcall unreg_handler( GKHANDLE h  );
 int    __fastcall push_data(LPVOID data,DWORD sz);
 int    __fastcall get_handlers_count();
 bool   __fastcall enum_handlers(DWORD idx,LPFA_HANDLER fah);
};

class TDfa
{
 protected:
 wchar_t      stop_event_name[MAX_PATH];
 TEvent       stop_event;
 TFaHandler * handlers[MAX_FA_COUNT];//Список хандлеров
 TModemmer  * modemmer;
 bool         running;
 TFASTMutex   locker;

 public:
 TDfa(TModemmer * _modemmer);
 void    __fastcall start        ();
 void    __fastcall stop         ();
 void    __fastcall release      ();
 bool    __fastcall reg_handler  (GKHANDLE handler,DWORD fa);
 bool    __fastcall unreg_handler(GKHANDLE handler,DWORD fa);
 bool    __fastcall handle       (LPMPROTO_HEADER,DWORD sz);
 bool    __fastcall enum_handlers(DWORD idx,LPFA_HANDLER fah);
 void    __fastcall get_fa_text   (LPFA_HANDLER lpfah);
};


/*ССылка на линию модема*/
struct  modem_line_ref:protected TSimpleDataQueue
{
 modem_line_ref(LPMODEM_LINE_PROPS);
 ~modem_line_ref(){clear();}

 MODEM_LINE_PROPS props;
 DWORD            tx_kadr_number;
 void  __fastcall set_props  (LPMODEM_LINE_PROPS props){CopyMemory(&this->props,props,sizeof(*props));}
 DWORD __fastcall set_state  (DWORD state){DWORD ret = props.line_state ^ state;  props.line_state = state;  return ret;}
 DWORD __fastcall get_state  (){  return props.line_state; }
 int   __fastcall queue_count(){  return this->QueueCount();}
 int   __fastcall queue_data(LPMPROTO_HEADER mph,__int64 q_time ,__int64 time_value);
 int   __fastcall get_data  (LPMPROTO_HEADER mph,DWORD sz);
 int   __fastcall peek_data (LPMPROTO_HEADER mph,DWORD sz);
 int   __fastcall top_size  (){return TopDataSize();}
 void  __fastcall clear(){   DropData(); };
 bool  __fastcall drop_data(DWORD timer_id);
 void  __fastcall drop_first(){DropFirst();}
 bool  __fastcall can_send();
 bool  __fastcall istx_ready();
 DWORD __fastcall erase_timeouts(__int64 curr_time);

 TFASTMutex & __fastcall get_locker(){return locker;}
};

class TModemInterface:public TGKModuleInterface
{
 public:
 TModemInterface(GKHANDLE  h):TGKModuleInterface(h){};
 TModemInterface(GKHANDLE  h,bool no_close):TGKModuleInterface(h,no_close){};
 bool    __fastcall get_line_addr(DWORD idx,LPMODEM_ADDR addr);
 DWORD   __fastcall get_number();
 DWORD   __fastcall get_line_state(DWORD line_number);
 LRESULT __fastcall get_props(LPMODEM_PROPS mp);
 LRESULT __fastcall get_line_props(DWORD ln,LPMODEM_LINE_PROPS mlp);
 LRESULT __fastcall get_line_stats(DWORD ln,LPMODEM_LINE_STATS mls);
 LRESULT __fastcall send(LPMPROTO_HEADER,DWORD len);
 LRESULT __fastcall recv(LPMPROTO_HEADER,DWORD len);
 LRESULT __fastcall enum_lines(DWORD idx,LPMODEM_LINE_PROPS mlp);
 LRESULT __fastcall set_number(DWORD number,LPDWORD old);
};

/*Ссылка на модем*/

#define MODEM_REF_TERMINATE_KEY  0
#define MODEM_REF_LINESTATE_KEY  1
#define MODEM_REF_TIMER_KEY      2
#define MODEM_REF_TX_KEY         3

class modem_ref:public TGKThread
{
  protected:
   TFASTMutex       locker;
   TModemmer       * modemmer;   //Владелец
   TModemInterface * modem_iface;//Интерфейс с модемом
   TIoCompletionPort icp;
   typedef           std::map<DWORD,modem_line_ref*> lines_map;
   lines_map         lines;/*Список линий*/
   char   *          buffer;
   DWORD             buffer_size;
   bool              __fastcall check_buffer(DWORD need_size); 

   
   modem_line_ref*   __fastcall find_line(DWORD num,bool create = true);
   bool              __fastcall BeforeExecute();   
   void              __fastcall BeforeTerminate();
   int               __fastcall Execute();
   void              __fastcall Terminate(){TThread::Terminate();icp.PostStatus(MODEM_REF_TERMINATE_KEY,-1,NULL);}
   void              __fastcall handle_line_state(DWORD line,DWORD state);
   void              __fastcall scan_lines();
   const char *      __fastcall get_gkthread_name(){return "Modemmer:: Modem reference  thread";}
   void              __fastcall release();
   void              __fastcall clear_tx_queue(modem_line_ref * mlr);
   void              __fastcall clear_lines();
   void              __fastcall do_recv();
   void              __fastcall do_send(modem_line_ref * mlr);
   DWORD             __fastcall OnException(DWORD ExceptionCode,LPEXCEPTION_POINTERS);
   void              __fastcall on_wait_timeout();

  public:
   modem_ref(TModemmer * _mdm,TModemInterface * modem_iface);
   ~modem_ref(){release();}
   TModemInterface * __fastcall get_interface(){return modem_iface;}
   bool    __fastcall start();
   bool    __fastcall stop();
   DWORD   __fastcall get_lines_count(){DWORD ret;lock(); ret = lines.size();unlock();return ret;}
   bool    __fastcall get_line_addr(DWORD idx,LPMODEM_ADDR addr);
   bool    __fastcall enum_lines(DWORD idx,MODEM_LINE_PROPS & lp);
   LRESULT __fastcall send(LPMPROTO_HEADER send_mph,DWORD wait);
   LRESULT __fastcall on_send_timeout(DWORD id,DWORD line);
   void    __fastcall post_line_state(DWORD modem_addr,DWORD line_state){icp.PostStatus(MODEM_REF_LINESTATE_KEY,line_state,(LPOVERLAPPED)modem_addr);}
   void    __fastcall post_txkey(DWORD modem_addr){icp.PostStatus(MODEM_REF_TX_KEY,0,(LPOVERLAPPED)modem_addr);}
   void    __fastcall post_timer     (DWORD addr,DWORD id){icp.PostStatus(MODEM_REF_TIMER_KEY,id,(LPOVERLAPPED)addr);}
   void    __fastcall update_modem_number(DWORD num);
   bool    __fastcall lock(){return locker.Lock();}
   bool    __fastcall unlock(bool full = false)
   { if(full) {return locker.FullUnlock() ? true : false;}  return locker.Unlock();}

};

/*Список активных модемов*/
class modems_list:protected std::map<DWORD,modem_ref*>
{
 protected:
 TFASTMutex      locker;
 TModemmer    *  modemmer;
 public:
 modems_list():modemmer(NULL){};
 ~modems_list(){release();}
 bool  __fastcall lock  ()  {return locker.Lock();}
 bool  __fastcall unlock(bool full = false)
 { if(full) {return locker.FullUnlock() ? true : false;}  return locker.Unlock();}
 void  __fastcall handle_line_state(LPMODEM_LINE_STATE mls);
 DWORD __fastcall get_modems_count(){DWORD ret;lock(); ret = size();unlock();return ret;}
 BOOL  __fastcall get_modem_addr(DWORD idx,LPMODEM_ADDR addr);
 BOOL  __fastcall get_modem_line_addr(DWORD idx,LPMODEM_ADDR addr);
 bool  __fastcall add(DWORD number,modem_ref * ref);
 modem_ref *  __fastcall remove(DWORD number);
 void         __fastcall release();
void          __fastcall stop();
 modem_ref *  __fastcall get_modem_ref(DWORD number);
 modem_ref *  __fastcall get_modem_ref_byidx(DWORD idx);
 void __fastcall set_modemmer(TModemmer * mdm){modemmer = mdm;}
 LRESULT __fastcall send(LPMPROTO_HEADER send_mph,DWORD wait);
 LRESULT __fastcall on_send_timeout(DWORD addr,DWORD id);
};



/*Таблица перенаправлений*/

typedef list<modem_addr> ma_list;

class   TRedirectTable
{
 protected:
 BOOL                          redirect_enabled;
 TMrSw                         locker;
 std::vector<LPREDIRECT_ENTRY> table;
 public:

  TRedirectTable(){redirect_enabled = FALSE;};
 ~TRedirectTable(){release();}
 void   __fastcall  release();
 DWORD  __fastcall  write (HKEY );
 DWORD  __fastcall  size(){DWORD ret = 0;rlock();ret = table.size();runlock();return ret;}
 DWORD  __fastcall  read  (HKEY );
 DWORD  __fastcall  add   (LPREDIRECT_ENTRY);
 DWORD  __fastcall  remove(DWORD index);
 DWORD  __fastcall  update(DWORD index,LPREDIRECT_ENTRY);
 BOOL   __fastcall  enum_entry(DWORD index,LPREDIRECT_ENTRY);
 DWORD  __fastcall  make_redirect_list(LPMPROTO_HEADER mph,ma_list &lst,LPDWORD);
 BOOL   __fastcall  redirect_enable(BOOL enable);
 BOOL   __fastcall  is_redirect_enabled(){BOOL ret; rlock();ret = redirect_enabled;runlock();return ret;}
 bool   __fastcall  rlock(DWORD to = INFINITE);
 bool   __fastcall  runlock();
 bool   __fastcall  wlock(DWORD to = INFINITE);
 bool   __fastcall  wunlock();

};



class TInternalLines
{
  protected:
  TFASTMutex locker;
  struct     int_line
  {
     GKHANDLE           line_handle;
     MODEM_LINE_STATS   line_stats;
     __int64            rx_update_time;
     __int64            tx_update_time;
     DWORD              tx_zero_bytes;
     DWORD              rx_zero_bytes;
     DWORD              tx_kadr_number;
     DWORD              rx_kadr_number;

     int_line(){ZeroMemory(this,sizeof(*this));}
     int_line(GKHANDLE handle,int num)
     {
      ZeroMemory(this,sizeof(*this));line_handle = handle;line_stats.dw_size = sizeof(line_stats);line_stats.addr.line = num;
     }
     int_line(const int_line & src)   {*this = src;}
     int_line & operator = (const int_line & src)
     {
      line_handle = src.line_handle;
      memcpy(&line_stats,&src.line_stats,sizeof(line_stats));
      rx_update_time = src.rx_update_time;
      tx_update_time = src.tx_update_time;
      tx_zero_bytes  = src.tx_zero_bytes;
      rx_zero_bytes  = src.rx_zero_bytes;
      tx_kadr_number = src.tx_kadr_number;
      rx_kadr_number = src.rx_kadr_number;

      return *this;
     }

  };

  int_line      lines[MODEM_ADDR_MAXVALUE];

  public:
  TInternalLines(){}
  DWORD      __fastcall reg_intline       (GKHANDLE);
  DWORD      __fastcall unreg_intline     (GKHANDLE,DWORD line);
  GKHANDLE   __fastcall get_intline_handle(DWORD line);
  DWORD      __fastcall get_next_kadr_num (DWORD line,bool tx);
  DWORD      __fastcall find_line         (GKHANDLE h);
  DWORD      __fastcall enum_lines        (DWORD idx,LPMODEM_LINE_PROPS mlp);
  DWORD      __fastcall get_lines_count   ();
  LRESULT    __fastcall get_line_stats    (DWORD line,LPMODEM_LINE_STATS mls);
  LRESULT    __fastcall get_line_props    (DWORD line,LPMODEM_LINE_PROPS mlp);
  void       __fastcall update_tx         (DWORD line,DWORD kadrs,DWORD bytes);
  void       __fastcall update_rx         (DWORD line,DWORD kadrs,DWORD bytes);
};


class TModemmer:public TGKModulesSet
{
 protected:
 DWORD               event_type;
 TDfa                dfa;
 TRedirectTable      redir_table;
 modems_list         modems;
 TGKModulesList      free_modems;//Список свободных модемов
 TInternalLines      int_lines;
 TGKModuleInterface  * __fastcall get_interface(GKHANDLE h,DWORD type);
 void                 __fastcall free_interface(TGKModuleInterface* mldr);
 WORD    __fastcall get_type       (){return MT_MODEMMER;}
 bool        __fastcall check_module_type(WORD mt) {return (mt&MT_MODEMMER) && ((mt&MT_WINDOW)==0) ? true:false ;}
 DWORD       __fastcall get_window_module (wchar_t * buf,DWORD bsz);
 DWORD       __fastcall get_config_data(DWORD mask,LPVOID buf,DWORD bsz);
 bool        __fastcall set_config_data(DWORD mask,LPVOID buf,DWORD bsz);
 bool        __fastcall compare_config_data(DWORD mask,LPVOID buf,DWORD bsz,LPDWORD ch_mask,BOOL * restart);


 bool    __fastcall read_settings  ();
 bool    __fastcall write_settings ();


 DWORD       __fastcall start(DWORD reason,LPARAM p2);
 DWORD       __fastcall stop (DWORD reason);
 LRESULT     __fastcall processing(DWORD cmd,LPARAM p1,LPARAM p2);
 LRESULT     __fastcall handle_line_state(LPMODEM_LINE_STATE mls);
 bool        __fastcall accept_loaded_module(TGKModuleInterface * iface,DWORD type);
 bool        __fastcall remove_module(GKHANDLE h,bool erase);
 void        __fastcall release(LPVOID);

 LRESULT     __fastcall get_line_stats     (DWORD addr,LPMODEM_LINE_STATS mls);
 LRESULT     __fastcall get_int_line_props (DWORD addr,LPMODEM_LINE_PROPS mlp);
 LRESULT     __fastcall get_line_props     (DWORD addr,LPMODEM_LINE_PROPS mlp);
 GKHANDLE    __fastcall get_modem_handle   (DWORD addr);

 LRESULT  __fastcall send    (LPMPROTO_HEADER, DWORD timeout);
 LRESULT  __fastcall redirect(LPMPROTO_HEADER mph,DWORD sz,BOOL *enable_handle);

 LRESULT  __fastcall redirect_enable(BOOL enable);
 LRESULT  __fastcall redirect_is_enabled(LPBOOL enable);
 LRESULT  __fastcall redirect_enum(DWORD index,LPREDIRECT_ENTRY re);
 LRESULT  __fastcall redirect_add (LPREDIRECT_ENTRY re);
 LRESULT  __fastcall redirect_remove(DWORD index);
 LRESULT  __fastcall redirect_update (LPREDIRECT_ENTRY re,LPDWORD index);
 void     __fastcall redirect_notify (DWORD cmd,DWORD index);
 DWORD   __fastcall get_mem_used();
 LRESULT __fastcall reg_internal_line(GKHANDLE h,DWORD lm);
 DWORD   __fastcall on_exception     (lpexcept_record er);
 LRESULT __fastcall handle_fa(LPMPROTO_HEADER mph,GKHANDLE h_from);
 LRESULT __fastcall ctrl_time_sync(LPMPROTO_HEADER mph);
 void    __fastcall reg_reports();
 virtual   DWORD   __fastcall check_license(){return MOD_LICENSE_REGISTERED; }

 public:
 TModemmer();
 virtual ~TModemmer();
 LRESULT  __fastcall input_handler(LPMPROTO_HEADER,DWORD sz);
 void     __fastcall txrx_notify(LPMPROTO_HEADER mph,DWORD code);
 LRESULT  __fastcall route   (LPMPROTO_HEADER mph,DWORD sz);
 void     __fastcall modemmer_report(DWORD type,wchar_t * text);

 protected:
 LRESULT  __fastcall enum_modems(DWORD idx,LPMODEM_ADDR ma);
 LRESULT  __fastcall enum_freemodems(DWORD idx,GKHANDLE * handle);
 LRESULT  __fastcall enum_modem_lines(DWORD idx,LPMODEM_LINE_PROPS mlp);
 LRESULT  __fastcall get_modem_props (LPMODEM_PROPS mp,GKHANDLE h);
 LRESULT  __fastcall change_modem_number(LPMODEM_CHANGE_NUMBER,BOOL no_call);


};





/** inline implementations**/


inline DWORD __fastcall TModemInterface::get_number()
 {  return (DWORD) call(MODEMCM_GET_NUMBER,0,0); }

inline LRESULT __fastcall TModemInterface::get_props(LPMODEM_PROPS mp)
 {  return call(MODEMCM_GET_MODEM_PROPS,(LPARAM)mp,0); }

 inline LRESULT __fastcall TModemInterface::get_line_props(DWORD ln,LPMODEM_LINE_PROPS mp)
 {  return call(MODEMCM_GET_LINE_PROPS,ln,(LPARAM)mp); }

inline LRESULT __fastcall TModemInterface::get_line_stats(DWORD line_number,LPMODEM_LINE_STATS mls)
 { return call(MODEMCM_GET_LINE_STATS,line_number,(LPARAM)mls); }

inline DWORD   __fastcall TModemInterface::get_line_state(DWORD line_number)
 {
  MODEM_LINE_STATE mls;mls.dw_size = sizeof(mls);
  return call(MODEMCM_GET_LINE_STATE,line_number,(LPARAM)&mls) == GKH_RET_SUCCESS ? mls.line_state:0;
 }



inline LRESULT __fastcall TModemInterface::send(LPMPROTO_HEADER hdr,DWORD len)
 {  return call(MODEMCM_SEND,(LPARAM)hdr,len); }

inline LRESULT __fastcall TModemInterface::recv(LPMPROTO_HEADER hdr,DWORD len)
 {return call(MODEMCM_RECV,(LPARAM)hdr,len);}

inline LRESULT __fastcall TModemInterface::enum_lines(DWORD idx,LPMODEM_LINE_PROPS mlp)
 { return call(MODEMCM_ENUM_LINES,idx,(LPARAM)mlp); }

inline LRESULT __fastcall TModemInterface::set_number(DWORD number,LPDWORD old)
 {  return call(MODEMCM_SET_NUMBER,number,(LPARAM)old); }




#endif
