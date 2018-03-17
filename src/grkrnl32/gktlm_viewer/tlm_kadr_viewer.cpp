//---------------------------------------------------------------------------

#pragma hdrstop
#include <vcl.h>
#include "tlm_kadr_viewer.h"
#include "sel_period.h"
#include <otdm_proto.h>
#include <modem_proto.h>
#include "history_data.h"
#include "tarch_props.h"
#include "trend_forms.h"
#include "gktlm_viewer_mod.hpp"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "gktlm_viewer_form"
#pragma resource "*.dfm"
TTlmKadrViewer *TlmKadrViewer;
//---------------------------------------------------------------------------
__fastcall TTlmKadrViewer::TTlmKadrViewer(TComponent* Owner)
	: TTlmViewerForm(Owner)
{
}
__fastcall TTlmKadrViewer::TTlmKadrViewer(TComponent* Owner,TGKTlmViewer * vm)
	: TTlmViewerForm(Owner,vm)
{
 kadr = 0;
 Fcurrent_selected = Fcurrent_over = -1;
 arch_list = NULL;
 temp_file_stream  = NULL;
 temp_data_storage = NULL;
 Fdrag_active      = false;
}
//---------------------------------------------------------------------------

void   __fastcall TTlmKadrViewer::CreateParams(TCreateParams & cp)
{
 TForm::CreateParams(cp);
 cp.Style|=WS_CLIPCHILDREN;
 cp.ExStyle |= WS_EX_TOOLWINDOW	;
}

void __fastcall TTlmKadrViewer::WndProc(Winapi::Messages::TMessage & msg)
{
 switch(msg.Msg)
 {
  case TLMKV_WM_ARCHIVE_PRESENT:
        actShowHistory->Enabled  = painter->get_selected_count()  ? true : false;
        bRequestOscilogramm->Enabled = actShowHistory->Enabled;
        miRequestOscillogramm->Enabled = bRequestOscilogramm->Enabled;
        bArchList->Enabled = true;
  break;
  case TLMKV_HANDLE_KADR_ARCHIVE:
  {
      LPVOID ptr = (LPVOID)msg.LParam;
      handle_arch_list((LPOTDM_KADR_ARCHIVE_RESPOND)ptr,msg.WParam);
  }
  default : TTlmViewerForm::WndProc(msg);break;
 }
}



void    __fastcall TTlmKadrViewer::request_history_all   (UnicodeString hist_caption,LONGLONG beg_tm, LONGLONG end_tm,DWORD alarm)
{

   Tselected_list sl;
   painter->get_selected(sl);
   painter->select_all();
   request_history    (hist_caption,beg_tm,end_tm,alarm,NULL);
   painter->set_selected(sl);
}

void    __fastcall TTlmKadrViewer::rename_arch_click     (TObject * sender)
{
 TListItem * Item = arch_list  ? arch_list->ListView1->Selected : NULL;
 if(Item &&
   (
   (arch_list->is_all && temp_data_storage && Item->Index < (int)temp_data_storage->get_record_count())
   ||
   (!arch_list->is_all && Item->Index < (int)kadr->hist_times.size())
   )
   )
 {
   Thist_times kar(true,0,0);
   if(arch_list->is_all)
   temp_data_storage->get_record(Item->Index,&kar,sizeof(kar));
   else
   kar = kadr->hist_times.at(Item->Index);


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
        kadr->hist_times.at(Item->Index) = kar;
        kar.kadr_id = this->kadr->kadr_id;
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

        modem_addr ma = viewer_module->get_modem_source();
        mproto_init(mph,0,FA_OTD_MEDIUM_ALARM_ARCHIVE,data_size,&ma);
        omph = (LPOTDM_PROTO_HEADER)mph->data;
        omph->data_size = query_size;
        omph->command   = OTDMPROTO_CMD_ARCHIVE_SET_PROPS;
        arch_hdr   = (LPOTDM_ARCHIVE_HEADER)omph->data;
        arch_kar   = (LPOTDM_KADR_ARCHIVE )arch_hdr->data;
        memcpy(arch_kar,&kar,sizeof(*arch_kar));
        viewer_module->send_data(mph);
  }
  delete ap;
 }
}

void __fastcall    TTlmKadrViewer::request_arch_list (TObject * sender)
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
       if(viewer_module)
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

        modem_addr ma = viewer_module->get_modem_source();
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
        arch_query->rec_ids.rec_id[0] = this->kadr->kadr_id;
        viewer_module->send_data(mph);
       }
}

void __fastcall    TTlmKadrViewer::get_selected_recid (Tselected_list & rec_ids)
{
 painter->get_selected_recid(rec_ids);
}

void   __fastcall TTlmKadrViewer::get_selected_recid (TListItems * items,Tselected_list & rec_ids)
{
 if(items)
   {
    for(int i = 0;i<items->Count;i++)
        {
          TListItem * item = items->Item[i];
          if(item->Checked)
            {
             TTlmKadrViewer * tv = dynamic_cast<TTlmKadrViewer *>(reinterpret_cast<TTlmViewerForm *>(item->Data));
             if(tv) tv->get_selected_recid(rec_ids);
            }
        }
   }
   else
    painter->get_selected_recid(rec_ids);

}


void __fastcall    TTlmKadrViewer::request_history (UnicodeString hist_period, LONGLONG beg_tm, LONGLONG end_tm,DWORD alarm,TListItems * items)
{
    Tselected_list rec_ids;
    get_selected_recid(items,rec_ids);
    DWORD sel_count = rec_ids.size();
    if(sel_count)
    {
        LPMPROTO_HEADER        mph;
        LPOTDM_PROTO_HEADER    omph;
        LPOTDM_ARCHIVE_HEADER  arch_hdr;
        LPOTDM_ARCHIVE_QUERY   arch_query;
        DWORD query_size = sizeof(*arch_hdr)- sizeof(arch_hdr->data[0]) + sizeof(*arch_query)+sizeof(DWORD)*(sel_count-1);
        DWORD data_size  = sizeof(*omph)-sizeof(omph->data[0])+query_size;
        DWORD need_size  = data_size+sizeof(*mph)-sizeof(mph->data[0]);


        std::unique_ptr<BYTE, array_deleter_t<BYTE> > buf(new BYTE[need_size]);
        mph = (LPMPROTO_HEADER) buf.get();

        modem_addr ma = viewer_module->get_modem_source();
        mproto_init(mph,0,FA_OTD_MEDIUM_ALARM_ARCHIVE,data_size,&ma);
        omph = (LPOTDM_PROTO_HEADER)mph->data;
        omph->data_size = query_size;
        omph->command   = OTDMPROTO_CMD_QUERY_ARCHIVE;
        arch_hdr   = (LPOTDM_ARCHIVE_HEADER)omph->data;
        arch_query = (LPOTDM_ARCHIVE_QUERY )arch_hdr->data;

        arch_query->alarm           =   alarm;
        arch_query->period.beg_time =   beg_tm;
        arch_query->period.end_time =   end_tm;
        arch_query->period.end_time+=   MSEC_NS100(10000); //+ 5 секунд
        arch_query->rec_ids.count = sel_count;
        Tselected_list::iterator rptr = rec_ids.begin(),rend = rec_ids.end();
        LPDWORD ids = arch_query->rec_ids.rec_id;
        while(rptr<rend)
        {
          *ids = *rptr;
          ++ids;
          ++rptr;
        }
        THistoryForm * hd  = NULL;
        try{
            THistoryForm * _hd = new THistoryForm(this,viewer_module,Caption,hist_period);
            hd = _hd;
            hd->request_proc   = request_history_all;
            hd->on_get_text    = get_record_text;
            hd->on_get_record  = get_record;

            hd->Visible        = true;
            hd->alarm          = alarm;
           }

        catch(Exception & e)
        {
         ShowMessage(e.Message);
        }
        catch(Exception *& e)
        {
         ShowMessage(e->Message);
        }
        arch_hdr->hist_id = (QWORD)hd;
        viewer_module->send_data(mph);
    }

}

void __fastcall TTlmKadrViewer::fill_opened_kadrs(TListItems * items)
{
  items->BeginUpdate();
  TGKTlmViewer::locker l(viewer_module->get_mutex());
  Tkadrs & kadrs_ordered = viewer_module->get_kadrs_ordered();
  Tkadrs::iterator kptr = kadrs_ordered.begin();
  Tkadrs::iterator kend = kadrs_ordered.end();
  while(kptr<kend)
  {
      if((*kptr)->tv)
         {
           TListItem * item = items->Add();
           item->Caption =    (*kptr)->tv->Caption;
           item->Data    =    (LPVOID)(*kptr)->tv;
           if((*kptr)->tv == this)
              item->Checked = true;
         }
      ++kptr;
  }
  items->EndUpdate();
}

void __fastcall TTlmKadrViewer::actShowHistoryExecute(TObject *Sender)
{
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
        kadr->ready_flags&=~(RDY_FLAG_ALARM_ARCHIVE|RDY_FLAG_ARCHIVE);
        TTlmViewerForm * ready_painter = viewer_module->get_ready_painter();
        if(ready_painter)
           ready_painter->Repaint();
   }
   delete sp;
}
//---------------------------------------------------------------------------

void __fastcall TTlmKadrViewer::actSelectAllExecute(TObject *Sender)
{
  painter->select_all();
}
//---------------------------------------------------------------------------


void    __fastcall TTlmKadrViewer::do_clean              (Tselected_list & lst)
{
  Tselected_list::iterator ptr = lst.begin(),end = lst.end();
  Trecords::iterator rec_ptr;
  TGKTlmViewer::locker l (viewer_module->get_mutex());
  Trecords & records = viewer_module->get_records();

  while(ptr<end)
  {
   mdb_kadr_entry ke;
   painter->get_kadr_entry(*ptr,ke);
   mdb_record_ex rec(ke.rec_id);
   if(stdlib_utils::binary_search(records,rec,&rec_ptr))
   {
     if(lst.size() == 1 &&  !rec_ptr->changes_count && !(rec_ptr->state&MDBR_STATE_FLASH) )
         {
          rec_ptr->state  |= MDBR_STATE_FLASH;
         }
         else
	 rec_ptr->clean();
         viewer_module->get_changed_records()[rec_ptr->rec_id]|= MDBR_FIELD_STATE;
   }
   ++ptr;
  }

  //Refresh();
  TTlmViewerForm * ready_painter = viewer_module->get_ready_painter();
  if(ready_painter)
   {
    PostMessage(ready_painter->Handle,TLMKV_WM_RESCAN_READY,0,kadr->kadr_id);
    ready_painter->repaint_changed_records();
   }
  repaint_changed_records();
  update_status_bar();

}

void    __fastcall TTlmKadrViewer::do_receipt               (Tselected_list & lst)
{
  Tselected_list::iterator ptr = lst.begin(),end = lst.end();
  Trecords::iterator rec_ptr;

  TGKTlmViewer::locker l (viewer_module->get_mutex());
  Trecords & records = viewer_module->get_records();
  while(ptr<end)
  {
   mdb_kadr_entry ke;
   painter->get_kadr_entry(*ptr,ke);
   mdb_record_ex rec(ke.rec_id);
   if(stdlib_utils::binary_search(records,rec,&rec_ptr))
   {
     if(lst.size() == 1 && rec_ptr->is_normal_state()  && !rec_ptr->changes_count  )
        rec_ptr->unreceipt();
        else
        rec_ptr->receipt();
        viewer_module->get_changed_records()[rec_ptr->rec_id]|= MDBR_FIELD_STATE;
   }
   ++ptr;
  }


  TTlmViewerForm * ready_painter = viewer_module->get_ready_painter();
  if(ready_painter)
  {
   PostMessage(ready_painter->Handle,TLMKV_WM_RESCAN_READY,0,kadr->kadr_id);
   PostMessage(ready_painter->Handle,TLMKV_WM_RECORD_CHANGES,0,0);
  }
  repaint_changed_records();
  update_status_bar();
}

void __fastcall TTlmKadrViewer::repaint_changed_records()
{
  TTlmViewerForm::repaint_changed_records();
  update_status_bar();
  painter->Repaint(true);
}



void __fastcall TTlmKadrViewer::actCleanExecute(TObject *Sender)
{
  //Съем того что в соответствии
 Tselected_list sl;
 bool is_shift_pressed =  (GetKeyState(VK_LSHIFT)&0x8000) || (GetKeyState(VK_RSHIFT)&0x8000) ? true : false;
 bool has_sel = painter->get_selected(sl) ? true : false;
 if(!has_sel || is_shift_pressed)
   {
    painter->select_all(false);
    painter->get_selected(sl);
    painter->select_clean(false);
   }
 do_clean(sl);
}
//---------------------------------------------------------------------------

void __fastcall TTlmKadrViewer::actKvitExecute(TObject *Sender)
{
  //Квитанция
 Tselected_list sl;
 bool is_shift_pressed =  (GetKeyState(VK_LSHIFT)&0x8000) || (GetKeyState(VK_RSHIFT)&0x8000) ? true : false;
 bool has_sel = painter->get_selected(sl) ? true : false;
 if(!has_sel || is_shift_pressed)
   {
    Tselected_list save_sl;
    painter->get_selected(save_sl);
    painter->select_all(false);
    painter->get_selected(sl);
    painter->set_selected(save_sl);
   }
 do_receipt(sl);
}
//---------------------------------------------------------------------------


void __fastcall  TTlmKadrViewer::size_for_kadr_size (LONG cx, LONG cy, int & form_width,int & form_height)
{
  form_width   = cx ;//+ (GetSystemMetrics(CM_CYDLGFRAME)<<1);
  form_width  += (GetSystemMetrics(SM_CXEDGE)<<1);
  form_height  = cy + ToolBar1->Height + StatusBar1->Height;
  form_height += (GetSystemMetrics(SM_CYEDGE)<<1);
}


 DWORD __fastcall TTlmKadrViewer::on_mouse_over   (TTlmPainter * p,DWORD entry  )
 {
   DWORD   over_id = painter->get_entry_rec_id(entry);
   TGKTlmViewer::locker l (viewer_module->get_mutex());
   Trecords & records = viewer_module->get_records();
   Trecords::iterator rptr;
   if(stdlib_utils::binary_search(records,mdb_record_ex(over_id),&rptr))
   {
     if(rptr->is_visible() || (painter && painter->get_show_hidden()) )
       current_over = over_id;
       else
       current_over = -1;

   }
   else
   current_over = -1;

   return TRUE;
 }

 BOOL  __fastcall TTlmKadrViewer::on_mouse_buttons(TTlmPainter * p,Tmouse_data * md)
 {
   if((md->mbuttons&MOUSE_BUTTON_RIGHT) && !md->down)
   {
     System::Types::TPoint p;
     p.x =  md->pt.x;
     p.y =  md->pt.y;
     p = PainterPanel->ClientToScreen(p);
     PopupMenu1->Popup(p.x,p.y);
     return TRUE;
   }
  return FALSE;
 }

void  __fastcall TTlmKadrViewer::get_record_status_text (DWORD rec_id,UnicodeString & str)
{
  TGKTlmViewer::locker l (viewer_module->get_mutex());
  Trecords & records = viewer_module->get_records();
  Trecords::iterator rptr;
  if(stdlib_utils::binary_search(records,mdb_record_ex(rec_id),&rptr))
  {
     UnicodeString rec_text;
     if(rptr->name!=rptr->sname)
        get_record_text(this,rec_id,rec_text);
     str.printf(L"ID-%u ",rptr->rec_id);
     if(rptr->is_discrete())
     {
      str.cat_printf(L"[ %s ]",DWORD(rptr->value) ? L" ВКЛ":L"ОТКЛ");
      //str.cat_printf(L" [н.п=%s,изм=%d]",rptr->get_normal_value() ? L" ВКЛ":L"ОТКЛ",rptr->changes_count);
     }
     else
     str.cat_printf(L"[ %.3f ]",rptr->value);

     str.cat_printf(L" %s |",rec_text.c_str());
     str.cat_printf(L" otd-addr %03hd.%03hd.%03hd.%03hd-%03hd"
                   ,(short)rptr->addr.addr.pu
                   ,(short)rptr->addr.addr.cp
                   ,(short)rptr->addr.addr.fa
                   ,(short)rptr->addr.addr.sb
                   ,(short)rptr->addr.param_number
                   );

     str.cat_printf(L" DIAG %02d(%s) OTD-DIAG %04X ",rptr->diag,rptr->diag ? L"BAD":L"OK",rptr->otd_pd);
     UnicodeString  state_str = get_state_text(rptr->state&MDBR_STATE_DYN_MASK);
     if(!state_str.IsEmpty())
         str.cat_printf(L"[S=%04X : %s]",rptr->state,state_str.c_str());


  }
}

DWORD __fastcall TTlmKadrViewer::on_select_entry (TTlmPainter * p,DWORD entry  )
{

  DWORD new_selected    = painter->get_entry_rec_id(entry);
  bool tu_enabled = false;
  mdb_record_ex rec;
  if(current_selected!=(DWORD)-1 &&  get_record(this,current_selected,false,rec) )
  {
    if(rec.state & (MDBR_STATE_TUTR_ACTIVE|MDBR_STATE_TUTR_PREPARE))
       select_tutr(current_selected,false);
  }
  current_selected = new_selected;

  if(painter->get_selected_count() == 1 && current_selected != (DWORD) -1)
    {

      if(get_record(this,current_selected,false,rec) )
      {
       tu_enabled = (rec.state & MDBR_STATE_TUFLAG) ? true : false;

      }

     actTuBegin->Enabled      =  tu_enabled;
     actRecordInfo->Enabled   =  true;
     actWorkInOut->Enabled    =  rec.is_discrete();
     //actClearTUError->Enabled =  rec.is_tu_error();
    }
    else
    {
     actTuBegin->Enabled    = false;
     actTuOnMore->Enabled   = false;
     actTuOffLess->Enabled  = false;
     actRecordInfo->Enabled = false;
     actWorkInOut->Enabled  = false;
    }

  return TRUE;
}

 void  __fastcall TTlmKadrViewer::update_status_bar     ()
 {
    UnicodeString str;
    DWORD rec_id = current_over!=(DWORD)-1 ? current_over: current_selected;
    get_record_status_text(rec_id,str);
    StatusBar1->Panels->Items[0]->Text = str;
 }

void __fastcall TTlmKadrViewer::check_archive_present()
{
           OTDM_ARCHIVE_HEADER ah;
           ah.hist_id =(QWORD)this;
           ah.data[0] = 0;
           modem_addr ma = viewer_module->get_modem_source();
           send_command(ma,FA_OTD_MEDIUM_ALARM_ARCHIVE,false,false,OTDMPROTO_CMD_ARCHIVE_PRESENT,0,&ah,sizeof(ah));

}
 void  __fastcall TTlmKadrViewer::set_current_selected  (DWORD rec_id)
 {
    if(current_selected !=rec_id)
    {
      Fcurrent_selected = rec_id;
      update_status_bar();
      //actShowHistory->Enabled =   ? true : false;
      if(painter->get_selected_count())
      {
      if( !actShowHistory->Enabled)
          {
           check_archive_present();
          }
      }
      else
      {
       actShowHistory->Enabled = false;
       bRequestOscilogramm->Enabled = false;
       bArchList->Enabled = false;

      }
    }
 }

 void  __fastcall TTlmKadrViewer::set_current_over      (DWORD rec_id)
 {
   if(current_over!=rec_id)
   {
    UnicodeString str;
    Fcurrent_over = rec_id;
    update_status_bar();
   }
 }

 void   __fastcall TTlmKadrViewer::update_kadr  (DWORD kadr_id,DWORD mask)
 {
   TTlmViewerForm::update_kadr  (kadr_id, mask);
   update_status_bar();
 }







void __fastcall TTlmKadrViewer::actRefreshExecute(TObject *Sender)
{
  //Отправить вызов
  kadr->timer = 0;
  modem_addr ma = viewer_module->get_modem_source();
  kadr_order(ma,kadr->kadr_id,true);
}
//---------------------------------------------------------------------------


void __fastcall TTlmKadrViewer::FormActivate(TObject *Sender)
{
 set_kadr_ready_flags(this->kadr,RDY_FLAG_SELECTED,RDY_FLAG_FLASH_NAME);
 if(painter)
 {
   painter->SetFocus();
   painter->SetActive();
 }
}
//---------------------------------------------------------------------------

void __fastcall TTlmKadrViewer::FormDeactivate(TObject *Sender)
{
 set_kadr_ready_flags(this->kadr,0,RDY_FLAG_SELECTED);
}
//---------------------------------------------------------------------------

void __fastcall TTlmKadrViewer::tbShowHiddenClick(TObject *Sender)
{
 painter->show_hidden(tbShowHidden->Down);
}
//---------------------------------------------------------------------------



void __fastcall TTlmKadrViewer::list_view_on_data(TObject *Sender, TListItem *Item)
{
  Thist_times hs(true,0,0);
  TGKTlmViewer::locker l (viewer_module->get_mutex());
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
  if(Item && Item->Index < (int)kadr->hist_times.size())
    hs = kadr->hist_times.at(Item->Index);
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

void __fastcall TTlmKadrViewer::bArchListClick(TObject *Sender)
{
 arch_list = new TArchList(this);
 arch_list->ListView1->OnData      = list_view_on_data;
 arch_list->TabControl1->OnChange  = arch_list_tab_change;
 arch_list->ViewBtn->OnClick       = arch_list_view_click;
 arch_list->DelBtn->OnClick        = arch_list_del;
 arch_list->ClearBtn->OnClick      = arch_list_del;
 arch_list->Icon->Assign(Icon);
 arch_list->end_date->Date         = TDateTime::CurrentDate();
 arch_list->beg_date->Date         = TDateTime::CurrentDate()-3;
 arch_list->bRefresh->OnClick      = request_arch_list;
 arch_list->bRename->OnClick       = rename_arch_click;


 arch_list_tab_change(this);
 arch_list->Position = poDesigned;
 System::Types::TPoint p = bArchList->ClientOrigin;
 p.y += bArchList->Height;
 //p = bArchList->ClientToScreen(p);
 arch_list->Left = p.x;
 arch_list->Top  = p.y;
 arch_list->Visible = true ;
 arch_list->OnDeactivate     = arch_list_deactivate;

}
//---------------------------------------------------------------------------

void __fastcall TTlmKadrViewer::arch_list_tab_change(TObject *Sender)
{

  arch_list->Panel1->Visible = !arch_list->is_all;
  arch_list->Panel2->Visible = !arch_list->Panel1->Visible;

  if(arch_list->is_all)
  {
    //Отправить запрос при необходимости

    if(!temp_data_storage )
       {
        request_arch_list(this);
       }
       else
       arch_list->ListView1->Items->Count = temp_data_storage->get_record_count();
  }
  else
  {
    TGKTlmViewer::locker l (viewer_module->get_mutex());
    arch_list->ListView1->Items->Count = kadr->hist_times.size();
  }
}

void __fastcall TTlmKadrViewer::arch_list_deactivate(TObject *Sender)
{
  if(arch_list)
  {
   delete arch_list;
   arch_list = NULL;
  }
}

void __fastcall TTlmKadrViewer::arch_list_view_click(TObject *Sender)
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
    if(arch_list->ListView1->Selected->Index < (int)kadr->hist_times.size())
       ht =  kadr->hist_times.at( arch_list->ListView1->Selected->Index );
   }

   if(ht.beg_time)
   {
      request_history(Caption,ht.beg_time-MSEC_NS100(viewer_module->get_params().history_time_before  ),ht.end_time,ht.alarms);
   }
 }
}

void __fastcall TTlmKadrViewer::arch_list_del(TObject *Sender)
{
  TGKTlmViewer::locker l (viewer_module->get_mutex());
  if(Sender == arch_list->ClearBtn)
  {
    kadr->hist_times.clear();
  }
  else
  {
   if(arch_list->ListView1->Selected && arch_list->ListView1->Selected->Index < (int)kadr->hist_times.size())
   {
    Thist_times_list::iterator ptr = kadr->hist_times.begin();
    advance(ptr,arch_list->ListView1->Selected->Index);
    kadr->hist_times.erase(ptr);
   }
  }
  kadr->ready_flags &=~(RDY_FLAG_ALARM_ARCHIVE|RDY_FLAG_ARCHIVE);

  update_kadr(kadr->kadr_id,MDB_KADR_FIELD_STATE);

  arch_list_tab_change(Sender);
}

void    __fastcall TTlmKadrViewer::handle_arch_list (LPOTDM_KADR_ARCHIVE_RESPOND kar,BOOL end)
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


void __fastcall TTlmKadrViewer::clear_temp_storage()
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

void __fastcall TTlmKadrViewer::FormClose(TObject *Sender, TCloseAction &Action)
{
   Action = caFree;
   clear_temp_storage();
}
//---------------------------------------------------------------------------

void __fastcall TTlmKadrViewer::actSelAllDiscreteExecute(TObject *Sender)
{
  painter->select_discrete();
 }
//---------------------------------------------------------------------------

void __fastcall TTlmKadrViewer::actSelAnalogsExecute(TObject *Sender)
{
 painter->select_analogs();
}
//---------------------------------------------------------------------------

void __fastcall TTlmKadrViewer::bRequestOscilogrammClick(TObject *Sender)
{
   //Отправка команды на заказ осцилограммы


   UnicodeString value;
   if(InputQuery(L"Запрос осцилограммы",L"Задайте разумную длительность в миллисекундах",value))
   {
    BYTE                     buff[512];
    bzero(buff,sizeof(buff));
    LPMPROTO_HEADER          mph  = (LPMPROTO_HEADER)buff;
    LPOTDM_PROTO_HEADER      omph = (LPOTDM_PROTO_HEADER)mph->data;
    LPOTDM_PROTO_OSCPARAM    oscp = (LPOTDM_PROTO_OSCPARAM)omph->data;
    omph->data_size               = sizeof(*oscp);
    omph->command                 = OTDMPROTO_CMD_START_OSCILLOGRAMM|OTDM_ENDDATA;

    oscp->rec_id     = current_selected;
    oscp->osc_lenght = _wtoi(value.c_str());
    if(!oscp->osc_lenght)  oscp->osc_lenght = 1000;
    modem_addr ma = viewer_module->get_modem_source();
    mproto_init(mph,0,FA_OTD_MEDIUM,sizeof(*oscp)+sizeof(*omph),&ma);
    viewer_module->send_data(mph);
   }
}
//---------------------------------------------------------------------------



void __fastcall TTlmKadrViewer::actTuOnMoreExecute(TObject *Sender)
{
  //ТУ включить/ТР больше

  modem_addr ma = viewer_module->get_modem_source();
  OTDM_PROTO_TUOPERATION tuop;
  bzero(&tuop,sizeof(tuop));
  tuop.rec_id  = current_selected;
  tuop.op_code = OTD_TUOP_ON;
  viewer_module->send_command(ma,FA_OTD_MEDIUM,true,true,OTDMPROTO_CMD_TUOPERATION,0,&tuop,sizeof(tuop));
  disable_tutr_acts(false);
}
//---------------------------------------------------------------------------

void __fastcall TTlmKadrViewer::actTuOffLessExecute(TObject *Sender)
{
  //ТУ отключить / ТР меньше
  modem_addr ma = viewer_module->get_modem_source();
  OTDM_PROTO_TUOPERATION tuop;
  bzero(&tuop,sizeof(tuop));
  tuop.rec_id  = current_selected;
  tuop.op_code = OTD_TUOP_OFF;
  viewer_module->send_command(ma,FA_OTD_MEDIUM,true,true,OTDMPROTO_CMD_TUOPERATION,0,&tuop,sizeof(tuop));
  disable_tutr_acts(false);

}
//---------------------------------------------------------------------------

void __fastcall TTlmKadrViewer::actTuCancelExecute(TObject *Sender)
{
 // ТУ/ТР отмена
  modem_addr ma = viewer_module->get_modem_source();
  OTDM_PROTO_TUOPERATION tuop;
  bzero(&tuop,sizeof(tuop));
  tuop.rec_id  = current_selected;
  tuop.op_code = OTD_TUTR_CMDDESELECT;
  viewer_module->send_command(ma,FA_OTD_MEDIUM,true,true,OTDMPROTO_CMD_TUOPERATION,0,&tuop,sizeof(tuop));

   {
       Trecords::iterator rec_ptr;
       TGKTlmViewer::locker l (viewer_module->get_mutex());
       Trecords & records = viewer_module->get_records();
       mdb_record_ex rec(current_selected);
       if(stdlib_utils::binary_search(records,rec,&rec_ptr) && (rec_ptr->state & (MDBR_STATE_TUTR_ACTIVE|MDBR_STATE_TUTR_PREPARE) ) )
       {
        tuop.op_code = OTD_TUTROP_CANCEL;
        viewer_module->send_command(ma,FA_OTD_MEDIUM,true,true,OTDMPROTO_CMD_TUOPERATION,0,&tuop,sizeof(tuop));
       }
   }
  disable_tutr_acts(true);

}
//---------------------------------------------------------------------------
void __fastcall TTlmKadrViewer::disable_tutr_acts(bool all)
{
  actTuOffLess->Enabled = false;
  actTuOnMore->Enabled  = false;
  if(all)
     actTuCancel->Enabled  = false;

}

void __fastcall TTlmKadrViewer::enable_tutr_acts()
{
  actTuOffLess->Enabled = true;
  actTuOnMore->Enabled  = true;
  actTuCancel->Enabled  = true;
}

void __fastcall TTlmKadrViewer::select_tutr(DWORD rec_id,bool _select)
{
  modem_addr ma = viewer_module->get_modem_source();
  OTDM_PROTO_TUOPERATION tuop;
  bzero(&tuop,sizeof(tuop));
  tuop.rec_id  = current_selected;
  tuop.op_code = _select ? OTD_TUTR_CMDSELECT : OTD_TUTR_CMDDESELECT;
  viewer_module->send_command(ma,FA_OTD_MEDIUM,true,true,OTDMPROTO_CMD_TUOPERATION,0,&tuop,sizeof(tuop));
  if(_select)
  {
      Trecords::iterator ptr;
      TGKTlmViewer::locker l (viewer_module->get_mutex());
      Trecords & records = viewer_module->get_records();
      mdb_record_ex rec(rec_id);
      if(stdlib_utils::binary_search(records,rec,&ptr))
      {
         ptr->receipt();
         viewer_module->get_changed_records()[rec_id] |=  MDBR_FIELD_STATE;
         repaint_changed_records();
      }
  }
}

void __fastcall TTlmKadrViewer::actTuBeginExecute(TObject *Sender)
{
  select_tutr(current_selected,true);
  this->enable_tutr_acts();
}
//---------------------------------------------------------------------------

void __fastcall TTlmKadrViewer::tbNoGrayBkgndClick(TObject *Sender)
{
    not_gray_bkgnd = tbNoGrayBkgnd->Down;
}
//---------------------------------------------------------------------------

void __fastcall TTlmKadrViewer::tbFixedClick(TObject *Sender)
{
  this->fixed = tbFixed->Down;
}
//---------------------------------------------------------------------------


void __fastcall TTlmKadrViewer::actRecordInfoExecute(TObject *Sender)
{
  if(current_over != (DWORD) -1)
  {
   mdb_record_ex rec;
   get_record(this,current_over,false,rec);
   set_record_info(rec);
  }
}
//---------------------------------------------------------------------------

void __fastcall TTlmKadrViewer::actWorkInOutExecute(TObject *Sender)
{
  modem_addr ma = viewer_module->get_modem_source();
  mdb_record_ex rec;
  OTDM_PROTO_WORK_INOUT wio;
  get_record(this,current_selected,false,rec);
  wio.rec_id = rec.rec_id;
  wio.worked = !rec.is_worked();
  viewer_module->send_command(ma,FA_OTD_MEDIUM,true,true,OTDMPROTO_CMD_WORK_INOUT,0,&wio,sizeof(wio));

}
//---------------------------------------------------------------------------

void __fastcall TTlmKadrViewer::actClearTUErrorExecute(TObject *Sender)
{
  //Снять неисправность ТУ
  modem_addr ma = viewer_module->get_modem_source();
  mdb_record_ex rec;
  OTDM_PROTO_WORK_INOUT wio;
  TGKTlmViewer::locker l (viewer_module->get_mutex());
  //Trecords & records = viewer_module->get_records();
  Tentryes_list::iterator ptr = kadr->entryes.begin();
  Tentryes_list::iterator end = kadr->entryes.end();
  while(ptr<end)
  {
    if(get_record(this,ptr->rec_id,false,rec) && rec.is_tu_error())
       viewer_module->send_command(ma,FA_OTD_MEDIUM,true,true,OTDMPROTO_CMD_CLEAR_TU_ERROR,0,&rec.rec_id,sizeof(rec.rec_id));
    ++ptr;
  }
}
//---------------------------------------------------------------------------

 BOOL  __fastcall TTlmKadrViewer::on_mouse_move(TTlmPainter * p,Tmouse_data * md)
 {
  if((md->mbuttons & MOUSE_BUTTON_LEFT) &&  current_over != (DWORD )-1 )
     {
       if(!Fdrag_active )
       {
        PainterPanel->BeginDrag(false,10);
        return FALSE;
       }
     }
  return FALSE;
 }



void __fastcall TTlmKadrViewer::PainterPanelStartDrag(TObject *Sender, TDragObject *&DragObject)

{
     DragObject = new TRecDrag(this);
     DragObject->AlwaysShowDragImages = true;
     DragObject->RightClickCancels    = true;
}
//---------------------------------------------------------------------------

void __fastcall TTlmKadrViewer::PainterPanelEndDrag(TObject *Sender, TObject *Target,
          int X, int Y)
{
  X = X;
}
//---------------------------------------------------------------------------

void __fastcall TTlmKadrViewer::PainterPanelDragDrop(TObject *Sender, TObject *Source,
          int X, int Y)
{
  X = X;
}
//---------------------------------------------------------------------------

void __fastcall TTlmKadrViewer::actNewTrendExecute(TObject *Sender)
{
   TTrendForm * tf = new TTrendForm(Owner);
   tf->chart_frame->viewer_module = viewer_module;
   tf->Icon->Assign(Icon);
   tf->Visible = true;
   tf->SetFocus() ;

   Application->ProcessMessages() ;
   TTlmViewerForm * ready_painter = viewer_module->get_ready_painter();

   if(tf->Top < ready_painter->Height)
      tf->Top = ready_painter->Height+2;
   if(current_selected != (DWORD) -1)
        tf->add_selected(this);

}
//---------------------------------------------------------------------------

