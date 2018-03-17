//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "paint_var_constructor.h"
#include <pictdlgs.h>

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "tlm_painter_form"
#pragma resource "*.dfm"
TPaintVariantConstruct *PaintVariantConstruct;
//---------------------------------------------------------------------------
__fastcall TPaintVariantConstruct::TPaintVariantConstruct(TComponent* Owner)
	: TTlmPainterForm(Owner)
{}
__fastcall TPaintVariantConstruct::TPaintVariantConstruct(TComponent* Owner,int _paint_context)
        : TTlmPainterForm(Owner)


{
 this->Fkadr = new single_kadr;
 paint_context = _paint_context;
 paint_dll         = tlmpaint::painter_dll::get_instance();
 paint_variant     = NULL;
 Fenum_variant     = NULL;
 Fget_variant_data = NULL;
 setup_list();
}
//---------------------------------------------------------------------------

bool __fastcall TPaintVariantConstruct::do_enum_variant(int idx,UnicodeString & var_name)
{
     if(this->Fenum_variant)
              return Fenum_variant(paint_context,idx,var_name);
      wchar_t text[MAX_PATH];
      if(paint_dll->is_loaded() && paint_dll->enum_discretes_ex(paint_context,idx,text,KERTL_ARRAY_COUNT(text)))
        {
           var_name = text;
           return true;
        }
    return false;
}

bool __fastcall TPaintVariantConstruct::do_get_variant (int idx,UnicodeString & img_name,UnicodeString & mask_name,int & cx,int & cy)
{
  bool ret = false;

  return ret;
}

void __fastcall TPaintVariantConstruct::setup_list()
{
    PaintVariantList->Items->Clear();
    int  idx = 0;
    UnicodeString var_name;
    while(do_enum_variant(idx++,var_name))
          PaintVariantList->Items->Add(var_name);
    Fchanges = 0;
    PaintVariantList->ItemIndex = 0;
    this->PaintVariantListSelect(PaintVariantList);
}

 bool __fastcall TPaintVariantConstruct::is_new_variant    ()
 {
   return PaintVariantList->Items->IndexOf(PaintVariantList->Text) < 0 ? true : false;
 }


void __fastcall TPaintVariantConstruct::PaintVariantListChange(TObject *Sender)
{
 bool is_new = is_new_variant();
 if(bDelete->Enabled != is_new)
 {
    TBitmap * bmp = new TBitmap;
    ImageList1->GetBitmap(is_new,bmp);
    bApply->Glyph->Assign(bmp) ;
    delete bmp;
 }

 bApply->Enabled  =  changes ? true : false;
 bDelete->Enabled =  is_new;
 StatusBar1->Panels->Items[0]->Text = PaintVariantList->ItemIndex;
}
//---------------------------------------------------------------------------

void __fastcall TPaintVariantConstruct::FormClose(TObject *Sender, TCloseAction &Action)

{
  Action = caFree;
}
//---------------------------------------------------------------------------

void __fastcall TPaintVariantConstruct::set_multiposition(int val)
{
  if(val>0)
     val = (int) (log((float)val)/log((float)2.0));
  multi_pos->ItemIndex = val;
}

int  __fastcall TPaintVariantConstruct::get_multiposition(void   )
{
  int mp = 1<<std::max(0,multi_pos->ItemIndex);
  return mp;
}

void __fastcall TPaintVariantConstruct::set_element_width(int val)
{

}

int  __fastcall TPaintVariantConstruct::get_element_width(void   )
{
 return _wtoi(this->element_width->Text.c_str());
}

void __fastcall TPaintVariantConstruct::set_elements_count(int val)
{

}

int  __fastcall TPaintVariantConstruct::get_elements_count(void   )
{
 return _wtoi(this->elements_count->Text.c_str());
}



int  __fastcall TPaintVariantConstruct::get_element_height(void   )
{
   if(pv_image->Picture->Bitmap->Handle)
      return pv_image->Picture->Bitmap->Height;
   return 32;
}

void __fastcall TPaintVariantConstruct::PaintVariantListSelect(TObject *Sender)
{
 StatusBar1->Panels->Items[0]->Text = PaintVariantList->ItemIndex;
 LPVOID pvar =   paint_dll->paint_var_get(paint_context,PaintVariantList->ItemIndex);
 if(pvar)
 {
   wchar_t file_name [2048];
   HBITMAP bmp;
   bmp = paint_dll->paint_var_get_image(pvar,file_name);
   pv_image->Picture->Bitmap->Handle = bmp;
   GroupBox2->Repaint() ;

   UnicodeString size_str;
   UnicodeString elem_str;
   UnicodeString width_str;
   int multipos = -1;
   if(bmp)
   {
     size_str.printf(L"%d x %d",pv_image->Picture->Bitmap->Width,pv_image->Picture->Bitmap->Height);
     width_str = paint_dll->paint_var_get_width   (pvar);
     elem_str =  paint_dll->paint_var_get_elements(pvar);
     multipos =  paint_dll->paint_var_get_multipos(pvar);
   }

   set_multiposition(multipos);

   image_size->Caption  = size_str;
   elements_count->Text = elem_str;
   element_width->Text  = width_str;

   image_name->Caption = file_name;
   bmp = paint_dll->paint_var_get_mask(pvar,file_name);
   pv_mask->Picture->Bitmap->Handle = bmp;
   mask_name->Caption = file_name;
   GroupBox3->Refresh() ;
   paint_variant = pvar;
   refresh_kadr();
 }

}
//---------------------------------------------------------------------------

   void      __fastcall TPaintVariantConstruct::get_record   (DWORD rec_id,MDB_RECORD & rec,bool & rec_visible) const
{

      rec_visible = true;
      bzero(&rec,sizeof(rec));
      rec.rec_id         = rec_id;
      rec.value          = rec_value->Position;
      rec.changes_count  = ch_count->ItemIndex;
      if(rec.changes_count >1 ) rec.changes_count = 3;
      bool nv = rec_value->Position ? true:false;
      if(st_not_normal->Checked)
         nv = !nv;
      if(nv)
         rec.state |= MDBR_STATE_DYN_NORMAL;
      if(st_work_out->Checked)
         rec.state |= MDBR_STATE_WORKOUT;
      if(st_tutr->Checked)
         rec.state |= MDBR_STATE_TUTR_ACTIVE;
      if(st_diag->Checked)
         rec.diag = MDBR_DIAG_BAD;
   }

const wchar_t * __fastcall TPaintVariantConstruct::get_name     (MDB_RECORD & rec,bool short_name) const
{
  return L"Test paint";
}




//void __fastcall TPaintVariantConstruct::PaintBox1Paint(TObject *Sender)
//{
//   if(paint_variant)
//   {
//       RECT r = PaintBox1->ClientRect;
//       bkgnd.paint(PaintBox1->Canvas->Handle,r);
//       _TLMOBJ_PAINT_PARAM1 pp;
//       bzero(&pp,sizeof(pp));
//       pp.dw_size  = sizeof(pp);
//       pp.paint_dc = PaintBox1->Canvas->Handle;
//       pp.short_name = L"Test paint";
//       get_record(0,pp.record);
//       pp.entry.size.cx = this->get_element_width();
//       if(pp.entry.size.cx) pp.entry.size.cx+=8;
//          else
//          pp.entry.size.cx = PaintBox1->Width-32;
//       pp.entry.size.cy = get_element_height()+8;
//       pp.entry.position.x           = (PaintBox1->Width - pp.entry.size.cx)/2;
//       pp.entry.position.y           = (PaintBox1->Height - pp.entry.size.cy)/2;
//       pp.entry.options.bkgnd_color  = bk_color->Color;
//       pp.entry.options.color        = fg_color->Color;
//       pp.entry.options.normal_value = st_not_normal->Checked ?  !((DWORD)pp.record.value) : (DWORD)pp.record.value;
//       pp.entry.state = MDB_KADR_ENTRY_STATE_SELECTED;
//       paint_dll->call(PMCMD_DO_PAINT_VARIANT,(LPARAM)&pp,(LPARAM)paint_variant);
//   }
//}
////---------------------------------------------------------------------------

void __fastcall TPaintVariantConstruct::rec_valueChange(TObject *Sender)
{
    if(rec_value->Position > 1)
    cb_rec_value->State = cbGrayed;
    else
    cb_rec_value->Checked = rec_value->Position ? true : false;
    refresh_kadr();;

}
//---------------------------------------------------------------------------

void __fastcall TPaintVariantConstruct::st_work_outClick(TObject *Sender)
{
   refresh_kadr();;
}
//---------------------------------------------------------------------------

void __fastcall TPaintVariantConstruct::SelBkColorClick(TObject *Sender)
{
    if(ColorDialog1->Execute())
    {
      if(Sender == SelBkColor)
        bk_color->Color = ColorDialog1->Color;
        else
        fg_color->Color = ColorDialog1->Color;
      refresh_kadr();
    }

}
//---------------------------------------------------------------------------

void __fastcall TPaintVariantConstruct::cb_rec_valueClick(TObject *Sender)
{
  wchar_t * st_names[] =
  {
   L"ÎÒÊË",
   L"ÂÊË",
   L"---"
  };
  cb_rec_value->Caption = st_names[cb_rec_value->State];

  if(cb_rec_value->State != cbGrayed)
     rec_value->Position = cb_rec_value->State;
}
//---------------------------------------------------------------------------

