//---------------------------------------------------------------------------

#ifndef gkiupmodem_frmH
#define gkiupmodem_frmH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "gkmodule_form.h"
//---------------------------------------------------------------------------
class TGkIPModemFrm : public TGKModuleForm
{
__published:	// IDE-managed Components
private:	// User declarations
public:		// User declarations
        __fastcall TGkIPModemFrm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TGkIPModemFrm *GkIPModemFrm;
//---------------------------------------------------------------------------
#endif
