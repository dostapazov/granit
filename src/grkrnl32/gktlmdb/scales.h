/*
  Определения сегмента шкалы
*/
#ifndef _SCALE_INC
#define _SCALE_INC

#ifndef KE_DEFS_INC
#include <ke_defs.h>
#endif

#pragma pack(push,1)

typedef struct _SCALE_SEG
{
 DWORD lo,hi;  //Границы сегмента шкалы
 DWORD count;
 float coeff[ANYSIZE_ARRAY];
}SCALE_SEG,*LPSCALE_SEG;

#pragma pack(pop)

#endif
 