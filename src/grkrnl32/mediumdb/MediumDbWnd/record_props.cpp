//---------------------------------------------------------------------------
#pragma hdrstop
#include <vcl.h>
#include "record_props.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "gkmodule_form"
#pragma link "record_frame"
#pragma resource "*.dfm"
TRecordProps *RecordProps;
//---------------------------------------------------------------------------
__fastcall TRecordProps::TRecordProps(TComponent* Owner)
	: TGKModuleForm(Owner)
{
}
//---------------------------------------------------------------------------

__fastcall TRecordProps::TRecordProps(TComponent* Owner,GKHANDLE handle)
	: TGKModuleForm(Owner,NULL,handle)
{
  RecordFrame1->gk_handle = handle;
  RecordFrame1->Align = alClient;
}


void __fastcall TRecordProps::FormDeactivate(TObject *Sender)
{
  Close();
}
//---------------------------------------------------------------------------
void __fastcall TRecordProps::SpeedButton1Click(TObject *Sender)
{
  RecordFrame1->apply_changes(RecordFrame1->apply_mask);
  RecordFrame1->apply_mask = 0;
}
//---------------------------------------------------------------------------
void __fastcall TRecordProps::SpeedButton2Click(TObject *Sender)
{
  if(GKH_RET_SUCCESS == RecordFrame1->apply_changes(RecordFrame1->apply_mask))
     Close();
}
//---------------------------------------------------------------------------

