;/*
; Error codes common for all units
; And probably GRKERNEL32
;
;
;
;*/
;#ifndef __GKCOMM_MSG_INC
;#define __GKCOMM_MSG_INC
;
;


FacilityNames = (
                 GKCommon  = 0x0100:FACILITY_GKCOMMON
                )

LanguageNames = (
                 Eng     = 0x409:GKERR_EN
                 Russian = 0x419:GKERR_RU
                )




MessageId    = 1
Severity     = Error
Facility     = GKCommon
SymbolicName = MERR_INVALID_BUFFER_SIZE
Language=Russian
����������� ����� ������ ������
.
Language=Eng
Buffer size to small
.

MessageId    = 2
Severity     = Error
Facility     = GKCommon
SymbolicName = MERR_DLLNOT_MODULE
Language=Russian
������������ ���������� �� �������� �������
.
Language=Eng
DLL is not module
.

MessageId    = 3
Severity     = Error
Facility     = GKCommon
SymbolicName = MERR_CREATE_GKHANDLE
Language=Russian
������ ������� ������ 
.
Language=Eng
Error creating handle
.


MessageId    = 4
Severity     = Error
Facility     = GKCommon
SymbolicName = MERR_UNSUPPORTED_MODULE_TYPE
Language=Russian
���������������� ��� ������
.
Language=Eng
Unsupported module type
.

MessageId    = 5
Severity     = Error
Facility     = GKCommon
SymbolicName = MERR_MODULE_NOTRUNNING
Language=Russian
������ � ��������� ���� ��� �����
.
Language=Eng
Module not running or paused
.

MessageId    = 6
Severity     = Error
Facility     = GKCommon
SymbolicName = MERR_OWNER_NOTRUNNING
Language=Russian
������ �������� � ��������� ���� ��� �����
.
Language=Eng
Owner module not running or paused
.


MessageId    = 7
Severity     = Error
Facility     = GKCommon
SymbolicName = MERR_WRITE_IN_REGISTRY
Language=Russian
������ ��� ������ � ��������� ������
.
Language=Eng
System registry write error
.

MessageId    = 8
Severity     = Error
Facility     = GKCommon
SymbolicName = MERR_REGISTRY_MODULE_IMAGE;
Language=Russian
������ ������ ����� ������ ������ �� �������
.
Language=Eng
Error read module image name from registry
.


MessageId    = 9
Severity     = Error
Facility     = GKCommon
SymbolicName = MERR_MODULE_INITIALIZATION
Language=Russian
������ ������������� ������
.
Language=Eng
Error module initialization
.

MessageId    = 10
Severity     = Error
Facility     = GKCommon
SymbolicName = MERR_GKHANDLE_EXISTS
Language=Russian
����� � ����� ������ ��� ����������
.
Language=Eng
Handle already exists
.

MessageId    = 11
Severity     = Error
Facility     = GKCommon
SymbolicName = MERR_INVALID_MODULE_CONFIG
Language=Russian
��������� ������������ ������
.
Language=Eng
Invalid module configuration
.

MessageId    = 12
Severity     = Error
Facility     = GKCommon
SymbolicName = MERR_EVAL_PERIOD_EXPIRED
Language=Russian
������ �������� ������������� �����;
.
Language=Eng
Evaluation period is expired
.

MessageId    = 13
Severity     = Error
Facility     = GKCommon
SymbolicName = MERR_UNREGISTERED_MODULE_COPY
Language=Russian
�������������������� ����� ������;
.
Language=Eng
Unregistered module copy
.


MessageId    = 100
Severity     = Error
Facility     = GKCommon
SymbolicName = MERR_NOMORE_DATA
Language=Russian
������ ������ ���;
.
Language=Eng
No more data
.

MessageId    = 1
Severity     = Warning
Facility     = GKCommon
SymbolicName = MWRN_UNKNOWN_MODULE_COMMAND
Language=Russian
����������� ��� ��������������� ������� ������
.
Language=Eng
Unknown or unimplemented module command 
.

;
;#endif
;

