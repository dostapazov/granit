;/*
; Error codes common for all units
; And probably GRKERNEL32
;
;
;
;*/
;#ifndef __GKERNEL_MSG_INC
;#define __GKERNEL_MSG_INC
;
;


FacilityNames = (
                 GKCommon  = 0x0100:FACILITY_GKCOMMON
                 GKERNEL  =  0x0330:FACILITY_GKERNEL
                )

LanguageNames = (
                 Eng     = 0x409:GKERNEL_EN
                 Russian = 0x419:GKERNEL_RU
                )



MessageId    = 1
Severity     = Error
Facility     = GKERNEL
SymbolicName = GKERNEL_ERR_EVAL_PERIOD_EXPIRED
Language=Russian
Период пробного использования истек
.
Language=Eng
Evaluation period is expired
.

MessageId    = 2
Severity     = Error
Facility     = GKERNEL
SymbolicName = GKERNEL_INVALID_COPY

Language=Russian
Незарегестрированая копия программы
.
Language=Eng
Unregistered copy
.

;
;#endif
;
