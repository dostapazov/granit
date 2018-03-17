/*

*/

#ifndef _DLL_PROCS_HPP
#define _DLL_PROCS_HPP

#include <kertl.hpp>


 template <typename R = int>
 class dll_stdc_proc
 {
  public :
  typedef R (WINAPI * dll_stdc_proc_type)();
  dll_stdc_proc():proc_addr(NULL){}
  dll_stdc_proc(HINSTANCE hinst,const char * proc_name){assign(hinst, proc_name);}
  bool __fastcall assign(HINSTANCE hinst,const char * proc_name){proc_addr = (dll_stdc_proc_type)GetProcAddress(hinst,proc_name); return is_valid() ? true:false;}
  R    operator ()    (){return is_valid() ? proc_addr():R();};
  bool __fastcall is_valid(){return (proc_addr && !IsBadCodePtr((FARPROC)proc_addr)) ? true:false;}
  protected:
  dll_stdc_proc_type proc_addr;
 };

 template<typename P1,typename R = int>
 class dll_stdc_proc1
 {
  public :
  typedef R (WINAPI * dll_stdc_proc_type)(P1);
  dll_stdc_proc1():proc_addr(NULL){}
  dll_stdc_proc1(HINSTANCE hinst,const char * proc_name){assign(hinst, proc_name);}
  bool __fastcall assign(HINSTANCE hinst,const char * proc_name){proc_addr = (dll_stdc_proc_type)GetProcAddress(hinst,proc_name); return is_valid() ? true:false;}
  R    operator ()   (P1 p1 ){return is_valid() ? proc_addr(p1):R();};
  bool __fastcall is_valid(){return (proc_addr && !IsBadCodePtr((FARPROC)proc_addr)) ? true:false;}
  protected:
  dll_stdc_proc_type proc_addr;
 };

 template< typename P1 ,typename P2,typename R = int>
 class dll_stdc_proc2
 {
  public :
  typedef R (WINAPI *dll_stdc_proc_type)(P1,P2);
  dll_stdc_proc2():proc_addr(NULL){}
  dll_stdc_proc2(HINSTANCE hinst,const char * proc_name){assign(hinst, proc_name);}
  bool __fastcall assign(HINSTANCE hinst,const char * proc_name){proc_addr = (dll_stdc_proc_type)GetProcAddress(hinst,proc_name); return is_valid() ? true:false;}
  R    operator ()   (P1 p1,P2 p2 ){return is_valid() ? proc_addr(p1,p2):R();};
  bool __fastcall is_valid(){return (proc_addr && !IsBadCodePtr((FARPROC)proc_addr)) ? true:false;}
  protected:
  dll_stdc_proc_type proc_addr;
 };

 template<typename P1 ,typename P2, typename P3,typename R = int>
 class dll_stdc_proc3
 {
  public :
  typedef R (WINAPI *dll_stdc_proc_type)(P1,P2,P3);
  dll_stdc_proc3():proc_addr(NULL){}
  dll_stdc_proc3(HINSTANCE hinst,const char * proc_name){assign(hinst, proc_name);}
  bool __fastcall assign(HINSTANCE hinst,const char * proc_name){proc_addr = (dll_stdc_proc_type)GetProcAddress(hinst,proc_name); return is_valid() ? true:false;}
  R    operator ()   (P1 p1,P2 p2,P3 p3 ){return is_valid() ? proc_addr(p1,p2,p3):R();};
  bool __fastcall is_valid(){return (proc_addr && !IsBadCodePtr((FARPROC)proc_addr)) ? true:false;}
  protected:
  dll_stdc_proc_type proc_addr;
 };

 template<typename P1 ,typename P2, typename P3,typename P4,typename R = int>
 class dll_stdc_proc4
 {
  public :
  typedef R (WINAPI *dll_stdc_proc_type)(P1,P2,P3,P4);
  dll_stdc_proc4():proc_addr(NULL){}
  dll_stdc_proc4(HINSTANCE hinst,const char * proc_name){assign(hinst, proc_name);}
  bool __fastcall assign(HINSTANCE hinst,const char * proc_name){proc_addr = (dll_stdc_proc_type)GetProcAddress(hinst,proc_name); return is_valid() ? true:false;}
  R    operator ()   (P1 p1,P2 p2,P3 p3,P4 p4 ){return is_valid() ? proc_addr(p1,p2,p3,p4):R();};
  bool __fastcall is_valid(){return (proc_addr && !IsBadCodePtr((FARPROC)proc_addr)) ? true:false;}
  protected:
  dll_stdc_proc_type proc_addr;
 };


 template<typename P1 ,typename P2, typename P3,typename P4,typename P5,typename R = int>
 class dll_stdc_proc5
 {
  public :
  typedef R (WINAPI *dll_stdc_proc_type)(P1,P2,P3,P4,P5);
  dll_stdc_proc5():proc_addr(NULL){}
  dll_stdc_proc5(HINSTANCE hinst,const char * proc_name){assign(hinst, proc_name);}
  bool __fastcall assign(HINSTANCE hinst,const char * proc_name){proc_addr = (dll_stdc_proc_type)GetProcAddress(hinst,proc_name); return is_valid() ? true:false;}
  R    operator ()   (P1 p1,P2 p2,P3 p3,P4 p4,P5 p5 ){return is_valid() ? proc_addr(p1,p2,p3,p4,p5):R();};
  bool __fastcall is_valid(){return (proc_addr && !IsBadCodePtr((FARPROC)proc_addr)) ? true:false;}
  protected:
  dll_stdc_proc_type proc_addr;
 };

 template< typename P1 ,typename P2, typename P3,typename P4,typename P5,typename P6,typename R = int>
 class dll_stdc_proc6
 {
  public :
  typedef R (WINAPI *dll_stdc_proc_type)(P1,P2,P3,P4,P5,P6);
  dll_stdc_proc6():proc_addr(NULL){}
  dll_stdc_proc6(HINSTANCE hinst,const char * proc_name){assign(hinst, proc_name);}
  bool __fastcall assign(HINSTANCE hinst,const char * proc_name){proc_addr = (dll_stdc_proc_type)GetProcAddress(hinst,proc_name); return is_valid() ? true:false;}
  R    operator ()   (P1 p1,P2 p2,P3 p3,P4 p4,P5 p5 ,P6 p6 ){return is_valid() ? proc_addr(p1,p2,p3,p4,p5,p6):R();};
  bool __fastcall is_valid(){return (proc_addr && !IsBadCodePtr((FARPROC)proc_addr)) ? true:false;}
  protected:
  dll_stdc_proc_type proc_addr;
 };

 template<typename P1 ,typename P2, typename P3,typename P4,typename P5,typename P6,typename P7,typename R = int>
 class dll_stdc_proc7
 {
  public :
  typedef R (WINAPI *dll_stdc_proc_type)(P1,P2,P3,P4,P5,P6,P7);
  dll_stdc_proc7():proc_addr(NULL){}
  dll_stdc_proc7(HINSTANCE hinst,const char * proc_name){assign(hinst, proc_name);}
  bool __fastcall assign(HINSTANCE hinst,const char * proc_name){proc_addr = (dll_stdc_proc_type)GetProcAddress(hinst,proc_name); return is_valid() ? true:false;}
  R    operator ()   (P1 p1,P2 p2,P3 p3,P4 p4,P5 p5 ,P6 p6,P7 p7 ){return is_valid() ? proc_addr(p1,p2,p3,p4,p5,p6,p7):R();};
  bool __fastcall is_valid(){return (proc_addr && !IsBadCodePtr((FARPROC)proc_addr)) ? true:false;}
  protected:
  dll_stdc_proc_type proc_addr;
 };

 template<typename P1 ,typename P2, typename P3,typename P4,typename P5,typename P6,typename P7,typename P8,typename R = int>
 class dll_stdc_proc8
 {
  public :
  typedef R (WINAPI *dll_stdc_proc_type)(P1,P2,P3,P4,P5,P6,P7,P8);
  dll_stdc_proc8():proc_addr(NULL){}
  dll_stdc_proc8(HINSTANCE hinst,const char * proc_name){assign(hinst, proc_name);}
  bool __fastcall assign(HINSTANCE hinst,const char * proc_name){proc_addr = (dll_stdc_proc_type)GetProcAddress(hinst,proc_name); return is_valid() ? true:false;}
  R    operator ()   (P1 p1,P2 p2,P3 p3,P4 p4,P5 p5 ,P6 p6,P7 p7,P8 p8 ){return is_valid() ? proc_addr(p1,p2,p3,p4,p5,p6,p7,p8):R();};
  bool __fastcall is_valid(){return (proc_addr && !IsBadCodePtr((FARPROC)proc_addr)) ? true:false;}
  protected:
  dll_stdc_proc_type proc_addr;
 };

 template<typename P1 ,typename P2, typename P3,typename P4,typename P5,typename P6,typename P7,typename P8,typename P9,typename R = int>
 class dll_stdc_proc9
 {
  public :
  typedef R (WINAPI *dll_stdc_proc_type)(P1,P2,P3,P4,P5,P6,P7,P8,P9);
  dll_stdc_proc9():proc_addr(NULL){}
  dll_stdc_proc9(HINSTANCE hinst,const char * proc_name){assign(hinst, proc_name);}
  bool __fastcall assign(HINSTANCE hinst,const char * proc_name){proc_addr = (dll_stdc_proc_type)GetProcAddress(hinst,proc_name); return is_valid() ? true:false;}
  R    operator ()   (P1 p1,P2 p2,P3 p3,P4 p4,P5 p5 ,P6 p6,P7 p7,P8 p8,P9 p9 ){return is_valid() ? proc_addr(p1,p2,p3,p4,p5,p6,p7,p8,p9):R();};
  bool __fastcall is_valid(){return (proc_addr && !IsBadCodePtr((FARPROC)proc_addr)) ? true:false;}
  protected:
  dll_stdc_proc_type proc_addr;
 };

 template<typename P1 ,typename P2, typename P3,typename P4,typename P5,typename P6,typename P7,typename P8,typename P9,typename P10,typename R = int>
 class dll_stdc_proc10
 {
  public :
  typedef R (WINAPI *dll_stdc_proc_type)(P1,P2,P3,P4,P5,P6,P7,P8,P9,P10);
  dll_stdc_proc10():proc_addr(NULL){}
  dll_stdc_proc10(HINSTANCE hinst,const char * proc_name){assign(hinst, proc_name);}
  bool __fastcall assign(HINSTANCE hinst,const char * proc_name){proc_addr = (dll_stdc_proc_type)GetProcAddress(hinst,proc_name); return is_valid() ? true:false;}
  R    operator ()   (P1 p1,P2 p2,P3 p3,P4 p4,P5 p5 ,P6 p6,P7 p7,P8 p8,P9 p9,P10 p10 ){return is_valid() ? proc_addr(p1,p2,p3,p4,p5,p6,p7,p8,p9,p10):R();};
  bool __fastcall is_valid(){return (proc_addr && !IsBadCodePtr((FARPROC)proc_addr)) ? true:false;}
  protected:
  dll_stdc_proc_type proc_addr;
 };

 template<typename P1 ,typename P2, typename P3,typename P4,typename P5,typename P6,typename P7,typename P8,typename P9,typename P10,typename P11,typename R = int>
 class dll_stdc_proc11
 {
  public :
  typedef R (WINAPI *dll_stdc_proc_type)(P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11);
  dll_stdc_proc11():proc_addr(NULL){}
  dll_stdc_proc11(HINSTANCE hinst,const char * proc_name){assign(hinst, proc_name);}
  bool __fastcall assign(HINSTANCE hinst,const char * proc_name){proc_addr = (dll_stdc_proc_type)GetProcAddress(hinst,proc_name); return is_valid() ? true:false;}
  R    operator ()   (P1 p1,P2 p2,P3 p3,P4 p4,P5 p5 ,P6 p6,P7 p7,P8 p8,P9 p9,P10 p10,P11 p11 ){return is_valid() ? proc_addr(p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11):R();};
  bool __fastcall is_valid(){return (proc_addr && !IsBadCodePtr((FARPROC)proc_addr)) ? true:false;}
  protected:
  dll_stdc_proc_type proc_addr;
 };

 template<typename P1 ,typename P2, typename P3,typename P4,typename P5,typename P6,typename P7,typename P8,typename P9,typename P10,typename P11,typename P12,typename R = int>
 class dll_stdc_proc12
 {
  public :
  typedef R (WINAPI *dll_stdc_proc_type)(P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11,P12);
  dll_stdc_proc12():proc_addr(NULL){}
  dll_stdc_proc12(HINSTANCE hinst,const char * proc_name){assign(hinst, proc_name);}
  bool __fastcall assign(HINSTANCE hinst,const char * proc_name){proc_addr = (dll_stdc_proc_type)GetProcAddress(hinst,proc_name); return is_valid() ? true:false;}
  R    operator ()   (P1 p1,P2 p2,P3 p3,P4 p4,P5 p5 ,P6 p6,P7 p7,P8 p8,P9 p9,P10 p10,P11 p11,P12 p12 ){return is_valid() ? proc_addr(p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12):R();};
  bool __fastcall is_valid(){return (proc_addr && !IsBadCodePtr((FARPROC)proc_addr)) ? true:false;}
  protected:
  dll_stdc_proc_type proc_addr;
 };

 template<typename P1 ,typename P2, typename P3,typename P4,typename P5,typename P6,
          typename P7,typename P8,typename P9,typename P10,typename P11,typename P12,
          typename P13,
          typename R = int>
 class dll_stdc_proc13
 {
  public :
  typedef R (WINAPI *dll_stdc_proc_type)(P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11,P12,P13);
  dll_stdc_proc13():proc_addr(NULL){}
  dll_stdc_proc13(HINSTANCE hinst,const char * proc_name){assign(hinst, proc_name);}
  bool __fastcall assign(HINSTANCE hinst,const char * proc_name){proc_addr = (dll_stdc_proc_type)GetProcAddress(hinst,proc_name); return is_valid() ? true:false;}
  R    operator ()   (P1 p1,P2 p2,P3 p3,P4 p4,P5 p5 ,P6 p6,P7 p7,P8 p8,P9 p9,P10 p10,P11 p11,P12 p12,P13 p13 ){return is_valid() ? proc_addr(p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13):R();};
  bool __fastcall is_valid(){return (proc_addr && !IsBadCodePtr((FARPROC)proc_addr)) ? true:false;}
  protected:
  dll_stdc_proc_type proc_addr;
 };


 template<typename P1 ,typename P2, typename P3,typename P4,typename P5,typename P6,
          typename P7,typename P8,typename P9,typename P10,typename P11,typename P12,
          typename P13,typename P14,
          typename R = int>
 class dll_stdc_proc14
 {
  public :
  typedef R (WINAPI *dll_stdc_proc_type)(P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11,P12,P13,P14);
  dll_stdc_proc14():proc_addr(NULL){}
  dll_stdc_proc14(HINSTANCE hinst,const char * proc_name){assign(hinst, proc_name);}
  bool __fastcall assign(HINSTANCE hinst,const char * proc_name){proc_addr = (dll_stdc_proc_type)GetProcAddress(hinst,proc_name); return is_valid() ? true:false;}
  R    operator ()   (P1 p1,P2 p2,P3 p3,P4 p4,P5 p5 ,P6 p6,P7 p7,P8 p8,P9 p9,
                      P10 p10,P11 p11,P12 p12,P13 p13,P14 p14 )
  {return is_valid() ? proc_addr(p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14):R();};
  bool __fastcall is_valid(){return (proc_addr && !IsBadCodePtr((FARPROC)proc_addr)) ? true:false;}
  protected:
  dll_stdc_proc_type proc_addr;
 };

 template<typename P1 ,typename P2, typename P3,typename P4,typename P5,typename P6,
          typename P7,typename P8,typename P9,typename P10,typename P11,typename P12,
          typename P13,typename P14,typename P15,
          typename R = int>
 class dll_stdc_proc15
 {
  public :
  typedef R (WINAPI *dll_stdc_proc_type)(P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11,P12,P13,P14,P15);
  dll_stdc_proc15():proc_addr(NULL){}
  dll_stdc_proc15(HINSTANCE hinst,const char * proc_name){assign(hinst, proc_name);}
  bool __fastcall assign(HINSTANCE hinst,const char * proc_name){proc_addr = (dll_stdc_proc_type)GetProcAddress(hinst,proc_name); return is_valid() ? true:false;}
  R    operator ()   (P1 p1,P2 p2,P3 p3,P4 p4,P5 p5 ,P6 p6,P7 p7,P8 p8,P9 p9,
                      P10 p10,P11 p11,P12 p12,P13 p13,P14 p14, P15 p15 )
  {return is_valid() ? proc_addr(p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15):R();};
  bool __fastcall is_valid(){return (proc_addr && !IsBadCodePtr((FARPROC)proc_addr)) ? true:false;}
  protected:
  dll_stdc_proc_type proc_addr;
 };

 template<typename P1 ,typename P2, typename P3,typename P4,typename P5,typename P6,
          typename P7,typename P8,typename P9,typename P10,typename P11,typename P12,
          typename P13,typename P14,typename P15,typename P16,
          typename R = int>
 class dll_stdc_proc16
 {
  public :
  typedef R (WINAPI *dll_stdc_proc_type)(P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11,P12,P13,P14,P15,P16);
  dll_stdc_proc16():proc_addr(NULL){}
  dll_stdc_proc16(HINSTANCE hinst,const char * proc_name){assign(hinst, proc_name);}
  bool __fastcall assign(HINSTANCE hinst,const char * proc_name){proc_addr = (dll_stdc_proc_type)GetProcAddress(hinst,proc_name); return is_valid() ? true:false;}
  R    operator ()   (P1 p1,P2 p2,P3 p3,P4 p4,P5 p5 ,P6 p6,P7 p7,P8 p8,P9 p9,
                      P10 p10,P11 p11,P12 p12,P13 p13,P14 p14, P15 p15, P16 p16 )
  {return is_valid() ? proc_addr(p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16):R();};
  bool __fastcall is_valid(){return (proc_addr && !IsBadCodePtr((FARPROC)proc_addr)) ? true:false;}
  protected:
  dll_stdc_proc_type proc_addr;
 };


 template <typename R = int>
 class dll_cdecl_proc
 {
  public :
  typedef R ( * dll_cdecl_proc_type)();
  dll_cdecl_proc():proc_addr(NULL){}
  dll_cdecl_proc(HINSTANCE hinst,const char * proc_name){assign(hinst, proc_name);}
  bool __fastcall assign(HINSTANCE hinst,const char * proc_name){proc_addr = (dll_cdecl_proc_type)GetProcAddress(hinst,proc_name); return is_valid() ? true:false;}
  R    operator ()    (){return is_valid() ? proc_addr():R();};
  bool __fastcall is_valid(){return (proc_addr && !IsBadCodePtr((FARPROC)proc_addr)) ? true:false;}
  protected:
  dll_cdecl_proc_type proc_addr;
 };

 template<typename P1,typename R = int>
 class dll_cdecl_proc1
 {
  public :
  typedef R ( * dll_cdecl_proc_type)(P1);
  dll_cdecl_proc1():proc_addr(NULL){}
  dll_cdecl_proc1(HINSTANCE hinst,const char * proc_name){assign(hinst, proc_name);}
  bool __fastcall assign(HINSTANCE hinst,const char * proc_name){proc_addr = (dll_cdecl_proc_type)GetProcAddress(hinst,proc_name); return is_valid() ? true:false;}
  R    operator ()   (P1 p1 ){return is_valid() ? proc_addr(p1):R();};
  bool __fastcall is_valid(){return (proc_addr && !IsBadCodePtr((FARPROC)proc_addr)) ? true:false;}
  protected:
  dll_cdecl_proc_type proc_addr;
 };

 template< typename P1 ,typename P2,typename R = int>
 class dll_cdecl_proc2
 {
  public :
  typedef R ( *dll_cdecl_proc_type)(P1,P2);
  dll_cdecl_proc2():proc_addr(NULL){}
  dll_cdecl_proc2(HINSTANCE hinst,const char * proc_name){assign(hinst, proc_name);}
  bool __fastcall assign(HINSTANCE hinst,const char * proc_name){proc_addr = (dll_cdecl_proc_type)GetProcAddress(hinst,proc_name); return is_valid() ? true:false;}
  R    operator ()   (P1 p1,P2 p2 ){return is_valid() ? proc_addr(p1,p2):R();};
  bool __fastcall is_valid(){return (proc_addr && !IsBadCodePtr((FARPROC)proc_addr)) ? true:false;}
  protected:
  dll_cdecl_proc_type proc_addr;
 };

 template<typename P1 ,typename P2, typename P3,typename R = int>
 class dll_cdecl_proc3
 {
  public :
  typedef R ( *dll_cdecl_proc_type)(P1,P2,P3);
  dll_cdecl_proc3():proc_addr(NULL){}
  dll_cdecl_proc3(HINSTANCE hinst,const char * proc_name){assign(hinst, proc_name);}
  bool __fastcall assign(HINSTANCE hinst,const char * proc_name){proc_addr = (dll_cdecl_proc_type)GetProcAddress(hinst,proc_name); return is_valid() ? true:false;}
  R    operator ()   (P1 p1,P2 p2,P3 p3 ){return is_valid() ? proc_addr(p1,p2,p3):R();};
  bool __fastcall is_valid(){return (proc_addr && !IsBadCodePtr((FARPROC)proc_addr)) ? true:false;}
  protected:
  dll_cdecl_proc_type proc_addr;
 };

 template<typename P1 ,typename P2, typename P3,typename P4,typename R = int>
 class dll_cdecl_proc4
 {
  public :
  typedef R ( *dll_cdecl_proc_type)(P1,P2,P3,P4);
  dll_cdecl_proc4():proc_addr(NULL){}
  dll_cdecl_proc4(HINSTANCE hinst,const char * proc_name){assign(hinst, proc_name);}
  bool __fastcall assign(HINSTANCE hinst,const char * proc_name){proc_addr = (dll_cdecl_proc_type)GetProcAddress(hinst,proc_name); return is_valid() ? true:false;}
  R    operator ()   (P1 p1,P2 p2,P3 p3,P4 p4 ){return is_valid() ? proc_addr(p1,p2,p3,p4):R();};
  bool __fastcall is_valid(){return (proc_addr && !IsBadCodePtr((FARPROC)proc_addr)) ? true:false;}
  protected:
  dll_cdecl_proc_type proc_addr;
 };


 template<typename P1 ,typename P2, typename P3,typename P4,typename P5,typename R = int>
 class dll_cdecl_proc5
 {
  public :
  typedef R ( *dll_cdecl_proc_type)(P1,P2,P3,P4,P5);
  dll_cdecl_proc5():proc_addr(NULL){}
  dll_cdecl_proc5(HINSTANCE hinst,const char * proc_name){assign(hinst, proc_name);}
  bool __fastcall assign(HINSTANCE hinst,const char * proc_name){proc_addr = (dll_cdecl_proc_type)GetProcAddress(hinst,proc_name); return is_valid() ? true:false;}
  R    operator ()   (P1 p1,P2 p2,P3 p3,P4 p4,P5 p5 ){return is_valid() ? proc_addr(p1,p2,p3,p4,p5):R();};
  bool __fastcall is_valid(){return (proc_addr && !IsBadCodePtr((FARPROC)proc_addr)) ? true:false;}
  protected:
  dll_cdecl_proc_type proc_addr;
 };

 template< typename P1 ,typename P2, typename P3,typename P4,typename P5,typename P6,typename R = int>
 class dll_cdecl_proc6
 {
  public :
  typedef R ( *dll_cdecl_proc_type)(P1,P2,P3,P4,P5,P6);
  dll_cdecl_proc6():proc_addr(NULL){}
  dll_cdecl_proc6(HINSTANCE hinst,const char * proc_name){assign(hinst, proc_name);}
  bool __fastcall assign(HINSTANCE hinst,const char * proc_name){proc_addr = (dll_cdecl_proc_type)GetProcAddress(hinst,proc_name); return is_valid() ? true:false;}
  R    operator ()   (P1 p1,P2 p2,P3 p3,P4 p4,P5 p5 ,P6 p6 ){return is_valid() ? proc_addr(p1,p2,p3,p4,p5,p6):R();};
  bool __fastcall is_valid(){return (proc_addr && !IsBadCodePtr((FARPROC)proc_addr)) ? true:false;}
  protected:
  dll_cdecl_proc_type proc_addr;
 };

 template<typename P1 ,typename P2, typename P3,typename P4,typename P5,typename P6,typename P7,typename R = int>
 class dll_cdecl_proc7
 {
  public :
  typedef R ( *dll_cdecl_proc_type)(P1,P2,P3,P4,P5,P6,P7);
  dll_cdecl_proc7():proc_addr(NULL){}
  dll_cdecl_proc7(HINSTANCE hinst,const char * proc_name){assign(hinst, proc_name);}
  bool __fastcall assign(HINSTANCE hinst,const char * proc_name){proc_addr = (dll_cdecl_proc_type)GetProcAddress(hinst,proc_name); return is_valid() ? true:false;}
  R    operator ()   (P1 p1,P2 p2,P3 p3,P4 p4,P5 p5 ,P6 p6,P7 p7 ){return is_valid() ? proc_addr(p1,p2,p3,p4,p5,p6,p7):R();};
  bool __fastcall is_valid(){return (proc_addr && !IsBadCodePtr((FARPROC)proc_addr)) ? true:false;}
  protected:
  dll_cdecl_proc_type proc_addr;
 };

 template<typename P1 ,typename P2, typename P3,typename P4,typename P5,typename P6,typename P7,typename P8,typename R = int>
 class dll_cdecl_proc8
 {
  public :
  typedef R ( *dll_cdecl_proc_type)(P1,P2,P3,P4,P5,P6,P7,P8);
  dll_cdecl_proc8():proc_addr(NULL){}
  dll_cdecl_proc8(HINSTANCE hinst,const char * proc_name){assign(hinst, proc_name);}
  bool __fastcall assign(HINSTANCE hinst,const char * proc_name){proc_addr = (dll_cdecl_proc_type)GetProcAddress(hinst,proc_name); return is_valid() ? true:false;}
  R    operator ()   (P1 p1,P2 p2,P3 p3,P4 p4,P5 p5 ,P6 p6,P7 p7,P8 p8 ){return is_valid() ? proc_addr(p1,p2,p3,p4,p5,p6,p7,p8):R();};
  bool __fastcall is_valid(){return (proc_addr && !IsBadCodePtr((FARPROC)proc_addr)) ? true:false;}
  protected:
  dll_cdecl_proc_type proc_addr;
 };

 template<typename P1 ,typename P2, typename P3,typename P4,typename P5,typename P6,typename P7,typename P8,typename P9,typename R = int>
 class dll_cdecl_proc9
 {
  public :
  typedef R ( *dll_cdecl_proc_type)(P1,P2,P3,P4,P5,P6,P7,P8,P9);
  dll_cdecl_proc9():proc_addr(NULL){}
  dll_cdecl_proc9(HINSTANCE hinst,const char * proc_name){assign(hinst, proc_name);}
  bool __fastcall assign(HINSTANCE hinst,const char * proc_name){proc_addr = (dll_cdecl_proc_type)GetProcAddress(hinst,proc_name); return is_valid() ? true:false;}
  R    operator ()   (P1 p1,P2 p2,P3 p3,P4 p4,P5 p5 ,P6 p6,P7 p7,P8 p8,P9 p9 ){return is_valid() ? proc_addr(p1,p2,p3,p4,p5,p6,p7,p8,p9):R();};
  bool __fastcall is_valid(){return (proc_addr && !IsBadCodePtr((FARPROC)proc_addr)) ? true:false;}
  protected:
  dll_cdecl_proc_type proc_addr;
 };

 template<typename P1 ,typename P2, typename P3,typename P4,typename P5,typename P6,typename P7,typename P8,typename P9,typename P10,typename R = int>
 class dll_cdecl_proc10
 {
  public :
  typedef R ( *dll_cdecl_proc_type)(P1,P2,P3,P4,P5,P6,P7,P8,P9,P10);
  dll_cdecl_proc10():proc_addr(NULL){}
  dll_cdecl_proc10(HINSTANCE hinst,const char * proc_name){assign(hinst, proc_name);}
  bool __fastcall assign(HINSTANCE hinst,const char * proc_name){proc_addr = (dll_cdecl_proc_type)GetProcAddress(hinst,proc_name); return is_valid() ? true:false;}
  R    operator ()   (P1 p1,P2 p2,P3 p3,P4 p4,P5 p5 ,P6 p6,P7 p7,P8 p8,P9 p9,P10 p10 ){return is_valid() ? proc_addr(p1,p2,p3,p4,p5,p6,p7,p8,p9,p10):R();};
  bool __fastcall is_valid(){return (proc_addr && !IsBadCodePtr((FARPROC)proc_addr)) ? true:false;}
  protected:
  dll_cdecl_proc_type proc_addr;
 };

 template<typename P1 ,typename P2, typename P3,typename P4,typename P5,typename P6,typename P7,typename P8,typename P9,typename P10,typename P11,typename R = int>
 class dll_cdecl_proc11
 {
  public :
  typedef R ( *dll_cdecl_proc_type)(P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11);
  dll_cdecl_proc11():proc_addr(NULL){}
  dll_cdecl_proc11(HINSTANCE hinst,const char * proc_name){assign(hinst, proc_name);}
  bool __fastcall assign(HINSTANCE hinst,const char * proc_name){proc_addr = (dll_cdecl_proc_type)GetProcAddress(hinst,proc_name); return is_valid() ? true:false;}
  R    operator ()   (P1 p1,P2 p2,P3 p3,P4 p4,P5 p5 ,P6 p6,P7 p7,P8 p8,P9 p9,P10 p10,P11 p11 ){return is_valid() ? proc_addr(p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11):R();};
  bool __fastcall is_valid(){return (proc_addr && !IsBadCodePtr((FARPROC)proc_addr)) ? true:false;}
  protected:
  dll_cdecl_proc_type proc_addr;
 };

 template<typename P1 ,typename P2, typename P3,typename P4,typename P5,typename P6,typename P7,typename P8,typename P9,typename P10,typename P11,typename P12,typename R = int>
 class dll_cdecl_proc12
 {
  public :
  typedef R ( *dll_cdecl_proc_type)(P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11,P12);
  dll_cdecl_proc12():proc_addr(NULL){}
  dll_cdecl_proc12(HINSTANCE hinst,const char * proc_name){assign(hinst, proc_name);}
  bool __fastcall assign(HINSTANCE hinst,const char * proc_name){proc_addr = (dll_cdecl_proc_type)GetProcAddress(hinst,proc_name); return is_valid() ? true:false;}
  R    operator ()   (P1 p1,P2 p2,P3 p3,P4 p4,P5 p5 ,P6 p6,P7 p7,P8 p8,P9 p9,P10 p10,P11 p11,P12 p12 ){return is_valid() ? proc_addr(p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12):R();};
  bool __fastcall is_valid(){return (proc_addr && !IsBadCodePtr((FARPROC)proc_addr)) ? true:false;}
  protected:
  dll_cdecl_proc_type proc_addr;
 };

 template<typename P1 ,typename P2, typename P3,typename P4,typename P5,typename P6,
          typename P7,typename P8,typename P9,typename P10,typename P11,typename P12,
          typename P13,
          typename R = int>
 class dll_cdecl_proc13
 {
  public :
  typedef R ( *dll_cdecl_proc_type)(P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11,P12,P13);
  dll_cdecl_proc13():proc_addr(NULL){}
  dll_cdecl_proc13(HINSTANCE hinst,const char * proc_name){assign(hinst, proc_name);}
  bool __fastcall assign(HINSTANCE hinst,const char * proc_name){proc_addr = (dll_cdecl_proc_type)GetProcAddress(hinst,proc_name); return is_valid() ? true:false;}
  R    operator ()   (P1 p1,P2 p2,P3 p3,P4 p4,P5 p5 ,P6 p6,P7 p7,P8 p8,P9 p9,P10 p10,P11 p11,P12 p12,P13 p13 ){return is_valid() ? proc_addr(p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13):R();};
  bool __fastcall is_valid(){return (proc_addr && !IsBadCodePtr((FARPROC)proc_addr)) ? true:false;}
  protected:
  dll_cdecl_proc_type proc_addr;
 };


 template<typename P1 ,typename P2, typename P3,typename P4,typename P5,typename P6,
          typename P7,typename P8,typename P9,typename P10,typename P11,typename P12,
          typename P13,typename P14,
          typename R = int>
 class dll_cdecl_proc14
 {
  public :
  typedef R ( *dll_cdecl_proc_type)(P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11,P12,P13,P14);
  dll_cdecl_proc14():proc_addr(NULL){}
  dll_cdecl_proc14(HINSTANCE hinst,const char * proc_name){assign(hinst, proc_name);}
  bool __fastcall assign(HINSTANCE hinst,const char * proc_name){proc_addr = (dll_cdecl_proc_type)GetProcAddress(hinst,proc_name); return is_valid() ? true:false;}
  R    operator ()   (P1 p1,P2 p2,P3 p3,P4 p4,P5 p5 ,P6 p6,P7 p7,P8 p8,P9 p9,
                      P10 p10,P11 p11,P12 p12,P13 p13,P14 p14 )
  {return is_valid() ? proc_addr(p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14):R();};
  bool __fastcall is_valid(){return (proc_addr && !IsBadCodePtr((FARPROC)proc_addr)) ? true:false;}
  protected:
  dll_cdecl_proc_type proc_addr;
 };

 template<typename P1 ,typename P2, typename P3,typename P4,typename P5,typename P6,
          typename P7,typename P8,typename P9,typename P10,typename P11,typename P12,
          typename P13,typename P14,typename P15,
          typename R = int>
 class dll_cdecl_proc15
 {
  public :
  typedef R ( *dll_cdecl_proc_type)(P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11,P12,P13,P14,P15);
  dll_cdecl_proc15():proc_addr(NULL){}
  dll_cdecl_proc15(HINSTANCE hinst,const char * proc_name){assign(hinst, proc_name);}
  bool __fastcall assign(HINSTANCE hinst,const char * proc_name){proc_addr = (dll_cdecl_proc_type)GetProcAddress(hinst,proc_name); return is_valid() ? true:false;}
  R    operator ()   (P1 p1,P2 p2,P3 p3,P4 p4,P5 p5 ,P6 p6,P7 p7,P8 p8,P9 p9,
                      P10 p10,P11 p11,P12 p12,P13 p13,P14 p14, P15 p15 )
  {return is_valid() ? proc_addr(p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15):R();};
  bool __fastcall is_valid(){return (proc_addr && !IsBadCodePtr((FARPROC)proc_addr)) ? true:false;}
  protected:
  dll_cdecl_proc_type proc_addr;
 };

 template<typename P1 ,typename P2, typename P3,typename P4,typename P5,typename P6,
          typename P7,typename P8,typename P9,typename P10,typename P11,typename P12,
          typename P13,typename P14,typename P15,typename P16,
          typename R = int>
 class dll_cdecl_proc16
 {
  public :
  typedef R ( *dll_cdecl_proc_type)(P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11,P12,P13,P14,P15,P16);
  dll_cdecl_proc16():proc_addr(NULL){}
  dll_cdecl_proc16(HINSTANCE hinst,const char * proc_name){assign(hinst, proc_name);}
  bool __fastcall assign(HINSTANCE hinst,const char * proc_name){proc_addr = (dll_cdecl_proc_type)GetProcAddress(hinst,proc_name); return is_valid() ? true:false;}
  R    operator ()   (P1 p1,P2 p2,P3 p3,P4 p4,P5 p5 ,P6 p6,P7 p7,P8 p8,P9 p9,
                      P10 p10,P11 p11,P12 p12,P13 p13,P14 p14, P15 p15, P16 p16 )
  {return is_valid() ? proc_addr(p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16):R();};
  bool __fastcall is_valid(){return (proc_addr && !IsBadCodePtr((FARPROC)proc_addr)) ? true:false;}
  protected:
  dll_cdecl_proc_type proc_addr;
 };


 #define dll_proc   dll_stdc_proc
 #define dll_proc1  dll_stdc_proc1 
 #define dll_proc2  dll_stdc_proc2 
 #define dll_proc3  dll_stdc_proc3 
 #define dll_proc4  dll_stdc_proc4 
 #define dll_proc5  dll_stdc_proc5 
 #define dll_proc6  dll_stdc_proc6 
 #define dll_proc7  dll_stdc_proc7 
 #define dll_proc8  dll_stdc_proc8 
 #define dll_proc9  dll_stdc_proc9 
 #define dll_proc10 dll_stdc_proc10
 #define dll_proc11 dll_stdc_proc11
 #define dll_proc12 dll_stdc_proc12
 #define dll_proc13 dll_stdc_proc13
 #define dll_proc14 dll_stdc_proc14
 #define dll_proc15 dll_stdc_proc15
 #define dll_proc16 dll_stdc_proc16
 


#endif

