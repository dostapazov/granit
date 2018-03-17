//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "mod_props.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "base_props"
#pragma resource "*.dfm"
TModProps *ModProps;
//---------------------------------------------------------------------------
__fastcall TModProps::TModProps(TComponent* Owner)
        : TBaseProps(Owner)
{
}
//---------------------------------------------------------------------------

int   __fastcall dtype2idx(DWORD type)
{
 switch(type)
 {
  case OTD_DISCRETE    : return 0;
  case OTD_ANALOG_BYTE : return 1;
  case OTD_ANALOG_WORD : return 2;
  case OTD_ANALOG_DWORD: return 3;
  case OTD_ANALOG_QWORD: return 4;
 }
 return 0;
}

DWORD __fastcall idx2dtype(int idx)
{
 switch(idx)
 {
  case 0: return OTD_DISCRETE    ;
  case 1: return OTD_ANALOG_BYTE ;
  case 2: return OTD_ANALOG_WORD ;
  case 3: return OTD_ANALOG_DWORD;
  case 4: return OTD_ANALOG_QWORD;
 }
 return OTD_ANALOG_BYTE;
}

void    __fastcall TModProps::setup(otd_proto * op,bool create ,lpotd_addr own_addr )
{
 TBaseProps::setup(op,create,own_addr);
 if(create)
 {
 }
 else
 {
  DataType->ItemIndex = dtype2idx(op->data->otd_type);
  PDiag->ItemIndex    = (op->personal_diag)   ? dtype2idx(op->personal_diag->otd_type)  :0 ;
  PCMask->ItemIndex   = (op->personal_chmask) ? dtype2idx(op->personal_chmask->otd_type):0 ;

 }
}

bool    __fastcall TModProps::get_create_data(LPTLMDB_MOD_CREATE mc,WideString & name)
{
 if(TBaseProps::get_create_data(mc,name))
 {
  mc->type_data = idx2dtype(DataType->ItemIndex);
  if(PDiag->ItemIndex)
    {
     mc->type_pdiag = idx2dtype(PDiag->ItemIndex);
     mc->parts|= OTD_PROTO_PART_PERSONAL_DIAG;
    }
  if(PCMask->ItemIndex)
    {
     mc->type_pchmask = idx2dtype(PCMask->ItemIndex);
     mc->parts|= OTD_PROTO_PART_PERSONAL_CHMASK;
    }
  return true;
 }
 return false;
}

void __fastcall TModProps::DataTypeClick(TObject *Sender)
{
  this->call_changes_notify(true);      
}
//---------------------------------------------------------------------------

