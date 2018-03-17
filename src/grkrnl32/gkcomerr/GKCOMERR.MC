;/*
; Error codes common for all units
; And probably GRKERNEL32
;
;
;
;*/
;#ifndef __GKCOMM_MSG_INC
;#define __GKCOMM_MSG_INC
;
;


FacilityNames = (
                 GKCommon  = 0x0100:FACILITY_GKCOMMON
                )

LanguageNames = (
                 Eng     = 0x409:GKERR_EN
                 Russian = 0x419:GKERR_RU
                )




MessageId    = 1
Severity     = Error
Facility     = GKCommon
SymbolicName = MERR_INVALID_BUFFER_SIZE
Language=Russian
Неправильно задан размер буфера
.
Language=Eng
Buffer size to small
.

MessageId    = 2
Severity     = Error
Facility     = GKCommon
SymbolicName = MERR_DLLNOT_MODULE
Language=Russian
Динамическая библиотека не является модулем
.
Language=Eng
DLL is not module
.

MessageId    = 3
Severity     = Error
Facility     = GKCommon
SymbolicName = MERR_CREATE_GKHANDLE
Language=Russian
Ошибка содания хендла 
.
Language=Eng
Error creating handle
.


MessageId    = 4
Severity     = Error
Facility     = GKCommon
SymbolicName = MERR_UNSUPPORTED_MODULE_TYPE
Language=Russian
Неподдерживаемый тип модуля
.
Language=Eng
Unsupported module type
.

MessageId    = 5
Severity     = Error
Facility     = GKCommon
SymbolicName = MERR_MODULE_NOTRUNNING
Language=Russian
Модуль в состоянии стоп или пауза
.
Language=Eng
Module not running or paused
.

MessageId    = 6
Severity     = Error
Facility     = GKCommon
SymbolicName = MERR_OWNER_NOTRUNNING
Language=Russian
Модуль владелец в состоянии стоп или пауза
.
Language=Eng
Owner module not running or paused
.


MessageId    = 7
Severity     = Error
Facility     = GKCommon
SymbolicName = MERR_WRITE_IN_REGISTRY
Language=Russian
Ошибка при записи в системный реестр
.
Language=Eng
System registry write error
.

MessageId    = 8
Severity     = Error
Facility     = GKCommon
SymbolicName = MERR_REGISTRY_MODULE_IMAGE;
Language=Russian
Ошибка чтения имени образа модуля из реестра
.
Language=Eng
Error read module image name from registry
.


MessageId    = 9
Severity     = Error
Facility     = GKCommon
SymbolicName = MERR_MODULE_INITIALIZATION
Language=Russian
Ошибка инициализации модуля
.
Language=Eng
Error module initialization
.

MessageId    = 10
Severity     = Error
Facility     = GKCommon
SymbolicName = MERR_GKHANDLE_EXISTS
Language=Russian
Хендл с таким именем уже существует
.
Language=Eng
Handle already exists
.

MessageId    = 11
Severity     = Error
Facility     = GKCommon
SymbolicName = MERR_INVALID_MODULE_CONFIG
Language=Russian
Ошибочная конфигурация модуля
.
Language=Eng
Invalid module configuration
.

MessageId    = 12
Severity     = Error
Facility     = GKCommon
SymbolicName = MERR_EVAL_PERIOD_EXPIRED
Language=Russian
Период пробного использования истек;
.
Language=Eng
Evaluation period is expired
.

MessageId    = 13
Severity     = Error
Facility     = GKCommon
SymbolicName = MERR_UNREGISTERED_MODULE_COPY
Language=Russian
Незарегестрированная копия модуля;
.
Language=Eng
Unregistered module copy
.


MessageId    = 100
Severity     = Error
Facility     = GKCommon
SymbolicName = MERR_NOMORE_DATA
Language=Russian
Данных больше нет;
.
Language=Eng
No more data
.

MessageId    = 1
Severity     = Warning
Facility     = GKCommon
SymbolicName = MWRN_UNKNOWN_MODULE_COMMAND
Language=Russian
Неизвестная или нереализованная команда модуля
.
Language=Eng
Unknown or unimplemented module command 
.

;
;#endif
;

