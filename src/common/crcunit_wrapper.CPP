#include <crcunit_wrapper.hpp>

 LONG      TCrcunitWrapper::usage = 0;
 HINSTANCE TCrcunitWrapper::hLib  = 0;

 dll_proc3<LPVOID,WORD,WORD,WORD>  TCrcunitWrapper::CalcRemind   ;
 dll_proc3<LPVOID,WORD,WORD,WORD>  TCrcunitWrapper::CalcKPK      ;
 dll_proc3<LPVOID,WORD,WORD,WORD>  TCrcunitWrapper::CalcGranitKPK;


 void __fastcall TCrcunitWrapper::AfterLoad()
 {
    CalcRemind.assign   (hLib,"CalcRemind");
    CalcKPK   .assign   (hLib,"CalcKPK")   ;
    CalcGranitKPK.assign(hLib,"CalcGranitKPK");
 }

 void __fastcall TCrcunitWrapper::BeforeUnload()
 {
    CalcRemind.assign   (0,0);
    CalcKPK   .assign   (0,0);
    CalcGranitKPK.assign(0,0);
 }

 bool __fastcall TCrcunitWrapper::load  ()
 {
  if(InterlockedIncrement(&usage)==1)
     {
      hLib = LoadLibrary("crcunit.dll");
      AfterLoad();
     }
   return hLib ? true:false;  
 }

 void __fastcall TCrcunitWrapper::unload()
 {
  if(InterlockedDecrement(&usage)==0)
     {
      BeforeUnload();
      FreeLibrary(hLib);
     }
     
 }








