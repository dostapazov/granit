#ifndef _TLM_NET_CLIENT_HPP
#define _TLM_NET_CLIENT_HPP

#include <wsockdef.h>
#include <modem_proto.h>
#include <mediumdb.h>
#include <otdm_proto.h>
#include <kethreads.hpp>
#include <vector>
#include <map>
#include <string>
#include <kertlio.hpp>
#include <modem_proto.h>
#include <otdm_proto.h>
#include <tlmpainter.hpp>



using namespace KeRTL;
using namespace std;

#define TLM_CLIENT_COMMON_TEMINATE L"$TlmClientTerminateEvent$"

#define WR_TERMINATE_CODE 0
#define WR_NETWORK_CODE   1






typedef std::map<DWORD,wstring>       names_map;
typedef std::vector<mdb_kadr>         kadr_list;
typedef std::vector<mdb_record_ex>    rec_list;

typedef std::vector<mdb_kadr_entry>   entryes_list;
typedef std::map<DWORD,entryes_list>  kadr_entryes;
typedef std::map<DWORD,HWND>          open_kadr_map;






/*
 При начале работы читаются данные конфигурации сохранённые
 при завершении.

 Устанавливается связь с сервером, после чего
 вызывается список кадров, и заказываются кадры которые
 были заказанны с одновременной синхронизацией
 Синхронизация  содержимого по изменению MDB_KADR::content_time,
 изображения  по изменению MDB_KADR::content_time image

*/

#define CS_NOTCONNECTED 0
#define CS_CONNECTING 1
#define CS_CONNECTED  2

#define TLMV_DATA_FOLDER   TEXT("TLMV_DATA\\")
#define TLMV_IMAGES_FOLDER TEXT("IMAGES\\")

#define TC_CMD_SET_BKGND     (WM_APP+100)
#define TC_CMD_SET_ENTRYES   (WM_APP+101)
#define TC_CMD_UPDATE_KADR   (WM_APP+102)
#define TC_CMD_SET_CAPTION   (WM_APP+103)
#define TC_CMD_REPAINT_KADR  (WM_APP+104)



class tlm_client: public KeRTL::TThread
{
 protected:
 static KeRTL::THeap  heap;
 TFASTMutex  locker;
 TFile       file;
 string      data_dir;
 //Список имен / длинных строк
 names_map   names;
 //Список  кадров на сервере
 kadr_list     kadrs;
 //Список содержимого кадров
 kadr_entryes  k_entryes;

 //список  заказанных кадров с содержимым  кадра

 //Сопоставление ID кадра - файлу фонового изображения



 //Список   записей  mdb_record
 rec_list            records;

 //Список открытых кадров
 open_kadr_map       opened_kadrs;


 HANDLE              terminate_event; // Событие завершения
 WSAEVENT            net_event;       // Сетевое событие
 TSynchroSet         events_set;
 TIPSocket       *   client_socket;
 string              server_addr;
 int                 server_port;
 int                 connected;
 int                 kpa_timer; 
 DWORD               packet_counter;
 modem_proto_buffer  recv_buffer;

 int                   rx_bytes_count;
 int                   rx_kadrs_count;
 int                   rx_errors_count;
 int                   tx_bytes_count;
 int                   tx_kadrs_count;

 DWORD                 image_for_kadr;

 DWORD  __fastcall OnException(DWORD ExceptionCode,LPEXCEPTION_POINTERS);
 bool   __fastcall BeforeExecute();
 int    __fastcall Execute();
 void   __fastcall BeforeTerminate();
 void   __fastcall Terminate(){PulseEvent(terminate_event);KeRTL::TThread::Terminate();}

 void   __fastcall connect_to_server();
 void   __fastcall disconnect_from_server();
 void   __fastcall handle_network_events();
 void   __fastcall on_net_read      (DWORD err);
 void   __fastcall on_net_write     (DWORD err);
 void   __fastcall on_net_connect   (DWORD err);
 void   __fastcall on_net_disconnect(DWORD err);
 void   __fastcall handle_rx();
 void   __fastcall on_timeout();
 void   __fastcall handle_mproto(LPOTDM_PROTO_HEADER omph);
 void   __fastcall handle_enum_kadrs(LPOTDM_PROTO_HEADER omph,bool end);
 void   __fastcall handle_image     (LPOTDM_PROTO_HEADER omph,bool beg,bool end);
 void   __fastcall handle_entryes   (LPOTDM_PROTO_HEADER omph,bool beg,bool end);
 void   __fastcall handle_records   (LPOTDM_PROTO_HEADER omph,bool beg,bool end);
 void   __fastcall handle_records_changed(LPOTDM_PROTO_HEADER omph,bool beg,bool end);
 void   __fastcall handle_record_names(LPOTDM_PROTO_HEADER omph,bool short_name);

 void   __fastcall send_keep_alive();
 DWORD  __fastcall send_command(bool begin,bool finish,DWORD cmd,DWORD err,LPVOID data,DWORD data_size );
 DWORD  __fastcall send_data(LPMPROTO_HEADER mph);
 bool   __fastcall add_kadr   (MDB_KADR & kadr, DWORD & ch_flags);
 bool   __fastcall add_record (mdb_record & rec,DWORD mask);
 void   __fastcall set_text(DWORD id,wstring & str);
 void   __fastcall request_kadr_image(DWORD kadr_id);
 void   __fastcall update_opened_kadr_entryes(DWORD kadr_id,HWND kadr_wnd = NULL);

 void   __fastcall store_server_data();
 void   __fastcall restore_server_data();
 void   __fastcall restore_orders();
 void   __fastcall refresh_opened_kadrs(bool always = false);

 public:

 tlm_client()
 {
  client_socket = NULL;net_event = NULL;
 };
 virtual ~tlm_client();
 bool   __fastcall start_work(char * server_name,int server_port,char * client_name,char * data_dir);

 string  __fastcall  get_text            (DWORD id );
 wstring __fastcall  get_wtext           (DWORD id );
 string  __fastcall  get_kadr_image_file (DWORD kid);
 wstring __fastcall  get_kadr_image_fileW(DWORD kid);
 string  __fastcall  get_data_dir();
 wstring __fastcall  get_data_dirW();
 void    __fastcall  set_bad_ready();

 bool    __fastcall  read_lock   (DWORD to = -1);
 bool    __fastcall  read_unlock ();
 bool    __fastcall  write_lock  (DWORD to = -1);
 bool    __fastcall  write_unlock();

 bool   __fastcall start_work(const char * server_addr,int server_port);

 kadr_list::size_type
       __fastcall find_kadr(DWORD id);
 bool  __fastcall get_kadr(DWORD id,mdb_kadr & kadr,bool by_offs);
 rec_list::size_type
       __fastcall find_record(DWORD id);
 bool  __fastcall get_record (DWORD id,mdb_record_ex & rec,bool by_offs);
 void  __fastcall send_order_kadr(DWORD id,bool order = true);
 int   __fastcall get_rx_bytes_count(){return rx_bytes_count;}
 int   __fastcall get_rx_kadrs_count(){return rx_kadrs_count;}
 int   __fastcall get_tx_bytes_count(){return tx_bytes_count;}
 int   __fastcall get_tx_kadrs_count(){return tx_kadrs_count;}
 int   __fastcall get_rx_errors_count(){return rx_errors_count;}
 int   __fastcall order_kadr(DWORD id,HWND kadr_wnd);
 int   __fastcall unorder_kadr(DWORD id,HWND kadr_wnd);
 int   __fastcall is_connected(){return connected;}
 int   __fastcall send_tu(DWORD rec_id,DWORD op_code);
 static
 wstring __fastcall get_term_event_name();

 void __fastcall receipt_kadr(DWORD kadr_id);
 void __fastcall receipt     (DWORD rec_id,bool end);
};

 inline bool   __fastcall tlm_client::read_lock(DWORD to)
 {
  return locker.Lock();
 }

 inline bool   __fastcall tlm_client::read_unlock()
 {
   return locker.Unlock();
 }

 inline bool   __fastcall tlm_client::write_lock(DWORD to)
 {
   return locker.Lock();
 }

 inline bool   __fastcall tlm_client::write_unlock()
 {
  return locker.Unlock();
 }

/*Для преобразования строк*/
 struct char2mb:public unary_function<char,wchar_t>
 {
  wchar_t operator ()(char & first)
  {
   wchar_t ret;
   MultiByteToWideChar(CP_ACP,MB_COMPOSITE ,&first,1,&ret,1);
   return ret;
  }
 };

 struct mb2char:public unary_function<wchar_t,char>
 {
  char operator ()(wchar_t & first)
  {
   char ret;
   WideCharToMultiByte(CP_ACP,WC_COMPOSITECHECK ,&first,1,&ret,1,NULL,NULL);
   return ret;
  }
 };




#endif


