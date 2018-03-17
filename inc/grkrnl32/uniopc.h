#ifndef UNIOPC_INC
#define UNIOPC_INC

#pragma pack(push,1)

struct TAnTag;
struct TBitTag;
struct TIntTag;
struct TStringTag;


struct TDataForOPCServer
{
  unsigned char dataIsValid;   /* признак того, что данные корректны
                                 (иначе BAD для всех тегов устройства)*/
  unsigned short anTagsQnty;   // количество аналоговых тегов
  const TAnTag *anTagsArray;   // указатель на массив аналоговых тегов
  unsigned short bitTagsQnty;  // количество битовых тегов
  const TBitTag *bitTagsArray; // указатель на массив битовых тегов
  unsigned short intTagsQnty;  // количество целых тегов
  const TIntTag *intTagsArray; // указатель на массив целых тегов

  unsigned short structVersionNumber;/* версия структуры.
    ВНИМАНИЕ!!!! если Вы пользуетесь данным заголовочным файлом,
    версия структуры в Ваших объектах должна быть равной 4 !!!
    */

  char DeviceName[64]; // имя устройства

  union
  {
    struct // а это сами расширения:
    {
      unsigned short stringTagsQnty;  // количество строковых тегов
      const TStringTag *stringTagsArray; // указатель на массив строковых тегов
    }  ension;

    char Reserved[128]; // Зарезервировано место для будущих расширений.
  } Ext;
                      // (эта память должна быть обнулена)
};



#pragma pack(pop)

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



}; // end of extern "C"

#endif


