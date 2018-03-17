//---------------------------------------------------------------------------
#ifndef crcunitH
#define crcunitH
//---------------------------------------------------------------------------

#define STD_GRANIT_POLINOM  0x1021
#ifdef __cplusplus
 extern "C" {
#endif

unsigned short WINAPI CalcRemind   (void * Buffer,unsigned short len,unsigned short Del);
unsigned short WINAPI CalcKPK      (void * Buffer,unsigned short len,unsigned short Del);
unsigned short WINAPI CalcGranitKPK(void  *buff  ,unsigned short len,unsigned short del);

#ifdef __cplusplus
 }
#endif


inline bool __fastcall TestKPK(void * Buffer,unsigned short len,unsigned short Del)
{
 return bool(CalcRemind(Buffer,len,Del)==0);
}

inline bool __fastcall TestGranitKPK(void * Buffer,unsigned short len,unsigned short Del)
{
 return bool(CalcGranitKPK(Buffer,len,Del)==0);
}



#endif


