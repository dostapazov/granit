#pragma hdrstop
#include "mediumdb.hpp"
#include <otdm_proto.h>
#include <kfname.hpp>
#include <tlm_painter.h>

DWORD   __fastcall   med_db::handle_otdm_proto(LPMPROTO_HEADER mph)
{
  if(mph->fa == FA_OTD_MEDIUM)
  {
   LPOTDM_PROTO_HEADER omph = (LPOTDM_PROTO_HEADER)mph->data;
   if(!(omph->command&OTDM_RESPOND))
   {
   switch(omph->command&OTDM_COMMAND_MASK)
   {
     case  OTDMPROTO_CMD_ENUM_KADRS     :
      mproto_enum_kadrs(mph->addr_from,omph->data_size <sizeof(DWORD) ? MDB_KADR_FIELD_NAME|MDB_KADR_FIELD_SNAME: omph->data[0]);   break;
     case  OTDMPROTO_CMD_GETKADR        :
      mproto_get_kadr(mph->addr_from,omph->command,omph->data[0],omph->data[1]); break;
     case  OTDMPROTO_CMD_GETKADR_IMAGE  :
      mproto_get_kadr_image(mph->addr_from,omph->data[0]);break;
     case  OTDMPROTO_CMD_GETKADR_ENTRYES:
      mproto_get_kadr_entryes(mph->addr_from,omph->data[0]);break;
     case  OTDMPROTO_CMD_GETKADR_RECORDS:
      mproto_get_kadr_records(mph->addr_from,omph->data[0]);break;
     case OTDMPROTO_CMD_GET_RECORD_SNAME:
      mproto_get_record_name(omph->command&OTDM_COMMAND_MASK,mph->addr_from,omph->data[0],true ,false );break;
     case OTDMPROTO_CMD_GET_RECORD_NAME:
      mproto_get_record_name(omph->command&OTDM_COMMAND_MASK,mph->addr_from,omph->data[0],false,false );break;
     case OTDMPROTO_CMD_GET_RECORD_FULL_SNAME:
      mproto_get_record_name(omph->command&OTDM_COMMAND_MASK,mph->addr_from,omph->data[0],true ,true  );break;
     case OTDMPROTO_CMD_GET_RECORD_FULL_NAME:
      mproto_get_record_name(omph->command&OTDM_COMMAND_MASK,mph->addr_from,omph->data[0],false,true  );break;
     case OTDMPROTO_CMD_TUOPERATION:
          {
           LPOTDM_PROTO_TUOPERATION tuop =(LPOTDM_PROTO_TUOPERATION)omph->data;
           record_operation(&mph->addr_from,tuop);
          }
          break;
     case OTDMPROTO_CMD_START_OSCILLOGRAMM:
          {
           LPOTDM_PROTO_OSCPARAM osp = LPOTDM_PROTO_OSCPARAM(omph->data);
           record_request_oscillogramm(osp->rec_id,osp->osc_lenght);
          }
          break;
     case OTDMPROTO_CMD_GET_PAINT_VARIANTS:
          mproto_get_paint_variants(mph->addr_from,omph->command&OTDM_COMMAND_MASK,mph->data,mph->data_size);
     break;
     case OTDMPROTO_CMD_WORK_INOUT:
          record_work_inout((LPOTDM_PROTO_WORK_INOUT)omph->data);
     break;
     case OTDMPROTO_CMD_CLEAR_TU_ERROR:
          record_clear_tu_error(omph->data[0]);
          break;
    }
   }
  }

  return GKH_RET_SUCCESS;
}


DWORD      __fastcall med_db::mproto_send(order_vector::iterator modem_low,
                                          order_vector::iterator modem_hi,
                                          DWORD mask,DWORD cmd,DWORD err,LPVOID data,
                                          DWORD data_size,bool begin,bool finish )
{
 DWORD ret = 0;
 while(modem_low<modem_hi)
 {
  if((modem_low->order_mask&mask) && mproto_send(modem_low->dest_addr,cmd,err,data,data_size,begin,finish))
      ++ret;
  ++modem_low;
 }
 return ret;
}

DWORD       __fastcall med_db::mproto_send(MODEM_ADDR & to,DWORD cmd,DWORD err,LPVOID data,DWORD data_size,bool begin,bool finish )
{
 return module->mproto_send(FA_OTD_MEDIUM,to,cmd,err,data,data_size,begin,finish);
}

DWORD      __fastcall med_db::mproto_enum_kadrs(MODEM_ADDR & from,DWORD mask)
{
  DWORD ret = GKH_RET_SUCCESS ;
     #if( FASTMUTEX_LOCKER != 0)
      TLockHelper l (locker);
     #else
       rd_lock    l (locker);
     #endif
  MDB_KADR kadr;
  DWORD index = 0;
  while(kadrs.enum_kadr(index,kadr)!=(DWORD)-1)
  {
   bool finish = kadrs.size()-index == 1 ? true:false;
   mproto_get_kadr(from,OTDMPROTO_CMD_ENUM_KADRS,kadr.kadr_id,mask,index == 0 ? true:false,finish);
   ++index;
  }
 //unlock();
 return ret;
}

 DWORD      __fastcall med_db::mproto_send_kadr_removed  (DWORD kadr_id)
 {
     #if( FASTMUTEX_LOCKER != 0)
      TLockHelper l (locker);
     #else
       wr_lock    l (locker);
     #endif
    order_vector::iterator  lo, hi;
    if(orders.get_modem_range(kadr_id,lo,hi))
    {
        OTDM_PROTO_KADR  proto_kadr;
        bzero(&proto_kadr,sizeof(proto_kadr));
        proto_kadr.mask = MDB_KADR_REMOVED;
        proto_kadr.kadr.kadr_id = kadr_id;
        mproto_send(lo,hi,-1,OTDMPROTO_CMD_GETKADR,0,&proto_kadr,sizeof(proto_kadr),true,true);
        //TODO Убрать подписку на кадр
    }
   return GKH_RET_SUCCESS;
 }

 DWORD      __fastcall med_db::mproto_get_kadr           (MODEM_ADDR & to,DWORD cmd,DWORD kadr_id,DWORD mask,bool begin,bool finish)
 {
     #if( FASTMUTEX_LOCKER != 0)
      TLockHelper l (locker);
     #else
       wr_lock    l (locker);
     #endif
   if(kadr_id == (DWORD)-1  )
   {

    if(mask == 0)
    {

     orders.remove_order(to,kadr_id); // Отписка от всех кадров
     wchar_t rep_text[MAX_PATH];
     wsprintfW(rep_text,L"Отказ от подписки на все кадры. Modem %03d.%03d",(DWORD)to.modem,(DWORD)to.line);
     module->mdb_report(REPORT_INFORMATION_TYPE,rep_text,0,0);
    }
    else
     {

      orders.add_order(to,kadr_id,mask);//Подписка на все записи всех кадров
      kadrs_list::iterator i = kadrs.begin_kadr(),e = kadrs.end_kadr();
      DWORD _cmd = cmd&(~OTDM_ORDER);
      begin = true;
      while(i<e)
      {
        mproto_get_kadr(to,_cmd,i->kadr_id,mask,begin,finish);
        begin = false;
        ++i;
      }
     }
   }
   else
   {
      kadrs_list::iterator kadr =  kadrs.find_kadr(kadr_id);
      if(kadrs.is_valid_kadr_ptr(kadr))
      {
       wchar_t rep_text[MAX_PATH];
       int rep_len = 0;
       if(mask)
       {
          DWORD sname_len;
          DWORD name_len ;
          BYTE buffer[8192];
          LPOTDM_PROTO_KADR  proto_kadr = (LPOTDM_PROTO_KADR)buffer;
          proto_kadr->mask = mask&~(MDB_KADR_FIELD_ENTRYES/*|MDB_KADR_FIELD_IMAGE*/);
          memcpy(&proto_kadr->kadr,&*kadr,sizeof(proto_kadr->kadr));
          proto_kadr->names[0] = 0;
          DWORD need_size = sizeof(*proto_kadr);
          wchar_t * text = proto_kadr->names;
          if(mask&MDB_KADR_FIELD_SNAME)
            {
             text[txt_table.get_text(kadr->sname_id,text,(sizeof(buffer)-need_size)/sizeof(wchar_t))]=0;
             sname_len = lstrlenW(text);
             need_size+=sname_len<<1;
             text+=sname_len;
             *text = 0;
             ++text;
             need_size+=2;
            }

            if(mask&MDB_KADR_FIELD_NAME)
            {
             text[txt_table.get_text(kadr->name_id,text,(sizeof(buffer)-need_size)/sizeof(wchar_t))]=0;
             name_len = lstrlenW(text) ;
             need_size+=name_len<<1;
             text+=name_len;
             *text = 0;
             ++text;
             need_size+=2;
             }
          DWORD send_result =  mproto_send(to,cmd&OTDM_COMMAND_MASK,0,buffer,need_size,false,finish);
          if(send_result == GKH_RET_SUCCESS)
          {
          if(mask&MDB_KADR_FIELD_ENTRYES)
            {
             mproto_get_kadr_entryes(to,kadr_id);
             mproto_get_kadr_records(to,kadr_id);
            }

           if(cmd&OTDM_ORDER)
           {
                 if(orders.add_order(to,kadr_id,mask))
                    rep_len = wsprintfW(rep_text,L"Оформление подписки на кадр %d, Modem %03d.%03d",kadr_id,(DWORD)to.modem,(DWORD)to.line);
           }
          }
          else
          {
            orders.remove_order(to,-1);
            rep_len = wsprintfW(rep_text,L"Ошибка передачи. удаление подписок  modem %03d.%03d",(DWORD)to.modem,(DWORD)to.line);
          }
       }
       else
       {
        if(orders.remove_order(to,kadr->kadr_id))
        rep_len = wsprintfW(rep_text,L"Отказ от подписки на кадр %d, Modem %03d.%03d",kadr_id,(DWORD)to.modem,(DWORD)to.line);
       }
       if(rep_len)
          module->mdb_report(REPORT_INFORMATION_TYPE,rep_text,0,0);
      }
      else
      {
       mproto_send(to,cmd,OTDMPROTO_ERR_NO_KADR,&kadr_id,sizeof(kadr_id),begin,finish);
      }
    }
  //unlock();
  return GKH_RET_SUCCESS;
 }

 DWORD      __fastcall med_db::remove_medium_order(modem_addr & ma,DWORD kadr_id)
 {
  DWORD ret;
     #if( FASTMUTEX_LOCKER != 0)
      TLockHelper l (locker);
     #else
       wr_lock    l (locker);
     #endif
  ret = orders.remove_order(ma,kadr_id);
  //unlock();
  return ret;
 }

 DWORD      __fastcall med_db::mproto_get_image     (DWORD cmd,MODEM_ADDR & to,DWORD image_id,__int64 image_time,DWORD kadr_id)
 {
     DWORD ret = 0;
     DWORD err = 0;
     BYTE  buffer[OTDM_DEF_CHUNK_SIZE];
     wchar_t file_name[MAX_PATH<<2];
     wchar_t name[MAX_PATH];
     wchar_t ext [MAX_PATH];
     DWORD file_name_len;
     {
     #if( FASTMUTEX_LOCKER != 0)
      TLockHelper l (locker);
     #else
       rd_lock    l (locker);
     #endif

     file_name_len = txt_table.get_text(image_id,file_name,sizeof(buffer)/sizeof(wchar_t));
     }


     bool is_resource = wcsstr(file_name,RESOURCE_BITMAP) ? true:false;
     if(!is_resource)
     {
      file_name[file_name_len] = 0;
      _wsplitpath(file_name,0,0,name,ext);
     }
     else
     {
     wcscpy(name,file_name);
     *ext = 0;
     }
     LPOTDM_PROTO_IMAGE_BEGIN image_beg = (LPOTDM_PROTO_IMAGE_BEGIN)  buffer;
     image_beg->kadr_id    = kadr_id;
     image_beg->image_id   = image_id;
     image_beg->image_time = image_time;
     DWORD len = sizeof(*image_beg)-sizeof(image_beg->image_name[0]);
     len+= wsprintfW(image_beg->image_name,L"%s%s",name,ext)<<1;
     len+=2;

     if(is_file_exist(file_name))
     {
      TFileStream  fs(file_name,true);
      if(fs.IsGood())
      {
       image_beg->image_size = fs.GetSize();
       mproto_send(to,cmd,0,buffer,len,true,false);
       bool finish = false;
       LPOTDM_PROTO_IMAGE_CHUNK image_chunk = (LPOTDM_PROTO_IMAGE_CHUNK)  buffer;
       image_chunk->kadr_id   = kadr_id;
       image_chunk->image_id  = image_id;
       DWORD rd_size = sizeof(buffer)-sizeof(*image_chunk)+sizeof(image_chunk->chunk_data[0]);
       while(!finish && ret == GKH_RET_SUCCESS)
       {
        len = image_chunk->chunk_size = fs.Read(image_chunk->chunk_data,rd_size);
        len+= sizeof(*image_chunk)-sizeof(image_chunk->chunk_data[0]);
        finish = image_chunk->chunk_size!=(WORD)rd_size;
        ret = mproto_send(to,cmd,0,buffer,len,false,finish);
       }
      }
      else
      err = OTDMPROTO_ERR_ERROR_OPEN_FILE;
     }
     else
     err = OTDMPROTO_ERR_NO_IMAGE;

  if(err)
     {
       LPOTDM_PROTO_IMAGE_BEGIN image_beg = (LPOTDM_PROTO_IMAGE_BEGIN)  buffer;
       image_beg->kadr_id    = kadr_id;
       image_beg->image_id   = image_id;
       image_beg->image_size = 0;
       mproto_send(to,cmd,err && is_resource ? 0 : err,image_beg,len,true,true);
     }

  return ret;
 }

 DWORD      __fastcall med_db::mproto_get_kadr_image(MODEM_ADDR & to,DWORD kadr_id)
 {
   DWORD ret = 0;
   DWORD err = 0;
   kadrs_list::iterator kadr;
     #if( FASTMUTEX_LOCKER != 0)
      TLockHelper l (locker);
     #else
       rd_lock    l (locker);
     #endif
   kadr = kadrs.find_kadr(kadr_id);
   if(kadrs.is_valid_kadr_ptr(kadr))
   {
    if(kadr->image_id!=(DWORD)-1)
    {
     ret = mproto_get_image(OTDMPROTO_CMD_GETKADR_IMAGE,to,kadr->image_id,kadr->image_time,kadr_id);
    }
    else
     err = OTDMPROTO_ERR_NO_IMAGE;
   }
   else
   err = OTDMPROTO_ERR_NO_KADR;
   //unlock();
   if(err)
     mproto_send(to,OTDMPROTO_CMD_GETKADR_IMAGE,err,&kadr_id,sizeof(kadr_id),true,true);
   return ret;
 }

/*
     DWORD      __fastcall med_db::mproto_get_kadr_entryes(MODEM_ADDR & to,DWORD kadr_id)
     {
      //TODO Переделать отправку элементов кадра с ограничением по размеру буфера
      DWORD err = 0;
      TLockHelper l(locker);//lock();
      kadrs_list::iterator kadr = kadrs.find_kadr(kadr_id);
      if(kadrs.is_valid_kadr_ptr(kadr))
      {
       kadr_entryes_table::iterator lo_entry,hi_entry;
       DWORD entryes_count = kadrs.get_entry_range(kadr_id,lo_entry,hi_entry);
       LPOTDM_PROTO_KADR_ENTRYES  entryes;
       DWORD need_size = sizeof(*entryes)-sizeof(entryes->entryes[0])+sizeof(entryes->entryes[0])*entryes_count;
       entryes =(LPOTDM_PROTO_KADR_ENTRYES) new BYTE[need_size];
       //DWORD check_size = sizeof(*entryes)-sizeof(entryes->entryes[0]);
       if(entryes)
       {
        entryes->kadr_id       = kadr_id;
        entryes->entryes_count = entryes_count;
        LPMDB_KADR_ENTRY dest = entryes->entryes;
        while(lo_entry<hi_entry)
        {
         memcpy(dest,&*lo_entry,sizeof(*dest));
         ++dest;
         ++lo_entry;
         //check_size+=sizeof(*dest);
        }
        //check_size = check_size;
        mproto_send(to,OTDMPROTO_CMD_GETKADR_ENTRYES,err,entryes,need_size,true,true);
        delete [] (LPBYTE) entryes;
       }
       else
       err = OTDMPROTO_ERR_NO_MEMORY;
      }
      else
      err = OTDMPROTO_ERR_NO_KADR;
      //unlock();
       if(err)
         mproto_send(to,OTDMPROTO_CMD_GETKADR_ENTRYES,err,&kadr_id,sizeof(kadr_id),true,true);
      return GKH_RET_SUCCESS;
     }

*/

 DWORD      __fastcall med_db::mproto_get_kadr_entryes(MODEM_ADDR & to,DWORD kadr_id)
 {
  //TODO Переделать отправку элементов кадра с ограничением по размеру буфера
  DWORD err = 0;
     #if( FASTMUTEX_LOCKER != 0)
      TLockHelper l (locker);
     #else
       rd_lock    l (locker);
     #endif
  kadrs_list::iterator kadr = kadrs.find_kadr(kadr_id);
  if(kadrs.is_valid_kadr_ptr(kadr))
  {
   kadr_entryes_table::iterator lo_entry,hi_entry;
   //DWORD entryes_count =
   kadrs.get_entry_range(kadr_id,lo_entry,hi_entry);
   LPOTDM_PROTO_KADR_ENTRYES  entryes;

   BYTE buffer[sizeof(*entryes)+sizeof(entryes->entryes[0])*127];
   entryes =(LPOTDM_PROTO_KADR_ENTRYES) buffer;
   DWORD len              = sizeof(*entryes)-sizeof(entryes->entryes[0]);

    bool beg_respond       = true;
    entryes->kadr_id       = kadr_id;
    entryes->entryes_count = 0;
    LPMDB_KADR_ENTRY dest = entryes->entryes;

    while(lo_entry<hi_entry)
    {
     if(len<2048)
     {
      memcpy(dest,&*lo_entry,sizeof(*dest));
      ++dest;
      ++lo_entry;
      ++entryes->entryes_count;
      len+=sizeof(*dest);
     }
     else
     {
       mproto_send(to,OTDMPROTO_CMD_GETKADR_ENTRYES,err,entryes,len,beg_respond,false);
       entryes->entryes_count = 0;
       dest                   = entryes->entryes;
       beg_respond            = false;
       len                    = sizeof(*entryes)-sizeof(entryes->entryes[0]);
     }
    }
    mproto_send(to,OTDMPROTO_CMD_GETKADR_ENTRYES,err,entryes,len,beg_respond,true);

  }
  else
  err = OTDMPROTO_ERR_NO_KADR;
  //unlock();
   if(err)
     mproto_send(to,OTDMPROTO_CMD_GETKADR_ENTRYES,err,&kadr_id,sizeof(kadr_id),true,true);
  return GKH_RET_SUCCESS;
 }

 DWORD      __fastcall med_db::mproto_get_kadr_records(MODEM_ADDR & to,DWORD kadr_id)
 {

  DWORD err = 0;
  sended_recs sr;
     #if( FASTMUTEX_LOCKER != 0)
      TLockHelper l (locker);
     #else
       rd_lock    l (locker);
     #endif
  kadrs_list::iterator kadr = kadrs.find_kadr(kadr_id);
  if(kadrs.is_valid_kadr_ptr(kadr))
  {
   kadr_entryes_table::iterator lo_entry,hi_entry;
   kadrs.get_entry_range(kadr_id,lo_entry,hi_entry);
   bool beg_respond = true;
   bool end_respond = false;
   LPOTDM_PROTO_KADR_RECORDS records;
   BYTE  buffer [sizeof(*records)+127*sizeof(records->records[0])];
   records = (LPOTDM_PROTO_KADR_RECORDS) buffer;
   records->kadr_id = kadr->kadr_id;
   records->rec_count = 0;
   DWORD len = sizeof(*records)-sizeof(records->records[0]);
   while(lo_entry<hi_entry)
   {
     if(len<2048)
      {
       if(check_not_send_record(&sr,lo_entry->rec_id))
       {
        mdb_record & rec = this->records.get_record(lo_entry->rec_id);
        records->records[records->rec_count] = rec;
        ++records->rec_count;
        len+=sizeof(MDB_RECORD);
       }
      ++lo_entry;
      }
      else
      {
       mproto_send(to,OTDMPROTO_CMD_GETKADR_RECORDS,err,records,len,beg_respond,end_respond);
       beg_respond = false;
       records->rec_count = 0;
       len = sizeof(*records)-sizeof(records->records[0]);
      }
   };
   if(!end_respond)
     mproto_send(to,OTDMPROTO_CMD_GETKADR_RECORDS,err,records,len,beg_respond,true);

  }
  else
  err = OTDMPROTO_ERR_NO_KADR;
  //unlock();
   if(err)
     mproto_send(to,OTDMPROTO_CMD_GETKADR_RECORDS,err,&kadr_id,sizeof(kadr_id),true,true);
  return GKH_RET_SUCCESS;
 }


// TODO дописать обработку  изменения имён (MDBR_FIELD_NAME|MDBR_FIELD_SNAME)

bool __fastcall med_db::check_not_send_record(sended_recs * sr,DWORD rec_id)
{
 /* Проверка того что запись не отправлена */
 if(sr)
 {
 sended_recs::iterator ptr = sr->begin(),end = sr->end();
 ptr = lower_bound(ptr,end,rec_id);
 if(ptr<end && *ptr == rec_id)
    {
     return false;
    }
  sr->insert(ptr,rec_id);
 }
 return true;

}

#pragma warn -8057
DWORD      __fastcall med_db::mproto_send_kadr_changes(DWORD kid,DWORD mask,
                                                       order_vector::iterator modem_low,
                                                       order_vector::iterator modem_hi,
                                                       sended_recs * sr)
{
 DWORD ret = GKH_RET_SUCCESS;
 BYTE buffer[2048+sizeof(MDB_RECORD)];
 DWORD len = 0;
 kadrs_list::iterator kadr  = this->kadrs.find_kadr(kid);

 if(kadrs.is_valid_kadr_ptr(kadr))
  {
    if(mask & MDB_KADR_FIELD_VALUES)
    {
     kadr_entryes_table::iterator ke_low,ke_hi; //взяли содержимое кадра
     DWORD count = kadrs.get_entry_range(kid,ke_low,ke_hi);
     if(count)
     {
      LPOTDM_PROTO_RECORDS  recs = (LPOTDM_PROTO_RECORDS) buffer;
      LPBYTE sr_ptr  = (LPBYTE) recs->records;
      recs->rec_count = 0;
      __int64 save_time = 0;
      while(ke_low<ke_hi)
      {
       if(record_changes.count(ke_low->rec_id))
        {
          DWORD rec_id   = ke_low->rec_id;
          DWORD rch_mask = record_changes[rec_id]|mask;
          if((rch_mask&(MDBR_VALUE_FIELDS)) && check_not_send_record(sr,rec_id))
          {
           //проверка на заполнения буфера
           //и отправку кадра
           //len>=sizeof(buffer)-sizeof(MDB_RECORD)

           if(len>= (sizeof(buffer)-sizeof(MDB_RECORD)-sizeof(*recs)))
           {
             len+=sizeof(recs->rec_count);
             mproto_send(modem_low,modem_hi,rch_mask,OTDMPROTO_CMD_RECORDS|OTDM_ORDER,0,buffer,len,true,true);
             len = 0;
             recs->rec_count = 0;
             sr_ptr  = (LPBYTE) recs->records;
             save_time = 0;
           }

           mdb_record & src_record = records.get_record(ke_low->rec_id);
//           mdb_record  src_record = records.get_record(ke_low->rec_id);
           LPOTDM_PROTO_RECORD proto_rec = (LPOTDM_PROTO_RECORD)sr_ptr;
           if((rch_mask & MDBR_FIELD_DIAG) && src_record.diag == MDBR_DIAG_GOOD )
              rch_mask|= MDBR_FIELD_VALUE|MDBR_FIELD_TIME;//При восстановлении готовости передаём и состояние
           proto_rec->mask = rch_mask & (MDBR_VALUE_FIELDS);
           sr_ptr+=sizeof(proto_rec->mask);
           len   +=sizeof(proto_rec->mask);
           //TODO Убрать из рассылки значения записей
           //если изменились имена
           if((proto_rec->mask&MDBR_FIELD_TIME) && save_time != src_record.time)
               save_time = src_record.time;
               else
               proto_rec->mask&=~MDBR_FIELD_TIME;

           if(proto_rec->mask & (MDBR_FIELD_VALUE|MDBR_FIELD_OTDVALUE|MDBR_FIELD_DIAG|MDBR_FIELD_STATE) ) //Завсегда добавляем значение
              proto_rec->mask|=(MDBR_FIELD_OTDVALUE|MDBR_FIELD_VALUE|MDBR_FIELD_OPTIONS);

           DWORD rec_len = otdm_pack_record(sr_ptr,sizeof(buffer)-len,proto_rec->mask,&src_record);
           sr_ptr+=rec_len;
           len+=rec_len;
           ++recs->rec_count;
           if(rch_mask &(MDBR_FIELD_NAME|MDBR_FIELD_SNAME))
             {
              mproto_send_name_changes(ke_low->rec_id,(rch_mask &(MDBR_FIELD_NAME|MDBR_FIELD_SNAME)),modem_low,modem_hi);
             }
          }
        }
        ++ke_low;
      }

      if(len)
      {
       len+=sizeof(recs->rec_count);
       mproto_send(modem_low,modem_hi,MDB_KADR_FIELD_VALUES,OTDMPROTO_CMD_RECORDS|OTDM_ORDER,0,buffer,len,true,true);
      }
     }
    }

    if(mask & (MDB_KADR_FIELD_ENTRYES|MDB_KADR_FIELD_MASK))
    {
     //Отправка изменений содержимого кадра

     order_vector::iterator m_low = modem_low,m_hi = modem_hi;
     while(m_low<m_hi)
      {
       DWORD _mask = mask;
       if(_mask & MDB_KADR_FIELD_ENTRYES)
       {
        mproto_get_kadr_records(m_low->dest_addr,kid);
        mproto_get_kadr_entryes(m_low->dest_addr,kid);
        _mask &=~(MDB_KADR_FIELD_VALUES|MDB_KADR_FIELD_ENTRYES);
       }

       if(_mask & MDB_KADR_FIELD_MASK)
         mproto_get_kadr(m_low->dest_addr,OTDMPROTO_CMD_GETKADR|OTDM_RESPOND,kid,_mask&MDB_KADR_FIELD_MASK,false);

       if(_mask & MDB_KADR_FIELD_IMAGE)
       {
        mproto_get_kadr_image(m_low->dest_addr,kid);
       }
      ++m_low;
      }
    }
  }
return ret;
}
#pragma warn .8057

DWORD      __fastcall med_db::mproto_get_record_name(DWORD cmd,MODEM_ADDR & to,DWORD rec_id,bool short_name,bool full_name)
{
 BYTE   buffer[4096];
 LPOTDM_PROTO_RECORD_NAME  rec_name = (LPOTDM_PROTO_RECORD_NAME)buffer;
 BYTE   buffer1[4096];
 LPMDB_RECORD_TEXT rt = (LPMDB_RECORD_TEXT) buffer1;
 rt->dw_size  = sizeof(rt);
 rt->text_len = 0;
 rt->text_size = (sizeof(buffer1)-sizeof(*rt)+sizeof(rt->text[0]))/sizeof(wchar_t);
 rt->text[0] = 0;
 DWORD send_sz   ;
 DWORD res   = full_name ? get_full_name(rec_id,rt,!short_name) : get_name(rec_id,rt,!short_name);
 if(res == GKH_RET_SUCCESS)
 {
   rec_name->rec_id = rec_id;
   mdb_record rec;
   get_record(rec_id,&rec);
   rec_name->name_id   = short_name ? rec.sname:rec.name;
    if(rt->text_len )
    {
     
     rec_name->len       = KeRTL::MIN((DWORD)(sizeof(buffer)-sizeof(*rec_name)+sizeof(rec_name->text[0]))/sizeof(wchar_t), rt->text_len);
     safe_strcpyn(rec_name->text,rt->text,rec_name->len);
     rec_name->text[rec_name->len]=0;
     send_sz = rec_name->len*sizeof(wchar_t)+sizeof(*rec_name)/*-sizeof(rec_name->text[0])*/;
     mproto_send(to,cmd,0,buffer,send_sz,true,true);
    }
 }
 else
 {
  mproto_send(to,cmd,OTDMPROTO_ERR_NORECORD,&rec_id,sizeof(DWORD),true,true);
 }

 return 0;
}

DWORD __fastcall med_db::mproto_send_name_changes(DWORD rec_id,DWORD mask,
                                          order_vector::iterator modem_low,
                                          order_vector::iterator modem_hi)
{
 DWORD ret = 0;
  if(mask&(MDBR_FIELD_NAME|MDBR_FIELD_SNAME))
  {
   while(modem_low<modem_hi)
   {
    if(mask&MDBR_FIELD_NAME)
      mproto_get_record_name(OTDMPROTO_CMD_GET_RECORD_NAME,modem_low->dest_addr,rec_id,false,true);
    if(mask&MDBR_FIELD_SNAME)
      mproto_get_record_name(OTDMPROTO_CMD_GET_RECORD_SNAME,modem_low->dest_addr,rec_id,true,false);
    ++modem_low;
    ++ret;
   }
  }

 return ret;
}

/*
 В запросе передаётся перечень
*/
DWORD      __fastcall med_db::mproto_get_paint_variants(MODEM_ADDR & to,DWORD cmd,LPVOID data,DWORD /*data_size*/)
{
  DWORD ret = GKH_RET_SUCCESS;
  LPOTDM_PAINT_VARIANTS request = (LPOTDM_PAINT_VARIANTS)data;
  LPMDB_PAINT_VARIANT   pv_beg  = request->pvars;
  LPMDB_PAINT_VARIANT   pv_end  = pv_beg;
                        pv_end+=request->count;
  DWORD last_pv_idx = -1;

  BYTE buffer[4096];

  LPOTDM_PAINT_VARIANTS resp  = (LPOTDM_PAINT_VARIANTS)buffer;
  LPMDB_PAINT_VARIANT   resp_pv = resp->pvars;
  resp->count = 0;
  DWORD resp_size = sizeof(resp->count);
  bool respond_begin = true;

  TLockHelper l(locker);
  Tmdb_paint_variants::iterator ptr;
  if(last_pv_idx == (DWORD)-1)
    ptr = paint_vars.begin();
    else
    paint_vars_search(pv_beg->idx,&ptr);

  while(ret == GKH_RET_SUCCESS && ptr<paint_vars.end())
  {
   //ptr->name_id
   if(ptr->image_id!=(DWORD)-1)
      mproto_get_image(OTDMPROTO_CMD_PAINT_IMAGE,to,ptr->image_id,-1);
   if(ptr->mask_id!=(DWORD)-1)
      mproto_get_image(OTDMPROTO_CMD_PAINT_IMAGE,to,ptr->mask_id ,-1);

   memcpy(resp_pv,&(*ptr),sizeof(*resp_pv));
   resp_size+=sizeof(*resp_pv);
   ++resp_pv;
   ++resp->count;
   ++ptr;
   if((resp_size+sizeof(*resp_pv))>sizeof(buffer))
      {
       ret = mproto_send(to,cmd|OTDM_RESPOND,0,resp,resp_size,respond_begin,false);
       respond_begin = false;
       resp_size = sizeof(resp->count);
       resp->count = 0;
       resp_pv = resp->pvars;
      }
  }

  ret = mproto_send(to,cmd|OTDM_RESPOND,0,resp,resp_size,respond_begin,true);

  return ret;
}

 void       __fastcall med_db::send_database_reopen  ()
 {

     #if( FASTMUTEX_LOCKER != 0)
      TLockHelper l (locker);
     #else
       rd_lock    l (locker);
     #endif
  while(orders.size())
  {
   MODEM_ADDR ma;
   if(orders.get_modem_addr(0,ma))
   {
      mproto_send(ma,OTDMPROTO_CMD_NOTIFY_DBCHANGE/*|OTDM_RESPOND*/,0,NULL,0,true,true);
      orders.remove_order(ma,-1);//Удалить подписку
   }
   else
   {
     DebugBreak();
     break;
   }
  }
 }




