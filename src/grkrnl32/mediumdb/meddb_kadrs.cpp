#pragma hdrstop
#include "mediumdb.hpp"

 DWORD      __fastcall med_db::kadr_enum(DWORD idx,LPMDB_KADR  kdr)
 {
  DWORD ret = GKH_RET_ERROR;
     #if( FASTMUTEX_LOCKER != 0)
      TLockHelper l (locker);
     #else
       rd_lock    l (locker);
     #endif
  mdb_kadr _kadr;
  ret = kadrs.enum_kadr(idx,_kadr);
  if(kdr)
     memcpy(kdr,&_kadr,sizeof(*kdr));
  //unlock();
  return ret;
 }

 DWORD      __fastcall med_db::kadr_get (DWORD kadr_id,LPMDB_KADR  kdr)
 {
  DWORD ret = GKH_RET_ERROR;
     #if( FASTMUTEX_LOCKER != 0)
      TLockHelper l (locker);
     #else
       rd_lock    l (locker);
     #endif
  kadrs_list::iterator _kadr = kadrs.find_kadr(kadr_id);
  if(kadrs.is_valid_kadr_ptr(_kadr))
   {
    ret = GKH_RET_SUCCESS;
    if(kdr)
      memcpy(kdr,&*_kadr,sizeof(*kdr));
   }

  //unlock();
  return ret;
 }

 DWORD      __fastcall med_db::kadr_create(LPMDB_KADR kdr)
 {
  DWORD ret = GKH_RET_ERROR;
     #if( FASTMUTEX_LOCKER != 0)
      TLockHelper l (locker);
     #else
       wr_lock    l (locker);
     #endif
  mdb_kadr _kadr;
  if(kdr)
  _kadr = *kdr;
  _kadr.image_time = _kadr.content_time = GetTime();
  kadrs.create_kadr(_kadr);
  ret = _kadr.kadr_id;
  if(kdr)
     kdr->kadr_id = _kadr.kadr_id;
   kadr_changes[kdr->kadr_id] = MDB_KADR_FIELD_MASK;
   handle_kadr_changes();
  //unlock();
  module->notify(MDB_NOTIFY_MASK_KADR,MDB_NOTIFY_CODE_KADR_CREATED,&ret,sizeof(ret));
  module->set_ccflags(MDB_CCFL_KADRS_TABLE,true);
  return ret;
 }

 __int64 __fastcall med_db::get_kadr_image_time(DWORD kadr_id)
 {
  __int64 ret ;
     #if( FASTMUTEX_LOCKER != 0)
      TLockHelper l (locker);
     #else
       rd_lock    l (locker);
     #endif
  kadrs_list::iterator _kadr = kadrs.find_kadr(kadr_id);
  ret = get_kadr_image_time(_kadr);
  //unlock();
  return ret;
 }


 __int64    __fastcall med_db::get_kadr_image_time(kadrs_list::iterator kadr)
 {
  return get_file_time(kadr->image_id);
 }

 __int64 __fastcall med_db::get_file_time(wchar_t * file_name)
 {
  __int64 ret;
  KeRTL::TFile file(file_name,OPEN_EXISTING);
  if(file.IsValid())
     GetFileTime(file(),NULL,NULL,(LPFILETIME)&ret);
     else
     ret = 0;
  file.Close();
  //unlock();
  return ret;

 }

 __int64 __fastcall med_db::get_file_time(DWORD _id)
 {
  wchar_t text[MAX_PATH<<2];
     #if( FASTMUTEX_LOCKER != 0)
      TLockHelper l (locker);
     #else
       rd_lock    l (locker);
     #endif
  txt_table.get_text(_id,text,sizeof(text)/sizeof(wchar_t));
  return get_file_time(text);
 }



 DWORD      __fastcall med_db::kadr_update (LPMDB_KADR kadr,DWORD mask)
 {
  DWORD ret = GKH_RET_ERROR;
  if(kadr && mask)
  {
     #if( FASTMUTEX_LOCKER != 0)
      TLockHelper l (locker);
     #else
       wr_lock    l (locker);
     #endif
  kadrs_list::iterator _kadr = kadrs.find_kadr(kadr->kadr_id);
  if(kadrs.is_valid_kadr_ptr(_kadr))
  {
    ret = GKH_RET_SUCCESS;
    DWORD changes      = mask;
   _kadr->name_id      = kadr->name_id;
   _kadr->image_id     = kadr->image_id;
   _kadr->sname_id     = kadr->sname_id;
   _kadr->kadr_size    = kadr->kadr_size;
   _kadr->kadr_flags   = kadr->kadr_flags;
   if(mask&MDB_KADR_FIELD_ENTRY_COUNT)
   _kadr->content_time = GetTime();
   if(mask&MDB_KADR_FIELD_IMAGE)
      _kadr->image_time   =  get_kadr_image_time(_kadr);

   /*Проверка работы записи историй*/
//   _kadr->hist_beg_time =_kadr->image_time;
//   _kadr->hist_end_time =_kadr->image_time+1000000;
//   changes |= MDB_KADR_FIELDS_HIST_TIMES|MDB_KADR_FIELD_STATE;
   /*Проверка работы записи историй*/

   kadr_changes[kadr->kadr_id] |= changes;
   handle_kadr_changes();

  }
  //unlock();
  if(ret == GKH_RET_SUCCESS)
     {
      MDB_KADR_CHANGED kc;
      kc.kadr_id = kadr->kadr_id;
      kc.mask    = mask;
      module->notify(MDB_NOTIFY_MASK_KADR,MDB_NOTIFY_CODE_KADR,&kc,sizeof(kc));
      module->set_ccflags(MDB_CCFL_KADRS_TABLE,true);
     }
 }
  return ret;
 }

 DWORD      __fastcall med_db::kadr_remove (DWORD id)
 {
  DWORD ret = GKH_RET_ERROR;
  {
     #if( FASTMUTEX_LOCKER != 0)
      TLockHelper l (locker);
     #else
       wr_lock    l (locker);
     #endif
      if(kadrs.is_valid_kadr_ptr(kadrs.find_kadr(id)))
      {
          kadrs.remove_kadr(id);
          ret = GKH_RET_SUCCESS;
          module->set_ccflags(MDB_CCFL_KADRS_TABLE,true);

      }
  }

  if(ret == GKH_RET_SUCCESS)
   {
    module->notify(MDB_NOTIFY_MASK_KADR,MDB_NOTIFY_CODE_KADR_REMOVED,&id ,sizeof(id));
    mproto_send_kadr_removed(id);
   }

  return ret;
 }

 DWORD      __fastcall med_db::kadr_get_entry_list(DWORD kadr_id,LPMDB_KADR_ENTRY_LIST list)
 {
  DWORD ret = GKH_RET_ERROR;
  if(module->check_data_size(list,sizeof(*list)))
  {
    list->count = 0;
     #if( FASTMUTEX_LOCKER != 0)
      TLockHelper l (locker);
     #else
       rd_lock    l (locker);
     #endif
    kadrs_list::iterator kadr = kadrs.find_kadr(kadr_id);
    if(kadrs.is_valid_kadr_ptr(kadr))
    {
     kadr_entryes_table::iterator  lo,hi;
     DWORD count = kadrs.get_entry_range(kadr_id,lo,hi);
     if(count)
       {
         LPMDB_KADR_ENTRY dest = list->entryes;

         while(lo<hi && list->count<list->capacity )
         {
          memcpy(dest,&*lo,sizeof(*dest));
          ++list->count;
          ++dest;
          ++lo;
         }
       if(count!= list->count)
          ret = -int(count);
          else
          ret = GKH_RET_SUCCESS;
      }
    }
    //unlock();
  }
  return ret;
 }

 DWORD      __fastcall med_db::kadr_make_record_list   (DWORD kadr_id,record_idlist & rec)
 {
     #if( FASTMUTEX_LOCKER != 0)
      TLockHelper l (locker);
     #else
       rd_lock    l (locker);
     #endif
    kadrs_list::iterator kadr = kadrs.find_kadr(kadr_id);
    if(kadrs.is_valid_kadr_ptr(kadr))
    {
     kadr_entryes_table::iterator  lo,hi;
     if(kadrs.get_entry_range(kadr_id,lo,hi))
       {
        while(lo<hi)
        {
         rec.push_back(lo->rec_id);
         ++lo;
        }
       }
    }
   return rec.size();
 }

 DWORD      __fastcall med_db::kadr_set_entry_list(DWORD kadr_id,LPMDB_KADR_ENTRY_LIST list)
 {
  DWORD ret = GKH_RET_ERROR;
  if(module->check_data_size(list,sizeof(*list)))
  {
     #if( FASTMUTEX_LOCKER != 0)
      TLockHelper l (locker);
     #else
       wr_lock    l (locker);
     #endif
    kadrs_list::iterator kadr = kadrs.find_kadr(kadr_id);
    if(kadrs.is_valid_kadr_ptr(kadr))
    {
     ret = GKH_RET_SUCCESS;

     kadrs.kadr_clear(kadr_id);
     for(DWORD i = 0;i<list->count;++i)
      {
       mdb_kadr_entry entry(list->entryes[i]);
       entry.kadr_id = kadr_id;
       kadrs.add_entry(entry);
      }
    }
    kadr->content_time = GetTime();
    kadr_changes[kadr_id] = MDB_KADR_FIELD_ENTRYES|kadr_update_diag(kadr_id);
    handle_kadr_changes();

    //unlock();
    if(ret == GKH_RET_SUCCESS)
    {
     module->set_ccflags(MDB_CCFL_KADRS_TABLE,true);
    }
  }
  return ret;
 }

 DWORD      __fastcall med_db::_kadr_update_diag(kadrs_list::iterator kadr)
 {
   DWORD ret              = 0;
   kadr->kadr_changes     = 0;
   kadr->alarm_changes    = 0;
   kadr->discrete_changes = 0;

   WORD new_diag     = 0;
   WORD new_state    = 0;
   kadr_entryes_table::iterator  lo,hi;
   DWORD entry_count = kadrs.get_entry_range(kadr->kadr_id,lo,hi);
   if(entry_count)
   {
    mdb_record record;
    while(lo<hi)
    {
    get_record(lo->rec_id,&record);
    new_diag |= record.diag;
    //if(record.diag&MDBR_DIAG_BAD) ++bad;
    new_state |= (record.state&MDBR_STATE_DYN_MASK) ;
    if(this->record_changes.count(record.rec_id))
       {
        DWORD rec_chmask = record_changes[record.rec_id];
        if(rec_chmask&(MDBR_FIELD_VALUE|MDBR_FIELD_DIAG|MDBR_FIELD_STATE))
           {
            if( (rec_chmask&MDBR_FIELD_STATE) && (record.state & (MDBR_STATE_ALARM|MDBR_STATE_DANGER)))
              {
               ++kadr->alarm_changes;ret|=MDB_KADR_FIELD_ALARM_CHANGES;
              }

            if((record.state & MDBR_STATE_DISCRETE) && (rec_chmask & MDBR_FIELD_VALUE))
              {
               ++kadr->discrete_changes;ret|=MDB_KADR_FIELD_DISCRETE_CHANGES;
              }

            ++kadr->kadr_changes; ret|=MDB_KADR_FIELD_CHANGES;
           }
       }

    ++lo;
    }

//   if(bad)
//    {
//     new_diag = (bad == entry_count) ? MDBR_DIAG_BAD:MDBR_DIAG_PARTIAL;
//    }
//    else
//    new_diag = MDBR_DIAG_GOOD;
   }

   if(kadr->kadr_diag!=new_diag)
      {
       kadr->kadr_diag=new_diag;
       ret|=MDB_KADR_FIELD_DIAG;
      }

   new_state &=MDBR_STATE_DYN_MASK;
   if((kadr->kadr_state&MDBR_STATE_DYN_MASK)!=(new_state&MDBR_STATE_DYN_MASK))
   {
       kadr->kadr_state= (kadr->kadr_state&MDBR_STATE_CONST_MASK)|new_state;
       ret|=MDB_KADR_FIELD_STATE;
   }
   return ret;
 }

 DWORD      __fastcall med_db::kadr_update_diag(DWORD kadr_id)
 {
  // Обнеовление диагностики кадра
  TLockHelper l(locker);
  DWORD ret = 0;
  kadrs_list::iterator kadr  = kadrs.find_kadr(kadr_id);
  if(kadrs.is_valid_kadr_ptr(kadr))
  {
    ret = _kadr_update_diag(kadr);
  }
  return ret;
 }



 DWORD      __fastcall med_db::kadr_find_byname     (wchar_t * long_name , LPDWORD kadr_id)
 {
  DWORD ret = GKH_RET_ERROR;
     #if( FASTMUTEX_LOCKER != 0)
      TLockHelper l (locker);
     #else
       rd_lock    l (locker);
     #endif
  DWORD kidx = 0;
  MDB_KADR kadr;
  while(ret && GKH_RET_SUCCESS == this->kadrs.enum_kadr(kidx++,kadr))
  {
     if(kadr.name_id!=(DWORD)-1)
     {
       wchar_t  kadr_name[1024];
       if(txt_table.get_text(kadr.name_id,kadr_name,KERTL_ARRAY_COUNT(kadr_name)) && 0 == wcscmp(long_name , kadr_name))
       {
          ret = GKH_RET_SUCCESS;
          if(kadr_id) *kadr_id = kadr.kadr_id;
          break;
       }

     }
  }
  return ret;
 }

 DWORD      __fastcall med_db::kadr_find_record     ( DWORD kadr_id      , DWORD rec_id)
 {
  DWORD ret = GKH_RET_ERROR;
     #if( FASTMUTEX_LOCKER != 0)
      TLockHelper l (locker);
     #else
       rd_lock    l (locker);
     #endif
  kadr_entryes_table::iterator  low, hi;
  if(kadrs.get_entry_range(kadr_id,rec_id,low,hi))
    ret = low->number ;
  return ret;
 }

  DWORD      __fastcall med_db::kadr_add_record      (DWORD kadr_id      , DWORD rec_id)
  {
     kadr_entryes_table::iterator ptr;
     #if( FASTMUTEX_LOCKER != 0)
      TLockHelper l (locker);
     #else
       wr_lock    l (locker);
     #endif
     kadrs_list::iterator kadr = kadrs.find_kadr(kadr_id);
     if(kadrs.is_valid_kadr_ptr(kadr))
     {
     ptr = this->kadrs.add_entry(kadr_id,rec_id);
     kadr->content_time = GetTime();
     kadr_changes[kadr_id] = MDB_KADR_FIELD_ENTRYES|kadr_update_diag(kadr_id);
     handle_kadr_changes();
     module->set_ccflags(MDB_CCFL_KADRS_TABLE,true);
     return ptr->number;
     }
   return GKH_RET_ERROR;
  }

