;/*
;
; Error codes common for GRANIT modem
;
;*/
;#ifndef _IEC60870_MODEM_MSG_INC
;#define _IEC60870_MODEM_MSG_INC
;#include <gkcomerr.h>
;
;


FacilityNames = (
                 IEC60870MDM_FCL = 0x0192:IEC60870MDM_FCL
                )

LanguageNames = (
                 Eng     = 0x409:IEC60870_MDM_ERR_EN
                 Russian = 0x419:IEC60870_MDM_ERR_RU
                )



MessageId    = 1
Severity     = Error
Facility     = IEC60870MDM_FCL
SymbolicName = GRSCERR_OBJECT_NOT_EXIST
Language=Russian
Объект не существует
.
Language=Eng
Object dos not exists
.


MessageId    = 108
Severity     = Error
Facility     = IEC60870MDM_FCL
SymbolicName = GRSCERR_UNKNOWN
Language=Russian
Неизвестная ошибка
.
Language=Eng
Unknown error
.

;#endif

