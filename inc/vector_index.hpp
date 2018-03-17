#ifndef __VECT_INDEX_HPP__
#define __VECT_INDEX_HPP__
#include <vector>
#include <algorithm>

namespace vector_index
{
  template <typename _V,typename _Comp>
  class vect_index_comparer
  {
    public:
    typedef std::vector<_V> vect;
    typedef vect::size_type size_type;
    vect_index_comparer():pvect(NULL){};
    vect_index_comparer(const vect * _pvect,const _Comp  _comp): pvect(_pvect),comp(_comp){}
    vect_index_comparer(const vect * _pvect,const _V & v,const _Comp  _comp):pvect(_pvect),base_v(v),comp(_comp){}
    vect_index_comparer(const vect_index_comparer & _src){*this = _src;}
    vect_index_comparer & operator = (const vect_index_comparer & _src)
    { comp = _src.comp; base_v = _src.base_v; pvect = _src.pvect; return *this; }

    bool operator ()(const size_type& i1, const size_type& i2)
    {
     if(pvect)
     {
      const _V * v1 ;
      size_type pvsz = pvect->size();
      if(i1< pvsz)
         v1 = &pvect->at(i1);
              else
         v1 = &base_v;
      const _V * v2 ;
      if(i2<pvsz)
         v2 = &pvect->at(i2) ;
             else
         v2 = &base_v;
      bool ret = comp(*v1,*v2);
      return ret;
     }
     return false;
    }
    private:
    const vect * pvect;
    _V     base_v;
    _Comp  comp;
  };

 template <typename _V, typename _iter ,typename _Comp >
_iter   index_lower_bound( std::vector<_V> & vect, const _V &v, _iter  __start, _iter  __finish, _Comp comp )
  {
    return  std::lower_bound(__start,__finish,vect.size()+1,vect_index_comparer<_V,_Comp> (&vect,v,comp));
  }

 template <typename _V, typename _iter ,typename _Comp >
_iter   index_upper_bound( std::vector<_V> & vect, const _V &v, _iter  __start, _iter  __finish, _Comp comp )
  {
    return  std::upper_bound(__start,__finish,vect.size()+1,vect_index_comparer<_V,_Comp> (&vect,v,comp));
  }



  template <typename _V,typename _Comp,typename size_type>
  void      index_build(const std::vector<_V> & vect,_Comp comp,std::vector<size_type> & result)
  {
     result.clear();
     result.reserve(vect.size());

     for(size_type i = 0;i<vect.size();i++)
       result.push_back(i);
     std::sort(result.begin(),result.end(),vect_index_comparer<_V,_Comp> (&vect,comp));
  }

template <typename _V,typename _Comp>
struct less
{
  _Comp comp;
  bool operator ()(const _V & v1,const _V & v2) {return (comp.compare(v1,v2) < 0) ? true : false;}
};

template <typename _V,typename _Comp>
struct equal
{
  _Comp comp;
  bool operator ()(const _V & v1,const _V & v2) {return (comp.compare(v1,v2) == 0) ? true : false;}
};


template < typename _V ,typename _Comp,typename _IndexComp>
class   indexed_vector
{
   public:
   typedef std::vector<_V>                container_t;
   typedef container_t::value_type        value_type;
   typedef container_t::pointer           pointer;
   typedef container_t::reference         reference;
   typedef container_t::const_reference   const_reference;
   typedef container_t::size_type         size_type;
   typedef container_t::iterator          iterator;
   typedef container_t::const_iterator    const_iterator;
   typedef std::vector<size_type>         index_t;
   typedef index_t::iterator              index_iterator;
   typedef index_t::const_iterator        const_index_iterator;

   private:
   container_t   data;
   index_t       index;
   bool          index_valid;
   public:
   indexed_vector  ():index_valid(false) {};
   size_type         __fastcall size       (){ return data.size();}
   iterator          __fastcall begin      (){ return data.begin();}
   iterator          __fastcall end        (){ return data.end();}
   index_iterator    __fastcall index_begin(){ build_index();return index.begin();}
   index_iterator    __fastcall index_end  (){ build_index();return index.end();  }
   iterator          __fastcall insert     (iterator ptr,const_reference val){index_valid = false; return data.insert(ptr,val);}
   iterator          __fastcall insert     (const_reference val)
   {
     iterator ptr = std::lower_bound(data.begin(),data.end(),val,less<_V,_Comp>());
     return  insert(ptr,val);
   }

   bool              __fastcall find       (const_reference val,iterator & ptr)
   {
      ptr = std::lower_bound(data.begin(),data.end(),val,less<_V,_Comp>());
      return (ptr<data.end() && equal<_V,_Comp>()(val,*ptr) ) ? true : false;
   }

   bool              __fastcall find       (const_reference val,index_iterator & index_ptr)
   {
      build_index();
      index_ptr = index_lower_bound(data,val,index_begin(),index_end(),less<_V,_IndexComp>());
      if(index_ptr<index_end())
        {
          _V & find_val = at(*index_ptr);
          return equal<_V,_IndexComp>()(val,find_val);

        }
      return false;
   }

   size_type             __fastcall range      (const_reference val1,const_reference val2,iterator       & range_begin,iterator & range_end)
   {
     range_begin  = std::lower_bound(range_begin,range_end,val1,less<_V,_Comp>());
     range_end    = std::upper_bound(range_begin,range_end,val2,less<_V,_Comp>());
     return  std::distance(range_begin,range_end);
   }

   size_type         __fastcall range      (const_reference val1,const_reference val2,index_iterator & range_begin,index_iterator & range_end)
   {
      build_index();
      range_begin   = index_lower_bound(data,val1,range_begin,range_end,less<_V,_IndexComp>());
      range_end     = index_upper_bound(data,val2,range_begin,range_end,less<_V,_IndexComp>());
      return  std::distance(range_begin,range_end);
   }

   reference         __fastcall at         (size_type pos      ) {return pos<size() ? data.at(pos) : *data.end();}
   reference         __fastcall index_at   (size_type index_pos)
   {
    build_index  ();
    return index_pos<index.size() ? at(index.at(index_pos)) : *data.end();
   };

   void         __fastcall build_index(){if(!index_valid){index_build(data,less<_V,_IndexComp>(),index); index_valid = true;}}
   void         __fastcall index_invalidate(){index_valid = false;}
   void         __fastcall clear(){index.clear();data.clear();index_valid = false;}
   void         __fastcall erase(iterator ptr)      {if(ptr<data.end()) {index_valid = false;data.erase(ptr);}}
   void         __fastcall erase(index_iterator ptr)
   {
     if(ptr<data.end())
      {
       iterator ptr = data.begin();
       advance(ptr,*iptr)
       erase(ptr);
      }
   }

};

}//end of namespace

#endif