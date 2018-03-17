#ifndef _UNIOPC_INC
#define _UNIOPC_INC

#include <opc_codes.h>
#include <kertl.hpp>
#include <kethreads.hpp>

using namespace KeRTL;
using namespace std;

#pragma pack(push,1)

enum{QualityUsed=0x80000000, QualityGood=0x800000c0, QualityBad=0x80000000};

struct TTagName
{
  char *name; // собственно наименование
  TTagName(){name=NULL;};
 ~TTagName() {free_name();};
  void setName(  char *aName );
  void __fastcall free_name(){if(name) delete [] name;}
};

inline   void TTagName::setName( char *aName )
{
     free_name();
     name = newstr((char*)aName);
};

template <typename T>

struct  opc_tag:public TTagName
{
 T         value;
 int       quality;
 FILETIME  time;
 opc_tag(char * aName = NULL):value(T()),quality(QualityBad)
 {time.dwLowDateTime = time.dwHighDateTime = 0 ;setName(aName);}
 opc_tag & operator = (opc_tag & tag)
 { free_name(); setName(tag.name);value = tag.value;quality = tag.quality;time = tag.time; return * this;}
};

typedef  opc_tag<BYTE>  TBitTag;
typedef  opc_tag<int>   TIntTag;
typedef  opc_tag<float> TAnTag;


struct TDataForOPCServer
{

  static   CRITICAL_SECTION  *  _locker;
  unsigned char dataIsValid;   /* признак того, что данные корректны
                                  (иначе BAD для всех тегов устройства)*/
  unsigned short anTagsCount;  // количество аналоговых тегов
  TAnTag * anTagsArray;   // указатель на массив аналоговых тегов
  unsigned short bitTagsCount; // количество битовых тегов
  TBitTag *bitTagsArray; // указатель на массив битовых тегов
  unsigned short intTagsCount; // количество целых тегов
  TIntTag *intTagsArray; // указатель на массив целых тегов

  unsigned short structVersionNumber;/* версия структуры.
    ВНИМАНИЕ!!!! если Вы пользуетесь данным заголовочным файлом,
    версия структуры в Ваших объектах должна быть равной 4 !!!
    */

  char DeviceName[64]; // имя устройства

  struct
  {
    /*struct // а это сами расширения:
    {
      unsigned short stringTagsQnty;  // количество строковых тегов
      const TStringTag *stringTagsArray; // указатель на массив строковых тегов
    }  ension;*/
    WORD   reserv_count;
    LPVOID reserv_ptr;
    char Reserved[128-sizeof(WORD)-sizeof(LPVOID)]; // Зарезервировано место для будущих расширений.
  } Ext;
                      // (эта память должна быть обнулена)
  TDataForOPCServer ();
  TDataForOPCServer (const char * dev_name,WORD anTagsCnt,WORD bitTagsCnt,WORD intTagsCnt,char * tag_name_prefix = 0);
  TDataForOPCServer (const TDataForOPCServer & src);
  ~TDataForOPCServer(){release_tags_arrays();};
  void __fastcall release_tags_arrays();
  TDataForOPCServer & operator = (const TDataForOPCServer & src);
  static void __fastcall lock();
  static void __fastcall unlock();


};


//=============== Функции, вызываемые OPC-сервером: ==============
// Реализацию этих функций необходимо написать разработчику dll ==
// примеры реализации в файлах dataservX.cpp
//===============================================================


extern "C"
{

/*  1) функция, возвращающая указатель 
       на структуру типа TDataForOPCServer (или на массив этих структур),
       описывающую устройства и теги. Возвращаемый этой функцией указатель
       используется OPC-сервером, чтобы узнать, где в dll что лежит
       (то есть где описания тегов и устройств и откуда брать данные)
       функция должна быть реализована разработчиком dll. Рекомендуется
       разместить структуру (иои массив) TDataForOPCServer статически,
       а в реализации функции просто вернуть на нее указавтель.
*/

__declspec( dllexport )
     const TDataForOPCServer * PASCAL GetDataForOPCServer( void );



//   2) функция для изменения значения аналогового тега
__declspec( dllexport )
       unsigned char PASCAL SetAnTagValue(
       unsigned  number, // устройства (HIWORD) и
                          // номер тега в массиве (LOWORD)
       float value            // новое значение параметра
       );

//   3) функция для изменения значения битового тега
__declspec( dllexport )
       unsigned char PASCAL SetBitTagValue(
       unsigned  number, // устройства (HIWORD) и
                          // номер тега в массиве (LOWORD)
       unsigned char value    // новое значение параметра
       );

//   4) функция для изменения значения целого тега
__declspec( dllexport )
       unsigned char PASCAL SetIntTagValue(
       unsigned  number, // устройства (HIWORD) и
                          // номер тега в массиве (LOWORD)
       int value    // новое значение параметра
       );


//   4) функция для изменения значения строкового тега
__declspec( dllexport )
       unsigned char PASCAL SetStringTagValue(
       unsigned  number, // устройства (HIWORD) и
                          // номер тега в массиве (LOWORD)
       const char* value    // новое значение параметра
       );


__declspec( dllexport )
       void PASCAL OnShutDown();



/* функция реально используется только OPC-сервером,
   для разработчика dll не нужна :
*/
__declspec( dllexport ) CRITICAL_SECTION* PASCAL  getTagLock();

__declspec( dllexport )
void exposeAnTag(TAnTag* tg, float val, unsigned qual, FILETIME tstamp);
__declspec( dllexport )
void exposeBitTag(TBitTag* tg, bool val, unsigned qual, FILETIME tstamp);
__declspec( dllexport )
void exposeIntTag(TIntTag* tg, int val, unsigned qual, FILETIME tstamp);




}; // end of extern "C"

#pragma pack(pop)

#endif
