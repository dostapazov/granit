//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "fa_props.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "base_props"
#pragma resource "*.dfm"
TFaProps *FaProps;
//---------------------------------------------------------------------------
__fastcall TFaProps::TFaProps(TComponent* Owner)
        : TBaseProps(Owner)
{
}
//---------------------------------------------------------------------------

void    __fastcall TFaProps::setup(otd_proto * op,bool create,lpotd_addr own_addr )
{
 TBaseProps::setup(op,create,own_addr);
 if(create)
 {
 }
 else
 {
  FaType->ItemIndex = (op->addr->fa&0x7);
  FaType->Enabled   = false; 
 }
}
void __fastcall TFaProps::FaTypeClick(TObject *Sender)
{
 Number->Text = FaType->ItemIndex;
 call_changes_notify(true);
}
//---------------------------------------------------------------------------

