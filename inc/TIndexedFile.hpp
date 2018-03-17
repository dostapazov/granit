//---------------------------------------------------------------------------

#ifndef TIndexedFileH
#define TIndexedFileH

#include <KeRTLio.hpp>
#include <map>
//---------------------------------------------------------------------------
using namespace KeRTL;
using namespace std;

// файл с произвольным доступом к записям произвольной длины
// параметр uids при создании указывает на использование уникального ключа и индекса по нему
// соответственно 2 способа доступа:
// 1 - несортированный по абсолютному номеру записи
// 2 - отсортированный по порядковому номеру записи или по id
// Структура файла:
//   общий заголовок - версия, число записей, неиспользуемое место, размер заголовка секции
//   заголовок секции - элемент=сдвиг до записи от начала файла,
//                      последний элемент указывает на следующий заголовок,
//                      0 указывает на отсутствие записи (ещё не размещена или удалена)
//     записи начинаются с размера, далее уникальный id (если используется)
// При сохранении текущий индекс дописывается в конец файла

#pragma warn -8098
const DWORD IndFileSignature='FDNI';  //INDexed File
#pragma warn .8098
const DWORD IndFileVersion=1;
const DWORD IndFileHeaderSize=1024;

//Возвращаемые ошибки
#define INDF_ERR_SUCCESS        DWORD(0)        // без ошибок
#define INDF_ERR_ERROR          DWORD(-1)       // общая ошибка
#define INDF_ERR_BUFSIZE        DWORD(-2)       // буфер недостаточного размера

//Заголовок файла
typedef struct _TIndFileHdr
{
  DWORD size;           //размер заголовка
  DWORD sig;            //кодовое слово
  union
  {
    DWORD version;      //версия
    struct
    {
      WORD ext_ver;     //версия данных в файле
      WORD int_ver;     //версия самого файла
    };
  };
  DWORD num_rec;        //кол-во записей
  DWORD unused_size;    //суммарный размер неиспользуемого места
  DWORD uid;            //последний неиспользованный уникальный id
  DWORD index_save;     //сдвиг до сохранённого индекса
  bool  with_index;     //используется индекс по уникальному ключу
  bool  closed;         //если false, надо пересчитать записи и неиспользуемое место
} TIndFileHdr;

// Индекс по id
typedef map<DWORD,DWORD,less<DWORD> >                   TIndex;
typedef map<DWORD,DWORD,less<DWORD> >::value_type       TInd_value;
typedef map<DWORD,DWORD,less<DWORD> >::iterator         TInd_iter;

KERTL_CLASS TIndexedFile
{
private:
  TFile file;
  DWORD hdr_sz;         //размер заголовка секции в DWORD'ах
  DWORD *sec_hdr;       //блок для размещения заголовка секции
  DWORD hdr_pos;        //начало текущего заголовка
  DWORD num_rec;
  DWORD unused_size;
  bool indexed;
  bool read_only;
  DWORD uid;
  TIndex id_keys;
  bool opened;
  wchar_t *f_name;
  void __fastcall ReCount();
  bool __fastcall Init();
  int __fastcall CheckVersion(DWORD ver);
  DWORD __fastcall GetPos(DWORD ind);
  void __fastcall IncUID(DWORD num);
public:
  TIndexedFile(bool uids = false);
  TIndexedFile(wchar_t * file_name, bool uids = false, bool ro = false);
  TIndexedFile(char    * file_name, bool uids = false, bool ro = false);
  ~TIndexedFile();
  bool __fastcall open  ();             //открыть последний закрытый файл
  bool __fastcall open  (wchar_t * file_name, bool uids = false, bool ro = false);
  bool __fastcall open  ( char   * file_name, bool uids = false, bool ro = false);
  bool __fastcall create(wchar_t * file_name, bool uids = false);
  bool __fastcall create( char   * file_name, bool uids = false);
  void __fastcall close();
  void __fastcall flush();
  DWORD __fastcall write(void * data, DWORD size);                               //возвр. номер записи (-1 - ошибка)
  DWORD __fastcall write(DWORD num, void * data, DWORD size, bool by_index=true);//старый стираем, новый получаем
  DWORD __fastcall read(DWORD num, void * buffer, DWORD sz, bool by_index=true); //возвр. 0 - удалено, -1 - ошибка, -2 - не хватило места, остальное - размер данных
  DWORD __fastcall erase(DWORD num, bool by_index=true);
  DWORD __fastcall clear();
  DWORD __fastcall get_count();
  DWORD __fastcall get_size();
  DWORD __fastcall get_unused_size();
  bool __fastcall shrink();              //сжать
  bool __fastcall index_used() {return indexed;};
  bool __fastcall file_opened() {return opened;};
  int __fastcall get_file_name(wchar_t *file_name, int sz);
  int __fastcall get_file_name( char   *file_name, int sz);
  DWORD __fastcall get_ext_version();
  bool __fastcall set_ext_version(DWORD ver);
};

#endif