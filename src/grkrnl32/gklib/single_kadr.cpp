#pragma hdrstop
#include <kadrs_table.hpp>
#include <algorithm>
#include <stdlib_utils.hpp>



 inline void         __fastcall  unselect_entry(mdb_kadr_entry & e)
 {
   e.set_selected(false);

 }

 void         __fastcall single_kadr::resort()
 {
   std::sort(entryes.begin(),entryes.end());
   if(_position_index_enabled) this->position_index_resort();
   selections_restore();
 }

 void         __fastcall single_kadr::clear_selections()
  {
   if(selected.size())
      selected.clear();
   std::for_each(entryes.begin(),entryes.end(),unselect_entry);
  }

 int          __fastcall single_kadr::selections_restore(bool from_sel_list )
 {
   if(from_sel_list)
   {
     entry_index::iterator sptr = selected.begin();
     entry_index::iterator send = selected.end  ();
     while(sptr<send)
     {
       DWORD offset = *sptr;
       mdb_kadr_entry * entry = get_by_offset(offset);
       if(entry) entry->set_selected(true);
       sptr++;
     }
   }
    selected.clear();
    for(unsigned i = 0;i<entryes.size();i++)
    {
        if(entryes.at(i).is_selected())
         selected.push_back(i);
    }
   return sel_count();
 }


 bool         __fastcall single_kadr::is_selected(kadr_entryes_table::iterator ptr)
 {
  return ptr->kadr_id == this->kadr.kadr_id && ptr<entryes.end() ? ptr->is_selected() : false;
 }

 bool         __fastcall single_kadr::is_selected(DWORD offset)
 {
  if(offset<this->entryes.size())
    return entryes.at(offset).is_selected();
  return false;
 }

    DWORD        __fastcall single_kadr::select(kadr_entryes_table::iterator ptr)
     {
          if( !ptr->is_selected())
          {
              ptr->set_selected(true);
              selected.push_back(distance(entryes.begin(),ptr));
          }
        return sel_count();
     }



 DWORD        __fastcall single_kadr::select(DWORD offs)
     {

       if(offs<entryes.size() )
         {
          kadr_entryes_table::iterator ptr =   entryes.begin()+ offs;
          return select(ptr);
         }

       return sel_count();
 }


     DWORD        __fastcall single_kadr::deselect(DWORD offset)
     {
       if(offset<entryes.size())
          {
           kadr_entryes_table::iterator ptr ;
           if(get_by_offset(offset,ptr))
           {
            selected.erase(find(selected.begin(),selected.end(),offset));
            ptr->set_selected(false);
           }
          }
       return sel_count();
     }


     DWORD        __fastcall single_kadr::deselect(kadr_entryes_table::iterator ptr)
     {
       if(is_valid_iterator(ptr) && ptr->is_selected())
       {
        return deselect(get_offset(ptr));
       }
       return sel_count();
     }

      mdb_kadr_entry * __fastcall single_kadr::get_selected(DWORD idx)
     {
      if(idx<sel_count())
      {
       idx = selected.at(idx);
       if(idx<entryes.size())
           return &entryes.at(idx);
      }
      return 0;
     }


 DWORD        __fastcall single_kadr::get_next   (DWORD offset,bool forward)
 {
  int offs = offset;
  if(offs<0 || offs > (int) this->get_entryes_count())
     {
      if( sel_count())
       offs = sel_count();
      else
       {
        offs = get_entryes_count();
       }
     }
     offs += forward ? 1:-1;
     if(offs<0) offs = entryes.size()-1;
     if((DWORD)offs>=entryes.size()) offs = 0;

     return offs;
 }

 DWORD     __fastcall single_kadr::select_next(bool forward,bool added)
 {
     int offset = sel_count();
     if(offset)
        offset = selected.at(offset-1);
     offset = get_next(offset,forward);
     if(!added)
      clear_selections();
      select(offset);
      return offset;
 }

 void    __fastcall single_kadr::select_all ()
 {
   DWORD i = 0;
   while(i<entryes.size())
        select(i++);
 }

 void         __fastcall single_kadr::select_in_rect(const POINT & lt,const POINT & br,bool added)
 {
   RECT r;
   int x1 = lt.x;
   int x2 = br.x;
   int y1 = lt.y;
   int y2 = br.y;
   if(x2<x1) std::swap(x1,x2);
   if(y2<y1) std::swap(y1,y2);
   SetRect(&r,x1,y1,x2,y2);
   if(!added) clear_selections();

//   entry_index::iterator lo;
//   entry_index::iterator hi;
//   if(get_range_position(x1,y1,x2,y2,lo,hi))
//   {
//     while(lo<hi)
//     {
//     POINT pt1,pt2;
//     mdb_kadr_entry & e = *get_by_offset(*lo);
//     pt1.x = pt2.x = e.position.x;
//     pt1.y = pt2.y = e.position.y;
//     pt2.x += e.size.cx;
//     pt2.y += e.size.cy;
//     if(PtInRect(&r,pt1) || PtInRect(&r,pt2))
//        select(*lo);
//
//      ++lo;
//     }
//   }

   kadr_entryes_table::iterator ptr = entryes.begin();
   kadr_entryes_table::iterator end = entryes.end  ();
   while(ptr<end)
   {
     POINT pt1,pt2;
     pt1.x = pt2.x = ptr->position.x;
     pt1.y = pt2.y = ptr->position.y;
     pt2.x += ptr->size.cx;
     pt2.y += ptr->size.cy;
     if(PtInRect(&r,pt1) || PtInRect(&r,pt2))
        select(ptr);
     ++ptr;
   }

 }

kadr_entryes_table::iterator __fastcall  single_kadr::add(const mdb_kadr_entry & entry)
{
  mdb_kadr_entry _e(entry);
  _e.kadr_id = kadr.kadr_id;
  kadr_entryes_table::iterator ret;
  if(!stdlib_utils::binary_search(entryes,_e,&ret))
     ret = entryes.insert(ret,entry);
   return ret;
}

kadr_entryes_table::iterator __fastcall single_kadr::add(DWORD record_id,DWORD number,int x,int y,int cx,int cy,DWORD color,DWORD bk_color )
{
 mdb_kadr_entry entry(kadr.kadr_id,record_id,number == (DWORD)-1 ? entryes.size():number);
 entry.position.x = x;
 entry.position.y = y;
 entry.size.cx    = cx;
 entry.size.cy    = cy;
 entry.options.color       = color;
 entry.options.bkgnd_color = bk_color;
 kadr_entryes_table::iterator ret = add(entry);
 return  ret;
}

     struct sel_transformer:public std::unary_function<DWORD,DWORD>
     {
      DWORD offset;
      DWORD operator()(DWORD & val) {return val<offset ? val : val-1;}
      sel_transformer(DWORD _offs):offset(_offs){}
      sel_transformer(const sel_transformer & src){ *this = src;}
      sel_transformer & operator = (const sel_transformer & src)
      {
       offset = src.offset;
       return *this;
      }
     };



 bool         __fastcall single_kadr::remove_entry   (kadr_entryes_table::iterator ptr)
 {
   if(ptr<entryes.end())
   {
    DWORD offset = this->get_offset(ptr);
    if(ptr->is_selected())
    {
       deselect(ptr);
    }
   std::transform(selected.begin(),selected.end(),selected.begin(),sel_transformer(offset));

    entryes.erase(ptr);
    return true;
   }
   return false;
 }

 bool         __fastcall single_kadr::remove_entry   (DWORD offset)
 {
  if(offset < entryes.size())
  {
    kadr_entryes_table::iterator ptr = entryes.begin();
    std::advance(ptr,offset);
    return remove_entry(ptr);
  }
  return false;
 }

 bool         __fastcall single_kadr::remove_entry   (DWORD rec_id,DWORD num)
 {
   kadr_entryes_table::iterator ptr;
   if(find_entry(rec_id,num,ptr))
      return remove_entry(ptr);
   return false;
 }

 int         __fastcall single_kadr::remove_selected()
 {
   int count = 0;
   std::sort(selected.begin(),selected.end());
   entry_index::reverse_iterator rbeg = selected.rbegin();
   entry_index::reverse_iterator rend = selected.rend  ();
   kadr_entryes_table::iterator entry_ptr;
   while(rbeg<rend)
   {
     if(get_by_offset(*rbeg,entry_ptr))
         entryes.erase(entry_ptr);
     ++count;
     ++rbeg;
   }
   selected.clear();
   if(_position_index_enabled) this->position_index_build();
   return count;
 }



   kadr_entryes_table::iterator
                  __fastcall single_kadr::lower_entry(DWORD rec_id,DWORD num)
{
   mdb_kadr_entry entry(kadr.kadr_id,rec_id,num);
   return std::lower_bound(entryes.begin(),entryes.end(),entry);
}

    kadr_entryes_table::iterator
                  __fastcall single_kadr::upper_entry(DWORD rec_id,DWORD num)
{
 mdb_kadr_entry entry(kadr.kadr_id,rec_id,num);
 return std::upper_bound(entryes.begin(),entryes.end(),entry);
}

bool         __fastcall single_kadr::find_entry(DWORD rec_id,DWORD num,kadr_entryes_table::iterator & ptr)
{
  mdb_kadr_entry entry(kadr.kadr_id,rec_id,num);
  return stdlib_utils::binary_search(entryes,entry,&ptr);
}

DWORD   __fastcall single_kadr::range_entry(DWORD rec_id,kadr_entryes_table::iterator &lo,kadr_entryes_table::iterator & hi)
{
 if(rec_id == (DWORD)-1)
 {
   lo = entryes.begin();
   hi = entryes.end();
 }
 else
 {
  lo = lower_entry(rec_id,0);
  hi = upper_entry(rec_id,-1);
 }
 return (DWORD)std::distance(lo,hi);
}


bool      __fastcall single_kadr::get_by_offset   (DWORD offset,single_kadr::entry_iterator & ptr)
{
   if(offset< this->entryes.size())
     {
       ptr = entryes.begin();
       std::advance(ptr,offset);
       return true;
     }
   return false;
}

DWORD        __fastcall single_kadr::remove_record(DWORD rec_id)
{
 kadr_entryes_table::iterator lo, hi;
 DWORD ret = range_entry(rec_id,lo,hi);
 if(ret)
   {
    kadr_entryes_table::iterator ptr = lo;
    while (ptr<hi)
        {
         deselect(ptr);
         ++ptr;
        }
    entryes.erase(lo,hi);
   }
 return ret;
}

     mdb_kadr_entry &         __fastcall entry_position_normalizer(mdb_kadr_entry & entry)
     {
       if(entry.size.cx < 0 ) {entry.position.x += entry.size.cx;entry.size.cx = -entry.size.cx;}
       if(entry.size.cy < 0 ) {entry.position.y += entry.size.cy;entry.size.cy = -entry.size.cy;}
       return entry;
     }
     void         __fastcall single_kadr::position_normalize()
     {
        std::transform(entryes.begin(),entryes.end(),entryes.begin(),entry_position_normalizer);
     }

     DWORD        __fastcall single_kadr::position_index_build()
     {
        position_idx.clear();
        position_normalize();
        mdb_kadr_entry ke(kadr.kadr_id,0,0);
        kadr_entry_comparer_t<mdb_kadr_entry_position_less> cmp(&entryes,ke);
        for(DWORD i = 0; i<entryes.size() ;i++ )
        {
          entry_index::iterator ptr = std::lower_bound(position_idx.begin(),position_idx.end(),i,cmp);
          position_idx.insert(ptr,i);
        }
      return position_idx.size();
     }

     void         __fastcall single_kadr::position_index_resort()
     {
       position_normalize();
       if(_position_index_enabled)
       {
          kadr_entry_comparer_t<mdb_kadr_entry_position_less> cmp(&entryes);
          std::sort(position_idx.begin(),position_idx.end(),cmp);
       }
     }
     void         __fastcall single_kadr::position_index_enable(bool enable)
     {
         if(_position_index_enabled != enable || position_idx.size() != this->entryes.size())
         {
             position_normalize  ();
             position_index_build();
             _position_index_enabled = this->position_idx.size() ? true : false;
         }
     }


     bool   __fastcall single_kadr::get_by_position (int x,int y,entry_index::iterator &ptr)
     {
          if(!_position_index_enabled || position_idx.size() != this->entryes.size()) position_index_enable(true);
          entry_index::iterator ptr_end = position_idx.end();
          mdb_kadr_entry ke(kadr.kadr_id,0,0);
          ke.position.x = x;
          ke.position.y = y;
          ke.size.cx = ke.size.cy = 1;
          kadr_entry_comparer_t<mdb_kadr_entry_position_less> cmp(&entryes,ke);
          ptr = std::lower_bound(position_idx.begin(),ptr_end,position_idx.size(),cmp);
          return ptr<ptr_end ? true : false;
     }

     DWORD           __fastcall single_kadr::get_at_position  (int x,int y)
     {
       entry_index::iterator pos;
       if(get_by_position(x,y,pos))
       {
         entry_index::iterator pos_end =  position_idx.end();
         while(pos<pos_end)
         {
           kadr_entryes_table::iterator entry_ptr ;
           if(get_by_offset(*pos,entry_ptr))
           {
           if(point_in_entry(*entry_ptr,x,y))
               return *pos;
           }
           ++pos;
         }
       }
       return DWORD(-1);
     }



     DWORD        __fastcall single_kadr::get_range_position (int x1,int y1,int x2,int y2,entry_index::iterator &lo,entry_index::iterator& hi)
     {
      DWORD ret = 0;
      if(this->_position_index_enabled)
         {
          lo = position_idx.begin();
          hi = position_idx.end  ();
          mdb_kadr_entry ke(kadr.kadr_id,0,0);
          ke.position.x = x1;
          ke.position.y = y1;
          ke.size.cx = ke.size.cy = 1;
          kadr_entry_comparer_t<mdb_kadr_entry_position_less> cmp(&entryes,ke);
          lo = std::lower_bound(lo,hi,position_idx.size(),cmp);
          ke.position.x = x2;
          ke.position.y = y2;
          ke.rec_id = -1;
          ke.number = -1;
          hi = std::lower_bound(position_idx.begin(),hi,position_idx.size(),cmp);
          if(lo == hi && hi <position_idx.end())
             ++hi;
          ret = std::distance(lo,hi);

         }
      return ret;
     }


   class  entryes_info_t:public std::unary_function<entry_index::value_type,void>
   {
     single_kadr &  sk;
     MDB_GPOINT     * left_top;
     MDB_GPOINT     * right_bottom;
     MDB_GSIZE      * min_size;
     MDB_GSIZE      * max_size;
     public:
     entryes_info_t(single_kadr & _sk,MDB_GPOINT     * _left_top,MDB_GPOINT* _right_bottom
                     ,MDB_GSIZE      * _min_size,MDB_GSIZE      * _max_size)
                     :sk(_sk),left_top(_left_top),right_bottom(_right_bottom)
                     ,min_size(_min_size),max_size(_max_size)
                     {
                      if(left_top    ){left_top->x = sk.kadr.kadr_size.cx;left_top->y = sk.kadr.kadr_size.cy;}
                      if(right_bottom){right_bottom->x = right_bottom->y = 0;}
                      if(min_size    ){min_size->cx = sk.kadr.kadr_size.cx;min_size->cx = sk.kadr.kadr_size.cy;}
                      if(max_size    ){max_size->cx = max_size->cy = 0;}
                     }
     entryes_info_t(const entryes_info_t & src)
                    :sk(src.sk),left_top(src.left_top),right_bottom(src.right_bottom)
                     ,min_size(src.min_size),max_size(src.max_size)
     {
     }

     void operator ()(const mdb_kadr_entry & entry)
     {
      if(left_top)     {left_top->x = std::min(left_top->x,entry.position.x);
                        left_top->y = std::min(left_top->y,entry.position.y);}

      if(right_bottom) {right_bottom->x = std::max(right_bottom->x,entry.position.x + entry.size.cx);
                        right_bottom->y = std::max(right_bottom->y,entry.position.y + entry.size.cy);}

      if(min_size    ) {min_size->cx = std::min(min_size->cx,entry.size.cx);
                        min_size->cy = std::min(min_size->cy,entry.size.cy);}

      if(max_size    ) {max_size->cx = std::max(max_size->cx,entry.size.cx);
                        max_size->cy = std::max(max_size->cy,entry.size.cy);}
     }


     void operator ()(const entry_index::value_type idx)
     {
      (*this)( sk.entryes.at(idx));
     }

   };

     void            __fastcall single_kadr::get_pos_sizes(entry_index::iterator beg
                                             ,entry_index::iterator end
                                             ,LPMDB_GPOINT left_top,LPMDB_GPOINT right_bottom
                                             ,LPMDB_GSIZE min_sz,LPMDB_GSIZE max_sz)
     {
      entryes_info_t entry_info(*this,left_top,right_bottom,min_sz,max_sz);
      for_each(beg,end,entry_info);
     }

     void            __fastcall single_kadr::get_pos_sizes(kadr_entryes_table::iterator beg
                                             ,kadr_entryes_table::iterator end
                                             ,LPMDB_GPOINT left_top,LPMDB_GPOINT right_bottom
                                             ,LPMDB_GSIZE min_sz,LPMDB_GSIZE max_sz)
     {
      entryes_info_t entry_info(*this,left_top,right_bottom,min_sz,max_sz);
      for_each(beg,end,entry_info);
     }

   class  entry_placer_t:public std::unary_function<entry_index::value_type,void>
   {
     single_kadr    &  sk;
     MDB_GSIZE      size_set;
     MDB_GPOINT     & position;
     int            left,top;
     int            step;
     bool           vert;
     bool           equal_size;
     public:
     entry_placer_t(single_kadr &  _sk,const MDB_GSIZE & _size_set,MDB_GPOINT & pt,int _step,bool _vert,bool _equal_size)
                   :sk(_sk),size_set(_size_set),position(pt),step(_step),vert(_vert),equal_size(_equal_size)

     {
      left = position.x;
      top  = position.y;
      step = std::max(step,1);
     }
     entry_placer_t(const entry_placer_t & src)
                   :sk(src.sk),size_set(src.size_set),position(src.position)
                   ,left(src.left),top(src.top),step(src.step),vert(src.vert)
                   ,equal_size(src.equal_size)
     {}

     void operator ()(mdb_kadr_entry & entry);
     void operator ()(const entry_index::value_type idx)
     {
      (*this)(sk.entryes.at(idx));
     }
   };

     void entry_placer_t::operator ()(mdb_kadr_entry & entry)
     {
      entry.position.x = position.x;entry.position.y = position.y;
      if(equal_size)
      {
       if( size_set.cx  ) entry.size.cx = size_set.cx;
       if( size_set.cy  ) entry.size.cy = size_set.cy;
      }
      if(vert)
        {
         position.y+= entry.size.cy + step;
         if(position.y >= sk.kadr.kadr_size.cy)
            {
             position.y = step ;//top;
             position.x+= size_set.cx+step;
             (*this)(entry);
            }
        }
        else
        {
         position.x += entry.size.cx + step;
         if(position.x >= sk.kadr.kadr_size.cx)
            {
             position.x = step;// left;
             position.y+= size_set.cy+step;
             (*this)(entry);
            }
        }
     }


   MDB_GPOINT     __fastcall single_kadr::layout_entryes   (entry_index::iterator  beg
                                                    ,entry_index::iterator  end
                                                    ,int from_left,int from_top
                                                    ,int step,bool vert,bool equal_size)

   {
     //Размещение элементов вертикально или горизонтально
     //C начальных координат с шагом с установкой одинакого размера по максимальному
     MDB_GSIZE  size;
     get_pos_sizes(beg,end,NULL,NULL,NULL,&size);
     MDB_GPOINT pos;
     pos.x  = from_left;
     pos.y  = from_top ;
     entry_placer_t placer(*this,size,pos,step,vert,equal_size);
     for_each(beg,end,placer);
     if(this->_position_index_enabled) position_index_resort();
     return pos;
   }

   DWORD    __fastcall single_kadr::make_selected_rec_ids    (records_ids_t &  rids_list)
   {
     DWORD prev_size = rids_list.size();
     entry_index::iterator sptr = this->selected.begin();
     entry_index::iterator send = this->selected.end  ();
     while(sptr<send)
     {
       mdb_kadr_entry * entry = this->get_by_offset(*sptr);
       records_ids_t::iterator rptr;
       if(!stdlib_utils::binary_search(rids_list,entry->rec_id,&rptr))
          rids_list.insert(rptr,entry->rec_id);
       ++sptr;
     }
     return  rids_list.size() - prev_size;
   }




