/*
 Error codes common for all units
 And probably GRKERNEL32



*/
#ifndef __GKERNEL_MSG_INC
#define __GKERNEL_MSG_INC


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
#define FACILITY_GKERNEL                 0x330
#define FACILITY_GKCOMMON                0x100


//
// Define the severity codes
//


//
// MessageId: GKERNEL_ERR_EVAL_PERIOD_EXPIRED
//
// MessageText:
//
//  Период пробного использования истек
//
#define GKERNEL_ERR_EVAL_PERIOD_EXPIRED  0xC3300001L

//
// MessageId: GKERNEL_INVALID_COPY
//
// MessageText:
//
//  Незарегестрированая копия программы
//
#define GKERNEL_INVALID_COPY             0xC3300002L


#endif

