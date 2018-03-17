/*******************************************************************************
 *
 * Модуль    : gkernel
 * Описание  : 
 * Автор     : Остапенко Д.В.
 * Версия    : 1
 * Создан    : 17.07.2002
 *
 ******************************************************************************/


#ifndef  _GRKERNEL_INC_
#define  _GRKERNEL_INC_

#ifndef GKMODULES_INC_H
 #include <gkmodules.h>
#endif
#include <gkreporter.h>


#define IDS_EVAL_PERIOD_EXPIRED             IDS_USER_STR+1

#ifndef RC_INVOKED
#include <gkernel_mod_err.h>

#pragma pack (push,4)

#define  GRKERNEL_STATE_ERROR       0x80000000
#define  GRKERNEL_STATE_WARN        0x40000000
#define  GRKERNEL_STATE_MASK        0xC0000000
#define  ISGRKERNEL_STATE_OK(x)    (((x&GRKERNEL_STATE_MASK))==0)
#define  ISGRKERNEL_STATE_WARN(x)  (((x&GRKERNEL_STATE_MASK))==GRKERNEL_STATE_WARN)
#define  ISGRKERNEL_STATE_ERROR(x) (((x&GRKERNEL_STATE_MASK))&GRKERNEL_STATE_ERROR)

#define  CMGK_SET_KERNEL_STATE     MCMD(MT_KERNEL,99)

//Установка сброс состояния ядра
// p1 = LPKERNEL_STATE  ,p2
typedef struct _KERNEL_STATE
{
 DWORD     dw_size;
 GKHANDLE  module;
 DWORD     module_error;
 BOOL      set;
}KERNEL_STATE,*LPKERNEL_STATE;


//Выделение памяти
#define CMGK_ALLOC_MEM MCMD(MT_KERNEL,100)
//p1 = DWORD sz , ret = LPVOID mem_block

// перераспеределение памяти
#define CMGK_REALLOC_MEM MCMD(MT_KERNEL,101)
//p1 = LPVOID ptr, p2 = DWORD new_sz, ret = LPVOID mem_block

//ОСвобождение памяти
#define CMGK_FREE_MEM MCMD(MT_KERNEL,102)
//p1 = LPVOID mem_block , ret = BOOL res

//Таймер ядра
typedef struct _GKTIMER
{
 DWORD        dw_size;
 GKHANDLE     tm_target;
 DWORD        tm_cmd;        //Команда   передаваемая хендлу
 LPARAM       tm_p1;         //Параметр  передаваемый хендлу
 __int64      alert_time;    //Время срабатывания
 __int64      alert_counter; //Счётчик
 BOOL         period;        //Периодичность
}GKTIMER,*LPGKTIMER;

/*Макрос определяющиё скока */
#define GKTIMER_REMAIN_TIME(x) NS100_MSEC(((x)->period) ? (-(x)->alert_counter) :((x)->alert_time - (x)->alert_counter ) )

#define CMGK_SET_TIMER   MCMD(MT_KERNEL,103)
/*
Установка таймера
 p1 = LPGKTIMER,p2 = 0
 Возврат TIMER_ID
*/

#define CMGK_RESET_TIMER MCMD(MT_KERNEL,104)
/*
 Сброс таймера
 p1 = TIMER_ID
 p2 = __int64 * new_tm_value; если NULL то сброс таймера
 Если таймер не периодичный то NULL равносильно отмене
*/

#define CMGK_CANCEL_TIMER MCMD(MT_KERNEL,105)
/*
 Отмена таймера
 p1 = TIMER_ID
*/

#define CMGK_GET_TIMER    MCMD(MT_KERNEL,106)
/*Команда получения содержимого таймера
p1 = TIMER_ID
p2 = LPGKTIMER
*/

#define CMGK_ONTIMER      MCMD(MT_KERNEL,111)
/*Команда для таймера если его собственная команда == 0*/
/*Возврат GKH_RET_ERROR равносилен отмене*/

//Процедура таймера наращивание времени работы пробных модулей
#define TIMER_ID_TRIAL  1

#define CMGK_CREATE_STDREPORTER    MCMD(MT_KERNEL,112)
/*Команда получения содержимого таймера
p1 = GKHANDLE - кто пытается создать (внешний репортёр)
*/

#define CMGK_DELETE_SDTREPORTER    MCMD(MT_KERNEL,113)
/*Команда получения содержимого таймера
p1 = GKHANDLE - кто пытается удалить (внешний репортёр)
*/

#define CMGK_SET_INSTANCE         MCMD(MT_KERNEL,114)
#define CMGK_GET_INSTANCE         MCMD(MT_KERNEL,115)
//Установка /получение имении Instance
//p1 = wchar_t * inst,p2 len/buffer size

#pragma pack (pop)
#endif // RC_INVOKED

#endif

 
