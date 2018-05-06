#ifndef __GRANIT_MODEM_INC
#define __GRANIT_MODEM_INC

#include  <modemmer.h>
#include  <granit_modem_err.h>


#define IDS_SC_FLOOR_TEXT           2000

#define IDS_SC_EVENT_LOGTITLE       2500
#define IDS_SC_TUTR_LOGTITLE        2501

#define IDS_SC_LOG0                 3000
#define IDS_SC_LOG1                 3001
#define IDS_SC_LOG2                 3002
#define IDS_SC_LOG3                 3003
#define IDS_SC_LOG4                 3004
#define IDS_SC_LOG5                 3005
#define IDS_SC_LOG6                 3006
#define IDS_SC_LOG7                 3007
#define IDS_SC_LOG8                 3008
#define IDS_SC_LOG9                 3009
#define IDS_SC_LOG10                3010

#define IDS_SC_TUTR_BEGIN           4000
#define IDS_SC_TUON                 (IDS_SC_TUTR_BEGIN+1)
#define IDS_SC_TUOFF                (IDS_SC_TUTR_BEGIN+2)
#define IDS_SC_TRSET                (IDS_SC_TUTR_BEGIN+4)
#define IDS_SC_TUTRCANCEL           (IDS_SC_TUTR_BEGIN+8)

#define IDS_SC_TUTR_SELECT          (IDS_SC_TUTR_BEGIN+0x10)
#define IDS_SC_TUTR_DESELECT        (IDS_SC_TUTR_BEGIN+0x40)



#define IDS_SC_TULOG                (IDS_SC_TUTR_BEGIN+300)
#define IDS_SC_TU_OBJ               (IDS_SC_TUTR_BEGIN+301)
#define IDS_SC_TU_GRPOBJ            (IDS_SC_TUTR_BEGIN+302)
#define IDS_SC_TR_GRPOBJ            (IDS_SC_TUTR_BEGIN+303)

#define IDS_SC_CHANNEL              (IDS_SC_TUTR_BEGIN+400)

#ifndef RC_INVOKED // Not a Resource Compiler

#include  <ft2usbio.h>
#include  <granit_proto.h>




#pragma pack(push,1)


#define REGDW_GRSCM_PUNUMBER_W       L"PuNumber"
#define REGSTR_GRSC_DEVICENAME_W     L"SCDeviceName"
#define REGSTR_GRSC_IOMODULE_W       L"SCIOModule"
#define REGSTR_GRSC_PUNAME_W         L"SCPUName"
#define REGSTR_GRSC_FLOOR            L"Floor_"
#define REGSTR_GRSC_FILENAME_W       L"SCDBFileName"
#define REGDW_GRSC_IOWATCHDOG        L"IoWatchdog"
#define REGDW_GRSC_IOSPEED           L"SCIOSpeed"

#define SC_FILENAME_TEMPL            L"SCData\\SCDB.scd"

#define CCFL_PUNUMBER               0x80000000
#define CCFL_DEVICE                 0x40000000
#define CCFL_DEVICE_NAME            0x00080000
#define CCFL_DEVICE_SPEED           0x00040000
#define CCFL_PUNAME                 0x20000000
#define CCFL_FLOOR0                 0x08000000
#define CCFL_FLOOR1                 0x04000000
#define CCFL_FLOOR2                 0x02000000
#define CCFL_FLOOR3                 0x01000000
#define CCFL_FLOORS                 0x0F000000
#define CCFL_FILENAME               0x00800000
#define CCFL_IOWATCHDOG             0x00400000


#define GRSC_TEXT_MAXSIZE      64
#define GRSCFLOOR_MAX_CPCOUNT  16         // На этаже не более 16 КП
#define GRCP_MAX_MODCOUNT      16         // На КП максимум    16 модулей


#define MNF_SCTXRX         0x00000100     // Маска напоминания Приёма передачи  СЦ
#define MNFSC              MNF_SCTXRX
#define MNCODE_SCTX          1
#define MNCODE_SCRX          2
#define MNCODE_SCTXRX_ERROR  3

#define MNF_SC             0x00000200   //Маска напоминания о событиях СЦ

typedef struct _GRSC_ACR  /*Структура уведомления об изменениях*/
{
  DWORD addr;
  DWORD parts;
  DWORD loN;
  DWORD hiN;
  DWORD what;
}GRSC_ACR,*LPGRSC_ACR;

#define MNCODE_SC_ADDED       1  //Добавление объекта
#define MNCODE_SC_CHANGED     2  //Изменение объекта
#define MNCODE_SC_REMOVED     3  //Удаление объекта
#define MNCODE_SC_DATACHANGED 4  /*Изменение данных объекта*/


#define GRSC_ACR_TUENTRY_ADDED    1
#define GRSC_ACR_TUENTRY_CHANGED  2
#define GRSC_ACR_TUENTRY_REMOVED  4



typedef union _grsc_objid
{
 struct{
         BYTE cp;
         BYTE grp;
         BYTE fa;
         BYTE obj;
       };
 DWORD  obj_id;
}grsc_objid,*lpgrsc_objid;


typedef struct _grsc_settings
{
 DWORD     dw_size;
 DWORD     modem_number;                  //Номер Модема
 DWORD     pu_number;                     //Номер ПУ
 wchar_t   io_module     [MAX_PATH];         //Модуль ввода вывода
 wchar_t   device_name   [MAX_PATH];         //Символьное имя устройства
 DWORD     io_speed;
 wchar_t   pu_name       [GRSC_TEXT_MAXSIZE];//Имя ПУ
 wchar_t   save_file_name[MAX_PATH];         //Имя файла для сохранения данных
 BOOL      enable_io_watchdog;
}grsc_settings,*lpgrsc_settings;



#define GRSC_FLAG_TEST                  0x80000000  //Флаг - признак тестового объекта
#define GRSC_FLAG_NOHANDLE              0x40000000  //Флаг - признак необрабатываемого объекта
#define GRSC_FLAG_CP_LOGHARDWARE_ERROR  0x00000001  //Флаг - Запись ошибок в лог
#define GRSC_FLAG_CP_NOTRECEIPT         0x00000002  //Флаг - запрета квитанции
#define GRSC_FLAG_CP_NUMBERED_TS        0x00000004


#define GRSC_FPRP_ADDR            0x00000001
#define GRSC_FPRP_FLAGS           0x00000002
#define GRSC_FPRP_NAME            0x00000004
#define GRSC_FPRP_COUNTS          0x00000008
#define GRSC_FPRP_TIMERS          0x00000010
#define GRSC_FPRP_MODULE          0x00000020
#define GRSC_FPRP_SWT             0x00000040
#define GRSC_FPRP_RESPOND_SMOOTH  0x00000080

#define GRSC_FPRP_TIMERS_TOALL    0x00001000



typedef  struct  _GR_FLOOR_PROPS
{
 DWORD    dw_size;
 DWORD    mask;
 otd_addr addr;
 DWORD    flags;
 DWORD    cp_count;
}GR_FLOOR_PROPS,*LPGR_FLOOR_PROPS;

#define GRPR_STATE_CALL_SENDED   0x00000001    // Отправлен вызов


typedef struct _GR_CP_PROPS
{
 DWORD       dw_size;
 DWORD       mask;
 otd_addr    addr;
 DWORD       flags;
 DWORD       grp_cnt;
 int         send_wait_time; // Время ожидания готовности от ЛУ до следующей передачи
 wchar_t  name[GRSC_TEXT_MAXSIZE];
}GR_CP_PROPS, *LPGR_CP_PROPS;

typedef struct _GR_GRP_PROPS
{
 DWORD    dw_size;
 DWORD    mask;
 otd_addr addr;
 DWORD    flags;
 DWORD    mod_number;
 DWORD    param_count;
 DWORD    data_size;
 DWORD    tutr_count;
 DWORD    work_timer   ;  //Таймер вызова работоспособного
 DWORD    nowork_timer ;  //Таймер вызова неработоспособного
 DWORD    respond_timer;  // Таймер  ответа
 DWORD    respond_smooth; // Счетчик не ответов на вызов при достжении которого
                          // объявляется неработоспособность группы
}GR_GRP_PROPS, *LPGR_GRP_PROPS;


/*Команда получения свойств*/
#define  SCCM_GET_PROPS    MCMD(MT_MODEM,1000)
#define  SCCM_SET_PROPS    MCMD(MT_MODEM,1001)
/*
 p1 =  DWORD otd_addr;    p2 = зависит от otd_addr_type()  LPGR_FLOOR_PROPS,LPGR_CP_PROPS или LPGR_GRP_PROPS;
*/

#define SCCM_CREATE_FLOOR   MCMD(MT_MODEM,1002)
#define SCCM_CREATE_CP      MCMD(MT_MODEM,1003)
#define SCCM_CREATE_GROUP   MCMD(MT_MODEM,1004)


#define SCCM_REMOVE         MCMD(MT_MODEM,1005)
// Удалить из базы

#define  SCCM_ENUM_CP       MCMD(MT_MODEM,1010)
#define  SCCM_ENUM_GRP      MCMD(MT_MODEM,1011)
/*Перечисление КП или групп на КП
 p1 = index; p2 =  LPGR_CP_PROPS или LPGR_GRP_PROPS
*/

#define SCCM_REFRESH        MCMD(MT_MODEM,1050)
//p1 = otd_addr

#define SCCM_GET_OTDDATA       MCMD(MT_MODEM,1100)
/*Получение данных по адресу*/
/*p1 = otd_addr  , p2 = LPSC_OTDDATA*/

typedef struct _SC_OTDDATA
{
  DWORD      dw_size;
  DWORD      parts;
  DWORD      lo_num;
  DWORD      hi_num;
  DWORD      data_size;
  DWORD      data_len;
  BYTE       data[ANYSIZE_ARRAY];
}SC_OTDDATA,*LPSC_OTDDATA;

#define SCCM_GET_DYNAMIC     MCMD(MT_MODEM,1110)
/*Получение динамических данных*/
/*p1 = otd_addr  , p2 = LPSC_DYNAMIC_XXX_*/

typedef struct _SC_DYNAMIC_CP
{
 DWORD   dw_size;
 DWORD   tx_deliver_time;
 DWORD   tx_byte_time;
 DWORD   rx_timer;
}SC_DYNAMIC_CP, *LPSC_DYNAMIC_CP;

typedef struct _SC_DYNAMIC_GRP
{
 DWORD   dw_size;
 BOOL    call_sended;
 DWORD   tm_counter;
 DWORD   respond_time;
 DWORD   nchd_counter;
 DWORD   chdbc_counter;
 DWORD   norespond_counter;
}SC_DYNAMIC_GRP,*LPSC_DYNAMIC_GRP;


#define SCCM_GET_TUENTRY    MCMD(MT_MODEM,1200)
//p1 =  LPGRSC_TU_ENTRY
#define SCCM_ADD_TUENTRY    MCMD(MT_MODEM,1201)
//p1 =  LPGRSC_TU_ENTRY
#define SCCM_SET_TUENTRY    MCMD(MT_MODEM,1202)
//p1 =  LPGRSC_TU_ENTRY
#define SCCM_DEL_TUENTRY    MCMD(MT_MODEM,1203)
//p1 =  LPGRSC_TU_ENTRY

#define SCCM_ENUM_TUENTRY   MCMD(MT_MODEM,1210)
//p1 = LPGRSC_TU_ENTRY,p2 = tu_index

#define GRSC_TUFL_NOCHECK            0x80000000  // Не проверять успешность ТУ
#define GRSC_TUFL_TSINVERSE          0x40000000  /* Признак инверсной ТС */
#define GRSC_TUFL_RAviaRC            0x20000000  /* ТР через ТУ */

#define GRSC_TUFL_DYN_CHECK          0x00080000
#define GRSC_TUFL_DYN_INVERSE        0x00040000


#define GRSC_TUFL_MASK               0xFFFF0000

#define GRSC_TUTR_STATE_SELECTED     0x00000001 //Объект ТУ/ТР выбран
#define GRSC_TUTR_STATE_ACTIVE       0x00000002 //Команда ТУ/ТР отправлена и активен таймер удержания контактов
#define GRSC_TUTR_STATE_HOLDING      0x00000004 //удержание контактов
#define GRSC_TUTR_STATE_WAITING      0x00000008 //Ожидание завершения

#define GRSC_TUTR_STATE_ACTIVE_MASK  (GRSC_TUTR_STATE_ACTIVE|GRSC_TUTR_STATE_HOLDING|GRSC_TUTR_STATE_WAITING)


#define GRSC_TUTR_STATE_QUEUED       0x00000010 //Команда поставлена в очередь
#define GRSC_TUTR_STATE_CMDON        0x00000020
#define GRSC_TUTR_STATE_CMDOFF       0x00000040
#define GRSC_TUTR_STATE_CMDCANCEL    0x00000080 //Команда отменяется


#define GRSC_TUTR_STATE_MASK     0x0000FFFF
  

typedef struct _GRSC_TU_ENTRY
{
 DWORD        dw_size;
 otd_addr     addr;             /* АДРЕС объекта    */
 DWORD        object;           /* Номер объекта    */
 DWORD        tu_module;        /* Номер модуля ТУ/ТР*/
 DWORD        tu_group;         /* Группа  ТУ/Р */
 DWORD        tu_object;        /* Объект  ТУ/Р */
 DWORD        cmd_on;           /* Команда  включения/>*/
 DWORD        cmd_off;          /* Команда отключения/<*/
 DWORD        tu_flags;         /* Флаги ТУ/Р */

 DWORD        tu_hold_time;     /* Время удержания контактов при ТУ*/
 DWORD        tu_timeout;       /* Время таймаута успешного ТУ, автоотмены для ТР*/
 DWORD        tu_select_timeout;/* Таймер автосброса выбора*/

 DWORD        tu_state;         /* Состояние команды*/
 __int64      tu_time;          /* Время отправки КОМАНДЫ ТУ/ТР*/
 
 DWORD        owner_addr;       /* Адрес модема владельца*/
 DWORD        curr_value;       /* Текущее значение */
 DWORD        end_value;        /* Конечное значение для ТР*/

}GRSC_TU_ENTRY,*LPGRSC_TU_ENTRY;


#define SCCM_TEST_TUTR    MCMD(MT_MODEM,2000)

typedef struct _SC_TEST_TUTR
{
 DWORD       dw_size;
 otd_addr    addr;
 DWORD       object;
 DWORD       tu_command;
}SC_TEST_TUTR,*LPSC_TEST_TUTR;

//otd_proto.h

//Сброс ошибки ТУ/ТР по объекту
#define SCCM_RESET_TU_ERROR MCMD(MT_MODEM,2001)
// p1 = otd_addr,p2 = obj;

#define SCCM_IMPORT_SCDB  MCMD(MT_MODEM,3000)
//p1 = wchar_t * file_name; 





#pragma pack(pop)

#endif // Not a Resource Compiler


#endif
