/*
 Таблица  заказов кадров

*/
#ifndef _ORDER_TABLE_HPP
#define _ORDER_TABLE_HPP

#include <mediumdb.h>
#include <vector>
#include <algorithm>
#include <functional>
#include <mediumdb.h>
#//include <modem_proto.h>

using namespace std;

#pragma pack (push,1)

struct  order_item:public ORDER_ITEM
{
// DWORD        kadr_id;    //Идентификатор КАДРА
//  modem_addr   dest_addr;  //Адрес заказавшего модема
// DWORD        order_mask; //Флаги заказа
 order_item()
 {
  dw_size        = sizeof(*this);
  kadr_id        = -1;
  dest_addr.addr = -1;
  order_mask     = -1;
 }

 order_item(DWORD _kadr_id,MODEM_ADDR & _dest_addr,DWORD _order_mask)
 {
  dw_size        = sizeof(*this);
  kadr_id        = _kadr_id;
  dest_addr.addr = _dest_addr.addr;
  order_mask     = _order_mask;
}
 order_item(const order_item & oi)
 {*this = oi;}
 order_item & operator = (const order_item & oi)
 { kadr_id = oi.kadr_id; dest_addr = oi.dest_addr;order_mask = oi.order_mask; return *this;}

};

typedef vector<order_item>  order_vector;
typedef vector<DWORD>       index_vector;

inline bool operator < (const order_item & oi1,const order_item oi2)
{
 if(oi1.kadr_id == oi2.kadr_id)
   return oi1.dest_addr < oi2.dest_addr ? true:false;
  return  oi1.kadr_id   < oi2.kadr_id   ? true:false;
}

inline bool operator == (const order_item & oi1,const order_item oi2)
{
 return (oi1.kadr_id == oi2.kadr_id && oi1.dest_addr == oi2.dest_addr) ? true:false;
}

inline bool operator != (const order_item & oi1,const order_item oi2)
{
 return (oi1==oi2) ? false:true;
}



struct order_item_cmp_byaddr
{
 order_vector * ov;
 DWORD          ov_size;
 order_item     base;
 order_item_cmp_byaddr(order_vector * _ov,order_item & _base):ov(_ov),base( _base )
 {ov_size = ov ? ov->size(): 0 ;}
 order_item_cmp_byaddr(const order_item_cmp_byaddr & ocmp)
 {
   ov = ocmp.ov; ov_size = ocmp.ov_size; base = ocmp.base;
 }
 bool operator () (const  DWORD & off1,const DWORD & off2);
};

inline  bool order_item_cmp_byaddr::operator () (const  DWORD & off1,const DWORD & off2)
{
  order_item & oi1 = off1<ov_size ? ov->at(off1):base;
  order_item & oi2 = off2<ov_size ? ov->at(off2):base;
  if( oi1.dest_addr.addr == oi2.dest_addr.addr)
     return oi1.kadr_id < oi2.kadr_id ? true:false;
  return oi1.dest_addr < oi2.dest_addr ? true:false;
}

#pragma pack (pop)

class orders_table
{
 protected:
 order_vector orders;
 index_vector index;
 DWORD __fastcall rebuild_index();
 public:
                     orders_table(){};
 DWORD    __fastcall size() {return orders.size();}
 DWORD    __fastcall add_order      (MODEM_ADDR & ma,DWORD kadr_id,DWORD mask);
 DWORD    __fastcall remove_order   (MODEM_ADDR & ma,DWORD kadr_id);
 DWORD    __fastcall get_modem_range(DWORD kadr_id,order_vector::iterator & lo,order_vector::iterator & hi);
 DWORD    __fastcall get_kadr_range (MODEM_ADDR & ma,index_vector::iterator & lo,index_vector::iterator &hi);
 bool     __fastcall get_modem_addr (DWORD index,MODEM_ADDR & ma);
 bool     __fastcall enum_orders    (DWORD index,LPORDER_ITEM order,bool by_modem);
 DWORD    __fastcall get_modem_list (order_vector & order_modem);
};





#endif
 