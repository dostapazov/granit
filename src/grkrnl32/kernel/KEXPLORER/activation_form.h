//---------------------------------------------------------------------------

#ifndef activation_formH
#define activation_formH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.Buttons.hpp>
//---------------------------------------------------------------------------
class TActivationForm : public TForm
{
__published:	// IDE-managed Components
	TEdit *ActivationCode;
	TLabel *Label1;
	TBitBtn *BitBtn1;
	TBitBtn *BitBtn2;
private:	// User declarations
public:		// User declarations
	__fastcall TActivationForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TActivationForm *ActivationForm;
//---------------------------------------------------------------------------
#endif
