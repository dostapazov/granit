#include <browse_folder.hpp>

#ifndef _NO_NAMESPACE
namespace KeWin
{
#endif

   

   LPITEMIDLIST  __fastcall TFolderBrowser::DoSelect( DWORD Flags,
                                                    DWORD root  ,
                                                    char * buffer,
                                                    int bsz,
                                                    const char * title
                                                    )
   {

    BROWSEINFOA bi;
    char  _buffer[MAX_PATH];
    ZeroMemory(&bi,sizeof(bi));
    bi.hwndOwner      = (::HWND)this->Parent;
    bi.pszDisplayName =_buffer;
    bi.lParam = (LPARAM)this;
    bi.lpfn   = (BFFCALLBACK)FolderBrowserProc;
    this->ModalDialog = true;
    SHGetSpecialFolderLocation((::HWND)Parent,root,(ITEMIDLIST**)&bi.pidlRoot ) ;
    bi.lpszTitle = title;
    bi.ulFlags   = Flags;

    LPITEMIDLIST idlist = SHBrowseForFolderA(&bi);
    hWnd = 0;
	if(buffer && bsz)
	{
	   if(idlist)
	   {
		SHGetPathFromIDListA(idlist,_buffer);
		lstrcpynA(buffer,_buffer,bsz);
	   }
	   else
		*buffer = 0;
    }


    return idlist;
   }




#ifndef _NO_NAMESPACE
}
#endif
