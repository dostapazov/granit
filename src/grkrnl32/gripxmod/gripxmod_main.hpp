/*Модуль клиента / сервера для на основе GRNET32.DLL*/

#ifndef __GRIPXMOD_HPP
#define __GRIPXMOD_HPP

#define _WINSOCKAPI_
#include <gripxmod.h>
#include <modem.hpp>
#include <KeThreads.hpp>
#include <kertlio.hpp>
#include <kestrm.hpp>
#include <wsockdef.h>
#include <otd.h>
#include <map>

using namespace std;


#define GRNET_TERMINATE_KEY   -1
#define GRNET_RX_KEY           1
#define GRNET_TX_KEY           2

#define GRNET_SERVER_RECEIPT_CONNECTION  255

/*Карта  диагностики групп  Гранита*/

class diag_map
{
 protected:
 TFASTMutex      locker;
 grdiag_map      gdmap;
 public:
 diag_map(){reset();}
 void  __fastcall reset();
 bool  __fastcall get_items(lp_grdiag_item gdi,DWORD begin = 0,DWORD count = MAX_CP_COUNT);
 DWORD __fastcall set_grdiag(DWORD cp_num,WORD diag);
 bool  __fastcall get_grdiag(DWORD cp_num,lp_grdiag_item gdi);
 WORD  __fastcall get_grdiag(DWORD cp_num);
 WORD  __fastcall get_grmask(DWORD cp_num);
 DWORD __fastcall mark_exist(DWORD cp,DWORD fa,DWORD grp);
 DWORD __fastcall set_otd_diag(lpotd_addr addr,DWORD otd_diag);
 DWORD __fastcall get_otd_diag(DWORD cp,DWORD fa,DWORD grp);
 bool  __fastcall lock(){return locker.Lock();}
 bool  __fastcall unlock(){return locker.Unlock();}
};



/*
   Очередь данных на передачу хранится в виде LPGRNET_KADR
*/

typedef std::map<sotd_param_addr,DWORD> tu_objects_map;


class grnet_line : public modem_line,public TGKThread
{
 protected:
 BOOL              work_mode; // Режим работы TRUE как клиент, FALSE как клиент в составе сервера
 char              server_name[GRNET_NAMES_LEN];
 char              client_name[GRNET_NAMES_LEN];
 DWORD             server_socket;
 DWORD             client_socket;
 DWORD             client_id;
 DWORD             pu_number; // Номер ПУ
 DWORD             client_state;
 DWORD             tx_retry_count; // Количество попыток передачи кадра при отсутствии квитанции
 DWORD             tx_retry_delay; // Тайм аут для получения квитанции
 BOOL              tu_enabled;
 tu_objects_map    active_tu;      //Список активных ту


 TIpxAddress       host_addr;      //Сетевой адрес партнёра
 TIpxSocket        ipx_socket;     //
 TIoCompletionPort icp;            //Порт завершения ввода/вывода
 OVERLAPPED        rx_overlapped;
 OVERLAPPED        tx_overlapped;

 DWORD             rx_kadr_number;  //Номер принятого кадра
 BYTE              rx_buffer[1024+sizeof(NOTIFY_CODE)];// приёмный буффер
 LPGRNET_KADR      rx_kadr;
 DWORD             rx_len;
 BYTE              tx_buffer[1024+sizeof(NOTIFY_CODE)];
 LPGRNET_KADR      tx_kadr;
 DWORD             tx_len;
 LPNOTIFY_CODE     rx_proto_notify;
 LPNOTIFY_CODE     tx_proto_notify;

 DWORD             tx_kadr_number;
 TSimpleDataQueue  tx_queue;//Очередь на передачу
 __int64           wait_receipt_timer;
 diag_map *        diag;

 //Функции линии
 DWORD   __fastcall get_line_flags(){return MLPF_RXABLE|MLPF_TXABLE;}
 DWORD   __fastcall get_max_frame_size(){return GRNET_DATA_LEN;}
 DWORD   __fastcall get_line_text  (wchar_t * text,DWORD text_sz);


 void    __fastcall on_connect     ();
 void    __fastcall on_disconnect  ();
 void    __fastcall release();
 bool    __fastcall do_start ();
 bool    __fastcall do_stop  ();


 void    __fastcall Terminate();
 bool    __fastcall BeforeExecute();
 int     __fastcall Execute  ();
 void    __fastcall BeforeTerminate();
 bool    __fastcall CheckTerminateRequest();


 void __fastcall begin_connection();//Инициация начала соединения

 /*Сетевые функции*/
 bool    __fastcall create_socket();
 void    __fastcall destroy_socket();
 public    :
 void    __fastcall disconnect();
 protected :
 int     __fastcall net_send (LPBYTE data,DWORD len);
 void    __fastcall do_send_kadr(LPGRNET_KADR kadr);
 void    __fastcall repeat_send_kadr();
 void    __fastcall handle_receive(DWORD bytes);
 void    __fastcall handle_receipt(LPGRNET_KADR);
 void    __fastcall receipt_kadr(LPGRNET_KADR);
 void    __fastcall handle_grnet_command(DWORD cmd,LPBYTE data,DWORD len);
 void    __fastcall handle_tlm_data(lp_gratony_kadr kadr,DWORD len);
 void    __fastcall handle_tlm_diag(DWORD begin_cp,LPWORD diag_data,DWORD cp_count);
 void    __fastcall send_next_kadr();
 void    __fastcall wait_receipt(bool enable) {set_client_state(CLIENT_STATE_WAIT_RECEIPT,enable,false);}
 DWORD   __fastcall get_wait_receipt_time(){return wait_receipt_timer ? NS100_MSEC(GetTime()-wait_receipt_timer):0;}
 /*Служебные функции*/
 void    __fastcall send_grnet_query(lpotd_addr addr);
 void    __fastcall make_query(DWORD pu,DWORD cp,DWORD fa,DWORD grp,bool undo_query);
 void    __fastcall make_pu_ready(DWORD diag);
 DWORD   __fastcall refresh(DWORD );
 void    __fastcall send_pu_name();

 /*Функции управления*/
 void     __fastcall tu_query   (lp_gratony_kadr kadr);
 void     __fastcall tu_begin   (lp_gratony_kadr kadr);
 void     __fastcall tu_reset   (lp_gratony_kadr kadr);
 void     __fastcall tu_on_more (lp_gratony_kadr kadr);
 void     __fastcall tu_off_less(lp_gratony_kadr kadr);
 void     __fastcall tu_cancel  (lp_gratony_kadr kadr);

 void    __fastcall on_timeout();
 void    __fastcall check_connect();
 WORD    __fastcall next_tx_kadr_number(){if(++tx_kadr_number == 0)  tx_kadr_number = 1;return tx_kadr_number;}
 bool    __fastcall send (LPMPROTO_HEADER mph,DWORD sz);
 DWORD   __fastcall get_tx_queue_size(){return tx_queue.QueueCount(); }

 const char * __fastcall get_gkthread_name(){return "TGRNetModule::grnet_line Thread";}

 public:
  grnet_line(DWORD ln,GRNET_SETTINGS & gs,diag_map * _diag);
  void  __fastcall set_tx_retry_param(DWORD retry_count,DWORD retry_delay){lock();tx_retry_count = retry_count;tx_retry_delay = retry_delay;unlock();}
  DWORD __fastcall get_client_info(LPGRNET_CLIENT_INFO cli);
  DWORD __fastcall set_client_state(DWORD state,bool set,bool notify = true);
  bool  __fastcall check_client_state(DWORD state,bool exact=false);
  DWORD __fastcall get_client_state();
  void  __fastcall connect_establish(LPGRNET_BEGIN_CONNECTION bc,TIpxAddress & remote_addr,BOOL tu_enable);
  bool  __fastcall send_data  (DWORD cmd,LPVOID data,DWORD data_len);
  void  __fastcall send_ready (DWORD cp,bool always = false);
  void  __fastcall update_modemmer_tx(DWORD bytes,DWORD kadrs){modem_line::update_modemmer_tx(bytes,kadrs);}
  void  __fastcall connect        (bool conn);
  DWORD __fastcall get_tuobj_active_state (otd_addr & addr,DWORD param);
  void  __fastcall set_tuobj_active_state (otd_addr & addr,DWORD param,DWORD );

};


class TGRNetModule:public TModemBase,public TGKThread //В случае работы сервера
{
  protected:
  typedef TModemBase Inherit ;

  GRNET_SETTINGS  settings;
  diag_map        diag_table; // Таблица диагностики
  DWORD           connected_clients;
  grnet_tu_client tu_clients[GRNET_MAX_TU_CLIENTS];


  DWORD __fastcall get_modem_flags();

  void        __fastcall     line_state_changed(DWORD number,DWORD line_state,DWORD changes);
  void        __fastcall     free_line(modem_line * line) { delete line;};
  DWORD       __fastcall     get_lines_limit() ;
  void        __fastcall     do_init();
  TIpxSocket        server_socket;
  TIoCompletionPort server_icp_port;

  friend grnet_line;

  void    __fastcall diag_reset();
  LRESULT __fastcall diag_get_items (lp_grdiag_item gdi,DWORD begin,DWORD cnt);
  void    __fastcall diag_notify(DWORD cp_num,DWORD count);


  DWORD          __fastcall get_config_data(DWORD mask,LPVOID buf,DWORD bsz);
  bool           __fastcall set_config_data(DWORD mask,LPVOID buf,DWORD bsz);
  bool           __fastcall compare_config_data(DWORD mask,LPVOID buf,DWORD bsz,LPDWORD ch_mask,BOOL * restart);
  bool           __fastcall check_config_data  (DWORD mask,LPVOID buf,DWORD bsz);
  DWORD          __fastcall get_modem_info(wchar_t * buff,DWORD buff_len);

  DWORD         __fastcall get_window_module (wchar_t * buf,DWORD bsz);
  LRESULT       __fastcall processing (DWORD cmd,LPARAM p1,LPARAM p2);
  void          __fastcall apply_settings();
  bool          __fastcall read_settings  ();
  bool          __fastcall write_settings ();
  BOOL          __fastcall can_start(DWORD reason,LPARAM p2);
  DWORD         __fastcall start(DWORD reason,LPARAM start_param);
  DWORD         __fastcall stop (DWORD reason);
  void          __fastcall release(LPVOID c);
  DWORD         __fastcall get_client_info(DWORD line_num,LPGRNET_CLIENT_INFO);
  void          __fastcall Terminate();
  bool          __fastcall BeforeExecute();
  int           __fastcall Execute();
  void          __fastcall handle_receive(LPGRNET_KADR kadr,DWORD len);
  grnet_line *  __fastcall find_client(LPGRNET_BEGIN_CONNECTION bc,const TIpxAddress & addr);
  bool          __fastcall create_server_socket();

  LRESULT __fastcall send          (LPMPROTO_HEADER,DWORD sz);
  LRESULT __fastcall send_otd      (grnet_line * line,lpotd_proto op);
  void    __fastcall send_otd_query(grnet_line * line,lpotd_proto op);
  void    __fastcall send_otd_data (grnet_line * line,lpotd_proto op);
  void    __fastcall send_otd_ctrl (grnet_line * line,lpotd_proto op);
  void    __fastcall make_order    (bool undo);

  void    __fastcall send_line_ready(DWORD line_num,DWORD cp,bool always = false);
  void    __fastcall send_line_ready(grnet_line * line,DWORD cp,bool always = false);
  void    __fastcall send_common_diag(DWORD cp,DWORD diag);

  const char * __fastcall get_gkthread_name(){return "TGRNetModule: server thread";}

 // Функции предобразования протокола
 int     __fastcall convert_ts   (LPBYTE dest,DWORD dest_sz,lp_gratony_kadr atony_kadr);
 int     __fastcall convert_tit  (LPBYTE dest,DWORD dest_sz,lp_gratony_kadr atony_kadr);
 int     __fastcall convert_ready(LPBYTE dest,DWORD dest_sz,lp_gratony_kadr atony_kadr);
 int     __fastcall grnet2otd    (lp_gratony_kadr atony_kadr,LPBYTE dest,DWORD dest_sz);
 int     __fastcall otd2grnet    (lpotd_proto op,LPBYTE buffer,DWORD buff_sz,LPDWORD Cmd);
 int     __fastcall convert_rx_data(LPWORD fa,LPBYTE in,int in_len,LPBYTE out,DWORD out_sz);

 LRESULT __fastcall get_tu_client (LPGRNET_TU_CLIENT gtc,DWORD idx);
 LRESULT __fastcall set_tu_client (LPGRNET_TU_CLIENT gtc,DWORD idx);
 LRESULT __fastcall del_tu_client (DWORD idx);
 DWORD   __fastcall find_tu_client(char * name,DWORD sock_num);
 BOOL    __fastcall is_client_tu_enabled(char * name,DWORD sock_num);
 grnet_line * __fastcall find_worked_client(char * name,DWORD sock_num);
 grnet_line * __fastcall get_client_byidx(DWORD idx);
 DWORD        __fastcall get_mem_used();
 DWORD        __fastcall get_pu_name(wchar_t * buff,DWORD bsz);
 virtual DWORD   __fastcall check_license(){return MOD_LICENSE_REGISTERED; }

  public:
  TGRNetModule();
  const GRNET_SETTINGS & __fastcall get_settings(){return settings;} 

};


#endif


