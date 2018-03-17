//---------------------------------------------------------------------------

#pragma hdrstop
#include <vcl.h>
#include "tlm_kadr_editor.h"
#include "gkmodule_form.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "tlm_painter_form"
#pragma resource "*.dfm"
TTlmKadrEditor *TlmKadrEditor;
//---------------------------------------------------------------------------
__fastcall TTlmKadrEditor::TTlmKadrEditor(TComponent* Owner)
	: TTlmPainterForm(Owner)
{
 Feditable       = true;
 Fin_setup       = 0;
 entry_props     = NULL;
}
//---------------------------------------------------------------------------

bool __fastcall TTlmKadrEditor::get_record(DWORD rec_id, MDB_RECORD & rec,bool * rec_visible) const
 {
       bool ret = TTlmPainterForm::get_record(rec_id,rec,rec_visible);
       rec.changes_count          = 0;
       if(tbDiag->Down)  rec.diag = 0;
       return ret;

 }
//---------------------------------------------------------------------------

UnicodeString __fastcall TTlmKadrEditor::get_image_file  ()
{
  if(kadr_data) return kadr_data->image_name;
  return TTlmPainterForm::get_image_file();
}

  UnicodeString   __fastcall TTlmKadrEditor::get_kadr_caption ()
  {
  UnicodeString str;
  if(kadr_data)
  {
    str = kadr_data->kadr_name;
    if(str.IsEmpty())
       str = kadr_data->kadr_sname;
  }
  if(str.IsEmpty())
     str = TTlmPainterForm::get_kadr_caption();
  if(kadr_data->is_new())
     str += L" [Новый]";
   else
   {
    if(kadr_changes)
     str += L" [Изменен]";
   }
  return str;
  }

  void         __fastcall TTlmKadrEditor::set_kadr     (single_kadr * kadr)
  {
      ++Fin_setup;
      TTlmPainterForm::set_kadr(kadr);
      int tag = kadr->kadr.kadr_flags & MDB_KADR_FLAG_IMAGE_BKGND_MASK;
      for(int i = 0 ;i<miBkMode->Count;i++)
         {
           TMenuItem * mi = miBkMode->Items[i];
           if(mi->Tag == tag) {mi->Checked = true;break;}
         }
      --Fin_setup;
      if(entry_props) select_changed(Fkadr->get_last_selected_offset(),true);
      update_commands();
      kadr_changes = kadr_changes;
  }
//---------------------------------------------------------------------------

  DWORD        __fastcall TTlmKadrEditor::get_kadr_changes()
  {
    Tkadr_data * kd = kadr_data;
    return kd ? kd->changes : 0;
  }
//---------------------------------------------------------------------------

  void         __fastcall TTlmKadrEditor::set_kadr_changes(DWORD val)
  {
    Tkadr_data * kd = kadr_data;
    if(!Fin_setup && kd)
    {
      if(kd->changes!=val)
         {
          kd->changes = val;
          TWinControl * wc = get_owner_control();
          if(wc && wc->Handle)  SendMessage(wc->Handle,WM_KADR_CHANGED,val,LPARAM(Fkadr));
         }
      Caption = get_kadr_caption();
      tbApply->Enabled = val;
      tbUndo->Enabled  = val  && !kd->is_new();
    }
  }
//---------------------------------------------------------------------------

void __fastcall TTlmKadrEditor::tbFullScreenClick(TObject *Sender)
{
  TPoint p;
  p.x = Left + Width/2;
  p.y = Top  + Height/2;
  Forms::TMonitor * mon = Screen->MonitorFromPoint(p);
  Top    = mon->WorkareaRect.top;
  Left   = mon->WorkareaRect.Left;
  Width  = mon->WorkareaRect.Width() ;
  Height = mon->WorkareaRect.Height() ;
}
//---------------------------------------------------------------------------

void __fastcall TTlmKadrEditor::miBkgndNoChangeClick(TObject *Sender)
{
  TMenuItem * mi = dynamic_cast<TMenuItem *>(Sender);
  if(mi)
    {
     tlmpaint::TBmpBkgnd::bkgnd_mode_t mode = tlmpaint::TBmpBkgnd::bkgnd_mode_t(mi->Tag&MDB_KADR_FLAG_IMAGE_BKGND_MASK);
     kadr_data->kadr.kadr_flags &= ~MDB_KADR_FLAG_IMAGE_BKGND_MASK;
     kadr_data->kadr.kadr_flags |= mode;
     kadr_changes               |= MDB_KADR_FIELD_FLAGS;
     set_bk_mode(mode);
    }
}
//---------------------------------------------------------------------------

void __fastcall TTlmKadrEditor::miSelectBkgndClick(TObject *Sender)
{
  if(PictDlg->Execute(Handle) )
    {
      UnicodeString image_name = PictDlg->FileName;
      set_bk_image(image_name);
      if(kadr_data)
         {
           kadr_data->image_name = image_name;
           kadr_changes |= MDB_KADR_FIELD_IMAGE;
         }
    }
}
//---------------------------------------------------------------------------

void __fastcall TTlmKadrEditor::FormResize(TObject *Sender)
{
   TTlmPainterForm::FormResize(Sender);
   Tkadr_data * kd = kadr_data;
   if(kd)
   {
     if(kd->kadr.kadr_size.cx != paint_area->Width || kd->kadr.kadr_size.cy != paint_area->Height)
     {
      kd->kadr.kadr_size.cx = paint_area->Width;
      kd->kadr.kadr_size.cy = paint_area->Height;
      kadr_changes |= MDB_KADR_FIELD_SIZE;
     }
   }
}
//---------------------------------------------------------------------------

void __fastcall TTlmKadrEditor::miGrid16x16Click(TObject *Sender)
{
   TMenuItem * mi = dynamic_cast<TMenuItem *>(Sender);
   int grid_size = 0;
   if(mi) grid_size = mi->Tag;
   set_bk_grid(grid_size);
}
//---------------------------------------------------------------------------
void __fastcall TTlmKadrEditor::FormDestroy(TObject *Sender)
{
  kadr_data->user_data = NULL;
}
//---------------------------------------------------------------------------
void __fastcall TTlmKadrEditor::FormClose(TObject *Sender, TCloseAction &Action)
{
  Action = caFree;
}
//---------------------------------------------------------------------------
void __fastcall TTlmKadrEditor::tbApplyClick(TObject *Sender)
{
   TWinControl * wc = get_owner_control();
   if(wc && wc->Handle)  SendMessage(wc->Handle,WM_APPLY_CHANGES,0,LPARAM(Fkadr));
}
//---------------------------------------------------------------------------
void __fastcall TTlmKadrEditor::tbUndoClick(TObject *Sender)
{
  TWinControl * wc = get_owner_control();
  if(wc && wc->Handle)  SendMessage(wc->Handle,WM_UNDO_CHANGES,0,LPARAM(Fkadr));
}
//---------------------------------------------------------------------------
void __fastcall TTlmKadrEditor::tbDiagClick(TObject *Sender)
{
   refresh_kadr();
}
//---------------------------------------------------------------------------
void __fastcall TTlmKadrEditor::on_entry_props_close(TObject *Sender, TCloseAction &Action)
{
  Action      = caFree;
  entry_props = NULL;
}
//---------------------------------------------------------------------------

void __fastcall  TTlmKadrEditor::show_props  (DWORD offset,bool selected)
{
 if(!entry_props)
    {
      entry_props = new TEntryProps(this,this->paint_context,WM_NOTIFY_ENTRY_CHANGED);
      entry_props->OnClose = on_entry_props_close;
      entry_props_cling(0,0);
    }

  if(!selected)
      offset =  Fkadr->get_selected_offset(Fkadr->sel_count()-1);
  if(offset != DWORD(-1) )
    {
       mdb_kadr_entry *entry = &*Fkadr->get_by_offset(offset);
       mdb_record   rec;
       get_record(entry->rec_id,rec,NULL);
       entry_props->discrete = rec.is_discrete();
       entry_props->entry = *entry;
       ShowWindow(entry_props->Handle,SW_SHOWNA);
       entry_props->Visible = true;
    }
    else
    entry_props->Visible = false;

}

void __fastcall  TTlmKadrEditor::select_changed  (DWORD offset,bool selected)
{
 if(tbProps->Down )
    show_props(offset,selected);
   else
     {
       if(entry_props)  entry_props->Close();
       entry_props = NULL;
     }
      TWinControl * wc = get_owner_control();
      if(wc && wc->Handle)  SendMessage(wc->Handle,WM_ENTRY_SELECT_CHANGE,offset,LPARAM(Fkadr));
  update_commands();
  TTlmPainterForm::select_changed(offset,selected);
}

void __fastcall TTlmKadrEditor::paint_areaDblClick(TObject *Sender)
{
  if(current_over != (DWORD)-1)
    this->tbPropsClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TTlmKadrEditor::WndProc(TMessage & msg)
{
  switch(msg.Msg)
  {
   case WM_NOTIFY_ENTRY_CHANGED :
        entry_props_chanhed((TEntryProps*)msg.WParam,(LPMDB_KADR_ENTRY) msg.LParam );
   break;
   case WM_MOVE:
        if(entry_props)
        {
         TTlmPainterForm::WndProc(msg);
         entry_props_cling(0,0);
        }
        break;
   default : TTlmPainterForm::WndProc(msg);break;
  }
}


struct opt_setter
{
  TTlmKadrEditor        & form;
  const mdb_kadr_entry  & src;
  bool                    discrete;
  tlmpaint::painter_dll * pdll;
  TLMOBJ_PAINT_PARAM1 pp;

  opt_setter(TTlmKadrEditor  & _form,const mdb_kadr_entry  & _src):form(_form),src(_src),pdll(NULL)
  {
    mdb_record  rec;get_record(src.rec_id,rec);
    discrete = rec.is_discrete();
    pdll     = tlmpaint::painter_dll::get_instance();
    bzero(&pp,sizeof(pp));
    pp.dw_size   = sizeof(pp);
    pp.paint_ctx = form.paint_context;
    pp.paint_dc  = form.paint_area->Canvas->Handle;
    pp.font      = form.paint_area->Canvas->Font->Handle;
  }

  void __fastcall   get_record(DWORD rec_id, MDB_RECORD & rec) const
  {
    form.get_record(rec_id,rec,NULL);
  }
  void  operator ()(DWORD idx);

};

  void  opt_setter::operator ()(DWORD idx)
  {
    mdb_kadr_entry & e = form.get_kadr_data()->entryes.at(idx);
    get_record(e.rec_id,pp.record);
    if(is_record_discrete(&pp.record) == discrete)
     {
      if(e.options.paint_as != src.options.paint_as || ( !discrete && ( e.options.digits != src.options.digits ||  e.options.prec != src.options.prec) ))
         {
           SIZE sz;
           pp.entry = e;
           pp.entry.options = src.options;
           pdll->calc_size(&pp,&sz);
           e.size<<sz;
         }
         e.options = src.options;
     }
  }



void __fastcall  TTlmKadrEditor::entry_props_chanhed( TEntryProps * e_props,LPMDB_KADR_ENTRY entry )
{
  kadr_entryes_table::iterator ptr;
  if(Fkadr->find_entry(entry->rec_id,entry->number,ptr))
     {
      entry_index::iterator beg = Fkadr->selected.begin();
      entry_index::iterator end = Fkadr->selected.end  ();
      opt_setter os(*this,*entry);
      std::for_each(beg,end,os);
      kadr_changes |= MDB_KADR_FIELD_ENTRYES;
      refresh_kadr();
     }

}


void __fastcall  TTlmKadrEditor::entry_props_cling(int dx,int dy)
{

  TPoint pt;
  pt.x = Left+Width;
  pt.y = Top;
  Forms::TMonitor * mon = Screen->MonitorFromPoint(pt);
  if(!dx && !dy)
  {
   if(mon->WorkareaRect.right< ( pt.x + entry_props->Width))
     pt.x = mon->WorkareaRect.right - entry_props->Width;
   if((entry_props->Top < Top) || (entry_props->Top+entry_props->Height > (Top+Height)))
     entry_props->Top  = pt.y;
   entry_props->Left = pt.x;
  }
  else
  {
    entry_props->Left += dx;
    entry_props->Top  += dy;
  }
}


void __fastcall TTlmKadrEditor::tbPropsClick(TObject *Sender)
{
  select_changed(Fkadr->get_selected_offset(Fkadr->sel_count()-1),true);
}
//---------------------------------------------------------------------------
void __fastcall TTlmKadrEditor::FormActivate(TObject *Sender)
{
 if(tbProps->Down && this->Fkadr->sel_count())
    show_props(Fkadr->get_selected_offset(Fkadr->sel_count()-1),true);
}
//---------------------------------------------------------------------------
void __fastcall TTlmKadrEditor::FormDeactivate(TObject *Sender)
{
  if(entry_props && Screen->ActiveForm != this->entry_props )
     entry_props->Visible = false;

}
//---------------------------------------------------------------------------
class arrange_setter_t
{
   single_kadr & skadr;
   LONG          v;
   bool          top;
   public :
   arrange_setter_t(single_kadr & _sk,LONG _v,bool _top ):skadr(_sk),v(_v),top(_top){}
   arrange_setter_t(const arrange_setter_t & src):skadr(src.skadr),v(src.v),top(src.top){}

   void operator ()(const entry_index::size_type & idx)
   {
     mdb_kadr_entry & e = skadr.entryes.at(idx);
     if(top)
       e.position.y = v;
       else
       e.position.x = v;
  }
};

void __fastcall  TTlmKadrEditor::do_arrange      (bool top)
{
   mdb_kadr_entry * entry =  kadr_data->get_last_selected();
   if(entry)
   {

    LONG & v = top ? entry->position.y : entry->position.x;
    LONG grid_step =  bkgnd.get_grid();
    if(grid_step)
      {
        LONG dv = v%grid_step;
        if(dv)
        {
          dv = grid_step-dv;
          if(dv < grid_step/2) v-=dv; else v+=dv;
        }
      }
    std::for_each(this->Fkadr->selected.begin(),Fkadr->selected.end(),arrange_setter_t(*Fkadr,v,top));
    refresh_kadr();
    kadr_changes|=MDB_KADR_FIELD_ENTRYES;
   }
}
//---------------------------------------------------------------------------

class size_setter_t
{
   single_kadr & skadr;
   MDB_GSIZE      sz;
   public :
   size_setter_t(single_kadr & _sk, MDB_GSIZE _sz ):skadr(_sk),sz(_sz  ){}
   size_setter_t(const size_setter_t & src):skadr(src.skadr),sz(src.sz){}
   void operator ()(const entry_index::size_type & idx)
   {
     mdb_kadr_entry & e = skadr.entryes.at(idx);
     e.size = sz;
  }
};

void __fastcall  TTlmKadrEditor::do_same_size_set()
{
   if(Fkadr->sel_count()>1)
   {
       mdb_kadr_entry * entry =  kadr_data->get_last_selected();
       if(entry)
       {
        std::for_each(this->Fkadr->selected.begin(),Fkadr->selected.end(),size_setter_t(*Fkadr,entry->size));
        refresh_kadr();
        kadr_changes|=MDB_KADR_FIELD_ENTRYES;
       }
   }
}
//---------------------------------------------------------------------------

void __fastcall TTlmKadrEditor::tbArrageLeftClick(TObject *Sender)
{
  do_arrange(false);
}
//---------------------------------------------------------------------------

void __fastcall TTlmKadrEditor::tbArangeTopClick(TObject *Sender)
{
  do_arrange(true);
}
//---------------------------------------------------------------------------

void __fastcall TTlmKadrEditor::tbSameSizeClick(TObject *Sender)
{
  do_same_size_set();
}
//---------------------------------------------------------------------------

void __fastcall TTlmKadrEditor::kadr_popupPopup(TObject *Sender)
{
// miEditRecord->Enabled = Fkadr && Fkadr->sel_count() == 1;
// miDelete->Enabled     = Fkadr && Fkadr->sel_count();
   miMakeEntryCopy ->Enabled = Fkadr && Fkadr->sel_count() == 1;
}
//---------------------------------------------------------------------------

void __fastcall TTlmKadrEditor::tbDeleteClick(TObject *Sender)
{
 //Удалить выделенное
 if(Fkadr)
 {
  Fkadr->remove_selected();
  kadr_changes |= MDB_KADR_FIELD_ENTRYES;
  TWinControl * wc = get_owner_control();
  if(wc && wc->Handle)  SendMessage(wc->Handle,WM_NOTIFY_ENTRYES_CHANGED,0,LPARAM(Fkadr));
 }
 this->refresh_kadr();
}
//---------------------------------------------------------------------------



void __fastcall TTlmKadrEditor::miLayoutVertClick(TObject *Sender)
{

  if(Fkadr->sel_count())
  {
  bool ctrl = (GetKeyState(VK_CONTROL)&0x8000) ? true : false;
  MDB_GPOINT pt;
  Fkadr->get_pos_sizes(Fkadr->selected.begin(),Fkadr->selected.end(),&pt,NULL,NULL,NULL);
  Fkadr->layout_entryes(Fkadr->selected.begin(),Fkadr->selected.end()
                       ,pt.x,pt.y
                       ,bkgnd.get_grid(),Sender == miLayoutVert ? true : false,ctrl);
  kadr_changes |= MDB_KADR_FIELD_ENTRYES;
  refresh_kadr();
  }
}
//---------------------------------------------------------------------------

void __fastcall TTlmKadrEditor::miLayoutInvisibleClick(TObject *Sender)
{
 selections_clear();
 kadr_entryes_table::iterator beg = Fkadr->entryes.begin();
 kadr_entryes_table::iterator end = Fkadr->entryes.end  ();
 while(beg<end)
  {
   if( (beg->position.x > (Fkadr->kadr.kadr_size.cx - beg->size.cx/2)) || (beg->position.y > (Fkadr->kadr.kadr_size.cy - beg->size.cy/2)) )
       select_entry(Fkadr->get_offset(beg),sel_mode_t::select);
   ++beg;
  }
  if(Fkadr->sel_count())
  {
  Fkadr->layout_entryes(Fkadr->selected.begin(),Fkadr->selected.end()
                       ,2,2
                       ,bkgnd.get_grid(),false,false);
  kadr_changes |= MDB_KADR_FIELD_ENTRYES;
  refresh_kadr();

  }
}
//---------------------------------------------------------------------------

void __fastcall  TTlmKadrEditor::on_record_props_close(TObject *Sender, TCloseAction &Action)
{
  Action = caFree;
  record_props = NULL;
}

//---------------------------------------------------------------------------

void __fastcall TTlmKadrEditor::miEditRecordClick(TObject *Sender)
{
  mdb_kadr_entry * entry = Fkadr->get_last_selected();
  TGKModuleForm * gkmf = dynamic_cast<TGKModuleForm *>(get_owner_control());
  if(entry && gkmf)
  {
    record_props = new TRecordProps(this,gkmf->mod_handle);
    mdb_record  rec ;
    get_record(entry->rec_id,rec,NULL);
    record_props->RecordFrame1->record = rec;
    record_props->OnClose = on_record_props_close;
    record_props->Visible = true;
  }
}
//---------------------------------------------------------------------------
void __fastcall  TTlmKadrEditor::show_entry_info(mdb_kadr_entry *entry)
{
  UnicodeString str;
  if(entry)
    {
      str.printf(L"rec_id %d номер %d, размер [ %3d X %3d ]",entry->rec_id,entry->number,entry->size.cx,entry->size.cy);
      mdb_record rec;
      get_record(entry->rec_id,rec,NULL);
      UnicodeString rec_name = get_name(rec,false);
      UnicodeString value;
      if(rec.is_discrete())
         value.printf(L"Значение %s",DWORD(rec.value) ? L" ON":L"OFF");
         else
         value.printf(L"Значение %.2f",rec.value);
      str.cat_printf(L"%s %s %s",value.c_str(),rec.diag ? L"BAD":L"OK",rec_name.c_str());
    }
    else
    {
     str.printf(L"Кадр ID %d размер [ %3d X %3d ] элементов в кадре %d",Fkadr->kadr.kadr_id,Fkadr->kadr.kadr_size.cx,Fkadr->kadr.kadr_size.cy,Fkadr->entryes.size());
    }
  StatusBar1->Panels->Items[0]->Text = str;
}


void __fastcall  TTlmKadrEditor::set_current_over(DWORD val)
{
    TTlmPainterForm::set_current_over(val);
    mdb_kadr_entry * entry = Fkadr->get_by_offset(val);
    if(!entry) entry = Fkadr->get_last_selected();
    show_entry_info(entry);
}

void __fastcall TTlmKadrEditor::FormCloseQuery(TObject *Sender, bool &CanClose)
{
   if(kadr_changes)
   {
    UnicodeString str;
    str.printf(L"Кадр %s\n\rСохранить изменения?",Caption.c_str());
    int res = MessageBox(Handle,str.c_str(),L"Закрытие редактора кадра",MB_YESNOCANCEL);
    switch(res)
    {
     case ID_YES:
          tbApplyClick(this);
     break;
     case ID_CANCEL:
          CanClose = false;
     break;
    }
   }
}
//---------------------------------------------------------------------------

class optimal_size_setter_t:protected tlmpaint::paint_action_t<TTlmKadrEditor>
{
  single_kadr * sk;
  public:
  optimal_size_setter_t(TTlmKadrEditor * kdr):
  tlmpaint::paint_action_t<TTlmKadrEditor>(kdr->paint_context,kdr->paint_area->Canvas->Handle,NULL,NULL,false,false,kdr)
  ,sk(kdr->get_kadr())
  {}

  optimal_size_setter_t(const optimal_size_setter_t & src):tlmpaint::paint_action_t<TTlmKadrEditor>(src),sk(src.sk)
  {}

  void operator() (mdb_kadr_entry & e){e.size<<calc_size(e);}
  void operator() (const DWORD idx   )
  {
   mdb_kadr_entry & e = sk->entryes.at(idx);
   (*this)(e);
  }
};

void __fastcall TTlmKadrEditor::miSetOptimalSizeClick(TObject *Sender)
{
  //Установка оптимального размера
  if(Fkadr->sel_count())
  {
   for_each(Fkadr->selected.begin(),Fkadr->selected.end(),optimal_size_setter_t(this));
   kadr_changes |= MDB_KADR_FIELD_ENTRYES;
   refresh_kadr();
  }
}
//---------------------------------------------------------------------------

void __fastcall TTlmKadrEditor::update_commands()
{
  bool sel_enabled  = Fkadr->sel_count();
  tbDelete->Enabled         = sel_enabled;
  miDelete->Enabled         = sel_enabled;
  tbArrageLeft->Enabled     = sel_enabled;
  miArrageLeft->Enabled     = sel_enabled;
  tbArangeTop->Enabled      = sel_enabled;
  miArrangeTop->Enabled     = sel_enabled;
  tbSameSize->Enabled       = sel_enabled;
  miSetOptimalSize->Enabled = sel_enabled;
  miSetSameSize->Enabled    = sel_enabled;
  miLayoutHorz->Enabled     = sel_enabled;
  miLayoutVert->Enabled     = sel_enabled;
  miLayoutInvisible->Enabled= sel_enabled;
}

void         __fastcall TTlmKadrEditor::update_paint_variants()
{
    if(entry_props && entry_props->discrete)
      {
        entry_props->update_paint_variants();
      }

}



void __fastcall TTlmKadrEditor::miSetKadrExtentAsBkgndClick(TObject *Sender)
{
  SIZE sz =  bkgnd.get_src_bitmap_size();
  if(Fkadr->kadr.kadr_size.cx != sz.cx || Fkadr->kadr.kadr_size.cy != sz.cy)
  {
   Fkadr->kadr.kadr_size << sz;
   reset_kadr(MDB_KADR_FIELD_SIZE);
   kadr_changes |= MDB_KADR_FIELD_SIZE;
  }
}
//---------------------------------------------------------------------------

void __fastcall TTlmKadrEditor::miMakeEntryCopyClick(TObject *Sender)
{
 //Копия текущего элемента
 //this->Fkadr
  mdb_kadr_entry src_entry =   *Fkadr->get_selected(0);
  mdb_kadr_entry new_entry =   src_entry;
  Fkadr->clear_selections();
  new_entry.position.x += std::max(2,bkgnd.get_grid());
  new_entry.position.y += std::max(2,bkgnd.get_grid());

  new_entry.number     = Fkadr->entryes.size();
  Fkadr->entryes.push_back(new_entry);
  Fkadr->position_index_resort();
  Fkadr->selections_restore(false);
  TWinControl * wc = get_owner_control();
  if(wc && wc->Handle)  SendMessage(wc->Handle,WM_NOTIFY_ENTRYES_CHANGED,0,LPARAM(Fkadr));
  kadr_changes|=MDB_KADR_FIELD_ENTRYES;
}
//---------------------------------------------------------------------------

