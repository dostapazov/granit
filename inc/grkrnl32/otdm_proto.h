/*

 Определения протокола
 БД Среднего уровня

*/

#ifndef _OTD_MEDIUM_PROTO_INC
#define _OTD_MEDIUM_PROTO_INC

#ifdef __linux__
#include <gklib/otd_proto_medium.h>
#else
#include <otd_proto_medium.h>
#endif

#pragma pack(push,1)

/*
 Младшее слово - команда

 Старшее слово информационные биты

*/

#define OTDM_RESPOND   0x80000000
#define OTDM_BEGINDATA 0x40000000
#define OTDM_ENDDATA   0x20000000
#define OTDM_ERROR     0x10000000
#define OTDM_SUBSCRIBE 0x08000000
#define OTDM_ORDER     OTDM_SUBSCRIBE


#define OTDM_COMMAND_MASK  0x0000FFFF

struct OTDM_PROTO_HEADER
{
 DWORD  command;
 DWORD  error;
 DWORD  data_size;
 DWORD  data[ANYSIZE_ARRAY];
};

//#define LPOTDM_PROTO_HEADER OTDM_PROTO_HEADER*
typedef OTDM_PROTO_HEADER* LPOTDM_PROTO_HEADER;

#define OTDM_PROTO_GET_SIZE(mph)  mph->data_size+sizeof(mph)-sizeof(mph->data)
#define OTDM_PROTO_PROTECT(mph)   *((LPBYTE)mph)+OTDM_PROTO_GET_SIZE(mph) = otd_calc_crc()
#define OTMD_PROTO_ISBEGIN(mph)   ((mph->command&OTDM_BEGINDATA) /*|| 0==(mph->command&(OTDM_BEGINDATA|OTDM_ENDDATA))*/)
#define OTMD_PROTO_ISEND(mph)     ((mph->command&OTDM_ENDDATA)   /*|| 0==(mph->command&(OTDM_BEGINDATA|OTDM_ENDDATA))*/)
#define OTDM_PROTO_IS_ERROR(mph)  ((mph->command&OTDM_ERROR))

#define OTDMPROTO_CMD_ENUM_KADRS        1
/*Команда перечисления кадров*/
/*
 Вызывает обработку OTDMPROTO_CMD_GETKADR для каждого кадра
*/


#define OTDMPROTO_CMD_GETKADR           2
/*Команда получения описателя кадра*/
/*
  Приём OTDM_PROTO_HEADER::data[0] = kadr_id ,OTDM_PROTO_HEADER::data[1] = mask
  Отправляет описатель кадра +  короткое имя + длинное имена
*/

struct OTDM_PROTO_KADR
{
 DWORD     mask; // Маска изменений или валидности полей
 MDB_KADR  kadr;
 wchar_t   names[ANYSIZE_ARRAY];
};

//#define LPOTDM_PROTO_KADR OTDM_PROTO_KADR*
typedef   OTDM_PROTO_KADR* LPOTDM_PROTO_KADR;

#define OTDMPROTO_CMD_GETKADR_IMAGE       3
/*
  Отправка изображения кадра
  Приём OTDM_PROTO_HEADER::data[0] = kadr_id

  Отправка
  1 кадр  - размер и имя файла изображения
  отальные кадры кусочки изображения по 4k
  последний с признаком завершения
*/

struct OTDM_PROTO_IMAGE_BEGIN
{
 DWORD    kadr_id;
 DWORD    image_id;
 DWORD    image_size;
 __int64  image_time;
 wchar_t  image_name[ANYSIZE_ARRAY];
};

//#define LPOTDM_PROTO_IMAGE_BEGIN OTDM_PROTO_IMAGE_BEGIN*
typedef OTDM_PROTO_IMAGE_BEGIN*  LPOTDM_PROTO_IMAGE_BEGIN;


#define OTDM_DEF_CHUNK_SIZE (4096 - sizeof(OTDM_PROTO_IMAGE_CHUNK)+sizeof(BYTE)-sizeof(OTDM_PROTO_IMAGE_CHUNK)+sizeof(BYTE))

struct OTDM_PROTO_IMAGE_CHUNK
{
 DWORD kadr_id;
 DWORD image_id;
 WORD  chunk_size;
 BYTE  chunk_data[ANYSIZE_ARRAY];
};

//#define LPOTDM_PROTO_IMAGE_CHUNK OTDM_PROTO_IMAGE_CHUNK*
typedef OTDM_PROTO_IMAGE_CHUNK* LPOTDM_PROTO_IMAGE_CHUNK;


#define OTDMPROTO_CMD_GETKADR_ENTRYES     4
/*
 Команда получения состава кадра
*/
struct OTDM_PROTO_KADR_ENTRYES
{
 DWORD kadr_id;
 DWORD entryes_count;
 MDB_KADR_ENTRY      entryes[ANYSIZE_ARRAY];
};

//#define LPOTDM_PROTO_KADR_ENTRYES OTDM_PROTO_KADR_ENTRYES*
typedef OTDM_PROTO_KADR_ENTRYES* LPOTDM_PROTO_KADR_ENTRYES;

#define OTDMPROTO_CMD_GETKADR_RECORDS      5
/* Команда получения данных кадра */
struct OTDM_PROTO_KADR_RECORDS
{
 DWORD      kadr_id;
 DWORD      rec_count;
 MDB_RECORD records[ANYSIZE_ARRAY];
};

//#define LPOTDM_PROTO_KADR_RECORDS OTDM_PROTO_KADR_RECORDS*
typedef OTDM_PROTO_KADR_RECORDS* LPOTDM_PROTO_KADR_RECORDS;

#define OTDMPROTO_CMD_RECORDS              6
/*
 Передача изменившихся записей по заказанным кадрам
*/
struct OTDM_PROTO_RECORD
{
 DWORD mask;
 DWORD rec_data[ANYSIZE_ARRAY];
};

//#define LPOTDM_PROTO_RECORD OTDM_PROTO_RECORD*
typedef OTDM_PROTO_RECORD* LPOTDM_PROTO_RECORD;

struct OTDM_PROTO_RECORDS
{
  DWORD rec_count;
  OTDM_PROTO_RECORD  records[ANYSIZE_ARRAY];
};

//#define LPOTDM_PROTO_RECORDS OTDM_PROTO_RECORDS*
typedef OTDM_PROTO_RECORDS* LPOTDM_PROTO_RECORDS;

#define OTDMPROTO_CMD_GET_RECORD_SNAME              7
#define OTDMPROTO_CMD_GET_RECORD_NAME               8
#define OTDMPROTO_CMD_GET_RECORD_FULL_SNAME         9
#define OTDMPROTO_CMD_GET_RECORD_FULL_NAME          10

/*Получение короткого или длинного имени записи*/
//OTDM_PROTO_HEADER::data[0] = record_id;

struct OTDM_PROTO_RECORD_NAME
{
 DWORD rec_id;
 DWORD name_id;
 DWORD len;
 wchar_t    text[ANYSIZE_ARRAY];
};

//#define LPOTDM_PROTO_RECORD_NAME OTDM_PROTO_RECORD_NAME*
typedef OTDM_PROTO_RECORD_NAME* LPOTDM_PROTO_RECORD_NAME;


#define OTDMPROTO_CMD_TUOPERATION     100

struct OTDM_PROTO_TUOPERATION
{
 DWORD rec_id   ; // ИД записи которой отправляется операция ТУ
 DWORD op_code  ; // Код операции cм otd_types.h
 float new_value; // Новое значение
};


//#define LPOTDM_PROTO_TUOPERATION OTDM_PROTO_TUOPERATION*
typedef OTDM_PROTO_TUOPERATION* LPOTDM_PROTO_TUOPERATION;


#define OTDMPROTO_CMD_WORK_INOUT        101
struct  OTDM_PROTO_WORK_INOUT
{
  DWORD rec_id;
  DWORD worked;
};

#define OTDMPROTO_CMD_CLEAR_TU_ERROR    102

typedef OTDM_PROTO_WORK_INOUT* LPOTDM_PROTO_WORK_INOUT;

#define  OTDMPROTO_CMD_START_OSCILLOGRAMM 200
struct OTDM_PROTO_OSCPARAM
{
 DWORD rec_id      ; // ИД записи
 DWORD osc_lenght  ; //Длительность осцилограммы в миллисекундах
};

typedef OTDM_PROTO_OSCPARAM* LPOTDM_PROTO_OSCPARAM;


// Команда уведомление о смене БД среднего уровня
// Клиенты должны очистить список имен, кадров, записей и переподписаться
#define OTDMPROTO_CMD_NOTIFY_DBCHANGE      300


/*
 Возвращаемые ошибки
*/
#define OTDMPROTO_NO_ERROR             0
#define OTDMPROTO_ERR_NO_MEMORY        1  // Недостаточно памяти для выполнения запроса
#define OTDMPROTO_ERR_NO_KADR          2  // Нет кадра с таким ID
#define OTDMPROTO_ERR_NO_IMAGE         3  // У кадра отсутсвует изображение
#define OTDMPROTO_ERR_ERROR_OPEN_FILE  4  // Ошибка открытия файла
#define OTDMPROTO_ERR_NORECORD         5  // Нет такой записи


//Протокол архивов FA_OTD_MEDIUM_ALARM_ARCHIVE

//Передаётся внутри OTDM_PROTO_HEADER

//Заголовок протокола историй
struct OTDM_ARCHIVE_HEADER
{
  QWORD hist_id;
  DWORD data[ANYSIZE_ARRAY];
};

typedef OTDM_ARCHIVE_HEADER* LPOTDM_ARCHIVE_HEADER;

#define OTDM_ARCHIVE_CHTIME_NULL 0x01
#define OTDM_ARCHIVE_RECID_NULL  0x02
#define OTDM_ARCHIVE_RVALUE_NULL 0x04
#define OTDM_ARCHIVE_KVANTS_NULL 0x08
#define OTDM_ARCHIVE_DIAG_NULL   0x10
#define OTDM_ARCHIVE_STATE_NULL  0x20

struct  OTDM_ARCHIVE_REC
{
  WORD  NULL_MASK;
  QWORD ch_time;
  DWORD rec_id;
  float rvalue;
  int   kvants;
  DWORD otdpd;
  WORD  diag;
  WORD  state;
  WORD  alarm;
  QWORD wr_time;
};

typedef OTDM_ARCHIVE_REC* LPOTDM_ARCHIVE_REC;


struct  OTDM_ARCHIVE_RECORDS_RESPOND
{
  DWORD count;
  OTDM_ARCHIVE_REC rec[ANYSIZE_ARRAY];
};

typedef OTDM_ARCHIVE_RECORDS_RESPOND* LPOTDM_ARCHIVE_RECORDS_RESPOND;

struct OTDM_ARCHIVE_REC_IDS
{
  DWORD count;
  DWORD rec_id[ANYSIZE_ARRAY];
};

typedef OTDM_ARCHIVE_REC_IDS* LPOTDM_ARCHIVE_REC_IDS;

struct OTDM_ARCHIVE_PERIOD
{
  QWORD  beg_time;
  QWORD  end_time;
};

typedef OTDM_ARCHIVE_PERIOD* LPOTDM_ARCHIVE_PERIOD;

struct OTDM_ARCHIVE_QUERY
{
  DWORD                alarm;
  OTDM_ARCHIVE_PERIOD  period;
  OTDM_ARCHIVE_REC_IDS rec_ids;
};

typedef OTDM_ARCHIVE_QUERY* LPOTDM_ARCHIVE_QUERY;

#define OTDMPROTO_CMD_QUERY_ARCHIVE  0x2000 //Запрос архивных данных
//запрос OTDM_PROTO_HEADER   OTDM_ARCHIVE_HEADER OTDM_ARCHIVE_QUERY
//Ответ  OTDM_PROTO_HEADER   OTDM_ARCHIVE_HEADER OTDM_ARCHIVE_RESPOND

#define OTDM_KADR_ARCHIVE_NAME_LEN 32
struct OTDM_KADR_ARCHIVE
{
 DWORD kadr_id;
 DWORD alarms;
 QWORD beg_time;
 QWORD end_time;
 BYTE  protect;
 char  arch_name[OTDM_KADR_ARCHIVE_NAME_LEN];
};
typedef OTDM_KADR_ARCHIVE* LPOTDM_KADR_ARCHIVE;

struct  OTDM_KADR_ARCHIVE_RESPOND
{
  DWORD count;
  OTDM_KADR_ARCHIVE  krec[ANYSIZE_ARRAY];
};

typedef OTDM_KADR_ARCHIVE_RESPOND* LPOTDM_KADR_ARCHIVE_RESPOND;


#define OTDMPROTO_CMD_QUERY_KADR_ARCHIVES 0x2001
#define OTDMPROTO_CMD_ARCHIVE_SET_PROPS   0x2002

#define OTDMPROTO_CMD_ARCHIVE_PRESENT     0x2010 //Запрос наличия модуля истории
//запрос OTDM_PROTO_HEADER   OTDM_ARCHIVE_HEADER
//ответ  OTDM_PROTO_HEADER   OTDM_ARCHIVE_HEADER




/*
Передача      вариантов отображения
начинается с  передачи изображений
изображения   передаются так-же как и изображения кадра
kadr_id - номер строки с именем изображения
*/
/*Запрос вариантов отрисовки*/
#define OTDMPROTO_CMD_GET_PAINT_VARIANTS  0x3000
struct OTDM_PAINT_VARIANTS
{
  DWORD              count;
  MDB_PAINT_VARIANT  pvars[ANYSIZE_ARRAY];
};

typedef OTDM_PAINT_VARIANTS * LPOTDM_PAINT_VARIANTS;

#define OTDMPROTO_CMD_PAINT_IMAGE  0x3003

typedef OTDM_PROTO_IMAGE_BEGIN*  LPOTDM_PROTO_PAINT_IMAGE_BEGIN;
typedef OTDM_PROTO_IMAGE_CHUNK*  LPOTDM_PROTO_PAINT_IMAGE_CHUNK;



#pragma pack(pop)

  #define OTDM_SET_PROTO_STATE(ps,omph) \
    ps->command    = (omph->command & OTDM_COMMAND_MASK); \
     ps->is_respond = (omph->command & OTDM_RESPOND) ? true:false; \
     ps->beg_data   = (omph->command&OTDM_BEGINDATA) ? true:false; \
     ps->end_data   = (omph->command&OTDM_ENDDATA)   ? true:false; \
     ps->is_error   = (omph->error || OTDM_PROTO_IS_ERROR(omph)) ? true : false; \
     ps->error      =  omph->error; \
     ps->data_size  = omph->data_size; \
     ps->p_data     = omph->data;


  struct OTDM_PROTO_STATE
  {
      DWORD command  ;
      bool  is_respond;
      bool  beg_data ;
      bool  end_data ;
      bool  is_error ;
      DWORD error;
      DWORD data_size;
      union{
            LPDWORD                    p_data;
            LPOTDM_PROTO_KADR          p_kadr;
            LPOTDM_PROTO_IMAGE_BEGIN   p_image_begin;
            LPOTDM_PROTO_IMAGE_CHUNK   p_image_chunk;
            LPOTDM_PROTO_KADR_ENTRYES  p_entryes;
            LPOTDM_PROTO_KADR_RECORDS  p_kadr_recs;
            LPOTDM_PROTO_RECORDS       p_records;
            LPOTDM_PROTO_RECORD_NAME   p_name;
            LPOTDM_PROTO_TUOPERATION   p_rc_op;
            LPOTDM_PROTO_WORK_INOUT    p_work_inout;
            LPOTDM_PROTO_OSCPARAM      p_oscparam;
            LPOTDM_ARCHIVE_HEADER      p_arch_hdr;
            LPOTDM_PAINT_VARIANTS      p_paint_vars;
            LPOTDM_ARCHIVE_HEADER      p_archive_hdr;
            LPOTDM_PROTO_TUOPERATION   p_tuop;
           };
    #ifdef __cplusplus
      OTDM_PROTO_STATE (LPOTDM_PROTO_HEADER omph)
      {
       OTDM_SET_PROTO_STATE(this,omph);
      }
   #endif
  };

  typedef OTDM_PROTO_STATE* LPOTDM_PROTO_STATE;


#ifdef __cplusplus
extern "C" {
#endif

DWORD WINAPI otdm_pack_record  (LPVOID buff,DWORD bsz,DWORD mask, LPMDB_RECORD  rec);
DWORD WINAPI otdm_unpack_record(LPVOID buff,DWORD bsz,DWORD mask, LPMDB_RECORD  rec);
DWORD WINAPI otdm_pack_kadr    (LPVOID data,DWORD data_size,DWORD mask,LPMDB_KADR kadr );
DWORD WINAPI otdm_unpack_kadr  (LPVOID data,DWORD data_size,DWORD mask,LPMDB_KADR kadr );

#ifdef __cplusplus
} /*extern "C" {*/
#endif

#endif
