#ifndef __GKIPMODEM_HPP_INC
#define __GKIPMODEM_HPP_INC

#include <wsockdef.h>
#include <modem.hpp>
#include <KeRTLio.hpp>
#include <kedatas.hpp>
#include <vector>
#include "gkipmodem_mod.h"

using namespace std;


using namespace KeRTL;

#define IPLINE_BUFFSZ   (8192)

class ip_line: public modem_line, protected TGKThread
{
 protected:
 char                   server_addr[MAX_PATH];
 DWORD                  server_port;
 TIPAddress             host_addr;
 TEvent                 terminate_event;
 TWaitableTimer         timer;
 WSAEVENT               network_event;
 DWORD                  no_rx_timer;
 DWORD                  no_tx_timer;
 TSynchroSet            ss;
 TIPSocket  *           socket;
 modem_proto_buffer     mpb;
 WORD                   last_pktnum;
 TEventSimpleDataQueue  send_queue;
 DWORD                  client_options;

 BOOL                   sending;
 WORD                   pkt_num;
 BOOL                   connect_error_log;

 typedef std::vector<QWORD>  alarms_list_t;
 alarms_list_t               alarms_list;     //список архивов
 BOOL                        otd_alarms_active;
 QWORD                       alarm_current;   //Архив который принимается в настоящее время
 int                         alarm_timeout;

 bool  __fastcall alarms_archive_handle(LPMPROTO_HEADER mph,sotd_proto & sop);
 void  __fastcall alarms_list_request  ();
 int   __fastcall alarm_print_time     (wchar_t * text,int bsz,QWORD timestamp);
 void  __fastcall alarm_handle_command (lpotd_addr addr,LPOTD_ARCH_HEADER ah,DWORD error);
 void  __fastcall alarm_add_archieves  (bool new_arch,lpotd_addr addr,LPOTD_ARCH_LIST al,bool is_end );
 void  __fastcall alarm_arch_get       (lpotd_addr addr,DWORD error,LPOTD_ARCH_HEADER ah,LPOTD_ARCH_LIST al,bool is_end);
 void  __fastcall alarm_start_download ();
 void  __fastcall alarm_timer          ();

 void  __fastcall send_keep_alive();
 DWORD __fastcall get_line_flags(){return MLPF_RXABLE|MLPF_TXABLE;};
 DWORD __fastcall get_line_text  (wchar_t * text,DWORD text_sz) ;
 DWORD __fastcall get_max_frame_size() {return IPLINE_BUFFSZ;}
 DWORD __fastcall get_tx_queue_size () {return send_queue.QueueCount() ;}
 bool  __fastcall do_start  ();
 bool  __fastcall do_stop   ();
 void  __fastcall do_connect();
 void  __fastcall on_connect();
 void  __fastcall on_disconnect() ;
 bool  __fastcall BeforeExecute();
 int   __fastcall Execute();
 void  __fastcall BeforeTerminate();
 void  __fastcall Terminate(){terminate_event.SetEvent(true);TThread::Terminate();}
 int   __fastcall read_from_socket();
 void  __fastcall handle_network_events();
 void  __fastcall otd_recv_prehandle(LPMPROTO_HEADER mph);
 void  __fastcall handle_recv(DWORD err);
 void  __fastcall handle_oob(DWORD err);
 void  __fastcall handle_disconnect(DWORD err);
 void  __fastcall handle_connect(DWORD err);
 void  __fastcall send_processing();
 bool  __fastcall do_send(LPMPROTO_HEADER mph);
 DWORD __fastcall send_data(LPBYTE ptr, DWORD sz);
 void  __fastcall on_timer();
 void  __fastcall send_query_arch_list();
 DWORD __fastcall OnException(DWORD ExceptionCode,LPEXCEPTION_POINTERS exp);

 public   :
 ip_line(DWORD number,DWORD client_options);
 ip_line(DWORD number,char * server_name,DWORD server_port,DWORD client_options);
 bool          __fastcall send              (LPMPROTO_HEADER mph,DWORD sz) ;
 bool          __fastcall accept_connection (SOCKET s,TIPAddress & host_addr);
 void          __fastcall get_line_addrs    (LPGKIPM_LINE_ADDRS addrs);
 void          __fastcall set_line_addrs    (LPGKIPM_LINE_ADDRS addrs);
 void          __fastcall ipm_report        (DWORD type,char    * descr,LPVOID data = NULL,DWORD data_sz = 0);
 void          __fastcall ipm_report        (DWORD type,wchar_t * descr,LPVOID data = NULL,DWORD data_sz = 0);
 void          __fastcall ipm_arch_report   (DWORD type,char    * descr,LPVOID data = NULL,DWORD data_sz = 0);
 void          __fastcall ipm_arch_report   (DWORD type,wchar_t * descr,LPVOID data = NULL,DWORD data_sz = 0);
 const char  * __fastcall get_gkthread_name (){return "ip-line";}
 const char  * __fastcall get_server_addr   ()  {return server_addr;}
       int     __fastcall get_server_port   ()  {return server_port;}
       DWORD   __fastcall get_packet_num    ()  { while(0 == (++pkt_num)); return pkt_num;}
       int     __fastcall get_client_options()  {return client_options;}
       void    __fastcall set_client_options(DWORD opts);
};

class TGkIPModem:public TModemBase , protected TGKThread
{
 protected:
 GKIPM_SETTINGS      settings;
 TIPSocket           *server_sockets[IPM_MAX_ADDR_COUNT];
 TEvent              terminate_event;
 WSAEVENT            server_socket_event;
 TSynchroSet         ss;
 DWORD               used_lines[MODEM_ADDR_MAXVALUE];
 DWORD               report_id;
 DWORD               arch_rep_id;

 const char* __fastcall get_gkthread_name();
 int         __fastcall Execute();
 bool        __fastcall BeforeExecute();
 void        __fastcall BeforeTerminate();

 bool        __fastcall read_settings();
 bool        __fastcall write_settings();
 DWORD       __fastcall get_config_data    (DWORD mask,LPVOID buf,DWORD bsz);
 bool        __fastcall set_config_data    (DWORD mask,LPVOID buf,DWORD bsz);
 bool        __fastcall compare_config_data(DWORD mask,LPVOID buf,DWORD bsz,LPDWORD ch_mask,BOOL * restart);
 DWORD       __fastcall get_window_module  (wchar_t * buf,DWORD bsz);
 LRESULT     __fastcall processing         (DWORD cmd,LPARAM p1,LPARAM p2);
 DWORD       __fastcall get_modem_flags();

 WORD        __fastcall    get_type(){return MT_MODEM_TCP;}

 void        __fastcall Terminate();
 void        __fastcall delete_sockets();
 void        __fastcall release(LPVOID);
 BOOL        __fastcall can_start(DWORD reason,LPARAM p2);
 BOOL        __fastcall can_stop (DWORD reason);
 DWORD       __fastcall start(DWORD reason,LPARAM p2);
 DWORD       __fastcall stop (DWORD reason);
 DWORD       __fastcall get_modem_info(wchar_t * buff,DWORD buff_sz);

 void        __fastcall free_line (modem_line *) ;
 DWORD       __fastcall get_lines_limit() {return MODEM_ADDR_MAXVALUE;}
 int         __fastcall convert_rx_data(LPWORD fa,LPBYTE in,int in_len,LPBYTE out,DWORD out_sz) ;
 void        __fastcall on_socket_event();
 LRESULT     __fastcall get_line_addrs(LPGKIPM_LINE_ADDRS addrs);
 LRESULT     __fastcall set_line_addrs(LPGKIPM_LINE_ADDRS addrs);

 void        __fastcall line_state_changed(DWORD number,DWORD line_state,DWORD changes);
 DWORD       __fastcall get_mem_used();
 void        __fastcall read_client_lines ();
 void        __fastcall write_client_lines();
 LRESULT     __fastcall create_line  (LPGKIPM_LINE_ADDRS addrs);
 LRESULT     __fastcall delete_line  (DWORD line_number);
 LRESULT     __fastcall get_line_opts(DWORD line_number,LPDWORD opts);
 LRESULT     __fastcall set_line_opts(DWORD line_number,  DWORD opts);

 virtual   DWORD   __fastcall check_license(){return MOD_LICENSE_REGISTERED; }
  void       __fastcall reg_reports();


 public   :
 TGkIPModem();
 DWORD       __fastcall find_free_line_number();
 ip_line *   __fastcall find_free_line();
 void        __fastcall ipm_report(bool arch,DWORD type,char    * descr,LPVOID data = NULL,DWORD data_sz = 0);
 void        __fastcall ipm_report(bool arch,DWORD type,wchar_t * descr,LPVOID data = NULL,DWORD data_sz = 0);
 protected:
 static BOOL WINAPI TGkIPModem::accept_checker(  IN LPWSABUF lpCallerId, IN LPWSABUF lpCallerData,
                             IN OUT LPQOS lpSQOS,IN OUT LPQOS lpGQOS,                             IN LPWSABUF lpCalleeId,OUT LPWSABUF lpCalleeData,                             OUT GROUP FAR * g,IN DWORD dwCallbackData);

};

inline  DWORD   __fastcall    TGkIPModem::get_modem_flags()
 {
  DWORD ret = MPF_DATA_SOURCE;
  lock();
  if(!(settings.flags&IPM_FLAG_ASCLIENT))
      ret  =  MPF_DATA_ACCEPTOR;
  unlock();
  return ret;
 }


#endif
