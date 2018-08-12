/*-----------------------------------------------------------------------------
 02.02.2004 11:24:57
 Модуль:   OTD_BASE
 Автор:    Остапенко Д.В.
 Назначение: Основы ОТД - ОПЕРАТИВНЫХ ТЕХНОЛОГИЧЕСКИХ ДАННЫХ

-----------------------------------------------------------------------------*/
#ifndef _OTD_BASE_INC
#define _OTD_BASE_INC

#ifndef _OTD_BASE_H
#define _OTD_BASE_H  1
#endif //_OTD_BASE_H == 1

#ifndef RC_INVOKED

#ifdef __linux__
#include <lin_ke_defs.h>
#include <gklib/otd_types.h>
#else
#include <ke_defs.h>
#include <otd_types.h>
#endif

#include<stdlib.h>


#if _OTD_BASE_H ==1

#pragma pack(push,1)

 struct otd_numbers
 {
   BYTE loN;
   BYTE hiN;
 };

 #define lpotd_numbers otd_numbers*

 #define OTD_SET_NUMBERS(n,start,count) ((n)->loN = (BYTE)start,(n)->hiN=(BYTE)((count>0) ?start+count-1:start))
 #define OTD_GET_COUNT(n) (1+(n)->hiN-(n)->loN)
 #define OTD_CHECK_NUMBER(n,i) (((DWORD)(n)->loN)<=i && i<=((DWORD)(n)->hiN))


 /*Массив ОТД данных*/
 struct otd_data
 {
  WORD        otd_type;
  otd_numbers numbers;
  BYTE        data[ANYSIZE_ARRAY];
 };

 #define lpotd_data otd_data*


//Вычисление необходимого кол-ва байт для типа

#ifdef __cplusplus
inline DWORD __fastcall OTD_DATA_CALC_DISCRETE_SIZE(DWORD type,DWORD count)
{
  return count*(type&OTD_TYPE_LEN)/8 + ((count*(type&OTD_TYPE_LEN)%8)? 1:0);
}

inline DWORD __fastcall OTD_DATA_CALC_SIZE(DWORD type, DWORD count)
{
 DWORD ret;
 if(OTD_GET_TYPE(type)==OTD_BIT_TYPE)
   ret = OTD_DATA_CALC_DISCRETE_SIZE(type,count);
   else
   ret = count*(type&OTD_TYPE_LEN);
   return ret;
}

inline DWORD __fastcall OTD_DATA_GET_DATA_SIZE(lpotd_data od)
{
 return OTD_DATA_CALC_SIZE(od->otd_type,OTD_GET_COUNT(&od->numbers));
}


inline DWORD __fastcall OTD_DATA_GET_SIZE(lpotd_data od)
{
 DWORD ret = sizeof(*(od))-1;
       ret+=OTD_DATA_GET_DATA_SIZE(od);
 return ret;
}


#else
#define OTD_DATA_CALC_DISCRETE_SIZE(type,count) (count*(type&OTD_TYPE_LEN)/8 + ((count*(type&OTD_TYPE_LEN)%8)? 1:0))
#define OTD_DATA_CALC_SIZE(type,count) (OTD_GET_TYPE(type)==OTD_BIT_TYPE ? OTD_DATA_CALC_DISCRETE_SIZE(type,count): count*(type&OTD_TYPE_LEN))
#define OTD_DATA_GET_SIZE(od) (sizeof(*(od))-1+OTD_DATA_CALC_SIZE((od)->otd_type,OTD_GET_COUNT(&(od)->numbers)))
#endif


//Заколовок блока  данных
struct otd_block
{
 WORD     block_type;
 WORD     block_size;
 BYTE     block_data[ANYSIZE_ARRAY];
};

#define lpotd_block otd_block*

//Вычисление размера данных под otd_block, чтобы размер block_data ==N
#define OTD_BLOCK_GET_SIZE_FOR_BUFFER(N) (N+sizeof(otd_block)-1)
#define OTD_BLOCK_GET_DATA_SIZE(bl) ((bl)->block_size-sizeof(otd_block)+1)
#define OTD_BLOCK_INIT(b,t,n) ((b)->block_type =(WORD)t,(b)->block_size=(WORD)(n-(sizeof(otd_block)-1)))
#define OTD_BLOCK_GET_SIZE(bl) (sizeof(*(bl))+(bl)->block_size-1)


#define OTD_TUOP_NOP        0
#define OTD_TUOP_ON         0x0001             //Включить
#define OTD_TUOP_OFF        0x0002             //Отключить
#define OTD_TROP_MORE       OTD_TUOP_ON      //Больше
#define OTD_TROP_LESS       OTD_TUOP_OFF     //Меньше
#define OTD_TROP_SET        0x0004             //Установка параметра
#define OTD_TUTROP_CANCEL   0x0008             //Отмена
#define OTD_TUTROP_UNDO     OTD_TUTROP_CANCEL

#define OTD_TUTR_OPMASK      0x000F

#define OTD_TUTR_CMDSELECT   0x0010  //Выбор объекта для ТУ
#define OTD_TUTR_CMDENABLE   0x0020  //Разрешение ТУ/ТР
#define OTD_TUTR_CMDDESELECT 0x0040  //Отмена выбора объекта
#define OTD_TUTR_CMDMASK     0x00F0

#define OTD_TUTR_CMDATTR_CHECK_SUCCESS 0x0001
#define OTD_TUTR_CMDATTR_INVERSE       0x0002

struct otd_tutr
{
 WORD                command;
 WORD                command_attr;
 QWORD               param;
 #ifdef __cplusplus
   otd_tutr():command(0),command_attr(0),param(0){}
 #endif
};

#define lpotd_tutr otd_tutr*
//typedef otd_tutr*  lpotd_tutr;


struct otd_text /*Очень напоминает RtlUnicodeString*/
{
 WORD len;
 wchar_t text[ANYSIZE_ARRAY];
};

typedef otd_text* lpotd_text; 

#ifdef __cplusplus
inline bool OTD_BLOCK_ISTEXT     ( lpotd_block  b) {return b->block_type == OTD_BLOCKTYPE_TEXT ? true:false;}
inline WORD OTD_TEXT_GET_CAPASITY( lpotd_block  b) {return OTD_BLOCK_ISTEXT(b) ? (b->block_size-sizeof(WORD))/sizeof(wchar_t) : 0;}

#else
#define OTD_BLOCK_ISTEXT(b) ((b)->block_type == OTD_BLOCKTYPE_TEXT)
#define OTD_TEXT_GET_CAPASITY(b)  (OTD_BLOCK_ISTEXT(b) ? ((b)->block_size-sizeof(WORD))/sizeof(wchar_t) : 0)

#endif

#define OTD_GET_TEXT_CAPASITY OTD_TEXT_GET_CAPASITY
#define OTD_GET_TEXT_LENGTH   OTD_TEXT_GET_LENGTH

#ifdef __cplusplus
extern "C"{
#endif
//Быстрое деление с остатком на степени 2
div_t winapi fast_divP2(DWORD value,DWORD denom);
//Вычисление размера массива данных для предопределённых типов


DWORD winapi otd_calc_data_size(DWORD type,DWORD count);

//Получение/Установка значения
int  winapi otd_get_value (lpotd_data array,DWORD number,LPVOID  value,size_t sz);
int  winapi otd_set_value (lpotd_data array,DWORD number,LPVOID  value,size_t sz);
int  winapi otd_copy      (lpotd_data dest,lpotd_data src,int start,int count);
int  winapi otd_inc_value (lpotd_data array,DWORD number,BOOL no_zero);
int  winapi otd_dec_value (lpotd_data array,DWORD number,BOOL no_zero);
int  winapi otd_zero_value(lpotd_data array,DWORD number);
int  winapi otd_fill      (lpotd_data array ,LPVOID val,DWORD val_sz);
void winapi otd_clear     (lpotd_data data);



int  winapi otd_text_copy (lpotd_block dest,lpotd_block src);
int  winapi otd_text_merge(lpotd_block dest,lpotd_block src);
int  winapi otd_text_compare(lpotd_block dest,lpotd_block src,BOOL case_sens);
WORD winapi otd_text_get_length  ( lpotd_block  b) ;
#define OTD_TEXT_GET_LENGTH(b) otd_text_get_length(b)

lpotd_block
     winapi otd_text_init   (LPVOID buf,int buff_sz,wchar_t * init_text);
int  winapi otd_text_getwstr(lpotd_block bl,wchar_t * str,int str_sz);
int  winapi otd_text_set    (lpotd_block bl,wchar_t * str);
#define otd_data_inrange(d,n) (OTD_CHECK_NUMBER(&(d)->numbers,(DWORD)n))


#ifdef __cplusplus
}
#endif
#pragma pack(pop)

#endif //ifndef RC_INVOKED
#endif
#endif



