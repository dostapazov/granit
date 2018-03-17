//---------------------------------------------------------------------------
#pragma hdrstop
#include <vcl.h>
#include "login_dlg.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TLoginDlg *LoginDlg;
//---------------------------------------------------------------------------
__fastcall TLoginDlg::TLoginDlg(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TLoginDlg::FormHide(TObject *Sender)
{
  user_passw->Text = UnicodeString();
}
//---------------------------------------------------------------------------
void __fastcall TLoginDlg::bCancelClick(TObject *Sender)
{
  Visible = false;
}
//---------------------------------------------------------------------------

void __fastcall TLoginDlg::FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)

{
 switch(Key)
 {
   case VK_RETURN:
        bLogin->OnClick(bLogin);
        break;
   case VK_ESCAPE:
        Visible = false;
        break;
 }


}
//---------------------------------------------------------------------------

