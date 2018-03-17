//---------------------------------------------------------------------------
#pragma hdrstop
#include "gkawpmodule.hpp"
//#include <vcl.h>
#include "arch_list.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TArchList *ArchList;
//---------------------------------------------------------------------------
__fastcall TArchList::TArchList(TComponent* Owner)
	: TForm(Owner)
{
 Constraints->MaxWidth = Width;
 Constraints->MinWidth = Width;
}
//---------------------------------------------------------------------------


    UnicodeString __fastcall TArchList::get_arch_name()
    {
     UnicodeString ret;
     TListItem * item = this->ListView1->Selected;
     if(item)
       ret = item->SubItems->Strings[2];
     return ret;
    }

    bool          __fastcall TArchList::get_protect  ()
    {
     bool ret = false;
     TListItem * item = this->ListView1->Selected;
     if(item)
       ret = item->SubItems->Strings[1].Length() ? true : false;
     return ret;
    }


    void          __fastcall TArchList::set_arch_name(UnicodeString text)
    {
     TListItem * item = this->ListView1->Selected;
     if(item)
        item->SubItems->Strings[2] = text;

    }

    void          __fastcall TArchList::set_protect  (bool prot)
    {
     TListItem * item = this->ListView1->Selected;
     if(item)
        item->SubItems->Strings[1] =  prot ? L"*":L"";

    }


void __fastcall TArchList::ListView1DblClick(TObject *Sender)
{
  if(ViewBtn->OnClick) ViewBtn->OnClick(Sender);
}
//---------------------------------------------------------------------------



