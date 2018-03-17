/*

 Error codes common for GRANIT modem

*/
#ifndef _IEC60870_MODEM_MSG_INC
#define _IEC60870_MODEM_MSG_INC
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
#define IEC60870MDM_FCL                  0x192


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
#define GRSCERR_OBJECT_NOT_EXIST         0xC1920001L

//
// MessageId: GRSCERR_UNKNOWN
//
// MessageText:
//
//  Неизвестная ошибка
//
#define GRSCERR_UNKNOWN                  0xC192006CL

#endif
