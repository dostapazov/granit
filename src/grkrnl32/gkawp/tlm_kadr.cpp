//---------------------------------------------------------------------------
#pragma hdrstop
#include <vcl.h>
#include "tlm_kadr.h"
#include "sel_period.h"
#include "history_data.h"
#include "trend_forms.h"
#include "arch_list.h"
#include "tarch_props.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "tlm_painter_form"
#pragma resource "*.dfm"
TTlmKadrForm *TlmKadrForm;
//---------------------------------------------------------------------------
__fastcall TTlmKadrForm::TTlmKadrForm(TComponent* Owner)
	: TTlmPainterForm(Owner),module(NULL)
{
}
//---------------------------------------------------------------------------
__fastcall TTlmKadrForm::TTlmKadrForm(TComponent* Owner,TGkAwpModule * _module)
	: TTlmPainterForm(Owner),module(_module)
{
}
//---------------------------------------------------------------------------
void      __fastcall TTlmKadrForm::AfterConstruction()
{
  TTlmPainterForm::AfterConstruction();
  Feditable   = false;
  drag_active = false;
  arch_list   = NULL;
  temp_data_storage = NULL;
  temp_file_stream  = NULL;
}
//---------------------------------------------------------------------------

void      __fastcall TTlmKadrForm::BeforeDestruction()
{
    clear_temp_storage() ;
    TTlmPainterForm::BeforeDestruction();
}
//---------------------------------------------------------------------------



void __fastcall TTlmKadrForm::FormClose(TObject *Sender, TCloseAction &Action)
{
  Action = caFree;
}
//---------------------------------------------------------------------------

void      __fastcall TTlmKadrForm::reset_kadr  (DWORD mask )
{
   if(mask&MDB_KADR_FIELD_DIAG)
   {
    bkgnd.set_grayed((Fkadr->kadr.kadr_diag && !disable_bk_grayed));
   }
   TTlmPainterForm::reset_kadr(mask);
}

bool      __fastcall TTlmKadrForm::get_disable_bk_grayed()
{
   return tbNoGrayBkgnd->Down;
}
void      __fastcall TTlmKadrForm::set_disable_bk_grayed(bool val)
{
     if(disable_bk_grayed !=val)
       {
         tbNoGrayBkgnd->Down = val;
         reset_kadr(0);
       }

}

void      __fastcall TTlmKadrForm::set_current_over(DWORD val)
{
  if(val != DWORD(-1))
  {
    mdb_kadr_entry * entry = Fkadr->get_by_offset(val);
    mdb_record rec;
    bool is_visible = false;
    if(entry->options.part_of_background || !get_record(entry->rec_id,rec,&is_visible) || !is_visible)
        val = -1;
  }
  TTlmPainterForm::set_current_over(val);
}

bool      __fastcall TTlmKadrForm::get_record   (DWORD rec_id,MDB_RECORD & rec,bool * rec_visible) const
{
   bool ret = false;
   bool is_visible = false;
   mdb_record * mdb_rec = (mdb_record *) &rec;
   if(module && module->get_record(rec_id,*mdb_rec))
      {
        if(mdb_rec->is_caution() )
           is_visible =  !is_record_normal(mdb_rec);
           else
             is_visible =  is_record_visible(&rec) || this->tbShowHidden->Down || rec.diag;
        ret = true;
      }
   if(rec_visible) *rec_visible = is_visible;
   return ret;
}
//---------------------------------------------------------------------------

UnicodeString   __fastcall TTlmKadrForm::get_text     (DWORD str_id) const
{
  UnicodeString str;
  if(module) module->get_string(str_id,str);
  return str;
}
//---------------------------------------------------------------------------
UnicodeString   __fastcall  TTlmKadrForm::get_image_file()
{
   UnicodeString str;
   if(module)
    str = module->get_kadr_image_file(Fkadr);
   return str;
}

bool      __fastcall TTlmKadrForm::is_entry_selectable(mdb_kadr_entry * entry)
{
  if(entry && !entry->options.part_of_background)
  {
    bool rec_visible = false;
    record_t rec;
    if(get_record(entry->rec_id,rec,&rec_visible))
       return rec_visible;
  }
  return false;
}

void      __fastcall  TTlmKadrForm::select_changed  (DWORD offset,bool selected)
{
    TTlmPainterForm::select_changed(offset,selected);
    DWORD sel_count               = Fkadr->sel_count();
    actWorkInout->Enabled         = sel_count;
    actTuErrorClear->Enabled      = sel_count;
    tbRequestOscilogramm->Enabled = sel_count;
    bool rc_beg_enabled = false;
    TGkAwpModule::locker l(module->get_mutex());
    mdb_kadr_entry * entry = Fkadr->get_by_offset(offset);
    if(selected )
      {
        if(sel_count == 1 && entry)
        {
        mdb_record rec;
        rc_beg_enabled = ( module->get_record(entry->rec_id,rec) &&
                          MDBR_STATE_TUFLAG == (rec.state&(MDBR_STATE_TUFLAG | MDBR_STATE_TUTR_PREPARE | MDBR_STATE_TUTR_ACTIVE ))
                          ) ? true : false;
        }
      }
      else
      {
        if(entry && actRcUndo->Enabled)
         remote_control_undo(entry->rec_id);

      }

      actRcBegin->Enabled   = rc_beg_enabled;
      actRcOffLess->Enabled = false;
      actRcOnMore->Enabled  = false;
      actRcUndo->Enabled    = false;
}

void __fastcall TTlmKadrForm::tbShowHiddenClick(TObject *Sender)
{
  refresh_kadr() ;
}
//---------------------------------------------------------------------------
void  __fastcall TTlmKadrForm::get_record_status_text (mdb_record & rec,UnicodeString & str)
{
     UnicodeString rec_text;
     if(rec.name!=rec.sname)
        rec_text = get_name(rec,false);
     str.printf(L"ID-%u ",rec.rec_id);
     if(rec.is_discrete())
     {
      str.cat_printf(L"[ %s ]",DWORD(rec.value) ? L" ВКЛ":L"ОТКЛ");
     }
     else
     str.cat_printf(L"[ %9.3f ]",rec.value);

     str.cat_printf(L" %s |",rec_text.c_str());
     str.cat_printf(L" otd-addr %03hd.%03hd.%03hd.%03hd-%03hd"
                   ,(short)rec.addr.addr.pu
                   ,(short)rec.addr.addr.cp
                   ,(short)rec.addr.addr.fa
                   ,(short)rec.addr.addr.sb
                   ,(short)rec.addr.param_number
                   );

     str.cat_printf(L" DIAG %02d(%s) OTD-DIAG %04X ",rec.diag,rec.diag ? L"BAD":L"OK",rec.otd_pd);
     UnicodeString  state_str = TGkAwpModule::get_state_text(rec.state&MDBR_STATE_DYN_MASK);
     if(!state_str.IsEmpty())
         str.cat_printf(L"[S=%04X : %s]",rec.state,state_str.c_str());
}

void __fastcall  TTlmKadrForm::show_entry_info       (mdb_kadr_entry *entry)
{
   UnicodeString text;
   if(entry && module)
     {
       mdb_record rec;
       if(module->get_record(entry->rec_id,rec))
            {
             get_record_status_text(rec,text);
             if(IsDebuggerPresent())
               text.cat_printf(L" EN=%d",entry->number);
            }
     }
     else
     {

        kadr_t * kadr = (kadr_t *)Fkadr;
        if(kadr)
        {
          text.cat_printf(L"Кадр ID-%03d %s",kadr->kadr.kadr_id,(kadr->kadr.kadr_diag && !module->is_source_connected()) ? L" нет связи с сервером":L"");
          if(IsDebuggerPresent())
             text.cat_printf(L" ,ready_state = %08X ch_count %d, new_ch_count %d",kadr->ready_state,kadr->changes,kadr->new_changes_count);

        }
     }
   StatusBar1->Panels->Items[0]->Text = text;
}
//---------------------------------------------------------------------------

void __fastcall TTlmKadrForm::tbRefreshClick(TObject *Sender)
{
  module->kadr_order((kadr_t*)Fkadr,true);
}
//---------------------------------------------------------------------------

void __fastcall TTlmKadrForm::tbNoGrayBkgndClick(TObject *Sender)
{
  reset_kadr(0);
}
//---------------------------------------------------------------------------

void __fastcall TTlmKadrForm::tbFixedClick(TObject *Sender)
{
 Ffixed = tbFixed->Down;
}
//---------------------------------------------------------------------------


void __fastcall TTlmKadrForm::miRecordDetailsClick(TObject *Sender)
{

  if(Fkadr->sel_count() )
      module->show_record_info(Fkadr->get_last_selected()->rec_id);
}
//---------------------------------------------------------------------------
void __fastcall TTlmKadrForm::paint_entryes(bool erase,bool flash)
{
  set_bk_grayed( (Fkadr->kadr.kadr_diag && !tbNoGrayBkgnd->Down));
  TTlmPainterForm::paint_entryes(erase,flash);
}
//---------------------------------------------------------------------------
struct treceipt_clean_act
{
  bool           receipt;
  bool           single;
  TGkAwpModule * module;
  kadr_t       * sk;
  treceipt_clean_act (bool _receipt,bool _single,TGkAwpModule * _module,kadr_t * _sk):receipt(_receipt),single(_single),module(_module),sk(_sk){}
  treceipt_clean_act (const treceipt_clean_act& src ):receipt(src.receipt),single(src.single),module(src.module),sk(src.sk){}

  void operator()(mdb_kadr_entry & e)
  {
    if(module)
    {
      if(receipt)
      module->receipt_record(e.rec_id,single);
      else
      module->clear_record  (e.rec_id,single);
    }
  }

  void operator()(DWORD   offset)
  {
    if(sk) (*this)(sk->entryes.at(offset));
  }

};

void __fastcall TTlmKadrForm::tbReceiptClick(TObject *Sender)
{
  bool is_shift = ((GetAsyncKeyState(VK_RSHIFT)|GetAsyncKeyState(VK_LSHIFT))&0x8000) ? true : false;
  treceipt_clean_act action(Sender == tbReceipt ? true : false,(!is_shift && Fkadr->sel_count() == 1 ),module,(kadr_t*)Fkadr);
  TGkAwpModule::locker l(module->get_mutex());
  if(is_shift)
      for_each(Fkadr->entryes.begin(),Fkadr->entryes.end(),action);
     else
     for_each(Fkadr->selected.begin(),Fkadr->selected.end(),action);
   module->kadr_reset_changes((kadr_t*)Fkadr);

}
//---------------------------------------------------------------------------
//void __fastcall TTlmKadrViewer::actWorkInOutExecute(TObject *Sender)
//{
//  modem_addr ma = viewer_module->get_modem_source();
//  mdb_record_ex rec;
//  OTDM_PROTO_WORK_INOUT wio;
//  get_record(this,current_selected,false,rec);
//  wio.rec_id = rec.rec_id;
//  wio.worked = !rec.is_worked();
//  viewer_module->send_command(ma,FA_OTD_MEDIUM,true,true,OTDMPROTO_CMD_WORK_INOUT,0,&wio,sizeof(wio));
//}
////---------------------------------------------------------------------------
//
//void __fastcall TTlmKadrViewer::actClearTUErrorExecute(TObject *Sender)
//{
//  //Снять неисправность ТУ
//  modem_addr ma = viewer_module->get_modem_source();
//  mdb_record_ex rec;
//  OTDM_PROTO_WORK_INOUT wio;
//  TGKTlmViewer::locker l (viewer_module->get_mutex());
//  //Trecords & records = viewer_module->get_records();
//  Tentryes_list::iterator ptr = kadr->entryes.begin();
//  Tentryes_list::iterator end = kadr->entryes.end();
//  while(ptr<end)
//  {
//    if(get_record(this,ptr->rec_id,false,rec) && rec.is_tu_error())
//       viewer_module->send_command(ma,FA_OTD_MEDIUM,true,true,OTDMPROTO_CMD_CLEAR_TU_ERROR,0,&rec.rec_id,sizeof(rec.rec_id));
//    ++ptr;
//  }
//}


void __fastcall TTlmKadrForm::actWorkInoutExecute(TObject *Sender)
{
  records_ids_t recs;
  if(module->make_kadr_selected_rec_ids(Fkadr,recs))
  {
     records_ids_t::iterator rec_beg = recs.begin();
     records_ids_t::iterator rec_end = recs.end  ();
     while(rec_beg != rec_end)
     {
        mdb_record rec;
        if(module->get_record(*rec_beg,rec))
        {
          OTDM_PROTO_WORK_INOUT wio;
          wio.rec_id = rec.rec_id;
          wio.worked = !rec.is_worked();
          module->send_command(FA_OTD_MEDIUM,true,true,OTDMPROTO_CMD_WORK_INOUT,0,&wio,sizeof(wio));
        }
        ++rec_beg;
     }
   }
}
//---------------------------------------------------------------------------

void __fastcall TTlmKadrForm::actTuErrorClearExecute(TObject *Sender)
{
  records_ids_t recs;
  if(module->make_kadr_selected_rec_ids(Fkadr,recs))
  {
     records_ids_t::iterator rec_beg = recs.begin();
     records_ids_t::iterator rec_end = recs.end  ();
     while(rec_beg != rec_end)
     {
        mdb_record rec;
        if(module->get_record(*rec_beg,rec)&& rec.is_tu_error())
          module->send_command(FA_OTD_MEDIUM,true,true,OTDMPROTO_CMD_CLEAR_TU_ERROR,0,&rec.rec_id,sizeof(rec.rec_id));
      ++rec_beg;
     }
  }

}
//---------------------------------------------------------------------------

void __fastcall TTlmKadrForm::FormShow(TObject *Sender)
{
   set_history_present(module->history_is_present());
   TTlmPainterForm::FormShow(Sender);
}
//---------------------------------------------------------------------------

void      __fastcall  TTlmKadrForm::fill_opened_kadrs(TListItems * items)
{
   items->BeginUpdate();
   TListItem * item = items->Add();
   item->Checked = true;
   item->Caption = Caption;
   item->Data    = Fkadr;
   kadrs_t kadrs;
   if(module->get_opened_kadrs(kadrs))
   {
      kadrs_t::iterator ptr = kadrs.begin();
      kadrs_t::iterator end = kadrs.end  ();
      while(ptr<end)
      {
        kadr_t * kdr = *ptr;
        if(kdr->user_data != this)
          {
           TListItem * item = items->Add();
           item->Caption = kdr->get_kadr_form()->Caption;
           item->Data    = kdr;
          }
        ++ptr;
      }

   }
   items->EndUpdate  ();
}

void __fastcall TTlmKadrForm::actShowHistoryExecute(TObject *Sender)
{
 //Запрос истории
   TSelPeriod * sp = new TSelPeriod(this);
   fill_opened_kadrs(sp->OpenedKadrs->Items);
   if(sp->ShowModal()== mrOk)
   {
        QWORD beg_tm = sp->get_begin_timestamp();
        QWORD end_tm = sp->get_end_timestamp  ();
        DWORD alarm = -1;
        if(sp->RadioGroup1->ItemIndex)
           alarm = sp->RadioGroup1->ItemIndex-1;
         request_history(sp->get_period_text(),beg_tm,end_tm,alarm,sp->OpenedKadrs->Items);
         Fkadr->ready_state&=~(rds_alarm_history);
   }
   delete sp;
}
//---------------------------------------------------------------------------
UnicodeString make_request_history_data(TListItems * items,records_ids_t & rec_ids)
{
  UnicodeString str;
  TListItem * itm;
  for(int idx = 0;idx<items->Count;idx++)
  {
    itm = items->Item[idx];
    if(itm->Checked)
    {
     if(!str.IsEmpty()) str+=L", ";
     str+=itm->Caption;
     kadr_t * kdr = (kadr_t *)itm->Data;
     kdr->make_selected_rec_ids(rec_ids);
    }
  }
 return str;
}

void __fastcall TTlmKadrForm::request_history(UnicodeString hist_capt,UnicodeString period_text,QWORD tm_beg,QWORD tm_end,DWORD alarm,records_ids_t & rec_ids)
{
    if(rec_ids.size())
    {
     THistoryForm * hd = new THistoryForm(this,module,hist_capt,period_text);
     hd->on_get_text    =  get_record_name;
     hd->on_get_record  =  get_record;
     module->request_history((QWORD)hd,tm_beg,tm_end,alarm,rec_ids);
     hd->Visible = true;
    }
}

void __fastcall TTlmKadrForm::request_history(UnicodeString period_text,QWORD tm_beg,QWORD tm_end,DWORD alarm,TListItems * items)
{
    records_ids_t  rec_ids;
    UnicodeString str;
    {
     TGkAwpModule::locker(module->get_mutex());
     str  =  make_request_history_data(items,rec_ids);
    }
    request_history(str,period_text,tm_beg,tm_end,alarm,rec_ids);
}


void __fastcall TTlmKadrForm::FormDestroy(TObject *Sender)
{
  if(Fkadr && Fkadr->user_data == this)
     Fkadr->user_data = NULL;
}
//---------------------------------------------------------------------------


void __fastcall TTlmKadrForm::actRcBeginExecute(TObject *Sender)
{
  //Начать ТУ
  TGkAwpModule::locker (module->get_mutex());
  mdb_kadr_entry * entry = Fkadr->get_last_selected();
  if(entry)
     remote_control_select(entry->rec_id,true);
     else
     actRcBegin->Enabled = false;
}
//---------------------------------------------------------------------------
void __fastcall TTlmKadrForm::remote_control_select(DWORD rec_id,bool _select)
{
  OTDM_PROTO_TUOPERATION tuop;
  bzero(&tuop,sizeof(tuop));
  tuop.rec_id  = rec_id;
  tuop.op_code = _select ? OTD_TUTR_CMDSELECT : OTD_TUTR_CMDDESELECT;
  module->send_command(FA_OTD_MEDIUM,true,true,OTDMPROTO_CMD_TUOPERATION,0,&tuop,sizeof(tuop));
//  if(_select)
//  {
//      Trecords::iterator ptr;
//      TGKTlmViewer::locker l (viewer_module->get_mutex());
//      Trecords & records = viewer_module->get_records();
//      mdb_record_ex rec(rec_id);
//      if(stdlib_utils::binary_search(records,rec,&ptr))
//      {
//         ptr->receipt();
//         viewer_module->get_changed_records()[rec_id] |=  MDBR_FIELD_STATE;
//         repaint_changed_records();
//      }
//  }
}

 void      __fastcall TTlmKadrForm::remote_control_undo  (DWORD rec_id)
 {
     record_t rec;
     if(module->get_record(rec_id,rec))
     {
      if((rec.state & MDBR_STATE_TUTR) == MDBR_STATE_TUTR_PREPARE)
         remote_control_select(rec_id,false);
         else
         {
          //TODO отправить команду отмены
         }
     }
 }


 void      __fastcall TTlmKadrForm::remote_control_enable(DWORD rec_id,bool enable)
 {
   TGkAwpModule::locker (module->get_mutex());
   mdb_kadr_entry * entry = Fkadr->get_last_selected();
   if(entry && entry->is_selected() && entry->rec_id == rec_id)
      {
        actRcBegin->Enabled   = !enable;
        actRcOffLess->Enabled =  enable;
        actRcOnMore->Enabled  =  enable;
        actRcUndo->Enabled    =  enable;
        module->clear_record  (entry->rec_id,false);
        module->receipt_record(entry->rec_id,false);
        module->receipt_record(entry->rec_id,true);
      }
 }



 void      __fastcall TTlmKadrForm::WndProc(TMessage & msg)
 {
    switch(msg.Msg)
    {
      case WM_RC_ENABLE        : remote_control_enable(msg.WParam,msg.LParam);
      break;
      case WM_RC_STATE_CHANGE  : remote_control_state_change(msg.WParam);
      break;
      case WM_HISTORY_PRESENT  : set_history_present(bool(msg.WParam));
      break;
      case WM_HANDLE_KADR_ARCHIVE:
                                handle_kadr_archives((LPOTDM_KADR_ARCHIVE_RESPOND)msg.LParam,bool(msg.WParam));
      break;
      default          :TTlmPainterForm::WndProc(msg);
      break;
    }
 }



void __fastcall TTlmKadrForm::actRcUndoExecute(TObject *Sender)
{
  TGkAwpModule::locker l(module->get_mutex());
  mdb_kadr_entry * entry = Fkadr->get_last_selected();
  if(entry)
     remote_control_undo(entry->rec_id);
}
//---------------------------------------------------------------------------
void      __fastcall TTlmKadrForm::remote_control_state_change(DWORD rec_id)
{
  TGkAwpModule::locker l(module->get_mutex());
  mdb_kadr_entry * entry = Fkadr->get_last_selected();
  if(entry && entry->rec_id == rec_id)
    {
     mdb_record rec;
     if(module->get_record(rec_id,rec) && !(rec.state & MDBR_STATE_TUTR))
        select_changed(Fkadr->get_last_selected_offset(),true);
    }
}
//---------------------------------------------------------------------------
void      __fastcall TTlmKadrForm::remote_control_on_more (DWORD rec_id)
{
  OTDM_PROTO_TUOPERATION tuop;
  bzero(&tuop,sizeof(tuop));
  tuop.rec_id  = rec_id;
  tuop.op_code = OTD_TUOP_ON;
  module->send_command(FA_OTD_MEDIUM,true,true,OTDMPROTO_CMD_TUOPERATION,0,&tuop,sizeof(tuop));
  remote_control_enable(rec_id,false);
  actRcBegin->Enabled   = false;
  actRcOffLess->Enabled =  false;
  actRcOnMore->Enabled  =  false;
}
//---------------------------------------------------------------------------

void      __fastcall TTlmKadrForm::remote_control_off_less(DWORD rec_id)
{
  OTDM_PROTO_TUOPERATION tuop;
  bzero(&tuop,sizeof(tuop));
  tuop.rec_id  = rec_id;
  tuop.op_code = OTD_TUOP_OFF;
  module->send_command(FA_OTD_MEDIUM,true,true,OTDMPROTO_CMD_TUOPERATION,0,&tuop,sizeof(tuop));
  actRcBegin->Enabled   = false;
  actRcOffLess->Enabled =  false;
  actRcOnMore->Enabled  =  false;

}
//---------------------------------------------------------------------------


void __fastcall TTlmKadrForm::actRcOnMoreExecute(TObject *Sender)
{
  TGkAwpModule::locker l(module->get_mutex());
  mdb_kadr_entry * entry = Fkadr->get_last_selected();
  if(entry)
     remote_control_on_more(entry->rec_id);
}
//---------------------------------------------------------------------------

void __fastcall TTlmKadrForm::actRcOffLessExecute(TObject *Sender)
{
  TGkAwpModule::locker l(module->get_mutex());
  mdb_kadr_entry * entry = Fkadr->get_last_selected();
  if(entry)
     remote_control_off_less(entry->rec_id);
}
//---------------------------------------------------------------------------
void      __fastcall TTlmKadrForm::start_drag         (TObject *Sender, TDragObject *&DragObject)
{
     DragObject = new TRecDrag(this);
}

void __fastcall  TTlmKadrForm::captured_mouse_move(TShiftState Shift, int X, int Y )
{
    if(!drag_active )
       {
        if( current_over == DWORD(-1) )
         {
          TTlmPainterForm::captured_mouse_move(Shift,X,Y);
          return;
         }
       }

    if(Fkadr && Fkadr->sel_count() &&  !drag_active)
    {
      ToolBar1->OnStartDrag = start_drag;
      ToolBar1->BeginDrag  (false,get_captured_delta());
      drag_active = true;
    }
}

void __fastcall TTlmKadrForm::end_drag()
{
  captured = false;
  mstate.mb_left.set_state(false,0,0);
  drag_active = false;
}




void __fastcall TTlmKadrForm::miNewTrendClick(TObject *Sender)
{
 TTrendForm * tf = new TTrendForm(Owner,module);
 tf->Icon->Assign(Icon);
 tf->add_selected(this);
 tf->Visible = true;
}
//---------------------------------------------------------------------------

void __fastcall TTlmKadrForm::FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)

{
   switch(Key)
   {
     case     VK_SPACE : module->kadr_reset_changes((kadr_t*)Fkadr); break;
     case     VK_F5    :
     {
     TAction * act;
     if(Shift.Contains(ssCtrl))
        act = actRcBegin;
       else
         act = actRcBegin;
      if(act->Enabled) act->Execute();
     }
     break;
     case VK_F7:  if(actRcOnMore->Enabled ) actRcOnMore ->Execute(); break;
     case VK_F9:  if(actRcOffLess->Enabled) actRcOffLess->Execute(); break;
     default: TTlmPainterForm::FormKeyDown(Sender,Key,Shift);
     break;
   }
}
//---------------------------------------------------------------------------

void __fastcall TTlmKadrForm::tbAlarmHistoryListClick(TObject *Sender)
{
  //
 arch_list = new TArchList(this);
 arch_list->ListView1->OnData      = arch_list_view_on_data;
 arch_list->TabControl1->OnChange  = arch_list_tab_change;
 arch_list->ViewBtn->OnClick       = arch_list_view_click;
 arch_list->DelBtn->OnClick        = arch_list_del;
 arch_list->ClearBtn->OnClick      = arch_list_del;
 arch_list->Icon->Assign(Icon);
 arch_list->end_date->Date         = TDateTime::CurrentDate();
 arch_list->beg_date->Date         = TDateTime::CurrentDate()-3;
 arch_list->bRefresh->OnClick      = arch_list_request;
 arch_list->bRename->OnClick       = arch_list_rename;


 arch_list_tab_change(this);
 arch_list->Position = poDesigned;
 System::Types::TPoint p = tbAlarmHistoryList->ClientOrigin;
 p.y += tbAlarmHistoryList->Height;
 //p = bArchList->ClientToScreen(p);
 arch_list->Left = p.x;
 arch_list->Top  = p.y;
 arch_list->Visible = true ;
 arch_list->OnDeactivate     = arch_list_deactivate;

}
//---------------------------------------------------------------------------

void __fastcall TTlmKadrForm::arch_list_tab_change(TObject *Sender)
{

  arch_list->Panel1->Visible = !arch_list->is_all;
  arch_list->Panel2->Visible = !arch_list->Panel1->Visible;

  if(arch_list->is_all)
  {
    //Отправить запрос при необходимости
    if(!temp_data_storage )
       {
        arch_list_request(this);
       }
       else
       arch_list->ListView1->Items->Count = temp_data_storage->get_record_count();
  }
  else
  {
    TGkAwpModule::locker l (module->get_mutex());
    arch_list->ListView1->Items->Count = ((kadr_t*)Fkadr)->get_hist_times().size();
  }
}

void __fastcall TTlmKadrForm::arch_list_deactivate(TObject *Sender)
{
  if(arch_list)
  {
   delete arch_list;
   arch_list = NULL;
  }
}

void __fastcall TTlmKadrForm::arch_list_view_click(TObject *Sender)
{
 if(arch_list->ListView1->Selected  )
 {
   Thist_times ht(true,0,0);
   if(arch_list->is_all)
   {
    if(temp_data_storage && DWORD(arch_list->ListView1->Selected->Index) < temp_data_storage->get_record_count())
    {
     OTDM_KADR_ARCHIVE kar;
     temp_data_storage->get_record(arch_list->ListView1->Selected->Index,&kar,sizeof(kar));
     ht = Thist_times(true,kar.beg_time,kar.end_time);
    }
   }
   else
   {
    hist_times_t &hist_times = ((kadr_t*)Fkadr)->get_hist_times();
    if(arch_list->ListView1->Selected->Index < (int)hist_times.size())
       ht =  hist_times.at( arch_list->ListView1->Selected->Index );
   }

   if(ht.beg_time)
   {
      UnicodeString period_text;
      records_ids_t  rec_ids;
      {
       TGkAwpModule::locker l(module->get_mutex());
       Fkadr->make_selected_rec_ids(rec_ids);
      }

      request_history(Caption,period_text,ht.beg_time-MSEC_NS100(1000),ht.end_time,ht.alarms,rec_ids);
   }
 }
}

void __fastcall TTlmKadrForm::arch_list_del(TObject *Sender)
{
  TGkAwpModule::locker l (module->get_mutex());
  hist_times_t & hist_times = ((kadr_t*)Fkadr)->get_hist_times();
  if(Sender == arch_list->ClearBtn)
  {
    hist_times.clear();
  }
  else
  {
   if(arch_list->ListView1->Selected && arch_list->ListView1->Selected->Index < (int)hist_times.size())
   {
    hist_times_t::iterator ptr = hist_times.begin();
    advance(ptr,arch_list->ListView1->Selected->Index);
    hist_times.erase(ptr);
   }
  }

  Fkadr->ready_state &=~(rds_alarm_history);

  //update_kadr(kadr->kadr_id,MDB_KADR_FIELD_STATE);

  arch_list_tab_change(Sender);
}

void    __fastcall TTlmKadrForm::arch_list_handle (LPOTDM_KADR_ARCHIVE_RESPOND kar,BOOL end)
{
      LPOTDM_KADR_ARCHIVE ka_beg = kar->krec;
      LPOTDM_KADR_ARCHIVE ka_end = ka_beg+kar->count;
      if(temp_data_storage)
      {
      while(ka_beg<ka_end)
       temp_data_storage->add_record(ka_beg++,sizeof(*ka_beg));
      if(arch_list && arch_list->TabControl1->TabIndex)
         arch_list->ListView1->Items->Count = temp_data_storage->get_record_count();
      }
}


void __fastcall TTlmKadrForm::clear_temp_storage()
{
  if(temp_data_storage)
     {
      temp_data_storage->clear();
      delete temp_data_storage;
      temp_data_storage = NULL;
     }
   if(temp_file_stream)
   {
    delete temp_file_stream;
    temp_file_stream = NULL;
    DeleteFile(temp_file_name);
   }

}

void __fastcall TTlmKadrForm::arch_list_view_on_data(TObject *Sender, TListItem *Item)
{
  Thist_times hs(true,0,0);
  TGkAwpModule::locker l (module->get_mutex());
  if(arch_list->is_all)
  {
    if(Item && temp_data_storage && Item->Index < (int)temp_data_storage->get_record_count())
    {
     OTDM_KADR_ARCHIVE kar;
     temp_data_storage->get_record(Item->Index,&kar,sizeof(kar));
     hs = Thist_times(true,kar.beg_time,kar.end_time,kar.protect,kar.arch_name);
    }
    else
    return;
  }
  else
  {
   hist_times_t & hist_times = ((kadr_t*)Fkadr)->get_hist_times() ;
   if(Item && Item->Index < (int)hist_times.size())
     hs = hist_times.at(Item->Index);
    else
    return;
  }

  UnicodeString text;
  THistoryForm::get_time_text(hs.beg_time,text,false);
  Item->Caption = text;
  THistoryForm::get_time_text(hs.end_time,text,false);
  Item->SubItems->Add(text);
  Item->SubItems->Add(hs.protect ? L"*":L"");
  text = UnicodeString(hs.arch_name,sizeof(hs.arch_name));
  Item->SubItems->Add(text);
}

void __fastcall    TTlmKadrForm::arch_list_request (TObject * sender)
{
        if(!temp_data_storage)
        {
          if(temp_file_name.IsEmpty())
          {
           wchar_t temp_dir[MAX_PATH<<1];
           GetTempPathW(KERTL_ARRAY_COUNT(temp_dir),temp_dir);
           temp_file_name = temp_dir;
           temp_file_name.cat_printf(L"%p.hist.tmp",this);
          }
          temp_file_stream  = new KeRTL::TFileStream(temp_file_name.c_str());
          temp_data_storage = new KeRTL::TStorage(temp_file_stream);
          temp_data_storage->create(1,"ARCHHIST",8,8192);
        }
        temp_data_storage->clear();
       if(module)
       {
        LPMPROTO_HEADER        mph;
        LPOTDM_PROTO_HEADER    omph;
        LPOTDM_ARCHIVE_HEADER  arch_hdr;
        LPOTDM_ARCHIVE_QUERY   arch_query;
        DWORD query_size = sizeof(*arch_hdr)- sizeof(arch_hdr->data[0]) + sizeof(*arch_query)/*+sizeof(DWORD)*/;
        DWORD data_size  = sizeof(*omph)-sizeof(omph->data[0])+query_size;
        DWORD need_size  = data_size+sizeof(*mph)-sizeof(mph->data[0]);

//        auto_ptr<BYTE> buf(new BYTE[need_size]);
//        memset(buf.get(),0,need_size-query_size);
//        mph = (LPMPROTO_HEADER) buf.get();

        std::unique_ptr<BYTE, array_deleter_t<BYTE> > buf(new BYTE[need_size]);
        mph = (LPMPROTO_HEADER) buf.get();

        modem_addr ma = module->get_modem_source();
        mproto_init(mph,0,FA_OTD_MEDIUM_ALARM_ARCHIVE,data_size,&ma);
        omph = (LPOTDM_PROTO_HEADER)mph->data;
        omph->data_size = query_size;
        omph->command   = OTDMPROTO_CMD_QUERY_KADR_ARCHIVES;
        arch_hdr   = (LPOTDM_ARCHIVE_HEADER)omph->data;
        arch_query = (LPOTDM_ARCHIVE_QUERY )arch_hdr->data;

        arch_hdr->hist_id           =  (QWORD)this;
        arch_query->alarm           =  TRUE;
        TDateTime tm(0,0,0,0);
        TDateTime dt = this->arch_list->beg_date->Date;
        arch_query->period.beg_time =  TSelPeriod::get_timestamp(dt,tm);
        dt = arch_list->end_date->Date;
        dt++;
        arch_query->period.end_time = TSelPeriod::get_timestamp(dt,tm);
        arch_query->rec_ids.count   =  1;
        arch_query->rec_ids.rec_id[0] = Fkadr->kadr.kadr_id;
        module->send_data(mph);
       }
}

void    __fastcall TTlmKadrForm::arch_list_rename     (TObject * sender)
{
 TListItem * Item = arch_list  ? arch_list->ListView1->Selected : NULL;
 hist_times_t & hist_times = ((kadr_t*)Fkadr)->get_hist_times();
 if(Item &&
   (
   (arch_list->is_all && temp_data_storage && Item->Index < (int)temp_data_storage->get_record_count())
   ||
   (!arch_list->is_all && Item->Index < (int)hist_times.size())
   )
   )
 {
   Thist_times kar(true,0,0);
   if(arch_list->is_all)
   temp_data_storage->get_record(Item->Index,&kar,sizeof(kar));
   else
   kar = hist_times.at(Item->Index);


  TArchProps * ap = new TArchProps(arch_list);
  ap->Edit1->Text        = arch_list->get_arch_name();
  ap->CheckBox1->Checked = arch_list->get_protect();
  if(mrOk == ap->ShowModal())
  {
   kar.protect = ap->CheckBox1->Checked;
   AnsiString str = ap->Edit1->Text;
   strncpy(kar.arch_name,str.c_str(),sizeof(kar.arch_name));
   if(arch_list->is_all)
      temp_data_storage->modify_record(Item->Index,&kar,sizeof(kar));
   else{
        hist_times.at(Item->Index) = kar;
        kar.kadr_id = Fkadr->kadr.kadr_id;
       }

   arch_list->ListView1->Repaint();

        LPMPROTO_HEADER        mph;
        LPOTDM_PROTO_HEADER    omph;
        LPOTDM_ARCHIVE_HEADER  arch_hdr;
        LPOTDM_KADR_ARCHIVE    arch_kar;
        DWORD query_size = sizeof(*arch_hdr)- sizeof(arch_hdr->data[0]) + sizeof(*arch_kar)/*+sizeof(DWORD)*/;
        DWORD data_size  = sizeof(*omph)-sizeof(omph->data[0])+query_size;
        DWORD need_size  = data_size+sizeof(*mph)-sizeof(mph->data[0]);

        //auto_ptr<BYTE> buf(new BYTE[need_size]);
        //memset(buf.get(),0,need_size-query_size);
        std::unique_ptr<BYTE, array_deleter_t<BYTE> > buf(new BYTE[need_size]);
        mph = (LPMPROTO_HEADER) buf.get();

        modem_addr ma = module->get_modem_source();
        mproto_init(mph,0,FA_OTD_MEDIUM_ALARM_ARCHIVE,data_size,&ma);
        omph = (LPOTDM_PROTO_HEADER)mph->data;
        omph->data_size = query_size;
        omph->command   = OTDMPROTO_CMD_ARCHIVE_SET_PROPS;
        arch_hdr   = (LPOTDM_ARCHIVE_HEADER)omph->data;
        arch_kar   = (LPOTDM_KADR_ARCHIVE )arch_hdr->data;
        memcpy(arch_kar,&kar,sizeof(*arch_kar));
        module->send_data(mph);
  }
  delete ap;
 }
}

void      __fastcall TTlmKadrForm::handle_kadr_archives  (LPOTDM_KADR_ARCHIVE_RESPOND respond,bool end_data)
{
//  hist_times_t & hist_times = ((kadr_t*)Fkadr)->get_hist_times();
  LPOTDM_KADR_ARCHIVE kar_beg = respond->krec;
  LPOTDM_KADR_ARCHIVE kar_end = kar_beg+respond->count;
  if(temp_data_storage)
  {
      while(kar_beg<kar_end)
       temp_data_storage->add_record(kar_beg++,sizeof(*kar_beg));
      if(arch_list && arch_list->TabControl1->TabIndex)
          arch_list->ListView1->Items->Count = temp_data_storage->get_record_count();
  }
}

DWORD     __fastcall TTlmKadrForm::make_caution_list     (records_t & list)
{
   TGkAwpModule::locker l (module->get_mutex());
   kadr_entryes_table::iterator ptr = Fkadr->entryes.begin();
   kadr_entryes_table::iterator end = Fkadr->entryes.end  ();
   while(ptr<end)
   {
    record_t rec;
    if(module->get_record(ptr->rec_id,rec) && rec.is_caution())
        list.push_back(rec);
    ++ptr;
   }
  return list.size();
}

void      __fastcall TTlmKadrForm::on_caution_click      (TObject * sender)
{
   TMenuItem * item = dynamic_cast<TMenuItem *>(sender);
   if(item )
   {
      if(item->Checked)
          remote_control_off_less(item->Tag);
          else
          remote_control_on_more (item->Tag);
   }
}


DWORD     __fastcall TTlmKadrForm::make_caution_menu     (TMenuItem * items)
{
  items->Clear();
  records_t   list;
  if(make_caution_list(list))
  {
   records_t::iterator ptr = list.begin();
   records_t::iterator end = list.end  ();
   while(ptr<end)
   {
     TMenuItem * item = new TMenuItem(items);
     item->Caption = this->get_record_name(ptr->rec_id,true);

     item->Checked = ptr->dw_value ? true : false;
     item->Tag     = ptr->rec_id;
     item->OnClick = on_caution_click;
     items->Add(item);
     ++ptr;
   }

  }
 return list.size();
}


void __fastcall TTlmKadrForm::CautionsPopupPopup(TObject *Sender)
{
 make_caution_menu(CautionsPopup->Items);
}
//---------------------------------------------------------------------------


void __fastcall TTlmKadrForm::tbRequestOscilogrammClick(TObject *Sender)
{
   UnicodeString value;
   if(Fkadr->sel_count() && InputQuery(L"Запрос осцилограммы",L"Задайте разумную длительность в миллисекундах",value))
   {
    BYTE                     buff[512];
    bzero(buff,sizeof(buff));
    LPMPROTO_HEADER          mph  = (LPMPROTO_HEADER)buff;
    LPOTDM_PROTO_HEADER      omph = (LPOTDM_PROTO_HEADER)mph->data;
    LPOTDM_PROTO_OSCPARAM    oscp = (LPOTDM_PROTO_OSCPARAM)omph->data;
    omph->data_size               = sizeof(*oscp);
    omph->command                 = OTDMPROTO_CMD_START_OSCILLOGRAMM|OTDM_ENDDATA;
    {
     TGkAwpModule::locker l (module->get_mutex());
     mdb_kadr_entry * entry = Fkadr->get_last_selected();
     if(!entry) return;
     oscp->rec_id     =  entry->rec_id;
    }
     oscp->osc_lenght = _wtoi(value.c_str());
     if(!oscp->osc_lenght)  oscp->osc_lenght = 1000;
     modem_addr ma = module->get_modem_source();
     mproto_init(mph,0,FA_OTD_MEDIUM,sizeof(*oscp)+sizeof(*omph),&ma);
     module->send_data(mph);
   }
}
//---------------------------------------------------------------------------

