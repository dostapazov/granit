//---------------------------------------------------------------------------
#pragma hdrstop
#include <vcl.h>
#include "activation_form.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TActivationForm *ActivationForm;
//---------------------------------------------------------------------------
__fastcall TActivationForm::TActivationForm(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
