//---------------------------------------------------------------------------

#ifndef Property_frmH
#define Property_frmH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include "ReadLogThread.h"
//---------------------------------------------------------------------------
class TFrmProp : public TForm
{
__published:	// IDE-managed Components
        TLabeledEdit *LName;
        TLabeledEdit *LFile;
        TLabeledEdit *LMaxSize;
        TLabeledEdit *LRecs;
        TLabeledEdit *LSize;
        TBitBtn *BitBtn1;
        TBitBtn *BitBtn2;
        TLabel *Label1;
        void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
private:	// User declarations
public:		// User declarations
        __fastcall TFrmProp(TComponent* Owner, AnsiString name, DWORD size, TViews *v);
        DWORD __fastcall GetSize();
};
//---------------------------------------------------------------------------
extern PACKAGE TFrmProp *FrmProp;
//---------------------------------------------------------------------------
#endif
