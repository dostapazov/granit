#ifndef _RECORD_TABLE_HPP
#define _RECORD_TABLE_HPP

//#include <mediumdb.h>
#include <otd_proto_medium.h>
#include <otd_diag.h>
#include <vector>
#include <kestrm.hpp>

#define RECORD_TABLE_STREAM_VER0 0
#define RECORD_TABLE_STREAM_VER1 1
#define RECORD_TABLE_STREAM_VER2 2
#define RECORD_TABLE_STREAM_VER  3

typedef std::vector<mdb_record> trecords;
typedef std::vector<DWORD>      trecords_index;

class records_table
{
 protected:
 DWORD             id_gen;
 trecords          table;
 trecords_index    addr_index;
 DWORD            __fastcall generate_id(){return ++id_gen;}
 void             __fastcall release();
 trecords_index::iterator
                  __fastcall lower_bound_addr(otd_param_addr & pa);
 trecords_index::iterator
                  __fastcall upper_bound_addr(otd_param_addr & pa);

 public :
  records_table():id_gen(0){};
  ~records_table(){release();}
  DWORD          __fastcall   create(otd_param_addr & addr);
  DWORD          __fastcall   remove(DWORD id);
  DWORD          __fastcall   insert(MDB_RECORD & rec);
  DWORD          __fastcall   size()            {return table.size();}
  mdb_record &   __fastcall   at (DWORD offset) {if(offset<table.size()) return table.at(offset);else return * table.end();}
  mdb_record &   __fastcall   at (DWORD offset,bool ByAddr);
  mdb_record &   __fastcall   get_record(DWORD id);
  mdb_record &   __fastcall   get_record(otd_param_addr & pa);

  DWORD          __fastcall   find_by_addr  (otd_addr & addr,DWORD pn);
  DWORD          __fastcall   find_by_addr  (otd_param_addr & pa);
  DWORD          __fastcall   find_by_id    (DWORD id);
  DWORD          __fastcall   get_addr_range(otd_param_addr & low_addr,otd_param_addr & hi_addr,trecords_index::iterator &low,trecords_index::iterator &hi);
  DWORD          __fastcall   get_count   (otd_addr & addr);
  DWORD          __fastcall   enum_objects(otd_addr & addr,DWORD idx);
  DWORD          __fastcall   enum_objects_II(otd_addr & addr,DWORD idx);
  DWORD          __fastcall   get_stream_ver(){return RECORD_TABLE_STREAM_VER;}
  DWORD          __fastcall   stream_write(KeRTL::TStream & strm);
  DWORD          __fastcall   stream_read (KeRTL::TStream & strm,DWORD ver,DWORD count);
  void           __fastcall   clear(){table.clear();addr_index.clear();id_gen = 0;}
  DWORD          __fastcall   rebuild_indexes();
  void           __fastcall   sort_indexes   ();
  bool           __fastcall   check_indexes();
  DWORD          __fastcall   get_idgen(){return id_gen;}

};

 inline DWORD          __fastcall   records_table::find_by_addr  (otd_addr & addr,DWORD pn)
 {
  sotd_param_addr pa(addr,pn);
  return find_by_addr(pa);
 }

 struct  rec_less
 {
   records_table * rt;
   sotd_param_addr pa;
   DWORD           rt_size;
   rec_less  (records_table * _rt):rt(_rt){rt_size = rt->size();}
   rec_less  (records_table * _rt,sotd_param_addr _pa):rt(_rt),pa(_pa){rt_size = rt->size();}
   rec_less  (const rec_less & ridl){*this = ridl;}
   rec_less&  operator = (const rec_less & ridl) {rt = ridl.rt; pa = ridl.pa; rt_size = ridl.rt_size; return *this;}
   bool operator ()(const DWORD & off1,const DWORD & off2)
   {

    sotd_param_addr pa1 = ((off1<rt_size) ? rt->at(off1).addr : pa);
    sotd_param_addr pa2 = ((off2<rt_size) ? rt->at(off2).addr : pa);
    if(pa1==pa2 )
     {
      if(off1<rt_size && off2<rt_size)
      {
      DWORD id1 =  rt->at(off1).rec_id ;
      DWORD id2 =  rt->at(off2).rec_id ;
      return id1<id2 ? true:false;
      }
      return false;
     }
     else
     if(pa1<pa2)
       return true;
     return  false;
   }
 };




#endif

 