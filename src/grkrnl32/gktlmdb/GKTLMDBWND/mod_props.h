//---------------------------------------------------------------------------

#ifndef mod_propsH
#define mod_propsH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "base_props.h"
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TModProps : public TBaseProps
{
__published:	// IDE-managed Components
        TRadioGroup *DataType;
        TRadioGroup *PDiag;
        TRadioGroup *PCMask;
        void __fastcall DataTypeClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
        __fastcall TModProps(TComponent* Owner);
void    __fastcall setup(otd_proto * op,bool create = false,lpotd_addr own_addr = NULL);
bool    __fastcall get_create_data(LPTLMDB_MOD_CREATE mc,WideString & name);

};
//---------------------------------------------------------------------------
extern PACKAGE TModProps *ModProps;
//---------------------------------------------------------------------------
#endif
