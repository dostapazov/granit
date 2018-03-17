/*

 Error codes common for VTD counters modem

*/
#ifndef _MODBUS_MODEM_MSG_INC
#define _MODBUS_MODEM_MSG_INC
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
// MessageId: MBERR_DEV_NOT_EXIST
//
// MessageText:
//
//  Устройство с таким номером не существует
//
#define MBERR_DEV_NOT_EXIST              0xC1420001L

//
// MessageId: MBERR_DEV_ALREADY_EXIST
//
// MessageText:
//
//  Устройство с таким номером уже существует
//
#define MBERR_DEV_ALREADY_EXIST          0xC1420002L

//
// MessageId: MBERR_INVALID_DEVNUMBER
//
// MessageText:
//
//  Некорректный  номер устройства (слишком большой или 0)
//
#define MBERR_INVALID_DEVNUMBER          0xC1420003L

//
// MessageId: MBERR_GROUP_ALREADY_EXIST
//
// MessageText:
//
//  Группа с таким номером уже существует
//
#define MBERR_GROUP_ALREADY_EXIST        0xC1420004L

//
// MessageId: MBERR_GROUP_NOT_EXIST
//
// MessageText:
//
//  Группа с таким номером не существует
//
#define MBERR_GROUP_NOT_EXIST            0xC1420005L

#endif
