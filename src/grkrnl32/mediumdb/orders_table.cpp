#pragma hdrstop
#include "orders_table.hpp"
#include <stdlib_utils.hpp>

 DWORD    __fastcall orders_table::add_order      (MODEM_ADDR & ma,DWORD kadr_id,DWORD mask)
 {
  /*Добавление подписки*/
  DWORD offset;
  DWORD ret = GKH_RET_ERROR;//FALSE;
  order_item oi(kadr_id,ma,mask);
  order_vector::iterator ptr = std::lower_bound(this->orders.begin(),orders.end(),oi);
  if(ptr<orders.end() && *ptr == oi )
   {
    //Изменение подписки
    ptr->order_mask = mask;
    offset = std::distance(orders.begin(),ptr);
   }
   else
   {
    ptr          = orders.insert(ptr,oi);
    offset = std::distance(orders.begin(),ptr);
    std::transform(index.begin(),index.end(),index.begin(),inc_index(offset));
    order_item_cmp_byaddr comparer(&orders,oi);
    index_vector::iterator i_ptr = std::lower_bound(index.begin(),index.end(),offset,comparer);
    index.insert(i_ptr,offset);
    ret = offset;
   }
   return ret;
 }

 DWORD    __fastcall orders_table::remove_order   (MODEM_ADDR & ma,DWORD kadr_id)
 {
   /*Удаление подписки на кадра*/
   DWORD ret = 0;
   index_vector::iterator idx_lo,idx_hi;
   if(get_kadr_range(ma,idx_lo,idx_hi))
   {
     while(idx_lo<idx_hi)
     {
      order_vector::iterator o_ptr = orders.begin();
      DWORD offset = *idx_lo;
      std::advance(o_ptr,offset);
      if((kadr_id == (DWORD)-1 )|| o_ptr->kadr_id == kadr_id )
      {
        orders.erase(o_ptr);
        index.erase(idx_lo);
        --idx_hi;
        ++ret;
        std::transform(index.begin(),index.end(),index.begin(),dec_index(offset));

      }
      else
      ++idx_lo;
     }
   }
   return ret;
 }

 DWORD    __fastcall orders_table::get_modem_range(DWORD kadr_id,order_vector::iterator & lo,order_vector::iterator & hi)
 {
  modem_addr lo_modem(0);
  modem_addr hi_modem(-1);
  lo = orders.begin(),hi = orders.end();
  lo = std::lower_bound(lo,hi,order_item(kadr_id,lo_modem,-1));
  hi = std::upper_bound(lo,hi,order_item(kadr_id,hi_modem,-1));
  DWORD count = std::distance(lo,hi);
  return count;
 }

 DWORD    __fastcall orders_table::get_modem_list (order_vector & order_modem)
 {
    index_vector::iterator  curr_ptr = index.begin();
    index_vector::iterator  curr_end = index.end  ();
    index_vector::iterator  end = curr_end;
    while(curr_ptr<end)
    {
      order_vector::value_type val (0,orders.at(*curr_ptr).dest_addr,-1);
      order_vector::iterator res_ptr;
      if(!stdlib_utils::binary_search(order_modem,val,&res_ptr))
          order_modem.insert(res_ptr,val);
      get_kadr_range(val.dest_addr,curr_ptr,curr_end);
      curr_ptr = curr_end;
    }
   return order_modem.size() ;
 }


 DWORD    __fastcall orders_table::get_kadr_range (MODEM_ADDR & ma,index_vector::iterator & lo,index_vector::iterator &hi)
 {
  order_item lo_item(0 ,ma,0);
  order_item hi_item(-1,ma,0);
  lo = index.begin(),hi = index.end();
  lo = lower_bound(lo,hi,-1,order_item_cmp_byaddr(&orders,lo_item));
  hi = upper_bound(lo,hi,-1,order_item_cmp_byaddr(&orders,hi_item));
  DWORD count = std::distance(lo,hi);
  return count;
 }

 DWORD __fastcall orders_table::rebuild_index()
 {
  order_vector::iterator o_beg = this->orders.begin(),o_end = orders.end();
  index.clear();
  index.reserve(orders.size());
  DWORD offset = 0;
  while(o_beg<o_end)
  {
   index_vector::iterator i_ptr = std::lower_bound(index.begin(),index.end(),offset,order_item_cmp_byaddr(&orders,*o_beg));
   index.insert(i_ptr,offset);

   ++o_end;
   ++offset;
  }
  return offset;
 }

  bool     __fastcall orders_table::get_modem_addr (DWORD idx,MODEM_ADDR & ma)
  {
    if(idx<this->index.size())
    {
      idx = index.at(idx);
      ma.addr = orders.at(idx).dest_addr.addr;
      return true;
    }
   return false;
  }

 bool     __fastcall orders_table::enum_orders    (DWORD idx,LPORDER_ITEM _order,bool by_modem)
 {
      if(idx<index.size())
      {
       if(by_modem)
          idx = index.at(idx);
       order_item & oi    = orders.at(idx);
       _order->kadr_id    = oi.kadr_id    ;
       _order->dest_addr  = oi.dest_addr  ;
       _order->order_mask = oi.order_mask ;
       return true;
      }
    return false;
 }





