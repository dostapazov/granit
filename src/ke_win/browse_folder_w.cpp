#include <browse_folder.hpp>

#ifndef _NO_NAMESPACE
namespace KeWin
{
#endif



   LPITEMIDLIST  __fastcall TFolderBrowser::DoSelect( DWORD Flags,
                                                      DWORD root  ,
                                                      wchar_t * buffer,
                                                      int bsz,
                                                      const wchar_t * title
                                                      )
   {
    BROWSEINFOW bi;
    wchar_t  _buffer[MAX_PATH*sizeof(wchar_t)];
    ZeroMemory(&bi,sizeof(bi));
    bi.hwndOwner      = (::HWND)this->Parent;
    bi.pszDisplayName =_buffer;
    bi.lParam = (LPARAM)this;
    bi.lpfn   = (BFFCALLBACK)FolderBrowserProc;
    SHGetSpecialFolderLocation((::HWND)Parent,root,(ITEMIDLIST**)&bi.pidlRoot ) ;
    bi.lpszTitle = title;
    bi.ulFlags   = Flags;
    this->ModalDialog = true;
    LPITEMIDLIST idlist = SHBrowseForFolderW(&bi);
    hWnd = 0;
    this->ModalDialog = false;
	if(buffer && bsz)
	{
	   if(idlist)
	   {
		SHGetPathFromIDListW(idlist,_buffer);
		lstrcpynW(buffer,_buffer,bsz);
	   }
	   else
		*buffer = 0;
    }

    return idlist;
   }




#ifndef _NO_NAMESPACE
}
#endif

