/*
 Error codes common for all units
 And probably GRKERNEL32



*/
#ifndef __GKTLMDB_MSG_INC
#define __GKTLMDB_MSG_INC


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
#define FACILITY_GKTLMDB                 0x130
#define FACILITY_GKCOMMON                0x100


//
// Define the severity codes
//


//
// MessageId: TLMDB_ERR_NOOWNER_SET
//
// MessageText:
//
//  Нет группы владельца модуля
//
#define TLMDB_ERR_NOOWNER_SET            0xC1300001L

//
// MessageId: TLMDB_ERR_INCORRECT_MODULE_ADDR
//
// MessageText:
//
//  Ошибка адреса модуля
//
#define TLMDB_ERR_INCORRECT_MODULE_ADDR  0xC1300002L

//
// MessageId: TLMDB_ERR_MODULE_EXIST
//
// MessageText:
//
//  Модуль с таким телемеханическим адресом уже существует
//
#define TLMDB_ERR_MODULE_EXIST           0xC1300003L

//
// MessageId: TLMDB_ERR_MODULE_EMPTY
//
// MessageText:
//
//  Модуль не создан (tlm_module::create)
//
#define TLMDB_ERR_MODULE_EMPTY           0xC1300004L

//
// MessageId: TLMDB_ERR_MODULE_NOTFOUND
//
// MessageText:
//
//  Модуль не найден
//
#define TLMDB_ERR_MODULE_NOTFOUND        0xC1300005L

//
// MessageId: TLMDB_ERR_MODULE_NUMBER
//
// MessageText:
//
//  Недопустимый номер модуля
//
#define TLMDB_ERR_MODULE_NUMBER          0xC1300006L

//
// MessageId: TLMDB_ERR_NO_MORE_ITEMS
//
// MessageText:
//
//  Нет больше элементов в БД
//
#define TLMDB_ERR_NO_MORE_ITEMS          0xC1300007L


#endif

