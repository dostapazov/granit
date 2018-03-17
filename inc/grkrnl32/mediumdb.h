#ifndef _MEDIUMDB_INC
#define _MEDIUMDB_INC

#include <gkmodules.h>


#define MDB_RCSTR_BEGIN    5000
#define MDB_RCSTR_ORDER_LINE_OFF  (MDB_RCSTR_BEGIN+0)
#define MDB_RCSTR_ORDER_KADR      (MDB_RCSTR_BEGIN+1)
#define MDB_RCSTR_UNORDER_KADR    (MDB_RCSTR_BEGIN+2)
#define MDB_RCSTR_UNORDER_ALLKADR (MDB_RCSTR_BEGIN+3)
#define MDB_RCSTR_TU_OPERATION    (MDB_RCSTR_BEGIN+4)
#define MDB_RCSTR_TR_OPERATION    (MDB_RCSTR_BEGIN+5)
#define MDB_RCSTR_TU_OPNONE       (MDB_RCSTR_BEGIN+6)
#define MDB_RCSTR_TU_OPON         (MDB_RCSTR_BEGIN+7)
#define MDB_RCSTR_TU_OPOFF        (MDB_RCSTR_BEGIN+8)
#define MDB_RCSTR_TU_OPMORE       (MDB_RCSTR_BEGIN+9)
#define MDB_RCSTR_TU_OPLESS       (MDB_RCSTR_BEGIN+10)
#define MDB_RCSTR_TU_OPSET        (MDB_RCSTR_BEGIN+11)
#define MDB_RCSTR_TU_OPCANCEL     (MDB_RCSTR_BEGIN+12)

#define MDB_RCSTR_ALARM_RESTORE    (MDB_RCSTR_BEGIN+13)
#define MDB_RCSTR_ALARM_OFF        (MDB_RCSTR_BEGIN+14)
#define MDB_RCSTR_ALARM_CHANGED    (MDB_RCSTR_BEGIN+15)

#ifndef RC_INVOKED

#include <otd_proto_medium.h>
#include <otd_diag.h>
#include <modem_proto.h>


#define  MEDIUMDB_MODULE_NAME   L"MediumDB"
#define  MEDIUMDB_WINDOW_MODULE L"MediumDbWnd.dll"

#define MEDIUMDB_DATA   L"MDBDATA\\"
#define REGSTR_MDBDATA  L"MDBDATA"

typedef struct _MDB_SETTINGS
{
 wchar_t mdb_path[MAX_PATH<<1];

}MDB_SETTINGS,*LPMDB_SETTINGS;

#define MDB_CCFL_RECORD_TABLE     0x01000000
#define MDB_CCFL_COMBINE_TABLE    0x02000000
#define MDB_CCFL_KADRS_TABLE      0x04000000
#define MDB_CCFL_PAINT_VARS       0x08000000

#define MDB_CCFL_MASK            (MDB_CCFL_KADRS_TABLE|MDB_CCFL_COMBINE_TABLE|MDB_CCFL_RECORD_TABLE|MDB_CCFL_PAINT_VARS)
#define MDB_CCFL_SETTINGS_MDBPATH 0x80000000

typedef struct _MDB_RECORD_CHANGED
{
 DWORD   rec_id;
 DWORD   mask;
}MDB_RECORD_CHANGED,*LPMDB_RECORD_CHANGED;

#define MDB_NOTIFY_MASK_RECORD                0x00000001
#define MDB_NOTIFY_CODE_RECORD_CHANGED        0x00000001
#define MDB_NOTIFY_CODE_RECORD_CREATED        0x00000002
#define MDB_NOTIFY_CODE_RECORD_REMOVED        0x00000003
#define MDB_NOTIFY_CODE_DBREOPEN              0x10000000

/*Уведомления об изменении кадра*/
#define MDB_NOTIFY_MASK_KADR                  0x00000002
#define MDB_NOTIFY_CODE_KADR                  0x00000001
#define MDB_NOTIFY_CODE_KADR_CREATED          0x00000002
#define MDB_NOTIFY_CODE_KADR_CONTENT          0x00000003
#define MDB_NOTIFY_CODE_KADR_REMOVED          0x00000004

#define MDB_NOTIFY_MASK_PAINTVAR              0x00000004
#define MDB_NOTIFY_CODE_PAINTVAR_ADD          1
#define MDB_NOTIFY_CODE_PAINTVAR_UPD          2
#define MDB_NOTIFY_CODE_PAINTVAR_CLEAR        3
#define MDB_NOTIFY_CODE_PAINTVAR_END_UPDATE   4

typedef struct _MDB_KADR_CHANGED
{
 DWORD   kadr_id;
 DWORD   mask;
}MDB_KADR_CHANGED,*LPMDB_KADR_CHANGED;


#pragma pack (push,1)

typedef  struct _MDB_COMBINE
{
  DWORD  id_combine;
  DWORD  obj_count;
  MDB_RECORD_TEXT  formula;
}MDB_COMBINE,*LPMDB_COMBINE;


#define MDBCM_RECORD_ENUM_BYID      MCMD(MT_MEDIUMDB,1)
//p1 = idx,p2 LPMDB_RECORD
#define MDBCM_RECORD_ENUM_BYADDR    MCMD(MT_MEDIUMDB,2)
//p1 = idx,p2 LPMDB_RECORD

#define MDBCM_ENUM_COMBINED         MCMD(MT_MEDIUMDB,18)
//p1 = idx,p2 LPMDB_RECORD

#define MDBCM_RECORD_GET            MCMD(MT_MEDIUMDB,3)
//p1 = Id,p2 = LPMDB_RECORD;
//Если id == -1 то ищется по адресу




//Получение короткого имени
#define MDBCM_RECORD_GET_SNAME     MCMD(MT_MEDIUMDB,4)
//p1 = id, p2 = LPMDB_RECORD_TEXT

//Получение полного имени
#define MDBCM_RECORD_GET_NAME      MCMD(MT_MEDIUMDB,5)
//p1 = id, p2 = LPMDB_RECORD_TEXT

//Установка короткого имени
#define MDBCM_RECORD_SET_SNAME     MCMD(MT_MEDIUMDB,6)
//p1 = id, p2 = LPMDB_RECORD_TEXT
//Установка  имени
#define MDBCM_RECORD_SET_NAME      MCMD(MT_MEDIUMDB,7)
//p1 = id, p2 = LPMDB_RECORD_TEXT

#define MDBCM_GET_COUNT            MCMD(MT_MEDIUMDB,8)
//p1 = DWORD otd_addr ret = count;

#define MDBCM_ENUM_OBJECTS         MCMD(MT_MEDIUMDB,9)
//Перечисление объектов по адресу
// p1 = otd_addr p2 = idx
// ret = id;


#define MDBCM_COMBINE_GET_COUNT    MCMD(MT_MEDIUMDB,10)
//Получение количества комбинированных объектов или кол-ва подобъектов
//p1 = Id - комбинированного или -1
//ret количество подобъектов

#define MDBCM_COMBINE_ENUM    MCMD(MT_MEDIUMDB,11)
//Перечисление вычисляемых объектов
//p1 = index  p2 = LPMDB_RECORD ret = id

#define MDBCM_COMBINE_GET_FORMULA MCMD(MT_MEDIUMDB,12)
//Получение формулы комбинированного объекта
//p1 = DWORD comb_id  p2 = LPMDB_RECORD_TEXT

#define MDBCM_COMBINE_SET_FORMULA MCMD(MT_MEDIUMDB,13)
//Установка формулы
//p1 = DWORD comb_id  p2 = LPMDB_RECORD_TEXT

#define MDBCM_COMBINE_GET_LIST     MCMD(MT_MEDIUMDB,14)
//Получение списка объектов входящих в состав комбинированного
//p1 = id  p2 = LPMDB_COMBINE_LIST

#define MDBCM_COMBINE_SET_LIST     MCMD(MT_MEDIUMDB,15)
//Получение списка объектов входящих в состав комбинированного
//p1 = id  p2 = LPMDB_COMBINE_LIST

typedef struct _MDB_COMBINE_LIST
{
 DWORD dw_size;
 DWORD capacity; //Количество obj_id
 DWORD start;    //Стартовый  индекс
 DWORD count;    //Количество
 DWORD obj_id[ANYSIZE_ARRAY];
}MDB_COMBINE_LIST,*LPMDB_COMBINE_LIST;

#define MDBCM_UPDATE_TEXT        MCMD(MT_MEDIUMDB,16)
//p1  = ID_TEXT Если == -1 то добавление
//p2  = LPMDB_RECORD_TEXT
//ret = ID_TEXT

#define MDBCM_GET_TEXT           MCMD(MT_MEDIUMDB,17)
//p1  = ID_TEXT
//p2  = LPMDB_RECORD_TEXT
//ret = ID_TEXT

#define MDBCM_ENUM_COMBINED      MCMD(MT_MEDIUMDB,18)


/*Получение информации о базе*/
#define MDBCM_GET_DBINFO         MCMD(MT_MEDIUMDB,20)
// p1 = LPMDB_DBINFO

typedef struct _MDB_DBINFO
{
 DWORD dw_size;
 DWORD gen_record_id;  // Следующее значение id - записи
 DWORD records_count;  // Количество записей
 DWORD names_count;    // количество имен
 DWORD combines_count; // Количество комбинированных объектов
 DWORD gen_kadr_id;    // следующее значение id кадра
 DWORD kadrs_count;    // Количество кадров

}MDB_DBINFO,*LPMDB_DBINFO;

#define MDBCM_CHECK_REPAIR         MCMD(MT_MEDIUMDB,50)

typedef DWORD CALLBACK (  * check_callback_proc)(LPMDB_RECORD,DWORD error);
typedef struct _MDB_CHECK_PARAM
{
 DWORD dw_size;
 check_callback_proc   c_proc;
}MDB_CHECK_PARAM,*LPMDB_CHECK_PARAM;


#define MDBCM_RECORD_CREATE      MCMD(MT_MEDIUMDB,100)
//Создание записи
//p1 = LPMDB_RECORD
//ret id - записи

#define MDBCM_RECORD_REMOVE      MCMD(MT_MEDIUMDB,101)
//Удаление записи  p1 = rec_id

#define MDBCM_RECORD_UPDATE      MCMD(MT_MEDIUMDB,102)
//Изменение записи
//p1 = LPMDB_RECORD
//p2 = field_mask
//ret id - записи

/*
 Добавление записей по маске адреса
 p1 = DWORD otd_addr_mask;

*/
#define MDBCM_ADDRECORDS_BYMASK  MCMD(MT_MEDIUMDB,110)




#define MDBCM_KADR_ENUM         MCMD(MT_MEDIUMDB,200)
/*
 Перечисление кадров
 p1 = index  p2 = LPMDB_KADR
 ret = kadr_id
*/
#define MDBCM_KADR_GET           MCMD(MT_MEDIUMDB,201)
/*
 Получение кадра
 p1 = kadr_id  p2 = LPMDB_KADR
 ret = kadr_id
*/

#define MDBCM_KADR_CREATE        MCMD(MT_MEDIUMDB,202)
/*
 Создание нового кадра
 p1 = LPMDB_KADR
 ret = kadr_id;

*/
#define MDBCM_KADR_REMOVE        MCMD(MT_MEDIUMDB,203)
/*
  Удаление кадра
  p1 = kadr_id
*/

#define MDBCM_KADR_UPDATE        MCMD(MT_MEDIUMDB,204)
/*
  Изменений кадра
  p1 = LPMDB_KADR
  p2 = kadr fields mask
*/


#define MDBCM_KADR_GET_ENTRY_LIST     MCMD(MT_MEDIUMDB,205)
// p1 == kadr_id;
// p2 = LPMDB_KADR_ENTRY_LIST
#define MDBCM_KADR_SET_ENTRY_LIST     MCMD(MT_MEDIUMDB,206)
// p1 == kadr_id;
// p2 = LPMDB_KADR_ENTRY_LIST

#define MDBCM_KADR_SET_NAME           MCMD(MT_MEDIUMDB,210)
//p1 = kadr_id
//p2 - LPMDB_RECORD_TEXT

#define MDBCM_KADR_SET_IMAGE           MCMD(MT_MEDIUMDB,211)
//p1 = kadr_id
//p2 - LPMDB_RECORD_TEXT

#define MDBCM_KADR_FIND                MCMD(MT_MEDIUMDB,212)
//Поиск кадра
// p1 = Wchar_t * kadr_name;
// p2 = LPDWORD   kadr_id

#define MDBCM_KADR_FIND_RECORD         MCMD(MT_MEDIUMDB,213)
//Поиск записи в кадре
//p1 = kadr_id
//p2 = record_id
//return LRESULT = entry_num or GKH_RET_ERROR

#define MDBCM_KADR_ADD_RECORD         MCMD(MT_MEDIUMDB,214)
//Добавить запись в кадр
//p1 = kadr_id
//p2 = record_id
//return LRESULT = entry_num or GKH_RET_ERROR

struct ORDER_ITEM
{
 DWORD       dw_size;
 DWORD       kadr_id;
 MODEM_ADDR  dest_addr;
 DWORD       order_mask;
};

typedef ORDER_ITEM* LPORDER_ITEM;

#define  MDBCM_ENUM_ORDERS             MCMD(MT_MEDIUMDB,250)
//Перечисление заказчиков
//p1  = index, p2 LPORDER_ITEM;
//ret  = GKH_RET_ERROR по завершению


#define MDBCM_PAINTVARS_ENUM           MCMD(MT_MEDIUMDB,300)
//Перечисление вариантов отрисовки
//p1 index p2 LPMDB_PAINT_VARIANT

#define MDBCM_PAINTVARS_ADD            MCMD(MT_MEDIUMDB,301)
//Добавление варианта отрисовки
//p1  LPMDB_PAINT_VARIANT

#define MDBCM_PAINTVARS_UPD            MCMD(MT_MEDIUMDB,302)
//Изменение варианта отрисовки
//p1  LPMDB_PAINT_VARIANT
#define MDBCM_PAINTVARS_CLEAR          MCMD(MT_MEDIUMDB,303)

#define MDBCM_PAINTVARS_END_UPDATE     MCMD(MT_MEDIUMDB,304)

#define MDBCM_COMBINE_REBUILD_INDEX    MCMD(MT_MEDIUMDB,1000)

/*Операции над записью*/
#define MDBCM_RECORD_OPERATION         MCMD(MT_MEDIUMDB,2000)
// p1  = record_id
// p2  = op_code

#define MDBR_OPERATION_TUON_MORE      OTD_TUOP_ON
#define MDBR_OPERATION_TUOFF_LESS     OTD_TUOP_OFF
#define MDBR_OPERATION_TUTR_CANCEL    OTD_TUTROP_CANCEL



#pragma pack (pop)


#endif // _RCINVOKED

#endif
