//---------------------------------------------------------------------------

#ifndef about_common_frameH
#define about_common_frameH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
//---------------------------------------------------------------------------
class TAboutCommonFrame : public TFrame
{
__published:	// IDE-managed Components
	TMemo *Memo2;
private:	// User declarations
public:		// User declarations
	__fastcall TAboutCommonFrame(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TAboutCommonFrame *AboutCommonFrame;
//---------------------------------------------------------------------------
#endif
