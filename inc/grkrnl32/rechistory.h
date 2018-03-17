#ifndef _RECHISTORY_INC_H_
#define _RECHISTORY_INC_H_

#include <gkmodules.h>
#include "rec_hist_err.h"

#ifndef RC_INVOKED

#define RECHISTORY_MOD_NAME    L"RECHISTORY_MOD"
#define RECHISTORY_WNDMOD_NAME L"rechistory_wnd"

#define RECHISTORY_NOTIFY_STORE       0x000001
#define RECHISTORY_NOTIFY_COMMIT      0x000002
#define RECHISTORY_NOTIFY_UPDIDX      0x000004
#define RECHISTORY_NOTIFY_READ        0x000008
#define RECHISTORY_NOTIFY_SEND        0x000010
#define RECHISTORY_NOTIFY_OPENDB      0x000020
#define RECHISTORY_NOTIFY_UPDATE      0x000040
#define RECHISTORY_NOTIFY_STATS       0x000080




//Множитель для хранения в БД реальных значений
#define FLOAT_MULTYPLIER 100000

#define RCH_CONF_CLIENTLIB  1
#define RCH_CONF_DBPATH     2
#define RCH_CONF_USERNAME   4
#define RCH_CONF_USERPASSW  8
#define RCH_CONF_WR_CHANGES          0x0010
#define RCH_CONF_WR_ALARM_ARCHIVES   0x0020
#define RCH_CONF_SQL_WRHISTROY       0x0040
#define RCH_CONF_SQL_RDHISTROY       0x0080
#define RCH_CONF_COMMIT_TIMEOUT      0x0100
#define RCH_CONF_COMMIT_AFTER        0x0200
#define RCH_CONF_TIMESTAMP_LIMIT     0x0400
#define RCH_CONF_SQL_CLEAR_HIST      0x0800
#define RCH_CONF_SQL_UPD_IDX         0x1000

#pragma pack(push,1)
struct REC_HIST_CONFIG
{
 DWORD dw_size;
 wchar_t client_lib[MAX_PATH<<1];
 wchar_t dbpath    [MAX_PATH];
 wchar_t user_name [32];
 wchar_t password  [32];
 BOOL    write_changes;
 BOOL    write_alarm_archives;
 int     commit_time_out;
 int     commit_after;
 __int64 timestamp_limit;

 char    sql_wr_history[2048];
 char    sql_rd_history[2048];
 char    sql_clear_hist[2048];
 char    sql_idx_update[2048];
};



#define RECHIST_MODULE_VERSION     2

#define REGSTR_CLIENT_LIB          L"CLIENT_LIB"
#define REGSTR_DBPATH              L"DB_PATH"
#define REGSTR_USER                L"SU"
#define REGSTR_PASSW               L"SP"
#define REGDW_WRITE_ALARM_ARCHIVES L"WR_ALARM_ARCHIVE"
#define REGDW_WRITE_CHANGES        L"WR_CHANGES"
#define REGDW_MOD_VERSION          L"ModuleVersion"
#define REGSTR_WRITE_QUERY         L"SQL_WRITE_QUERY"
#define REGSTR_READ_QUERY          L"SQL_READ_QUERY"
#define REGSTR_CLEAR_HIST          L"SQL_CLEAR_HIST"
#define REGSTR_IDX_UPDATE          L"SQL_IDX_UPDATE"
#define REGBIN_ORDERS              L"ORDERS"
#define REGDW_COMMIT_TIMEOUT       L"COMMIT_TIMEOUT"
#define REGDW_COMMIT_AFTER         L"COMMIT_AFTER"
#define REGBIN_TIMESTAMP_LIMIT     L"TIMESTAMP_LIMIT"


#define RCH_CMD_ENUM_KADRS  MCMD(MT_MEDIUMDB_HISTORY,1)
#define RCH_CMD_ADD_KADR    MCMD(MT_MEDIUMDB_HISTORY,2)
#define RCH_CMD_REMOVE_KADR MCMD(MT_MEDIUMDB_HISTORY,3)


typedef REC_HIST_CONFIG* LPREC_HIST_CONFIG;

//Команда восстановления БД
#define RHCM_RESTORE_DATABASE MCMD(MT_MEDIUMDB_HISTORY,1000)
//p1 = LPRECHIST_RESTORE

struct RECHIST_RESTORE
{
 DWORD dw_size;
 char target_name[MAX_PATH<<1];
 char user_name[64];
 char password [64];
 int page_size;
 int buffers;
};

typedef RECHIST_RESTORE* LPRECHIST_RESTORE;

//Запуск обсдуживания бд
#define RHCM_START_MAINTANCE MCMD(MT_MEDIUMDB_HISTORY,1001)

//Получение статистики бд
#define RHCM_GET_STATISTICS  MCMD(MT_MEDIUMDB_HISTORY,1002)


#pragma pack(pop)
#endif
#endif

