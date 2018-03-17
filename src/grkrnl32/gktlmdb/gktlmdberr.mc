;/*
; Error codes common for all units
; And probably GRKERNEL32
;
;
;
;*/
;#ifndef __GKTLMDB_MSG_INC
;#define __GKTLMDB_MSG_INC
;
;


FacilityNames = (
                 GKCommon  = 0x0100:FACILITY_GKCOMMON
                 GKtlmdb   = 0x0130:FACILITY_GKTLMDB
                )

LanguageNames = (
                 Eng     = 0x409:GKERR_EN
                 Russian = 0x419:GKERR_RU
                )



MessageId    = 1
Severity     = Error
Facility     = GKtlmdb
SymbolicName = TLMDB_ERR_NOOWNER_SET
Language=Russian
��� ������ ��������� ������
.
Language=Eng
No module ownership group
.

MessageId    = 2
Severity     = Error
Facility     = GKtlmdb
SymbolicName = TLMDB_ERR_INCORRECT_MODULE_ADDR
Language=Russian
������ ������ ������
.
Language=Eng
Module address error
.

MessageId    = 3
Severity     = Error
Facility     = GKtlmdb
SymbolicName = TLMDB_ERR_MODULE_EXIST
Language=Russian
������ � ����� ���������������� ������� ��� ����������
.
Language=Eng
Module with same address already exists
.


MessageId    = 4
Severity     = Error
Facility     = GKtlmdb
SymbolicName = TLMDB_ERR_MODULE_EMPTY
Language=Russian
������ �� ������ (tlm_module::create)
.
Language=Eng
Module not created (tlm_module::create)

.

MessageId    = 5
Severity     = Error
Facility     = GKtlmdb
SymbolicName = TLMDB_ERR_MODULE_NOTFOUND
Language=Russian
������ �� ������
.
Language=Eng
Module not found
.

MessageId    = 6
Severity     = Error
Facility     = GKtlmdb
SymbolicName = TLMDB_ERR_MODULE_NUMBER
Language=Russian
������������ ����� ������
.
Language=Eng
Module number out of range
.

MessageId    = 7
Severity     = Error
Facility     = GKtlmdb
SymbolicName = TLMDB_ERR_NO_MORE_ITEMS
Language=Russian
��� ������ ��������� � ��
.
Language=Eng
No more items
.









;
;#endif
;
