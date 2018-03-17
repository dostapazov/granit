#ifndef _PROCESSMDM_INC_HPP
#define _PROCESSMDM_INC_HPP

#include <modem.hpp>
#include "processmdm.h"
#include "kedatas.hpp"
using namespace KeRTL;

class process_line : public modem_line,public TGKThread
{
 protected:
 PMSETTINGS            settings;
 TEvent                term_event;
 TProcess              process;
 HANDLE                write_pipe;
 HANDLE                stdin_pipe;
 HANDLE                read_pipe;
 HANDLE                stdout_pipe;
 modem_proto_buffer    mpb;
 SECURITY_ATTRIBUTES   proc_sa;
 SECURITY_DESCRIPTOR   sd;
 STARTUPINFOW          startup_info;
 PROCESS_INFORMATION   proccess_info;
 TEventSimpleDataQueue send_queue;

 bool          __fastcall run_process();
 void          __fastcall cleanup_process();

 const char  * __fastcall get_gkthread_name(){ return "TProcess Line thread";}
 void  __fastcall Terminate(){term_event.SetEvent(true);TGKThread::Terminate();}
 int   __fastcall Execute  ();
 bool  __fastcall BeforeExecute();
 void  __fastcall BeforeTerminate();

 DWORD __fastcall get_line_flags(){return MLPF_RXABLE|MLPF_TXABLE;}
 DWORD __fastcall get_max_frame_size(){return -1;}
 DWORD __fastcall get_tx_queue_size (){return send_queue.QueueCount();}
 DWORD __fastcall get_line_text  (wchar_t * text,DWORD text_sz) ;

 bool  __fastcall do_start ();
 bool  __fastcall do_stop  ();
 bool  __fastcall send     (LPMPROTO_HEADER mph,DWORD sz) ;
 void  __fastcall send_processing();
 void  __fastcall recv_processing();
 void  __fastcall handle_recv();
 void  __fastcall on_disconnect();
 DWORD __fastcall OnException(DWORD ExceptionCode,LPEXCEPTION_POINTERS);

 public   :
 process_line(DWORD number);
 void   __fastcall set_start_param(LPPMSETTINGS sp){lock();memcpy(&settings,sp,sizeof(*sp));unlock();}

};

class TProcessModem:public TModemBase
{
 protected:
 PMSETTINGS         settings;
 bool    __fastcall read_settings  ();
 bool    __fastcall write_settings ();
 DWORD   __fastcall get_config_data(DWORD mask,LPVOID buf,DWORD bsz);
 bool    __fastcall set_config_data(DWORD mask,LPVOID buf,DWORD bsz);
 bool    __fastcall compare_config_data(DWORD mask,LPVOID buf,DWORD bsz,LPDWORD ch_mask,BOOL * restart);
 bool    __fastcall check_config_data  (DWORD mask,LPVOID buf,DWORD bsz);
 int     __fastcall convert_rx_data(LPWORD fa,LPBYTE in,int in_len,LPBYTE out,DWORD out_sz) ;
 void    __fastcall free_line     (modem_line *) ;
 DWORD   __fastcall get_lines_limit() {return 1;};
 BOOL    __fastcall can_start(DWORD reason,LPARAM p2);
 DWORD   __fastcall start(DWORD reason,LPARAM start_param);
 DWORD   __fastcall get_window_module (wchar_t * buf,DWORD bsz);
 DWORD   __fastcall get_modem_flags()  { return MPF_DATA_ACCEPTOR;}
 public:
 TProcessModem();



};

#endif
