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
Ошибка загрузки клиентской библиотеки
.
Language=Eng
Error loading client library
.

MessageId    = 2
Severity     = Error
Facility     = REC_HIST_FCL
SymbolicName = REC_HIST_ERROR_CONNECT_DATABASE
Language=Russian
Ошибка подключения к базе данных;
.
Language=Eng
Database connect error
.

MessageId    = 3
Severity     = Error
Facility     = REC_HIST_FCL
SymbolicName = REC_HIST_ERROR_NO_TABLES
Language=Russian
База данных не проинициализирована.(Отсутствуют необходимые таблицы))
.
Language=Eng
The database is not initialized. (Lack of the necessary tables)
.

MessageId    = 4
Severity     = Error
Facility     = REC_HIST_FCL
SymbolicName = REC_HIST_ERROR_SQL_QUERY
Language=Russian
Ошибка SQL-запроса
.
Language=Eng
Error SQL-query
.

MessageId    = 5
Severity     = Error
Facility     = REC_HIST_FCL
SymbolicName = REC_HIST_ERROR_START_TRANSACTION
Language=Russian
Ошибка старта транзакции
.
Language=Eng
Error start transaction
.

MessageId    = 6
Severity     = Error
Facility     = REC_HIST_FCL
SymbolicName = REC_HIST_ERROR_PREPARE_QUERY
Language=Russian
Ошибка в тексте запроса
.
Language=Eng
Error in the query statement
.

;#endif
