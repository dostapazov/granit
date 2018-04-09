/*Реализация списка заказов*/
#pragma hdrstop
#include "orders.hpp"
#include "gktlmdb_mod.hpp"



 void   __fastcall TOrders::add_order(TGKTlmDB* mod,modem_addr & ma,otd_addr & addr,DWORD order_parts)
 {
  TCHAR mon_text  [MAX_PATH];
  int  mon_len  = 0;
  wr_lock();
  otd_storage & ost = this->orders[ma];
  proto_pointer entry = ost.find(addr);
  if(order_parts&OTD_PROTO_PART_DATA)
     order_parts|=OTD_PROTO_PART_DIAG;
  if(entry>=ost.end())
   {
    if(order_parts)
    {
     DWORD  need_sz = sizeof(DWORD)<<1;
     LPBYTE ptr = new BYTE[need_sz];
     safe_memcpy(ptr,&addr,sizeof(addr));
     safe_memcpy(ptr+sizeof(addr),&order_parts,sizeof(WORD));
     sotd_proto sop(ptr,need_sz);
     sop.op.addr->fa&=OTD_FA_ALL;
     if(sop.op.addr->fa == OTD_FA_ALL || sop.op.addr->fa == OTD_FA_DATA_MASK)
       sop.op.addr->fa = OTD_ADDR_MAXVALUE;
     entry = ost.insert(sop.op);
     mon_len = wsprintf(mon_text,_T("Оформление подписки модем %03d.%03d  адрес %03d.%03d.%03d.%03d  parts %04X"),
               (DWORD) ma.modem,(DWORD)ma.line,(DWORD)addr.pu,(DWORD)addr.cp,(DWORD)addr.fa&OTD_FA_ALL,(DWORD)addr.sb,order_parts);
    }
   }
   else
    {
     if(order_parts)
       {
        mon_len = wsprintf(mon_text,_T("Изменение подписки модем %03d.%03d  адрес %03d.%03d.%03d.%03d  parts %04X"),
                           (DWORD) ma.modem,(DWORD)ma.line,(DWORD)addr.pu,(DWORD)addr.cp,(DWORD)addr.fa&OTD_FA_ALL,(DWORD)addr.sb,order_parts);
        *entry->op.ver_parts = order_parts;
       }
    }
  wr_unlock();
  if( mod && mon_len++ )
     mod->notify(TLMDB_DBLOW_MONITOR,TLMDB_DBLOW_MONITOR_ORDER,mon_text,mon_len*sizeof(TCHAR));

 }


 bool   __fastcall TOrders::remove_order(TGKTlmDB* mod,modem_addr & ma,otd_addr & addr)
 {
  bool ret = false;
  TCHAR mon_text  [4096];
  int  mon_len  = 0;
  wr_lock();
  if(orders.count(ma))
  {
   sotd_addr sa(addr);
   if((sa.fa&OTD_FA_ALL)==OTD_FA_ALL)
       sa.fa = OTD_ADDR_MAXVALUE;
   otd_storage & ost = this->orders[ma];
   if(sa.addrtype() == OTD_ADDR_TYPE_BROADCAST)
   {
    release_storage(ost);
    orders.erase(ma);
    ret = true;
    mon_len = wsprintf(mon_text,_T("Удаление из подписчиков модем %03d.%03d  "),
             (DWORD) ma.modem,(DWORD)ma.line);
   }
   else
   {
     proto_pointer low_ptr = ost.begin(),hi_ptr = ost.end();
     if(ost.get_range(sa,low_ptr,hi_ptr))
     {
      mon_len = wsprintf(mon_text,_T("Снятие подписки модем %03d.%03d  "),
                 (DWORD) ma.modem,(DWORD)ma.line);

      while(low_ptr<hi_ptr)
      {
       if(sa.is_include(*low_ptr->op.addr,true))
         {
          mon_len+=wsprintf(mon_text+mon_len,_T("%03d.%03d.%03d.%03d. "),(DWORD)low_ptr->op.addr->pu,(DWORD)low_ptr->op.addr->cp,(DWORD)low_ptr->op.addr->fa,(DWORD)low_ptr->op.addr->sb);
          this->release_sop(low_ptr);
          ost.erase(low_ptr);
          --hi_ptr;
         }
         else
         ++low_ptr;
      }
      if(!ost.size())
       {
        orders.erase(ma);
        mon_len+=wsprintf(mon_text+mon_len,_T("\rУдаление подписки"));
        
       }

     }
   }
  }
  wr_unlock();
  if( mod && mon_len++ )
     mod->notify(TLMDB_DBLOW_MONITOR,TLMDB_DBLOW_MONITOR_ORDER,mon_text,mon_len*sizeof(TCHAR));
  return ret;
 }


 DWORD  __fastcall TOrders::get_order_addrs(otd_addr & addr,DWORD parts,order_list & list)
 {
  rd_lock();
  modem_orders::iterator m_beg = orders.begin();
  modem_orders::iterator m_end = orders.end();
  while(m_beg!=m_end)
  {
   otd_storage & ost = m_beg->second;
   proto_pointer addr_hi = ost.end();
   proto_pointer addr_low = ost.begin();//ost.low_ptr(addr,ost.begin(),addr_hi);
   while(addr_low<addr_hi)
    {
     if(otd_addr_include(&addr,addr_low->op.addr) || otd_addr_include(addr_low->op.addr,&addr))
     {
      if((*addr_low->op.ver_parts)&parts)
         {
          modem_order_param mop(m_beg->first,(*addr_low->op.ver_parts)&(parts|OTD_PROTO_FLAG_PARTIAL_DATA));
          list.push_back( mop);
          break;
         }

     }
     ++addr_low;
    }
   ++m_beg;
  }
  rd_unlock();
  return list.size();
 }

 void   __fastcall TOrders::release_sop(proto_pointer ptr)
 {
   if(ptr->op.addr)
     delete (LPBYTE)ptr->op.addr;
 }

 void   __fastcall TOrders::release_storage(otd_storage & ost)
 {

  wr_lock();
  proto_pointer b_ptr = ost.begin();
  proto_pointer e_ptr = ost.end();
  while(b_ptr<e_ptr)
  {
   release_sop(b_ptr);

   ++b_ptr;
  }
  ost.clear();
  wr_unlock();
 }

  LRESULT  __fastcall TOrders::orders_enum(DWORD idx,LPTLMDB_ORDER_ENTRY entry)
  {
   LRESULT ret = RET_ORDERS_SUCCESS;
   WORD m_idx = HIWORD(idx);
   WORD o_idx = LOWORD(idx);
   rd_lock();
   if((DWORD)m_idx<orders.size())
     {
      modem_orders::iterator o_ptr = orders.begin();
      std::advance(o_ptr,m_idx);
      entry->addr.addr = o_ptr->first.addr;
      if((DWORD)o_idx<o_ptr->second.size())
      {
        proto_pointer p = o_ptr->second.begin();
        advance(p,o_idx);
        entry->ordered_addr.addr =  p->op.addr->addr;
        entry->ordered_parts     = DWORD(*p->op.ver_parts)&OTD_PROTO_PARTSMASK;
      }
      else
      ret = RET_ORDERS_NO_MORE_ORDERS;

     }
     else
     ret = RET_ORDERS_NO_MORE_MODEMS;
   rd_unlock();
   return  ret;
  }





