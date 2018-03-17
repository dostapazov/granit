/*
 Error codes common for all units
 And probably GRKERNEL32



*/
#ifndef __GKCOMM_MSG_INC
#define __GKCOMM_MSG_INC


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
#define FACILITY_GKCOMMON                0x100


//
// Define the severity codes
//


//
// MessageId: MERR_INVALID_BUFFER_SIZE
//
// MessageText:
//
//  ����������� ����� ������ ������
//
#define MERR_INVALID_BUFFER_SIZE         0xC1000001L

//
// MessageId: MERR_DLLNOT_MODULE
//
// MessageText:
//
//  ������������ ���������� �� �������� �������
//
#define MERR_DLLNOT_MODULE               0xC1000002L

//
// MessageId: MERR_CREATE_GKHANDLE
//
// MessageText:
//
//  ������ ������� ������ 
//
#define MERR_CREATE_GKHANDLE             0xC1000003L

//
// MessageId: MERR_UNSUPPORTED_MODULE_TYPE
//
// MessageText:
//
//  ���������������� ��� ������
//
#define MERR_UNSUPPORTED_MODULE_TYPE     0xC1000004L

//
// MessageId: MERR_MODULE_NOTRUNNING
//
// MessageText:
//
//  ������ � ��������� ���� ��� �����
//
#define MERR_MODULE_NOTRUNNING           0xC1000005L

//
// MessageId: MERR_OWNER_NOTRUNNING
//
// MessageText:
//
//  ������ �������� � ��������� ���� ��� �����
//
#define MERR_OWNER_NOTRUNNING            0xC1000006L

//
// MessageId: MERR_WRITE_IN_REGISTRY
//
// MessageText:
//
//  ������ ��� ������ � ��������� ������
//
#define MERR_WRITE_IN_REGISTRY           0xC1000007L

//
// MessageId: MERR_REGISTRY_MODULE_IMAGE
//
// MessageText:
//
//  ������ ������ ����� ������ ������ �� �������
//
#define MERR_REGISTRY_MODULE_IMAGE       0xC1000008L    

//
// MessageId: MERR_MODULE_INITIALIZATION
//
// MessageText:
//
//  ������ ������������� ������
//
#define MERR_MODULE_INITIALIZATION       0xC1000009L

//
// MessageId: MERR_GKHANDLE_EXISTS
//
// MessageText:
//
//  ����� � ����� ������ ��� ����������
//
#define MERR_GKHANDLE_EXISTS             0xC100000AL

//
// MessageId: MERR_INVALID_MODULE_CONFIG
//
// MessageText:
//
//  ��������� ������������ ������
//
#define MERR_INVALID_MODULE_CONFIG       0xC100000BL

//
// MessageId: MERR_EVAL_PERIOD_EXPIRED
//
// MessageText:
//
//  ������ �������� ������������� �����;
//
#define MERR_EVAL_PERIOD_EXPIRED         0xC100000CL

//
// MessageId: MERR_UNREGISTERED_MODULE_COPY
//
// MessageText:
//
//  �������������������� ����� ������;
//
#define MERR_UNREGISTERED_MODULE_COPY    0xC100000DL

//
// MessageId: MERR_NOMORE_DATA
//
// MessageText:
//
//  ������ ������ ���;
//
#define MERR_NOMORE_DATA                 0xC1000064L

//
// MessageId: MWRN_UNKNOWN_MODULE_COMMAND
//
// MessageText:
//
//  ����������� ��� ��������������� ������� ������
//
#define MWRN_UNKNOWN_MODULE_COMMAND      0x81000001L


#endif

