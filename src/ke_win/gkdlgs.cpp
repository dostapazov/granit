// Реализация диалового окна
// GranitKernel32 V 1.0
// Остапенко Д.В. СДТУ 1993-2000

#include <gkdlgs.hpp>
//winuser.h

#ifndef _NO_NAMESPACE
namespace KeWin
{
#endif


#ifdef __BORLANDC__
    #pragma warn -8057
#endif
    LRESULT CALLBACK DLG_WND_PROC(HWND hWnd,UINT Msg,WPARAM wp,LPARAM lp)
    {
      return 0;
    }

#ifdef __BORLANDC__
    #pragma warn .8057
#endif



    LRESULT  CALLBACK TDialog::DefaultProc(HWND hWnd,UINT Msg,WPARAM wp,LPARAM lp)
	{
	 LRESULT ret = TWindow::DefaultProc(hWnd,Msg,wp,lp);
	 ::SetWindowLong((::HWND)hWnd,DWL_MSGRESULT,ret);
	 return ret;
	}

	 #pragma warn -8057
	BOOL CALLBACK    TDialog::DlgProc(HWND hWnd,UINT Msg,WPARAM wp,LPARAM lp)
	{
	 if(Msg == WM_INITDIALOG)
		{
		 TDialog * dlg;
		 dlg = reinterpret_cast<TDialog*>(CAST(LPVOID(lp)));
		 dlg->hWnd = hWnd;
		 dlg->UD = ::SetWindowLong((::HWND)hWnd,GWL_USERDATA,(LONG)lp);
		 dlg->SuperclassProc = (WNDPROC)(dlg->SetWindowLong(DWL_DLGPROC,LONG(DefaultProc)));
		 if(dlg->OnWMInitDialog())
		  return false;
		 return true;
		}

	  //return TWindow::DefaultProc(hWnd,Msg,wp,lp);
	  return 0;
	}
	 #pragma warn .8057

    bool __fastcall TDialog::DoCreate()
    {
      //Не модальный вариант работы
      if(!IsModal())
      {
      SetMessageExtraInfo(LPARAM(this));
#ifdef __BORLANDC__
       hWnd = CreateDialogParamA((::HINSTANCE)hInstance,ResId,(::HWND)Parent,(DLGPROC)DlgProc,LPARAM(this));
#else
       hWnd = CreateDialogParamA((::HINSTANCE)hInstance,ResId,(::HWND)Parent,(DLGPROC)DlgProc,LPARAM(this));
#endif
      SetMessageExtraInfo(0);

       return bool(hWnd ? true:false);
      }
      return false;
    }

    int  __fastcall TDialog::ShowModal()
    {
     //По названию ясно что модальный

     if(!IsModal())
     {

      SetMessageExtraInfo(LPARAM(this));
      ModalDialog = true;
      EndMessageLoopCode = DialogBoxParamA((::HINSTANCE)this->hInstance,ResId,(::HWND)Parent,(DLGPROC)DlgProc,LPARAM(this));
      ModalDialog = false;
      hWnd = 0;
      return EndMessageLoopCode;
     }
     return IDCANCEL;
    }


   void __fastcall TDialog::EndModal(int eCode)
   {
     if(IsModal())
        EndDialog((::HWND)hWnd,eCode);
     ShowWindow(SW_HIDE);
     TWindow::EndModal(eCode);
   }

   bool         __fastcall TDialog::OnWMInitDialog()
   {
         ClientDC = GetDC((::HWND)hWnd);
         HWND first_child = GetWindow((::HWND)hWnd,GW_CHILD);
         if(first_child && (::GetWindowLong((::HWND)first_child,GWL_STYLE)&WS_TABSTOP)==0)
         {
           HWND next_child = first_child;
         do{
             next_child = GetNextWindow((::HWND)next_child,GW_HWNDNEXT);
             if(next_child && (::GetWindowLong((::HWND)next_child,GWL_STYLE)&WS_TABSTOP))
              {
               first_child = next_child;
               break;
              }
            }while(next_child && next_child!=first_child);
         }

         ::SetFocus((::HWND)first_child);
    return true;
   }

   bool   __fastcall TDialog::OnWMCommand  (HWND From,int nCode,int id)
    {
     if(IsModal() && (id >= IDOK && id <= IDNO))
        {
         EndModal(id);
         return true;
        }
	 return TWindow::OnWMCommand(From,nCode,id);
    }



#ifndef _NO_NAMESPACE
}// end of namespace
#endif

