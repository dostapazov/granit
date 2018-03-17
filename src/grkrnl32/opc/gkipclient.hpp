/*
 Реализация класса Обеспечивающего связь с TCP/IP сервером
 Ядра - Гранит-Н
*/


#ifndef _GKIPCLIENT_HPP_
#define _GKIPCLIENT_HPP_

#include  <wsockdef.h>
#include  <uniopc.h>
#include  <modem_proto.h>
#include  <otdm_proto.h>
#include  <vector>
#include "dataserv_config.h"
#include <map.h>


typedef std::vector<TDataForOPCServer> opc_data;
typedef std::multimap<DWORD,opc_data::iterator>  TKadrLinks;// Карта сопоставления
                                                             // номеров заказанных
                                                             // кадров и Устройств

struct  obj_num_kid
{
 DWORD  kadr_id;
 DWORD  number;
 obj_num_kid(){kadr_id = number = -1;}
 obj_num_kid(DWORD kid,DWORD num):kadr_id(kid),number(num){}
 obj_num_kid  & operator = (const obj_num_kid & src){kadr_id = src.kadr_id;number = src.number;return *this;} 
};

typedef std::multimap<DWORD,obj_num_kid>             TObjLinks ; // карта принадлежности
                                                                 // объектов - кадрам и номерам


using namespace KeRTL;

class TGKIPClient:public TThread
{
 protected:
 BOOL        wait_terminate;
 TSynchroSet synchro_set;
 TEvent      terminate_event;

 BOOL        client_connected;
 DWORD       client_disconnecting;
 
 WSAEVENT    client_event;       //Событие клиента
 TIPSocket * client_socket;      //Гнездо клиента
 char        server_name[MAX_PATH];
 int         server_port;
 modem_proto_buffer   in_buffer; // Приёмный буфер

 TKadrLinks        kadr_links;
 TObjLinks         obj_links;
 
 opc_data          data; // Данные для OPC-сервера

 void    __fastcall do_connect();
 void    __fastcall do_disconnect();
 void    __fastcall handle_socket_events();
 void    __fastcall on_socket_read      (DWORD err);
 void    __fastcall on_socket_connect   (DWORD err);
 void    __fastcall on_socket_write     (DWORD err);
 void    __fastcall on_socket_disconnect(DWORD err);

 void    __fastcall handle_mproto(LPMPROTO_HEADER mph);
 void    __fastcall handle_kadr(LPOTDM_PROTO_KADR pk);
 void    __fastcall handle_kadr_entryes(LPOTDM_PROTO_KADR_ENTRYES pke);
 void    __fastcall handle_kadr_records(LPOTDM_PROTO_KADR_RECORDS pkr);
 void    __fastcall handle_records(LPOTDM_PROTO_RECORDS krecs);


 void    __fastcall send_kadr_orders();
 int     __fastcall send_data(LPBYTE data,DWORD len);

 void    __fastcall Terminate(){terminate_event.SetEvent(true);}
 public:
 TGKIPClient();
 ~TGKIPClient();

 bool __fastcall BeforeExecute();
 void __fastcall BeforeTerminate();
 int  __fastcall Execute();
 TDataForOPCServer * __fastcall get_data(){return &data.front();}

};



#endif

