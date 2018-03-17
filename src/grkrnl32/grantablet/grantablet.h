#ifndef _GRAN_TABLET_H
#define _GRAN_TABLET_H
#include <modemmer.h>

#define CCFL_IOMODULE   0x80000000
#define CCFL_IODEVICE   0x40000000
#define CCFL_IOSPEED    0x20000000
#define CCFL_DBFILE     0x10000000
#define CCFL_IOWATCHDOG 0x08000000
#define CCFL_IONOTRESET 0x04000000

#define CCFL_OUT_FLOORS 0x00800000


#define GRTBL_REGSTR_IOMODULE_W    L"IoModule"
#define GRTBL_REGSTR_IODEVICE_W    L"IoDevice"
#define GRTBL_REGDW_IOSPEED_W      L"IoSpeed"
#define GRTBL_REGSTR_DBFILENAME_W  L"DbFileName"
#define GRTBL_REGDW_IOWATCHDOG_W   L"IoWatchDog"
#define GRTBL_REGDW_IONOTRESET_W   L"IoNotReset"
#define GRTBL_REGDATA_FLOORS_W     L"FloorsData"

#define GRTBL_NM_TXRX          0x00800000 //Уведомления о передаче, приёме
#define GRTBL_NC_RX            1
#define GRTBL_NC_TX            2

#define GRTBL_NM_KADRDATA      0x00400000  //Уведомления о перечислении  кадров 
#define GRTBL_NC_KADRDATA      1
#define GRTBL_NC_KADRDATA_END  2




#pragma pack (push,1)

typedef struct _GRTBL_SETTINGS
{
 DWORD   dw_size;
 DWORD   modem_number;          // Номер модема
 wchar_t io_module[MAX_PATH<<1];// IoModule
 wchar_t io_device[MAX_PATH];   // Устройство
 DWORD   io_speed;              // Скорость обмена
 BOOL    io_watchdog;           //Отправлять пустые кадры во время простоя
 BOOL    io_not_reset;          //Не сбрасывать устройство при отсутствии входящих кадров
 wchar_t db_file  [MAX_PATH<<1];// Имя файла хранилище конфигурации этажей вывода
}GRTBL_SETTINGS,*LPGRTBL_SETTINGS;

#define  OUTPUT_MODE_READY    0
#define  OUTPUT_MODE_DISCRETE 1
#define  OUTPUT_MODE_ANALOG   2

typedef struct _OUTPUT_KADR_PARAM
{
 DWORD   dw_size;
 DWORD   floor_number;
 DWORD   output_position;
 DWORD   output_mode;
 DWORD   output_kadr_id;
 DWORD   output_entry_count;
 DWORD   kadr_name_size;
 DWORD   kadr_name_len ;
 wchar_t kadr_name [ANYSIZE_ARRAY];
}OUTPUT_KADR_PARAM,*LPOUTPUT_KADR_PARAM;

#define GRTBL_CMD_ENUM_OUTPUT_KADR      MCMD(MT_MODEM,5000)
/* Перечисление  кадров
 p1 = OUTPUT_KADR_PARAM, p2 = index
*/

#define GRTBL_CMD_GET_OUTPUT_KADR_PARAM  MCMD(MT_MODEM,5001)
/* Перечисление  кадров вывода
 p1 = LPOUTPUT_KADR_PARAM
*/

#define GRTBL_CMD_CREATE_FLOOR           MCMD(MT_MODEM,5100)
/*
 создание этажа
 p1 = номер этажа
*/

#define GRTBL_CMD_SET_FLOOR_NUMBER       MCMD(MT_MODEM,5101)
/*Смена номера этажа
 p1 = old_number; p2 = new _number
*/

#define GRTBL_CMD_DELETE_FLOOR           MCMD(MT_MODEM,5102)
/*
 Удаление этажа влспроизведения
 p1 = номер этажа
*/

#define GRTBL_CMD_CREATE_FLOOR_OUTPUT          MCMD(MT_MODEM,5200)

#define GRTBL_CMD_SET_FLOOR_OUTPUT_PARAM       MCMD(MT_MODEM,5201)
/*Установка параметров вывода */

#define GRTBL_CMD_DELETE_FLOOR_OUTPUT          MCMD(MT_MODEM,5202)


#define GRTBL_CMD_ENUM_DBKADRS  MCMD(MT_MODEM,6000)
//Запрос на перечисление кадров БД
//p1 = GKHANDLE;




#pragma pack(pop)

#endif

 