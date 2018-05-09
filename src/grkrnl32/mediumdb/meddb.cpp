#pragma hdrstop
#include "mediumdb.hpp"
#include <KeStrm.hpp>
#include <kfname.hpp>
#include <list>
#include <stdio.h>
#include <iterator>
#include <otd_proto.h>


struct tbl_header
{
 char   magic[32];
 DWORD  stream_ver;
 DWORD  count;
};

#define RECORDS_MAGIC   "MediumDB::RecordsTable"
#define COMBINE_MAGIC   "MediumDB::CombineTable"
#define KADRS_MAGIC     "MediumDB::KadrsTable"
#define PAINTVARS_MAGIC "MediumDB::PaintVariants"

//#define RECORDS_FILENAME_OLD    L"RECORDS.MDB"
//#define COMBINE_FILENAME_OLD    L"COMBINE.MDB"
//#define KADRS_FILENAME_OLD      L"KADRS.MDB"
//#define PAINTVARS_FILENAME_OLD  L"PAINTVRS.MDB"

#define RECORDS_FILENAME    L"RECORDS"
#define COMBINE_FILENAME    L"COMBINE"
#define KADRS_FILENAME      L"KADRS"
#define PAINTVARS_FILENAME  L"PAINTVRS"


 KeRTL::TFileStream * __fastcall create_file_stream(wchar_t * folder,wchar_t * file,bool read_only)
 {
   wchar_t file_name[MAX_PATH<<2];
  _wmakepath(file_name,NULL,folder,file,L"MDB");
   KeRTL::TFileStream * stream = new KeRTL::TFileStream(file_name,read_only);
   return stream;
 }

 bool       __fastcall med_db::read_records         (wchar_t* folder)
 {
  bool ret = false;
  TLockHelper l(locker);
  records.clear();
  KeRTL::TFileStream * stream = create_file_stream(folder,RECORDS_FILENAME,true);
  if(stream)
  {
   if(stream->IsGood())
   {
     tbl_header hdr;
     stream->Read(&hdr,sizeof(hdr));
     if(memcmp(hdr.magic,RECORDS_MAGIC,sizeof(RECORDS_MAGIC))==0)
     {
      ret = true;
      records.stream_read(*stream,hdr.stream_ver,hdr.count);
      for(DWORD i = 0 ;i<records.size();i++)
       {
        mdb_record & rec = records.at(i);
         if( !(rec.options &MDBR_OPT_CONST_MINMAX) &&(!((int)rec.max_value - int(rec.min_value))))
            update_minmax_value(rec);
         if(fabs(rec.min_value - rec.max_value)<.000001)
             rec.max_value = rec.min_value + 1;
         if((rec.state&MDBR_STATE_DISCRETE) && rec.kvants > (DWORD)rec.max_value)
             rec.kvants = (DWORD)rec.max_value;

       }
     }
   }
   delete stream;
  }
  return ret;
 }

 bool       __fastcall med_db::read_kadrs           (wchar_t* folder)
 {
  bool ret = false;
  TLockHelper l(locker);
  kadrs.clear();
  KeRTL::TFileStream * stream = create_file_stream(folder,KADRS_FILENAME,true);
  if(stream)
  {
   if(stream->IsGood())
   {
     tbl_header hdr;
     stream->Read(&hdr,sizeof(hdr));
     if(memcmp(hdr.magic,KADRS_MAGIC,sizeof(KADRS_MAGIC))==0)
     {
      kadrs.stream_read(*stream,hdr.stream_ver,hdr.count);
      kadrs_list::iterator kdr     = kadrs.begin_kadr();
      kadrs_list::iterator kdr_end = kadrs.end_kadr();
      while(kdr<kdr_end)
      {
        kdr->hist_beg_time = kdr->hist_end_time = 0;
        this->_kadr_update_diag(kdr);
        ++kdr;
      }
       ret = true;
     }
   }
   delete stream;
  }
  return ret;
 }

 bool       __fastcall med_db::read_combines        (wchar_t* folder)
 {
  bool ret = false;
  TLockHelper l(locker);
  this->combine.clear();
  KeRTL::TFileStream * stream = create_file_stream(folder,COMBINE_FILENAME,true);
  if(stream)
  {
   if(stream->IsGood())
   {
     tbl_header hdr;
     stream->Read(&hdr,sizeof(hdr));
     if(memcmp(hdr.magic,COMBINE_MAGIC,sizeof(COMBINE_MAGIC))==0)
     {
       combine.stream_read(*stream,hdr.stream_ver,hdr.count);
       ret = true;
     }
   }
   delete stream;
  }
  return ret;
 }

 bool       __fastcall med_db::read_paintvars       (wchar_t* folder)
 {
  bool ret = false;
  TLockHelper l(locker);
  this->paint_vars.clear();
  KeRTL::TFileStream * stream = create_file_stream(folder,PAINTVARS_FILENAME,true);
  if(stream)
  {
   if(stream->IsGood())
   {
     tbl_header hdr;
     stream->Read(&hdr,sizeof(hdr));
     if(memcmp(hdr.magic,PAINTVARS_MAGIC,sizeof(PAINTVARS_MAGIC))==0)
     {
       paint_vars_read(*stream,hdr.stream_ver,hdr.count);
       ret = true;
     }
   }
   delete stream;
  }
  return ret;
 }


 bool       __fastcall med_db::open  (wchar_t* folder)
 {
  TLockHelper l(locker);
  txt_table.close();
  wchar_t name[MAX_PATH<<1];
  _wmakepath(name,NULL,folder,L"names.mdb",NULL);
  if(!txt_table.open(name))
  {
   module->report(L"Ошибка открытия файла имен...",L"Принудительное открытие");
   txt_table.open(name,true);
  }

  read_records  (folder);
  read_combines (folder);
  read_kadrs    (folder);
  read_paintvars(folder);

  record_changes.clear();
  tutr_actives.clear();

  return true;
 }


void       __fastcall med_db::store_records(wchar_t * folder)
{
 #if( FASTMUTEX_LOCKER != 0)
  TLockHelper l (locker);
 #else
   rd_lock l(locker);
 #endif

  TFileStream * strm = create_file_stream(folder,RECORDS_FILENAME,false);
  if(strm)
  {
  if(strm->IsGood())
   {
    tbl_header hdr;
    ZeroMemory(&hdr,sizeof(hdr));
    memcpy(hdr.magic,RECORDS_MAGIC,sizeof(RECORDS_MAGIC));
    hdr.stream_ver = records.get_stream_ver();
    hdr.count = records.size();
    strm->Write(&hdr,sizeof(hdr));
    records.stream_write(*strm);
   }
   delete strm;
  }
}

void       __fastcall med_db::store_kadrs(wchar_t * folder)
{
 #if( FASTMUTEX_LOCKER != 0)
  TLockHelper l (locker);
 #else
   rd_lock l(locker);
 #endif
  TFileStream * strm = create_file_stream(folder,KADRS_FILENAME,false);
  if(strm)
  {
  if(strm->IsGood())
   {
    tbl_header hdr;
    ZeroMemory(&hdr,sizeof(hdr));
    memcpy(hdr.magic,KADRS_MAGIC,sizeof(KADRS_MAGIC));
    hdr.stream_ver = kadrs.get_stream_ver();

    hdr.count = kadrs.size();
    strm->Write(&hdr,sizeof(hdr));
    kadrs.stream_write(*strm);
   }
   delete strm;
  }
}

void       __fastcall med_db::store_combines(wchar_t * folder)
{
 #if( FASTMUTEX_LOCKER != 0)
  TLockHelper l (locker);
 #else
   rd_lock l(locker);
 #endif
  TFileStream * strm = create_file_stream(folder,COMBINE_FILENAME,false);
  if(strm)
  {
  if(strm->IsGood())
   {
    tbl_header hdr;
    ZeroMemory(&hdr,sizeof(hdr));
    memcpy(hdr.magic,COMBINE_MAGIC,sizeof(RECORDS_MAGIC));
    hdr.stream_ver = combine.get_stream_ver();
    hdr.count = combine.size();
    strm->Write(&hdr,sizeof(hdr));
    combine.stream_write(*strm);
   }
   delete strm;
  }

}


void       __fastcall med_db::store_paintvars(wchar_t * folder)
{
 #if( FASTMUTEX_LOCKER != 0)
  TLockHelper l (locker);
 #else
   rd_lock l(locker);
 #endif
  TFileStream * strm = create_file_stream(folder,PAINTVARS_FILENAME,false);
  if(strm)
  {
      if(strm->IsGood())
       {
        tbl_header hdr;
        ZeroMemory(&hdr,sizeof(hdr));
        memcpy(hdr.magic,PAINTVARS_MAGIC,sizeof(PAINTVARS_MAGIC));
        hdr.stream_ver = paint_vars_stream_ver();
        hdr.count = paint_vars.size();
        strm->Write(&hdr,sizeof(hdr));
        paint_vars_write(*strm);
       }
       else
       {
        char  text[1024];
        KeRTL::GetErrorMessage(GetLastError(),text,sizeof(text),(HINSTANCE)NULL);
        KeRTL::GetErrorMessage(GetLastError(),text,sizeof(text),(HINSTANCE)NULL);
       }
       delete strm;
  }


}

void       __fastcall med_db::store(wchar_t * folder,DWORD mask)
{
 wchar_t file[MAX_PATH<<1];
 tbl_header hdr;

 #if( FASTMUTEX_LOCKER != 0)
  TLockHelper l (locker);
 #else
   rd_lock l(locker);
 #endif

 GetCurrentDirectoryW(KERTL_ARRAY_COUNT(file),file);

 //if(mask & MDB_CCFL_RECORD_TABLE)
 store_records(folder);

 if(mask & MDB_CCFL_COMBINE_TABLE)
  store_combines(folder);

 if(mask & MDB_CCFL_KADRS_TABLE)
  store_kadrs(folder);

 if(mask & MDB_CCFL_PAINT_VARS)
  store_paintvars(folder);

  txt_table.reopen();
}


 void       __fastcall med_db::config_change(DWORD mask)
 {
   if(module)
     module->set_ccflags(mask,true);
 }



 DWORD      __fastcall med_db::create(LPMDB_RECORD rec,char * formula,bool no_call_data)
 {
  DWORD ret = -1;
  if(rec->addr.addr.pu!=OTD_ADDR_MAXVALUE || (rec->options&MDBR_OPT_COMBINED))
  {
     #if( FASTMUTEX_LOCKER != 0)
      TLockHelper l (locker);
     #else
       wr_lock    l (locker);
     #endif
   DWORD offset =  records.find_by_addr(rec->addr);
   if(offset==(DWORD)-1 || rec->addr.param_number!=(DWORD)-1 ||(rec->options&MDBR_OPT_COMBINED) )
   {
   if(!(rec->options&MDBR_OPT_COMBINED) && otd_addr_type(&rec->addr.addr)>OTD_ADDR_TYPE_PU )
      auto_create_owners (*rec);
    if(rec->options&MDBR_OPT_PSEUDO)
    {
     rec->diag    = 0;rec->otd_pd = 0;
    }

    if(is_record_discrete(rec)) rec->otd_val = 0;

    ret = records.insert (*rec);
    if(rec->options&MDBR_OPT_COMBINED)
      {
        combine.create_combine_object(rec->rec_id,formula);
        config_change(MDB_CCFL_COMBINE_TABLE);
      }
      else
      {
       update_minmax_value(records.get_record(ret));
       if(rec->options & MDBR_OPT_ALARM)
          {
           record_changes[rec->rec_id] = MDBR_ALARMS_CHANGED;
           handle_record_changes();
          }
      }


    if(ret != (DWORD) GKH_RET_ERROR)
    {
      config_change(MDB_CCFL_RECORD_TABLE);
      if(module)
      {
          if(!(rec->options&MDBR_OPT_COMBINED) && !no_call_data)
              module->send_call(rec->addr.addr);
          module->notify(MDB_NOTIFY_MASK_RECORD,MDB_NOTIFY_CODE_RECORD_CREATED,&rec->rec_id,sizeof(rec->rec_id));
      }
    }
   }
   else
   SetLastError(RPC_S_ENTRY_ALREADY_EXISTS /*MDB_ERR_OBJECT_ALREADY_EXISTS*/);
  }
  return ret;
 }

DWORD __fastcall med_db::remove_group(DWORD group_rec_id,otd_param_addr & pa)
{
  DWORD ret = 0;
     #if( FASTMUTEX_LOCKER != 0)
      TLockHelper l (locker);
     #else
       wr_lock    l (locker);
     #endif
  sotd_param_addr low_addr(pa);
  sotd_param_addr hi_addr (pa);
  if(low_addr.addr.pu ==OTD_ADDR_MAXVALUE) low_addr.addr.pu = 0;
  if(low_addr.addr.cp ==OTD_ADDR_MAXVALUE) low_addr.addr.cp = 0;
  if((low_addr.addr.fa&OTD_FA_ALL) ==OTD_FA_ALL)low_addr.addr.fa = 0,hi_addr.addr.fa = -1;
  if(low_addr.addr.sb ==OTD_ADDR_MAXVALUE) low_addr.addr.sb = 0;
  low_addr.param_number = 0;
  hi_addr.param_number = -1;
  trecords_index::iterator low_idx,hi_idx;
  ret = records.get_addr_range(low_addr,hi_addr,low_idx,hi_idx);
  if(ret)
  {
   //Строим список ID для удаления
   std::vector<DWORD> remove_list;
   remove_list.reserve(ret);
   while(low_idx<hi_idx)
   {
    DWORD id = records.at(*low_idx).rec_id;
    if(id!=group_rec_id)
       remove_list.push_back(id);
    ++low_idx;
   }
   std::vector<DWORD>::iterator rem_begin = remove_list.begin(),rem_end = remove_list.end();
   while(rem_begin<rem_end)
   {

    DWORD id =*rem_begin;
    if(combine.remove_object(id) || combine.remove(id))
       config_change(MDB_CCFL_COMBINE_TABLE);
     //TODO remove_from_combines
     if(kadrs.remove_entry(id))
       config_change(MDB_CCFL_KADRS_TABLE);
     mdb_record & rec = records.get_record(id);
     if(rec.addr == pa)
       rec.diag = 0;
       update_owner_diag(id,0);
     records.remove(id);
     record_changed(id,MDBR_RECORD_REMOVED);
    ++rem_begin;
   }

  }
  //unlock();
 return ret;
}

DWORD      __fastcall med_db::remove(DWORD id)
{
  DWORD ret = GKH_RET_ERROR;
     #if( FASTMUTEX_LOCKER != 0)
      TLockHelper l (locker);
     #else
       wr_lock    l (locker);
     #endif
  //Удаление объектов и подобъектов
  DWORD offset = records.find_by_id(id);
  if(offset<records.size())
  {

  mdb_record & rec = records.at(offset);

  if(!(rec.options&MDBR_OPT_COMBINED) && rec.addr.param_number == (DWORD)-1 )
  {
     trecords_index::iterator lptr,hptr;
     DWORD obj_count = records.get_addr_range(rec.addr,rec.addr,lptr,hptr);
     if(obj_count<2)
        remove_group(rec.rec_id,rec.addr);
  }
    if(combine.remove_object(id) ||  combine.remove(id))
       {
        config_change(MDB_CCFL_COMBINE_TABLE);
       }

    entry_index::iterator lo,hi;
    DWORD count = kadrs.get_kadr_range(id,lo,hi);
    
    if(count)
      {
       while(lo<hi)
       {
        DWORD kid = kadrs.entry_at(*lo)->kadr_id;
        DWORD ch  = kadr_update_diag(kid);
        if(ch)
        kadr_changes[kid] = ch;
        ++lo;
       }
       kadrs.remove_entry(id);
       config_change(MDB_CCFL_KADRS_TABLE);
      }
   
    rec.diag = MDBR_DIAG_GOOD;
    update_owner_diag(id,0);
    record_changed(id,MDBR_RECORD_REMOVED);
    records.remove(id);
    config_change (MDB_CCFL_RECORD_TABLE);

    ret = GKH_RET_SUCCESS;
  }
  
  if(kadr_changes.size())
     handle_kadr_changes();
  //unlock();
  return ret;
}

DWORD         __fastcall med_db::get_record_parents_id(DWORD rec_id,LPDWORD buf,DWORD count)
{
  DWORD ret   = 0;
  mdb_record  rec;
     #if( FASTMUTEX_LOCKER != 0)
      TLockHelper l (locker);
     #else
       rd_lock    l (locker);
     #endif
  if(GKH_RET_SUCCESS == get_record(rec_id,&rec))
  {
    sotd_param_addr pa( rec.addr );
    pa.param_number = -1;
    pa.addr.sb      = -1;
    pa.addr.fa      = -1;
    if(pa != rec.addr && ret<count)
    {
      DWORD offset = records.find_by_addr(pa);
      if(offset != DWORD(-1))
         {
          rec = records.at(offset);
          buf[ret++] = rec.rec_id;
         }
    }

    pa.addr.cp = -1;
    if(pa!=rec.addr && ret<count)
    {
      DWORD offset = records.find_by_addr(pa);
      if(offset != DWORD(-1))
         {
          rec = records.at(offset);
          buf[ret++] = rec.rec_id;
         }
    }
  }
  return ret;
}

//void         __fastcall med_db::auto_create_owners(MDB_RECORD & rec)
//{
// //Автоматичекое создание ПУ и КП при их отсутствии
// mdb_record grp_rec;
// sotd_addr addr(rec.addr.addr);
// if(  addr.addrtype()!=OTD_ADDR_TYPE_BROADCAST)
// {
// if(rec.addr.param_number != (DWORD)-1 && records.find_by_addr(addr,-1)==(DWORD)-1)
//  {
//    grp_rec.addr = sotd_param_addr(addr,-1);
//    records.insert(grp_rec);
//    config_change(MDB_CCFL_RECORD_TABLE);
//  }
//
// while(addr.addrtype()>OTD_ADDR_TYPE_CP)
//      addr = addr.owner_addr();
// if(records.find_by_addr(addr,-1)==(DWORD) -1)
//   {
//    grp_rec.addr = sotd_param_addr(addr,-1);
//    records.insert(grp_rec);
//    if(module)
//       module->send_call(grp_rec.addr.addr,OTD_PROTO_PART_DIAG|OTD_PROTO_PART_NAME);
//    config_change(MDB_CCFL_RECORD_TABLE);
//   }
//
//      addr = addr.owner_addr();
// if(records.find_by_addr(addr,-1)==(DWORD) -1)
//   {
//    grp_rec.addr = sotd_param_addr(addr,-1);
//    records.insert(grp_rec);
//    if(module)
//       module->send_call(grp_rec.addr.addr,OTD_PROTO_PART_DIAG|OTD_PROTO_PART_NAME);
//    config_change(MDB_CCFL_RECORD_TABLE);
//   }
// }
//}

void         __fastcall med_db::auto_create_owners(MDB_RECORD & rec)
{
 //Автоматичекое создание ПУ и КП при их отсутствии
 mdb_record grp_rec;
 sotd_param_addr  addr(rec.addr);
 DWORD atype = addr.addrtype();

 if(atype  !=OTD_ADDR_TYPE_BROADCAST)
 {
     int ins_count = 0;
     do
     {
         addr  = addr.owner_addr();
         atype = addr.addrtype  ();
         if(rec.addr != addr && atype != OTD_ADDR_TYPE_FA && records.find_by_addr(addr)==(DWORD) -1)
           {
            grp_rec.addr = addr;
            records.insert(grp_rec);
            if(module)
               module->send_call(grp_rec.addr.addr,OTD_PROTO_PART_DIAG|OTD_PROTO_PART_NAME);
            ++ins_count;
           }

     }while(atype>OTD_ADDR_TYPE_PU && OTD_ADDR_TYPE_BROADCAST < OTD_ADDR_TYPE_BROADCAST);

   if(ins_count)
     config_change(MDB_CCFL_RECORD_TABLE);
 }
}

DWORD        __fastcall med_db::record_change_addr   (mdb_record & record,LPMDB_RECORD src_rec)
{
  DWORD  ret = GKH_RET_SUCCESS;
  if(record.addr != src_rec->addr)
  {
       trecords_index::iterator low_ptr,hi_ptr;
       otd_param_addr low_addr = record.addr;
       if(record.addr.param_number==(DWORD)-1 && src_rec->addr.param_number==(DWORD)-1 )
       {
        low_addr.param_number   = 0;
       }
       if(records.get_addr_range(low_addr,record.addr,low_ptr,hi_ptr))
       {
           while(low_ptr<hi_ptr)
           {
            mdb_record & upd_rec = records.at(*low_ptr);
            upd_rec.addr.addr = src_rec->addr.addr;
            if(src_rec->addr.param_number!=DWORD(-1))
              upd_rec.addr.param_number = src_rec->addr.param_number;
              upd_rec.diag = MDBR_DIAG_BAD;
            ++low_ptr;
           }
         records.rebuild_indexes();
       }
  }
  return ret ;
}


DWORD      __fastcall med_db::update(LPMDB_RECORD rec,DWORD mask)
{
 DWORD ret = 0;
     #if( FASTMUTEX_LOCKER != 0)
      TLockHelper l (locker);
     #else
       wr_lock    l (locker);
     #endif

 mdb_record & record = records.get_record(rec->rec_id);
    if(mask & MDBR_FIELD_ADDR)
      {
       ret = record_change_addr(record,rec);
       if(ret) return GKH_RET_ERROR;
      }

 if(record.rec_id == rec->rec_id)
   {
    if(mask & MDBR_FIELD_PERCENT)
       record.percent = rec->percent;
    if(mask & MDBR_FIELD_NAME)
       {
        if(record.name != rec->name)
           record.name = rec->name;
           else
           mask &=~ MDBR_FIELD_NAME;

       }
    if(mask & MDBR_FIELD_SNAME)
    {
        if(record.sname != rec->sname)
           record.sname = rec->sname;
           else
           mask &=~ MDBR_FIELD_SNAME;
    }
    if(mask & MDBR_FIELD_STATE)
      {
       if(record.addr.param_number ==(DWORD) -1 && !(record.options&MDBR_OPT_COMBINED))
        record.state&=(MDBR_STATE_DYN_MASK|MDBR_STATE_DISCRETE);
        else
        record.state&=(MDBR_STATE_DYN_MASK);
       record.state|= (rec->state&MDBR_STATE_CONST_MASK);
       #ifdef _DEBUG
        if(record.options&MDBR_OPT_COMBINED)
        {
          record.state &=~(MDBR_STATE_TUTR_ACTIVE|MDBR_STATE_TUTR_PREPARE|MDBR_STATE_TUTR_ERROR);
          record.state|=(rec->state&(MDBR_STATE_TUTR_ACTIVE|MDBR_STATE_TUTR_PREPARE|MDBR_STATE_TUTR_ERROR));
        }
       #endif

      }

    if(mask & MDBR_FIELD_OPTIONS)
       {
        rec->options |= record.options&(MDBR_OPT_FLOAT_TYPE);
        DWORD opt_changed = record.options^rec->options;
        record.options&=MDBR_OPT_COMBINED;
        record.options|=rec->options;
        if(opt_changed & MDBR_OPT_PSEUDO)
           {
            //Изменение псевдо
            if(record.options& MDBR_OPT_PSEUDO)
             record.diag = MDBR_DIAG_GOOD;
            else
            {
               record.diag = MDBR_DIAG_BAD;
               module->send_call(record.addr.addr,-1);
            }
            mask|=MDBR_FIELD_DIAG;
           }

        if(record.options &(MDBR_OPT_ALARM|MDBR_OPT_DANGER ))
           {
            record.state &=~(MDBR_STATE_DYN_NORMAL|MDBR_STATE_NORMAL);
            mask|=MDBR_FIELD_STATE;
           }

        if((opt_changed&MDBR_OPT_ALARM) || ((record.options&MDBR_OPT_ALARM) && (opt_changed&MDBR_OPT_INVERSE_SCALE)))
           mask |= MDBR_ALARMS_CHANGED;

       }

     if(mask&MDBR_FIELD_MIN_DANGER)
        record.min_danger = rec->min_danger;
     if(mask&MDBR_FIELD_MAX_DANGER)
        record.max_danger = rec->max_danger;

     if(mask&MDBR_FIELD_MIN_ALARM)
        {
         if((record.options&MDBR_OPT_ALARM) && record.has_min_alarm() && mdb_record(*rec).has_min_alarm() &&  int(100000.0*(record.min_alarm - rec->min_alarm)))
            mask |= MDBR_ALARMS_CHANGED;
            record.min_alarm = rec->min_alarm;
        }
     if(mask&MDBR_FIELD_MAX_ALARM)
     {
       if((record.options&MDBR_OPT_ALARM) && record.has_max_alarm() && mdb_record(*rec).has_max_alarm() && int(100000.0*(record.max_alarm - rec->max_alarm)))
            mask |= MDBR_ALARMS_CHANGED;
            record.max_alarm = rec->max_alarm;
     }


    if(mask & MDBR_FIELD_SCALEID)
       {
        record.scale_id = rec->scale_id;
        mask|=update_minmax_value(record);

       }

    if(mask&(MDBR_FIELD_KVANTS|MDBR_FIELD_MIN_VALUE|MDBR_FIELD_MAX_VALUE|MDBR_FIELD_SCALEID))
      {
       record.set_scale(rec->min_value,rec->max_value,rec->kvants);
      }

    if(mask&(MDBR_FIELD_KVANTS|MDBR_FIELD_MIN_VALUE|MDBR_FIELD_MAX_VALUE|MDBR_FIELD_SCALEID|MDBR_FIELD_MIN_DANGER|MDBR_FIELD_MAX_DANGER|MDBR_FIELD_MIN_ALARM|MDBR_FIELD_MAX_ALARM|MDBR_FIELD_OPTIONS))
    {
       mask|=calc_record(record,0);
       mask|=update_minmax_value(record);
    }



    if(mask&(~MDBR_COMBINE_FORMULA))
    {
     mdb_record mr(*rec);
     record_changed(mr, mask | MDBR_FIELD_VALUE );

//     if(mask !=MDBR_FIELD_STATE)
//     record_changed(mr,mask|MDBR_FIELD_VALUE);
//     else
//     record_changed(mr,mask);

     config_change(MDB_CCFL_RECORD_TABLE);
     handle_record_changes();
    }
   }
 //unlock();
 return ret;
}


 DWORD      __fastcall med_db::get_count   (otd_addr & addr)
 {
   DWORD ret;
     #if( FASTMUTEX_LOCKER != 0)
      TLockHelper l (locker);
     #else
       rd_lock    l (locker);
     #endif
   ret = records.get_count(addr);
   //unlock();
   return  ret;
 }

 DWORD      __fastcall med_db::enum_objects(otd_addr & addr,DWORD idx)
 {
   DWORD ret;
     #if( FASTMUTEX_LOCKER != 0)
      TLockHelper l (locker);
     #else
       rd_lock    l (locker);
     #endif
     ret = records.enum_objects_II(addr,idx);
   //unlock();
   return  ret;
 }

 DWORD      __fastcall med_db::enum_orders(DWORD idx,LPORDER_ITEM oi)
 {
    DWORD ret = GKH_RET_ERROR;
    if(this->module->check_data_size(oi,sizeof(*oi),true))
    {
     #if( FASTMUTEX_LOCKER != 0)
      TLockHelper l (locker);
     #else
       rd_lock    l (locker);
     #endif
     if(orders.enum_orders(idx,oi,true))
       ret = GKH_RET_SUCCESS;
    }
   return ret;
 }



 DWORD      __fastcall med_db::enum_records(DWORD idx,BOOL by_addr,LPMDB_RECORD rec)
 {
  DWORD ret = -1;
     #if( FASTMUTEX_LOCKER != 0)
      TLockHelper l (locker);
     #else
       rd_lock    l (locker);
     #endif
  if(idx<this->records.size())
  {
   mdb_record & found_rec = records.at(idx,by_addr ? true:false);
   if(&found_rec && found_rec.rec_id!=(DWORD)-1)
   {
    ret = found_rec.rec_id;
    if(rec)
       memcpy(rec,&found_rec,sizeof(*rec));
   }
  }
  else
  SetLastError(0);//NO_MORE_RECORDS
  //unlock();
  return ret;
 }

 DWORD      __fastcall med_db::get_record  (DWORD id,LPMDB_RECORD rec)
 {
   DWORD ret = GKH_RET_ERROR;
     #if( FASTMUTEX_LOCKER != 0)
      TLockHelper l (locker);
     #else
       rd_lock    l (locker);
     #endif
   DWORD offset ;
    if(id!=(DWORD)-1)
      offset = records.find_by_id(id) ;
      else
      offset =records.find_by_addr(rec->addr);
   if(offset<records.size())
   {
    memcpy(rec,&records.at(offset),sizeof(*rec));;
    ret = GKH_RET_SUCCESS;
   }
   else
   SetLastError(ERROR_CLASS_DOES_NOT_EXIST/*ERR_NO_RECORD*/);
   return ret;
 }


 DWORD      __fastcall med_db::get_name    (DWORD id,LPMDB_RECORD_TEXT rt,bool long_name)
 {
   DWORD ret = GKH_RET_ERROR;
     #if( FASTMUTEX_LOCKER != 0)
      TLockHelper l (locker);
     #else
       rd_lock    l (locker);
     #endif
   DWORD offset = records.find_by_id(id);
   if(offset<records.size())
   {
    mdb_record &rec = records.at(offset);
    DWORD text_id = long_name ? rec.name:rec.sname;
    if(text_id!=(DWORD)-1)
     {
      ret = GKH_RET_SUCCESS;
      rt->text_len = txt_table.get_text(text_id,rt->text ,rt->text_size);
     }
     else
     {
      ret = GKH_RET_SUCCESS;
      wchar_t src_text[MAX_PATH];
      int len = 0;
      if(rec.addr.param_number == (DWORD)-1)
       {
        switch(otd_addr_type(&rec.addr.addr))
        {
         case OTD_ADDR_TYPE_PU:
              len = swprintf(src_text,L"ПУ %03d",(DWORD)rec.addr.addr.pu);
         break;
         case OTD_ADDR_TYPE_CP:
              len = swprintf(src_text,L"КП %03d",(DWORD)rec.addr.addr.cp);
         break;
         case OTD_ADDR_TYPE_SB:
              len = swprintf(src_text,L"%s модуль %03d",rec.addr.addr.fa ? L"Аналоговый":L" Дискретный", (DWORD)rec.addr.addr.sb);

         break;
        }
       }
       else
         swprintf(src_text,L"объект № - %03d",rec.addr.param_number);
      rt->text_len = KeRTL::MIN(rt->text_size,(DWORD)len);
      if(rt->text_len)
         safe_strcpyn(rt->text,src_text,rt->text_len);
     }
   }
   //unlock();
   return ret;
 }

 DWORD __fastcall  med_db::get_name(DWORD rec_id,bool long_name,wchar_t * buf,DWORD bsz)
 {
  BYTE buffer[4096];
  LPMDB_RECORD_TEXT rt = (LPMDB_RECORD_TEXT)buffer;
  rt->dw_size   = sizeof(buffer);
  rt->text_len  = 0;
  rt->text_size = bsz;
  get_name(rec_id,rt,long_name);
  safe_strcpyn(buf,rt->text,rt->text_len);
  buf[rt->text_len] = 0;
  return rt->text_len;
 }

 DWORD __fastcall  med_db::get_full_name(DWORD rec_id,bool long_name,wchar_t * buf,DWORD bsz)
 {
  DWORD  len = 0;
  DWORD  rec_ids[8]   = {0};
  DWORD  parent_count = get_record_parents_id(rec_id,rec_ids,KERTL_ARRAY_COUNT(rec_ids));
  while(parent_count && len<bsz)
       {
         if(len)
            buf[len++] = '.';
            len += get_name(rec_ids[--parent_count],long_name,buf+len,bsz-len);
       }
  if(len<(bsz-1))
    {
       if(len) buf[len++] = '.';
       len += get_name(rec_id,long_name,buf+len,bsz-len);
    }
    return len;
 }

 DWORD      __fastcall med_db::get_full_name    (DWORD id,LPMDB_RECORD_TEXT rt,bool long_name)
 {
   rt->text_len = get_full_name(id,long_name,rt->text,rt->text_size);
   return rt->text_len ? GKH_RET_SUCCESS : GKH_RET_ERROR;
 }

 DWORD      __fastcall med_db::set_name    (DWORD id,LPMDB_RECORD_TEXT rt,bool long_name)
 {
   return set_name(id,rt->text,long_name);
 }

 DWORD      __fastcall med_db::set_name    (DWORD id,wchar_t * text,bool long_name)
 {
   DWORD ret = GKH_RET_ERROR;
     #if( FASTMUTEX_LOCKER != 0)
      TLockHelper l (locker);
     #else
       wr_lock    l (locker);
     #endif
   DWORD offset = records.find_by_id(id);
   if(offset<records.size())
   {
    ret = GKH_RET_SUCCESS;
    mdb_record &rec = records.at(offset);
    DWORD & text_id = long_name ? rec.name:rec.sname;
    if(text_id!=(DWORD)-1)
     {
      txt_table.update(text_id,text);
     }
     else
     {
        text_id = txt_table.add(text);
//      if(long_name) rec.name = text_id;
//      else
//      rec.sname = text_id;
      config_change(MDB_CCFL_RECORD_TABLE);
     }
     record_changed(id,long_name? MDBR_FIELD_NAME:MDBR_FIELD_SNAME);
     this->handle_record_changes();
     ret = text_id;
   }
   //unlock();
   return ret;
 }

 DWORD      __fastcall  med_db::update_text(DWORD text_id,LPMDB_RECORD_TEXT text)
 {
   DWORD ret = GKH_RET_ERROR ;
   if(module->check_data_size(text,sizeof(*text)))
   {
     #if( FASTMUTEX_LOCKER != 0)
      TLockHelper l (locker);
     #else
       wr_lock    l (locker);
     #endif
   if(text_id != DWORD(-1))
      ret = txt_table.update(text_id,text->text,text->text_len);
   if(ret == (DWORD)GKH_RET_ERROR)
     ret = txt_table.add(text->text,text->text_len);
   //unlock();
   }
   return  ret;
 }


 DWORD      __fastcall  med_db::get_text   (DWORD text_id,LPMDB_RECORD_TEXT text)
 {
   DWORD ret = -1;
   if(module->check_data_size(text,sizeof(*text)))
   {
     #if( FASTMUTEX_LOCKER != 0)
      TLockHelper l (locker);
     #else
       rd_lock    l (locker);
     #endif
   text->text_len = ret = txt_table.get_text(text_id,text->text,text->text_size);
   //unlock();
   }
   return  ret;
 }


 DWORD      __fastcall  med_db::get_info(LPMDB_DBINFO dbinfo)
 {
   DWORD ret = GKH_RET_ERROR;
   if(module->check_data_size(dbinfo,sizeof(*dbinfo)))
   {

    //lock();
     #if( FASTMUTEX_LOCKER != 0)
      TLockHelper l (locker);
     #else
       rd_lock    l (locker);
     #endif
    dbinfo->gen_record_id  = 1+records.get_idgen();
    dbinfo->records_count  = records.size();
    dbinfo->combines_count = combine.size();
   //unlock();


   //text_locker.Lock();
   dbinfo->names_count   = this->txt_table.count();
   //text_locker.unlock();

   //kadrs_locker.Lock();
   dbinfo->kadrs_count   = kadrs.size();
   dbinfo->gen_kadr_id   = 1+kadrs.get_genid();
   //kadrs_locker.unlock();
   ret = GKH_RET_SUCCESS;
   }
  return ret;
 }

 DWORD      __fastcall med_db::check_db(LPMDB_CHECK_PARAM cp)
 {
   DWORD ret = 0;
   wchar_t rep_text[1024];
     #if( FASTMUTEX_LOCKER != 0)
      TLockHelper l (locker);
     #else
       rd_lock    l (locker);
     #endif
   DWORD rec_cnt = records.size();
   module->mdb_report(REPORT_SUCCESS_TYPE,L"Начало проверки базы",0,0);
   //Проверка индексов
   for(DWORD i =0;i<rec_cnt;++i)
   {
     mdb_record  rec = records.at(i);
     DWORD idx = records.find_by_addr(rec.addr);
     if(i!=idx)
       {
        /*Не совпали значения индексов
         Возможно это дубль
        */
        mdb_record rec2 = records.at(idx);
        if(rec2.addr != rec.addr)
          {
           //Ошибка целостности индексов
           swprintf(rep_text,L"Нарушение целостности индексов  записи %d и %d ",rec.rec_id,rec2.rec_id);
           module->mdb_report(REPORT_ERROR_TYPE,rep_text,0,0);
           ++ret;
          }
           else
           {
            swprintf(rep_text,L"Дублирование адреса зиписи %d и %d ",rec.rec_id,rec2.rec_id);
            module->mdb_report(REPORT_WARNING_TYPE,rep_text,0,0);
           }

        if(cp && cp->c_proc)
           cp->c_proc(&rec,1);

       }

     auto_create_owners(rec);
   }
   //unlock();
   if(!ret)
   module->mdb_report(REPORT_SUCCESS_TYPE,L"Проверка  базы завершена ошибок нет",0,0);
   else
   module->mdb_report(REPORT_ERROR_TYPE,L"В результате проверки базы обнаружены ошибки ",0,0);

  return ret;
 }


bool       __fastcall med_db::kadr_exists(DWORD kadr_id)
{
     #if( FASTMUTEX_LOCKER != 0)
      TLockHelper l (locker);
     #else
       rd_lock    l (locker);
     #endif
  return kadrs.is_valid_kadr_ptr(kadrs.find_kadr(kadr_id));
}

DWORD    __fastcall med_db::kadrs_get_range(DWORD rec_id,entry_index & kadr_list)
{
  //Формируем список кадров в состав которых входит запись
     #if( FASTMUTEX_LOCKER != 0)
      TLockHelper l (locker);
     #else
       rd_lock    l (locker);
     #endif
  entry_index::iterator lo,hi;
  if(kadrs.get_kadr_range(rec_id,lo,hi))
  {
   while(lo<hi)
   {
     DWORD kadr_id = kadrs.entry_at(*lo)->kadr_id;
     entry_index::iterator ptr = lower_bound(kadr_list.begin(),kadr_list.end(),kadr_id,std::less<DWORD>());
     if(!(ptr<kadr_list.end() && *ptr == kadr_id))
     {
      kadr_list.insert(ptr,kadr_id);
     }
    ++lo;
   }
  }

  return kadr_list.size();
}

#pragma warn -8057
void   __fastcall med_db::archives_increment(lpotd_addr addr,bool alarm,__int64 hist_begin,__int64 hist_end,trecords & recs,trecords_index & alarms)
{
  //Наращивание кол-ва принятых архивов
  otd_param_addr lo_addr,hi_addr;
  lo_addr.addr.addr = hi_addr.addr.addr = addr->addr;
  lo_addr.addr.fa = 0; lo_addr.addr.sb = 0;
  lo_addr.param_number = 0;
  hi_addr.addr.fa = hi_addr.addr.sb = -1;
  hi_addr.param_number = -1;
  int kcc = 0;
  {
     #if( FASTMUTEX_LOCKER != 0)
      TLockHelper l (locker);
     #else
       wr_lock    l (locker);
     #endif
  trecords_index::iterator lo_rec,hi_rec;
  if(records.get_addr_range(lo_addr,hi_addr,lo_rec,hi_rec))
  {
    //Есть записи по указзанному кп
    entry_index  kadr_list;
    while(lo_rec<hi_rec)
    {
     kadrs_get_range(records.at(*lo_rec).rec_id,kadr_list);
     ++lo_rec;
    }
   entry_index::iterator kptr = kadr_list.begin();
   entry_index::iterator kend = kadr_list.end  ();
   while(kptr<kend)
   {
     kadrs_list::iterator kadr;
     if(kadrs.find_kadr(*kptr,kadr))
       {
        if(kadr->hist_beg_time != hist_begin)
        {
         kadr_state ks;
         ks.kadr_state = kadr->kadr_state;
         if(alarm)
             while(!(++ks.alarm_arch_count)) ;
            else
             while(!(++ks.arch_count)) ;
         kadr->kadr_state = ks.kadr_state;
         kadr->hist_beg_time = hist_begin;
         kadr->hist_end_time = hist_end;
         kadr_changes[kadr->kadr_id]|= MDB_KADR_FIELD_STATE|MDB_KADR_FIELDS_HIST_TIMES;
         ++kcc;
        }
       }
     ++kptr;
   }
   }
   if(kcc)
      handle_kadr_changes();
  }
}
 #pragma warn .8057








