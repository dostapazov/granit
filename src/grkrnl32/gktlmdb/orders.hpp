/*
  Список подписки на ОТД данные БД нижнего уровня

*/

#ifndef _ORDER_INC_HPP
#define _ORDER_INC_HPP

#include <otd.h>
#include <modem_proto.h>
#include <kertl.hpp>
#include <otd_storage.hpp>
#include <vector>
#include <map>
#include <algorithm>
#include <gktlmdb.h>

#ifndef FASTMUTEX_LOCKER
#define FASTMUTEX_LOCKER 1
#endif


using namespace std;

class TGKTlmDB;

struct  modem_order_param:public modem_addr
{
  DWORD order_parts;
  modem_order_param():order_parts(0){}
  modem_order_param(const modem_addr & ma,DWORD p):modem_addr(ma),order_parts(p){}
  modem_order_param(const modem_order_param & mop){*this = mop;}
  modem_order_param & operator = (const modem_order_param & mop){addr = mop.addr;order_parts=mop.order_parts;return *this;}
};

typedef std::vector<modem_order_param>   order_list;
typedef std::map<modem_addr,otd_storage> modem_orders;

class TOrders
{
 protected:
 #if FASTMUTEX_LOCKER == 1
  KeRTL::TFASTMutex locker;
 #else
  KeRTL::TMrSw locker;
 #endif

 modem_orders orders;
 void   __fastcall release_sop(proto_pointer ptr);
 void   __fastcall release_storage(otd_storage & ost);
 public:
 TOrders(){};
 void   __fastcall add_order      (TGKTlmDB*,modem_addr & ma,otd_addr & addr,DWORD parts);
 bool   __fastcall remove_order   (TGKTlmDB*,modem_addr & ma,otd_addr & addr);
 DWORD  __fastcall get_order_addrs(otd_addr & addr,DWORD parts,order_list & list);

 bool   __fastcall wr_lock  (DWORD tm = INFINITE);
 bool   __fastcall wr_unlock();
 bool   __fastcall rd_lock  (DWORD tm = INFINITE);
 bool   __fastcall rd_unlock();
 LRESULT  __fastcall orders_enum(DWORD idx,LPTLMDB_ORDER_ENTRY entry);

 class rd_locker
 {
  TOrders * orders;
  rd_locker(){};
  public:
  rd_locker  (TOrders& ord,DWORD to = INFINITE):orders(&ord){ ord.rd_lock(to);}
  ~rd_locker (){orders->rd_unlock();}
 };
};

inline  bool __fastcall TOrders::wr_lock(DWORD tm )
 {
  #if FASTMUTEX_LOCKER == 1
  return locker.Lock(tm);
  #else
  return locker.LockForWrite(tm);
  #endif
 }

inline  bool __fastcall TOrders::wr_unlock()
 {
  #if FASTMUTEX_LOCKER == 1
  return locker.Unlock();
  #else
  return locker.WriteUnlock();
  #endif
 }

inline  bool __fastcall TOrders::rd_lock(DWORD tm )
 {
  #if FASTMUTEX_LOCKER == 1
  return locker.Lock(tm);
  #else
  return locker.LockForRead(tm);
  #endif
 }

inline  bool __fastcall TOrders::rd_unlock()
{
  #if FASTMUTEX_LOCKER == 1
  return locker.Unlock();
  #else
  return locker.ReadUnlock();
  #endif
}



#endif

