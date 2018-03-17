//---------------------------------------------------------------------------

#ifndef tlm_viewer_mainformH
#define tlm_viewer_mainformH
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
        void __fastcall FormShow(TObject *Sender);
private:	// User declarations
public:		// User declarations
        __fastcall TTlmViewerMainForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TTlmViewerMainForm *TlmViewerMainForm;
//---------------------------------------------------------------------------
#endif
