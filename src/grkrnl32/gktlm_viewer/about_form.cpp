//---------------------------------------------------------------------------

#pragma hdrstop
#include <vcl.h>

#include "about_form.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "about_common_frame"
#pragma resource "*.dfm"
TAboutForm *AboutForm;
//---------------------------------------------------------------------------
__fastcall TAboutForm::TAboutForm(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TAboutForm::FormDeactivate(TObject *Sender)
{
 Close();
}
//---------------------------------------------------------------------------
void __fastcall TAboutForm::BitBtn1Click(TObject *Sender)
{
  Close();
}
//---------------------------------------------------------------------------

