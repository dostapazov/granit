//---------------------------------------------------------------------------

#pragma warn -8058
#include <vcl.h>
#pragma warn .8058
#pragma hdrstop

#include "sel_folder_dlg.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TSelectFolderDlg *SelectFolderDlg;
//---------------------------------------------------------------------------
__fastcall TSelectFolderDlg::TSelectFolderDlg(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------


AnsiString __fastcall TSelectFolderDlg::GetFolder()
{
        return DirectoryListBox1->Directory;
}
void __fastcall TSelectFolderDlg::BitBtn3Click(TObject *Sender)
{
  AnsiString str = Folder;
  str+='\\';
  str+=Edit1->Text;
  ForceDirectories(str);
  DirectoryListBox1->Update();
}
//---------------------------------------------------------------------------
