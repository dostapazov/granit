#pragma hdrstop
#include "gktlmdb_mod.hpp"

//bool   __fastcall  modem2pu::set(DWORD pu_num,modem_addr & addr)
//{
// if(pu_num < OTD_ADDR_MAXVALUE)
// {
//  //Задать соответствие ПУ - номеру модема
//  if(addr.modem<MODEM_ADDR_MAXVALUE)
//   pu_m[pu_num] = addr.modem ;
//  else
//   {
//    pu_m[pu_num] = -1;
//   }
//  conflict[pu_num ]= FALSE;
//  return true;
// }
// return false;
//}

void       __fastcall  modem2pu::clear()
{
  otd_modem.clear();
  modem_otd.clear();
  bzero(this->conflict,sizeof(conflict));
}


void       __fastcall  modem2pu::erase(modem_addr ma)
{
  modem_range_t range = modem_otd.equal_range(ma);
  while(range.first != range.second)
  {
    otd_modem.erase(range.first->second);
    ++range.first;
  }
  modem_otd.erase(ma);
  /*sotd_addr addr(_addr);
  addr.fa = addr.sb = -1;
  if(otd_modem.count(addr))
     otd_modem.erase(addr);
  modem_otd.erase(ma);
  */
}

bool       __fastcall  modem2pu::set(modem_addr ma,otd_addr _addr)
{
  sotd_addr addr(_addr);
  addr.fa = addr.sb = -1;
  if(!otd_modem.count(addr))
    {
     modem_otd.insert( modem_otd_t::value_type(ma,addr));
     otd_modem[addr] = ma;
     return true;
    }
   return false;
}

modem_addr __fastcall  modem2pu::get_modem(otd_addr _addr)
{
  sotd_addr addr(_addr);
  addr.fa = addr.sb = -1;
  modem_addr ma;
  if(otd_modem.count(addr))
     ma = otd_modem[addr];
  return  ma;
}


 BOOL   __fastcall  modem2pu::get_conflict(DWORD pu)
 {
   if(pu < OTD_ADDR_MAXVALUE)
   {
    return conflict[pu];
   }
  return FALSE;
 }
 void   __fastcall  modem2pu::set_conflict(DWORD pu,BOOL conf)
 {
   if(pu < OTD_ADDR_MAXVALUE)
   {
     conflict[pu] = conf;
   }
 }

  otd_addr   __fastcall  modem2pu::get_otd  (modem_addr & ma)
  {
     otd_addr addr;
     addr.addr = 0;
    if(modem_otd.count(ma))
      {
       modem_otd_t::iterator ptr = modem_otd.find(ma);
       if(ptr!=modem_otd.end())
          {
           addr.addr = ptr->second.addr;
           addr.fa = addr.sb = -1;
          }
      }
    return addr;
  }



//DWORD  __fastcall  modem2pu::get_modem(DWORD pu)
//{
//  modem_addr  addr(-1);
// if(pu<OTD_ADDR_MAXVALUE)
//    {
//     addr.modem = pu_m[pu];
//     if(addr.modem<MODEM_ADDR_MAXVALUE)
//        addr.sta = 0;
//    }
// return addr.addr;
//}
//
//DWORD  __fastcall  modem2pu::get_pu   (modem_addr & ma,DWORD cnt)
//{
//  DWORD ret = -1;
//  for(int i = 0;ret == (DWORD)-1 && i<OTD_ADDR_MAXVALUE;i++)
//     {
//       if(pu_m[i] == (DWORD)ma.modem && !cnt--)
//             {
//              ret = i;
//             }
//     }
//  return ret;
//}

 