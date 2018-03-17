#pragma hdrstop
#include "comb_table.hpp"


 //TODO Может при добавлении перестраивать индексы ?
 //см rebuild index;



 DWORD __fastcall combine_table::stream_write  (KeRTL::TStream & strm)
 {
  DWORD ret = 0;
  //1 Первым делом пишем таблицу Вычисляемых объектов
  DWORD sz = size();
  strm<<sz;
  iterator beg = this->begin(),end = this->end();
  while(beg!=end)
   {
    std::string & formula = beg->second;
    strm<<beg->first;
    DWORD formula_len = formula.length();
    strm<<formula_len;
    if(formula_len)
       strm.Write((char*)formula.c_str(),formula_len);
    ++beg;
   }

  sz = list.size();
  strm<<sz;
  strm.Write(&*list.begin(),sz*sizeof(combine_list::value_type));
  sz = index.size();
  strm<<sz;
  strm.Write(&*index.begin(),sz*sizeof(obj_index::value_type));
  return  ret;
 }

 void __fastcall combine_table::clear()
 {
  this->list.clear();
  this->index.clear();
  std::map<DWORD,std::string>::clear();
 }

 #pragma warn -8057

 DWORD __fastcall combine_table::stream_read    (KeRTL::TStream & strm,DWORD ver,DWORD count)
 {
  DWORD ret = 0;
  DWORD sz;
  strm>>sz;
  this->clear();
  ret = size();
  ret +=ret;

  while(sz)
  {
   std::string formula;
   DWORD formula_len;
   DWORD id;
   strm>>id>>formula_len;
   if(formula_len)
     {
      formula.resize(formula_len+1,0);
      strm.Read(&*formula.begin(),formula_len);
     }
   this->create_combine_object(id,(char*)formula.c_str());
   --sz;
  }
  strm>>sz;
  list.resize(sz);
  strm.Read(&*list.begin(),sz*sizeof(combine_list::value_type));
  strm>>sz;
  index.resize(sz);
  strm.Read(&*index.begin(),sz*sizeof(obj_index::value_type));
  //rebuild_index();
  return  ret;
 }
 #pragma warn .8057


 DWORD __fastcall combine_table::create_combine_object(DWORD comb_id,char * formula)
 {
   if(count(comb_id)==0)
     {
      (*this)[comb_id] = std::string(formula? formula:"");
     }
   return comb_id;
 }


 DWORD __fastcall combine_table::set_combine_formula  (DWORD comb_id,char * formula)
 {
   if(count(comb_id))
     {
      std::string _formula(formula ? formula:"");
      find(comb_id)->second = _formula;
      return comb_id;
     }
  return -1;
 }

std::string 
       __fastcall combine_table::get_formula(DWORD comb_id)
       {
        std::string ret;
        if(count(comb_id))
          {
           ret =  (*this)[comb_id];
          }
        return ret;
       }

 struct comb_entry_less
 {
  bool operator ()(const combine_entry & ce1,const combine_entry & ce2){
  return  (ce1<ce2);}

 };

 struct comb_index_less
 {
   combine_entry    ce;
   combine_list   * list;
   DWORD list_size;
   comb_index_less (combine_list * _lst,combine_entry & _ce):list(_lst),ce(_ce){list_size = list->size();};
   comb_index_less (const  comb_index_less & col):list(col.list),ce(col.ce),list_size(col.list_size){};
   bool operator ()(const DWORD & off1,const DWORD & off2)
   {
     combine_entry ce1 = off1<list_size ? list->at(off1) : ce;
     combine_entry ce2 = off2<list_size ? list->at(off2) : ce;
     return  combine_entry_cmp(ce1,ce2,true)<0 ? true:false;

   }

 };

 struct modify_index:public std::unary_function<DWORD,DWORD>
 {
  bool inc;
  DWORD index;
  modify_index(DWORD _idx,bool _inc):index(_idx),inc(_inc){};
  modify_index(const modify_index & mi):index(mi.index),inc(mi.inc){};
  DWORD operator () (DWORD & x)
  {
   if(inc)
     return  x<index ? x:++x;
    else
     return  x>index ? --x:x;

  }
 };

 DWORD __fastcall combine_table::add_object           (DWORD comb_id,DWORD obj_id)
 {
   if(count(comb_id))
     {
      combine_entry ce(comb_id,obj_id);
      if(list.size())
       {
        combine_list::iterator ptr = std::lower_bound(list.begin(),list.end(),ce,comb_entry_less());
        if(ptr>=list.end() || *ptr!=ce)
        {
        ptr = list.insert(ptr,ce);
        DWORD offset = std::distance(list.begin(),ptr);
        obj_index::iterator idx_ptr = std::lower_bound(index.begin(),index.end(),-1,comb_index_less(&list,ce));
        std::transform(index.begin(),index.end(),index.begin(),modify_index(offset,true)); // увеличить индексы
        index.insert(idx_ptr,offset);
        }
       }
       else
       {
        list.insert (list.end(),ce);
        index.insert(index.end(),0);
       }
      return comb_id;
     }
  return -1;
 }

 DWORD __fastcall combine_table::remove_object        (DWORD combine_id,DWORD id_obj)
 {
  //Удаление обхекта из состава комбинированного
  DWORD   ret = 0;
  if(count(combine_id))
  {
    combine_list::iterator low,hi;
    if(get_entry_range(combine_id,low,hi))
    {
     while(low<hi)
     {
      if(low->obj_id == id_obj)
         {
          ++ret;
          obj_index::iterator i_ptr = std::lower_bound(index.begin(),index.end(),-1,comb_index_less(&list,*low));
          DWORD off1 = *i_ptr;
          #ifdef _DEBUG
          DWORD off2 = std::distance(list.begin(),low);
          if(off1!=off2)
            DebugBreak();
          #endif
          index.erase(i_ptr);
          std::transform(index.begin(),index.end(),index.begin(),modify_index(off1,false));
          list.erase(low);
          --hi;
         }
         else
         ++low;
     }
    }
  }
  return  ret;
 }

 DWORD __fastcall combine_table::clear_combine_list   (DWORD comb_id)
 {
  combine_list::iterator low,hi;
  DWORD count = get_entry_range(comb_id,low,hi);
  if(count)
  {
   obj_index obj_list;
   obj_list.reserve(count);
   while(low<hi) //строим список объектов
     {
      obj_list.push_back(low->obj_id);
      ++low;
     }
   obj_index::iterator   beg = obj_list.begin(),end = obj_list.end();
   while(beg<end)
   {
    remove_object(comb_id,*beg);
    ++beg;
   }
  }
  return count;
 }


 DWORD __fastcall combine_table::remove_object        (DWORD id_obj)
 {
  //Удаление объекта из всех комбинированных
  DWORD ret = 0;
  obj_index::iterator low,hi;
  DWORD count = get_combine_range(id_obj,low,hi);
  if(count)
  {
   obj_index comb_list;
   comb_list.reserve(count);
   //Постороение списка комбинированных,которым принадлежит удаляемый объект
   while(low<hi)
   {
    comb_list.push_back(*low);
    ++low;
   }
   low = comb_list.begin();
   hi  = comb_list.end();
   while(low<hi)
   {
    ret+=remove_object(*low,id_obj);
    ++low;
   }
  }
  return  ret;
 }


 DWORD __fastcall combine_table::get_entry_range  (DWORD id_combine,combine_list::iterator & low,combine_list::iterator & hi)
 {
   combine_entry low_entry(id_combine,0);
   combine_entry hi_entry(id_combine,MAXINT);
   low = std::lower_bound(this->list.begin(),list.end(),low_entry);
   hi  = std::upper_bound(low,list.end(),hi_entry);
   return std::distance(low,hi);
 }

 DWORD __fastcall combine_table::get_combine_range (DWORD id_obj,obj_index::iterator & low,obj_index::iterator & hi)
 {
   combine_entry  ce(0,id_obj);
   combine_entry  ce1(MAXINT,id_obj);
   low = std::lower_bound(index.begin(),index.end(),-1, comb_index_less(&list,ce));
   hi  = std::upper_bound(low,index.end(),-1, comb_index_less(&list,ce1));
   return std::distance(low,hi);
 }

 DWORD __fastcall combine_table::get_entry_count      (DWORD id)
 {
  combine_list::iterator low,hi;
  DWORD ret = get_entry_range(id,low,hi);
  return  ret;
 }

 DWORD __fastcall combine_table::get_combine_id       (DWORD idx)
 {
  DWORD ret = -1;
  if(idx<size())
   {
    iterator ptr = begin();
    advance(ptr,idx);
    ret = ptr->first;
   }
   return  ret;
 }

 bool  __fastcall combine_table::remove               (DWORD id)
 {
  if(count(id))
  {
   clear_combine_list(id);
   erase(id);
   return true;
  }
  return  false;
 }

DWORD __fastcall combine_table::rebuild_index()
{
 index.clear();
 index.reserve(list.size());
 combine_list::iterator beg = list.begin(),end = list.end();
 DWORD offset = 0;
 obj_index::iterator i_ptr;
 while(beg<end)
 {
  obj_index::iterator idx_ptr = std::lower_bound(index.begin(),index.end(),offset,comb_index_less(&list,*beg));
  index.insert(idx_ptr,offset);
  ++offset;
  ++beg;
 }
 return index.size();
}











