#ifndef CRCUNIT_WRAPPER_HPP
#define CRCUNIT_WRAPPER_HPP

#include <KeRTL.hpp>
#include <dll_procs.hpp>
#include <crcunit.h>
using namespace KeRTL;

class TCrcunitWrapper
{
 public:
 static dll_proc3<LPVOID,WORD,WORD,WORD>  CalcRemind   ;
 static dll_proc3<LPVOID,WORD,WORD,WORD>  CalcKPK      ;
 static dll_proc3<LPVOID,WORD,WORD,WORD>  CalcGranitKPK;
 static bool __fastcall TestKPK(void * Buffer,unsigned short len,unsigned short Del);
 static bool __fastcall TestGranitKPK(void * Buffer,unsigned short len,unsigned short Del);
 bool __fastcall load  ();
 void __fastcall unload();
 protected:
 static LONG usage;
 static HINSTANCE hLib;
 void __fastcall AfterLoad();
 void __fastcall BeforeUnload();

};


inline bool __fastcall TCrcunitWrapper::TestKPK(void * Buffer,unsigned short len,unsigned short Del)
{ return (CalcRemind(Buffer,len,Del)==0) ? true:false;}

inline bool __fastcall TCrcunitWrapper::TestGranitKPK(void * Buffer,unsigned short len,unsigned short Del)
{ return (CalcGranitKPK(Buffer,len,Del)==0) ? true:false;}


#endif


 