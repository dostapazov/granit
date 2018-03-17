#ifndef _VTDCNT_HPP_
#define _VTDCNT_HPP_

#include <wsockdef.h>
#include <modem.hpp>
#include <KeRTLio.hpp>
#include <kedatas.hpp>
#include <rx_buffer.hpp>
#include "vtdcnt.h"
#include <tregstry.hpp>
#include <otd_storage.hpp>
#include <kestrm.hpp>

using namespace KeRTL;





#pragma pack(push,1)
   struct group_timer{
   DWORD     timer_value;
   DWORD     timer_counter;
   };
#pragma pack(pop)


class vtd_line: public modem_line
{
 protected:
   VTDDEV_PARAM     dev_param;
   otd_storage      groups;

   vtd_num_date_time  dev_date_time;
   vtd_base_config    base_config;

   DWORD       __fastcall get_line_text  (wchar_t * text,DWORD text_sz) ;
   DWORD       __fastcall get_max_frame_size(){return 1024;}
   DWORD       __fastcall get_tx_queue_size() {return 0;}
   DWORD       __fastcall get_line_flags()    {return MLPF_TXABLE|MLPF_RXABLE|MLPF_DATASOURCE;}
   bool        __fastcall do_start ();
   bool        __fastcall do_stop  ();
   void        __fastcall on_connect()    ;
   void        __fastcall on_disconnect() ;
   bool        __fastcall send        (LPMPROTO_HEADER mph,DWORD sz) ;
   void        __fastcall send_group(proto_pointer,DWORD parts = -1,DWORD lo=0,DWORD hi=-1,DWORD to = 0);

   void        __fastcall refresh_std(DWORD mask = -1);
   void        __fastcall refresh_vg (DWORD mask = -1);
   void        __fastcall refresh_y  (DWORD mask = -1);

   void        __fastcall refresh_grp_std(proto_pointer ptr);
   bool        __fastcall is_group_exist(DWORD num);


   bool        __fastcall handle_respond_std(lpvtd_query_kadr qk,lpvtd_resp_kadr vk);
   void        __fastcall handle_param_std(lpvtd_query_kadr qk,lpvtd_resp_kadr vk);
   void        __fastcall handle_curr_param_std(lpvtd_query_kadr qk,lpvtd_resp_kadr vk);



   bool        __fastcall handle_respond_vg (lpvtd_query_kadr qk,lpvtd_resp_kadr vk);
   bool        __fastcall handle_respond_y  (lpvtd_query_kadr qk,lpvtd_resp_kadr vk);

   void  __fastcall send_dev_query(LPVOID data,DWORD sz);
   void  __fastcall set_diag(DWORD diag,bool send = false);
   void  __fastcall set_line_number(DWORD new_num);
   static
   WORD __fastcall  calc_vtd_crc(LPBYTE buf,DWORD len);
 public:
  vtd_line():modem_line(-1){   ZeroMemory(&dev_date_time,sizeof(dev_date_time));ZeroMemory(&base_config,sizeof(base_config));
};
   vtd_line(VTDDEV_PARAM & param);

   void __fastcall on_timer();

   bool       __fastcall update_device(LPVTDDEV_PARAM dp);

   bool        __fastcall enum_groups (DWORD idx,LPVTDDEV_GROUP grp);
   bool        __fastcall get_group   (DWORD number,LPVTDDEV_GROUP grp);
   bool        __fastcall create_group(LPVTDDEV_GROUP grp);
   bool        __fastcall delete_group(DWORD number);
   bool        __fastcall update_group(DWORD number,LPVTDDEV_GROUP grp);

   void        __fastcall get_dev_num_date_time(lpvtd_num_date_time dttm){CopyMemory(dttm,&dev_date_time,sizeof(dev_date_time));}
   void        __fastcall get_dev_base_config(lpvtd_base_config bc){CopyMemory(bc,&base_config,sizeof(base_config));}
   bool        __fastcall get_group_data(LPVTD_GET_GROUP_DATA dg);


   void        __fastcall get_param(LPVTDDEV_PARAM dp){ if (dp) memcpy(dp,&dev_param,sizeof(*dp));}
   bool        __fastcall handle_respond(lpvtd_query_kadr qk,lpvtd_resp_kadr vk);
   void        __fastcall swrite(TStream & s);
   void        __fastcall sread (TStream & s);
   DWORD       __fastcall refresh(DWORD sbl);
   DWORD       __fastcall get_dev_type(){return dev_param.type;}

};
/*
 Опрос линий  осуществляется поочерёдно по кругу

*/

class TVtdCntModem:public TModemBase,protected TGKThread
{
 protected:
 VTDCNTMOD_CONFIG mod_config;
 BOOL             is_connected;
 DWORD            not_connected_time;
 TSynchroSet      ss;
 TEvent           term_event;
 TIPSocket *      socket;
 WSAEVENT         socket_event;
 TWaitableTimer   wt_timer;
 TSynchroSet      sync_set;

 BYTE             last_query[MAX_PATH];//Последний отправленный вызов
 BYTE             last_query_size;     //  Длина последнего запроса
 __int64          last_query_time;     //  Время отправки вызова
 TSimpleDataQueue query_queue;         //Очередь на передачу
 rx_buffer        rxb;
 DWORD            rep_id;


 void     __fastcall do_report(DWORD evt_type,wchar_t * rep_text,LPVOID data,DWORD dt_sz);

 void __fastcall change_puname();
 /*Функциональность нити*/
  void  __fastcall Terminate(){TGKThread::Terminate();term_event.SetEvent(true);}
  bool  __fastcall BeforeExecute();
  void  __fastcall BeforeTerminate();
  int   __fastcall Execute();

 bool       __fastcall check_dev_param(LPVTDDEV_PARAM dp);
 LRESULT    __fastcall enum_device  ( LPVTDDEV_PARAM dev,DWORD idx );
 LRESULT    __fastcall get_device   ( LPVTDDEV_PARAM dev,DWORD num );
 vtd_line * __fastcall get_device   ( DWORD num );
 LRESULT    __fastcall create_device( LPVTDDEV_PARAM dev);
 LRESULT    __fastcall delete_device(DWORD num);
 LRESULT    __fastcall update_device( LPVTDDEV_PARAM dp,DWORD number);

 LRESULT    __fastcall enum_groups (DWORD dev_num,DWORD idx,LPVTDDEV_GROUP grp);
 LRESULT    __fastcall get_group   (DWORD dev_num,DWORD number,LPVTDDEV_GROUP grp);
 LRESULT    __fastcall create_group(DWORD dev_num,LPVTDDEV_GROUP grp);
 LRESULT    __fastcall delete_group(DWORD dev_num,DWORD number);
 LRESULT    __fastcall update_group(DWORD dev_num,DWORD number,LPVTDDEV_GROUP grp);
 LRESULT    __fastcall get_dev_num_date_time(DWORD num,lpvtd_num_date_time dttm);
 LRESULT    __fastcall get_dev_base_config  (DWORD num,lpvtd_base_config bc);
 LRESULT    __fastcall get_dev_group_data(LPVTD_GET_GROUP_DATA dg);

 LRESULT     __fastcall    send (LPMPROTO_HEADER mph,DWORD sz);


 void       __fastcall do_connect();
 void       __fastcall handle_socket_events();
 void       __fastcall on_socket_connect   (DWORD err);
 void       __fastcall on_socket_disconnect(DWORD err);
 void       __fastcall on_socket_write     (DWORD err);
 void       __fastcall on_socket_read      (DWORD err);
 void       __fastcall handle_timer();

 bool       __fastcall process_vtd_respond();

 LRESULT     __fastcall processing(DWORD cmd,LPARAM p1,LPARAM p2);
 void        __fastcall free_line (modem_line *) ;

 BOOL    __fastcall can_start(DWORD reason,LPARAM p2);
 DWORD   __fastcall start(DWORD reason,LPARAM p2);
 DWORD   __fastcall stop (DWORD reason);
 void    __fastcall do_send_dev_query();

 const char  * __fastcall get_gkthread_name();
 DWORD __fastcall OnException(DWORD ExceptionCode,LPEXCEPTION_POINTERS);
 DWORD   __fastcall on_exception  (lpexcept_record er);
 void       __fastcall send_pu_diag(DWORD diag);


 public:
 TVtdCntModem();
 WORD        __fastcall    get_type(){return MT_MODEM_VTD;}


 DWORD       __fastcall get_modem_flags(){return  MPF_DATA_SOURCE;}

 void        __fastcall read_devices (TRegsReader & rr);
 void        __fastcall write_devices(TRegsWriter & wr);

 void        __fastcall read_devices (TStream & s);
 void        __fastcall write_devices(TStream & s);

 bool        __fastcall read_settings();
 bool        __fastcall write_settings();
 DWORD       __fastcall get_config_data    (DWORD mask,LPVOID buf,DWORD bsz);
 bool        __fastcall set_config_data    (DWORD mask,LPVOID buf,DWORD bsz);
 bool        __fastcall compare_config_data(DWORD mask,LPVOID buf,DWORD bsz,LPDWORD ch_mask,BOOL * restart);
 DWORD       __fastcall get_window_module  (wchar_t * buf,DWORD bsz);
 DWORD       __fastcall get_lines_limit() {return MODEM_ADDR_MAXVALUE;}
 int         __fastcall convert_rx_data(LPWORD fa,LPBYTE in,int in_len,LPBYTE out,DWORD out_sz) ;
 DWORD       __fastcall get_mem_used();
 void        __fastcall send_dev_query(LPVOID data,DWORD sz);
 bool        __fastcall chek_server_connect(){return is_connected ? true:false;}

};


#endif


