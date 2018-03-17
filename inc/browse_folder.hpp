#ifndef GRKE_FOLDER_BROWSER_INC
#define GRKE_FOLDER_BROWSER_INC



#ifndef _SHLOBJ_H_
#include <shlobj.h>
#endif

#ifndef GRKE_WINDOW_INC
 #include <gkdlgs.hpp>
#endif

#ifndef _CLOSURE_METOD_INC
#include <keclosure.hpp>
#endif


#include <vc5core.h>


#ifndef BIF_NEWDIALOGSTYLE
#define BIF_NEWDIALOGSTYLE     0x0040   // Use the new dialog layout with the ability to resize
                                        // Caller needs to call OleInitialize() before using this API
#endif

#ifndef BIF_USENEWUI
#define BIF_USENEWUI           (BIF_NEWDIALOGSTYLE | BIF_EDITBOX)
#endif

#pragma pack (push,8)

#ifndef _NO_NAMESPACE
#ifndef KrnlWin
#define KrnlWin KeWin
#endif
namespace KeWin
{
#endif


 KEWIN_CLASS TFolderBrowser:protected KeWin::TDialog
 {
  protected:
  static LRESULT WINAPI FolderBrowserProc(LPARAM hWnd, UINT Msg,LPARAM p1,LPARAM p2);
  TClosureMethod<LPVOID,TFolderBrowser*,BOOL> __on_sel_changed;
  LPITEMIDLIST current_select;
  public:
   TFolderBrowser(HWND parent):TDialog(parent,0),current_select(0){}
   void            __fastcall SetClosure(TClosureMethod<LPVOID,TFolderBrowser*,BOOL>::closure_method cMethod,LPVOID);
   LPITEMIDLIST    __fastcall GetSelected(){return current_select;}
   LPITEMIDLIST    __fastcall DoSelect(DWORD Flags,DWORD root = 0,char    * buffer = 0,int bsz =0,const char * title=0);
   LPITEMIDLIST    __fastcall DoSelect(DWORD Flags,DWORD root = 0,wchar_t * buffer = 0,int bsz =0,const wchar_t * title=0);

   LPITEMIDLIST    __fastcall SelectFolder  (char    * buffer ,int bsz ,const char *title    = 0,bool NewUI=false,bool UseNetwork = false);
   LPITEMIDLIST    __fastcall SelectFolder  (wchar_t * buffer ,int bsz ,const wchar_t *title = 0,bool NewUI=false,bool UseNetwork = false);
   LPITEMIDLIST    __fastcall SelectNetwork (char    * buffer ,int bsz ,const char    *title = 0);
   LPITEMIDLIST    __fastcall SelectNetwork (wchar_t * buffer ,int bsz ,const wchar_t *title = 0);
   LPITEMIDLIST    __fastcall SelectPrinters(char    * buffer ,int bsz ,const char    *title = 0);
   LPITEMIDLIST    __fastcall SelectPrinters(wchar_t * buffer ,int bsz ,const wchar_t *title = 0);
   virtual   BOOL  __fastcall OnSelChanged(LPITEMIDLIST){return TRUE;}
             void  __fastcall EnableOk(bool enable){if(hWnd)::SendMessage((::HWND)hWnd,BFFM_ENABLEOK,WPARAM(enable),LPARAM(enable));}
             void  __fastcall SetStatusText(char    * status);
             void  __fastcall SetStatusText(wchar_t * status);
 };

inline   void            __fastcall TFolderBrowser::SetClosure
                        (TClosureMethod<LPVOID,TFolderBrowser*,BOOL>::closure_method cMethod,LPVOID UD)
{
  __on_sel_changed.SetClosure(UD,cMethod,this);
}

inline   LPITEMIDLIST   __fastcall TFolderBrowser::SelectFolder(char    * buffer ,
                                                                int bsz ,
                                                                const char *title,
                                                                bool NewUI,
                                                                bool UseNetwork )

   {
	return DoSelect(
                    BIF_BROWSEFORCOMPUTER|
                    BIF_RETURNFSANCESTORS|
                    BIF_RETURNONLYFSDIRS |( NewUI ? BIF_USENEWUI:0)|
                    (UseNetwork ?0:BIF_DONTGOBELOWDOMAIN) ,
                    CSIDL_DRIVES,
                    buffer,bsz,
                    title);
   }

inline   LPITEMIDLIST   __fastcall TFolderBrowser::SelectFolder(wchar_t *buffer,
                                                                int bsz ,
                                                                const wchar_t *title,
                                                                bool NewUI,
                                                                bool UseNetwork )
   {
    return DoSelect(
                    BIF_BROWSEFORCOMPUTER|
                    BIF_RETURNFSANCESTORS |
                    BIF_RETURNONLYFSDIRS |( NewUI ? BIF_USENEWUI:0)|
                    (UseNetwork ?0:BIF_DONTGOBELOWDOMAIN) ,
                    CSIDL_DRIVES,
                    buffer,bsz,
                    title);
   }

inline LPITEMIDLIST    __fastcall TFolderBrowser::SelectNetwork(char  * buffer ,int bsz ,const char *title)
{
 return DoSelect(BIF_BROWSEFORCOMPUTER|BIF_NEWDIALOGSTYLE,
                 CSIDL_NETWORK,
                 buffer,bsz,title);
}

inline LPITEMIDLIST    __fastcall TFolderBrowser::SelectNetwork(wchar_t  * buffer ,int bsz ,const wchar_t *title)
{
 return DoSelect(BIF_BROWSEFORCOMPUTER|BIF_NEWDIALOGSTYLE,
                 CSIDL_NETWORK,
                 buffer,bsz,title);
}

inline LPITEMIDLIST    __fastcall TFolderBrowser::SelectPrinters(char  * buffer ,int bsz ,const char *title)
{
 return DoSelect(BIF_BROWSEFORPRINTER|BIF_DONTGOBELOWDOMAIN,
                 CSIDL_PRINTERS,
                 buffer,bsz,title);
}

inline LPITEMIDLIST    __fastcall TFolderBrowser::SelectPrinters(wchar_t  * buffer ,int bsz ,const wchar_t *title)
{
 return DoSelect(BIF_BROWSEFORPRINTER|BIF_DONTGOBELOWDOMAIN,
                 CSIDL_PRINTERS,
                 buffer,bsz,title);
}

inline  void  __fastcall TFolderBrowser::SetStatusText(char    * status)
{
 if(hWnd)
   ::SendMessage((::HWND)hWnd,BFFM_SETSTATUSTEXTA,0,LPARAM(status));
}

inline  void  __fastcall TFolderBrowser::SetStatusText(wchar_t * status)
{
 if(hWnd)
   ::SendMessage((::HWND)hWnd,BFFM_SETSTATUSTEXTW,0,LPARAM(status));
}


#ifndef _NO_NAMESPACE
}
#endif

#endif
