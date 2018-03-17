/*

 Error codes common for GRANIT modem

*/
#ifndef _GRMODEM_MSG_INC
#define _GRMODEM_MSG_INC
#include <gkcomerr.h>


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
#define GRMD_FCL                         0x142


//
// Define the severity codes
//


//
// MessageId: GRSCERR_OBJECT_NOT_EXIST
//
// MessageText:
//
//  Объект не существует
//
#define GRSCERR_OBJECT_NOT_EXIST         0xC1420001L

//
// MessageId: GRSCERR_OBJECT_ALREADY_EXIST
//
// MessageText:
//
//  Объект уже сужествует
//
#define GRSCERR_OBJECT_ALREADY_EXIST     0xC1420002L

//
// MessageId: GRSCERR_INVALID_ADDRESS
//
// MessageText:
//
//  Неправильный адрес
//
#define GRSCERR_INVALID_ADDRESS          0xC1420003L

//
// MessageId: GRSCERR_FLOOR_NUMBER
//
// MessageText:
//
//  Недопустимый номер этажа
//
#define GRSCERR_FLOOR_NUMBER             0xC1420004L

//
// MessageId: GRSCERR_FLOOR_COUNT_EXCEED
//
// MessageText:
//
//  Количество этажей достигло максимального значения
//
#define GRSCERR_FLOOR_COUNT_EXCEED       0xC1420005L

//
// MessageId: GRSCERR_TUTR_DESCRIPT_NOT_EXIST
//
// MessageText:
//
//  Отсутствует описатель  объекта ТУ/ТР
//
#define GRSCERR_TUTR_DESCRIPT_NOT_EXIST  0xC1420014L

//
// MessageId: GRSCERR_TRCMDSET_NOT_SUPPORTED
//
// MessageText:
//
//  Команда установки аналового параметра не поддерживается
//
#define GRSCERR_TRCMDSET_NOT_SUPPORTED   0xC1420015L

//
// MessageId: GRSCERR_TUTROBJECT_NOT_SELECTED
//
// MessageText:
//
//  Объект ТУ/ТР не выбран
//
#define GRSCERR_TUTROBJECT_NOT_SELECTED  0xC1420016L

//
// MessageId: GRSCERR_TUTROBJECT_LOCKED
//
// MessageText:
//
//  Объект ТУ/ТР  зянят
//
#define GRSCERR_TUTROBJECT_LOCKED        0xC1420017L

//
// MessageId: GRSCERR_TUTRCOMMAND_NOT_ACTIVE
//
// MessageText:
//
//  Команда ТУ/ТР не отправлена
//
#define GRSCERR_TUTRCOMMAND_NOT_ACTIVE   0xC1420018L

//
// MessageId: GRSCERR_TUTRCOMMAND_ACTIVE
//
// MessageText:
//
//  Команда ТУ/ТР отправлена
//
#define GRSCERR_TUTRCOMMAND_ACTIVE       0xC1420019L

//
// MessageId: GRSCWRN_TUTR_AUTORESET
//
// MessageText:
//
//  Автоотмена выбора ТУ/ТР по истечению таймера
//
#define GRSCWRN_TUTR_AUTORESET           0x8142001AL

//
// MessageId: GRSCERR_TEST_TUTR_DISABLED
//
// MessageText:
//
//  Тестовые ТУ/ТР запрещены на рабочем оборудовании
//
#define GRSCERR_TEST_TUTR_DISABLED       0xC142001BL

//
// MessageId: GRSCERR_INVALID_TUMODULE_NUMBER
//
// MessageText:
//
//  Неправильный номер модуля ТУ/ТР
//
#define GRSCERR_INVALID_TUMODULE_NUMBER  0xC142001CL

//
// MessageId: GRSCWRN_TUCOMMAND_IS_QUEUED
//
// MessageText:
//
//  Команда поставлена  в очередь на выполнение
//
#define GRSCWRN_TUCOMMAND_IS_QUEUED      0x8142001DL

//
// MessageId: GRSCINFO_TUTR_SENDED
//
// MessageText:
//
//  Команда отправлена на КП
//
#define GRSCINFO_TUTR_SENDED             0x4142001EL

//
// MessageId: GRSCERR_TUON
//
// MessageText:
//
//  Объект не включился по ТУ
//
#define GRSCERR_TUON                     0xC142001FL

//
// MessageId: GRSCERR_TUOFF
//
// MessageText:
//
//  Объект не отключился по ТУ
//
#define GRSCERR_TUOFF                    0xC1420020L

//
// MessageId: GRSCERR_TU_EXIST
//
// MessageText:
//
//  Такой объект ТУ/ТР уже создан
//
#define GRSCERR_TU_EXIST                 0xC1420021L

//
// MessageId: GRSCERR_GRPROTO_SIZE
//
// MessageText:
//
//  Ошибочный размер структуры gr_proto
//
#define GRSCERR_GRPROTO_SIZE             0xC1420064L

//
// MessageId: GRSCERR_BUFFPTR
//
// MessageText:
//
//  Ошибка указателя буфера
//
#define GRSCERR_BUFFPTR                  0xC1420065L

//
// MessageId: GRSCERR_ZERO_DATA
//
// MessageText:
//
//  Кадр с данными 0
//
#define GRSCERR_ZERO_DATA                0xC1420066L

//
// MessageId: GRSCERR_CRC_ERROR
//
// MessageText:
//
//  Ошибка КПК
//
#define GRSCERR_CRC_ERROR                0xC1420067L

//
// MessageId: GRSCERR_KADRTYPE
//
// MessageText:
//
//  Ошибка типа кадра      gr_ptoto::type.ktype
//
#define GRSCERR_KADRTYPE                 0xC1420068L

//
// MessageId: GRSCERR_INFOTYPE
//
// MessageText:
//
//  Ошибка вида информации gr_ptoto::info.inf
//
#define GRSCERR_INFOTYPE                 0xC1420069L

//
// MessageId: GRSCERR_NORMINVERSE
//
// MessageText:
//
//  Несовпадение прямой и инверсной части
//
#define GRSCERR_NORMINVERSE              0xC142006AL

//
// MessageId: GRSCERR_UNKNOWN
//
// MessageText:
//
//  Неизвестная ошибка
//
#define GRSCERR_UNKNOWN                  0xC142006CL

#endif
