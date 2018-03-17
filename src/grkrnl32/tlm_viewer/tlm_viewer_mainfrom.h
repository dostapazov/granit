//---------------------------------------------------------------------------

#ifndef tlm_viewer_mainfromH
#define tlm_viewer_mainfromH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
class TTlmViewerMainForm : public TForm
{
__published:	// IDE-managed Components
        TLabel *Label1;
private:	// User declarations
public:		// User declarations
        __fastcall TTlmViewerMainForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TTlmViewerMainForm *TlmViewerMainForm;
//---------------------------------------------------------------------------
#endif
