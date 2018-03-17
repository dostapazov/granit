//---------------------------------------------------------------------------

#pragma hdrstop
#include <vcl.h>
#include "gktlm_viewer_mod.hpp"
#include "gktlm_viewer_form.h"
#include "history_data.h"
#include <KeRTLio.hpp>
#include <kfname.hpp>
#include "gktlm_viewer_mod.hpp"
#include <stdlib_utils.hpp>

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

#define TLM_VIEWER_IMAGES_FOLDER L"image\\"


TTlmViewerForm *TlmViewerForm;

//WORD                TTlmViewerForm::hot_key_clear_all   = 0;
//WORD                TTlmViewerForm::hot_key_receipt_all = 0;
//READY_COLORS        TTlmViewerForm::rdy_colors = {0};

using namespace stdlib_utils;


bool  __fastcall TTlmViewerForm::get_record        (TObject * sender,DWORD id ,bool offset, mdb_record_ex & rec)
{
  return viewer_module->__get_record(id,offset,rec);
}

 bool __fastcall TTlmViewerForm::get_text         (DWORD id,UnicodeString & name)
   {
    return viewer_module->__get_text(id,name);
   }

 void __fastcall TTlmViewerForm::get_record_text(TObject * sender,DWORD rec_id,UnicodeString & str)
  {
   viewer_module->get_record_text(rec_id,str);
  }





//---------------------------------------------------------------------------
__fastcall TTlmViewerForm::TTlmViewerForm(TComponent* Owner,TGKTlmViewer *_viewer_module)
	: TForm(Owner)
        , viewer_module(_viewer_module)
        , Fpainter(NULL)
	, kadr    (NULL)
        , Ffixed  (false)

{
}

 __fastcall TTlmViewerForm::TTlmViewerForm   (TComponent* Owner)
 	: TForm(Owner)
        , viewer_module(NULL)
        , Fpainter(NULL)
	, kadr    (NULL)
        , Ffixed  (false)

 {
 }

//-------------------------------------------------------------------------
//       		 modem_addr      TTlmViewerForm::modem_source;
// TTlmViewerForm::mutex_type      TTlmViewerForm::mut;
//       		 Tnames          TTlmViewerForm::names;
//       		 Tkadrs          TTlmViewerForm::kadrs_ordered;
//       		 Tkadrs          TTlmViewerForm::kadrs_avail;
//       		 Trecords        TTlmViewerForm::records;
//                 Trecord_changes TTlmViewerForm::changed_records;
//                 Tpaint_variants TTlmViewerForm::paint_vars;



 bool __fastcall TTlmViewerForm::is_painter_created()
 {
   return NULL!=Fpainter() ? true : false;
 }

 DWORD WINAPI  TTlmViewerForm::get_record_callback(LPVOID ud,TTlmPainter * ,mdb_record_ex * rec)
 {
   TTlmViewerForm * tvf = reinterpret_cast<TTlmViewerForm *>(ud);
   if(tvf && rec)
      return tvf->get_record(tvf,rec->rec_id,false,*rec);
    return false;
 }

 wstring WINAPI    TTlmViewerForm::get_text_callback  (LPVOID ud,TTlmPainter * ,DWORD str_id)
 {
   wstring ret;
   UnicodeString str;
   TTlmViewerForm * tvf = reinterpret_cast<TTlmViewerForm *>(ud);
   if(tvf && tvf->get_text(str_id,str));
      ret = str.c_str();
    return ret;
 }

 DWORD __fastcall TTlmViewerForm::on_select_entry(TTlmPainter * p,DWORD entry  )
 {

  return 0;
 }

 DWORD WINAPI TTlmViewerForm::on_select_entry_callback(LPVOID ud,TTlmPainter * p ,DWORD entry)
 {
    TTlmViewerForm * tvf = reinterpret_cast<TTlmViewerForm *>(ud);
    if(tvf)
      {
        return tvf->on_select_entry(p,entry);
      }
    return 0;
 }


 bool          __fastcall TTlmViewerForm::create_painter ()
 {
   if(!painter_created)
   {
     KeWin::TRect r(0,0,PainterPanel->ClientWidth,PainterPanel->ClientHeight);
     int cx_frame = GetSystemMetrics(SM_CXFIXEDFRAME);
     int cy_frame = GetSystemMetrics(SM_CYFIXEDFRAME);
     r.Move(cx_frame,cy_frame);
     r.Grow(-cx_frame,-cy_frame);
     char class_name [512];
     sprintf(class_name,"Painter$%u",GetTickCount());
     painter->Create(PainterPanel->Handle,NULL,r,-1,NULL,WS_VISIBLE|WS_CHILDWINDOW/*|WS_TABSTOP*/,WS_EX_CONTROLPARENT,class_name);
     painter->SetSize(PainterPanel->Width,PainterPanel->Height);
     painter->set_bkgnd(*&wstring());
     painter_save_font = painter->SetFont  (this->PainterPanel->Font->Handle,false);
     //painter->SetFlags (FLWND_DOUBLE_BUFFERED,this->viewer_module->get_enable_double_buffered());
     DoubleBuffered = true;
     painter->set_getter_record (get_record_callback      ,this);
     painter->set_getter_text   (get_text_callback        ,this);
     painter->set_on_mouse_over (on_mouse_over_callback   ,this);
     painter->set_on_select     (on_select_entry_callback ,this);
     painter->set_mouse_buttons_callback(on_mouse_buttons_callback,this);
     painter->set_mouse_move_callback   (on_mouse_move_callback   ,this);
     painter->set_keydown_closure(on_keydown_callback,this);
     PainterPanelResize(this);
   }
  return is_painter_created();
 }

 BOOL  __fastcall TTlmViewerForm::on_mouse_buttons(TTlmPainter * p,Tmouse_data * md)
 {

  return FALSE;
 }

 BOOL  __fastcall TTlmViewerForm::on_mouse_move(TTlmPainter * p,Tmouse_data * md)
 {
  return FALSE;
 }


 BOOL      WINAPI TTlmViewerForm::on_mouse_buttons_callback(LPVOID  ud,TTlmPainter * p,Tmouse_data * md)
 {
    TTlmViewerForm * tvf = reinterpret_cast<TTlmViewerForm *>(ud);
    if(tvf)
    {
      return tvf->on_mouse_buttons(p,md);
    }
  return FALSE;
 }

 BOOL      WINAPI TTlmViewerForm::on_mouse_move_callback(LPVOID  ud,TTlmPainter * p,Tmouse_data * md)
 {
    TTlmViewerForm * tvf = reinterpret_cast<TTlmViewerForm *>(ud);
    if(tvf)
    {
      return tvf->on_mouse_move(p,md);
    }
  return FALSE;
 }


BOOL    WINAPI    TTlmViewerForm::on_keydown_callback      (LPVOID ud,TTlmPainter * p,Tkeyboard_data * kd)
{
   BOOL ret = FALSE;
   TTlmViewerForm * tvf = reinterpret_cast<TTlmViewerForm *>(ud);
   if(tvf && kd)
   {
    switch(kd->v_key)
    {
      case VK_SPACE :
      tvf->set_kadr_ready_flags(tvf->kadr,0,RDY_FLAG_FLASH_NAME);
      ret = TRUE;
      break;
      case VK_UP :
      case VK_LEFT :
                   tvf->painter->select_next(false);
                   ret = TRUE;
      break;
      case VK_DOWN  :
      case VK_RIGHT :
                   tvf->painter->select_next(true);
                   ret = TRUE;
      break;
    }
   }

  return ret;
}


 DWORD __fastcall TTlmViewerForm::on_mouse_over         (TTlmPainter * p,DWORD entry)
 {
     return 0;
 }

 DWORD WINAPI     TTlmViewerForm::on_mouse_over_callback(LPVOID ud,TTlmPainter * p,DWORD entry)
 {
    TTlmViewerForm * tvf = reinterpret_cast<TTlmViewerForm *>(ud);
    if(tvf)
    {
      return tvf->on_mouse_over(p,entry);
    }
   return 0;
 }


 void          __fastcall TTlmViewerForm::destroy_painter()
 {
   if(painter_created)
      {
       painter->SetFont  (painter_save_font,false);
       painter->Destroy();
      }
 }



void  __fastcall TTlmViewerForm::handle_recv(LPMPROTO_HEADER mph)
{
   switch(mph->fa)
   {
     case FA_OTD_MEDIUM               : handle_mproto( mph );
          break;
     case FA_OTD_MEDIUM_ALARM_ARCHIVE:  handle_mproto_archive(mph);
          break;
   }
}

void   __fastcall TTlmViewerForm::handle_mproto_archive(LPMPROTO_HEADER mph)
{
 LPOTDM_PROTO_HEADER omph = (LPOTDM_PROTO_HEADER)mph->data;
 if((omph->command& OTDM_RESPOND) && mph->data_size >= sizeof(OTDM_ARCHIVE_HEADER))
 {
   LPOTDM_ARCHIVE_HEADER ahdr = (LPOTDM_ARCHIVE_HEADER) omph->data;
   TForm * f = reinterpret_cast<TForm*>((LPVOID)ahdr->hist_id);
   if(f && IsWindow(f->Handle))
    {
     switch((omph->command&OTDM_COMMAND_MASK))
     {
      case OTDMPROTO_CMD_QUERY_ARCHIVE:
           SendMessage(f->Handle,HDCM_HANDLE_ARCHIVE,(omph->command&OTDM_ENDDATA) ? TRUE : FALSE,(LPARAM)ahdr->data);
      break;
      case OTDMPROTO_CMD_ARCHIVE_PRESENT:
           SendMessage(f->Handle,TLMKV_WM_ARCHIVE_PRESENT,0 , 0);
      break;
      case OTDMPROTO_CMD_QUERY_KADR_ARCHIVES:
           SendMessage(f->Handle,TLMKV_HANDLE_KADR_ARCHIVE,(omph->command&OTDM_ENDDATA) ? TRUE : FALSE,(LPARAM)ahdr->data);
      break;
     }
   }
 }

}

void   __fastcall TTlmViewerForm::handle_medproto_error (LPOTDM_PROTO_HEADER omph)
{
  //bool beg_resp = (omph->command&OTDM_BEGINDATA) ? true:false;
  //bool end_resp = (omph->command&OTDM_ENDDATA) ? true:false;
  DWORD cmd = omph->command&OTDM_COMMAND_MASK;
  LPOTDM_PROTO_KADR p_kadr = (LPOTDM_PROTO_KADR )omph->data;
  switch(cmd)
  {
   case OTDMPROTO_CMD_GETKADR:
   case OTDMPROTO_CMD_ENUM_KADRS:
   {
    Tkadrs::iterator ptr;
    TGKTlmViewer::locker l(viewer_module->get_mutex());
    if(binary_search  ( viewer_module->get_kadrs_avail(),&Tmdb_kadr_ex(p_kadr->kadr),&ptr))
       viewer_module->get_kadrs_avail().erase(ptr);
    if(binary_search(viewer_module->get_kadrs_ordered(),&Tmdb_kadr_ex(p_kadr->kadr),&ptr))
       viewer_module->get_kadrs_ordered().erase(ptr);
   }
   break;

  }
}

void   __fastcall TTlmViewerForm::handle_mproto(LPMPROTO_HEADER mph)
{
 LPOTDM_PROTO_HEADER omph = (LPOTDM_PROTO_HEADER)mph->data;
 if(omph->command & OTDM_RESPOND )
 {

  bool beg_resp = (omph->command&OTDM_BEGINDATA) ? true:false;
  bool end_resp = (omph->command&OTDM_ENDDATA) ? true:false;
  DWORD cmd = omph->command&OTDM_COMMAND_MASK;
  if(omph->error || OTDM_PROTO_IS_ERROR(omph))
     handle_medproto_error(omph);
   else
   {
   switch(cmd)
   {
    case OTDMPROTO_CMD_ENUM_KADRS:
         handle_get_kadr(viewer_module->get_kadrs_avail(),NULL,omph,end_resp);
    break;
    case OTDMPROTO_CMD_GETKADR:
         //Обработка перечисления кадров
         viewer_module->set_modem_source(mph->addr_from);
         handle_get_kadr(viewer_module->get_kadrs_ordered(),&viewer_module->get_names(),omph,end_resp);
         break;

    case OTDMPROTO_CMD_GETKADR_IMAGE:
         handle_image(omph,beg_resp,end_resp);
         break;
    case OTDMPROTO_CMD_GETKADR_ENTRYES:
         handle_entryes(omph,beg_resp,end_resp);
         break;
    case OTDMPROTO_CMD_GETKADR_RECORDS:
         handle_records(omph,beg_resp,end_resp);
         break;
    case OTDMPROTO_CMD_RECORDS:
         handle_records_changed(omph,beg_resp,end_resp);
         break;
    case OTDMPROTO_CMD_GET_RECORD_SNAME     :
    case OTDMPROTO_CMD_GET_RECORD_FULL_SNAME:
         handle_record_names(omph,true);
    break;
    case OTDMPROTO_CMD_GET_RECORD_NAME      :
    case OTDMPROTO_CMD_GET_RECORD_FULL_NAME :
         handle_record_names(omph,false);
    break;
    default:
    DebugBreak();
    break;
   }
  }
 }
}

void __fastcall copy_kadr(Tmdb_kadr_ex * dest,MDB_KADR * src,DWORD  ch_flags)
{
      if(ch_flags&MDB_KADR_FIELD_DIAG)
           {
            if(dest->kadr_diag != src->kadr_diag)
              {
               dest->kadr_diag   = src->kadr_diag;
               dest->ready_timer = 0;
               dest->ready_flags &=~RDY_FLAG_STABLE;
              }
           }
      if(ch_flags&MDB_KADR_FIELD_STATE)
          dest->kadr_state=src->kadr_state;
      if(ch_flags&MDB_KADR_FIELD_DISCRETE_CHANGES)
         dest->discrete_changes = src->discrete_changes;
      if(ch_flags&MDB_KADR_FIELD_CHANGES)
         dest->kadr_changes = src->kadr_changes;
      if(ch_flags&MDB_KADR_FIELD_ALARM_CHANGES)
         dest->alarm_changes = src->alarm_changes;
      if(ch_flags&MDB_KADR_FIELD_ENTRY_COUNT)
         dest->entry_count = src->entry_count;
      if(ch_flags&MDB_KADR_FIELD_IMAGE)
         {
          dest->image_id   = src->image_id;
          dest->image_time = src->image_time;
         }
      if(ch_flags& MDB_KADR_FIELD_FLAGS)
         dest->kadr_flags = src->kadr_flags;

      if(ch_flags& MDB_KADR_FIELD_SIZE)
      {
         dest->kadr_size.cx = src->kadr_size.cx ;
         dest->kadr_size.cy = src->kadr_size.cy;
      }
      if(ch_flags&MDB_KADR_FIELD_SNAME)
        dest->sname_id = src->sname_id;
      if(ch_flags&MDB_KADR_FIELD_NAME)
        dest->name_id = src->name_id;
}

DWORD  __fastcall TTlmViewerForm::handle_get_kadr(Tkadrs & klist,Tnames * _names,LPOTDM_PROTO_HEADER omph,bool end)
{
  LPOTDM_PROTO_KADR p_kadr = (LPOTDM_PROTO_KADR )omph->data;
  Tkadrs::iterator ptr;
  wchar_t * kname = p_kadr->names;
  DWORD ch = p_kadr->mask;
  TGKTlmViewer::locker l(viewer_module->get_mutex());
  Tmdb_kadr_ex * kadr;
  if(binary_search(klist,&Tmdb_kadr_ex(p_kadr->kadr),&ptr))
     {
      kadr = *ptr;


      if((ch&MDB_KADR_FIELD_IMAGE) && kadr->image_time==p_kadr->kadr.image_time)
          ch&=~MDB_KADR_FIELD_IMAGE;
      kadr_state ks1;
      ks1.kadr_state = kadr->kadr_state;

      copy_kadr(kadr,&p_kadr->kadr,ch);

      if(ch&MDB_KADR_FIELD_STATE)
      {
      kadr_state ks2;
      ks2.kadr_state = kadr->kadr_state;

      if(ks2.arch_count && ks1.arch_count!=ks2.arch_count)
         {
          kadr->ready_flags |= RDY_FLAG_ARCHIVE;
          if(ch&MDB_KADR_FIELDS_HIST_TIMES)
           kadr->push_hist_times(false,p_kadr->kadr.hist_beg_time ,p_kadr->kadr.hist_end_time);
         }

      if(ks2.alarm_arch_count && ks1.alarm_arch_count!=ks2.alarm_arch_count)
         {
          kadr->ready_flags |= RDY_FLAG_ALARM_ARCHIVE;
          if(ch&MDB_KADR_FIELDS_HIST_TIMES)
           kadr->push_hist_times(true,p_kadr->kadr.hist_beg_time ,p_kadr->kadr.hist_end_time);
         }

      }

      if( (ch&(MDB_KADR_FIELD_DISCRETE_CHANGES) && p_kadr->kadr.discrete_changes) || (ch&(MDB_KADR_FIELD_ALARM_CHANGES) && p_kadr->kadr.alarm_changes))
        {
           set_kadr_ready_flags(kadr,RDY_FLAG_FLASH_NAME,0);
        }

     }
     else
     {
       Tmdb_kadr_ex * nkadr = new Tmdb_kadr_ex(p_kadr->kadr);
       ptr = klist.insert(ptr,nkadr);
       kadr = *ptr;
     }


  if(_names && (ch&MDB_KADR_FIELD_SNAME) && kadr->sname_id!=(DWORD)-1)
    {
     UnicodeString text;
     text   = kname;
     kname +=lstrlenW(kname)+1;
     Tnames & nm = *_names;
     UnicodeString & name = nm[kadr->sname_id];
     if(0 == name.Compare(text))
       ch&=~MDB_KADR_FIELD_SNAME;
       else
       name = text;
    }
    else
     ch &=~MDB_KADR_FIELD_SNAME;

  if(_names && (ch&MDB_KADR_FIELD_NAME) && kadr->name_id!=(DWORD)-1)
    {
     UnicodeString text = kname;
     Tnames & nm = *_names;
     UnicodeString & name = nm[kadr->name_id];
     if(0 == name.Compare(text))
       ch&=~MDB_KADR_FIELD_NAME;
       else
       name = text;
    }
    else
    ch &=~MDB_KADR_FIELD_NAME;


  TTlmViewerForm * tv = kadr->get_viewer_form();
  if(tv)
    {
       // Обновить изображение
       // Установить размер
       // изменить режим фона
       // Перерисовать кадр
       PostMessage(tv->Handle,TLMKV_WM_UPDATE_KADR,ch,kadr->kadr_id);
    }

 return ch;
}



void __fastcall TTlmViewerForm::PainterPanelResize(TObject *Sender)
{
 if(painter_created)
   {
     KeWin::TRect r(0,0,PainterPanel->Width,PainterPanel->Height);
     GetSystemMetrics(SM_CXFIXEDFRAME);
     r.Grow(-(GetSystemMetrics(SM_CXFIXEDFRAME)<<1),-(GetSystemMetrics(SM_CYFIXEDFRAME)<<1));
     painter->SetSize(r.Width(),r.Height());
   }
}
//---------------------------------------------------------------------------

void   __fastcall TTlmViewerForm::handle_record_names(LPOTDM_PROTO_HEADER omph,bool short_name)
 {
  LPOTDM_PROTO_RECORD_NAME rn = (LPOTDM_PROTO_RECORD_NAME) omph->data;
  TGKTlmViewer::locker l(viewer_module->get_mutex());
  UnicodeString name(rn->text,rn->len);
  viewer_module->set_text(rn->name_id,name);
  Trecords::iterator rptr ;
  if(binary_search(viewer_module->get_records(),*&mdb_record_ex(rn->rec_id),&rptr))
  {
   DWORD & name_id = short_name ? rptr->sname : rptr->name;
   if(name_id != rn->name_id)
      {
       name_id = rn->name_id;
       viewer_module->config_change(CCFL_RECORDS);
      }
  }
 }

//LONG TTlmViewerForm::discrete_disparity_count = 0;
//LONG TTlmViewerForm::discrete_back_count      = 0;


void   __fastcall TTlmViewerForm::copy_record(mdb_record_ex * dest,mdb_record_ex * src,DWORD &  mask)
{
       DWORD clear_mask = 0;
       if(mask&MDBR_FIELD_VALUE)
          {
           if(dest->value != src->value)
           {
            DWORD ck = dest->inc_changes();
            dest->value = src->value;
            if(dest->is_discrete() && !dest->is_static() && ck && ck<5)
            {
               if(dest->is_normal_state())
                 viewer_module->inc_discrete_back_count();
                 else
                 viewer_module->inc_discrete_disparity_count();

            }
           }
           else
           clear_mask|=MDBR_FIELD_VALUE;
          }

       if(mask&MDBR_FIELD_DIAG )
       {
           if(dest->diag != src->diag )
              dest->diag = src->diag ;
          else
           clear_mask|=MDBR_FIELD_DIAG;
       }


       if(mask&MDBR_FIELD_STATE)
          {
            src->state &= ~MDBR_STATE_FLASH;
            if(dest->state!=src->state)
             {
              dest->state  = src->state | ((dest->state&MDBR_STATE_FLASH) ? MDBR_STATE_FLASH : 0);
             }
             else
             clear_mask|=MDBR_FIELD_STATE;
          }

       if(mask&MDBR_FIELD_TIME )
       {
          if(dest->time != src->time )
          dest->time = src->time;
          else
          clear_mask|=MDBR_FIELD_TIME;
       }

       if(mask&MDBR_FIELD_MIN_VALUE )
          dest->min_value = src->min_value ;
       if(mask&MDBR_FIELD_MAX_VALUE )
          dest->max_value = src->max_value ;
       if(mask&MDBR_FIELD_NAME     )
          dest->name = src->name ;
       if(mask&MDBR_FIELD_SNAME    )
          dest->sname = src->sname ;

       if(mask&MDBR_FIELD_ADDR     )
          dest->addr = src->addr ;
       if(mask&MDBR_FIELD_OTDVALUE )
          dest->otd_val = src->otd_val ;
       if(mask&MDBR_FIELD_OTDPD    )
          dest->otd_pd = src->otd_pd ;
       if(mask&MDBR_FIELD_OPTIONS  )
          dest->options = src->options ;
       if(mask&MDBR_FIELD_SCALEID  )
          dest->scale_id = src->scale_id ;
       if(mask&MDBR_FIELD_PERCENT  )
          dest->percent = src->percent ;
       if(mask&MDBR_FIELD_LASTVALUE )
          dest->last_value = src->last_value ;
       if(mask&MDBR_FIELD_SECURITY  )
          dest->security = src->security ;
       if(mask&MDBR_FIELD_MIN_ALARM)
          dest->min_alarm = src->min_alarm;
       if(mask&MDBR_FIELD_MAX_ALARM)
          dest->max_alarm = src->max_alarm;
       if(mask&MDBR_FIELD_MIN_DANGER)
          dest->min_danger = src->min_danger;
       if(mask&MDBR_FIELD_MAX_DANGER)
          dest->max_danger = src->max_danger;

       mask&=~clear_mask;
}

void   __fastcall TTlmViewerForm::add_record(mdb_record_ex & rec,DWORD & mask)
{
 //TODO обработать изменения имен
  bool  need_refresh_names = false;
  Trecords::iterator rptr;
  TGKTlmViewer::locker l(viewer_module->get_mutex());
  Trecords & records = viewer_module->get_records();
 if(!binary_search(records,rec,&rptr))
     {
      rptr = records.insert(rptr,rec);
      viewer_module->config_change(CCFL_RECORDS);
      need_refresh_names = true;
     }
   else
    {
      if(mask&MDBR_RECORD_REMOVED)
      {
        records.erase(rptr);
      }
      else
      {
       if((rptr->diag && !rec.diag) || ((mask&MDBR_FIELD_NAME) && rptr->name != rec.name   )  || ( (mask&MDBR_FIELD_SNAME) && rptr->sname != rec.sname ))
          need_refresh_names = true;
          copy_record(&*rptr,&rec,mask);
          rec.name  = rptr->name;
          rec.sname = rptr->sname;
      }

    }
   Tnames & names = viewer_module->get_names();
   if(rec.name!=DWORD(-1)  && (need_refresh_names || !names.count(rec.name) )  )
      request_names(rec.rec_id,false,true);
   if(rec.sname!=DWORD(-1) && (need_refresh_names || !names.count(rec.sname) )  )
         request_names(rec.rec_id,true,false);
    viewer_module->get_changed_records()[rec.rec_id]|= mask;
}

void   __fastcall TTlmViewerForm::handle_records_changed(LPOTDM_PROTO_HEADER omph,bool beg,bool end)
{
 LPOTDM_PROTO_RECORDS p_recs = (LPOTDM_PROTO_RECORDS) omph->data;
 if(p_recs->rec_count)
 {
  LPBYTE src_ptr = (LPBYTE)p_recs->records;
  mdb_record_ex rec;
  __int64  time = 0;
  for(DWORD i = 0;i<p_recs->rec_count;i++)
  {
   LPOTDM_PROTO_RECORD proto_rec = (LPOTDM_PROTO_RECORD) src_ptr;
   DWORD mask = proto_rec->mask;
   src_ptr+=sizeof(proto_rec->mask);
   src_ptr+= otdm_unpack_record(src_ptr,omph->data_size,proto_rec->mask,&rec);
   if(mask  & MDBR_FIELD_TIME)
      time =  rec.time;
      else
      {
       if(time)
         {
          rec.time = time;
          mask|= MDBR_FIELD_TIME;
         }
      }
     add_record(rec,mask);
  }
  PostMessage(this->Handle,TLMKV_WM_RECORD_CHANGES,0,0);
 }
}

void   __fastcall TTlmViewerForm::request_names(DWORD rec_id,bool short_name,bool long_name)
{
  modem_addr modem_source = viewer_module->get_modem_source() ;
  if(long_name)
   viewer_module->send_command(modem_source,FA_OTD_MEDIUM,true,true,OTDMPROTO_CMD_GET_RECORD_FULL_NAME,0,&rec_id,sizeof(rec_id));
  if(short_name)
   viewer_module->send_command(modem_source,FA_OTD_MEDIUM,true,true,OTDMPROTO_CMD_GET_RECORD_SNAME,0,&rec_id,sizeof(rec_id));
}


 void   __fastcall TTlmViewerForm::handle_records   (LPOTDM_PROTO_HEADER omph,bool beg,bool end)
 {

  LPOTDM_PROTO_KADR_RECORDS kr = (LPOTDM_PROTO_KADR_RECORDS) omph->data;
  LPMDB_RECORD b_ptr = kr->records, e_ptr = b_ptr+kr->rec_count;

  while(b_ptr<e_ptr)
  {
   mdb_record_ex rec(*b_ptr);
   DWORD mask = -1;
   mask &=~(MDBR_FIELD_NAME|MDBR_FIELD_SNAME|MDBR_RECORD_REMOVED);
   add_record(rec,mask);
   ++b_ptr;
  }
  if(end )
   this->update_kadr(kr->kadr_id,MDB_KADR_FIELD_ENTRYES|MDB_KADR_FIELD_VALUES);
  }


 int    __fastcall compare_entryes(LPMDB_KADR_ENTRY  e1,LPMDB_KADR_ENTRY e2)
 {
  return memcmp(e1,e2,sizeof(MDB_KADR_ENTRY));
 }


// void   __fastcall TTlmViewerForm::handle_entryes   (LPOTDM_PROTO_HEADER omph,bool beg,bool end)
// {
//  /*Обработка записей содержимого кадра*/
//  int changes_count = 0;
//  LPOTDM_PROTO_KADR_ENTRYES ent = (LPOTDM_PROTO_KADR_ENTRYES)omph->data;
//  Tkadrs::iterator kptr ;
//
//  locker l(mut);
//  if(binary_search(kadrs_ordered,&Tmdb_kadr_ex (ent->kadr_id),&kptr))
//  {
//     Tmdb_kadr_ex * kadr = *kptr;
//     Tentryes & ent_list =  kadr->get_entryes();
//     LPMDB_KADR_ENTRY src_ptr   = ent->entryes;
//
//     if(beg) // Начало с данными об элементах кадра  Сбрасываем флаг изменений
//        {
//         kadr->flags&=~KADR_FLAGS_ENTRYES_CHANGED;
//        }
//
//     if(ent_list.size()<src_ptr->number+ent->entryes_count)
//        {
//         DWORD new_size = src_ptr->number+ent->entryes_count;
//         ent_list.resize(new_size);
//         kadr->flags|=KADR_FLAGS_ENTRYES_CHANGED;
//        }
//
//     Tentryes::iterator dest_ptr = ent_list.begin();
//     std::advance(dest_ptr,src_ptr->number);
//
//     Tentryes::iterator dest_end = dest_ptr;
//     std::advance(dest_end,ent->entryes_count);
//
//     while(dest_ptr<dest_end)
//     {
//      if(compare_entryes(&*dest_ptr,src_ptr))
//         {
//          *dest_ptr = *src_ptr;
//          kadr->flags|=KADR_FLAGS_ENTRYES_CHANGED;
//         }
//      ++dest_ptr;++src_ptr;
//     }
//
//   if(end && (kadr->flags&KADR_FLAGS_ENTRYES_CHANGED))
//     {
//      TTlmViewerForm * tv = (*kptr)->get_viewer_form();
//      if(tv)
//         PostMessage(tv->Handle,TLMKV_WM_SET_ENTRYES,(*kptr)->kadr_id,0);
//      viewer_module->config_change(CCFL_KADRS);
//      kadr->flags&=~KADR_FLAGS_ENTRYES_CHANGED;
//     }
//  }
// }

 void   __fastcall TTlmViewerForm::handle_entryes   (LPOTDM_PROTO_HEADER omph,bool beg,bool end)
 {
  /*Обработка записей содержимого кадра*/
  //int changes_count = 0;
  LPOTDM_PROTO_KADR_ENTRYES ent = (LPOTDM_PROTO_KADR_ENTRYES)omph->data;
  Tkadrs::iterator kptr ;

  TGKTlmViewer::locker l(viewer_module->get_mutex());
  Tkadrs & kadrs_ordered = viewer_module->get_kadrs_ordered();
  if(binary_search(kadrs_ordered,&Tmdb_kadr_ex (ent->kadr_id),&kptr))
  {
     Tmdb_kadr_ex * kadr = *kptr;
     Tentryes_list & ent_list =  kadr->get_entryes();
     LPMDB_KADR_ENTRY src_ptr   = ent->entryes;

     if(beg) // Начало с данными об элементах кадра  Сбрасываем флаг изменений
        {
         kadr->flags |=KADR_FLAGS_ENTRYES_CHANGED;
         ent_list.clear();
        }


     for(DWORD i = 0;i<ent->entryes_count;i++)
     {
      ent_list.push_back(*src_ptr);
      ++src_ptr;
     }

   if(end && (kadr->flags&KADR_FLAGS_ENTRYES_CHANGED))
     {
      TTlmViewerForm * tv = (*kptr)->get_viewer_form();
      if(tv)
         PostMessage(tv->Handle,TLMKV_WM_SET_ENTRYES,(*kptr)->kadr_id,0);
         viewer_module->config_change(CCFL_KADRS);
         kadr->flags&=~KADR_FLAGS_ENTRYES_CHANGED;
     }
  }
 }




 UnicodeString __fastcall TTlmViewerForm::get_image_filename(Tmdb_kadr_ex * kadr)
 {
   UnicodeString str;
   UnicodeString name;
   if(kadr && get_text(kadr->image_id,name))
   {
     str = viewer_module->get_data_dir();
     str+=IMAGE_FOLDER;
     str+=name;
   }
  return str;
 }

 UnicodeString __fastcall TTlmViewerForm::get_image_filename(DWORD kadr_id)
 {
   UnicodeString str;
   Tkadrs::iterator kptr;
   TGKTlmViewer::locker l(viewer_module->get_mutex());
   Tkadrs & kadrs_ordered = viewer_module->get_kadrs_ordered();

   if(binary_search(kadrs_ordered,&Tmdb_kadr_ex(kadr_id),&kptr) )
   {
    str = get_image_filename(*kptr);
   }
  return str;
 }

 void   __fastcall TTlmViewerForm::file_begin( KeRTL::TFile & file,LPOTDM_PROTO_IMAGE_BEGIN  ibeg)
 {
     file.Close();
     UnicodeString image_name = ibeg->image_name;
     viewer_module->set_text(ibeg->image_id,image_name);
     UnicodeString file_name = viewer_module->get_data_dir();
     file_name+=IMAGE_FOLDER;
     ForceDirs(file_name.c_str());
     file_name+=image_name;
     file.Open(file_name.c_str(),OPEN_ALWAYS);
     if(file.IsValid())
     {
      file.SetSize(ibeg->image_size);
      file.SeekWr(0,FILE_BEGIN);
      LPFILETIME f_time = (LPFILETIME)& ibeg->image_time;
      SetFileTime(file(),f_time,f_time,f_time);
     }

 }

 void   __fastcall TTlmViewerForm::file_begin( LPOTDM_PROTO_IMAGE_BEGIN  ibeg)
 {
   Tkadrs::iterator kptr;
   TGKTlmViewer::locker l(viewer_module->get_mutex());
   Tkadrs & kadrs_ordered = viewer_module->get_kadrs_ordered() ;
   if(binary_search(kadrs_ordered,&Tmdb_kadr_ex(ibeg->kadr_id),&kptr))
   {
     KeRTL::TFile & file = (*kptr)->get_file();
     file_begin(file, ibeg);
   }
 }

 void   __fastcall TTlmViewerForm::file_chunk(KeRTL::TFile & file, LPOTDM_PROTO_IMAGE_CHUNK ichunk,bool end)
 {
   file.Write(ichunk->chunk_data,ichunk->chunk_size);
   if(end)
      file.Close();
 }

 void   __fastcall TTlmViewerForm::file_chunk( LPOTDM_PROTO_IMAGE_CHUNK ichunk,bool end)
 {
   Tkadrs::iterator kptr;
   TGKTlmViewer::locker l(viewer_module->get_mutex());
   Tkadrs & kadrs_ordered = viewer_module->get_kadrs_ordered() ;
   if(binary_search(kadrs_ordered,&Tmdb_kadr_ex(ichunk->kadr_id),&kptr))
   {
    KeRTL::TFile & file = (*kptr)->get_file();
    file_chunk(file,ichunk,end);

    if(end)
    {

      TTlmViewerForm * tv =(*kptr)->get_viewer_form();
      if(tv)
         PostMessage(tv->Handle,TLMKV_WM_RELOAD_IMAGE,(*kptr)->kadr_id,0);
    }
   }
 }


 void   __fastcall TTlmViewerForm::handle_image     (LPOTDM_PROTO_HEADER omph,bool beg,bool end)
 {
  if(beg)
  {
   //Начало приёма кадра
   LPOTDM_PROTO_IMAGE_BEGIN img_beg = (LPOTDM_PROTO_IMAGE_BEGIN) omph->data;
   file_begin((LPOTDM_PROTO_IMAGE_BEGIN)img_beg);
  }
  else
  {
    LPOTDM_PROTO_IMAGE_CHUNK chunk = (LPOTDM_PROTO_IMAGE_CHUNK)omph->data;
    file_chunk(chunk,end);
  }
 }

void __fastcall  TTlmViewerForm::WndProc    (Winapi::Messages::TMessage & msg)
{
  switch(msg.Msg)
  {

    case TLMKV_WM_RELOAD_IMAGE:  set_kadr_image  (msg.WParam);
         break;
    case TLMKV_WM_SET_ENTRYES :  set_kadr_entryes(msg.WParam);
         break;
    case TLMKV_WM_UPDATE_KADR :
         update_kadr(msg.LParam,msg.WParam);
         break;
    case TLMKV_WM_RECORD_CHANGES:
         repaint_changed_records();
         break;
    case WM_SYSCOMMAND:
         switch(msg.WParam)
         {
           case SC_MOVE  :
           case SC_MOVE|2:
           case SC_CLOSE :
           case SC_SIZE  :    if(fixed) return;
           default       :    TForm::WndProc(msg); break;
         }
         break;
    default:
    TForm::WndProc(msg);
    break;
  }
}

void __fastcall  TTlmViewerForm::repaint_changed_records()
{
 if(painter_created)
 {
   TGKTlmViewer::locker l(viewer_module->get_mutex());
   Trecord_changes::iterator ptr = viewer_module->get_changed_records().begin();
   Trecord_changes::iterator end = viewer_module->get_changed_records().end  ();
   while(ptr!=end)
   {
    if(RecordInfo && RecordInfo->Visible && (DWORD)RecordInfo->Tag == ptr->first)
       {
        mdb_record_ex rec;
        get_record(this,ptr->first,false,rec);
        set_record_info(rec);
       }
    painter->repaint_record(ptr->first);
    ++ptr;
   }
  }

}

bool __fastcall check_image_timestamp(__int64 image_time,AnsiString  image_name)
{
  KeRTL::TFile file (image_name.c_str(),OPEN_EXISTING);
  if(file.IsValid())
  {
   __int64 file_time;
   GetFileTime(file(),(LPFILETIME)&file_time,NULL,NULL);
   if(image_time == file_time) return true;
  }
  return false;
}

void __fastcall TTlmViewerForm::set_kadr_image   (Tmdb_kadr_ex * kadr)
{
  if(painter && kadr && kadr->image_id!=(DWORD)-1)
  {
    UnicodeString image_fname = get_image_filename(kadr);
    if(KeRTL::is_file_exist(image_fname.c_str()) && check_image_timestamp(kadr->image_time,image_fname))
       painter->set_bkgnd(*&wstring(image_fname.c_str()));
       else
       {
         request_kadr_image(kadr->kadr_id);
       }
  }
}

void __fastcall  TTlmViewerForm::set_kadr_image     (DWORD kadr_id)
{
   TGKTlmViewer::locker l(viewer_module->get_mutex());
   Tkadrs::iterator kptr;
   if(binary_search(viewer_module->get_kadrs_ordered(),&Tmdb_kadr_ex(kadr_id),&kptr))
      set_kadr_image(*kptr);
}


void __fastcall TTlmViewerForm::set_kadr_entryes(Tmdb_kadr_ex * kadr)
{
  if(is_painter_created())
  {
   //painter->clear_entryes();
   {
       TGKTlmViewer::locker l(viewer_module->get_mutex());
       Tentryes_list &ent = kadr->get_entryes();
       painter->set_entryes_count(ent.size());
       Tentryes_list::iterator ent_ptr = ent.begin();
       Tentryes_list::iterator ent_end = ent.end  ();
       int idx = 0;
       while(ent_ptr<ent_end)
       {
         painter->set_entry(idx,*ent_ptr);
         ++idx;
         ++ent_ptr;
       }
   }
   painter->Repaint();
  }
}

void __fastcall  TTlmViewerForm::set_kadr_entryes   (DWORD kadr_id)
{
  Tkadrs::iterator kptr;
  TGKTlmViewer::locker l(viewer_module->get_mutex());
  if(binary_search(viewer_module->get_kadrs_ordered(),&Tmdb_kadr_ex(kadr_id),&kptr))
  {
   set_kadr_entryes(*kptr);
  }
}


UnicodeString __fastcall TTlmViewerForm::get_kadr_name(Tmdb_kadr_ex & kadr ,bool full_name)
{
 UnicodeString str;
 if(!full_name || !get_text(kadr.name_id,str))
    if(!get_text(kadr.sname_id,str))
      str = get_kadr_text(kadr);
 return str;
}

UnicodeString __fastcall TTlmViewerForm::get_kadr_text(Tmdb_kadr_ex & kadr)
{
   UnicodeString str;
   if( !get_text(kadr.sname_id,str) && !get_text(kadr.name_id,str))
       str.printf(L"Kadr ID-%u",kadr.kadr_id);
   return str;
}

void __fastcall TTlmViewerForm::FormDestroy(TObject *Sender)
{
    if(kadr) {kadr->set_viewer_form(NULL); kadr = NULL;}
    destroy_painter();
}
//---------------------------------------------------------------------------

void   __fastcall TTlmViewerForm::check_kadr_image      (DWORD kadr_id)
{
 //Проверка измображения и запрос при необходимости
  Tkadrs::iterator kptr;
  TGKTlmViewer::locker l(viewer_module->get_mutex());
 if(binary_search(viewer_module->get_kadrs_ordered(),&Tmdb_kadr_ex(kadr_id),&kptr)  && (*kptr)->image_id != DWORD(-1))
 {
     bool need_request = true;
     AnsiString image_file = this->get_image_filename(kadr_id);

     if(image_file.Length())
     {
      if(is_file_exist(image_file.c_str()))
        {
           KeRTL::TFile file(image_file.c_str(),OPEN_EXISTING);
           if(file.IsValid())
           {
             __int64  file_time = 0;
             if(GetFileTime(file(),(LPFILETIME)&file_time,NULL,NULL))
             {
                if((*kptr)->image_time == file_time )
                 need_request = false;
             }
           }
        }
     }
     if(need_request)
        request_kadr_image(kadr_id);
 }
}

void   __fastcall TTlmViewerForm::request_kadr_image    (DWORD kadr_id)
{
   viewer_module->send_command(viewer_module->get_modem_source(),FA_OTD_MEDIUM,true,true,OTDMPROTO_CMD_GETKADR_IMAGE,0,&kadr_id,sizeof(kadr_id));
}



void   __fastcall TTlmViewerForm::update_kadr  (DWORD kadr_id,DWORD mask)
{
  if(painter_created && kadr)
  {
   bool full_repaint = false;
   if(mask & (MDB_KADR_FIELD_NAME|MDB_KADR_FIELD_SNAME))
	  Caption = get_kadr_name(*kadr,true);

  if(mask&(MDB_KADR_FIELD_DIAG|MDB_KADR_FIELD_STATE|MDB_KADR_FIELD_FLAGS))
	  {
		full_repaint = true;
	  }

	if(mask&(MDB_KADR_FIELD_CHANGES|MDB_KADR_FIELD_DISCRETE_CHANGES|MDB_KADR_FIELD_ALARM_CHANGES))
	   {
		full_repaint = false;
	   }

	if(mask&MDB_KADR_FIELD_ENTRY_COUNT)
	   {
		set_kadr_entryes(kadr);
	   }

	if(mask&(MDB_KADR_FIELD_IMAGE))
	  {
		set_kadr_image(kadr);
		full_repaint = true;
	  }

	if(mask&MDB_KADR_FIELD_SIZE)
	{
	  set_kadr_size(kadr->kadr_size.cx,kadr->kadr_size.cy);
	  full_repaint = true;

	}

	if(mask )
       {
        painter->set_kadr(kadr,mask);
        painter->Repaint(full_repaint,NULL);
       }
  }

}

 void __fastcall  TTlmViewerForm::set_kadr_size      (LONG cx, LONG cy)
 {
    int new_width  = this->ClientWidth;
    int new_height = this->ClientHeight;
    size_for_kadr_size(cx,cy,new_width,new_height);
    ClientWidth  = new_width;
	ClientHeight = new_height;
	HMONITOR mon = MonitorFromWindow(Handle,MONITOR_DEFAULTTOPRIMARY);
	MONITORINFO mon_info;
	bzero(&mon_info,sizeof(mon_info));
	mon_info.cbSize   =  sizeof(mon_info);
	GetMonitorInfo(mon,&mon_info);
	KeWin::TRect r = mon_info.rcWork;
        int height = viewer_module->get_ready_painter()->Height;
	r.Grow(0,-height);
	r.Move(0 ,height);
	//Application->ProcessMessages();
    //Top  = max(int(r.top),Top);
 }

 void __fastcall  TTlmViewerForm::size_for_kadr_size (LONG cx, LONG cy, int & form_width,int & form_height)
 {
   form_width  = ClientWidth;
   form_height = ClientHeight;
 }

void    __fastcall TTlmViewerForm::open_kadr (Tmdb_kadr_ex * _kadr)
{
   if(!is_painter_created())   create_painter();
   if(kadr!=_kadr)
   {
     kadr =_kadr;
     painter->set_kadr(kadr,-1);
     update_kadr(kadr->kadr_id,-1);
   }
}

void __fastcall TTlmViewerForm::FormClose(TObject *Sender, TCloseAction &Action)
{
    Action = caFree;
}
//---------------------------------------------------------------------------

void __fastcall TTlmViewerForm::kadr_order(modem_addr ma,DWORD kadr_id,bool order )
{
  if(kadr_id!=(DWORD)-1)
  {
   DWORD order_data[2];
   order_data      [0] = kadr_id;
   order_data      [1] = order ? -1:0;
   viewer_module->send_command(ma,FA_OTD_MEDIUM,true,true,OTDMPROTO_CMD_GETKADR |OTDM_ORDER,0,order_data,sizeof(order_data));
  }
}

void   __fastcall TTlmViewerForm::request_paint_variants()
{
  bool req_beg = true,req_end = false;
  BYTE buffer[4096];
  LPOTDM_PAINT_VARIANTS req  = (LPOTDM_PAINT_VARIANTS)buffer;
  LPMDB_PAINT_VARIANT   req_pv = req->pvars;
  req->count = 0;
  DWORD send_size = sizeof(req->count);

  TGKTlmViewer::locker l(viewer_module->get_mutex());
  Tpaint_variants::iterator beg = viewer_module->get_paint_vars().begin();
  Tpaint_variants::iterator end = viewer_module->get_paint_vars().end();
  while(beg<end)
  {
    memcpy(req_pv,&*beg,sizeof(*req_pv));
    send_size+=sizeof(*req_pv);
    ++req->count;
    ++req_pv;
    ++beg;
    if((send_size+sizeof(*req_pv))>sizeof(buffer) && beg<end)
       {
        viewer_module->send_command(viewer_module->get_modem_source(),FA_OTD_MEDIUM,req_beg,req_end,OTDMPROTO_CMD_GET_PAINT_VARIANTS|OTDM_ORDER,0,buffer,send_size);
        req->count = 0;
        req_beg = false;
        send_size = sizeof(req->count);
       }
  }

  req_end = true;
  viewer_module->send_command(viewer_module->get_modem_source(),FA_OTD_MEDIUM,req_beg,req_end,OTDMPROTO_CMD_GET_PAINT_VARIANTS|OTDM_ORDER,0,buffer,send_size);
}



void __fastcall TTlmViewerForm::set_kadr_ready_flags(Tmdb_kadr_ex * _kadr,DWORD set_flags,DWORD reset_flags)
{
  if(_kadr)
    {
       DWORD old_flags = _kadr->ready_flags;
       if(set_flags)
       _kadr->ready_flags|= set_flags;
       if(reset_flags)
       {
        if(reset_flags&RDY_FLAG_STABLE)
           _kadr->ready_flags&=~RDY_FLAG_STABLE;
        _kadr->ready_flags&=~reset_flags;
       }

       if((old_flags^_kadr->ready_flags) && viewer_module->get_ready_painter())
          PostMessage(viewer_module->get_ready_painter()->Handle,TLMKV_WM_UPDATE_KADR,MDB_KADR_FIELD_VALUES,_kadr->kadr_id);
    }
}

UnicodeString   __fastcall TTlmViewerForm::get_state_tu_text(DWORD state)
{
  UnicodeString ret;
   if(state & (MDBR_STATE_TUTR_ERROR|MDBR_STATE_TUTR_PREPARE|MDBR_STATE_TUTR_ON_MORE|MDBR_STATE_TUTR_OFF_LESS))
   {
    ret = L"ТУ:";
    if(state&MDBR_STATE_TUTR_PREPARE    ) ret+= L"Подготовка ";
    if(state&MDBR_STATE_TUTR_ON_MORE    ) ret+= L"Активно-ВКЛ ";
    if(state&MDBR_STATE_TUTR_OFF_LESS   ) ret+= L"Активно-ОТКЛ ";
    if(state&MDBR_STATE_TUTR_ERROR       ) ret+= L"Ошибка ";
   }
  return ret;
}



UnicodeString   __fastcall TTlmViewerForm::get_state_text(DWORD state)
{
  UnicodeString ret;
   if(state&MDBR_STATE_DISCRETE      ) ret+= L"Дискретный ";
   if(state&MDBR_STATE_WORKOUT       ) ret+= L"Выведен из работы ";
   if(state&MDBR_STATE_TUFLAG        ) ret+= L"Наличие ТУ ";
   if(state&MDBR_STATE_HIDDEN        ) ret+= L"Скрытый ";
   if(state&MDBR_STATE_STATIC        ) ret+= L"Статичный ";
   if(state&MDBR_STATE_NORMAL        ) ret+= L"";
   if(state&MDBR_STATE_NORECEIPT     ) ret+= L"Неквитируемый ";
   if(state&MDBR_STATE_PREKVIT       ) ret+= L"";
   if(state&MDBR_STATE_FLASH         ) ret+= L"Мигание ";
   if(state&MDBR_STATE_DYN_NORMAL    ) ret+= L"";
   ret += get_state_view_text(state);
   ret += get_state_tu_text  (state);
  return ret;
}

UnicodeString   __fastcall TTlmViewerForm::get_state_view_text(DWORD state)
{
  UnicodeString ret;
  if(state & MDBR_STATE_WORKOUT)
     ret +=L"Выведен из работы";
  else
  {
   if(state&MDBR_STATE_ALARM         ) ret+= L"Авария";
     else
     {
      if(state&MDBR_STATE_DANGER        ) ret+= L"Предупреждение ";
     }
  }
  return ret;
}


 bool __fastcall  TTlmViewerForm::get_not_gray_bkgnd ()
 {
   return painter ?  (bool)painter->get_not_gray_bkgnd() : false;
 }

 void __fastcall  TTlmViewerForm::set_not_gray_bkgnd (bool value)
 {
     if(painter) painter->set_not_gray_bkgnd(value);
 }

 void __fastcall  TTlmViewerForm::set_fixed(bool val)
 {
   if(fixed != val)
   {
    Ffixed = val;
    if(val)
    {
     //Зафиксировать
    }
    else
    {
      //Расфиксировать
    }
   }
 }

void __fastcall TTlmViewerForm::FormCloseQuery(TObject *Sender, bool &CanClose)
{
  CanClose = !fixed;
}
//---------------------------------------------------------------------------

UnicodeString __fastcall TTlmViewerForm::get_time_text(__int64 tmval,char time_sep,bool msec)
{
        UnicodeString text;
        SYSTEMTIME st;
        FileTimeToSystemTime((LPFILETIME)&tmval,&st);
        text.printf(L"%04hd-%02hd-%02hd %02hd%c%02hd%c%02hd",st.wYear,st.wMonth,st.wDay,st.wHour,(char)time_sep,st.wMinute,(char)time_sep,st.wSecond);
        //int _msc = tmval%10000;
        if(msec) text.cat_printf(L".%03hd",st.wMilliseconds);
        return text;
}

//---------------------------------------------------------------------------
void   __fastcall TTlmViewerForm::set_record_info(const mdb_record_ex & rec)
{
  if(!RecordInfo)
     {
      TTlmViewerForm * ready_painter = viewer_module->get_ready_painter();
      if(ready_painter)
         {
           RecordInfo = new TRecordInfo(ready_painter);
           RecordInfo->Top  = ready_painter->Top  + ready_painter->Height;
           RecordInfo->Left = ready_painter->Left + ready_painter->Width - RecordInfo->Width;
         }

     }
  RecordInfo->Visible = true;
  RecordInfo->Tag = rec.rec_id;
  TListItem * item ;
  UnicodeString str;
  item = RecordInfo->get_item(RECINFO_RECID);
  item->SubItems->Strings[0] = rec.rec_id;

  item = RecordInfo->get_item(RECINFO_OTDADDR);
  str.printf(L"%03hd.%03hd.%03hd.%03hd-%03hd"
                   ,(short)rec.addr.addr.pu
                   ,(short)rec.addr.addr.cp
                   ,(short)rec.addr.addr.fa
                   ,(short)rec.addr.addr.sb
                   ,(short)rec.addr.param_number
                   );
  item->SubItems->Strings[0] = str;


  item = RecordInfo->get_item(RECINFO_TYPE);
  item->SubItems->Strings[0] = rec.is_discrete() ? L"Дискретный":L"Аналоговый";

  item = RecordInfo->get_item(RECINFO_SNAME);
  item->SubItems->Strings[0] = get_text(rec.sname);

  item = RecordInfo->get_item(RECINFO_NAME);
  item->SubItems->Strings[0] = get_text(rec.name);
  RecordInfo->Caption = item->SubItems->Strings[0];

  item = RecordInfo->get_item(RECINFO_VALUE);
  if(rec.is_discrete())
     str.printf(L"%s",DWORD(rec.dw_value) ? L" ВКЛ":L"ОТКЛ");
     else
     str.printf(L"%.3f",rec.value);
     if(rec.options& MDBR_OPT_DOUBLE_TYPE)
        str.cat_printf(L" [%.4lf]",rec.otd_val_dbl);
        else
        {
         if(rec.options& MDBR_OPT_FLOAT_TYPE)
            str.cat_printf(L" [%.4f]",rec.otd_val_flt);
            else
            str.cat_printf(L" [%d]",rec.is_signed_kvants() ? rec.get_signed_kvants(rec.otd_val) : rec.otd_val);
        }

  item->SubItems->Strings[0] = str;

  item = RecordInfo->get_item(RECINFO_TIME);
  str = get_time_text(rec.time,':',true);
  item->SubItems->Strings[0] = str;

  item = RecordInfo->get_item(RECINFO_DIAG);
  str.printf(L"%s [otd_pdiag %X]",rec.diag ? L"BAD":L"OK",rec.otd_pd);
  item->SubItems->Strings[0] = str;

  item = RecordInfo->get_item(RECINFO_WORK);
  item->SubItems->Strings[0] = rec.is_worked() ? L"да":L"выведен";

  item = RecordInfo->get_item(RECINFO_TU);
  item->SubItems->Strings[0] = this->get_state_tu_text(rec.state) ;

  item = RecordInfo->get_item(RECINFO_STATE);
  item->SubItems->Strings[0] = this->get_state_view_text(rec.state) ;
}



