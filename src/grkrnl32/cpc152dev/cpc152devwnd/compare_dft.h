//---------------------------------------------------------------------------

#ifndef compare_dftH
#define compare_dftH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ComCtrls.hpp>
//---------------------------------------------------------------------------
class TCompareDFT : public TForm
{
__published:	// IDE-managed Components
	TListView *ListView1;
	TStatusBar *StatusBar1;
private:	// User declarations
public:		// User declarations
	__fastcall TCompareDFT(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TCompareDFT *CompareDFT;
//---------------------------------------------------------------------------
#endif
