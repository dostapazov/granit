//---------------------------------------------------------------------------

#ifndef login_dlgH
#define login_dlgH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.Buttons.hpp>
#include <Vcl.ComCtrls.hpp>
//---------------------------------------------------------------------------
class TLoginDlg : public TForm
{
__published:	// IDE-managed Components
	TLabel *Label1;
	TLabel *Label2;
	TEdit *user_name;
	TEdit *user_passw;
	TBitBtn *bLogin;
	TBitBtn *bCancel;
	TStatusBar *StatusBar1;
	void __fastcall FormHide(TObject *Sender);
	void __fastcall bCancelClick(TObject *Sender);
	void __fastcall FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
private:	// User declarations
public:		// User declarations
	__fastcall TLoginDlg(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TLoginDlg *LoginDlg;
//---------------------------------------------------------------------------
#endif
