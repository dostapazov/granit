#pragma hdrstop
#include "gkawpmodule.hpp"
#include "awp_mod_param.h"
#include "awp_main_form.h"
#include "tlm_kadr.h"



DWORD  __fastcall TGkAwpModule::send_data(LPMPROTO_HEADER mph)
{
  mph->internal          = MP_INTERNAL_DEFAULT;
  mph->pkt_num           = InterlockedIncrement(&pkt_num);
  mproto_protect(mph);
  DWORD ret = modemmer.call(MDMCM_SEND,(LPARAM)mph,MPROTO_SIZE(mph));
  return ret;
}
//-----------------------------------------------------------------------------

DWORD       __fastcall TGkAwpModule::send_command(modem_addr addr_to,DWORD fa,bool begin,bool finish,DWORD cmd,DWORD err,LPVOID data,DWORD data_size )
{
 DWORD ret = 0;
 BYTE   buffer[16384];
 LPMPROTO_HEADER     mph;
 LPOTDM_PROTO_HEADER omph;
 DWORD  need_size = sizeof(MPROTO_HEADER)-sizeof(mph->data[0])+sizeof(*omph)-sizeof(omph->data[0])+data_size;
 if(need_size<sizeof(buffer))
   mph = (LPMPROTO_HEADER) buffer;
  else
   mph = (LPMPROTO_HEADER) new BYTE[need_size];
 if(mph)
 {
 ZeroMemory(mph,need_size);
 mph->addr_to.addr   = addr_to.addr;

 mph->addr_from.addr = modem_addr(0,0,int_line,-1).addr;
 mph->data_size = need_size+(sizeof(mph->data)-sizeof(*mph));
 mph->fa = fa;
 omph =(LPOTDM_PROTO_HEADER) mph->data;
 omph->command    = cmd&(OTDM_COMMAND_MASK|OTDM_ORDER);
 //omph->command   |= OTDM_RESPOND;
 if(begin)
   omph->command  |= OTDM_BEGINDATA;
 if(finish)
  omph->command  |= OTDM_ENDDATA;
  if(err)
  omph->command  |= OTDM_ERROR;
  omph->error     = err;
  omph->data_size = data_size;
  memcpy(omph->data,data,data_size);
  ret = send_data(mph);
  if( buffer!=(LPBYTE)mph)
      delete  [](LPBYTE)mph;
  }
  return ret;
}
//-----------------------------------------------------------------------------

void    __fastcall TGkAwpModule::request_record_names (DWORD rec_id,bool short_name,bool long_name)
{
    if(long_name)
       send_command(params.modem_source,FA_OTD_MEDIUM,true,true,OTDMPROTO_CMD_GET_RECORD_FULL_NAME,0,&rec_id,sizeof(rec_id));
    if(short_name)
       send_command(params.modem_source,FA_OTD_MEDIUM,true,true,OTDMPROTO_CMD_GET_RECORD_SNAME,0,&rec_id,sizeof(rec_id));
}
//-----------------------------------------------------------------------------
void    __fastcall TGkAwpModule::request_kadr_image (DWORD kadr_id)
{
  send_command(params.modem_source,FA_OTD_MEDIUM,true,true,OTDMPROTO_CMD_GETKADR_IMAGE,0,&kadr_id,sizeof(kadr_id));
 }
//-----------------------------------------------------------------------------


 void    __fastcall TGkAwpModule::send_order_command(DWORD kadr_id,bool order)
 {
   if(this->is_running())
   {
    modem_addr addr_to = params.modem_source;
    DWORD order_data[2];
    order_data      [0] = kadr_id;
    order_data      [1] = order ? -1:0;
    DWORD cmd = OTDMPROTO_CMD_GETKADR | OTDM_ORDER;
    send_command(addr_to,FA_OTD_MEDIUM,true,true,cmd,0,order_data,sizeof(order_data));
   }
 }
//-----------------------------------------------------------------------------

 int TGkAwpModule::rx_thread_proc   ()
 {
  TSynchroSet ss;
  ss+= *thread_term_event;
  ss+= rx_queue->GetEvent();
  DWORD wr = -1;
  rx_buffer rxb;
  while(!thread_term_request && wr)
  {
    wr = ss.Wait(INFINITE);
    while(wr && !thread_term_request && rx_queue->QueueCount())
        {
         int len = 0;
         while(!rx_queue->GetFromQueue(rxb.begin(),rxb.get_free_size(),&len))
               rxb.set_size(rx_queue->TopDataSize());

         __try{
               handle_recv((LPMPROTO_HEADER)rxb.begin());
              }
         __finally{};
         rxb.clear();
        }
  }
  rx_queue->DropData() ;
  return 0;
 }
//-----------------------------------------------------------------------------

 LRESULT __fastcall TGkAwpModule::handle_recv( LPMPROTO_HEADER mph)
 {
  LRESULT ret = GKH_RET_SUCCESS;
    if(mproto_check(mph))
    {
     switch(mph->fa)
     {
       case FA_SECURITY    : ret = handle_security(mph->addr_from, (LPOTDM_PROTO_HEADER)mph->data,mph->data_size);break;
       case FA_OTD_MEDIUM  : ret = handle_otdm    (mph->addr_from, (LPOTDM_PROTO_HEADER)mph->data,mph->data_size);break;
       case FA_OTD_MEDIUM_ALARM_ARCHIVE:
                             ret = handle_history (mph->addr_from, (LPOTDM_PROTO_HEADER)mph->data,mph->data_size);break;
     }
    }
   return ret;
 }
//-----------------------------------------------------------------------------

 LRESULT __fastcall TGkAwpModule::handle_otdm( MODEM_ADDR & from, LPOTDM_PROTO_HEADER omph,DWORD data_size)
 {
     OTDM_PROTO_STATE ps(omph);

     if(ps.is_respond )
     {
      if(ps.command == OTDMPROTO_CMD_ENUM_KADRS) handle_enum_kadrs  (from,ps);
      else
      {
       if(is_source_addr(from))
       {
          switch(ps.command)
          {
           case OTDMPROTO_CMD_GETKADR        : handle_get_kadr    (ps);
           break;
           case OTDMPROTO_CMD_GETKADR_IMAGE  : handle_kadr_image  (ps);
           break;
           case OTDMPROTO_CMD_GETKADR_ENTRYES: handle_kadr_entryes(ps);
           break;
           case OTDMPROTO_CMD_GETKADR_RECORDS: handle_get_records (ps);
           break;
           case OTDMPROTO_CMD_RECORDS        : handle_records     (ps);
           break;
           case OTDMPROTO_CMD_GET_RECORD_SNAME     :
           case OTDMPROTO_CMD_GET_RECORD_FULL_SNAME: handle_name  (ps,false);
           break;
           case OTDMPROTO_CMD_GET_RECORD_NAME      :
           case OTDMPROTO_CMD_GET_RECORD_FULL_NAME : handle_name  (ps,true);
           break;
           case    OTDMPROTO_CMD_GET_PAINT_VARIANTS: handle_paint_variants(ps);
           break;
           case    OTDMPROTO_CMD_PAINT_IMAGE       : handle_paint_image(ps);
           break;
           case   OTDMPROTO_CMD_NOTIFY_DBCHANGE    : handle_db_change   ();
           break;
           case  OTDMPROTO_CMD_TUOPERATION         : handle_tu_operation(ps);
          }
       }
      }
     }
     return GKH_RET_SUCCESS;
 }
//-----------------------------------------------------------------------------

void __fastcall TGkAwpModule::handle_name        (OTDM_PROTO_STATE & ps,bool long_name)
{
 //Обработка имени записи
  if(!ps.is_error)
    set_string(ps.p_name->name_id,UnicodeString (ps.p_name->text,ps.p_name->len));
}
//-----------------------------------------------------------------------------
void __fastcall TGkAwpModule::_increment_sounds(record_t & dst_rec)
{
  if(is_record_normal(&dst_rec))
     ++back_count;
     else
     {
      if(dst_rec.is_alarm_state() && !params.sound_alarm.IsEmpty())
         {
          ++alarms_count;
          return;
         }

      if(dst_rec.is_danger_state() && !params.sound_danger.IsEmpty())
         {
          ++danger_count;
          return;
         }
      ++disparity_count;
     }
}
//-----------------------------------------------------------------------------
DWORD __fastcall TGkAwpModule::_copy_record_compare(record_t & dst_rec,const LPMDB_RECORD  src_rec,DWORD mask)
{
    DWORD  ch_mask = 0;

    if( (mask & MDBR_FIELD_DIAG      ) && (dst_rec.diag != src_rec->diag)    ) {dst_rec.diag = src_rec->diag  ;ch_mask |=MDBR_FIELD_DIAG;}

    if( (mask & MDBR_FIELD_STATE     ) && ((dst_rec.state&~(MDBR_STATE_FLASH|MDBR_STATE_DYN_NORMAL)) != (src_rec->state&~(MDBR_STATE_FLASH|MDBR_STATE_DYN_NORMAL))))
      {
       WORD new_state  = (src_rec->state&~(MDBR_STATE_FLASH|MDBR_STATE_DYN_NORMAL))|(dst_rec.state&(MDBR_STATE_FLASH|MDBR_STATE_DYN_NORMAL));
       if(new_state & MDBR_STATE_NORECEIPT)
          {
           //Если неквитируемый, то  приводим к нормальное положение к виду источника
           new_state &=~MDBR_STATE_DYN_NORMAL;
           if(new_state&MDBR_STATE_NORMAL)
               new_state |=MDBR_STATE_DYN_NORMAL;
          }

       WORD state_ch_mask = dst_rec.state ^ new_state;
            dst_rec.state = new_state;
       ch_mask|= MDBR_FIELD_STATE;
       if(state_ch_mask&MDBR_STATE_TUTR)
          ch_mask |= MDBR_TUTR_STATE_CHANGED;
      }

    if( (mask & MDBR_FIELD_TIME      ) && dst_rec.time != src_rec->time    )  {dst_rec.time   = src_rec->time; ch_mask |= MDBR_FIELD_TIME; }

    if( (mask & MDBR_FIELD_KVANTS    ) && dst_rec.kvants != src_rec->kvants)  {dst_rec.kvants = src_rec->kvants; ch_mask|=MDBR_FIELD_KVANTS;}

    if( (mask & MDBR_FIELD_MIN_VALUE ) ) {dst_rec.min_value = src_rec->min_value;ch_mask |= MDBR_FIELD_MIN_VALUE;}
    if( (mask & MDBR_FIELD_MAX_VALUE ) ) {dst_rec.max_value = src_rec->max_value;ch_mask |= MDBR_FIELD_MAX_VALUE;}

   {
    locker l(mut);
    if( (mask & MDBR_FIELD_NAME ) || ( src_rec->name != DWORD(-1) && (dst_rec.name  != src_rec->name  || !names.count(src_rec->name)) )  )
     {
         dst_rec.name  = src_rec->name ; ch_mask|=MDBR_FIELD_NAME ;
     }

    if( (mask & MDBR_FIELD_SNAME) || (src_rec->sname != DWORD(-1) && (dst_rec.sname != src_rec->sname || !names.count(src_rec->sname)) ) )
     {
         dst_rec.sname = src_rec->sname; ch_mask|=MDBR_FIELD_SNAME;
     }
    }

    if( (mask & MDBR_FIELD_MIN_DANGER) ) {dst_rec.min_danger = src_rec->min_danger; ch_mask|=MDBR_FIELD_MIN_DANGER;}
    if( (mask & MDBR_FIELD_MAX_DANGER) ) {dst_rec.max_danger = src_rec->max_danger; ch_mask|=MDBR_FIELD_MAX_DANGER;}

    if( (mask & MDBR_FIELD_MIN_ALARM ) ) {dst_rec.min_alarm  = src_rec->min_alarm ; ch_mask|=MDBR_FIELD_MIN_ALARM ;}
    if( (mask & MDBR_FIELD_MAX_ALARM ) ) {dst_rec.max_alarm  = src_rec->max_alarm ; ch_mask|=MDBR_FIELD_MAX_ALARM ;}

    if( (mask & MDBR_FIELD_ADDR      ) && memcmp(&dst_rec.addr,&src_rec->addr,sizeof(dst_rec.addr)) ) {dst_rec.addr = src_rec->addr; ch_mask|=MDBR_FIELD_ADDR;}
    if( (mask & MDBR_FIELD_OTDVALUE  ) && dst_rec.otd_val != src_rec->otd_val) {dst_rec.otd_val = src_rec->otd_val; ch_mask |= MDBR_FIELD_OTDVALUE;}
    if( (mask & MDBR_FIELD_OTDPD     ) && dst_rec.otd_pd  != src_rec->otd_pd ) {dst_rec.otd_pd  = src_rec->otd_pd ; ch_mask |= MDBR_FIELD_OTDPD;   }
    if( (mask & MDBR_FIELD_OPTIONS   ) && dst_rec.options != src_rec->options)
       {
        dst_rec.options = src_rec->options;
        if(dst_rec.options & (MDBR_OPT_ALARM|MDBR_OPT_DANGER ))
           {
            //Если стал аварийным то нормальное положение только 0
            dst_rec.state &=~(MDBR_STATE_DYN_NORMAL|MDBR_STATE_NORMAL);
            ch_mask|=MDBR_FIELD_STATE;
           }
        ch_mask |= MDBR_FIELD_OPTIONS;
       }

    if( (mask & MDBR_FIELD_VALUE     ) && (fabs(dst_rec.value - src_rec->value)>.00001) )
       {
        dst_rec.value = src_rec->value;
         if(dst_rec.is_discrete() && !dst_rec.is_static() )
         {
          ++dst_rec.changes_count;
          if(!dst_rec.is_caution())
              dst_rec.state |= MDBR_STATE_FLASH;

          if((dst_rec.changes_count&MDBR_CHANGES_COUNT_MASK) <= MDBR_CH_COUNT_MANY_IGNORE)   _increment_sounds(dst_rec);
         }
          dst_rec.changes_count|=MDBR_CHANGES_NEW_ATTRIB;
          ch_mask|=MDBR_FIELD_CH_COUNT;
          ch_mask|=MDBR_FIELD_VALUE;
       }


//    if( (mask & MDBR_FIELD_SCALEID   ) )
//    if( (mask & MDBR_FIELD_PERCENT   ) )
//    if( (mask & MDBR_FIELD_LASTVALUE ) )
//    if( (mask & MDBR_FIELD_SECURITY  ) )
//    if( (mask & MDBR_FIELD_CH_COUNT  ) )

//    if( mask & MDBR_FIELD_RESERV1    )
//    if( mask & MDBR_FIELD_RESERV2    )
//    if( mask & MDBR_FIELD_RESERV3    )
//    if( mask & MDBR_FIELD_RESERV4    )
//    if( mask & MDBR_FIELD_RESERV5    )
//
//    if( mask & MDBR_ALARMS_CHANGED   )
//    if( mask & MDBR_COMBINE_FORMULA  ) // изменние формулы
//    if( mask & MDBR_COMBINE_LIST     ) // изменение списка

    if( (mask & MDBR_RECORD_REMOVED  ) ) ch_mask|=MDBR_RECORD_REMOVED;// Запись удалена

  return ch_mask;
}
void __fastcall TGkAwpModule::handle_get_records (OTDM_PROTO_STATE & ps)
{
  if(!ps.is_error)
   {

    LPMDB_RECORD rec_beg = ps.p_kadr_recs->records;
    LPMDB_RECORD rec_end = rec_beg+ps.p_kadr_recs->rec_count;
    while(rec_beg<rec_end)
    {

     DWORD ch_mask = 0x003FFFF;
     locker l(mut);
     records_t::iterator rptr;
     if(search_record_t(rec_beg->rec_id,&rptr))
        {

         ch_mask&= ~(MDBR_FIELD_NAME|MDBR_FIELD_SNAME);
         ch_mask = _copy_record_compare(*rptr,rec_beg,ch_mask);
        }
        else
        {
         records.insert(rptr,*rec_beg);
         cc_flags |= CCFL_RECORDS;
        }
     if(ch_mask)  record_changes[rec_beg->rec_id] |= ch_mask;
     ++rec_beg;
    }
   }
}
//-----------------------------------------------------------------------------
void __fastcall TGkAwpModule::handle_records     (OTDM_PROTO_STATE & ps)
{
  if(!ps.is_error && ps.data_size )
  {
     LPBYTE src_ptr   = (LPBYTE) ps.p_records->records;
     DWORD  rec_count =  ps.p_records->rec_count;
     int  data_size = ps.data_size-sizeof(ps.p_records->rec_count);
     locker l(mut);
     for(DWORD i = 0;i<rec_count;i++)
     {
       LPOTDM_PROTO_RECORD prec = (LPOTDM_PROTO_RECORD)src_ptr;
         mdb_record rec;
         DWORD ch_mask = prec->mask;
         int len = sizeof(prec->mask)+otdm_unpack_record(prec->rec_data,data_size,ch_mask,&rec);
         records_t::iterator rptr;
         if(search_record_t(rec.rec_id,&rptr))
             ch_mask = _copy_record_compare(*rptr,&rec,ch_mask);
            else
            {
             records.insert(rptr,rec);
             cc_flags |= CCFL_RECORDS;
            }
         if(ch_mask) record_changes[rec.rec_id] |= ch_mask;
       src_ptr+=len;
       data_size-=len;
     }
  }
}
//-----------------------------------------------------------------------------
void __fastcall TGkAwpModule::handle_kadr_state  (kadr_t * kadr ,LPOTDM_PROTO_KADR  pkadr)
{
  if(pkadr->mask & MDB_KADR_FIELDS_HIST_TIMES)
  {
   kadr_state old_state,new_state;
   old_state.kadr_state = kadr->kadr.kadr_state;
   new_state.kadr_state = pkadr->kadr.kadr_state;
   if(new_state.arch_count && new_state.arch_count != old_state.arch_count)
       kadr->push_hist_times(false,pkadr->kadr.hist_beg_time,pkadr->kadr.hist_end_time);

   if(new_state.alarm_arch_count && new_state.alarm_arch_count != old_state.alarm_arch_count)
       kadr->push_hist_times(true,pkadr->kadr.hist_beg_time,pkadr->kadr.hist_end_time);
   }
   kadr->kadr.kadr_state = pkadr->kadr.kadr_state;
}
//-----------------------------------------------------------------------------
void __fastcall TGkAwpModule::handle_get_kadr    (OTDM_PROTO_STATE & ps)
{
  if(!ps.is_error)
  {

    kadr_t * kadr = (kadr_t *)get_kadr(ps.p_kadr->kadr.kadr_id);
    if(kadr)
    {
      //Установить период опроса
      locker l(mut);
      if(ps.p_kadr->mask == MDB_KADR_REMOVED)
      {
         remove_kadr_order(ps.p_kadr->kadr.kadr_id);
      }
      else
      {
      kadr->recall_timer_set(get_kadr_recall_period());
      if(kadr->kadr.kadr_diag != ps.p_kadr->kadr.kadr_diag)
         {
          kadr->ready_state &=~rds_stable_ready;
          ps.p_kadr->mask|= MDB_KADR_FIELD_DIAG;
         }
         else
         ps.p_kadr->mask&= ~MDB_KADR_FIELD_DIAG;

      if(ps.p_kadr->mask & MDB_KADR_FIELD_STATE)
          handle_kadr_state(kadr,ps.p_kadr);

      kadr->kadr = ps.p_kadr->kadr;
      if(ps.p_kadr->mask &(MDB_KADR_FIELD_NAME|MDB_KADR_FIELD_SNAME))
        {
          UnicodeString lname,sname;
          get_kadr_names(ps.p_kadr,sname,lname);
          if((ps.p_kadr->mask &MDB_KADR_FIELD_NAME) && !set_string(kadr->kadr.name_id,lname))
             ps.p_kadr->mask&= ~MDB_KADR_FIELD_NAME;

          if(ps.p_kadr->mask &MDB_KADR_FIELD_SNAME && !set_string(kadr->kadr.sname_id,sname))
              ps.p_kadr->mask&= ~MDB_KADR_FIELD_SNAME;

        }


      kadr_changes[kadr->kadr.kadr_id]|=ps.p_kadr->mask;
     }
    }
  }else
   {
     remove_kadr_order(*ps.p_data);
     set_config_changes(CCFL_KADRS);
     if(main_form) PostMessage(main_form->Handle,WM_DO_LAYOUT_READY,0,0);
   }
}
//-----------------------------------------------------------------------------
void __fastcall TGkAwpModule::handle_kadr_entryes(OTDM_PROTO_STATE & ps)
{
 //Получение элеменотов кадра
 if(!ps.is_error)
 {
    kadr_t * skadr = (kadr_t *)get_kadr(ps.p_entryes->kadr_id);
    if(skadr)
      {
        LPMDB_KADR_ENTRY beg = ps.p_entryes->entryes;
        LPMDB_KADR_ENTRY end = beg + ps.p_entryes->entryes_count;
        locker l(mut);
        if(ps.beg_data) skadr->entryes.clear();
        while(beg<end)
             {
              beg->state &= ~MDB_KADR_ENTRY_STATE_SELECTED;
              skadr->entryes.push_back(*beg);
              ++beg;
             }

        if(ps.end_data  )
        {
           kadr_scan_ready(skadr,true);
           if(skadr->user_data)
           {
            skadr->selections_restore(true);
            PostMessage(skadr->get_kadr_form()->Handle,WM_SHOW_REFRESH,0,0);
           }
        }
      }
 }
}
//-----------------------------------------------------------------------------
bool __fastcall check_image_timestamp(__int64 image_time,UnicodeString  image_name)
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

 UnicodeString   __fastcall TGkAwpModule::get_kadr_image_file(single_kadr * sk,bool no_request)
 {
     UnicodeString file_name;
     if(sk && sk->kadr.image_id != DWORD(-1))
     {
      get_string(sk->kadr.image_id,file_name);
      file_name = get_kadr_image_file(file_name);
      if(!no_request && (file_name.IsEmpty() || !check_image_timestamp(sk->kadr.image_time,file_name)))
          request_kadr_image(sk->kadr.kadr_id);
     }
    return file_name;
 }
//-----------------------------------------------------------------------------
 UnicodeString   __fastcall TGkAwpModule::make_module_file_name(UnicodeString fname,UnicodeString folder)
 {
   UnicodeString file_name;
   if(!fname.IsEmpty())
   {
     locker l(mut);
     file_name = params.data_folder;
     if(!file_name.IsEmpty()) file_name+=L'\\';
        if(!folder.IsEmpty())
        {
         file_name+=folder;
         file_name+=L'\\';
        }
    ForceDirs(file_name.c_str());
        file_name+=fname;
   }
   return file_name;

 }

//-----------------------------------------------------------------------------
bool __fastcall TGkAwpModule::file_image_begin(KeRTL::TFile & file,bool paint_image,LPOTDM_PROTO_IMAGE_BEGIN image_begin)
{
   file.Close();
   UnicodeString file_name(image_begin->image_name);
   set_string(image_begin->image_id,file_name);
   file_name = paint_image ? this->get_painter_image_file(file_name) : get_kadr_image_file(file_name);
   file.Open(file_name.c_str());
   if(file.IsValid())
   {
     file.SetSize(image_begin->image_size);
     LPFILETIME  file_time = (LPFILETIME)&image_begin->image_time;
     SetFileTime(file(),file_time,file_time,file_time);
     CloseHandle(file.Dup(false));
     file.SeekWr(0,FILE_BEGIN);
     file.SeekRd(0,FILE_BEGIN);
     return true;
   }
   return false;
}

void __fastcall TGkAwpModule::handle_kadr_image  (OTDM_PROTO_STATE & ps)
{
 locker l(mut);
 kadr_t * skadr = (kadr_t *)get_kadr(ps.p_image_chunk->kadr_id);
 if(skadr)
 {
   if(!ps.is_error)
   {
      KeRTL::TFile & file = skadr->get_file();
      if(ps.beg_data)
         {
          skadr->kadr.image_time = ps.p_image_begin->image_time;
          skadr->kadr.image_id   = ps.p_image_begin->image_id;
          file_image_begin(file,false,ps.p_image_begin);
         }
         else
         {
           file.Write(ps.p_image_chunk->chunk_data,ps.p_image_chunk->chunk_size);
         }

      if(ps.end_data)
        {
         file. Close();
         if(skadr->user_data)
         {
            TTlmKadrForm * kform = skadr->get_kadr_form();
            HWND kwnd = kform->Handle;
            if(IsWindow(kwnd))
               PostMessage(kwnd,WM_RESET_KADR,MDB_KADR_FIELD_IMAGE,0);
         }
        }
   }
   else
   {
    skadr->get_file().Close();
   }
 }

}
//-----------------------------------------------------------------------------
LRESULT __fastcall TGkAwpModule::handle_enum_kadrs(MODEM_ADDR & from,OTDM_PROTO_STATE & ps)
{
  //Обработка списка перечисденных кадров
  if(mod_params_wnd) SendMessage(mod_params_wnd->Handle,WNDCM_HANDLE_ENUM_KADRS,(WPARAM)&from,(LPARAM)&ps);
  return GKH_RET_SUCCESS;
}
//-----------------------------------------------------------------------------
void __fastcall TGkAwpModule::get_kadr_names(LPOTDM_PROTO_KADR pkadr,UnicodeString & short_name,UnicodeString & long_name)
{
  if(pkadr)
  {
    wchar_t * name_ptr = pkadr->names;
    if(pkadr->mask&MDB_KADR_FIELD_SNAME)
        {
         short_name = name_ptr;
         name_ptr+=wcslen(name_ptr)+1;
        }
    if(pkadr->mask&MDB_KADR_FIELD_NAME)
        {
         long_name = name_ptr;
         name_ptr+=wcslen(name_ptr)+1;
        }
  }
}
//-----------------------------------------------------------------------------
DWORD  __fastcall TGkAwpModule::get_record_ready_state(const mdb_record &rec,LPDWORD  ch_count)
{
   DWORD ready_state = 0;
   if(!rec.is_caution())
   {
        bool is_danger      =  rec.is_danger_state(); //  is_danger_state();
        bool is_alarm       =  rec.is_alarm_state ();
        DWORD changes_count = 0;
        if(rec.is_discrete())
          {
            changes_count = record_changes_count(&rec);
            bool is_normal = is_record_normal(&rec);
            bool is_static = is_record_static(&rec);

            bool is_on     = DWORD(rec.value) ? true : false;
                             ready_state |= is_on ? rds_notnormal_on : rds_notnormal_off;

              if(is_danger)  ready_state |= rds_notnormal_danger;
              if(is_alarm )  ready_state |= rds_notnormal_alarm;

              if(!is_static  && is_record_flashing(&rec))
                             ready_state |= rds_notnormal_flashing;

            if(!is_normal)
            {
              if(!rec.is_static() && changes_count> MDBR_CH_COUNT_MANY )
                    ready_state |= (ready_state<<16);//Устаноить много

            }
            else
            {
              if(changes_count)
              {
              ready_state <<=8;
              if(!rec.is_static() && changes_count>MDBR_CH_COUNT_MANY)
                   ready_state |= (ready_state<<8);
              }
              else
              ready_state &= rds_state_flashing;
            }
          }
          else
          {
            if(is_danger && !is_alarm)
               ready_state |= rds_analog_danger;
            if(is_alarm )
               ready_state |= rds_analog_alarm;
          }


         if(rec.is_tu_error()         ) ready_state |= rds_rc_error;

         if(is_record_rc_prepare(&rec))
            ready_state |= rds_rc_prepare;
         if(is_record_rc_acitive(&rec))
            ready_state |= rds_rc_active;
         if(ch_count) *ch_count = changes_count;
   }
    return ready_state;
}
//-----------------------------------------------------------------------------
DWORD  __fastcall TGkAwpModule::kadr_scan_ready(kadr_t * kadr,bool update)
{
   //Проход по всем записям кадра с установкой соответсвующих флагов готовности
   DWORD new_ready_state = kadr->ready_state & (rds_alarm_history|rds_stable_ready|rds_name_flashing);
   DWORD ch_mask         = 0;
   mdb_record            rec;
   locker l(mut);
   kadr_entryes_table::iterator ptr = kadr->entryes.begin();
   kadr_entryes_table::iterator end = kadr->entryes.end  ();
   kadr->new_changes_count = 0;
   while(ptr<end)
   {

   if(get_record(ptr->rec_id,rec) && is_record_worked(&rec))
        {
         DWORD  rec_ch_count = 0;
         new_ready_state |= get_record_ready_state(rec,&rec_ch_count);
         kadr->new_changes_count+= rec_ch_count ;
        }

    ++ptr;
   }
   if(kadr->old_changes_count!=kadr->new_changes_count)
     {
       if(kadr->new_changes_count)
          new_ready_state|= rds_name_flashing;
       kadr->changes = kadr->old_changes_count=kadr->new_changes_count;
     }

    if(!kadr->changes) new_ready_state &=~ rds_name_flashing;

   ch_mask  = kadr->ready_state ^ new_ready_state;
   if(update && ch_mask)
     kadr->ready_state = new_ready_state ; /*main_form->repaint_kadr(kadr);*/
   return ch_mask;
}
//-----------------------------------------------------------------------------

void    __fastcall TGkAwpModule::_kadr_set_bad_ready(kadr_t * kadr)
{
   locker l(mut);
   kadr->kadr.kadr_diag = MDBR_DIAG_BAD;
   kadr_changes[kadr->kadr.kadr_id] |=  MDB_KADR_FIELD_DIAG;

   kadr_entryes_table::iterator e_ptr =   kadr->entryes.begin();
   kadr_entryes_table::iterator e_end =   kadr->entryes.end  ();
   while (e_ptr<e_end)
   {
     records_t::iterator   rec_ptr;
     records_t::value_type rv(e_ptr->rec_id);
     if(stdlib_utils::binary_search(this->records,rv,mdb_record_id_less(),&rec_ptr))
        {
         if(rec_ptr->diag == MDBR_DIAG_GOOD)
            {
             rec_ptr->diag = MDBR_DIAG_BAD;
             record_changes[rv.rec_id]|=MDBR_FIELD_DIAG;
            }
        }
     ++e_ptr;
   }
}
//-----------------------------------------------------------------------------

void    __fastcall TGkAwpModule::kadrs_set_bad_ready()
{
  locker l(mut);
  kadrs_t::iterator kptr = kadrs.begin();
  kadrs_t::iterator kend = kadrs.end  ();
  while(kptr<kend)
   {
     _kadr_set_bad_ready(*kptr);
     ++kptr;
   }
}
//-----------------------------------------------------------------------------
struct kadrs_scan_ready_functor
{
   LPDWORD ready_state;
   kadrs_scan_ready_functor(DWORD * _ready_state):ready_state(_ready_state){*ready_state = 0;}
   kadrs_scan_ready_functor(const kadrs_scan_ready_functor &src):ready_state(src.ready_state){}
   void operator()(const kadr_t * kdr)
    {
     *ready_state |= kdr->ready_state;
    }
};

DWORD    __fastcall TGkAwpModule::kadrs_scan_ready()
{
  locker l(mut);
  for_each(kadrs.begin(),kadrs.end(),kadrs_scan_ready_functor(&ready_state));
  return ready_state;
}
//-----------------------------------------------------------------------------
void __fastcall inc_kadr_changes (record_t & rec,DWORD & changes)
{
        if(is_record_new_changes(&rec))
        {
          DWORD changes_count = record_changes_count(&rec);
          if(rec.is_discrete() )
            {
             if(!rec.is_static() && (changes_count<MDBR_CH_COUNT_MANY_IGNORE))
             ++changes;
            }
            else
            {
              if(rec.is_alarm_state() || rec.is_danger_state())
                ++changes;
            }
        }
}

void __fastcall TGkAwpModule::handle_record_depended(DWORD rec_id,DWORD mask)
{
  //Установка изменившихся кадров из-за записей
  if(mask)
   {
     locker l(mut);
     record_t rec(rec_id);
     records_t::iterator rec_ptr;
     if(search_record_t(rec_id,&rec_ptr))
     {
         rec = *rec_ptr;
         kadrs_t::iterator kptr = kadrs.begin();
         kadrs_t::iterator kend = kadrs.end();
         while(kptr<kend)
         {
          kadr_t * kadr = *kptr;
          kadr_entryes_table::iterator lo_entry,hi_entry;
          if(kadr->range_entry(rec_id,lo_entry,hi_entry))
             {
              if(rec.state & MDBR_STATE_TUTR_ACTIVE)
                 rec.state = rec.state;
              if((mask & MDBR_TUTR_STATE_CHANGED) && kadr->user_data)
                 {
                   while(lo_entry<hi_entry)
                   {
                    if(lo_entry->is_selected())
                       PostMessage(kadr->get_kadr_form()->Handle,WM_RC_STATE_CHANGE,rec_id,mask);
                    ++lo_entry;
                   }
                 }
             }
             kadr_changes[kadr->kadr.kadr_id]|= MDB_KADR_FIELD_CHANGES;
          ++kptr;
         }
         record_clear_new_attr(&rec);
     }
   }
}
//-----------------------------------------------------------------------------
void __fastcall TGkAwpModule::handle_record_changes()
{
  //Обработка изменений записей
  locker l(mut);
  if(record_changes.size())
  {
  changes_map_t::iterator ptr = record_changes.begin();
  changes_map_t::iterator end = record_changes.end  ();
  while(ptr!=end)
   {
     DWORD mask    = ptr->second;
     DWORD rec_id  = ptr->first;
     if(mask & (MDBR_FIELD_NAME|MDBR_FIELD_SNAME))
        request_record_names(rec_id,(mask&MDBR_FIELD_SNAME),(mask&MDBR_FIELD_NAME));
        handle_record_depended(rec_id,mask);
        show_record_info(rec_id,false);
     ++ptr;
   }
   record_changes.clear();
  }
}
//------------------------------------------------------------------------------
void __fastcall TGkAwpModule::handle_kadrs_changes()
{
      //Перенести в обработку handle_kadr_changes;
  locker l(mut);
  if(!kadr_changes.size()) return;
  changes_map_t::iterator ptr = kadr_changes.begin();
  changes_map_t::iterator end = kadr_changes.end  ();
  int refresh_count = 0;
  int sname_changed = 0;
  while(ptr!=end)
   {
    DWORD mask = ptr->second;
    kadr_t * kadr = (kadr_t*)get_kadr(ptr->first);
    if(kadr)
    {
      bool need_repaint = false;
      if(kadr_scan_ready(kadr,true))
      {
        mask |= MDB_KADR_CHANGE_READY;
      }

      if((mask & MDB_KADR_FIELD_DIAG) /*&& kadr->kadr.kadr_diag == MDBR_DIAG_GOOD */)
          kadr->stable_timer_set(params.ready_stable_timer*60*1000);
      if(mask & MDB_KADR_FIELD_SNAME)
          sname_changed++;
         else
            need_repaint = (mask & (MDB_KADR_CHANGE_READY|MDB_KADR_FIELD_CHANGES|MDB_KADR_FIELD_ENTRY_COUNT|MDB_KADR_FIELD_DIAG)) ? true:false;
      if(kadr->user_data)
          {
           if(mask&(MDB_KADR_FIELD_NAME|MDB_KADR_FIELD_FLAGS|MDB_KADR_FIELD_SIZE))
              PostMessage(kadr->get_kadr_form()->Handle,WM_RESET_KADR,mask,0);
              else
              {
               if(need_repaint) PostMessage(kadr->get_kadr_form()->Handle,WM_SHOW_REFRESH,0,0);
              }
          }
          refresh_count+=need_repaint;
    }
    ++ptr;
   }
   if(sname_changed)
     PostMessage(main_form->Handle,WM_DO_LAYOUT_READY,0,0);
     else
     {
     if(refresh_count)
        PostMessage(main_form->Handle,WM_SHOW_REFRESH,0,0);
     }
  kadr_changes.clear();
}
//------------------------------------------------------------------------------
void    __fastcall TGkAwpModule::do_timer_work          (int period)
{
   locker l(mut);
   bool  flash_switch = false;
   flash_timer -= period;
   if((flash_timer<=0))
   {
    flash_timer = params.flash_period;
    ++flash_counter ;
    flash_switch = true;
    if(0 == (flash_counter%10))
         history_check_present();
   }

   kadrs_t::iterator kptr = kadrs.begin(), kend = kadrs.end();
   while(kptr<kend)
   {
     kadr_t * kdr = *kptr;

     if(kdr->stable_timer_dec(period))
        kdr->ready_state |= rds_stable_ready;
     if(kdr->recall_timer_dec(period))
        kadr_order(kdr,true);

     if(flash_switch && kdr->user_data &&  ((kdr->ready_state&rds_state_flashing) || kdr->sel_count()) )
         PostMessage(kdr->get_kadr_form()->Handle,WM_FLASH_KADR,flash_counter,0);
     ++kptr;
   }
   if(flash_switch ) PostMessage(main_form->Handle,WM_FLASH_KADR,flash_counter,0);
}
//------------------------------------------------------------------------------

int     TGkAwpModule::timer_thread_proc      ()
{
   flash_timer          = params.flash_period;
   flash_counter        = 0;
   DWORD timer_period = 100;
   DWORD prev_tick = GetTickCount();
   DWORD new_tick  = prev_tick;
   while(!thread_term_request)
   {

     Sleep(timer_period - std::min(timer_period,new_tick-prev_tick));
     prev_tick = GetTickCount();

      if(!thread_term_request) do_timer_work(timer_period); else break;
      if(!thread_term_request) handle_record_changes();     else break;
      if(!thread_term_request) handle_kadrs_changes ();     else break;
      new_tick = GetTickCount();
   }
  return 0;
}
//------------------------------------------------------------------------------

void    __fastcall TGkAwpModule::_clear_record      (record_t & rec,bool single)
{
   DWORD mask = 0;

   bool is_flashing = is_record_flashing(&rec);

   if(rec.changes_count)
   {
      mask |= MDBR_FIELD_CH_COUNT;
      rec.changes_count = 0;
   }

   if(is_flashing )
   {
     rec.state &=~MDBR_STATE_FLASH;
     mask |= MDBR_FIELD_STATE;
   }
   else
   {
     if(single && !mask)
      rec.state |= MDBR_STATE_FLASH;
      mask |= MDBR_FIELD_STATE;

   }

   if(mask)
     record_changes[rec.rec_id] |= mask;

}
//------------------------------------------------------------------------------
void    __fastcall TGkAwpModule::_receipt_record    (record_t & rec,bool single)
{
  if(rec.is_discrete())
  {
   if(rec.changes_count || is_record_flashing(&rec)) _clear_record(rec,false);
      else
      {
       if(0==(rec.state & MDBR_STATE_NORECEIPT) )
         {
           if( !is_record_normal(&rec))
           {
                  if((DWORD)rec.value) rec.state |= MDBR_STATE_DYN_NORMAL;
                     else
                     rec.state &= ~MDBR_STATE_DYN_NORMAL;

           }
           else
           {
            //Расквитирование
            if(single )
              {
                rec.state = ((rec.state & MDBR_STATE_DYN_NORMAL) ? 0 : MDBR_STATE_DYN_NORMAL )|(rec.state & ~MDBR_STATE_DYN_NORMAL);
              }

           }
          record_changes[rec.rec_id] |= MDBR_FIELD_STATE;
          }
      }
  }
}
//------------------------------------------------------------------------------
void   __fastcall TGkAwpModule::clear_all       ()
{
 locker l(mut);
 //Снятие всех
 records_t::iterator rptr = records.begin();
 records_t::iterator rend = records.end  ();
 while(rptr<rend)
 {
      _clear_record(*rptr,false);
   ++rptr;
 }
 kadrs_reset_changes  ();
}
//------------------------------------------------------------------------------
void   __fastcall TGkAwpModule::clear_record    (DWORD rec_id,bool single)
{
  locker l(mut);
  records_t::iterator rptr;
  if(search_record_t(rec_id,&rptr))
     _clear_record(*rptr,single);
}
//------------------------------------------------------------------------------
void   __fastcall TGkAwpModule::receipt_all     ()
{
 locker l(mut);

 records_t::iterator rptr = records.begin();
 records_t::iterator rend = records.end  ();
 while(rptr<rend)
 {
      _receipt_record(*rptr,false);
   ++rptr;
 }
 kadrs_reset_changes  ();
}
//------------------------------------------------------------------------------
void   __fastcall TGkAwpModule::receipt_record  (DWORD rec_id,bool single)
{
  locker l(mut);
  records_t::iterator rptr;
  if(search_record_t(rec_id,&rptr))
     _receipt_record(*rptr,single);
}
//------------------------------------------------------------------------------
void    __fastcall TGkAwpModule::handle_db_change      ()
{
 clear_all_tables();
 paint_vars_requested = false;
 if(main_form)
 {
  PostMessage(main_form->Handle,WM_DO_LAYOUT_READY,0,0);
  PostMessage(main_form->Handle,WM_DO_SHOW_PARAMS ,0,0);
 }
}
//------------------------------------------------------------------------------

void    __fastcall TGkAwpModule::handle_tu_operation   (OTDM_PROTO_STATE & ps)
{
   locker l(mut);
   mdb_kadr_entry * entry = main_kadr.get_last_selected();
   if(entry)
    {
     kadr_t * kadr = (kadr_t *)get_kadr(entry->rec_id);
     if(kadr && kadr->user_data)
        PostMessage(kadr->get_kadr_form()->Handle,WM_RC_ENABLE,ps.p_tuop->rec_id,(!ps.is_error && ps.p_tuop->op_code == OTD_TUTR_CMDENABLE) ? true : false );
    }
}



