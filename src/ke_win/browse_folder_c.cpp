
#include <browse_folder.hpp>


#ifndef _NO_NAMESPACE
namespace KeWin
{
#endif

   LRESULT WINAPI TFolderBrowser::FolderBrowserProc(LPARAM hWnd, UINT Msg,LPARAM p1,LPARAM p2)
   {

    TFolderBrowser * fb = dynamic_cast<TFolderBrowser*>(TWindow::CAST(LPVOID(p2)));

    if(BFFM_INITIALIZED == Msg)
      {
       if (fb)
        {
         fb->hWnd = (HWND)hWnd;
         fb->EnableOk(false);
        }
       return 0;
      }
      if(Msg == BFFM_SELCHANGED)
      {
       if(fb)
        {
         fb->current_select = (LPITEMIDLIST)p1;
         if(fb->__on_sel_changed.HaveMethod())
            return fb->__on_sel_changed.CallMethod();
            else
            return fb->OnSelChanged((LPITEMIDLIST)p1);
        }
       return 0;
      }
    return 0;
   }



#ifndef _NO_NAMESPACE
}
#endif


