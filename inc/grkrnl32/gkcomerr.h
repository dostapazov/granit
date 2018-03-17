/*
 Error codes common for all units
 And probably GRKERNEL32



*/
#ifndef __GKCOMM_MSG_INC
#define __GKCOMM_MSG_INC


//
//  Values are 32 bit values layed out as follows:
//
//   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
//   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
//  +---+-+-+-----------------------+-------------------------------+
//  |Sev|C|R|     Facility          |               Code            |
//  +---+-+-+-----------------------+-------------------------------+
//
//  where
//
//      Sev - is the severity code
//
//          00 - Success
//          01 - Informational
//          10 - Warning
//          11 - Error
//
//      C - is the Customer code flag
//
//      R - is a reserved bit
//
//      Facility - is the facility code
//
//      Code - is the facility's status code
//
//
// Define the facility codes
//
#define FACILITY_GKCOMMON                0x100


//
// Define the severity codes
//


//
// MessageId: MERR_INVALID_BUFFER_SIZE
//
// MessageText:
//
//  Неправильно задан размер буфера
//
#define MERR_INVALID_BUFFER_SIZE         0xC1000001L

//
// MessageId: MERR_DLLNOT_MODULE
//
// MessageText:
//
//  Динамическая библиотека не является модулем
//
#define MERR_DLLNOT_MODULE               0xC1000002L

//
// MessageId: MERR_CREATE_GKHANDLE
//
// MessageText:
//
//  Ошибка содания хендла 
//
#define MERR_CREATE_GKHANDLE             0xC1000003L

//
// MessageId: MERR_UNSUPPORTED_MODULE_TYPE
//
// MessageText:
//
//  Неподдерживаемый тип модуля
//
#define MERR_UNSUPPORTED_MODULE_TYPE     0xC1000004L

//
// MessageId: MERR_MODULE_NOTRUNNING
//
// MessageText:
//
//  Модуль в состоянии стоп или пауза
//
#define MERR_MODULE_NOTRUNNING           0xC1000005L

//
// MessageId: MERR_OWNER_NOTRUNNING
//
// MessageText:
//
//  Модуль владелец в состоянии стоп или пауза
//
#define MERR_OWNER_NOTRUNNING            0xC1000006L

//
// MessageId: MERR_WRITE_IN_REGISTRY
//
// MessageText:
//
//  Ошибка при записи в системный реестр
//
#define MERR_WRITE_IN_REGISTRY           0xC1000007L

//
// MessageId: MERR_REGISTRY_MODULE_IMAGE
//
// MessageText:
//
//  Ошибка чтения имени образа модуля из реестра
//
#define MERR_REGISTRY_MODULE_IMAGE       0xC1000008L    

//
// MessageId: MERR_MODULE_INITIALIZATION
//
// MessageText:
//
//  Ошибка инициализации модуля
//
#define MERR_MODULE_INITIALIZATION       0xC1000009L

//
// MessageId: MERR_GKHANDLE_EXISTS
//
// MessageText:
//
//  Хендл с таким именем уже существует
//
#define MERR_GKHANDLE_EXISTS             0xC100000AL

//
// MessageId: MERR_INVALID_MODULE_CONFIG
//
// MessageText:
//
//  Ошибочная конфигурация модуля
//
#define MERR_INVALID_MODULE_CONFIG       0xC100000BL

//
// MessageId: MERR_EVAL_PERIOD_EXPIRED
//
// MessageText:
//
//  Период пробного использования истек;
//
#define MERR_EVAL_PERIOD_EXPIRED         0xC100000CL

//
// MessageId: MERR_UNREGISTERED_MODULE_COPY
//
// MessageText:
//
//  Незарегестрированная копия модуля;
//
#define MERR_UNREGISTERED_MODULE_COPY    0xC100000DL

//
// MessageId: MERR_NOMORE_DATA
//
// MessageText:
//
//  Данных больше нет;
//
#define MERR_NOMORE_DATA                 0xC1000064L

//
// MessageId: MWRN_UNKNOWN_MODULE_COMMAND
//
// MessageText:
//
//  Неизвестная или нереализованная команда модуля
//
#define MWRN_UNKNOWN_MODULE_COMMAND      0x81000001L


#endif

