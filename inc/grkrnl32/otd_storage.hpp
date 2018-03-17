/*
  Хранилище sotd_proto
  
*/

#ifndef _OTD_STORAGE_HPP
#define _OTD_STORAGE_HPP
#ifdef __linux__
#include <lin_ke_defs.h>
#include <gklib/otd.h>
#else
#include <kertl.hpp>
#include <otd.h>
#endif

#include <vector>

using namespace KeRTL;
using namespace std;

typedef vector<sotd_proto>         otd_proto_vector ;
typedef otd_proto_vector::iterator proto_pointer;



class otd_storage
{
 protected:
 otd_proto_vector  otd_list;
 public:
 otd_storage(){};
 proto_pointer __fastcall begin(){return otd_list.begin();}
 proto_pointer __fastcall end  (){return otd_list.end();}

 proto_pointer __fastcall find      (otd_addr & addr);
 bool          __fastcall find      (otd_addr & addr,proto_pointer & ptr);
 proto_pointer __fastcall low_ptr   (otd_addr & addr,proto_pointer beg,proto_pointer end);
 proto_pointer __fastcall high_ptr  (otd_addr & addr,proto_pointer beg,proto_pointer end);
 DWORD         __fastcall get_range (otd_addr & addr, proto_pointer & low,proto_pointer & high);

 proto_pointer __fastcall insert    (otd_proto& sop);
 void          __fastcall erase     (proto_pointer & p);

 void          __fastcall clear();
 DWORD         __fastcall copy_by_addr(otd_addr & addr,otd_storage & dest);
 int           __fastcall get_parts(proto_pointer ptr,DWORD parts,DWORD low,DWORD hi,LPBYTE buffer,DWORD bsz);
 int           __fastcall get_parts_addr(otd_addr    & addr,DWORD parts,DWORD low,DWORD hi,LPBYTE buffer,DWORD bsz);
 int           __fastcall get_parts_idx (DWORD idx,DWORD parts,DWORD low,DWORD hi,LPBYTE buffer,DWORD bsz);
 DWORD         __fastcall get_count(otd_addr & addr);
 DWORD         __fastcall size(){return otd_list.size();}
 void          __fastcall reserve(DWORD count){otd_list.reserve(count);}

};

inline  void __fastcall otd_storage::clear()
 {
  otd_list.clear();
 }

 inline  bool          __fastcall otd_storage::find      (otd_addr & addr,proto_pointer & ptr)
 {
   ptr = find(addr);
   return ptr<otd_list.end() && ptr->op.addr && addr == *ptr->op.addr ? true : false;
 }



#endif
 