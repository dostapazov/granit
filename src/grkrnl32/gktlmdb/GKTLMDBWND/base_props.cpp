//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "base_props.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TBaseProps *BaseProps;
//---------------------------------------------------------------------------
__fastcall TBaseProps::TBaseProps(TComponent* Owner)
        : TFrame(Owner)
{
 on_prop_changes = NULL;
}
//---------------------------------------------------------------------------


void __fastcall TBaseProps::setup(lpotd_proto  op,bool create,lpotd_addr own_addr)
{
  wchar_t text[MAX_PATH];

  addr = sotd_addr(create ? *own_addr : *op->addr);
  if(create)
  {
  }
  else
  {
  this->Number->Text = int(addr.number());
  int first_num = 0;
  int obj_count = 0;
  if(op->data)
  {
   first_num = (int)DWORD(op->data->numbers.loN);
   obj_count = (int)OTD_GET_COUNT(&op->data->numbers);
  }
  if(op->name)
   otd_text_getwstr(op->name,text,MAX_PATH);
   else
   *text = 0;
  ObjFirstNumber->Text = first_num;
  ObjCount->Position = obj_count;
  ObjFirstNumberChange(this);
  Name->Text = text;
  }
}


void __fastcall TBaseProps::NumberChange(TObject *Sender)
{
 call_changes_notify(true);       
}
//---------------------------------------------------------------------------

void __fastcall TBaseProps::ObjFirstNumberChange(TObject *Sender)
{
  if(addr.addrtype()==OTD_ADDR_TYPE_CP)
  {
    int num = atoi(AnsiString(ObjFirstNumber->Text).c_str());
    int new_num = KeRTL::MIN(num,3);
    if(new_num!=num)
    {
      ObjFirstNumber->Text = new_num;
    }
    ObjCount->Max= 4-new_num;
    Edit1->Text = ObjCount->Position;

  }
  else
  ObjCount->Max=254;
  call_changes_notify(true);  
}
//---------------------------------------------------------------------------

