#ifndef _MODBUSMOD_HPP_INC_
#define _MODBUSMOD_HPP_INC_

#include <wsockdef.h>

#include "modbusmod.h"
#include <modem.hpp>
#include <KeRTLio.hpp>
#include <kedatas.hpp>
#include <rx_buffer.hpp>
#include <tregstry.hpp>
#include <otd_storage.hpp>
#include <kestrm.hpp>
#include <kertlio.hpp>
#include <modbus_proto.h>

using namespace KeRTL;


#define MODBUS_TERMINATE_EVENT L"$modbus_lines_terminate#%X"

#define TIMER_RESOLUTION 10

class modbus_line : public modem_line,public TGKThread
{
  protected:
     MODBUS_POINT            modbus_point;
     TEventSimpleDataQueue  * tx_queue;
     otd_storage              groups;
     TEvent                 * term_event;
     TEvent                 * line_event;
     TIPSocket              * socket;
     TCommPort              * comm_port;
     rx_buffer              rxb;
     WORD                   frame_number;
     DWORD                  watch_dog_timer;
     DWORD                  watch_dog_time;
     lpotd_proto            requested;
     DWORD __fastcall get_max_frame_size() {return 1024;}
     DWORD __fastcall get_tx_queue_size () {return 0;}
     DWORD __fastcall get_line_flags    () {return MLPF_TXABLE|MLPF_RXABLE|MLPF_DATASOURCE;}
     DWORD __fastcall get_line_text     (wchar_t * text,DWORD text_sz);
     bool  __fastcall send              (LPMPROTO_HEADER mph,DWORD sz) ;
     void  __fastcall do_recv_group(lpotd_proto op,DWORD parts = -1,DWORD lo=0,DWORD hi=-1,DWORD to = 0);

     bool  __fastcall handle_modbus_respond(LPMODBUS_FRAME mf,DWORD len);

     void  __fastcall set_diag(lpotd_proto op,DWORD mod_diag,int number,DWORD diag_state,bool rcv);
     void  __fastcall set_diag(proto_pointer & p,DWORD mod_diag,int number,DWORD diag_state,bool rcv);

     void  __fastcall set_diag(DWORD diag,bool send);


     bool  __fastcall BeforeExecute         ();
     bool  __fastcall before_socket_execute ();
     bool  __fastcall before_comport_execute();
     bool  __fastcall socket_connect   ();
     void  __fastcall socket_disconnect();

     void  __fastcall socket_handle_event();
     void  __fastcall socket_handle_connect   (DWORD err);
     void  __fastcall socket_handle_disconnect(DWORD err);
     void  __fastcall socket_handle_recv      (DWORD err);
     void  __fastcall socket_handle_send      (DWORD err);
     void  __fastcall socket_send_queue       ();
     void  __fastcall do_socket_send          (LPVOID buf,int len,DWORD notify);

     void  __fastcall do_handle_comport_frame ();
     void  __fastcall do_handle_ethernet_frame();


     void  __fastcall on_connect   ();
     void  __fastcall on_disconnect() ;
     void  __fastcall on_timer     () ;


     void  __fastcall socket_timer();

     void  __fastcall BeforeTerminate   ();
     void  __fastcall before_socket_terminate ();
     void  __fastcall before_comport_terminate();

     int   __fastcall Execute           ();

     int   __fastcall socket_execute ();
     int   __fastcall comport_execute();


     bool  __fastcall do_start();
     bool  __fastcall do_stop ();

     void  __fastcall refresh           (proto_pointer & p);
     void  __fastcall next_refresh_from (proto_pointer & p);
     DWORD __fastcall OnException(DWORD ExceptionCode,LPEXCEPTION_POINTERS);
     void  __fastcall do_recv_point_name();



  public:
             __fastcall modbus_line    (LPMODBUS_POINT mdev = NULL);
       void  __fastcall get_point      (LPMODBUS_POINT mdev);
       bool  __fastcall set_config     (LPMODBUS_POINT mdev);
     LRESULT __fastcall group_enum     (LPMODBUS_GROUP mbg,DWORD idx);

     LRESULT __fastcall do_create_group(LPMODBUS_GROUP mbg);
     LRESULT __fastcall group_create   (LPMODBUS_GROUP mbg);
     LRESULT __fastcall group_get      (LPMODBUS_GROUP mbg);
     LRESULT __fastcall group_update   (LPMODBUS_GROUP mbg);
     LRESULT __fastcall group_delete   (LPMODBUS_GROUP mbg);

       void  __fastcall write        (TStream & s);
       void  __fastcall read         (TStream & s);
       DWORD __fastcall refresh(DWORD sbl);
     void    __fastcall fetch_comm_event(DWORD dev_no,bool log);
static WORD  __fastcall get_otd_fa   (DWORD reg_no);
static int   __fastcall get_term_event_name(TGKModule * mod,wchar_t * name,int sz);
       void  __fastcall do_report(DWORD evt_type,wchar_t * rep_text,LPVOID data,DWORD dt_sz);
};

#define IOPRT_KEY_TERMINATE  (0)
#define IOPRT_KEY_COMPORT    (1)
#define IOPRT_KEY_SOCKET     (2)
#define IOPRT_KEY_TX         (3)

#define SOCKET_BUFF_SIZE     4096

class TModbusModem : public TModemBase
{
 protected:

  MODBUS_MOD_CONFIG dev_cfg;
  DWORD             rep_id;

  void        __fastcall free_line     (modem_line * ml) ;
  DWORD       __fastcall get_modem_flags(){return  MPF_DATA_SOURCE;}
  int         __fastcall convert_rx_data(LPWORD fa,LPBYTE in,int in_len,LPBYTE out,DWORD out_sz) ;
  DWORD       __fastcall get_lines_limit() {return MODEM_ADDR_MAXVALUE;}
  DWORD       __fastcall get_config_data    (DWORD mask,LPVOID buf,DWORD bsz);
  bool        __fastcall set_config_data    (DWORD mask,LPVOID buf,DWORD bsz);
  bool        __fastcall compare_config_data(DWORD mask,LPVOID buf,DWORD bsz,LPDWORD ch_mask,BOOL * restart);
  bool        __fastcall read_settings();
  bool        __fastcall write_settings();
  modbus_line * __fastcall get_modbus_line(DWORD dev_num);

  LRESULT __fastcall point_create   (LPMODBUS_POINT mdev);
  LRESULT __fastcall point_update   (LPMODBUS_POINT mdev);
  LRESULT __fastcall point_remove   (DWORD dev_no);
  LRESULT __fastcall point_get      (LPMODBUS_POINT mdev);
  LRESULT __fastcall point_enum     (LPMODBUS_POINT mdev,DWORD idx);

  LRESULT __fastcall group_enum      (LPMODBUS_GROUP mbg,DWORD idx);

  LRESULT __fastcall group_cr_get_upd_del(DWORD cmd,LPMODBUS_GROUP mbg);

  BOOL    __fastcall can_start(DWORD reason,LPARAM p2);
  DWORD   __fastcall start    (DWORD reason,LPARAM p2);
  DWORD   __fastcall stop     (DWORD reason);

  LRESULT __fastcall processing         (DWORD cmd,LPARAM p1,LPARAM p2);
  DWORD   __fastcall get_window_module  (wchar_t * buf,DWORD bsz);

  void    __fastcall do_recv_puname();

  int     __fastcall write_lines    (TStream & s);
  int     __fastcall read_lines     (TStream & s);
  void    __fastcall do_recv_pu_diag(DWORD diag );

  LRESULT __fastcall send (LPMPROTO_HEADER mph,DWORD sz);

  LRESULT __fastcall fetch_comm_event(DWORD line_no,DWORD dev_no,bool log);
  virtual  DWORD      __fastcall check_license(){return MOD_LICENSE_REGISTERED; }

 public   :
 TModbusModem();
  void       __fastcall do_report(DWORD evt_type,wchar_t * rep_text,LPVOID data,DWORD dt_sz);
  WORD       __fastcall get_type ()  {return MT_MODEM_MODBUS;}
  DWORD      __fastcall get_mem_used();

 };

#endif


