//---------------------------------------------------------------------------

#ifndef sel_div_prefH
#define sel_div_prefH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.Buttons.hpp>
//---------------------------------------------------------------------------
class TSelItemDivPref : public TForm
{
__published:	// IDE-managed Components
	TLabel *Label1;
	TEdit *Edit1;
	TLabel *Label2;
	TEdit *Edit2;
	TBitBtn *BitBtn1;
	TBitBtn *BitBtn2;
private:	// User declarations
public:		// User declarations
	__fastcall TSelItemDivPref(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TSelItemDivPref *SelItemDivPref;
//---------------------------------------------------------------------------
#endif
