#ifndef __GKTLMDB_INC
#define __GKTLMDB_INC

#include <gkmodules.h>
#include <gktlmdberr.h>
#include <gktlmdb.rh>

#ifndef _RC_INVOKED
#include <otd.h>
#include <modem_proto.h>
#include <otd_addr.h>


#define GKTLMDB_HANDLE_NAME_A "GKTLMDB"
#define GKTLMDB_HANDLE_NAME_W L"GKTLMDB"
#define TLMDB_NAME_MAXSIZE   64

#define TLMDB_REGSTR_DBPATH   L"DbPath"
#define TLMDB_REGDW_OPTIONS   L"options"




#pragma pack(push,1)

#define CCFL_REGPARAMS 1 // флаг наличия изменений в настройках
#define CCFL_DB        4 // флаг наличия изменений в базе данных


#define TLM_DBLOW_NOTIFY_CHANGE         0x00000001
#define TLM_DBLOW_CODE_ADDED            1         //Добавлен элемент ы БД
#define TLM_DBLOW_CODE_REMOVED          2         //Удалён элемент из БД
#define TLM_DBLOW_CODE_CHANGED          4         //Изменен при обработке или еще как нибудь

#define TLMDB_DBLOW_MONITOR             0x00000002
#define TLMDB_DBLOW_MONITOR_CHANGES     1
#define TLMDB_DBLOW_MONITOR_RESPOND     2
#define TLMDB_DBLOW_MONITOR_ORDER       3

#define TLMDBLOW_MAXNAMELEN             128


typedef struct _DBLOW_CHANGES_STRUCT
{
 DWORD addr;
 DWORD parts;
 DWORD loN;
 DWORD hiN;
}DBLOW_CHANGES_STRUCT,*LPDBLOW_CHANGES_STRUCT;

//Расширение 
struct dblow_ext
{
   BOOL          is_static;
   DWORD         modem_source;
};



/* Команды обрабатываемые модулем GKTLMDB*/
/*+соответствующие им структуры*/

typedef struct _TLMDB_SETTINGS
{
 DWORD           options;
 wchar_t         db_file_name[MAX_PATH<<1];//Путь к файлу для сохранения БД
}TLMDB_SETTINGS,*LPTLMDB_SETTINGS;

#define TLMDB_SMASK_DBF   1
#define TLMDB_SMASK_OPT   2

//#define TLMDB_OPT_HANDLE_ALARM_ARCHIVES   1


typedef struct _TLMDB_LOW_GET_ENTRY
{

 DWORD dw_size;
 DWORD parts;
 DWORD loN;
 DWORD hiN;
 DWORD data_size;
 int   data_len;
 BYTE  data[ANYSIZE_ARRAY];

}TLMDB_LOW_GET_ENTRY,*LPTLMDB_LOW_GET_ENTRY;

#define TLMDBCM_DBLOW_ENUM_ENTRY MCMD(MT_TLMDB,1)
//p1   = index ,p2 = LPTLMDB_GET_DBENTRY

#define TLMDBCM_DBLOW_GET_ENTRY MCMD(MT_TLMDB,2)
//p1 = otd_addr p2 = LPTLMDB_GET_DBENTRY

#define TLMDBCM_DBLOW_GET_SRCMODEM MCMD(MT_TLMDB,10)
//p1   = otd_addr
//ret  = modem_addr;

typedef struct  _TLMDB_LOW_RECREATE
{
 DWORD   dw_size;
 DWORD   lo_obj;
 DWORD   hi_obj;
 wchar_t name[TLMDBLOW_MAXNAMELEN];
}TLMDB_LOW_RECREATE,*LPTLMDB_LOW_RECREATE;

#define TLMDBCM_DBLOW_RECREATE     MCMD(MT_TLMDB,20)
//p1 = otd_addr;
//p2 = LPTLMDB_LOW_RECREATE;


#define TLMDBCM_DBLOW_MAKE_STATIC  MCMD(MT_TLMDB,21)
//p1 = otd_addr;
//p2 = TRUE static FALSE dynamic;


//Отправить вызов на 
#define TLMDBCM_DBLOW_REFRESH_ADDR MCMD(MT_TLMDB,100)
//p1 = otd_addr  , p2 = BOOL order

#define TLMDCMD_LOWDB_REMOVE_INVALID_DINAMIC  MCMD(MT_TLMDB,200)
//Удалить неисправные динамические объекты

typedef struct _TLMDB_ORDER_ENTRY
{
 DWORD         dw_size;
 MODEM_ADDR    addr;
 otd_addr      ordered_addr;
 DWORD ordered_parts;

}TLMDB_ORDER_ENTRY,*LPTLMDB_ORDER_ENTRY;

#define TLMDBCM_ENUM_ORDERS      MCMD(MT_TLMDB,300)
//Перечисление подписок
//HIWORD(p1) - индекс модема,LOWORD(p1) индекс заказа,p2 = LPTLMDB_ORDER_ENTRY

#define RET_ORDERS_SUCCESS        0
#define RET_ORDERS_NO_MORE_ORDERS 1
#define RET_ORDERS_NO_MORE_MODEMS 2



//Команда ТУ/ТР
#define TLMDBCM_DBLOW_TUOFF       MCMD(MT_TLMDB,500)
#define TLMDBCM_DBLOW_TUON        MCMD(MT_TLMDB,501)
#define TLMDBCM_DBLOW_TUUNDO      MCMD(MT_TLMDB,502)
//p1 = DWORD addr, p2 = obj






#pragma pack(pop)
#endif
#endif


