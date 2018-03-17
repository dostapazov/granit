#include <gkcomdlg.hpp>
#pragma hdrstop
//#include <commdlg.h>


#ifndef _NO_NAMESPACE
namespace KrnlWin
{
#endif

/* NOTE 

  Т.к. нет Platform SDK under W2K
  чтобы работало нормально необходимо в commdlg.h добавить в описание OPENFILENAME
  следующие поля:
#if (_WIN32_WINNT >= 0x0500)
   void *       pvReserved;
   DWORD        dwReserved;
   DWORD        FlagsEx;
#endif // (_WIN32_WINNT >= 0x0500)

*/

UINT APIENTRY TOpenDialog::CDHook(HWND w,UINT msg,WPARAM wp,LPARAM lp)
{

#ifdef _WIN64
  TOpenDialog * dlg = dynamic_cast<TOpenDialog*>(CAST(LPVOID(::GetWindowLongPtr((::HWND)w,GWL_USERDATA))));
#else
  TOpenDialog * dlg = dynamic_cast<TOpenDialog*>(CAST(LPVOID(::GetWindowLong((::HWND)w,GWL_USERDATA))));
#endif

  if(msg == WM_INITDIALOG)
   {
     LPOPENFILENAME  ofn =(LPOPENFILENAME)lp;
     DlgProc(w,msg,wp,LPARAM(ofn->lCustData));
#ifdef _WIN64
     dlg = dynamic_cast<TOpenDialog*>(CAST(LPVOID(::GetWindowLongPtr((::HWND)w,GWL_USERDATA))));
#else
     dlg = dynamic_cast<TOpenDialog*>(CAST(LPVOID(::GetWindowLong((::HWND)w,GWL_USERDATA))));
#endif
     if(dlg)
     {
      dlg->ModalDialog = true;
     }
     return true;
   }

   if(dlg)
   {

   }
   return FALSE;
}

void __fastcall TOpenDialog::EndModal(int /*eCode*/)
{
 PostMessage(WM_COMMAND,0,IDCANCEL);
}


TOpenDialog::TOpenDialog(HWND w,LPSTR  title,DWORD Flags,LPSTR  initDir,LPSTR Template,HINSTANCE hInst):
TDialog(w,Template,hInst)
{
 FileMask = _FileMask;
 File = _File;
 FreeFileMask();
 ZeroMemory(&ofn,sizeof(ofn));
 ofn.lStructSize = sizeof(ofn);
 //Если Win2000 то не уменьшать
 if(KeRTL::TSysInfo::GetMajorVersion()<5)
 ofn.lStructSize-=(sizeof(LPVOID)+2*sizeof(DWORD));
 //#endif
 ofn.hwndOwner      = (::HWND)w;
 ofn.hInstance      = (::HINSTANCE)hInst;
 ofn.lpTemplateName = Template;
 ofn.Flags          = Flags|OFN_EXPLORER;
 ofn.lpstrInitialDir= initDir;
 ofn.lpstrTitle     = title;
 ofn.lpstrFile      = this->File;
 ofn.nMaxFile       = sizeof(_File);
 SetFileNameBuffSize(this->_File,sizeof(_File));
}

TOpenDialog::~TOpenDialog()
{FreeFileMask();}

void __fastcall TOpenDialog::FreeFileMask()
{
 if(FileMask!=_FileMask)
   {
    delete [] FileMask;
    FileMask = _FileMask;
    _MaskCount =_MaskLength =_MaskSize =0;
   }
   _MaskCount =_MaskLength =0;
   _MaskSize  = sizeof(_FileMask);
}

void __fastcall TOpenDialog::SetFlags(DWORD32 Flags,bool set)
{
 if(set)
   ofn.Flags|=Flags;
   else
   ofn.Flags&=~Flags;
}

void __fastcall TOpenDialog::SetFileNameBuffSize(char * buffer,int sz)
{
 this->File    = buffer;
 this->_FileSz = sz;
}

void __fastcall TOpenDialog::AddFileMask(char * Descr,char * mask)
{
 int len = lstrlenA(Descr)+lstrlenA(mask)+2;
 if(len+1+this->_MaskLength>=this->_MaskSize) //+1 обязательно для двойного нуля
   {
    // Перевыделение маски
    int    NewMaskSize=_MaskSize*2;
    char * NewMask = new char[NewMaskSize];
    memcpy(NewMask,FileMask,_MaskSize+1);
    if(FileMask!=this->_FileMask)
      delete FileMask;
       FileMask   = NewMask;
      _MaskSize   = NewMaskSize;
      AddFileMask(Descr,mask);
   }
   lstrcpyA(FileMask+_MaskLength,Descr);
   lstrcpyA(FileMask+_MaskLength+lstrlenA(Descr)+1,mask);
   _MaskLength+=len;
   FileMask[_MaskLength]=0;
   _MaskCount++;
}


int  __fastcall TOpenDialog::ShowModal()
{
 *File = 0;
 if(ofn.lpTemplateName!=0)
   ofn.Flags|= OFN_ENABLETEMPLATE;

 SetFlags(OFN_ENABLEHOOK,true);
 SetFlags(OFN_EXPLORER,true);

 ofn.lpfnHook  = (LPOFNHOOKPROC)CDHook;
 ofn.lCustData = (LONG)this;
 ofn.lpstrFilter = this->FileMask;
 ofn.lpstrFile   = this->File;
 this->_FileCount = 0;
 BOOL ret = GetOpenFileName(&ofn);
 if(ret)
  {
   //Подсчет кол-ва файлов
   LPSTR  str = this->File+ofn.nFileOffset;
   int len;
   do{
      len = lstrlen(str);
      str+=len+1;
      if(len) _FileCount++;
     }while(len && ofn.Flags&OFN_ALLOWMULTISELECT);
  }
 return (ret) ? IDOK:IDCANCEL;
}

LRESULT  __fastcall TOpenDialog::MessageHandler(MSG & msg)
{
  if(ofn.Flags&OFN_EXPLORER)
   return TDialog::MessageHandler(msg);
   TDialog::MessageHandler(msg);
   return 0;
}

int   __fastcall TOpenDialog::OnWMNotify(int /*CtrlId*/,LPNMHDR mnhdr)
{
 LPOFNOTIFY  on = (LPOFNOTIFY)mnhdr;
 switch(on->hdr.code)
 {
  case CDN_FILEOK      :       if(!OnFileOK(on->pszFile))
                                {
                                 SetWindowLong(DWL_MSGRESULT,1);
                                 return 1;
                                }
                                break;
  case CDN_FOLDERCHANGE:       OnFolderChange();break;
  case CDN_TYPECHANGE  :       OnTypeChange(on->lpOFN->nFilterIndex);break;
  case CDN_SELCHANGE   :       OnSelChange(on->pszFile);break;
  case CDN_INITDONE    :       OnInitDone() ;break;
  case CDN_SHAREVIOLATION:     return OnShareViolation(on->pszFile);
  case CDN_HELP        :       OnHelp();break;  
 }
 return 0;
}

 bool __fastcall TOpenDialog::GetFileName(int idx,char * buff,int bsz)
 {
  //Взять имя файла
  if(buff && bsz)
  {
    *buff = 0;
    if(idx<_FileCount)
    {
      char f[MAX_PATH];
      bool Multy = _FileCount >1 ? true:false;
      char * str = _File;
      lstrcpy(f,_File);
      if(Multy)
      {
        str+=ofn.nFileOffset;
        int i(0);
        while(i<idx)
        {
         str+=lstrlen(str)+1;
         i++;
        }
        lstrcat(f,"\\");
        lstrcat(f,str);
      }
      if(lstrlenA(f)<bsz)
         lstrcpyA(buff,f);
    }
	return lstrlenA(buff) ? true:false; 
  }
  return false;
 }


 bool  __fastcall TOpenDialog::OnFileOK(LPCSTR /*fName*/)
 {
   return true;
 }

 void  __fastcall TOpenDialog::OnFolderChange( )
 {}
 void  __fastcall TOpenDialog::OnInitDone()
 {}
 void  __fastcall TOpenDialog::OnSelChange(LPCSTR /*fName*/)
 {}
 DWORD __fastcall TOpenDialog::OnShareViolation(LPSTR /*fName*/)
 {return 0;}
 void  __fastcall TOpenDialog::OnTypeChange(int /*t_idx*/)
 {}
 void  __fastcall TOpenDialog::OnHelp()
 {}

 


#ifndef _NO_NAMESPACE
}//endof namespace
#endif
