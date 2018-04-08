#pragma hdrstop
#include <record_table.hpp>
#include <algorithm>
#include <functional>
#include <limits>

 void           __fastcall records_table::release()
 {
 }

 DWORD          __fastcall records_table::create(otd_param_addr & addr)
 {
   mdb_record rec(addr);
   return insert(rec);
 }


 DWORD    __fastcall records_table::insert(MDB_RECORD & rec)
 {
   //Добавление новой записи  Возможно дублирование по адресам
   DWORD ret ;
   //if(!rec.rec_id || rec.rec_id == (DWORD)-1)
   rec.rec_id = generate_id();
   if(rec.addr.addr.fa == 0 || (rec.addr.param_number==(DWORD)-1 && !(rec.options&MDBR_OPT_COMBINED)))
      {
       rec.state|=MDBR_STATE_DISCRETE;
      }
   DWORD offset = size();

   table.insert(table.end(),rec);

   trecords_index::iterator i_ptr = std::lower_bound(this->addr_index.begin(),addr_index.end(),offset,rec_less(this));
   addr_index.insert(i_ptr,offset);
   ret  = rec.rec_id;
  return ret;
 }


  DWORD          __fastcall   records_table::remove(DWORD id)
  {

   DWORD offset = this->find_by_id(id);
   if(offset<size())
   {
    trecords::iterator ptr = table.begin();
    std::advance(ptr,offset);
    trecords_index::iterator i_ptr = std::lower_bound(this->addr_index.begin(),addr_index.end(),offset,rec_less(this));
    table.erase(ptr);
    addr_index.erase(i_ptr);
    std::transform(addr_index.begin(),addr_index.end(),addr_index.begin(),dec_index(offset));
    return  id;
   }
   return -1;
  }

  DWORD __fastcall records_table::rebuild_indexes()
  {
   addr_index.clear();
   DWORD count = table.size();
   addr_index.reserve(count);
   trecords::iterator r_ptr = table.begin(),re_ptr = table.end();
   DWORD offset = 0;
   while(r_ptr<re_ptr)
   {
   trecords_index::iterator i_ptr = std::lower_bound(this->addr_index.begin(),addr_index.end(),offset,rec_less(this));
   addr_index.insert(i_ptr,offset);
   ++offset;
   ++r_ptr;
   }
   return addr_index.size();
  }

 void    __fastcall   records_table::sort_indexes   ()
 {
   std::sort(addr_index.begin(),addr_index.end(),rec_less(this));
 }


  bool           __fastcall   records_table::check_indexes()
  {
    bool ret = true;

    trecords::iterator end_ptr = table.end();
    trecords::iterator r1_ptr = end_ptr;
    trecords::iterator r2_ptr = end_ptr;
    if(table.size()== addr_index.size())
    {
     DWORD count = table.size();
	 for(DWORD i = 0;ret && i<count;i++)
      {
       r2_ptr = table.begin();
       std::advance(r2_ptr,addr_index.at(i));
       if(r1_ptr!=end_ptr)
       {
        if(r1_ptr != r2_ptr)
         {
          sotd_param_addr a1 = r1_ptr->addr,a2 = r2_ptr->addr;
          int cmp_res = otd_addr_cmp_part(&a1.addr,&a2.addr,sizeof(otd_addr));
          if(cmp_res>0)
           {
            DebugBreak();
            //Записи не правильно отсортированы
            //Ошибка в сравнении адресов
            ret = false;
           }
           else
           {
             if(cmp_res==0 && a1.param_number>a2.param_number)
             {
              DebugBreak();
            //Записи не правильно отсортированы
            //Ошибка в сравнении адресов в части номера параметра
                ret = false;
            }
           }
         }
         else
         {
          DebugBreak();
          // дубляж записей
          ret = false;
         }
       }
       r1_ptr = r2_ptr;
      }
    }
    else
    {
     DebugBreak();
     //Несовпадение размера записей и индексов
      ret = false;
    }
    return ret;
  }


  mdb_record &   __fastcall   records_table::at(DWORD offset,bool ByAddr)
  {
    if(ByAddr)
      offset = addr_index[offset];
   return at(offset);
  }

  DWORD          __fastcall   records_table::find_by_id    (DWORD id)
  {
   DWORD ret = -1;
   mdb_record rec;
   rec.rec_id = id;
   trecords::iterator ptr = std::lower_bound(table.begin(),table.end(),rec,mdb_record_id_less());
   if(ptr<table.end() && ptr->rec_id == id)
      ret = std::distance(table.begin(),ptr);

   return ret;
  }

  mdb_record &   __fastcall   records_table::get_record(DWORD id)
  {
   DWORD offset = find_by_id(id);
   return at(offset);
  }

  mdb_record &   __fastcall   records_table::get_record(otd_param_addr & pa)
  {
   DWORD offset = find_by_addr(pa);
   return at(offset);
  }


  trecords_index::iterator  __fastcall records_table::lower_bound_addr(otd_param_addr & pa)
  {
   trecords_index::iterator ret = addr_index.end();
   ret = std::lower_bound(addr_index.begin(),ret,-1,rec_less(this,pa));
   return ret;
  }

  trecords_index::iterator  __fastcall records_table::upper_bound_addr(otd_param_addr & pa)
  {
   trecords_index::iterator ret = addr_index.end();
   ret = std::upper_bound(addr_index.begin(),ret,-1,rec_less(this,pa));
   return ret;
  }

  DWORD          __fastcall   records_table::find_by_addr  (otd_param_addr & pa)
  {
    DWORD ret = -1;
    trecords_index::iterator i_ptr = lower_bound_addr(pa);
    if(i_ptr<addr_index.end())
      {
       if(at(*i_ptr).addr == pa)
         ret=*i_ptr;
      }
    return ret;
  }

  DWORD          __fastcall   records_table::get_addr_range(otd_param_addr & low_addr,otd_param_addr & hi_addr,trecords_index::iterator &low,trecords_index::iterator &hi)
  {
     low = lower_bound_addr(low_addr);
     hi  = upper_bound_addr(hi_addr);
     return std::distance(low,hi);
  }

KeRTL::TStream & operator <<(KeRTL::TStream & s,otd_param_addr  t){s.Write(&t,sizeof(t));return s;}
KeRTL::TStream & operator >>(KeRTL::TStream & s,otd_param_addr & t){s.Read(&t,sizeof(t));return s;}

  /*Перегруженный оператор записи в поток*/
  KeRTL::TStream & operator <<(KeRTL::TStream & strm,mdb_record & rec)
  {

     strm<<rec.rec_id     <<rec. addr       <<rec. value     <<rec. diag       <<rec. state     <<rec. time
         <<rec.otd_val    <<rec.otd_pd      <<rec. options   <<rec. scale_id   <<rec. percent   <<rec. name
         <<rec. sname     <<rec. last_value <<rec. last_time <<rec. security
         <<rec.kvants     <<rec. min_value  <<rec. max_value
         <<rec.min_danger <<rec.max_danger <<rec.min_alarm <<rec.max_alarm;

   return strm;
  }

  /*Перегруженный оператор чтения из потока*/
  KeRTL::TStream & operator >>(KeRTL::TStream & strm,mdb_record & rec)
  {
     strm>>rec.rec_id     >>rec. addr       >>rec. value     >>rec. diag       >>rec. state     >>rec. time
         >>rec.otd_val    >>rec.otd_pd      >>rec. options   >>rec. scale_id   >>rec. percent   >>rec. name
         >>rec. sname     >>rec. last_value >>rec. last_time >>rec. security
         >>rec.kvants     >>rec. min_value  >>rec. max_value
         >>rec.min_danger >>rec.max_danger >>rec.min_alarm >>rec.max_alarm;
   return strm;
  }

  void __fastcall record_read_v0(KeRTL::TStream & strm,mdb_record & rec)
  {
    float k_value,off_value;
    strm >>rec.rec_id     >>rec. addr    >>rec. value    >>rec. diag       >>rec. state     >>rec. time
         >>rec.otd_val    >>rec.otd_pd   >>rec. options  >> k_value >> off_value >>rec. scale_id
         >>rec. percent   >>rec. name    >>rec. sname    >>rec. last_value >>rec. last_time >>rec. security
         >>rec. min_value >>rec. max_value;
   rec.kvants = 250;
   rec.set_scale(off_value,off_value+k_value*rec.kvants,(DWORD)rec.kvants);
  }

  void __fastcall record_read_v1(KeRTL::TStream & strm,mdb_record & rec)
  {
    float k_value,off_value;

    strm >>rec.rec_id     >>rec. addr    >>rec. value    >>rec. diag       >>rec. state     >>rec. time
         >>rec.otd_val    >>rec.otd_pd   >>rec. options  >>k_value    >> rec.kvants    >>off_value >>rec. scale_id
         >>rec. percent   >>rec. name    >>rec. sname    >>rec. last_value >>rec. last_time >>rec. security
         >>rec. min_value >>rec. max_value;
         rec.set_scale(off_value,off_value+k_value*(float)rec.kvants,rec.kvants);
         rec.max_alarm = rec.max_danger = rec.max_value;
         rec.min_alarm = rec.min_danger = rec.min_value;

  }

  void __fastcall record_read_v2(KeRTL::TStream & strm,mdb_record & rec)
  {
     DWORD otd_val;
     strm>>rec.rec_id     >>rec. addr       >>rec. value     >>rec. diag       >>rec. state     >>rec. time
         >>otd_val    >>rec.otd_pd      >>rec. options   >>rec. scale_id   >>rec. percent   >>rec. name
         >>rec. sname     >>rec. last_value >>rec. last_time >>rec. security
         >>rec.kvants     >>rec. min_value  >>rec. max_value
         >>rec.min_danger >>rec.max_danger >>rec.min_alarm >>rec.max_alarm;
         rec.otd_val = otd_val;
  }





  DWORD          __fastcall   records_table::stream_write(KeRTL::TStream & strm)
  {
    if(!check_indexes())
        rebuild_indexes();

    DWORD ret = strm.Write(&id_gen,sizeof(id_gen));
    trecords::iterator beg = table.begin(),end = table.end();
    while(beg<end)
    {
     strm<<*beg;
     ++beg;
    }

    //ret+=strm.Write(table.begin(),table.size()*sizeof(records::value_type));
    ret+=strm.Write(&*addr_index.begin(),addr_index.size()*sizeof(trecords_index::value_type));
    return ret;

  }

 #pragma warn -8057
  DWORD          __fastcall   records_table::stream_read (KeRTL::TStream & strm,DWORD ver,DWORD count)
  {

   DWORD ret ;
   clear();
   table.resize(count,trecords::value_type());
   addr_index.resize(count,trecords_index::value_type());
   strm>>id_gen;
   trecords::iterator beg ,end;

   beg = table.begin(),end = table.end();
   while(beg<end)
   {

     if(ver < RECORD_TABLE_STREAM_VER)
       {
        switch(ver)
        {
         case RECORD_TABLE_STREAM_VER0 :record_read_v0(strm,*beg);break;
         case RECORD_TABLE_STREAM_VER1 :record_read_v1(strm,*beg);break;
         case RECORD_TABLE_STREAM_VER2 :record_read_v2(strm,*beg);break;
        }
       }
     else
       strm>>*beg;

       ++beg;
   }

   //ret = strm.Read(table.begin(),count*sizeof(records::value_type));
   ret = strm.Read(&*addr_index.begin(),count*sizeof(trecords_index::value_type));


   beg = table.begin(),end = table.end();
   while(beg<end)
        {
         if(beg->is_pseudo())
         {
          beg->diag    = MDBR_DIAG_GOOD;
          beg->otd_pd  = MDBR_DIAG_GOOD;

         }
         else
          {
           beg->diag = MDBR_DIAG_BAD;
           beg->otd_pd     = OTD_PDIAG_NODATA;
          }

         beg->state&=MDBR_STATE_CONST_MASK;

         if(beg->addr.addr.fa == 0 || (beg->addr.param_number==(DWORD)-1 && !(beg->options&MDBR_OPT_COMBINED)))
            {
             beg->state|=MDBR_STATE_DISCRETE;
            }
         beg->last_value = beg->value;
         beg->last_time  = beg->time;
         if(!(beg->options&MDBR_OPT_DANGER))
            beg->min_danger = beg->max_danger = std::numeric_limits<float>::infinity();
         if(!(beg->options&MDBR_OPT_ALARM))
            beg->min_alarm = beg->max_alarm = std::numeric_limits<float>::infinity();
         ++beg;
        }
         if(!check_indexes())
         {
          rebuild_indexes();
          check_indexes();
         }
         return ret;
  }
#pragma warn .8057

  DWORD          __fastcall   records_table::get_count   (otd_addr & addr)
  {
     sotd_param_addr lpa(addr,0),hpa(addr,-1);
     if(lpa.addr.pu == OTD_ADDR_MAXVALUE) lpa.addr.pu = 0;
     if(lpa.addr.cp == OTD_ADDR_MAXVALUE) lpa.addr.cp = 0;
     if((lpa.addr.fa&OTD_FA_ALL) == OTD_FA_ALL) lpa.addr.fa = 0,hpa.addr.fa = OTD_ADDR_MAXVALUE;
     if(lpa.addr.sb == OTD_ADDR_MAXVALUE) lpa.addr.sb = 0;
     trecords_index::iterator lptr,hptr;
     DWORD ret = 0;
     DWORD type = otd_addr_type(&addr);
     DWORD type_diff;

     if(get_addr_range(lpa,hpa,lptr,hptr))
       {
         while(lptr<hptr)
         {
          mdb_record & rec = at(*lptr);
          type_diff = otd_addr_type(&rec.addr.addr);
          switch(type)
          {
           case OTD_ADDR_TYPE_BROADCAST: if(type_diff == OTD_ADDR_TYPE_PU) ++ret;break;
           case OTD_ADDR_TYPE_PU       : if(type_diff == OTD_ADDR_TYPE_CP) ++ret;break;
           case OTD_ADDR_TYPE_CP       : if(type_diff == OTD_ADDR_TYPE_SB) ++ret;break;
           case OTD_ADDR_TYPE_SB       : if(type_diff == OTD_ADDR_TYPE_SB && rec.addr.param_number!=(DWORD)-1) ++ret;break;
          }
          ++lptr;
         }
       }
     return ret;
  }

  DWORD          __fastcall   records_table::enum_objects(otd_addr & addr,DWORD idx)
  {
     sotd_param_addr lpa(addr,0),hpa(addr,-1);
     if(lpa.addr.pu == OTD_ADDR_MAXVALUE) lpa.addr.pu = 0;
     if(lpa.addr.cp == OTD_ADDR_MAXVALUE) lpa.addr.cp = 0;
     if((lpa.addr.fa&OTD_FA_ALL) == OTD_FA_ALL) lpa.addr.fa = 0,hpa.addr.fa = OTD_ADDR_MAXVALUE;
     if(lpa.addr.sb == OTD_ADDR_MAXVALUE) lpa.addr.sb = 0;
     trecords_index::iterator lptr,hptr;
     DWORD ret = -1;
     DWORD type = otd_addr_type(&addr);
     DWORD type_diff;
     DWORD passed = 0;

     if(get_addr_range(lpa,hpa,lptr,hptr))
       {
         while(lptr<hptr && idx>=passed)
         {
          mdb_record & rec = at(*lptr);
          ret = rec.rec_id;
          type_diff = otd_addr_type(&rec.addr.addr);

          switch(type)
          {
           case OTD_ADDR_TYPE_BROADCAST: if(type_diff == OTD_ADDR_TYPE_PU)
                                          ++passed;
                                          break;
           case OTD_ADDR_TYPE_PU       : if(type_diff == OTD_ADDR_TYPE_CP)
                                           ++passed;
                                            break;
           case OTD_ADDR_TYPE_CP       : if(type_diff == OTD_ADDR_TYPE_SB && rec.addr.param_number == (DWORD)-1)
                                            ++passed;
                                            break;
           case OTD_ADDR_TYPE_SB       : if(type_diff == OTD_ADDR_TYPE_SB && rec.addr.param_number!=(DWORD)-1)
                                            ++passed;
                                            break;
          }
          ++lptr;
         }
         if(idx>=passed)
           ret = -1;
       }
     return ret;
  }

  
  DWORD          __fastcall   records_table::enum_objects_II(otd_addr & addr,DWORD idx)
  {
    DWORD ret ;
    return enum_objects(addr,idx);
    /*Старый алгоритм перечисления т.к. новый плохо фурычит*/
    /*
    sotd_param_addr pa(addr,-1);
    DWORD  type = otd_addr_type(&addr);

    if(type == OTD_ADDR_TYPE_SB || type == OTD_ADDR_TYPE_CP)
       return enum_objects(addr,idx);

    DWORD  passed = 0;                                                   `
    LPBYTE addr_byte = &pa.addr.pu;
    if(type<OTD_ADDR_TYPE_SB)
       addr_byte+=(type+1);
    *addr_byte = 0;
    if(type == OTD_ADDR_TYPE_CP)
      {
       *(addr_byte+1)=0;
      }
    while(passed<=idx && *addr_byte<OTD_ADDR_MAXVALUE)
    {
     DWORD offset = *lower_bound_addr(pa);
     if(offset<size())
     {
      mdb_record & rec = this->at(offset);
      if(pa == rec.addr)
       {
        if(++passed<=idx)
          ++(*addr_byte);
          else
          return  rec.rec_id;
       }
       else
       {
        LPBYTE src_addr_byte = &rec.addr.addr.pu;
        if(type<OTD_ADDR_TYPE_SB)
          src_addr_byte+=(type+1);
        *addr_byte = *src_addr_byte;
        if(type == OTD_ADDR_TYPE_CP)
          *(addr_byte+1)=*(src_addr_byte+1);

       }
     }
     else
     break;
    }
    return  ret;
    */
  }

