//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "LayoutEdit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TLaoutEdit *LaoutEdit;
//---------------------------------------------------------------------------
__fastcall TLaoutEdit::TLaoutEdit(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TLaoutEdit::ListView1SelectItem(TObject *Sender,
      TListItem *Item, bool Selected)
{
  BUp->Enabled  = Selected;
  BDn->Enabled  = Selected;
  BDel->Enabled = Selected;
}
//---------------------------------------------------------------------------
void __fastcall TLaoutEdit::BDelClick(TObject *Sender)
{
  ListView1->Selected->Checked = !ListView1->Selected->Checked;
}
//---------------------------------------------------------------------------
void __fastcall TLaoutEdit::BUpClick(TObject *Sender)
{
  TListItem * item = ListView1->Selected;
  if(item->Index>0)
  {
   TListItem * n_item = ListView1->Items->Insert(item->Index-1);
   n_item->Data = item->Data;
   n_item->Caption = item->Caption;
   n_item->SubItems->Add(item->SubItems->Strings[0]);
   n_item->Checked = item->Checked;
   n_item->Selected = true;
   n_item->Focused  = true;
   item->Delete();
  }



}
//---------------------------------------------------------------------------
void __fastcall TLaoutEdit::BDnClick(TObject *Sender)
{
  TListItem * item = ListView1->Selected;
  if(item->Index<ListView1->Items->Count-1)
  {
   item = ListView1->Items->Item[item->Index+1];
   TListItem * n_item = ListView1->Items->Insert(item->Index-1);
   n_item->Data = item->Data;
   n_item->Caption = item->Caption;
   n_item->SubItems->Add(item->SubItems->Strings[0]);
   n_item->Checked = item->Checked;
   item->Delete();
  }
}
//---------------------------------------------------------------------------
