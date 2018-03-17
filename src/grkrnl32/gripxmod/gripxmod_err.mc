;/*
; Error codes common for modemmer and modems
;
;
;*/
;#ifndef _GRIPXMOD_MSG_INC
;#define _GRIPXMOD_
;#include <gkcomerr.h>
;
;


FacilityNames = (
                 GRNET_FACILITY = 0x0141:GRNET_FACILITY
                )

LanguageNames = (
                 Eng     = 0x409:GKERR_EN
                 Russian = 0x419:GKERR_RU
                )



MessageId    = 1
Severity     = Error
Facility     = GRNET_FACILITY
SymbolicName = GRNET_ERR_IPXPROTO_NOT_INSTALLED
Language=Russian
Протокол IPX не установлен
.
Language=Eng
Protocol IPX is not installed
.

MessageId    = 2
Severity     = Error
Facility     = GRNET_FACILITY
SymbolicName = GRNET_ERR_SETTINGS
Language=Russian
Не правильно установлены параметры модуля
.
Language=Eng
Invalid module parameters
.


;
;#endif
;
