//---------------------------------------------------------------------------
#pragma hdrstop

#include "awp_main_form.h"
#include "ready_painter.h"
#include "tlm_kadr.h"
#include "record_info.h"
#include <vector_index.hpp>



//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "tlm_painter_form"
#pragma resource "*.dfm"
TAwpMainForm *AwpMainForm;
//---------------------------------------------------------------------------
__fastcall TAwpMainForm::TAwpMainForm(TComponent* Owner)
	: TTlmPainterForm(Owner),module(NULL),about(NULL)
        ,Fready_space(2)
        ,Fready_equal_size(false)
        ,ready_font(NULL)
        ,ready_state_font(NULL)
        ,ready_painter   (NULL)

{
  Fmulty_selected  = false;
  Ffixed = true;
}
//---------------------------------------------------------------------------
__fastcall TAwpMainForm::TAwpMainForm(TComponent* Owner,TGkAwpModule * mod)
	: TTlmPainterForm(Owner),module(mod),about(NULL)
        ,Fready_space(2)
        ,Fready_equal_size(false)
        ,ready_font(NULL)
        ,ready_state_font(NULL)
        ,ready_painter   (NULL)

{
  Fmulty_selected  = false;
  Ffixed = true;
  miDebug->Visible = IsDebuggerPresent();
}

void __fastcall    TAwpMainForm::release_ready_painter()
{
   if(ready_painter) delete ready_painter;
      ready_painter = NULL;
}

//---------------------------------------------------------------------------

single_kadr     * __fastcall TAwpMainForm::get_kadr         (DWORD kadr_id)
{
  if(module)
     return module->get_kadr(kadr_id);
  return NULL;
}

bool      __fastcall TAwpMainForm::get_record   (DWORD rec_id,MDB_RECORD & rec,bool * rec_visible) const
{
   rec.rec_id = rec_id;
   if(rec_visible) *rec_visible = true;
   return true;
}


//---------------------------------------------------------------------------

ready_painter_t * __fastcall TAwpMainForm::get_ready_painter()
{
   if(!ready_painter)
         {
          ready_painter = new ready_painter_t(this->ready_font->Handle,this->ready_state_font->Handle,&ready_colors);
          ready_painter->on_get_text = on_get_text;
          ready_painter->on_get_kadr = get_kadr;
         }
   return ready_painter;
}


void    __fastcall TAwpMainForm::set_window_monitor(int mon_idx,bool no_layout)
{
    Forms::TMonitor * mon ;
    if(mon_idx>= 0 && mon_idx < Screen->MonitorCount)
         mon = Screen->Monitors[mon_idx];
     else
        mon = this->Monitor;
   Left  = mon->WorkareaRect.Left;
   Top   = mon->WorkareaRect.Top;
   Width = mon->WorkareaRect.Width();
   if(Fkadr) { Fkadr->kadr.kadr_size.cx = paint_area->Width; }
   if(!no_layout) this->layout_ready();
}

void __fastcall TAwpMainForm::tbMainMenuClick(TObject *Sender)
{
  System::Types::TPoint pt = tbMainMenu->ClientOrigin;
  pt.y += tbMainMenu->Height;
  MainPopup->Popup(pt.x,pt.y);
}
//---------------------------------------------------------------------------

void __fastcall TAwpMainForm::miParametersClick(TObject *Sender)
{
  if(module)
     module->param_window_show();
}
//---------------------------------------------------------------------------


void __fastcall TAwpMainForm::about_close(TObject *Sender,TCloseAction & ca)
{
  ca = caFree;
  if(Sender == about)  about = NULL;
}

void __fastcall  TAwpMainForm::set_form_screen_position     (TForm * frm)
{
    if(frm)
    {
     TRect r = Monitor->WorkareaRect;
     r.top    += Height;
     r.bottom -= Height;
     frm->Position = poDesigned;
     frm->Top   = r.top  + (r.Height() - frm->Height)/2;
     frm->Left  = r.left + (r.Width () - frm->Width )/2;
     if(frm->Top< (Top+Height) ) frm->Top = Top+Height+1;
    }
}

void __fastcall TAwpMainForm::miAboutClick(TObject *Sender)
{
   if(!about)
      {
        about = new TAboutForm(this);
        set_form_screen_position(about);
        about->OnClose = about_close;
        about->Visible = true;
      }
        about->SetFocus();
}
//---------------------------------------------------------------------------

void __fastcall TAwpMainForm::FormDestroy(TObject *Sender)
{
  hot_key_register(HOTKEY_ID_CLEAR_ALL  ,0);
  hot_key_register(HOTKEY_ID_RECEIPT_ALL,0);
  release_ready_painter();
  if(ready_font      ) delete ready_font;
  if(ready_state_font) delete ready_state_font;
}
//---------------------------------------------------------------------------
 void __fastcall  TAwpMainForm::set_ready_font       (const LOGFONTW & lf,bool state,bool no_layout)
 {
   HFONT font = CreateFontIndirectW(&lf);
   if(font){
      TFont * & fnt = state ? this->ready_state_font : ready_font;
      if(!fnt) fnt =  new TFont;
      fnt->Handle = font;
      release_ready_painter();
      if(!no_layout)  layout_ready();
   }
 }

 void __fastcall  TAwpMainForm::set_ready_colors     (const READY_COLORS & rclrs)
 {
   memcpy(&ready_colors,&rclrs,sizeof(ready_colors));
   release_ready_painter();
   if(Visible) refresh_kadr();
 }

void __fastcall  TAwpMainForm::update_ready_sizes   (LPMDB_GSIZE  max_size)
{
 ready_painter_t * rp =  get_ready_painter();
 if(max_size)
 max_size->cx = max_size->cy = 0;
 kadr_entryes_table::iterator e_beg =  Fkadr->entryes.begin(),e_end = Fkadr->entryes.end();
 kadr_entryes_table::iterator e_ptr = e_beg;
 while(e_ptr<e_end)
 {
  e_ptr->size = rp->ready_calc_size(paint_area->Canvas->Handle,*e_ptr);
  if(max_size)
  {
     max_size->cx = std::max(max_size->cx,e_ptr->size.cx);
     max_size->cy = std::max(max_size->cy,e_ptr->size.cy);
  }
  ++e_ptr;
 }
}

void __fastcall  TAwpMainForm::layout_ready         ()
{
 //Расположение элементов готовности
 release_ready_painter();
 MDB_GSIZE max_size;
 update_ready_sizes(&max_size);

 entry_index  number_index;
 vector_index::index_build(Fkadr->entryes,mdb_kadr_entry_number_less(),number_index);

 entry_index::iterator i_ptr = number_index.begin();
 entry_index::iterator i_end = number_index.end();

 kadr_entryes_table::iterator e_ptr =  Fkadr->entryes.begin(),e_end = Fkadr->entryes.end();


 int x_pos = Fready_space;
 int y_pos = Fready_space;
 while(i_ptr<i_end)
 {
   Fkadr->get_by_offset(*i_ptr,e_ptr);
   if(this->Fready_equal_size) {e_ptr->size.cx = max_size.cx;e_ptr->size.cy = max_size.cy;}
   if((x_pos+e_ptr->size.cx) <= paint_area->ClientWidth)
   {
      e_ptr->position.x = x_pos;
      e_ptr->position.y = y_pos;
      x_pos+= e_ptr->size.cx+Fready_space;
     ++i_ptr;
   }
   else
   {
     x_pos = Fready_space;
     y_pos += max_size.cy+Fready_space;
   }
 }
 y_pos += max_size.cy;//+Fready_space;
 Fkadr->kadr.kadr_size.cy = std::max(y_pos,paint_area->Constraints->MinHeight);
 reset_kadr     (MDB_KADR_FIELD_SIZE);
}

void __fastcall  TAwpMainForm::set_kadr_layout_params(int _ready_space,bool _ready_equal_size,bool no_layout)
{
  bool need_update = false;
  if(Fready_space     != _ready_space     ){Fready_space         = _ready_space;     need_update = true;}
  if(Fready_equal_size!= _ready_equal_size){Fready_equal_size    = _ready_equal_size;need_update = true;}
  if(need_update && !no_layout) layout_ready();
}

 struct ready_paint_action_t
 {
   ready_painter_t * ready_painter;
   HDC  dc;
   bool erase;
   bool flash_erase;
   bool show_rc_error;
   HBITMAP  bk_bmp;
   ready_paint_action_t(ready_painter_t * rp,HDC _dc,bool _erase,bool _flash_erase,HBITMAP bmp,bool _show_rc_error)
                       :ready_painter(rp),dc(_dc),erase(_erase),flash_erase(_flash_erase),bk_bmp(bmp),show_rc_error(_show_rc_error)
   {}
   ready_paint_action_t(const ready_paint_action_t& src)
   {*this = src;}
   ready_paint_action_t & operator = (const ready_paint_action_t& src)
   {
     ready_painter = src.ready_painter;
     dc = src.dc;
     erase = src.erase;flash_erase = src.flash_erase;bk_bmp = src.bk_bmp;
     show_rc_error = src.show_rc_error;
     return *this;
   }
   void  __fastcall operator()(mdb_kadr_entry & entry)
   {
    ready_painter->ready_paint(dc,entry,erase,flash_erase,bk_bmp,show_rc_error);
   }
 };

 void         __fastcall TAwpMainForm::paint_entryes    (bool erase,bool flash)
 {
   ready_painter_t * rp = get_ready_painter();
   for_each(Fkadr->entryes.begin(),Fkadr->entryes.end(),ready_paint_action_t(rp,paint_area->Canvas->Handle,erase,(Fflash_counter&1),this->bkgnd(),module->ready_show_rc_error()));
 }

 void      __fastcall  TAwpMainForm::select_changed  (DWORD offset,bool selected)
 {
  if(selected && !Feditable)
    {
      mdb_kadr_entry * entry = Fkadr->get_by_offset(offset);
      TTlmKadrForm * kadr_form = module->open_kadr_form(entry->rec_id,false);
      if(kadr_form)
      {
        if(!kadr_form->Visible)
        {
           set_form_screen_position(kadr_form);
           kadr_form->Visible = true;
        }
         kadr_form->SetFocus() ;
     }
    }
 };

void   __fastcall TAwpMainForm::show_record_info(const DWORD rec_id,bool _set)
{
  if(!_set && ( !RecordInfo || (DWORD)RecordInfo->Tag != rec_id)) return;
  mdb_record rec;
  if(module->get_record(rec_id,rec))
  {
  if(!RecordInfo)
     {
      RecordInfo = new TRecordInfo(this);
      TRect r = BoundsRect;
      RecordInfo->Left = r.Right - RecordInfo->Width;
      RecordInfo->Top  = r.Bottom;
     }
  RecordInfo->Tag = rec_id;
  RecordInfo->Visible = true;
  RecordInfo->Tag = rec.rec_id;
  TListItem * item ;
  UnicodeString str;
  item = RecordInfo->get_item(RECINFO_RECID);
  item->SubItems->Strings[0] = rec.rec_id;

  item = RecordInfo->get_item(RECINFO_OTDADDR);
  if(rec.addr.addr.addr != (DWORD)-1)
  {
      str.printf(L"%03hd.%03hd.%03hd.%03hd-%03hd"
                       ,(short)rec.addr.addr.pu
                       ,(short)rec.addr.addr.cp
                       ,(short)rec.addr.addr.fa
                       ,(short)rec.addr.addr.sb
                       ,(short)rec.addr.param_number
                       );
  }
  else
      str = L"---";
  item->SubItems->Strings[0] = str;

  item = RecordInfo->get_item(RECINFO_TYPE);
  str  = rec.is_discrete() ? L"Дискретный":L"Аналоговый";
  if(rec.is_pseudo()) {
     str.cat_printf(L" [%s]",rec.is_caution() ? L"Плакат" :L"Искуственный");
  }
  item->SubItems->Strings[0] = str;

  item = RecordInfo->get_item(RECINFO_SNAME);
  item->SubItems->Strings[0] = module->_get_string(rec.sname);

  item = RecordInfo->get_item(RECINFO_NAME);
  item->SubItems->Strings[0] = module->_get_string(rec.name);
  RecordInfo->Caption = item->SubItems->Strings[0];

  item = RecordInfo->get_item(RECINFO_VALUE);

  if(rec.is_discrete())
     {
      wchar_t * discrete_str[] =  { L"ОТКЛ",L" ВКЛ" };
      bool is_on     = bool(rec.dw_value);
      bool is_normal = (rec.state&MDBR_STATE_DYN_NORMAL) ? true : false;
      str.printf(L"%s [НП-%s]", discrete_str[is_on],discrete_str[is_normal]);
     }
     else
     {
     str.printf(L"%7.3f  otd_value",rec.value);
     if(rec.options& MDBR_OPT_DOUBLE_TYPE)
        str.cat_printf(L" [%7.3lf]",rec.otd_val_dbl);
        else
        {
         if(rec.options& MDBR_OPT_FLOAT_TYPE)
            str.cat_printf(L" [%7.3f]",rec.otd_val_flt);
            else
            str.cat_printf(L" [%d]",rec.is_signed_kvants() ? rec.get_signed_kvants(rec.otd_val) : rec.otd_val);
        }
       str.cat_printf(L" (min %.3f, max %.3f)",rec.min_value,rec.max_value);
     }


  item->SubItems->Strings[0] = str;

  item = RecordInfo->get_item(RECINFO_TIME);
  str = TGkAwpModule::get_time_text(rec.time,':',true);
  item->SubItems->Strings[0] = str;

  item = RecordInfo->get_item(RECINFO_DIAG);
  str.printf(L"%s [otd_pdiag %X]",rec.diag ? L"BAD":L"OK",rec.otd_pd);
  item->SubItems->Strings[0] = str;

  item = RecordInfo->get_item(RECINFO_WORK);
  item->SubItems->Strings[0] = rec.is_worked() ? L"да":L"выведен";

  item = RecordInfo->get_item(RECINFO_TU);
  item->SubItems->Strings[0] = TGkAwpModule::get_state_tu_text(rec.state) ;

  item = RecordInfo->get_item(RECINFO_STATE);
   str = (rec.state&MDBR_STATE_NORECEIPT) ? L"Неквитируемый" :L"";
   str += TGkAwpModule::get_state_view_text(rec.state);
  item->SubItems->Strings[0] = str;
  item = RecordInfo->get_item(RECINFO_CH_CNT);
  item->SubItems->Strings[0] = record_changes_count(&rec);
  item = RecordInfo->get_item(RECINFO_OPTIONS);
  str.printf(L"%X",rec.options);
  item->SubItems->Strings[0] = str;
  }
}

void __fastcall TAwpMainForm::WndProc(TMessage & msg)
{
   switch(msg.Msg)
     {
      case  WM_SHOW_REC_INFO:
             show_record_info(msg.LParam,msg.WParam ? true : false);
      break;
      case  WM_DO_LAYOUT_READY    : layout_ready(); break;
      case  WM_DO_CLOSE_ALL_KADRS : if(module) module->close_all_kadrs(msg.WParam ? true : false);
      break;
      case  WM_DO_SHOW_PARAMS     : if(module) module->param_window_show();
      break;
      case WM_HOTKEY:
           switch(msg.WParam)
           {
               case HOTKEY_ID_CLEAR_ALL   :module->clear_all   ();break;
               case HOTKEY_ID_RECEIPT_ALL :module->receipt_all ();break;
           }
      break;
      case WM_ON_SOURCE_CONNECT:  on_source_connect(msg.WParam);break;
      default: TTlmPainterForm::WndProc(msg);
      break;
     }

}

void      __fastcall  TAwpMainForm::on_source_connect(bool connect)
{
    if(connect)
    {

    }
    else
    {

    }
    set_login_session_id(0);

}

//---------------------------------------------------------------------------

void __fastcall TAwpMainForm::FormCloseQuery(TObject *Sender, bool &CanClose)
{
  CanClose = false;
}
//---------------------------------------------------------------------------


void __fastcall TAwpMainForm::miCloseAllKadrsClick(TObject *Sender)
{
  module->close_all_kadrs(false);
}
//---------------------------------------------------------------------------
void __fastcall TAwpMainForm::mi_open_kadr_click(TObject *Sender)
{
      TMenuItem * mi = dynamic_cast<TMenuItem*>(Sender);
      if(mi)
        module->open_kadr_form(mi->Tag,true);
}

void __fastcall TAwpMainForm::MainPopupPopup(TObject *Sender)
{
  //Перечисление открытых кадров
  while(miOpenedKadrs->Count)
  {
     TMenuItem * mi = miOpenedKadrs->Items[miOpenedKadrs->Count-1];
     if(mi!= this->miCloseAllKadrs)
        delete mi;
        else
        break;
  }

  kadr_entryes_table::iterator ptr = Fkadr->entryes.begin();
  kadr_entryes_table::iterator end = Fkadr->entryes.end  ();

  while(ptr<end)
  {
     kadr_t * kadr = (kadr_t*)module->get_kadr(ptr->rec_id);
     if(kadr->user_data)
     {
       TForm * form = kadr->get_kadr_form();
       if(form)
       {
        TMenuItem * item = new TMenuItem(miOpenedKadrs);
        item->Caption = form->Caption;
        item->Tag     = kadr->kadr.kadr_id;
        item->OnClick = mi_open_kadr_click;
        miOpenedKadrs->Add(item);
       }
      }
     ++ptr;
  }
  miOpenedKadrs->Visible = miOpenedKadrs->Count>1;
}
//---------------------------------------------------------------------------

 void __fastcall hot_key_vcl2winapi(WORD hot_key,DWORD & flags,DWORD & vk)
 {
   vk = LOBYTE(hot_key);
   WORD mod = HIBYTE(hot_key);
   flags = 0;

   if(mod&(0x20<<hkShift))
      flags|= MOD_SHIFT;

   if(mod&(0x20<<hkCtrl))
      flags|= MOD_CONTROL;

   if(mod&(0x20<<hkAlt))
      flags|= MOD_ALT;

 }

 bool __fastcall  TAwpMainForm::hot_key_register  (int id,WORD hot_key)
 {
   UnregisterHotKey(Handle,id);
   if(hot_key)
   {
      DWORD flags,vk;
      hot_key_vcl2winapi(hot_key,flags,vk);
      return RegisterHotKey(Handle,id,flags,vk);
   }
  return true;
 }


void __fastcall TAwpMainForm::miDbhMakeallFlashingClick(TObject *Sender)
{
 //Сделать
 module->dbg_set_all_records_state(MDBR_STATE_FLASH,true);
}
//---------------------------------------------------------------------------

void      __fastcall TAwpMainForm::move_current_ready(move_direction_t dir)
{
    mdb_kadr_entry * entry = this->Fkadr->get_last_selected();
    if(entry)
    {
       POINT pt;
       pt <<entry->position;
       pt.x += entry->size.cx/2;
       pt.y += entry->size.cy/2;
       switch(dir)
       {
         case move_direction_t::md_left :  pt.x-=entry->size.cx;
         break;
         case move_direction_t::md_right:  pt.x+=entry->size.cx;
         break;
         case move_direction_t::md_up   :  pt.y-=entry->size.cy;
         break;
         case move_direction_t::md_down :  pt.y+=entry->size.cy;
         break;
       }

       mdb_kadr_entry * other_entry = Fkadr->get_by_offset(Fkadr->get_at_position(pt.x,pt.y));
       if(other_entry)
         {
           std::swap(other_entry->number,entry->number);
           layout_ready();
         }
    }
}


void      __fastcall  TAwpMainForm::do_move_selected(int dx,int dy)
{
  if(Feditable && Fkadr && Fkadr->sel_count() == 1)
  {
    TTlmPainterForm::do_move_selected(dx,dy);
  }
}

struct    entry_compare_by_position
{
   bool operator() (const mdb_kadr_entry &e1,const mdb_kadr_entry &e2)
   {

      int  v1 = e1.position.y;
      int  v2 = e2.position.y;

      if(abs(v1 - v2) < e1.size.cy)
         {
          v1 = e1.position.x;
          v2 = e2.position.x;
         }
      return v1 < v2 ? true : false;
   }
  bool operator() (const mdb_kadr_entry *e1,const mdb_kadr_entry *e2)
  {
     return (*this)(*e1,*e2);
  }

};
void      __fastcall  TAwpMainForm::on_mouse_up     (TMouseButton button,TShiftState shift,int X, int Y)
{
   TTlmPainterForm::on_mouse_up(button,shift,X,Y);
   if(editable && Fkadr && Fkadr->sel_count() == 1)
     {
        Fkadr->position_index_resort();
        entry_index index;
        vector_index::index_build(Fkadr->entryes,entry_compare_by_position(),index);
        entry_index::iterator ptr = index.begin();
        entry_index::iterator end = index.end  ();
        DWORD number = 0;
        while(ptr<end)
        {
         mdb_kadr_entry * entry = Fkadr->get_by_offset(*ptr);
         if(entry) entry->number = number++;
         ++ptr;
        }
        layout_ready();
        module->set_config_changes(CCFL_KADRS|CCFL_NAMES);
     }
}


void __fastcall TAwpMainForm::tbMinimizeClick(TObject *Sender)
{
  module->show_all_windows(false);
  wp_tray_icon->IconIndex = -1;
  wp_tray_icon->BalloonTitle = Caption;
  wp_tray_icon->Visible = true;
  TrayTimerTimer(this);
  TrayTimer->Enabled    = true;

}
//---------------------------------------------------------------------------

void __fastcall TAwpMainForm::wp_tray_iconDblClick(TObject *Sender)
{
  wp_tray_icon->Visible = false;
  TrayTimer->Enabled    = false;
  module->show_all_windows(true);
}
//---------------------------------------------------------------------------



void __fastcall TAwpMainForm::TrayTimerTimer(TObject *Sender)
{
  if(module)
  {
   int prev_idx = wp_tray_icon->IconIndex;
   wp_tray_icon->IconIndex = -1;
   int icon_idx = 0;
   TBalloonFlags bf = bfInfo;
   UnicodeString str = L"Все хорошо" ;
   if(module->is_source_connected())
   {
       DWORD ready_state = module->kadrs_scan_ready();
       if(ready_state&rds_any_danger)
       {
          icon_idx = 1;
          str = L"Не все хорошо" ;
          bf = bfWarning;
       }
       if(ready_state&rds_any_alarm)
       {
          icon_idx = 2;
          str = L"Где-то совсем плохо" ;
          bf = bfError;
       }
   }
   else
   {
    icon_idx = 3;
    str = L"Отсутствует связь с сервером" ;
    bf = bfError;
   }

   wp_tray_icon->IconIndex = icon_idx;
   wp_tray_icon->Hint = str;
   if(prev_idx!=icon_idx)
     {
      wp_tray_icon->BalloonFlags   = bf;
      wp_tray_icon->BalloonHint    = str;
      wp_tray_icon->BalloonTimeout = 1000;
      wp_tray_icon->ShowBalloonHint();
     }
  }

}
//---------------------------------------------------------------------------

void __fastcall TAwpMainForm::miLoginClick(TObject *Sender)
{
  if(!login_dlg)
  {
   login_dlg = new TLoginDlg(this);
   login_dlg->Icon->Assign(Icon);
   login_dlg->bLogin->OnClick = on_login_click;
  }
  login_dlg->Show    ();
  login_dlg->SetFocus();
}
//---------------------------------------------------------------------------

 void __fastcall  TAwpMainForm::on_login_click(TObject * sender)
 {
  module->send_login_request(login_dlg->user_name->Text,login_dlg->user_passw->Text);
 }
//---------------------------------------------------------------------------

void __fastcall  TAwpMainForm::set_login_session_id(SESSION_ID ssid    )
{
  if(login_dlg)
  {
    UnicodeString str = ssid == 0 ? L"Не авторизовано" : L"Ключ сессии";
    if(ssid)
       str.cat_printf(L" %X:%X",((LPDWORD)&ssid)[1],((LPDWORD)&ssid)[0]);
    login_dlg->user_passw->Text = UnicodeString();
    login_dlg->StatusBar1->Panels->Items[0]->Text = str;
    if(!ssid) login_dlg->SetFocus();
  }
}

//---------------------------------------------------------------------------

