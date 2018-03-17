
#ifndef _GRATONY32_INC_
#define _GRATONY32_INC_

#include <gkmodules.h>
#include <modemmer.h>



#define REG_ATONY_WORKMODE   L"Mode"
#define REG_ATONY_PORTNUM    L"Port"
#define REG_ATONY_PORTSPEED  L"PortSpeed"
#define REG_ATONY_PORTPARITY L"PortParity"
#define REG_ATONY_PORTSTOPS  L"PortStops"
#define REG_ATONY_PUNUMBER   L"PuNumber"


#define KADR_ERR              0 /*Ошибочный кадр*/
#define KADR_DATA             1
#define KADR_SYNC             2
#define KADR_RX_READY         3
#define KADR_TX_QUERY         4
#define KADR_TX_ENABLE        5
#define KADR_TX_CONFIRM       6
#define KADR_TX_ERROR         7
#define KADR_DATA_ERROR       8
#define KADR_TX               0x8000
#define KADR_RX               0x4000


#define ATONY32_NOTIFY_PROTO  0x80000000
#define ATONY32_NOTIFY_TXDATA 0x40000000
#define ATONY32_NOTIFY_RXDATA 0x20000000

#define MASTER_MODE FALSE
#define SLAVE_MODE  TRUE



#pragma pack(push,1)

#define ATONY_CFG_WORKMODE   0x0001
#define ATONY_CFG_PUNUM      0x0002
#define ATONY_CFG_UART       0x0004
#define ATONY_CFG_MODEMNUM   0x0008


typedef struct _ATONY32_SETTINGS
{
 DWORD modem_number;
 DWORD work_mode;
 DWORD pu_number;
 DWORD port_num;
 DWORD port_speed;
 DWORD port_oddeven;
 DWORD port_stopbits;
}ATONY32_SETTINGS,*LPATONY32_SETTINGS;

#pragma pack(pop)



#endif
