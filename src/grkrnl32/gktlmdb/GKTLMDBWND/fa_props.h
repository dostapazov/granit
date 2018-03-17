//---------------------------------------------------------------------------

#ifndef fa_propsH
#define fa_propsH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "base_props.h"
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TFaProps : public TBaseProps
{
__published:	// IDE-managed Components
        TRadioGroup *FaType;
        void __fastcall FaTypeClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
        __fastcall TFaProps(TComponent* Owner);
void    __fastcall setup(otd_proto * op,bool create = false,lpotd_addr own_addr = NULL);        
};
//---------------------------------------------------------------------------
extern PACKAGE TFaProps *FaProps;
//---------------------------------------------------------------------------
#endif
