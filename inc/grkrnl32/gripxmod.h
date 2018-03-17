#ifndef __GRIPXMOD_H
#define __GRIPXMOD_H

#include <gripxmod_err.h>
#include <granit_dos.h>
#include "grnet_dos.h"
#include <modemmer.h>
#include <wsipx.h>
#include <string.h>

#define GRNET_WM_AS_SERVER  0
#define GRNET_WM_AS_CLIENT  1
#define GRNET_WM_UNDEFINED  (-1)

#define CCFL_TU_CLIENTS    0x80000000 


#pragma pack (push,1)


#define GRNET_REGDW_WORKMODE          L"WorkMode"
#define GRNET_REGDW_PUNUMBER          L"PuNumber"
#define GRNET_REGDW_SERVER_SOCKET     L"ServerSocket"
#define GRNET_REGDW_CLIENT_SOCKET     L"ClientSocket"
#define GRNET_REGDW_TX_RETRY_COUNT    L"TxRetyCount "
#define GRNET_REGDW_TX_RETRY_DELAY    L"TxRetyDelay "
#define GRNET_REGSTR_SERVER_NAME      "ServerName"
#define GRNET_REGSTR_CLIENT_NAME      "ClientName"
#define GRNET_REGBIN_TUCLIENTS        L"TuClients"
#define GRNET_REGSTR_PUNAME           L"PuName"




#define CLIENT_STATE_FIND_SERVER      0x00000001
#define CLIENT_STATE_DISCONNECTING    0x00000004
#define CLIENT_STATE_DISCONNECTED     0x00000008
#define CLIENT_STATE_MASK             0x00000007
#define CLIENT_STATE_WAIT_RECEIPT     0x80000000


#define GRNET_NOTIFY_PROTO        0x00800000
#define GRNET_NOTIFY_TXDATA       0x00400000
#define GRNET_NOTIFY_RXDATA       0x00200000
#define GRNET_NOTIFY_CLIENT_STATE 0x00100000
#define GRNET_NOTIFY_READY        0x00080000

#define GRNET_NCODE_TXDATA        0
#define GRNET_NCODE_RXDATA        0
#define GRNET_NCODE_RXDATA_ERROR  1
#define GRNET_NCODE_TXDATA_ERROR  1

#define GRNET_NCODE_TUCLIENTS     1512


#define GRNET_CFG_MODEMNUMBER     1
#define GRNET_CFG_WORKMODE        2
#define GRNET_CFG_PUNUMBER        4
#define GRNET_CFG_SERVERPARAM     8
#define GRNET_CFG_CLIENTPARAM     16
#define GRNET_CFG_TXPARAM         32
#define GRNET_CFG_PUNAME          64


#define GRNET_CFG_RESTARTMASK (GRNET_CFG_MODEMNUMBER|GRNET_CFG_WORKMODE|GRNET_CFG_PUNUMBER|GRNET_CFG_SERVERPARAM|GRNET_CFG_CLIENTPARAM)


typedef struct _GRNET_SETTINGS
{
 DWORD    modem_number;
 DWORD    work_mode;
 DWORD    pu_number;//Номер ПУ для запроса ответа
 char     server_name[GRNET_NAMES_LEN];
 DWORD    server_socket;
 char     client_name[GRNET_NAMES_LEN];
 DWORD    client_socket;
 DWORD    tx_retry_count;
 DWORD    tx_retry_delay;
 wchar_t  pu_name[MAX_PATH>>1];
}GRNET_SETTINGS,*LPGRNET_SETTINGS;

#define GRNETCM_SETTINGS          MCMD(MT_MODEM,1000)
/*p1 = LPGRNET_SETTINGS p2 = BOOL FALSE get/ TRUE set*/
#define GRNETCM_GET_CLIENT_INFO    MCMD(MT_MODEM,1001)
//p1 = номер линии  p2 = LPGRNET_CLIENT_INFO;

typedef struct _GRNET_CLIENT_INFO
{
 DWORD dw_size;
 sockaddr_ipx  host_addr;
 sockaddr_ipx  local_addr;
 WORD          client_id;
 DWORD         client_state;
 char          client_name[GRNET_NAMES_LEN];
 char          server_name[GRNET_NAMES_LEN];
}GRNET_CLIENT_INFO,*LPGRNET_CLIENT_INFO;


#define GRNETCM_GETDIAG_TABLE   MCMD(MT_MODEM,1002)
/*Команда получения таблицы диагностики*/
//p1 = lp_granit_diag,p2 = DWORD sz


#define GRNET_MAX_TU_CLIENTS    4

typedef struct _GRNET_TU_CLIENT
{
 DWORD dw_size;
 char  name[GRNET_NAMES_LEN];
 DWORD sock_num;
}GRNET_TU_CLIENT,*LPGRNET_TU_CLIENT;
                           
#ifdef __cplusplus
struct grnet_tu_client:public GRNET_TU_CLIENT
{
 grnet_tu_client(){dw_size = sizeof(*this); name[0] = 0;sock_num = 0;}
 grnet_tu_client(char * name,DWORD sock_num);
 grnet_tu_client(GRNET_TU_CLIENT & tc){(*this) = tc;};
 grnet_tu_client & operator =  (GRNET_TU_CLIENT & tc);
 int               __fastcall compare(GRNET_TU_CLIENT & tc);
 bool              operator < (GRNET_TU_CLIENT & tc){return  compare(tc)<0;}
};

 inline  grnet_tu_client::grnet_tu_client(char * _name,DWORD _sock_num)
 {
  dw_size = sizeof(*this);
  lstrcpynA(name,_name,sizeof(name));
  sock_num = _sock_num;
 }

 inline  grnet_tu_client & grnet_tu_client::operator =  (GRNET_TU_CLIENT & tc)
 {
  dw_size = sizeof(*this);
  lstrcpynA(name,tc.name,sizeof(name));
  sock_num = tc.sock_num;
  return *this;
 }

 inline int     __fastcall grnet_tu_client::compare(GRNET_TU_CLIENT & tc)
 {
   //int res = strncmpi(name,tc.name,sizeof(name));
   int res = _strnicoll(name,tc.name,sizeof(name));
   res =   (res == 0) ? (int)sock_num - (int)tc.sock_num : res;
   return res;
 }

inline int               operator == (grnet_tu_client & tc1,grnet_tu_client & tc2)
 { return (tc1.compare(tc2) == 0);}


#endif



#define GRNET_GET_TUCLIENT     MCMD(MT_MODEM,1004)
//Модификация клиента с разрешёнными операциями ТУ
//p1 = LPGRNET_TU_CLIENT ret = idx

#define GRNET_SET_TUCLIENT     MCMD(MT_MODEM,1005)
//Модификация клиента с разрешёнными операциями ТУ
//p1 = LPGRNET_TU_CLIENT ret = idx

#define GRNET_DEL_TUCLIENT     MCMD(MT_MODEM,1006)
//Удаление  клиента с разрешёнными операциями ТУ
//p1 = idx


#pragma pack (pop)



#endif
 