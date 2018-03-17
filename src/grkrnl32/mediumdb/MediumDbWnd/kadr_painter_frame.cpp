//---------------------------------------------------------------------------
#pragma hdrstop
#include <vcl.h>

#include "kadr_painter_frame.h"
#include "kadrs_editor.h"
#include "mediumdb_frm.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TKadrPainterFrame *KadrPainterFrame;
//---------------------------------------------------------------------------
__fastcall TKadrPainterFrame::TKadrPainterFrame(TComponent* Owner)
        : TForm(Owner)
{
 Fset_kadr_change = NULL;
 Fon_select_entry = NULL;
 kadr_painter     = NULL;
 Ftree_node       = NULL;
}

__fastcall TKadrPainterFrame::TKadrPainterFrame(TComponent* Owner,TTreeNode * node,single_kadr * sk)
        : TForm(Owner)
{
 Fset_kadr_change = NULL;
 Fon_select_entry = NULL;
 kadr_painter     = new TKadrPainter(this);
 Ftree_node       = node;

 kadr_painter->on_over_entry     = over_entry_handler;
 kadr_painter->on_select_entry   = select_entry_handler;
 kadr_painter->set_kadr_change   = set_kadr_change_handler;


 this->set_kadr_size(sk->kadr.kadr_size.cx,sk->kadr.kadr_size.cy);
 kadr_painter->Left = 0;
 kadr_painter->Top  = 0;

 kadr_painter->ManualDock(this,0,alClient);
 //kadr_painter->Parent  = this;

 kadr_painter->Align = alClient;
 kadr_painter->kadr_data = sk;
 kadr_painter->Visible = true;
 ActiveControl = kadr_painter;
 get_paint_variants(true);
 get_paint_variants(false);
}
//---------------------------------------------------------------------------
void __fastcall TKadrPainterFrame::get_paint_variants(bool discrete)
{
 TComboBox * cb = discrete ? DiscretePaint : AnalogPaint;
 int save_idx = cb->ItemIndex;
 cb->Items->Clear();
 int idx = 0;
 UnicodeString pv_name;
 while(kadr_painter->enum_paint_variant(discrete,idx,pv_name))
 {
   cb->Items->Add(pv_name);
   ++idx;
 }
 cb->ItemIndex = save_idx;

}

void __fastcall TKadrPainterFrame::update_caption()
{
  AnsiString capt = kadr_painter->get_text(kadr_painter->kadr_data->kadr.name_id);
  if(!capt.Length())
     capt = kadr_painter->get_text(kadr_painter->kadr_data->kadr.sname_id);
  if(!capt.Length())
    capt.printf("Кадр ID № ",kadr_painter->kadr_data->kadr.kadr_id);
  Caption = capt;
}

void __fastcall TKadrPainterFrame::FormShow(TObject *Sender)
{
  update_caption();
  kadr_painter->SetFocus();
}
//---------------------------------------------------------------------------
void   __fastcall TKadrPainterFrame::on_kadr_change   (DWORD mask)
{
   kadr_painter->on_kadr_change(mask);
   if(mask & MDB_KADR_FIELD_NAME)
       update_caption();
}

void   __fastcall TKadrPainterFrame::on_record_change (DWORD mask,DWORD rec_id)
{
   /*
   char text[512];
   sprintf(text,"rch %d m %04X",rec_id,mask);
   GKD_TRACE(text,0);
   */
   kadr_painter->on_record_change(mask,rec_id);
   mdb_kadr_entry * entry = kadr_painter->over_entry;
   if(!entry)
      entry = kadr_painter->kadr_data->get_selected(0);
   if(entry && entry->rec_id == rec_id)
      this->show_entry_info(entry);
}

void    __fastcall TKadrPainterFrame::show_entry_info(mdb_kadr_entry * entry)
{
 AnsiString str;
 if(entry)
 {
  str.printf("ID %03d № = %d [ %d, %d : %d, %d ]",entry->rec_id,entry->number,entry->position.x,entry->position.y,entry->size.cx,entry->size.cy);
  StatusBar1->Panels->Items[0]->Text = str;
  mdb_short_record sr;
  kadr_painter->get_record(entry->rec_id,sr);
  if(sr.state & MDBR_STATE_DISCRETE)
  str.printf("%s",(DWORD)sr.value ? "ВКЛ":"ОТКЛ");
  else
  str.printf("%03.2f",sr.value);
  str.cat_printf("- %s",sr.diag ? (sr.diag == MDBR_DIAG_BAD ? "Bad":"Partial"):"OK");
  StatusBar1->Panels->Items[1]->Text = str;
  AnsiString name = kadr_painter->get_name(sr.rec_id,true );
  if(!name.Length())
    name = kadr_painter->get_name(sr.rec_id,false );
  str.printf("%d.%d.%d.%d",(DWORD)sr.addr.addr.pu,(DWORD)sr.addr.addr.cp,(DWORD)sr.addr.addr.fa,(DWORD)sr.addr.addr.sb);
  if(sr.addr.param_number!=(DWORD)-1)
   str.cat_printf("-%d ",sr.addr.param_number);
  str+=name;
  if(sr.state&MDBR_STATE_DANGER)
  str+=" danger";
  if(sr.state&MDBR_STATE_ALARM)
  str+=" alarm";
  StatusBar1->Panels->Items[2]->Text = str;
 }
 else
 {

  StatusBar1->Panels->Items[0]->Text = str;
  StatusBar1->Panels->Items[1]->Text = str;
  if(kadr_painter->kadr_data)
  str.printf("kadr size  %d , %d",kadr_painter->kadr_data->kadr.kadr_size.cx,kadr_painter->kadr_data->kadr.kadr_size.cy);
  StatusBar1->Panels->Items[2]->Text = str;
 }

}


void    __fastcall TKadrPainterFrame::over_entry_handler(mdb_kadr_entry * entry)
{
  if(!entry && kadr_painter->kadr_data && kadr_painter->kadr_data->sel_count() )
     entry = kadr_painter->kadr_data->get_selected(0);
  show_entry_info(entry);
}


void   __fastcall  TKadrPainterFrame::setup_entry_param(mdb_kadr_entry * entry)
{
  if(entry)
  {
   BkgndColor->Color   = (TColor)entry->options.bkgnd_color;
   TextColor->Color    = (TColor)entry->options.color;
   DeleteBtn->Enabled  = true;
   mdb_record sr;

   kadr_painter->get_record(entry->rec_id,sr);
   if(sr.state&MDBR_STATE_DISCRETE)
      {
       DiscretePaint->ItemIndex = entry->options.paint_as;
       DiscretePaint->Visible = true;
       AnalogPaint->Visible   = false;
       Panel2->Visible = false;
      }
      else
      {
       AnalogPaint->ItemIndex = entry->options.paint_as;
       AnalogPaint->Visible   = true;
       DiscretePaint->Visible = false;
       Panel2->Visible = true;
       Precision->Position     = entry->options.prec;
       Digits->Position        = entry->options.digits;
      }
  }
  else
  {
  DeleteBtn->Enabled     = false;
  DiscretePaint->Visible = false;
  AnalogPaint->Visible   = false;
  }
  //if(entry_paint_props) entry_paint_props->set_entry(entry)
  show_entry_info(entry);
}

void    __fastcall TKadrPainterFrame::select_entry_handler(mdb_kadr_entry * entry,bool add)
{
  if(Fon_select_entry)
     Fon_select_entry(entry,add);
  DWORD sel_count = kadr_painter->kadr_data->sel_count();
  miSetNormalSize->Enabled = sel_count>0 ? true:false;
  miEditInDB->Enabled    = sel_count>0 ? true:false;
  miArrangeTop->Enabled  = sel_count>1 ? true:false;
  miSetOneSize->Enabled  = sel_count>1 ? true:false;
  miArrangeLeft->Enabled = sel_count>1 ? true:false;
  miArrangeGridY->Enabled = kadr_painter->grid_visible && sel_count>0 ? true:false;
  miArrangeGridX->Enabled = miArrangeGridY->Enabled;
  kadr_painter->SetFocus();
}

void    __fastcall TKadrPainterFrame::set_kadr_change_handler (DWORD kadr_id,DWORD mask)
{
  if(this->Fset_kadr_change)
      Fset_kadr_change(kadr_id,mask);
}


void __fastcall TKadrPainterFrame::BkgndColorClick(TObject *Sender)
{
  if(DeleteBtn->Enabled)
   {
    ColorDialog1->Color = BkgndColor->Color;
    if(ColorDialog1->Execute())
     {
       BkgndColor->Color = ColorDialog1->Color;
       kadr_painter->set_sel_bk_color(ColorDialog1->Color);
       kadr_painter->changes|= MDB_KADR_FIELD_ENTRYES;
     }
   }
}
//---------------------------------------------------------------------------

void __fastcall TKadrPainterFrame::TextColorClick(TObject *Sender)
{
  if(DeleteBtn->Enabled)
   {
    ColorDialog1->Color = TextColor->Color;
    if(ColorDialog1->Execute())
     {
       TextColor->Color = ColorDialog1->Color;
       kadr_painter->set_sel_text_color(ColorDialog1->Color);
       kadr_painter->changes|= MDB_KADR_FIELD_ENTRYES;

     }
   }

}
//---------------------------------------------------------------------------

void __fastcall TKadrPainterFrame::AnalogPaintChange(TObject *Sender)
{
  if(kadr_painter->kadr_data->sel_count())
  {
   if(kadr_painter->set_sel_paint_var(AnalogPaint->ItemIndex,false))
      kadr_painter->changes|= MDB_KADR_FIELD_ENTRYES;
  }
}
//---------------------------------------------------------------------------

void __fastcall TKadrPainterFrame::DiscretePaintChange(TObject *Sender)
{
  if(kadr_painter->kadr_data->sel_count())
  {
   if(kadr_painter->set_sel_paint_var(DiscretePaint->ItemIndex,true))
    kadr_painter->changes|= MDB_KADR_FIELD_ENTRYES;
  }
}
//---------------------------------------------------------------------------


void __fastcall TKadrPainterFrame::PrecisionClick(TObject *Sender,
      TUDBtnType Button)
{
  single_kadr * kd = kadr_painter->kadr_data;
  if(kd && kd->sel_count())
  {
    kadr_painter->set_sel_prec_digits(Precision->Position,Digits->Position);
    kadr_painter->changes|= MDB_KADR_FIELD_ENTRYES;
  }
}
//---------------------------------------------------------------------------



void __fastcall TKadrPainterFrame::miArrangeTopClick(TObject *Sender)
{
  kadr_painter->arrange_sel_top();      
}
//---------------------------------------------------------------------------

void __fastcall TKadrPainterFrame::miArrangeLeftClick(TObject *Sender)
{
 kadr_painter->arrange_sel_left();        
}
//---------------------------------------------------------------------------

void __fastcall TKadrPainterFrame::miSetOneSizeClick(TObject *Sender)
{
 kadr_painter->arrange_sel_size();
}
//---------------------------------------------------------------------------

void __fastcall TKadrPainterFrame::N3Click(TObject *Sender)
{
  kadr_painter->hide_bkgnd = !kadr_painter->hide_bkgnd;
  N3->Caption = kadr_painter->hide_bkgnd ? "Показать фон":"Скрыть фон";
}
//---------------------------------------------------------------------------



void __fastcall TKadrPainterFrame::miEnableGridClick(TObject *Sender)
{
 TComponent * c = dynamic_cast<TComponent*>(Sender);
 if(c)
 {
  kadr_painter->grid_visible = c->Tag ? true:false;
  if(c->Tag != (int) kadr_painter->grid_step)
   {
    kadr_painter->grid_step = c->Tag;
    miArrangeGridY->Enabled = kadr_painter->grid_step && kadr_painter->selected_count > 0 ? true : false;
    miArrangeGridX->Enabled = miArrangeGridY->Enabled ;
   }
 }
}
//---------------------------------------------------------------------------



void __fastcall TKadrPainterFrame::miSetNormalSizeClick(TObject *Sender)
{
  kadr_painter->reset_size();
}
//---------------------------------------------------------------------------

void __fastcall TKadrPainterFrame::N8Click(TObject *Sender)
{
 kadr_painter->move_outsiding();
}
//---------------------------------------------------------------------------

void __fastcall TKadrPainterFrame::N9Click(TObject *Sender)
{
 kadr_painter->select_all();
}
//---------------------------------------------------------------------------

void __fastcall TKadrPainterFrame::FormResize(TObject *Sender)
{
 show_entry_info(NULL);
 TKadrsEditor* ke = dynamic_cast<TKadrsEditor*>(this->Owner);
 if(ke && ke->current_kadr_frame == this && !ke->in_setup)
 {

   ke->in_setup = (true);
   ke->KadrWidth->Text  = this->kadr_painter->Width;
   ke->KadrHeight->Text = this->kadr_painter->Height;
   ke->in_setup = (false);
 }
}
//---------------------------------------------------------------------------
void __fastcall TKadrPainterFrame::set_kadr_size (int w,int h)
{
 ClientWidth  = calc_frame_width (w);
 ClientHeight = calc_frame_height(h);
 if(kadr_painter->kadr_data)
 {
  kadr_painter->kadr_data->kadr.kadr_size.cx = w;
  kadr_painter->kadr_data->kadr.kadr_size.cy = h;
  kadr_painter->OnResize(this);
  kadr_painter->Repaint() ;
 }
}





void __fastcall TKadrPainterFrame::miArrangeGridYClick(TObject *Sender)
{
  //Выровнять по сетке
  kadr_painter->arrange_sel_grid(Sender == miArrangeGridY ? false:true);
}
//---------------------------------------------------------------------------

void __fastcall TKadrPainterFrame::N4Click(TObject *Sender)
{
  TKadrsEditor* ke = dynamic_cast<TKadrsEditor*>(this->Owner);
  if(ke && ke->current_kadr_frame == this)
  {
    ke->KadrBkgnd->Text = UnicodeString();
  }
}
//---------------------------------------------------------------------------

void __fastcall TKadrPainterFrame::FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)

{
    bool AltPressed  =  Shift.Contains(ssAlt);
    bool CtrlPressed =  Shift.Contains(ssCtrl);
switch(Key)
 {
  case VK_RETURN:
                 if(CtrlPressed && ApplyBtn->Enabled && ApplyBtn->OnClick)
                   {Key = 0; ApplyBtn->OnClick(this);}
                  break;
  case VK_BACK:
        if(AltPressed && UndoBtn->Enabled && UndoBtn->OnClick )
           {Key = 0;UndoBtn->OnClick(this);}
       break;
  case VK_DOWN:
  case VK_UP:
   break;
  }
}
//---------------------------------------------------------------------------
void __fastcall TKadrPainterFrame::add_paint_variant(bool discrete,int idx,UnicodeString & name)
{
 TComboBox * cb = discrete ? DiscretePaint : AnalogPaint;
 while(cb->Items->Count<=idx)
     cb->Items->Add(cb->Items->Count);
     cb->Items->Strings[idx] = name;
}
void __fastcall TKadrPainterFrame::SelectBkgndBtnClick(TObject *Sender)
{
  //Обновить фоновое изображение
  kadr_painter->reload_bkgnd_image();
}
//---------------------------------------------------------------------------

void __fastcall TKadrPainterFrame::FormActivate(TObject *Sender)
{
  if(tree_node)
     tree_node->Selected = true;
   if(kadr_painter && Visible)
   {
     kadr_painter->SetFocus();
     this->ActiveControl = kadr_painter;
   }
}
//---------------------------------------------------------------------------


void __fastcall TKadrPainterFrame::miBkgndKadrSizeProportionalClick(TObject *Sender)

{
   TMenuItem * mi = dynamic_cast<TMenuItem*>(Sender);
   if(mi)
   {
     bkgnd_mode = mi->Tag;

   }
}
//---------------------------------------------------------------------------

int     __fastcall TKadrPainterFrame::get_bkgnd_mode()
{
    return (kadr_painter->kadr_data->kadr.kadr_flags & MDB_KADR_FLAG_IMAGE_BKGND_MASK);
}

void    __fastcall TKadrPainterFrame::set_bkgnd_mode(int new_mode)
{
    int old_mode = kadr_painter->kadr_data->kadr.kadr_flags & MDB_KADR_FLAG_IMAGE_BKGND_MASK;
    new_mode &= MDB_KADR_FLAG_IMAGE_BKGND_MASK;
    if(old_mode != new_mode   )
    {
      kadr_painter->kadr_data->kadr.kadr_flags &= ~ MDB_KADR_FLAG_IMAGE_BKGND_MASK;
      kadr_painter->kadr_data->kadr.kadr_flags |= new_mode;
      kadr_painter->on_kadr_change(MDB_KADR_FIELD_FLAGS);
      TKadrsEditor* ke = dynamic_cast<TKadrsEditor*>(this->Owner);
      if(ke && ke->current_kadr_frame == this)
      {
        ke->KadrBkgndOptions->ItemIndex = new_mode;
      }

      switch(new_mode)
      {
      case MDB_KADR_FLAG_IMAGE_BKGND_DEFAULT                : miBkgndNoChange->Checked = true;break;
      case MDB_KADR_FLAG_IMAGE_BKGND_CENTER                 : miBkgndCentered->Checked = true;break;
      case MDB_KADR_FLAG_IMAGE_BKGND_STRETCH                : miBkgndKadrSize->Checked = true;break;
      case MDB_KADR_FLAG_IMAGE_BKGND_STRETCH_PROPORTIONAL   : miBkgndKadrSizeProportional->Checked = true;break;

      }

    }
}



void __fastcall TKadrPainterFrame::DiagOffClick(TObject *Sender)
{
//  if(!entry_paint_props) entry_paint_props = new TPaintProps(Owner);
//      {
//
//       //entry_paint_props->setup_entry()
//      }

   kadr_painter->diag_off = !kadr_painter->diag_off;
   DiagOff->Down  = kadr_painter->diag_off;

}
//---------------------------------------------------------------------------

void __fastcall TKadrPainterFrame::WndProc(Messages::TMessage &Message)
{
   switch(Message.Msg)
   {

//    case WM_GETDLGCODE:
//         Message.Result = DLGC_WANTALLKEYS;
//    break;
     default: TForm::WndProc(Message); break;
   }
}



