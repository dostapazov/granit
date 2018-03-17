;/*
;
; Error codes common for GRANIT modem
;
;*/
;#ifndef _GRMODEM_MSG_INC
;#define _GRMODEM_MSG_INC
;#include <gkcomerr.h>
;
;


FacilityNames = (
                 GRMD_FCL = 0x0142:GRMD_FCL
                )

LanguageNames = (
                 Eng     = 0x409:GKERR_EN
                 Russian = 0x419:GKERR_RU
                )



MessageId    = 1
Severity     = Error
Facility     = GRMD_FCL
SymbolicName = GRSCERR_OBJECT_NOT_EXIST
Language=Russian
Объект не существует
.
Language=Eng
Object dos not exists
.

MessageId    = 2
Severity     = Error
Facility     = GRMD_FCL
SymbolicName = GRSCERR_OBJECT_ALREADY_EXIST
Language=Russian
Объект уже сужествует
.
Language=Eng
Object already exists
.

MessageId    = 3
Severity     = Error
Facility     = GRMD_FCL
SymbolicName = GRSCERR_INVALID_ADDRESS
Language=Russian
Неправильный адрес
.
Language=Eng
Invalid address
.


MessageId    = 4
Severity     = Error
Facility     = GRMD_FCL
SymbolicName = GRSCERR_FLOOR_NUMBER
Language=Russian
Недопустимый номер этажа
.
Language=Eng
Invalid floor number
.


MessageId    = 5
Severity     = Error
Facility     = GRMD_FCL
SymbolicName = GRSCERR_FLOOR_COUNT_EXCEED
Language=Russian
Количество этажей достигло максимального значения
.
Language=Eng
Floors capacity exceed
.

MessageId    = 20
Severity     = Error
Facility     = GRMD_FCL
SymbolicName = GRSCERR_TUTR_DESCRIPT_NOT_EXIST
Language=Russian
Отсутствует описатель  объекта ТУ/ТР
.
Language=Eng
Remote Control/Adjusting Description is absent for object
.


MessageId    = 21
Severity     = Error
Facility     = GRMD_FCL
SymbolicName = GRSCERR_TRCMDSET_NOT_SUPPORTED
Language=Russian
Команда установки аналового параметра не поддерживается
.
Language=Eng
Remote Set analog parameters is not supported
.

MessageId    = 22
Severity     = Error
Facility     = GRMD_FCL
SymbolicName = GRSCERR_TUTROBJECT_NOT_SELECTED
Language=Russian
Объект ТУ/ТР не выбран
.
Language=Eng
RC/RA object is not selected 
.

MessageId    = 23
Severity     = Error
Facility     = GRMD_FCL
SymbolicName = GRSCERR_TUTROBJECT_LOCKED
Language=Russian
Объект ТУ/ТР  зянят
.
Language=Eng
RC/RA object is locked
.

MessageId    = 24
Severity     = Error
Facility     = GRMD_FCL
SymbolicName = GRSCERR_TUTRCOMMAND_NOT_ACTIVE
Language=Russian
Команда ТУ/ТР не отправлена
.
Language=Eng
RC/RA command is not sended
.

MessageId    = 25
Severity     = Error
Facility     = GRMD_FCL
SymbolicName = GRSCERR_TUTRCOMMAND_ACTIVE
Language=Russian
Команда ТУ/ТР отправлена
.
Language=Eng
RC/RA command is sended
.

MessageId    = 26
Severity     = Warning
Facility     = GRMD_FCL
SymbolicName = GRSCWRN_TUTR_AUTORESET
Language=Russian
Автоотмена выбора ТУ/ТР по истечению таймера
.
Language=Eng
Automatic reset RC/RA selecting by timer is expired
.


MessageId    = 27
Severity     = Error
Facility     = GRMD_FCL
SymbolicName = GRSCERR_TEST_TUTR_DISABLED
Language=Russian
Тестовые ТУ/ТР запрещены на рабочем оборудовании
.
Language=Eng
RC/RA commands are disabled for testing purpose
.


MessageId    = 28
Severity     = Error
Facility     = GRMD_FCL
SymbolicName = GRSCERR_INVALID_TUMODULE_NUMBER
Language=Russian
Неправильный номер модуля ТУ/ТР
.
Language=Eng
The number of module RC/RA is invalid
.

MessageId    = 29
Severity     = Warning
Facility     = GRMD_FCL
SymbolicName = GRSCWRN_TUCOMMAND_IS_QUEUED
Language=Russian
Команда поставлена  в очередь на выполнение
.
Language=Eng
The command is queued to execute
.

MessageId    = 30
Severity     = Informational
Facility     = GRMD_FCL
SymbolicName = GRSCINFO_TUTR_SENDED
Language=Russian
Команда отправлена на КП
.
Language=Eng
The command is sended to Control Point
.

MessageId    = 31
Severity     = Error
Facility     = GRMD_FCL
SymbolicName = GRSCERR_TUON
Language=Russian
Объект не включился по ТУ
.
Language=Eng
The object is not come to ON state by Remote operation
.

MessageId    = 32
Severity     = Error
Facility     = GRMD_FCL
SymbolicName = GRSCERR_TUOFF
Language=Russian
Объект не отключился по ТУ
.
Language=Eng
The object is not come to OFF state by Remote operation
.
MessageId    = 33
Severity     = Error
Facility     = GRMD_FCL
SymbolicName = GRSCERR_TU_EXIST
Language=Russian
Такой объект ТУ/ТР уже создан
.
Language=Eng
The RC/RA object already exists
.


MessageId    = 100
Severity     = Error
Facility     = GRMD_FCL
SymbolicName = GRSCERR_GRPROTO_SIZE
Language=Russian
Ошибочный размер структуры gr_proto
.
Language=Eng
Error size of struct gr_proto
.

MessageId    = 101
Severity     = Error
Facility     = GRMD_FCL
SymbolicName = GRSCERR_BUFFPTR
Language=Russian
Ошибка указателя буфера
.
Language=Eng
Buffer pointer error
.

MessageId    = 102
Severity     = Error
Facility     = GRMD_FCL
SymbolicName = GRSCERR_ZERO_DATA
Language=Russian
Кадр с данными 0
.
Language=Eng
Zero data frame
.

MessageId    = 103
Severity     = Error
Facility     = GRMD_FCL
SymbolicName = GRSCERR_CRC_ERROR
Language=Russian
Ошибка КПК
.
Language=Eng
Error CRC
.


MessageId    = 104
Severity     = Error
Facility     = GRMD_FCL
SymbolicName = GRSCERR_KADRTYPE
Language=Russian
Ошибка типа кадра      gr_ptoto::type.ktype
.
Language=Eng
Kadr type error
.

MessageId    = 105
Severity     = Error
Facility     = GRMD_FCL
SymbolicName = GRSCERR_INFOTYPE
Language=Russian
Ошибка вида информации gr_ptoto::info.inf
.
Language=Eng
Error kadr info type gr_ptoto::info.inf
.

MessageId    = 106
Severity     = Error
Facility     = GRMD_FCL
SymbolicName = GRSCERR_NORMINVERSE
Language=Russian
Несовпадение прямой и инверсной части
.
Language=Eng
Normal and inverse part not equal
.

MessageId    = 108
Severity     = Error
Facility     = GRMD_FCL
SymbolicName = GRSCERR_UNKNOWN
Language=Russian
Неизвестная ошибка
.
Language=Eng
Unknown error
.


;#endif

