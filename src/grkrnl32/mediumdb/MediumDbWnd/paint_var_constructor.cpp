//---------------------------------------------------------------------------
#pragma hdrstop

#include <vcl.h>
#include "paint_var_constructor.h"
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
 Fin_setup       = 0;
 Fchanges        = 0;
 Fglobal_changes = 0;

 rec_paint_name = L"short name";
 Fkadr = new single_kadr;
 Fkadr->add(0,0,0,0,0,0,0,0);
 paint_dll         = tlmpaint::painter_dll::get_instance();

 paint_context     = paint_dll->paint_context_copy_discrete(-1, _paint_context);

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
 bool is_new =  is_new_variant();
 bDelete->Enabled = !is_new;
 if(is_new && (!bAdd->Enabled || !paint_variant))
    {
      set_paint_variant(NULL);
      paint_variant = paint_dll->paint_var_create(paint_context);
      multi_pos->ItemIndex = 0;
      elements_count->Text = 1;
      bAdd->Enabled = true;
      setup_controls(paint_variant);
      changes = 0;
    }
 StatusBar1->Panels->Items[0]->Text = PaintVariantList->ItemIndex;
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
 return std::max(1,_wtoi(this->elements_count->Text.c_str()));
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
 set_paint_variant(pvar);
 changes = 0;
 bDelete->Enabled = true;
}

void __fastcall TPaintVariantConstruct::setup_controls   (LPVOID pvar)
{
  ++Fin_setup;
  if(pvar)
  {
   wchar_t file_name [4098];
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

  }
  //else
  bAdd->Enabled = is_new_variant();
  --Fin_setup;
}

void __fastcall TPaintVariantConstruct::set_paint_variant(LPVOID pvar)
{
   if(paint_variant != pvar)
      {
       if(paint_variant) paint_dll->paint_var_delete  (paint_variant);
        paint_variant   =   paint_dll->paint_var_get_copy(pvar);
        setup_controls(paint_variant);
        refresh_kadr();
      }
}
//---------------------------------------------------------------------------

bool      __fastcall TPaintVariantConstruct::get_record   (DWORD rec_id,MDB_RECORD & rec,bool * rec_visible) const
{

      if(rec_visible)
       *rec_visible = true;
      bzero(&rec,sizeof(rec));
      rec.rec_id         = rec_id;
      rec.value          = rec_value->Position;
      rec.changes_count  = ch_count->ItemIndex;
      rec.state          = MDBR_STATE_DISCRETE;
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
    return true;
}

const wchar_t * __fastcall TPaintVariantConstruct::get_name     (MDB_RECORD & rec,bool short_name) const
{
   return rec_paint_name.c_str();
}

void __fastcall TPaintVariantConstruct::rec_valueChange(TObject *Sender)
{
    if(rec_value->Position > 1)
    cb_rec_value->State = cbGrayed;
    else
    cb_rec_value->Checked = rec_value->Position ? true : false;
    refresh_kadr();

}
//---------------------------------------------------------------------------

void __fastcall TPaintVariantConstruct::st_work_outClick(TObject *Sender)
{
   refresh_kadr();
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
   L"ОТКЛ",
   L"ВКЛ",
   L"---"
  };
  cb_rec_value->Caption = st_names[cb_rec_value->State];

  if(cb_rec_value->State != cbGrayed)
     rec_value->Position = cb_rec_value->State;
}
//---------------------------------------------------------------------------

void __fastcall TPaintVariantConstruct::FormDestroy(TObject *Sender)
{
  if(this->paint_variant)
      paint_dll->paint_var_delete(paint_variant);
  if(paint_context!=-1)
     paint_dll->paint_context_delete(paint_context);
}
//---------------------------------------------------------------------------

 void __fastcall TPaintVariantConstruct::setup_paint_entry     ()
 {
   mdb_kadr_entry & entry = Fkadr->entryes.at(0);
   entry.size.cx = get_element_width ();
   entry.size.cy = get_element_height();

   if(!pv_image->Picture->Bitmap->Width || !entry.size.cx || !entry.size.cy)
   {
    TSize sz =   paint_area->Canvas->TextExtent(rec_paint_name.c_str());
    entry.size<<sz;
   }

   entry.size.cx+=4;entry.size.cy+=4;
   entry.options.color        = fg_color->Color;
   entry.options.bkgnd_color  = bk_color->Color;
   entry.position.x           = (paint_area->Width  - entry.size.cx)/2;
   entry.position.y           = (paint_area->Height - entry.size.cy)/2;

 }

 void __fastcall TPaintVariantConstruct::paint_entryes         (bool erase,bool)
 {
     setup_paint_entry();
     TTlmPainterForm::paint_entryes(erase,false);
 }

 void __fastcall TPaintVariantConstruct::paint_selected_entryes(bool erase,bool)
 {
    setup_paint_entry();
    TTlmPainterForm::paint_selected_entryes(erase,false);
 }

void __fastcall TPaintVariantConstruct::pv_imageClick(TObject *Sender)
{
   if(OpenPictureDialog1->Execute(Handle))
   {
      this->set_image(OpenPictureDialog1->FileName,Sender == pv_mask ? true : false);
   }
}
//---------------------------------------------------------------------------

void __fastcall TPaintVariantConstruct::set_image(UnicodeString file_name,bool mask)
{
      TImage      * image = pv_image  ;
      TStaticText * stext = image_name;
      if(mask)
        {
         image = pv_mask;
         stext = mask_name;
        }

      if(paint_variant)
      {
      HBITMAP bmp =  freeimage::TFreeImage::load_image(file_name.c_str());
      image->Picture->Bitmap->Handle = bmp;
      stext->Caption = file_name;
         if(mask)
           paint_dll->paint_var_set_mask (paint_variant,file_name.c_str());
         else
         {
           paint_dll->paint_var_set_image(paint_variant,file_name.c_str());
           element_width->Text = pv_image->Picture->Bitmap->Width/get_elements_count();
           UnicodeString str;
           str.printf(L"%d x %d",pv_image->Picture->Bitmap->Width,pv_image->Picture->Bitmap->Height);
           image_size->Caption = str;

         }
         changes++;
       refresh_kadr();
      }


}


void __fastcall TPaintVariantConstruct::elements_countChange(TObject *Sender)
{
   if(!Fin_setup)
   {
     int new_count = _wtoi(elements_count->Text.c_str());
     if(!new_count ) new_count = paint_dll->paint_var_get_elements(paint_variant);
     new_count = std::max(1,new_count);
     int width = this->pv_image->Picture->Bitmap->Width / new_count;
     element_width->Text = width;
   }
}
//---------------------------------------------------------------------------

void __fastcall TPaintVariantConstruct::element_widthChange(TObject *Sender)
{
  if(!Fin_setup)
  {
    int width = _wtoi(element_width->Text.c_str());
    if(width)
       paint_dll->paint_var_set_width(paint_variant,width);
    ++Fin_setup;
       elements_count->Text  = paint_dll->paint_var_get_elements(paint_variant) ;
    --Fin_setup;
     changes++;
     refresh_kadr();
  }
}
//---------------------------------------------------------------------------

void __fastcall TPaintVariantConstruct::multi_posChange(TObject *Sender)
{
  //rec_value->Max   = (1<<multi_pos->ItemIndex)/2;
  if(!Fin_setup)
  {
    paint_dll->paint_var_set_multipos(paint_variant,1<<multi_pos->ItemIndex);
    ++changes;
    refresh_kadr();
  }
}
//---------------------------------------------------------------------------
void __fastcall TPaintVariantConstruct::set_global_changes(int ch)
{
  if(Fglobal_changes!= ch)
   {
     Fglobal_changes = ch;
     this->bSave->Enabled = ch ? true : false;
   }
}

void __fastcall TPaintVariantConstruct::set_changes       (int ch)
{
  if(changes != ch)
     {
      Fchanges = ch;
      bApply->Enabled  = !is_new_variant() && ch;
      bAdd->Enabled    = is_new_variant();
     }
}

//---------------------------------------------------------------------------
void __fastcall TPaintVariantConstruct::bDeleteClick(TObject *Sender)
{
   //Удалить paint_variant
   if(!is_new_variant())
      {
       paint_dll->paint_var_remove(paint_context,PaintVariantList->ItemIndex);
       PaintVariantList->Items->Delete(PaintVariantList->ItemIndex);
       changes = 0;
       if(!PaintVariantList->Items->Count)
          {
           PaintVariantList->Text = UnicodeString();
           PaintVariantListSelect(Sender);
          }
      }
   ++global_changes;
}
//---------------------------------------------------------------------------

void __fastcall TPaintVariantConstruct::bApplyClick(TObject *Sender)
{
  //Применить изменения
  LPVOID dstpv = paint_dll->paint_var_get(paint_context,PaintVariantList->ItemIndex);
  paint_dll->paint_var_set_image(dstpv,image_name->Caption.c_str());
  paint_dll->paint_var_set_mask (dstpv,mask_name->Caption.c_str());
  paint_dll->paint_var_set_width(dstpv,this->get_element_width());
  paint_dll->paint_var_set_multipos(dstpv,this->get_multiposition());
  changes = 0;
  ++global_changes;
}
//---------------------------------------------------------------------------

void __fastcall TPaintVariantConstruct::bAddClick(TObject *Sender)
{
  //Добавить  в список
  int idx = paint_dll->paint_var_add(paint_variant,PaintVariantList->Text.c_str());
  if(idx>=0)
  {
    PaintVariantList->Items->Add(PaintVariantList->Text);
    paint_variant = NULL;
    PaintVariantList->ItemIndex = idx;
    PaintVariantListSelect(PaintVariantList);
    changes = 0;
   ++global_changes;
  }
}
//---------------------------------------------------------------------------

void __fastcall TPaintVariantConstruct::bSaveClick(TObject *Sender)
{
  ModalResult = mrOk;
  Close();
}
//---------------------------------------------------------------------------

void __fastcall TPaintVariantConstruct::FormClose(TObject *Sender, TCloseAction &Action)

{
  Action = caFree;
}
//---------------------------------------------------------------------------

void __fastcall TPaintVariantConstruct::FormCloseQuery(TObject *Sender, bool &CanClose)

{
  if(global_changes && ModalResult!=mrOk)
    {
      int res = MessageBoxW(Handle,L"Сохранить изменения ?",Caption.c_str(),MB_YESNOCANCEL|MB_ICONQUESTION);
      switch(res)
      {
        case IDYES   : ModalResult = mrOk;break;
        case IDCANCEL: CanClose    = false;break;
        case IDNO    : ModalResult = mrCancel;break;
      }
    }
}
//---------------------------------------------------------------------------

