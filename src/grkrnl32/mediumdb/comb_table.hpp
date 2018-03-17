#ifndef _COMB_TIBLE_HPP
#define _COMB_TIBLE_HPP
//#include <values.h>
#include <algorithm>
#include <functional>
#include "mediumdb.h"
#include <vector>
#include <kestrm.hpp>
#include <map>
#include <string>

  



#define COMBINE_TABLE_STREAM_VER    0


struct combine_entry
{
 DWORD comb_id;    //Идентификатор Комбинированного объекта
 DWORD obj_id;     //Идентификатор объекта входящего в состав
 combine_entry(){comb_id = obj_id = MAXINT;}
 combine_entry(DWORD cid,DWORD oid)  {comb_id = cid&MAXINT ;  obj_id= oid&MAXINT;}
 combine_entry(const combine_entry  & ce){*this = ce;}
 combine_entry & operator = (const combine_entry & ce){comb_id = ce.comb_id;obj_id = ce.obj_id;return * this;} 
};

inline int combine_entry_cmp(const combine_entry & ce1,const  combine_entry ce2,bool by_obj = false)
{
 int res = (int) by_obj ? (int)ce1.obj_id - (int)ce2.obj_id : (int)ce1.comb_id - (int)ce2.comb_id;
 if(!res)
   {
    res =  (int) by_obj ?  (int)ce1.comb_id - (int)ce2.comb_id : (int)ce1.obj_id - (int) ce2.obj_id;
   }
 return res;
}

inline bool operator < (const combine_entry & ce1,const  combine_entry &ce2)
{
 return combine_entry_cmp(ce1,ce2,false) < 0 ? true:false;
}


inline bool operator == (const combine_entry & ce1,const  combine_entry &ce2)
{
 return combine_entry_cmp(ce1,ce2,false) == 0 ? true:false;
}

inline bool operator != (const combine_entry & ce1,const  combine_entry & ce2)
{ return combine_entry_cmp(ce1,ce2) ? true:false;}



typedef std::vector<combine_entry>  combine_list;
typedef std::vector<DWORD>          obj_index;

class combine_table:protected std::map<DWORD,std::string>
{
 protected:
  combine_list    list;   // Составляющие комбинированного объекта
  obj_index       index;

 public   :
 combine_table(){};
 std::string
       __fastcall get_formula(DWORD comb_id);
 DWORD __fastcall get_stream_ver(){return COMBINE_TABLE_STREAM_VER;}
 DWORD __fastcall stream_write  (KeRTL::TStream & strm);
 DWORD __fastcall stream_read    (KeRTL::TStream & strm,DWORD ver,DWORD count);
 DWORD __fastcall create_combine_object(DWORD comb_id,char * formula);
 DWORD __fastcall set_combine_formula  (DWORD comb_id,char * formula);
 DWORD __fastcall add_object           (DWORD comb_id,DWORD obj_id);
 DWORD __fastcall remove_object        (DWORD combine_id,DWORD id_obj);
 DWORD __fastcall remove_object        (DWORD id_obj);  
 DWORD __fastcall get_entry_count      (DWORD id);
 DWORD __fastcall get_combine_id       (DWORD idx); 
 DWORD __fastcall get_entry_range      (DWORD id_combine,combine_list::iterator & low,combine_list::iterator & hi);
 DWORD __fastcall get_combine_range    (DWORD id_obj,obj_index::iterator & low,obj_index::iterator & hi);
 bool  __fastcall is_combine           (DWORD comb_id){return count(comb_id) ? true:false;}
 DWORD __fastcall clear_combine_list   (DWORD comb_id);
 bool  __fastcall remove               (DWORD id);
 DWORD __fastcall rebuild_index        ();
 combine_entry &  __fastcall get_entry (DWORD offset){ return offset<list.size() ? list.at(offset):*list.end();}
 std::map<DWORD,std::string>::size;
 void __fastcall clear(); 
    

};


#endif

