/*
 Error codes common for modemmer and modems


*/
#ifndef _GRIPXMOD_MSG_INC
#define _GRIPXMOD_
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
#define GRNET_FACILITY                   0x141


//
// Define the severity codes
//


//
// MessageId: GRNET_ERR_IPXPROTO_NOT_INSTALLED
//
// MessageText:
//
//  Протокол IPX не установлен
//
#define GRNET_ERR_IPXPROTO_NOT_INSTALLED 0xC1410001L

//
// MessageId: GRNET_ERR_SETTINGS
//
// MessageText:
//
//  Не правильно установлены параметры модуля
//
#define GRNET_ERR_SETTINGS               0xC1410002L


#endif

