/*

 Error codes common for GRANIT modem

*/
#ifndef _GRREC_HIST_ERR_INC
#define _GRREC_HIST_ERR_INC
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
#define REC_HIST_FCL                     0x143


//
// Define the severity codes
//


//
// MessageId: REC_HIST_ERROR_LOAD_CLIENTLIB
//
// MessageText:
//
//  Ошибка загрузки клиентской библиотеки
//
#define REC_HIST_ERROR_LOAD_CLIENTLIB    0xC1430001L

//
// MessageId: REC_HIST_ERROR_CONNECT_DATABASE
//
// MessageText:
//
//  Ошибка подключения к базе данных;
//
#define REC_HIST_ERROR_CONNECT_DATABASE  0xC1430002L

//
// MessageId: REC_HIST_ERROR_NO_TABLES
//
// MessageText:
//
//  База данных не проинициализирована.(Отсутствуют необходимые таблицы))
//
#define REC_HIST_ERROR_NO_TABLES         0xC1430003L

//
// MessageId: REC_HIST_ERROR_SQL_QUERY
//
// MessageText:
//
//  Ошибка SQL-запроса
//
#define REC_HIST_ERROR_SQL_QUERY         0xC1430004L

//
// MessageId: REC_HIST_ERROR_START_TRANSACTION
//
// MessageText:
//
//  Ошибка старта транзакции
//
#define REC_HIST_ERROR_START_TRANSACTION 0xC1430005L

//
// MessageId: REC_HIST_ERROR_PREPARE_QUERY
//
// MessageText:
//
//  Ошибка в тексте запроса
//
#define REC_HIST_ERROR_PREPARE_QUERY     0xC1430006L

#endif
