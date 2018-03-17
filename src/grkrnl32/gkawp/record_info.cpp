//---------------------------------------------------------------------------
#pragma hdrstop
#include <vcl.h>
#include "record_info.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TRecordInfo *RecordInfo = NULL;
//---------------------------------------------------------------------------
__fastcall TRecordInfo::TRecordInfo(TComponent* Owner)
	: TForm(Owner)
{

}
//---------------------------------------------------------------------------
void __fastcall TRecordInfo::FormDestroy(TObject *Sender)
{
  if(RecordInfo == this) RecordInfo = NULL;
}
//---------------------------------------------------------------------------
void __fastcall TRecordInfo::FormShow(TObject *Sender)
{
  int         idx = 0;
  TListItem * item = get_item(idx++);
  UnicodeString str;
  while(item)
   {
     if(!item->SubItems->Count)
        item->SubItems->Add(str);
     item = get_item(idx++);
   }
}
//---------------------------------------------------------------------------

void __fastcall TRecordInfo::FormClose(TObject *Sender, TCloseAction &Action)
{
  Action = caFree;
}
//---------------------------------------------------------------------------



