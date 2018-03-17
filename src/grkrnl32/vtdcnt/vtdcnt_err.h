/*

 Error codes common for VTD counters modem

*/
#ifndef _VTDMODEM_MSG_INC
#define _VTDMODEM_MSG_INC
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
#define VTDD_FCL                         0x142


//
// Define the severity codes
//


//
// MessageId: VTDERR_DEV_NOT_EXIST
//
// MessageText:
//
//  Устройство с таким номером не существует
//
#define VTDERR_DEV_NOT_EXIST             0xC1420001L

//
// MessageId: VTDERR_DEV_ALREADY_EXIST
//
// MessageText:
//
//  Устройство с таким номером уже существует
//
#define VTDERR_DEV_ALREADY_EXIST         0xC1420002L

//
// MessageId: VTDERR_INVALID_DEVNUMBER
//
// MessageText:
//
//  Слишком большой номер устройства
//
#define VTDERR_INVALID_DEVNUMBER         0xC1420003L

//
// MessageId: VTDERR_INVALID_DEVTYPE
//
// MessageText:
//
//  Несуществующий тип  устройства
//
#define VTDERR_INVALID_DEVTYPE           0xC1420004L

//
// MessageId: VTDERR_INVALID_CONFIGURATION
//
// MessageText:
//
//  Ошибка конфигурации модуля ВТД счётчиков
//
#define VTDERR_INVALID_CONFIGURATION     0xC1420005L

#endif
