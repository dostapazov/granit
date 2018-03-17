/*-----------------------------------------------------------------------------
 11.02.2004 9:49:11 
 Модуль:   otd_dblow
 Автор:    Остапенко Д.В.
 Назначение: Определения для ОТД БД - нижнего уровня
 
-----------------------------------------------------------------------------*/




#ifndef OTD_DB_H
#define OTD_DB_H 1

#include <ke_defs.h>
#include <otd_base.h>
#include <otd_diag.h>

#if OTD_DB_H == 1

/*
 Модуль(cублок) состоит из следующих частей:

  dword       address //№ л/c , № модуля//
  word        parts;   битовый описатель частей
 [word        version]
 [word        diag]      //диагностика
 [FILETIME    timestamp]
 [otd_data    data]      //массив данных
 [otd_data    chmask]    //
 [otd_block   stdparam  //стандартные параметры  модуля
 [otd_block   state]
 [otd_text    name ]
 word kpk           //Контрольная сумма
*/

#define OTD_PART_VERSION     0x0001
#define OTD_PART_EXTCMD      0x0002
#define OTD_PART_DIAG        0x0004 //Диагностик
#define OTD_PART_TIMESTAMP   0x0008
#define OTD_PART_DATA        0x0010 //Данные otd_data
#define OTD_PART_CHMASK      0x0020 //Маска изменений
#define OTD_PART_STDPARAM    0x0040 //стандартные параметры
#define OTD_PART_STATE       0x0080 //Состояние
#define OTD_PART_NAME        0x0100 //Имя otd_text;
#define OTD_PART_EXTCMD_DATA 0x0200 //Данные расширения команды


#pragma pack(push,1)

typedef struct _otd_stdparam
{
 DWORD   hardware_type;     //Тип оборудования
 DWORD   options;           //
 DWORD   ctime_work;        //Таймер вызова работосрособного
 DWORD   ctime_nowork;      //Таймер вызова неработоспособного
}otd_stdparam,*lpotd_stdparam;

typedef struct _otd_state
{
 DWORD state_mask;
 WORD  query_count;//Отправлено вызовов
 WORD  nc_count;   //Пришло данных без изменения
}otd_state,*lpotd_state;

//структура разобранного протокола
typedef struct _otd_proto_parsed
{
 lpotd_addr    addr;
 LPWORD        parts;
 LPWORD        version;
 LPWORD        ext_cmd;
 LPWORD        diag;
 LPFILETIME    time_stamp;
 lpotd_data    data;
 lpotd_data    chmask;
 lpotd_block   param;
 lpotd_block   state;
 lpotd_text    name;
 LPWORD        kpk;
};


#pragma pack(pop)

#endif
#endif

 