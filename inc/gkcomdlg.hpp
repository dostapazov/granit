/*****************  Заголовок  для Common Dialog *****************************/
#include <gkdlgs.hpp>
#include  <commdlg.h>

#ifndef _NO_NAMESPACE
namespace KrnlWin
{
#endif

#pragma pack(push,8)

typedef UINT (APIENTRY * CD_HOOK)(HWND,UINT,WPARAM,LPARAM);



KEWIN_CLASS TOpenDialog:public TDialog
{
 OPENFILENAME  ofn;

 char * FileMask;

 char   _FileMask[MAX_PATH];
 int    _MaskCount;
 int    _MaskLength;
 int    _MaskSize;

 char  * File;

 char   _File    [MAX_PATH*2];
 int    _FileSz;
 int    _FileCount;

 static UINT APIENTRY CDHook(HWND w,UINT msg,WPARAM wp,LPARAM lp);
              void  __fastcall FreeFileMask();
          LRESULT   __fastcall MessageHandler(MSG & msg);
              int   __fastcall OnWMNotify(int CtrlId,LPNMHDR mnhdr);
              void  __fastcall EndModal(int eCode);
     virtual  bool  __fastcall OnFileOK(LPCSTR fName);
     virtual  void  __fastcall OnFolderChange();
     virtual  void  __fastcall OnInitDone();
     virtual  void  __fastcall OnSelChange(LPCSTR fName);
     virtual  DWORD __fastcall OnShareViolation(LPSTR fName);
     virtual  void  __fastcall OnTypeChange(int);
     virtual  void  __fastcall OnHelp();
 public:
              TOpenDialog(HWND w,LPSTR title,DWORD Flags,LPSTR initDir = 0,LPSTR Template = 0,HINSTANCE hInst = GetModuleHandle(0));
     virtual ~TOpenDialog();
              void __fastcall SetFlags(DWORD32 Flags,bool set);
              void __fastcall SetFileNameBuffSize(char * buffer,int sz);
              void __fastcall AddFileMask(char * Descr,char * mask);
              int  __fastcall ShowModal();
              void __fastcall ClearFileMask(){FreeFileMask();}
              bool __fastcall GetFileName(int idx,char * buff,int bsz);
              int  __fastcall GetFileCount(){return _FileCount;}

};

#pragma pack(pop)

#ifndef _NO_NAMESPACE
}//end of namespace
#endif

