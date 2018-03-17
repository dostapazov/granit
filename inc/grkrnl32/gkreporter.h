#ifndef GKREPORTER_HPP
#define GKREPORTER_HPP

#include <gkmodules.h>


//тип процедуры для представления доп. данных записи лога в виде строки
//Data - сначала DWORD (размер данных), далее сами данные
//str - буфер для преобразованных в текст данных
//sz - размер буфера
//return - длина строки+1 (если размер буфера мал - значение отрицательное
typedef int (WINAPI * ReportDataToStrA)(LPVOID Data, char *str, DWORD sz, int Code);
typedef int (WINAPI * ReportDataToStrW)(LPVOID Data, wchar_t *str, DWORD sz, int Code);

#pragma pack(push,1)

/* Имя хендла REPORTER'a */
#define REPORTER_NAME_W L"STD_GKREPORTER"
#define REPORTER_NAME_A  "STD_GKREPORTER"

/* Имя хендла внешнего репортёра */
#define REPORTEREX_NAME_W L"EXT_GKREPORTER"
#define REPORTEREX_NAME_A  "EXT_GKREPORTER"

//Простое сообщение
//p1 = (char*) str1
//p2 = (char*) str2
#define  RCM_MESSAGE_A     MCMD(MT_REPORTER,1)
//Простое сообщение UNICODE
//p1 = (wchar_t*) str1
//p2 = (wchar_t*) str2
#define  RCM_MESSAGE_W      MCMD(MT_REPORTER,2)

//Исключение
//p1 LPEXCEPTION_POINTERS,
//p2 = char * text
#define  RCM_EXCEPTION      MCMD(MT_REPORTER,3)

//p1 = lpexcept_record,
//p2 = char * text
#define  RCM_GKH_EXCEPTION  MCMD(MT_REPORTER,4)

/* Регистрация типа сообщения */
// p1 = char * msg_type_name (краткое название - имя файла)
// p2 = char * полное название
//ret - (UINT) номер события
#define RCM_REGISTER_MSG_TYPE_A MCMD(MT_REPORTER,5)

/* Регистрация типа сообщения */
// p1 = wchar_t * msg_type_name (краткое название - имя файла)
// p2 = wchar_t * полное название
//ret - (UINT) номер события
#define RCM_REGISTER_MSG_TYPE_W MCMD(MT_REPORTER,6)

//Установить адрес функции обработки доп. данных события лога
//p1 = UINT - зарегестрированный тип сообщений
//p2 = адрес функции
#define RCM_SET_REGISTER_FUNC   MCMD(MT_REPORTER,7)

//Получить адрес функции обработки доп. данных события лога по номеру или имени
//p1 = UINT - зарегестрированный тип сообщений
//p2 = char * - если p1==-1, msg_type_name (краткое название - имя файла)
//ret = адрес функции
#define RCM_GET_REGISTER_FUNC   MCMD(MT_REPORTER,8)

//Получить имя сообщения по номеру
//p1 = UIN - номер сообщения
//p2 = TEventNameA *
#define RCM_GET_MSG_NAME_A      MCMD(MT_REPORTER,9)

//Получить имя сообщения по номеру
//p1 = UIN - номер сообщения
//p2 = TEventNameW *
#define RCM_GET_MSG_NAME_W      MCMD(MT_REPORTER,10)

//Очистить файл лога
//p1 = UINT - зарегестрированный тип сообщений
#define RCM_CLEAR_EVENT         MCMD(MT_REPORTER,11)

//Сообщение через структуру
//p1 = TReportMes *
//p2 = 0
#define  RCM_MESSAGE_EVENT      MCMD(MT_REPORTER,12)

//Типы событий
#define REPORT_SUCCESS_TYPE     0       //успех
#define REPORT_INFORMATION_TYPE 1       //уведомление
#define REPORT_WARNING_TYPE     2       //предупреждение
#define REPORT_ERROR_TYPE       4       //ошибка

//Звуковые сообщения
//p1 = bool       0- сброс, 1- установить
//p2 = bool       0- прочитать, 1- изменить
#define  RCM_MESSAGE_SOUND      MCMD(MT_REPORTER,13)

//Задать максимальный размер лог-файла
//p1 = UINT   - зарегестрированный тип сообщений
//p2 = DWORD  - размер файла, 0 - безразмерный
#define  RCM_MESSAGE_FILESIZE   MCMD(MT_REPORTER,14)

struct TReportMes
{
  UINT log;                     //имя файла лога
  const wchar_t *desc;          //описание события
  DWORD type;                   //тип события
  GKHANDLE source;              //источник события
  DWORD data_size;              //размер доп. данных
  DWORD data[ANYSIZE_ARRAY];    //доп. данные
  #ifdef __cplusplus
  TReportMes(){ZeroMemory(this,sizeof(*this));}
  #endif
};

struct TEventNameA
{
  char *s_short, *s_full;
  int sz_short, sz_full;        //размер строк, если мал возвращается отрицательное число
};

struct TEventNameW
{
  wchar_t *s_short, *s_full;
  int sz_short, sz_full;        //размер строк, если мал возвращается отрицательное число
};

//Стандартные логи событий
#define EVENT_SYSTEM          0 //"System" - системные события (запуск, останов модуля и т.п.)
#define EVENT_EXCEPTIONS      1 //"Exceptions" - для исключений

#pragma pack(pop)

#endif


