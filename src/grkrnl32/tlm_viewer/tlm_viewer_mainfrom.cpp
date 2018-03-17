//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "tlm_viewer_mainfrom.h"
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
