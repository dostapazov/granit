#ifndef __STDLIB_UTILS_HPP__
#define __STDLIB_UTILS_HPP__
#include <vector>

namespace stdlib_utils
{
using namespace std;

#ifdef __clang__
#define _clang_typename_ typename
#else
#define _clang_typename_
#endif

template <typename T >
bool binary_search(_clang_typename_ std::vector<T>::iterator  beg, _clang_typename_ std::vector<T>::iterator  end, const T & v , _clang_typename_ std::vector<T>::iterator * _ptr = NULL )
{
  _clang_typename_ std::vector<T>::iterator ptr = std::lower_bound(beg,end,v);
  if(_ptr ) *_ptr = ptr;
     return (ptr<end && *ptr == v ) ? true : false ;
}

template <typename T >
inline bool binary_search(_clang_typename_ std::vector<T> & t, const T & v , _clang_typename_ std::vector<T>::iterator * _ptr = NULL )
{
  return binary_search(t.begin(),t.end(),v,_ptr);
}

template <typename T ,typename comp_t>
bool binary_search(_clang_typename_ std::vector<T>::iterator beg,_clang_typename_ std::vector<T>::iterator end, const T & v ,comp_t comp ,_clang_typename_ std::vector<T>::iterator * _ptr = NULL )
{
 _clang_typename_ std::vector<T>::iterator ptr = std::lower_bound(beg,end,v,comp);
 if(_ptr ) *_ptr = ptr;
 return (ptr<end && *ptr == v ) ? true : false ;
}

template <typename T ,typename comp_t>
inline bool binary_search(_clang_typename_ std::vector<T> & t, const T & v ,comp_t comp ,_clang_typename_ std::vector<T>::iterator * _ptr = NULL )
{
 return binary_search(t.begin(),t.end(),v,comp,_ptr);
}



template <typename T >
DWORD  get_range(_clang_typename_ std::vector<T> & t,const T & v, _clang_typename_ std::vector<T>::iterator * _range_begin ,_clang_typename_ std::vector<T>::iterator * _range_end )
{
 _clang_typename_ std::vector<T>::iterator ptr = t.end();
 *_range_begin = *_range_end = ptr;
 ptr = std::lower_bound(t.begin(),t.end(),v);
 if(ptr<t.end() && *ptr == v )
    {
     *_range_begin = ptr;
     *_range_end   = std::upper_bound(ptr,t.end(),v);
    }
  return std::distance(*_range_begin,*_range_end);
}

template <typename T ,typename comp_t>
DWORD  get_range(_clang_typename_ std::vector<T> & t,const T & v, _clang_typename_ std::vector<T>::iterator * _range_begin ,_clang_typename_ std::vector<T>::iterator * _range_end , comp_t comp )
{

 _clang_typename_ std::vector<T>::iterator ptr = t.end();
 *_range_begin = *_range_end = ptr;
 ptr = std::lower_bound(t.begin(),t.end(),v,comp);
 if(ptr<t.end() && *ptr == v )
    {
     *_range_begin = ptr;
     *_range_end   = std::upper_bound(ptr,t.end(),v,comp);
    }
  return std::distance(*_range_begin,*_range_end);
}



template <typename T>
_clang_typename_ std::vector<T>::iterator sorted_add(_clang_typename_ std::vector<T> & t,const T & v)
{
 _clang_typename_ std::vector<T>::iterator ptr = std::lower_bound(t.begin(),t.end(),v);
 ptr = t.insert(ptr,v);
 return ptr;
}

template <typename T,typename comp_t>
_clang_typename_ std::vector<T>::iterator sorted_add(_clang_typename_ std::vector<T> & t,const T & v,comp_t comp)
{
 _clang_typename_ std::vector<T>::iterator ptr = std::lower_bound(t.begin(),t.end(),v,comp);
 ptr = t.insert(ptr,v);
 return ptr;
}

}//end of stdlib_utils


#endif
