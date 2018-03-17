;/*
; Error codes common for modemmer and modems
;
;
;*/
;#ifndef __MODEMMER_MSG_INC
;#define __MODEMMER_MSG_INC
;#include <gkcomerr.h>
;
;


FacilityNames = (
                 GKModemmer = 0x0140:FACILITY_GKMODEMMER
                )

LanguageNames = (
                 Eng     = 0x409:GKERR_EN
                 Russian = 0x419:GKERR_RU
                )



MessageId    = 1
Severity     = Error
Facility     = GKModemmer
SymbolicName = MDM_ERR_INVALID_FA
Language=Russian
Некорректый ФА
.
Language=Eng
Invalid FA
.

MessageId    = 2
Severity     = Error
Facility     = GKModemmer
SymbolicName = MDM_ERR_NO_FA_HANDLER
Language=Russian
Отсутстуют обработчики ФА
.
Language=Eng
No FA Handlers
.

MessageId    = 50
Severity     = Error
Facility     = GKModemmer
SymbolicName = MDM_ERR_NO_MODEM
Language=Russian
Модем с таким номером не существует
.
Language=Eng
modem number absent 
.

MessageId    = 51
Severity     = Error
Facility     = GKModemmer
SymbolicName = MDM_ERR_NO_MODEMLINE
Language=Russian
Линия модема не существует
.
Language=Eng
No modem line
.

MessageId    = 52
Severity     = Error
Facility     = GKModemmer
SymbolicName = MDM_ERR_USED_MODEM_NUMBER
Language=Russian
Такой номер модема уже используется
.
Language=Eng
This modem number always used
.

MessageId    = 53
Severity     = Error
Facility     = GKModemmer
SymbolicName = MDM_ERR_NO_MODEM_NUMBER
Language=Russian
Модему не присвоен номер
.
Language=Eng
Modem whith unassigned number
.

MessageId    = 54
Severity     = Error
Facility     = GKModemmer
SymbolicName = MDM_ERR_INVALID_PU_NUMBER
Language=Russian
Неправильный номер ПУ
.
Language=Eng
Invalid PU number
.

MessageId    = 55
Severity     = Error
Facility     = GKModemmer
SymbolicName = MDM_ERR_INVALID_MODEM_NUMBER
Language=Russian
Номер модема должен быть в диапазоне 1 - 250
.
Language=Eng
Modem number must be between 1-250
.


MessageId    = 100
Severity     = Error
Facility     = GKModemmer
SymbolicName = MDM_ERR_SEND_BUSY
Language=Russian
Модем занят передачей данных
.
Language=Eng
Modem is sending data now
.

MessageId    = 101
Severity     = Error
Facility     = GKModemmer
SymbolicName = MDM_ERR_LINE_NOT_CONNECT;
Language=Russian
Обрыв связи по линии
.
Language=Eng
Modem line not connected
.

MessageId    = 102
Severity     = Error
Facility     = GKModemmer
SymbolicName = MDM_ERR_LINE_RX_READY;
Language=Russian
Нет готовности передачи линии
.
Language=Eng
Line not ready to send
.




;/*Предупреждения*/

MessageId    = 1
Severity     = Warning
Facility     = GKModemmer
SymbolicName = MDM_WRN_HANDLER_EXIST
Language=Russian
Обработчик уже зарегесрирован
.
Language=Eng
Handler already registered
.

;
;#endif
;
