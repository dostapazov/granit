---------------------------------------------------------------------------

#pragma hdrstop

#include <vcl.h>
#include "text_editor.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TTextEditor *TextEditor;
//---------------------------------------------------------------------------
__fastcall TTextEditor::TTextEditor(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TTextEditor::ToolButton1Click(TObject *Sender)
{
 ModalResult = mrOk;
}
//---------------------------------------------------------------------------
void __fastcall TTextEditor::ToolButton2Click(TObject *Sender)
{
  ModalResult = mrCancel;
}
//---------------------------------------------------------------------------

