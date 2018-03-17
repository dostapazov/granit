/*
 Error codes common for modemmer and modems


*/
#ifndef __MODEMMER_MSG_INC
#define __MODEMMER_MSG_INC
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
#define FACILITY_GKMODEMMER              0x140


//
// Define the severity codes
//


//
// MessageId: MDM_ERR_INVALID_FA
//
// MessageText:
//
//  ����������� ��
//
#define MDM_ERR_INVALID_FA               0xC1400001L

//
// MessageId: MDM_ERR_NO_FA_HANDLER
//
// MessageText:
//
//  ���������� ����������� ��
//
#define MDM_ERR_NO_FA_HANDLER            0xC1400002L

//
// MessageId: MDM_ERR_NO_MODEM
//
// MessageText:
//
//  ����� � ����� ������� �� ����������
//
#define MDM_ERR_NO_MODEM                 0xC1400032L

//
// MessageId: MDM_ERR_NO_MODEMLINE
//
// MessageText:
//
//  ����� ������ �� ����������
//
#define MDM_ERR_NO_MODEMLINE             0xC1400033L

//
// MessageId: MDM_ERR_USED_MODEM_NUMBER
//
// MessageText:
//
//  ����� ����� ������ ��� ������������
//
#define MDM_ERR_USED_MODEM_NUMBER        0xC1400034L

//
// MessageId: MDM_ERR_NO_MODEM_NUMBER
//
// MessageText:
//
//  ������ �� �������� �����
//
#define MDM_ERR_NO_MODEM_NUMBER          0xC1400035L

//
// MessageId: MDM_ERR_INVALID_PU_NUMBER
//
// MessageText:
//
//  ������������ ����� ��
//
#define MDM_ERR_INVALID_PU_NUMBER        0xC1400036L

//
// MessageId: MDM_ERR_INVALID_MODEM_NUMBER
//
// MessageText:
//
//  ����� ������ ������ ���� � ��������� 1 - 250
//
#define MDM_ERR_INVALID_MODEM_NUMBER     0xC1400037L

//
// MessageId: MDM_ERR_SEND_BUSY
//
// MessageText:
//
//  ����� ����� ��������� ������
//
#define MDM_ERR_SEND_BUSY                0xC1400064L

//
// MessageId: MDM_ERR_LINE_NOT_CONNECT
//
// MessageText:
//
//  ����� ����� �� �����
//
#define MDM_ERR_LINE_NOT_CONNECT         0xC1400065L    

//
// MessageId: MDM_ERR_LINE_RX_READY
//
// MessageText:
//
//  ��� ���������� �������� �����
//
#define MDM_ERR_LINE_RX_READY            0xC1400066L    

/*��������������*/
//
// MessageId: MDM_WRN_HANDLER_EXIST
//
// MessageText:
//
//  ���������� ��� ��������������
//
#define MDM_WRN_HANDLER_EXIST            0x81400001L


#endif

