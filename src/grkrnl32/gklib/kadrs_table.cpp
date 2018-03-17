#pragma hdrstop
#include <kadrs_table.hpp>
#include <algorithm>
#include <list>

   kadrs_list::iterator      __fastcall kadrs_table::create_kadr()
   {
    mdb_kadr mk;
    return  create_kadr(mk);
   }


   void                  __fastcall kadrs_table::clear()
   {
    this->kadrs.clear();
    this->entry_table.clear();
    this->index.clear();
    this->gen_kadrid = 0;
   }

   kadrs_list::iterator      __fastcall kadrs_table::create_kadr(MDB_KADR & kdr)
   {
     kdr.kadr_id = ++gen_kadrid;
     return kadrs.insert(std::lower_bound(kadrs.begin(),kadrs.end(),kdr),kdr);
   }

   kadrs_list::iterator      __fastcall kadrs_table::find_kadr(DWORD id)
   {
     mdb_kadr kdr(id,-1,-1);
     kadrs_list::iterator ptr = kadrs.begin(),end = kadrs.end();
     ptr = std::lower_bound(ptr,end,kdr);
     if(ptr<end && ptr->kadr_id == id)
       return ptr;
     return end;
   }

   DWORD           __fastcall kadrs_table::enum_kadr(DWORD idx)
   {
    mdb_kadr kdr;
    return enum_kadr(idx,kdr);
   }

   DWORD           __fastcall kadrs_table::enum_kadr(DWORD idx,MDB_KADR & kdr)
   {
    DWORD ret = -1;
    if(idx<kadrs.size())
      {
       kdr = kadrs.at(idx);
       ret = kdr.kadr_id;
      }
    return ret;
   }

   bool            __fastcall kadrs_table::remove_kadr(DWORD id)
   {
     bool ret = false;
     //Удаление кадра;
     //Удалить из списка
     //удалить все элементы из таблицы соответсвующие кадру
     //Перестроить индексы
     kadrs_list::iterator kadr_ptr = find_kadr(id);
     if(is_valid_kadr_ptr(kadr_ptr))
     {
      ret = true;
      kadr_clear(id);
      kadrs.erase(kadr_ptr);
     }
     return ret;
   }

   kadr_entryes_table::iterator __fastcall kadrs_table::add_entry (DWORD id,DWORD rec_id)
   {
    mdb_kadr_entry ke(id,rec_id,-1);
    return add_entry(ke);
   }

   kadr_entryes_table::iterator __fastcall kadrs_table::add_entry (MDB_KADR_ENTRY & ke)
   {
    kadr_entryes_table::iterator  low = entry_table.end(),hi;
    kadrs_list::iterator kadr = find_kadr( ke.kadr_id);
    if(is_valid_kadr_ptr(kadr))
    {

     DWORD count = this->get_entry_range(ke.kadr_id,low,hi);
     if(!ke.size.cx) ke.size.cx = 20;
     if(!ke.size.cy) ke.size.cy = 20;
     if(!ke.options.color && !ke.options.bkgnd_color) ke.options.color = 0xFFFF;
     if(ke.number==(DWORD)-1)
        ke.number = count;
      low = std::lower_bound  (low,hi,ke);
      low = entry_table.insert(low,ke);
      DWORD offset = distance(entry_table.begin(),low);
      transform(index.begin(),index.end(),index.begin(),inc_index(offset));
      entry_index::iterator i_ptr = std::lower_bound(index.begin(),index.end(),offset,kadr_entry_less_by_recid(&entry_table,*low));
      index.insert(i_ptr,offset);
      ++kadr->entry_count;
    }
    return low;
   }

   kadr_entryes_table::iterator __fastcall kadrs_table::find_entry(DWORD kid,DWORD id,DWORD number)
   {
    mdb_kadr_entry ke(kid,id,number);
    return find_entry(ke);
   }

   kadr_entryes_table::iterator __fastcall kadrs_table::find_entry(MDB_KADR_ENTRY & ke)
   {
     kadr_entryes_table::iterator  ptr = std::lower_bound  (entry_table.begin(),entry_table.end(),ke);
     if(*ptr != ke)
     ptr = entry_table.end();
     return ptr;
   }

   DWORD           __fastcall kadrs_table::enum_entry(DWORD kid,DWORD idx)
   {
    mdb_kadr_entry ke;
    return enum_entry(kid,idx,ke);
   }

   DWORD           __fastcall kadrs_table::enum_entry(DWORD kid,DWORD idx,MDB_KADR_ENTRY & ke)
   {
     kadr_entryes_table::iterator  low,hi;
     DWORD count = get_entry_range(kid,low,hi);
     if(idx<count)
     {
      std::advance(low,idx);
      ke = *low;
      return ke.rec_id;
     }
     return -1;
   }

   bool            __fastcall kadrs_table::remove_entry(MDB_KADR * kdr,MDB_KADR_ENTRY & entry)
   {
    bool ret = false;
    if(kdr)
    {
      kadrs_list::iterator kadr = find_kadr(kdr->kadr_id);
        if(is_valid_kadr_ptr(kadr))
        {
          kadr_entryes_table::iterator _entry = find_entry(entry);
          if(_entry<this->entry_table.end())
          {
           ret = true;
           --kadr->entry_count;
           DWORD offset = std::distance(entry_table.begin(),_entry);
           entry_index::iterator i_ptr = std::lower_bound(index.begin(),index.end(),offset,kadr_entry_less_by_recid(&entry_table,*_entry));
           entry_table.erase(_entry);
           index.erase(i_ptr);
           std::transform(index.begin(),index.end(),index.begin(),dec_index(offset));
          }
        }
    }
    return ret;
   }

   bool                  __fastcall kadrs_table::remove_entry(DWORD rec_id)
   {
    //Удаление записи из всех кадров
    bool ret = false;
    entry_index::iterator lo,hi;
    DWORD count = get_kadr_range(rec_id,lo,hi);
    if(count)
    {
     ret = true;
     kadr_entryes_table  remove_list;
     remove_list.reserve(count);
     while(lo<hi)
      {
       mdb_kadr_entry & entry = entry_table.at(*lo);
       if(entry.rec_id == rec_id)
          remove_list.push_back( entry );
       ++lo;
      }

     kadr_entryes_table::iterator beg = remove_list.begin(),end = remove_list.end();
     kadrs_list::iterator kadr = kadrs.end();
     while(beg<end)
     {
      if(!is_valid_kadr_ptr(kadr) || kadr->kadr_id!= beg->kadr_id)
         kadr = find_kadr(beg->kadr_id);
      remove_entry(&*kadr,*beg);
      ++beg;
     }

    }
    return ret;
   }

   DWORD         __fastcall kadrs_table::kadr_clear(DWORD kadr_id)
   {
      kadr_entryes_table::iterator lo,hi;
      DWORD ret = get_entry_range(kadr_id,lo,hi);
      if(ret)
      {
       kadrs_list::iterator kadr = find_kadr(kadr_id);
       kadr->entry_count = 0;
       entry_table.erase(lo,hi);
       build_index();
      }
     return ret;
   }

   DWORD           __fastcall kadrs_table::get_entry_range(DWORD kadr_id,kadr_entryes_table::iterator & low,kadr_entryes_table::iterator & hi)
   {
    /*Получение диапазона записей кадра*/
    low = entry_table.begin(),hi = entry_table.end();
    mdb_kadr_entry lo_entry(kadr_id,0,0);
    mdb_kadr_entry hi_entry(kadr_id,-1,-1);
    low = std::lower_bound(low,hi,lo_entry);
    hi  = std::upper_bound(low,hi,hi_entry);
    return distance(low,hi);
   }

   DWORD           __fastcall kadrs_table::get_entry_range(DWORD kadr_id,DWORD rec_id,kadr_entryes_table::iterator & low,kadr_entryes_table::iterator & hi)
   {
    /*Получение диапазона записей кадра*/
    low = entry_table.begin(),hi = entry_table.end();
    mdb_kadr_entry lo_entry(kadr_id,rec_id,0);
    mdb_kadr_entry hi_entry(kadr_id,rec_id,-1);
    low = std::lower_bound(low,hi,lo_entry);
    hi  = std::upper_bound(low,hi,hi_entry);
    return distance(low,hi);
   }

   DWORD           __fastcall kadrs_table::get_kadr_range (DWORD rec_id,entry_index::iterator & low,entry_index::iterator & hi)
   {
    mdb_kadr_entry lo_entry (0,rec_id,0);
    mdb_kadr_entry hi_entry (-1,rec_id,-1);
    low = index.begin(),hi = index.end();
    low = std::lower_bound(low,hi,-1,kadr_entry_less_by_recid(&entry_table,lo_entry));
    hi  = std::upper_bound(low,hi,-1,kadr_entry_less_by_recid(&entry_table,hi_entry));
    return distance(low,hi);

   }


   DWORD __fastcall    kadrs_table::build_index()
   {
    //Построение индексов

    DWORD size = entry_table.size();
    index.clear();
    index.reserve(size);
    DWORD offset = 0;
    mdb_kadr_entry _entry;
    kadr_entry_less_by_recid __comp(&entry_table,_entry);
    while(offset<size)
    {
     entry_index::iterator i_ptr = std::lower_bound(index.begin(),index.end(),offset,__comp);
     index.insert(i_ptr,offset);
     ++offset;
    }
    return size;
   }

   kadr_entryes_table::iterator      __fastcall kadrs_table::entry_at    (DWORD offset)
   {
    kadr_entryes_table::iterator ptr = entry_table.begin();
    if(offset<entry_table.size())
       std::advance(ptr,offset);
       else
       ptr = entry_table.end();
       return ptr;
    }

   DWORD                 __fastcall kadrs_table::stream_write  (KeRTL::TStream & strm)
   {
    DWORD ret;
    strm<<this->gen_kadrid;
    strm<<kadrs.size();
    strm<<entry_table.size();
    ret =strm.Write(&*kadrs.begin(),kadrs.size()*sizeof(kadrs_list::value_type));
    ret+=strm.Write(&*entry_table.begin(),entry_table.size()*sizeof( kadr_entryes_table::value_type));
    ret+=strm.Write(&*index.begin(),index.size()*sizeof(entry_index::value_type));
    return ret;
   }

   DWORD __fastcall read_kadrs_ver0(KeRTL::TStream & strm,kadrs_list::iterator beg,kadrs_list::iterator end)
   {
     DWORD ret = 0;
     DWORD rd_size = sizeof(MDB_KADR);
     rd_size -= (sizeof(beg->hist_beg_time)<<1);
     while(beg<end)
     {
       mdb_kadr &kadr = *beg;
       ret+=strm.Read(&kadr,rd_size);
       kadr.hist_beg_time = kadr.hist_end_time = 0;
       ++beg;
     }
     return ret;
   }

   int __fastcall read_entryes_ver0(KeRTL::TStream & strm,kadr_entryes_table::iterator beg ,kadr_entryes_table::iterator end)
   {
     int ret = 0;
     DWORD rd_size = sizeof(MDB_KADR_ENTRY);
     rd_size -= (sizeof(beg->state));
     while(beg<end)
     {
       MDB_KADR_ENTRY & entry = *beg;
       ret += strm.Read(&entry,rd_size);
       ++beg;
     }

     return ret;
   }

   #pragma warn -8057
   DWORD                 __fastcall kadrs_table::stream_read    (KeRTL::TStream & strm,DWORD ver,DWORD count)
   {
    DWORD ret;
    DWORD kadrs_size = 0, entry_count = 0;
    strm>>gen_kadrid>>kadrs_size>>entry_count;

    kadrs.resize(kadrs_size);
    entry_table.resize(entry_count);
    index.resize(entry_count);
    switch(ver)
    {
     case KADRS_TABLE_STREAM_VER_0 : ret = read_kadrs_ver0(strm,kadrs.begin(),kadrs.end());
     break;
     default:
     ret = strm.Read (&*kadrs.begin(),kadrs.size()*sizeof(kadrs_list::value_type));
     break;
    }
    switch(ver)
    {
     case KADRS_TABLE_STREAM_VER_0:
     case KADRS_TABLE_STREAM_VER_1:
          ret += read_entryes_ver0(strm,entry_table.begin(),entry_table.end());
     break;
     default:
     ret+=strm.Read (&*entry_table.begin(),entry_table.size()*sizeof( kadr_entryes_table::value_type));
     break;
    }
    ret+=strm.Read (&*index.begin(),index.size()*sizeof(entry_index::value_type));
    sort(entry_table.begin(),entry_table.end());
    build_index();

    kadrs_list::iterator k_ptr = kadrs.begin(),end = kadrs.end();
    while(k_ptr<end)
    {
     kadr_entryes_table::iterator lo,hi;
     k_ptr->entry_count = this->get_entry_range(k_ptr->kadr_id,lo,hi);
     k_ptr->kadr_diag   = MDBR_DIAG_BAD;
     k_ptr->discrete_changes = k_ptr->alarm_changes = k_ptr->kadr_changes = 0;
     k_ptr->kadr_state = 0;
     ++k_ptr;
    }
    /*
    entry_index::iterator l,h;
    DWORD kadr_id ;
    get_kadr_range(33,l,h);
    kadr_id  = *l;
    get_kadr_range(33,l,h);
    kadr_id  = *l;
    get_kadr_range(222,l,h);
    kadr_id  = *l;
    */
    return ret;
   }
#pragma warn .8057



