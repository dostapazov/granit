/*-----------------------------------------------------------------------------
 11.02.2004 16:34:17 
 Модуль:   otd_hardwtype
 Автор:    Остапенко Д.В.
 Назначение: Регистрируемые типы оборудования
 
-----------------------------------------------------------------------------*/



#ifndef OTD_HARDW_TYPE_H
#define OTD_HARDW_TYPE_H 1

#include <otd_base.h>

#pragma pack(push,1)

/*Предопределённые типы оборудования*/
#define OTD_HARDW_GRANIT   1  /*Гранит*/
#define OTD_HARDW_TM512    2  /*ТМ512*/
#define OTD_HARDW_VRTF3    3  /*ВРТФ3*/



typedef LRESULT ( WINAPI * hardw_spec_callback)(LPVOID data,lpotd_block spec_block,LPARAM func);

typedef struct _otd_hardware_type
{
 WORD    hardware_type;//Тип оборудования
 wchar_t description[];//Описание оборудования
 WORD    def_work_query_time;
 WORD    def_nowork_query_time;


}otd_hardware_type,*lpotd_hardware_type;

#pragma pack(pop)

#endif
 