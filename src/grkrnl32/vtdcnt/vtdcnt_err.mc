;/*
;
; Error codes common for VTD counters modem
;
;*/
;#ifndef _VTDMODEM_MSG_INC
;#define _VTDMODEM_MSG_INC
;#include <gkcomerr.h>
;
;


FacilityNames = (
                 VTD_FCL = 0x0142:VTDD_FCL
                )

LanguageNames = (
                 Eng     = 0x409:VTDERR_EN
                 Russian = 0x419:VTDERR_RU
                )



MessageId    = 1
Severity     = Error
Facility     = VTD_FCL
SymbolicName = VTDERR_DEV_NOT_EXIST
Language=Russian
Устройство с таким номером не существует
.
Language=Eng
Device with number is not exists.
.

MessageId    = 2
Severity     = Error
Facility     = VTD_FCL
SymbolicName = VTDERR_DEV_ALREADY_EXIST
Language=Russian
Устройство с таким номером уже существует
.
Language=Eng
Device with number is already exists.
.

MessageId    = 3
Severity     = Error
Facility     = VTD_FCL
SymbolicName = VTDERR_INVALID_DEVNUMBER
Language=Russian
Слишком большой номер устройства
.
Language=Eng
Device number too big
.

MessageId    = 4
Severity     = Error
Facility     = VTD_FCL
SymbolicName = VTDERR_INVALID_DEVTYPE
Language=Russian
Несуществующий тип  устройства
.
Language=Eng
Invalid device type
.

MessageId    = 5
Severity     = Error
Facility     = VTD_FCL
SymbolicName = VTDERR_INVALID_CONFIGURATION
Language=Russian
Ошибка конфигурации модуля ВТД счётчиков
.
Language=Eng
Error VTD counters module  configuration
.


;#endif


