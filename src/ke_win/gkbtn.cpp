#include <gkwctrls.hpp>
#pragma hdrstop

#ifndef _NO_NAMESPACE
namespace KeWin
{
#endif

  bool __fastcall TButton::DoCreate(HWND Parent,const wchar_t * Caption,TRect & r,int Id )
  {
    Create(Parent,Caption,r,Id,0,0,0,L"BUTTON");
    return hWnd ? true:false;
  }


#ifndef _NO_NAMESPACE
//namespace KeWin
}
#endif
