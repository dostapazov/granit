//---------------------------------------------------------------------------

#ifndef GKReporterExH
#define GKReporterExH

#define MNF_REPORTEREX          0x40000000      //маска напоминания окна репортера
#define MNCODE_REPORT_ADDED     0               //добавлено сообщение
#define MNCODE_REPORT_REFRESH   1               //перечитать файл лога
#define MNCODE_REPORT_NEWEVENT  2               //зарегистрировано новое событие


#pragma warn -8098
const DWORD Signature='FLKG';  //Granit Kernel Log File
#pragma warn .8098
const DWORD LogVersion=1;

//Заголовок файла лога
typedef struct _TLogHeader
{
  DWORD size;           //размер заголовка
  DWORD sig;            //кодовое слово
  DWORD version;        //версия
  DWORD num_rec;        //кол-во записей
  DWORD file_size;      //размер файла при ограничении размера, если 0 - без ограничения
  DWORD first_rec;      //начало первой записи
  DWORD last_rec;       //начало последней записи
} TLogHeader, *lplog_header;

//Далее файл содержит записи переменной длины вида:
//struct _TMess
//{
//  DWORD length;
//  ...
//  DWORD length;
//}
//для двунаправленного последовательного сканирования

//Постоянной длинны заголовок записи лога
typedef struct _TMess
{
  DWORD    length;         //размер структуры
  __int64  time;           //время возникновения события
  DWORD    type;           //тип события
  DWORD    num_strings;    //количество строк в переменной части
  DWORD    strings_offset; //смещение строк
  DWORD    data_length;    //размер доп. данных
  DWORD    data_offset;    //смещение доп. данных
  //
  //Затем следуют:
  //
  //wchar_t strings[]   //источник, описание события и др.
  //LPVOID data[];      //доп. данные
  //char pud[];         //дополнение до границы DWORD
  //DWORD length;
} TMess;

//для записи/чтения лога exception
typedef struct _exception_pointers
{
 EXCEPTION_RECORD except_rec;
 CONTEXT          except_ctx;
 DWORD            esp_dump[16];
 DWORD            ebp_dump[16];
}exception_pointers, *lpexception_pointers;


#endif
