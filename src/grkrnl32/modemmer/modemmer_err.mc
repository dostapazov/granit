;/*
; Error codes common for modemmer and modems
;
;
;*/
;#ifndef __MODEMMER_MSG_INC
;#define __MODEMMER_MSG_INC
;#include <gkcomerr.h>
;
;


FacilityNames = (
                 GKModemmer = 0x0140:FACILITY_GKMODEMMER
                )

LanguageNames = (
                 Eng     = 0x409:GKERR_EN
                 Russian = 0x419:GKERR_RU
                )



MessageId    = 1
Severity     = Error
Facility     = GKModemmer
SymbolicName = MDM_ERR_INVALID_FA
Language=Russian
����������� ��
.
Language=Eng
Invalid FA
.

MessageId    = 2
Severity     = Error
Facility     = GKModemmer
SymbolicName = MDM_ERR_NO_FA_HANDLER
Language=Russian
���������� ����������� ��
.
Language=Eng
No FA Handlers
.

MessageId    = 50
Severity     = Error
Facility     = GKModemmer
SymbolicName = MDM_ERR_NO_MODEM
Language=Russian
����� � ����� ������� �� ����������
.
Language=Eng
modem number absent 
.

MessageId    = 51
Severity     = Error
Facility     = GKModemmer
SymbolicName = MDM_ERR_NO_MODEMLINE
Language=Russian
����� ������ �� ����������
.
Language=Eng
No modem line
.

MessageId    = 52
Severity     = Error
Facility     = GKModemmer
SymbolicName = MDM_ERR_USED_MODEM_NUMBER
Language=Russian
����� ����� ������ ��� ������������
.
Language=Eng
This modem number always used
.

MessageId    = 53
Severity     = Error
Facility     = GKModemmer
SymbolicName = MDM_ERR_NO_MODEM_NUMBER
Language=Russian
������ �� �������� �����
.
Language=Eng
Modem whith unassigned number
.

MessageId    = 54
Severity     = Error
Facility     = GKModemmer
SymbolicName = MDM_ERR_INVALID_PU_NUMBER
Language=Russian
������������ ����� ��
.
Language=Eng
Invalid PU number
.

MessageId    = 55
Severity     = Error
Facility     = GKModemmer
SymbolicName = MDM_ERR_INVALID_MODEM_NUMBER
Language=Russian
����� ������ ������ ���� � ��������� 1 - 250
.
Language=Eng
Modem number must be between 1-250
.


MessageId    = 100
Severity     = Error
Facility     = GKModemmer
SymbolicName = MDM_ERR_SEND_BUSY
Language=Russian
����� ����� ��������� ������
.
Language=Eng
Modem is sending data now
.

MessageId    = 101
Severity     = Error
Facility     = GKModemmer
SymbolicName = MDM_ERR_LINE_NOT_CONNECT;
Language=Russian
����� ����� �� �����
.
Language=Eng
Modem line not connected
.

MessageId    = 102
Severity     = Error
Facility     = GKModemmer
SymbolicName = MDM_ERR_LINE_RX_READY;
Language=Russian
��� ���������� �������� �����
.
Language=Eng
Line not ready to send
.




;/*��������������*/

MessageId    = 1
Severity     = Warning
Facility     = GKModemmer
SymbolicName = MDM_WRN_HANDLER_EXIST
Language=Russian
���������� ��� ��������������
.
Language=Eng
Handler already registered
.

;
;#endif
;
