#include "gripxmod_main.hpp"
//#inclue  <algorithm>
#include   <algorithm>


 DWORD __fastcall tu_client_list::count  ()
 {
  DWORD ret;
  lock();
  ret = list.size();
  unlock();
  return ret;
 }

 DWORD __fastcall tu_client_list::add    (GRNET_TU_CLIENT & gtc)
 {
  DWORD ret;
  lock();
  ret = find(gtc.name,gtc.sock_num);
  if(ret >= list.size())
  {
    grnet_tu_client tc(gtc);
    std::vector<grnet_tu_client>::iterator ptr = std::lower_bound(list.begin(),list.end(),tc);
    list.insert()
  }
  unlock();
  return ret;
 }
 DWORD __fastcall tu_client_list::get    (DWORD idx,GRNET_TU_CLIENT & gtc)
 {
  DWORD ret = -1;
  lock();
  if(idx<list.size())
  {
   grnet_tu_client & tc = list.at(idx);
   gtc.sock_num = tc.sock_num;
   lstrcpyn(gtc.name,tc.name,sizeof(gtc.name));
   ret = idx;
  }
  unlock();
  return ret;
 }
 DWORD __fastcall tu_client_list::remove (DWORD idx)
 {
  DWORD ret;
  lock();
  unlock();
  return ret;
 }
 DWORD __fastcall tu_client_list::update (DWORD idx,GRNET_TU_CLIENT & tc)
 {
  DWORD ret;
  lock();
  unlock();
  return ret;
 }

 DWORD __fastcall tu_client_list::find   (char * name,DWORD sock_num)
 {
  DWORD ret;
  lock();
  unlock();
  return ret;
 }
 DWORD __fastcall tu_client_list::release()
 {
  DWORD ret;
  lock();
  unlock();
  return ret;
 }
 DWORD __fastcall tu_client_list::write(TStream * strm)
 {
  DWORD ret;
  lock();
  unlock();
  return ret;
 }
 DWORD __fastcall tu_client_list::read (TStream * strm)
 {
  DWORD ret;
  lock();
  unlock();
  return ret;
 }

