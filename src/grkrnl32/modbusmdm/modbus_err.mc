;/*
;
; Error codes common for VTD counters modem
;
;*/
;#ifndef _MODBUS_MODEM_MSG_INC
;#define _MODBUS_MODEM_MSG_INC
;#include <gkcomerr.h>
;
;


FacilityNames = (
                 MB_FCL = 0x0142:VTDD_FCL
                )

LanguageNames = (
                 Eng     = 0x409:MODBUESERR_EN
                 Russian = 0x419:MODBUESERR_RU
                )



MessageId    = 1
Severity     = Error
Facility     = MB_FCL
SymbolicName = MBERR_DEV_NOT_EXIST
Language=Russian
Устройство с таким номером не существует
.
Language=Eng
Device with number is not exists.
.

MessageId    = 2
Severity     = Error
Facility     = MB_FCL
SymbolicName = MBERR_DEV_ALREADY_EXIST
Language=Russian
Устройство с таким номером уже существует
.
Language=Eng
Device with number is already exists.
.

MessageId    = 3
Severity     = Error
Facility     = MB_FCL
SymbolicName = MBERR_INVALID_DEVNUMBER
Language=Russian
Некорректный  номер устройства (слишком большой или 0)
.
Language=Eng
Invalid Device number (too big or zero)
.

MessageId    = 4
Severity     = Error
Facility     = MB_FCL
SymbolicName = MBERR_GROUP_ALREADY_EXIST
Language=Russian
Группа с таким номером уже существует
.
Language=Eng
Group  is already exist
.

MessageId    = 5
Severity     = Error
Facility     = MB_FCL
SymbolicName = MBERR_GROUP_NOT_EXIST
Language=Russian
Группа с таким номером не существует
.
Language=Eng
Group  is not exist
.



;#endif


