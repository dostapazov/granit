//---------------------------------------------------------------------------

#ifndef tarch_propsH
#define tarch_propsH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.Buttons.hpp>
//---------------------------------------------------------------------------
class TArchProps : public TForm
{
__published:	// IDE-managed Components
	TLabel *Label1;
	TEdit *Edit1;
	TCheckBox *CheckBox1;
	TBitBtn *BitBtn1;
	TBitBtn *BitBtn2;
private:	// User declarations
public:		// User declarations
	__fastcall TArchProps(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TArchProps *ArchProps;
//---------------------------------------------------------------------------
#endif
