//---------------------------------------------------------------------------
#pragma warn -8058
#include <vcl.h>
#pragma warn .8058
#pragma hdrstop

#include "Property_frm.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFrmProp *FrmProp;
//---------------------------------------------------------------------------
__fastcall TFrmProp::TFrmProp(TComponent* Owner, AnsiString name, DWORD size, TViews *v)
        : TForm(Owner)
{
  LName->Text=name;
  LMaxSize->Text=IntToStr((int)size/1024);
  LFile->Text=v->name;
  LRecs->Text=IntToStr(v->count);
  LSize->Text=IntToStr(v->log->GetSize())+" байт";
}
//---------------------------------------------------------------------------

DWORD __fastcall TFrmProp::GetSize()
{
  return LMaxSize->Text.ToInt()*1024;
}

void __fastcall TFrmProp::FormCloseQuery(TObject *Sender, bool &CanClose)
{
  if (ModalResult==mrCancel) return;
  try
  {
    DWORD sz=LMaxSize->Text.ToInt();
    if (sz && sz<10)
    {
      CanClose=false;
      ShowMessage("Размер файла событий должен быть более 10 кБ");
    }
  }
  catch (...)
  {
    CanClose=false;
  }
}
//---------------------------------------------------------------------------

