#ifndef __GKIPMODEM_H_INC
#define __GKIPMODEM_H_INC

#define IDS_TCP_SERVER  1000
#define IDS_TCP_CLIENT  1001

#define IDS_CONNECTION_WAIT 1500
#define IDS_LINE_NO         1501
#define IDS_CONNECT_WITH    1502
#define IDS_FREE_LINE       1503

#define IDS_REP_CONNECT_ESTABLISH   2000
#define IDS_REP_CLIENT_CONNECTED    2001
#define IDS_REP_RECV_ERROR          2002
#define IDS_REP_SYNC_ERROR          2003
#define IDS_REP_CHECKSUM_ERROR      2004
#define IDS_REP_LOSTKADR_ERROR      2005
#define IDS_REP_DISCONNECT          2006
#define IDS_REP_CONNECT_ERROR       2007
#define IDS_REP_TRANSMIT_ERROR      2008
#define IDS_REP_KPA_ERROR           2009
#define IDS_REP_NO_RX_ERROR         2010
#define IDS_REP_ZERO_CHECKSUM_ERROR 2011

#ifndef RC_INVOKED

#include <winsock2.h>
#include <modemmer.h>

#pragma pack (push,1)

#define REGDW_IPM_FLAGS       L"IPM_Flags"
#define REGDW_IPM_PORT        L"IPM_Port"
#define REGDW_IPM_BINDCOUNT   L"IPM_BindCount"
#define REGBIN_IPM_BINDLIST   L"IPM_BindList"
#define REGSTR_IPM_SERVERADDR  "IPM_ServerAddr"
#define REGBIN_IPM_LINES      L"IPM_Lines"
#define REGDW_IPM_CLIENT_OPTIONS L"IPM_ClientOpts"

#define IPM_MAX_ADDR_COUNT  32

typedef struct _GKIPM_SETTINGS
{
 DWORD dw_size;
 DWORD modem_number;                   //Номер модема
 DWORD flags;                          //Клиент или сервер
 DWORD port;                           //Номер порта
 DWORD bind_count;                     //Количество BIND адресов
 DWORD client_options;                 // Дополнительные параметры клиента
 DWORD bind_addrs [IPM_MAX_ADDR_COUNT];//Список BIND  адресов
 char  server_addr[MAX_PATH];          //Адрес сервера
}GKIPM_SETTINGS,*LPGKIPM_SETTINGS;
//Если bind_count == 0 То привязка  TIPAddress((DWORD)0,port);

#define CCFL_IPM_FLAGS       0x80000000
#define CCFL_IPM_PORT        0x40000000
#define CCFL_IPM_BINDPARAMS  0x20000000
#define CCFL_IPM_SERVERADDR  0x10000000
#define CCFL_IPM_CLIENT_OPTS 0x08000000

#define CCFL_IPM_CLIENTS     0x00800000

#define IPM_FLAG_ASCLIENT    0x00000001

typedef struct _GKIPM_LINE_ADDRS
{
 DWORD        dw_size;
 DWORD        line_number;
 sockaddr_in  local_addr;
 char         host_name[MAX_PATH];
 DWORD        host_port;
}GKIPM_LINE_ADDRS, *LPGKIPM_LINE_ADDRS;

//p1 = LPGKIPM_LINE_ADDRS
#define CMIPM_GETLINE_ADDRS  MCMD(MT_MODEM_TCP,1000)

//p1 = LPGKIPM_LINE_ADDRS
#define CMIPM_SETLINE_ADDRS  MCMD(MT_MODEM_TCP,1001)

//p1 = LPGKIPM_LINE_ADDRS
#define CMIPM_CREATE_LINE    MCMD(MT_MODEM_TCP,1002)

//p1 = line_number
#define CMIPM_DELETE_LINE    MCMD(MT_MODEM_TCP,1003)
#define CMIPM_GET_LINE_OPTS  MCMD(MT_MODEM_TCP,1004)
#define CMIPM_SET_LINE_OPTS  MCMD(MT_MODEM_TCP,1005)

#define  IPM_CLOPT_OTD_REPLACE_ZERO_PUNUMBER    0x00001
#define  IPM_CLOPT_OTD_REPLACE_PUNUMBER_ALWAYS  0x00003
#define  IPM_CLOPT_OTD_ALARM_ARCHIVE_SUPPORT    0x00004
#define  IPM_CLOPT_OTD_ALARM_ARCHIVE_ERASE      0x00008

#define IPM_CLOPT_OTD_SUPPORT                   0x00007

#define IPM_CLOPT_COMPATIBLE_MODE               0x80000






#pragma pack (pop)
#endif
#endif

