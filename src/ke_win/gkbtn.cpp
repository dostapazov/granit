#include <gkwctrls.hpp>
#pragma hdrstop

#ifndef _NO_NAMESPACE
namespace KeWin
{
#endif

  bool __fastcall TButton::DoCreate(HWND Parent,char * Caption,TRect & r,int Id )
  {
    Create(Parent,Caption,r,Id,0,0,0,"BUTTON");
    return hWnd ? true:false;
  }


#ifndef _NO_NAMESPACE
//namespace KeWin
}
#endif
