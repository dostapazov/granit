#ifndef __MODBUS_MOD_INC__
#define __MODBUS_MOD_INC__

#include <gkmodules.h>
#include <modemmer.h>
#include "modbus_err.h"




#define IDS_LINE_TEMPL   (IDS_USER_STR+0)

#ifndef RC_INVOKED  // Not a Resource Compiler

#include <otd.h>
#include <modbus_proto.h>

#pragma pack (push,1)

#define REGDW_MODBUS_PUNUMBER         L"PuNumber"
#define REGSTR_MODBUS_PUNAME          L"PuName"
#define REGDW_MODBUS_COM_PORT         L"ComPort"
#define REGDW_MODBUS_COM_SPEED        L"ComSpeed"
#define REGDW_MODBUS_COM_ODDEVEN      L"ComOddEven"
#define REGDW_MODBUS_COM_STOPBITS     L"ComStopbits"
#define REGSTR_MODBUS_HOST_ADDR       L"HostAddr"
#define REGDW_MODBUS_HOST_PORT        L"HostPort"
#define REGDW_MODBUS_MEDIA_TYPE       L"MediaType"
#define REGBIN_MODBUS_POINTS          L"Points"


#define CCFL_PUNUMBER        0x80000000
#define CCFL_PUNAME          0x40000000
#define CCFL_MEDIATYPE       0x20000000

#define CCFL_COMPORT_PARAMS  0x08000000
#define CCFL_HOST_PARAMS     0x04000000

#define CCFL_POINTS         0x00080000

#define MBNF_TXRX               0x00000100   // Флаг уведомления о приеме/передаче

#define MBNC_TX                0x00000001    //Код уведомления о передаче
#define MBNC_RX                0x00000002    //Код уведомления о приеме

#define MBNC_TXTCP             0x00000003    //Код уведомления о передаче
#define MBNC_RXTCP             0x00000004    //Код уведомления о приеме


#define MBNC_RXKADR            0x00000005    //Код уведомления о приеме кадра
#define MBNC_TXKADR            0x00000006    //Код уведомления о передаче кадра


#define MBNF_DEV_CHANGED       0x00000200   //Флаг Уведомления об изменении состава линии

#define MBNC_DEV_CREATED       0x00000001
#define MBNC_DEV_DELETED       0x00000002
#define MBNC_DEV_UPDATED       0x00000003

#define MBNC_GROUP_CREATED     0x00000010
#define MBNC_GROUP_DELETED     0x00000020
#define MBNC_GROUP_UPDATED     0x00000030

#define MB_GROUP_DATA_CHANGED  0x00000100 //Уведомление об изменении группы

#define MODBUS_MEDIA_TYPE_COMPORT  0
#define MODBUS_MEDIA_TYPE_ETHERNET 1
#define MODBUS_MEDIA_TYPE_COMPORT_VIA_ETHERNET 2

#define MODBUS_MODEM_TYPE  MT_MODEM_TYPE(MT_MODEM_MODBUS)

#define OTD_FA_FROM_MODBUS_CMD(x) (  (x == MODBUS_READ_DISCRETE_OUTPUT|| x == MODBUS_READ_DISCRETE_INPUT) ? OTD_FA_DISCRETE : (x == MODBUS_READ_HOLDING_REGISTERS ||x == MODBUS_READ_INPUT_REGISTERS ) ? OTD_FA_ANALOG : -1)


typedef struct _MODBUS_MOD_CONFIG
{
 DWORD   dw_size;
 DWORD   modem_number;
 DWORD   pu_number;
 wchar_t pu_name[128];
}MODBUS_MOD_CONFIG, * LPMODBUS_MOD_CONFIG;

#define MODBUS_CONFIG_BUFF_SIZE (sizeof(MODULE_CONFIG_DATA)+sizeof(MODBUS_MOD_CONFIG))

#define MODBUS_DEVICES_LIMIT 247


typedef struct _MODBUS_POINT
{
 DWORD   dw_size;
 DWORD   point_no;
 wchar_t point_name[MAX_PATH];
 DWORD   media_type;
 struct{
   DWORD port_num;
   DWORD port_speed;
   DWORD port_oddeven;
   DWORD port_stopbits;
   DWORD port_delay;
  }comport_settings;

  struct
  {
   wchar_t host_addr[1024];
   DWORD   host_port;
  }host_params;
 #ifdef __cplusplus
 _MODBUS_POINT(){memset(this,0,sizeof(*this)); dw_size = sizeof(*this); point_no = -1;}
 _MODBUS_POINT(const _MODBUS_POINT & src){*this = src;}
 _MODBUS_POINT & operator = (const  _MODBUS_POINT & src ) {memcpy(this,&src,sizeof(*this)); return *this;}
 #endif
}MODBUS_POINT, *LPMODBUS_POINT;


//Перечисление КП
//p1 = LPMODBUS_POINT, p2 = (DWORD) idx;
#define MBCM_ENUM_POINT  MCMD(MT_MODEM_MODBUS,1000)


//Прочитать параметры кп по номеру
//p1 = LPMODBUS_POINT, p2 = (DWORD) dev_num;
#define MBCM_GET_POINT        MCMD(MT_MODEM_MODBUS,1001)

//Создание устройства
//p1 = LPMODBUS_POINT

#define MBCM_CREATE_POINT     MCMD(MT_MODEM_MODBUS,1002)

//Удаление устройства
//p1 = DWORD dev_number

#define MBCM_REMOVE_POINT    MCMD(MT_MODEM_MODBUS,1003)

//Изменение устройства
//p1 = LPMODBUS_POINT
#define MBCM_UPDATE_POINT    MCMD(MT_MODEM_MODBUS,1004)



typedef struct _MODBUS_GROUP
{
  DWORD dw_size;
  WORD point_no;
  WORD group_no;
  WORD dev_no;
  WORD command;
  WORD start_no;
  WORD count;
  DWORD refresh_time;
  DWORD respond_time;
  DWORD requested;
  LONG  timer;
  DWORD last_error;
  DWORD error_count;
  DWORD real_respond_time;
  DWORD real_respond_time_min;
  DWORD real_respond_time_max;
#ifdef __cplusplus
 _MODBUS_GROUP(){memset(this,0,sizeof(*this)); dw_size = sizeof(*this);point_no = dev_no = group_no = -1;}
 _MODBUS_GROUP(const _MODBUS_GROUP & src){*this = src;}
 _MODBUS_GROUP & operator = (const _MODBUS_GROUP & src)
 { memcpy(this,&src,sizeof(*this)); return *this;}

#endif
}MODBUS_GROUP, *LPMODBUS_GROUP;

/*

*/

#define MBCM_ENUM_GROUP   MCMD(MT_MODEM_MODBUS,1005)
// p1 = LPMODBUS_GROUP , p2 index

#define MBCM_CREATE_GROUP MCMD(MT_MODEM_MODBUS,1006)
// p1 = LPMODBUS_GROUP

#define MBCM_GET_GROUP    MCMD(MT_MODEM_MODBUS,1007)
// p1 = LPMODBUS_GROUP

#define MBCM_UPDATE_GROUP MCMD(MT_MODEM_MODBUS,1008)
// p1 = LPMODBUS_GROUP

#define MBCM_DELETE_GROUP MCMD(MT_MODEM_MODBUS,1009)
//p1 = dev_no,p2 = GROUP_NO


#define MBCM_SERVICE_FETCH_COMMEVENT_CTRL   MCMD(MT_MODEM_MODBUS,2000)
#define MBCM_SERVICE_FETCH_COMMEVENT_LOG    MCMD(MT_MODEM_MODBUS,2001)


#pragma pack (pop)

#endif // RC_INVOKED
#endif // __MODBUS_MOD_INC__
 