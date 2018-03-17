#ifndef _DFA_INC_H
#define _DFA_INC_H

#ifndef GKMODULES_INC_H
#include <gkmodules.h>
#endif

#define DFA_HANDLE_NAME L"DFA"

//Команды ДФА

//Регистрация обработчика
//p1 = LPDFA_HANDLER_DATA
//p2 = BOOL true - register, false - unregister
#define DFACMD_REGISTERHANDLER MCMD(MT_DFA,1)


typedef struct _DFA_HANDLER_DATA
{
 DWORD     dw_size;//Размер структуры для контроля
 DWORD     fa;     //Номер фа
 GKHANDLE  module; //Модуль обработчик
 DWORD     module_command;//Команда отправляемая модулю //для обработки
 wchar_t  *handler_name;
 int       handler_phase;//Порядок ( Приоритет ) обработки
}DFA_HANDLER_DATA,*LPDFA_HANDLER_DATA;

#define DFACMD_HANDLE MCMD    (MT_DFA,2)

#define DFACMD_HANDLE_AND_WAIT(MT_DFA,3)


typedef struct _DFA_HANDLE_DATA
{
 GKHANDLE      handler_module;//Обработчик
 HANDLE        h_done_event;  //Событие завершения обработки
 LRESULT       ret_code;      //Код возврата обработки
 DWORD         data_size;     //размер данных для обработки
 BYTE          data[1];       //Начало данных
}DFA_HANDLE_DATA,*LPDFA_HANDLE_DATA;


#endif
 