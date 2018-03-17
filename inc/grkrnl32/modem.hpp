#ifndef __MODEM_HPP
#define __MODEM_HPP

#include <modemmer.h>
#include <gkmodules.hpp>
#include <vector>
#include <map>
#include <kedatas.hpp>
#include <stdio.h>

using namespace KeRTL;
using namespace std;

class TModemBase;

  class modem_line
  {
   protected:
   TFASTMutex        locker;
   TModemBase     *  owner;            //Владелец линии

   DWORD             kadr_number;      //Номер кадра
   DWORD             line_number;      //Номер линии
   DWORD             line_state;       //Состояние линии
   DWORD             tx_speed  ;       //Скорость передачи в линии bps
   DWORD             rx_speed  ;       //Скорость приёма   в линии bps
   DWORD             connect_lost;     //Количество обрывов связи
   DWORD             tx_bytes;         //Передано байт
   DWORD             tx_kadrs;         //Передано кадров
   DWORD             tx_errors;        //Ошибок при передаче
   DWORD             tx_quality;       //Качество канала  на передачу * 100
   DWORD             tx_middle_speed;  //Средняя скорость передачи    * 100
   DWORD             tx_max_speed,tx_min_speed;
   DWORD             tx_zero_time_bytes;
   DWORD             tx_total_kadrs;
   DWORD             tx_total_errors;
   DWORD             tx_total_bytes;

   DWORD             rx_bytes;        //принято байт
   DWORD             rx_kadrs;        //принято кадров
   DWORD             rx_errors;       //ошибок при приёме
   DWORD             rx_quality;      //Качество приёма
   DWORD             rx_middle_speed; //Средняя скорость приёма   * 100
   DWORD             rx_max_speed,rx_min_speed;
   DWORD             rx_zero_time_bytes; // Количество принятых байт при 0 разнице во времени
   DWORD             rx_total_kadrs;  //Общее кол-во кадров
   DWORD             rx_total_errors;//Общее количество ошибок
   DWORD             rx_total_bytes; //Общее количество байт

   DWORD             modemmer_rx_kadrs;
   DWORD             modemmer_rx_bytes;
   DWORD             modemmer_tx_kadrs;
   DWORD             modemmer_tx_bytes;

   __int64           tx_update_time;
   __int64           rx_update_time;
   __int64           begin_connect_time;

  public:
           void  __fastcall update_stats_tx(DWORD bytes,DWORD kadrs,DWORD errors);
           void  __fastcall update_stats_rx(DWORD bytes,DWORD kadrs,DWORD errors);
           void  __fastcall update_modemmer_tx(DWORD bytes,DWORD kadrs);
           void  __fastcall update_modemmer_rx(DWORD bytes,DWORD kadrs);
  protected:         
   virtual void  __fastcall calc_tx_quality();
   virtual void  __fastcall calc_rx_quality();


   virtual void  __fastcall release(){set_state(MODEM_LINE_STATE_EXIST,false);}
   virtual DWORD __fastcall get_line_text  (wchar_t * text,DWORD text_sz) = 0;
   virtual DWORD __fastcall get_max_frame_size() = 0;
   virtual DWORD __fastcall get_tx_queue_size () = 0;
   virtual bool  __fastcall do_start (){return true;}
   virtual bool  __fastcall do_stop  (){return true;}
   virtual void  __fastcall on_connect   ();
   virtual void  __fastcall on_disconnect();

   public:
   modem_line(DWORD ln);
   virtual
   ~modem_line(){release();}
   void          __fastcall set_owner(TModemBase * _owner);
   virtual bool          __fastcall lock();
   virtual bool          __fastcall unlock();

   virtual void  __fastcall set_line_number(DWORD new_num);     
   virtual bool  __fastcall send        (LPMPROTO_HEADER mph,DWORD sz) = 0;
   void          __fastcall queue_rxdata(WORD otd_fa,LPBYTE data,DWORD len,bool first,DWORD addr_to = 0);
   virtual
   DWORD         __fastcall set_state   (DWORD state,bool set,bool notify = true);
   BOOL          __fastcall check_state (DWORD stfl,bool exact);

   virtual void  __fastcall begin_redirect(LPREDIRECT_ENTRY){};
   virtual void  __fastcall end_redirect  (LPREDIRECT_ENTRY){};


   bool          __fastcall start  ();//Запуск линии в работу
   bool          __fastcall stop   ();
   void          __fastcall connect(bool conn);
   __int64       __fastcall get_connect_time();

   DWORD         __fastcall get_state   (){DWORD ret;lock();ret = line_state;unlock();return ret;};
   bool          __fastcall get_stats   (LPMODEM_LINE_STATS ls);
   bool          __fastcall get_props   (LPMODEM_LINE_PROPS lp);
   virtual
   DWORD         __fastcall get_line_flags()= 0;
   virtual
   DWORD __fastcall refresh(DWORD sbl){return GKH_RET_ERROR;}/*Функция обновления данных по линии*/
   bool          operator < (modem_line  & l1);
   DWORD         __fastcall get_number  (){DWORD ret;lock();ret = line_number;unlock();return ret; }
   TModemBase *  __fastcall get_owner   (){TModemBase * ret; lock();ret = owner;unlock();return ret;}
   bool          __fastcall is_locked   () {return (locker.GetOwnerThreadId()==GetCurrentThreadId()) ? true:false;}
   bool          __fastcall is_worked   () {return check_state(MODEM_LINE_STATE_WORKED,false) ? true:false;}
   bool          __fastcall is_connected() {return check_state(MODEM_LINE_STATE_CONNECT,false)? true:false;}
   bool          __fastcall is_tx_ready () {return check_state(MODEM_LINE_STATE_TXREADY,true) ? true:false;}
   bool          __fastcall is_rx_ready () {return check_state(MODEM_LINE_STATE_RXDATA,false) ? true:false;}
  };


/*
 Хранение принятых данных данных
 в очередм MODEM_ADDR + data
*/
class TModemDataQueue:protected TSimpleDataQueue
{
 public :
 TModemDataQueue(){};

 int    __fastcall  put_data (LPMPROTO_HEADER hdr,bool first);
 int    __fastcall  put_data (LPMPROTO_HEADER hdr,LPVOID data,DWORD sz,bool first);
 int    __fastcall  put_data (LPMODEM_ADDR addr_to,LPMODEM_ADDR addr_from,WORD kadr_num,WORD fa,LPVOID data,DWORD sz,bool first);
 bool   __fastcall  peek_data(LPMODEM_ADDR addr_to,LPMODEM_ADDR addr_from,LPWORD kadr_num,LPWORD fa,LPVOID data,DWORD bsz,int * out = 0);
 bool   __fastcall  get_data (LPMODEM_ADDR addr_to,LPMODEM_ADDR addr_from,LPWORD kadr_num,LPWORD fa,LPVOID data,DWORD bsz,int * out = 0);
 LPMPROTO_HEADER __fastcall  peek_data(LPVOID buffer,DWORD bsz,int * rsz);
 LPMPROTO_HEADER __fastcall  get_data (LPVOID buffer,DWORD bsz,int * rsz);
 int    __fastcall  queue_count(){return QueueCount();};
 void   __fastcall  clear(){DropData();};
 void   __fastcall  drop_first(){DropFirst();}
 int    __fastcall  top_size(){return TopDataSize();}

 #ifdef _WIN64
 using TSimpleDataQueue::Lock;
 using TSimpleDataQueue::Unlock;
 #else
 TSimpleDataQueue::Lock;
 TSimpleDataQueue::Unlock;
 #endif

};


/*Базовый класс для реализации модемов*/
class TModemBase:public TGKModule
{
  protected:
  DWORD             modem_number;         //Номер модема
  TModemDataQueue   rx_queue;             //Общая приёмная очередь
  LPBYTE            temp_rd_buffer;
  DWORD             temp_rd_buffer_size;
  typedef std::map<DWORD,modem_line*> lines_map;
  lines_map       lines;   //список линий


  LRESULT     __fastcall    processing(DWORD cmd,LPARAM p1,LPARAM p2);
  void        __fastcall    release(LPVOID arg);
  BOOL        __fastcall    can_start(DWORD reason,LPARAM start_param);
  DWORD       __fastcall    start(DWORD reason,LPARAM start_param);
  DWORD       __fastcall    stop (DWORD reason);
  DWORD       __fastcall    lines_start(bool start);

  bool        __fastcall    read_settings();
  bool        __fastcall    write_settings();
  WORD        __fastcall    get_type(){return MT_MODEM;}

  bool        __fastcall    add_line(modem_line *);
  bool        __fastcall    remove_line(DWORD num);
  void        __fastcall    clear_lines();
  virtual
  void        __fastcall    free_line     (modem_line *) = 0;
  virtual
  DWORD       __fastcall    get_modem_flags()=0;

  LRESULT     __fastcall    recv          (LPMPROTO_HEADER mph,DWORD sz);
  virtual int __fastcall    convert_rx_data(LPWORD fa,LPBYTE in,int in_len,LPBYTE out,DWORD out_sz) = 0;
  virtual
  LRESULT     __fastcall    send (LPMPROTO_HEADER mph,DWORD sz);
  LRESULT     __fastcall    refresh_line(DWORD line_number,DWORD subline);

  LRESULT     __fastcall    get_line_state(DWORD line,LPMODEM_LINE_STATE mls);
  LRESULT     __fastcall    get_line_stats(DWORD ln,LPMODEM_LINE_STATS ls);
  LRESULT     __fastcall    get_line_props(DWORD num,LPMODEM_LINE_PROPS lp);
  LRESULT     __fastcall    enum_lines    (DWORD idx,LPMODEM_LINE_PROPS lp);
  LRESULT     __fastcall    get_props     (LPMODEM_PROPS);

  void         __fastcall   queue_rxdata(DWORD line_number,DWORD kadr_num,WORD otd_fa,LPVOID data,DWORD sz,bool first = false); 
  void         __fastcall   queue_rxdata(DWORD addr_to,DWORD line_number,DWORD kadr_num,WORD otd_fa,LPVOID data,DWORD sz,bool first = false);
  void         __fastcall   queue_rxdata(LPMPROTO_HEADER mph,bool first = false); 
  virtual
  DWORD        __fastcall   get_modem_info(wchar_t * buff,DWORD buff_len);
  virtual
  DWORD        __fastcall   get_lines_limit() = 0;
  void         __fastcall   notify_line_state (DWORD number,DWORD line_state,DWORD changes);
  virtual void __fastcall   line_state_changed(DWORD number,DWORD line_state,DWORD changes);
  friend                    modem_line;
  virtual
  LRESULT  __fastcall begin_redirect(LPREDIRECT_ENTRY);
  virtual
  LRESULT  __fastcall end_redirect  (LPREDIRECT_ENTRY);
  void     __fastcall notify_change_number(DWORD new_num,DWORD old_num);


  public:
  TModemBase(DWORD number=-1);
  DWORD        __fastcall get_modem_number();
  LRESULT      __fastcall set_modem_number(DWORD num,LPDWORD old_num);
  modem_line * __fastcall get_line(DWORD number);
  modem_line * __fastcall get_line_byindex(DWORD idx);
  DWORD        __fastcall get_lines_count();
  bool         __fastcall renumber_line(DWORD old_num,DWORD new_num);


};


inline void     __fastcall   TModemBase::queue_rxdata(DWORD line_number,DWORD kadr_num,WORD otd_fa,LPVOID data,DWORD sz,bool first )
{
  queue_rxdata(0,line_number,kadr_num,otd_fa,data,sz,first);
}


inline bool   __fastcall  TModemDataQueue::get_data (LPMODEM_ADDR addr_to,LPMODEM_ADDR addr_from,LPWORD kadr_num,LPWORD fa,LPVOID data,DWORD bsz,int * out )
{
 bool ret =   peek_data(addr_to,addr_from,kadr_num,fa,data,bsz,out);
 if(ret)
   DropFirst();
 return ret;  
}

  inline    bool     modem_line::operator < ( modem_line  & l)
  {
   return get_number() < l.get_number();
  }

 inline  bool __fastcall modem_line::lock()
 {
  #if 0 // _DEBUG
  char text[MAX_PATH];
  TGKThread * gkt = dynamic_cast<TGKThread*>(this);
  if(!gkt)    gkt = dynamic_cast<TGKThread*>(owner);
  sprintf(text,"Modem line lock tid %X owner %X %s",GetCurrentThreadId(),locker().OwningThread,gkt ? gkt->get_gkthread_name():"");
  TRACE(text,0);
  #endif
  return locker.Lock();
 }

 inline  bool __fastcall modem_line::unlock()
 {
  #if 0 //_DEBUG
  char text[MAX_PATH];
  TGKThread * gkt = dynamic_cast<TGKThread*>(this);
  if(!gkt)    gkt = dynamic_cast<TGKThread*>(owner);
  sprintf(text,"Modem line unlock tid %X %s",GetCurrentThreadId(),gkt ? gkt->get_gkthread_name():"");
  TRACE(text,0);
  #endif
  return locker.Unlock();
 }


#endif
