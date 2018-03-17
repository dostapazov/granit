//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "test_form.h"
#include "paint_var_constructor.h"
#include <boost/bind.hpp>
#include <boost/function.hpp>

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "tlm_painter_form"
#pragma resource "*.dfm"
TTlmTestFrom *TlmTestFrom;
//---------------------------------------------------------------------------
__fastcall TTlmTestFrom::TTlmTestFrom(TComponent* Owner)
	: TTlmPainterForm(Owner)
{
  this->Feditable = true;
  tlmpaint::painter_dll * pd = tlmpaint::painter_dll::get_instance();
  //std::wstring lib_name(L"tlmobj_std_painter.dll");
  std::wstring lib_name(L"tlmobj_painter.dll");
  pd->load_library(lib_name);
  paint_context =  pd->paint_context_create();
  wchar_t name [256];
  pd->enum_discretes_ex(0,0,name,KERTL_ARRAY_COUNT(name));
  pd->enum_discretes_ex(0,1,name,KERTL_ARRAY_COUNT(name));
  boost::bind(&TTlmTestFrom::init,this)();
}

void  TTlmTestFrom::init()
{
  single_kadr  & Fkadr = *new single_kadr;
  Fkadr.kadr =  mdb_kadr(1,-1,-1);
  kadr_entryes_table::iterator ptr;
  for(int i = 1;i<5;i++)
    {
      ptr = Fkadr.add(10+i,i,150*i,50,32,32,RGB(255,255,0),RGB(0,0,255));
      ptr->options.paint_as    = 2; //2 - (i&1);
      ptr->options.paint_flags = 5-i;

    }
  for(int i = 1;i<5;i++)
      {
       Fkadr.add(i,-1,50*i,150,45,45,RGB(255,255,0),RGB(0,255,0))->options.paint_as = 2;
      }

  for(int i = 1;i<5;i++)
      {
        ptr = Fkadr.add(i+120,-1,100*i,250,90,30,RGB(0,255,255));
        ptr->options.prec = 2;
        if(i == 1)
          {
           ptr->options.paint_as = 0;
           ptr->options.bkgnd_color  = clBlack;
           ptr->options.color        = clGreen;
          }
        if(i == 2)
            ptr->options.paint_as = 1;
        if(i == 3)
        {
            ptr->options.paint_as    = 2;
            ptr->options.paint_flags = PAINT_FLAG_SHORT_NAME_BOTTOM;
        }

        if(i == 4)
        {
            ptr->options.paint_as    = 0;
            ptr->options.paint_flags = PAINT_FLAG_SHORT_NAME_TOP;
        }
      }
  Fkadr.position_index_enable(true);
  Fkadr.kadr.kadr_size.cx = this->paint_area->ClientWidth-10;
  Fkadr.kadr.kadr_size.cy = this->paint_area->ClientHeight+5;
  this->set_kadr(&Fkadr);
}
//---------------------------------------------------------------------------
void __fastcall TTlmTestFrom::N7Click(TObject *Sender)
{
   //Установка режима фонового изображения
  TMenuItem *item = dynamic_cast<TMenuItem *>(Sender);
  if(item)
      this->set_bk_mode(tlmpaint::TBmpBkgnd::bkgnd_mode_t( item->Tag));
}
//---------------------------------------------------------------------------
void __fastcall TTlmTestFrom::ToolButton1Click(TObject *Sender)
{
  if(OpenPictureDialog1->Execute(Handle))
   this->set_bk_image(OpenPictureDialog1->FileName);
}
//---------------------------------------------------------------------------
const wchar_t * __fastcall TTlmTestFrom::get_name  (MDB_RECORD & rec,bool short_name) const
{
    UnicodeString & name = const_cast<UnicodeString &>(name_str);
    name.sprintf(L"rec_id %d",rec.rec_id);
    return name.c_str() ;
}

bool __fastcall TTlmTestFrom::get_record(DWORD rec_id,MDB_RECORD & rec,bool * rec_visible) const
{
     rec.name   = rec_id;
     rec.sname  = rec_id;
     rec.rec_id = rec_id;
     if(rec_id<100)
     {
      rec.state = MDBR_STATE_DISCRETE;
      rec.value = rec_id & 1;
      if(rec_id == 1 || rec_id == 11)
         rec.diag  = 0;
      else
         rec.diag   = 0;
       rec.changes_count = rec_id&7;
     }
     else
     {
       rec.state = 0;
       if(rec_id == 121)
       {
        rec.value = -(float)rec_id;
        rec.diag  = 0;
       }
       else
       {
         rec.value = 777.99;
         rec.max_alarm  = 677.99;
         rec.max_danger = 500;
         rec.state |= MDBR_STATE_ALARM|MDBR_STATE_DANGER;
         rec.diag  = 0;
       }
       rec.max_value = 1000;
       rec.min_value = -1000;
       rec.changes_count = 0;
     }
   return true;
}

void __fastcall TTlmTestFrom::ToolButton4Click(TObject *Sender)
{
 Timer1->Enabled  = (!Timer1->Enabled && Fkadr->sel_count()) ? true : false ;
 if(!Timer1->Enabled) this->refresh_kadr() ;
}
//---------------------------------------------------------------------------
void __fastcall TTlmTestFrom::Timer1Timer(TObject *Sender)
{
  Timer1->Tag = Timer1->Tag+1;
  paint_selected_entryes(false,(Timer1->Tag&1) ? true : false);
}
//---------------------------------------------------------------------------
void __fastcall TTlmTestFrom::ToolButton3Click(TObject *Sender)
{
  TPaintVariantConstruct * pvc = new TPaintVariantConstruct(this,paint_context);
  pvc->ShowModal();
  delete pvc;
}
//---------------------------------------------------------------------------
void __fastcall TTlmTestFrom::N8Click(TObject *Sender)
{
  TMenuItem *item = dynamic_cast<TMenuItem *>(Sender);
  if(item)
  {
     bkgnd.set_grayed(item->Checked);
     refresh_kadr();
  }
}
//---------------------------------------------------------------------------
void __fastcall TTlmTestFrom::N16x161Click(TObject *Sender)
{
  // Установка размера сетки
  TMenuItem *item = dynamic_cast<TMenuItem *>(Sender);
  if(item)
     {
      bkgnd.set_grid(item->Tag);
      refresh_kadr();
     }
}
//---------------------------------------------------------------------------
void __fastcall  TTlmTestFrom::set_current_over(DWORD over)
{
    if(this->current_over != over)
    {
     TTlmPainterForm::set_current_over(over);
     UnicodeString str;
     if(over!=(DWORD)-1)
     {
      mdb_kadr_entry & ke = *Fkadr->get_by_offset(over);
      str.printf(L"rec_id %d , number %d",ke.rec_id,ke.number);
     }
     else
     str.printf(L"sel_count %d",Fkadr->sel_count());
     StatusBar1->Panels->Items[0]->Text = str;
    }
}
