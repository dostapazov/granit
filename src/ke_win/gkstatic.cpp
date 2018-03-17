#include <gkwctrls.hpp>
#pragma hdrstop

#ifndef _NO_NAMESPACE
namespace KeWin
{
#endif

  bool __fastcall TStatic::DoCreate(HWND Parent,char * Caption,TRect & r,int Id )
  {
    Create(Parent,Caption,r,Id,0,0,0,"STATIC");
    return hWnd ? true:false;
  }

#ifndef _NO_NAMESPACE
//namespace KeWin
}
#endif
 
