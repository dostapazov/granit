#include <gkcommctrl.hpp>

#ifndef _NO_NAMESPACE
namespace KrnlWin
{
#endif

  bool      __fastcall TProgressBar::DoCreate(HWND parent,int id)//Создание по ресурсу
  {
     this->CheckAndLoad(this);
   this->CreateFromHWND(::GetDlgItem((::HWND)parent,id));
   /*if(hWnd)
      ClientDC = GetDC(hWnd);*/
   SetRange(minRange,maxRange);
   SetPos(Position);
   return hWnd ? true:false;
  }

  bool      __fastcall TProgressBar::DoCreate(HWND parent,TRect & r,DWORD Style,DWORD ExStyle,int id)
  {
   this->CheckAndLoad(this);
   Create((::HWND)parent,0,r,id,0,Style,ExStyle,PROGRESS_CLASSA);
   if(hWnd)
      ClientDC = GetDC((::HWND)hWnd);
   SetRange(minRange,maxRange);
   SetPos(Position);
   return hWnd ? true:false;

  }


#ifndef _NO_NAMESPACE
}
#endif
