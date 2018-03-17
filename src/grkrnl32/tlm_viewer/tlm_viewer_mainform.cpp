//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "tlm_viewer_mainform.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TTlmViewerMainForm *TlmViewerMainForm;
//---------------------------------------------------------------------------
__fastcall TTlmViewerMainForm::TTlmViewerMainForm(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TTlmViewerMainForm::FormShow(TObject *Sender)
{
  Top  = 0;
  Left = 0;
  Width = Screen->Width;
}
//---------------------------------------------------------------------------
