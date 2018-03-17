#ifndef _GRNET_DEF_INC
#define _GRNET_DEF_INC

#include <ke_defs.h>
#include <granit_dos.h>

#define GRNET_NAMES_LEN      16
#define DEF_POLINOM_DEL     64709

#ifndef GRNET_DATA_LEN
 #define GRNET_DATA_LEN 256
#endif


const long NetSecret = 0x5850494EuL;    //NIPX


 const long
          CmConnect          =  1,
          CmDisconnect       =  2,
          CmData             =  3,
          CmRetranslation    =  5, //Передача телемеханич .данных (TKadr)
          CmBeginTU          =  6, //Начало Управления
          CmResetTU          =  7, //Отмена Управления
          CmQueryTU          =  8, //Запрос клиента на начало управления
          CmTuFalse          =  9, //Неуспешное ТУ
          CmQuery            =  10, //Запрос данных по всей базе
          CmTuOn             =  11, // ТУ - Включить
          CmTuOff            =  12,
          CmTrNorm           =  13,
          CmTuCancel         =  14,
          CmEnableTU         =  15,
          CmDisableTU        =  16,
          CmErrorTU          =  17,
          CmActivateTR       =  18,
          CmQueryCp          =  20, //Вызов по КП
	  CmQueryFa          =  21, //Вызов по фа
	  CmQueryGroup       =  22, //Вызов по группе

          CmIdle             = 0x100,

          CmTxFile           = 0x200,//Начало передачи файла
          CmTxFileChunk      = 0x201,//Очередной кусок
          CmTxFileDone       = 0x202,//Завершение файла
          CmRxFileDone       = 0x203,//Завершение файла
          CmCantTxFile       = 0x204,//Не возмодно передать файл
          CmErrorTxFile      = 0x205,//Ошибка при передаче файла
          CmResumeTxFile     = 0x206,//Восстановление передачи

          CmGetAllRetro      = 0x400,//Запрос на передачу ретро
          CmGetAllRetroOK    = 0x401,//Передача ретро началась
          CmGetAllRetroNone  = 0x402,//Ретро не ведется
          CmGetAllRetroBad   = 0x403,//Невозможно в данный момент передать ре


          CmModemOff         = 45000, //Модем потерял связь


          CmInformation      = 0xFFF0,
          CmError            = 0xFFFA,
          CmServerNotFound   = 0xFFFB,
          CmTestString       = 0xFFFC,

          CmCheckConnect     = 0xFFFD,
          CmOutputQueueEmpty = 0xFFFE //Очередь на передачу пуста
          ;


#define    CONNECT_NO_ERROR      0
#define    CONNECT_SPACE_ERROR   1  //Нет места в списке
#define    CONNECT_LEVEL_ERROR   2  //Такой уровень клиентов не поддерживается
#define    CONNECT_DENY_ERROR    3  //Сервер отказал в соединении

#define    GRNET_FLAG_RECEIPT     0x00000001
#define    GRNET_FLAG_NEEDRECEIPT 0x00010000

#pragma pack (push,1)

typedef struct _GRNET_KADR
{
  DWORD  secret_dword;
  WORD   receipt;
  WORD   need_receipt;
  DWORD  command;
  WORD   id;
  WORD   kadr_num;
  WORD   retry_num;
  WORD   data_len;
  DWORD  check_sum;
  BYTE   data[ANYSIZE_ARRAY];
}GRNET_KADR,*LPGRNET_KADR;

typedef struct _GRNET_BEGIN_CONNECTION
{
 char server_name[GRNET_NAMES_LEN]; // ..
 char client_name[GRNET_NAMES_LEN]; // ..
 WORD client_level;                 // .. устанавливается клиентом
 /*************************/
 WORD ID;                           // ..
 WORD error_connection;             // ..Устанавливается сервером
}GRNET_BEGIN_CONNECTION ,*LPGRNET_BEGIN_CONNECTION;


#pragma pack (pop)

void   __fastcall grnet_protect  (LPGRNET_KADR kadr);
BOOL   __fastcall grnet_check    (LPGRNET_KADR kadr);
BOOL   __fastcall grnet_init_kadr(LPGRNET_KADR kadr,DWORD cmd,WORD kadr_num,WORD id,BOOL need_receipt,LPVOID data,DWORD data_len);
inline
DWORD  __fastcall grnet_kadr_len(LPGRNET_KADR kadr)
{ return kadr->data_len+sizeof(GRNET_KADR)-sizeof(kadr->data);}


#endif

 