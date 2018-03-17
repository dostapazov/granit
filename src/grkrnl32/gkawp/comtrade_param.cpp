//---------------------------------------------------------------------------

#pragma hdrstop
#include <vcl.h>
#include "comtrade_param.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TComtradeParam *ComtradeParam;
//---------------------------------------------------------------------------
__fastcall TComtradeParam::TComtradeParam(TComponent* Owner)
	: TForm(Owner)
{

}
//---------------------------------------------------------------------------
void __fastcall TComtradeParam::ctNameChange(TObject *Sender)
{
  if(Sender == ctName)
     ctFile->Text = ctName->Text;
  bOk->Enabled = !ctName->Text.IsEmpty() && !ctFreq->Text.IsEmpty() && !ctFreqSample->Text.IsEmpty() && !ctFile->Text.IsEmpty()
  ? true : false;

}
//---------------------------------------------------------------------------
void __fastcall TComtradeParam::bSelProgramClick(TObject *Sender)
{
  OpenDialog1->FileName = ProgramName->Text;
  if(OpenDialog1->Execute(Handle))
  {
    ProgramName->Text = OpenDialog1->FileName;
  }

}
//---------------------------------------------------------------------------

