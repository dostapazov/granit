//---------------------------------------------------------------------------

#ifndef paint_propsH
#define paint_propsH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TPaintProps : public TForm
{
__published:	// IDE-managed Components
	TLabel *Label1;
	TComboBox *ComboBox1;
	TColorBox *BcColor;
	TStaticText *StaticText1;
	TStaticText *StaticText2;
	TColorBox *ColorBox1;
	void __fastcall FormDeactivate(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TPaintProps(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TPaintProps *PaintProps;
//---------------------------------------------------------------------------
#endif
