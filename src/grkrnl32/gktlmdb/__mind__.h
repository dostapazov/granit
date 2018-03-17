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
 dword       parts;   битовый описатель частей
 [word        diag]      //диагностика
 [otd_data    data]      //массив данных
 [otd_data    chmask]    //
 [otd_block]  std_param  //стандартные параметры  модуля
*/

#define OTD_PART_DIAG      0x0001
#define OTD_PART_DATA      0x0002
#define OTD_PART_CHMASK    0x0004
#define OTD_PART_STDPARAM  0x0008
#define OTD_PART_STATE     0x0010


#pragma pack(push,1)

typedef struct _otd_stdparam
{
 DWORD   hardware_type;     //Тип оборудования
 DWORD   options;           //
 DWORD   ctime_work;        //Таймер вызова работосрособного
 DWORD   ctime_nowork;      //Таймер вызова неработоспособного
}otd_stdparam,*lpotd_stdparam;


typedef struct _otd_text /*Очень напоминает RtlUnicodeString*/
{
 WORD size;
 WORD len;
 wchar_t text[ANYSIZE_ARRAY];
}otd_text,* lpotd_text;

#pragma pack(pop)

#endif
#endif

 