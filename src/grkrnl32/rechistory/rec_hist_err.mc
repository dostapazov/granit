;/*
;
; Error codes common for GRANIT modem
;
;*/
;#ifndef _GRREC_HIST_ERR_INC
;#define _GRREC_HIST_ERR_INC
;#include <gkcomerr.h>
;
;


FacilityNames = (
                 REC_HIST_FCL = 0x0143:REC_HIST_FCL
                )

LanguageNames = (
                 Eng     = 0x409:REC_HIST_ERR_EN
                 Russian = 0x419:REC_HIST_ERR_RU
                )



MessageId    = 1
Severity     = Error
Facility     = REC_HIST_FCL
SymbolicName = REC_HIST_ERROR_LOAD_CLIENTLIB
Language=Russian
������ �������� ���������� ����������
.
Language=Eng
Error loading client library
.

MessageId    = 2
Severity     = Error
Facility     = REC_HIST_FCL
SymbolicName = REC_HIST_ERROR_CONNECT_DATABASE
Language=Russian
������ ����������� � ���� ������;
.
Language=Eng
Database connect error
.

MessageId    = 3
Severity     = Error
Facility     = REC_HIST_FCL
SymbolicName = REC_HIST_ERROR_NO_TABLES
Language=Russian
���� ������ �� �������������������.(����������� ����������� �������))
.
Language=Eng
The database is not initialized. (Lack of the necessary tables)
.

MessageId    = 4
Severity     = Error
Facility     = REC_HIST_FCL
SymbolicName = REC_HIST_ERROR_SQL_QUERY
Language=Russian
������ SQL-�������
.
Language=Eng
Error SQL-query
.

MessageId    = 5
Severity     = Error
Facility     = REC_HIST_FCL
SymbolicName = REC_HIST_ERROR_START_TRANSACTION
Language=Russian
������ ������ ����������
.
Language=Eng
Error start transaction
.

MessageId    = 6
Severity     = Error
Facility     = REC_HIST_FCL
SymbolicName = REC_HIST_ERROR_PREPARE_QUERY
Language=Russian
������ � ������ �������
.
Language=Eng
Error in the query statement
.

;#endif
