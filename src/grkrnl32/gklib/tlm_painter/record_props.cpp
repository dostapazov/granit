//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "record_props.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TRecordProps *RecordProps;
//---------------------------------------------------------------------------
__fastcall TRecordProps::TRecordProps(TComponent* Owner)
	: TForm(Owner),Flp_rec(NULL)
{

}
//---------------------------------------------------------------------------
void __fastcall TRecordProps::set_record(LPMDB_RECORD _lp_rec)
{
  if(Flp_rec != _lp_rec )
  {
    Flp_rec = _lp_rec;
    update_view(PropsTree->Selected);
  }
}

void __fastcall TRecordProps::update_view(TTreeNode * node)
{
   if(node)
   {

   }
}
