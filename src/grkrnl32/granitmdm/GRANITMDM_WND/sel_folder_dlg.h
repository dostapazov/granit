//---------------------------------------------------------------------------

#ifndef sel_folder_dlgH
#define sel_folder_dlgH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <FileCtrl.hpp>
//---------------------------------------------------------------------------
class TSelectFolderDlg : public TForm
{
__published:	// IDE-managed Components
        TDriveComboBox *DriveComboBox1;
        TDirectoryListBox *DirectoryListBox1;
        TLabel *Label1;
        TBitBtn *BitBtn1;
        TBitBtn *BitBtn2;
        TBitBtn *BitBtn3;
        TEdit *Edit1;
        void __fastcall BitBtn3Click(TObject *Sender);
private:
        AnsiString __fastcall GetFolder();	// User declarations
public:		// User declarations
        __fastcall TSelectFolderDlg(TComponent* Owner);
        __property AnsiString Folder  = { read=GetFolder };
};
//---------------------------------------------------------------------------
extern PACKAGE TSelectFolderDlg *SelectFolderDlg;
//---------------------------------------------------------------------------
#endif
