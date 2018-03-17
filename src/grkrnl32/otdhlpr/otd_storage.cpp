#ifdef __WIN32__
 #include <otd_storage.hpp>
#else
 #include <gklib/otd_storage.hpp>
#endif
 #include <algorithm>


 /**< Поиск по адресу */
 proto_pointer __fastcall otd_storage::find (otd_addr & addr)
 {
   sotd_addr sa(addr);
   if(sa.fa!=OTD_ADDR_MAXVALUE)
      sa.fa&=OTD_FA_ALL;
   if(sa.fa == OTD_FA_ALL)
      sa.fa = OTD_ADDR_MAXVALUE;
   proto_pointer ret = otd_list.end(),ptr;
   sotd_proto sop;
   sop.op.addr = &sa;

   ptr = lower_bound(otd_list.begin(),ret,sop,sotd_proto_less(addr));
   if( ptr<otd_list.end() &&  *ptr == sa)
     ret = ptr;
   return ret;
 }

 ///**< Нижняя граница адреса */
 proto_pointer __fastcall otd_storage::low_ptr (otd_addr & addr,proto_pointer beg,proto_pointer end)
 {
   proto_pointer ret;
   sotd_proto_less comp(addr);
   proto_pointer _beg = otd_list.begin(),_end = otd_list.end();
   if(beg>_beg && beg<_end)
      _beg = beg;
   if(end>_beg && end<_end)
      _end = end;
   ret = lower_bound(_beg,_end,sotd_proto(),comp);
   return ret;
 }

 /**< Верхняя граница адреса */
 proto_pointer __fastcall otd_storage::high_ptr(otd_addr & addr,proto_pointer beg,proto_pointer end)
 {
   proto_pointer ret;
   proto_pointer _beg = otd_list.begin(),_end = otd_list.end();
   if(beg>_beg && beg<_end)
      _beg = beg;
   if(end>_beg && end<_end)
      _end = end;
   ret = upper_bound(_beg,_end,sotd_proto(),sotd_proto_less(addr));
   return ret;
 }

 /**< Получение диапазона соответствующего требуемому адресу */
 DWORD          __fastcall otd_storage::get_range(otd_addr & addr, proto_pointer & low,proto_pointer & high)
 {
   sotd_addr low_addr (addr);
   sotd_addr high_addr(addr);
   low_addr.fa&=OTD_FA_ALL;
   high_addr.fa &=OTD_FA_ALL;
   if(low_addr.sb == OTD_ADDR_MAXVALUE)
      low_addr.sb = 0;
   if(low_addr.fa == OTD_FA_ALL || low_addr.fa == OTD_ADDR_MAXVALUE)
      {low_addr.fa = 0;high_addr.fa = OTD_ADDR_MAXVALUE;}
   if(low_addr.cp == OTD_ADDR_MAXVALUE)
      low_addr.cp = 0;
   if(low_addr.pu == OTD_ADDR_MAXVALUE)
      low_addr.pu = 0;
   low   = this->low_ptr(low_addr,low,high);
   high  = this->high_ptr(high_addr,low,high);
   return (DWORD)distance(low,high);
 }

 /**< Вставка с упорядочиванием */
 proto_pointer __fastcall otd_storage::insert(otd_proto & sop)
 {
  return otd_list.insert(low_ptr(*sop.addr,otd_list.begin(),otd_list.end()),sop);
 }

/**<  Формирование списка удовлетворяющего адресу
      Адрес может быть вещательным
 */
DWORD         __fastcall otd_storage::copy_by_addr(otd_addr & addr,otd_storage & dest)
{
  proto_pointer low,hi;
  get_range(addr,low,hi);
  while(low < hi)
  {
   if(otd_addr_include(low->op.addr,&addr))
      dest.otd_list.push_back(*low);
   low++;
  }
  return dest.otd_list.size();
}

void          __fastcall otd_storage::erase    (proto_pointer & p)
{
 otd_list.erase(p);
}

int           __fastcall otd_storage::get_parts_idx (DWORD idx,DWORD parts,DWORD low,DWORD hi,LPBYTE buffer,DWORD bsz)
{
 int ret = 0;
 if(idx< this->otd_list.size())
 {
      proto_pointer l_ptr = otd_list.begin();
      advance(l_ptr,idx);
      ret = get_parts(l_ptr,parts,low,hi,buffer,bsz);
   }
 return ret;
}

int           __fastcall otd_storage::get_parts(proto_pointer ptr,DWORD parts,DWORD low,DWORD hi,LPBYTE buffer,DWORD bsz)
{
      int ret;
      otd_proto_param opp;
      ZeroMemory(&opp,sizeof(opp));
      opp.dw_size = sizeof(opp);
      //Получение данных 1 объекта
      otd_proto_getparam(&(*ptr).op,&opp);
      opp.parts&=parts;
      opp.parts|=(parts&OTD_PROTO_FLAGS_MASK);
      DWORD f,l;
      f = KeRTL::MAX(low,(DWORD)opp.dt_param.first);
      l = KeRTL::MIN(hi,(DWORD)opp.dt_param.first+opp.dt_param.count-1);
      opp.dt_param.first = f;
      opp.dt_param.count = l-f+1;

      f = KeRTL::MAX(low,(DWORD)opp.pd_param.first);
      l = KeRTL::MIN(hi,(DWORD)opp.pd_param.first+opp.pd_param.count-1);
      opp.pd_param.first = f;
      opp.pd_param.count = l-f+1;

      f = KeRTL::MAX(low,(DWORD)opp.cm_param.first);
      l = KeRTL::MIN(hi,(DWORD)opp.cm_param.first+opp.cm_param.count-1);
      opp.cm_param.first = f;
      opp.cm_param.count = l-f+1;
      if(opp.name_sz &&  (opp.parts & OTD_PROTO_PART_NAME))
          opp.name_sz =  OTD_TEXT_GET_LENGTH(ptr->op.name);

      DWORD need_size = otd_proto_calc_size_ex(&opp,0);
      if(need_size<=bsz)
      {
       ret = otd_proto_getparts_ex(buffer,bsz,&opp,&(*ptr).op,0);
      }
      else
      ret = -(int)need_size;
      return ret;
}

int           __fastcall otd_storage::get_parts_addr(otd_addr    & addr,DWORD parts,DWORD low,DWORD hi,LPBYTE buffer,DWORD bsz)
{
 int ret = 0;
 DWORD count;
 proto_pointer l_ptr,h_ptr;
 count = get_range(addr,l_ptr,h_ptr);
 if(count)
 {
    if(count == 1)
    {
      ret = get_parts_idx(distance(otd_list.begin(),l_ptr),parts,low,hi,buffer,bsz);
    }
    else
    {
      otd_proto_param opp  ;
      bzero(&opp,sizeof(opp));
      opp.dw_size = sizeof(opp);
      //Получение данных 1 объекта
      otd_proto_getparam(&(*h_ptr).op,&opp);
     //Получение  данных группы объектов
      otd_proto_getparam(&(*h_ptr).op,&opp);
    }
 }
 return ret;
}

DWORD         __fastcall otd_storage::get_count(otd_addr & addr)
{
  //Подсчет количества подобъектов
  //КП в ПУ или МОДУЛЕЙ в КП
  DWORD range_count,count = 0;
  proto_pointer l_ptr = otd_list.begin(),h_ptr = otd_list.end();
  range_count = get_range(addr,l_ptr,h_ptr);
  if(range_count)
  {
   DWORD type = otd_addr_type(&addr);
   while(l_ptr<h_ptr)
    {

     DWORD diff = otd_addr_type(l_ptr->op.addr)-type ;
     if(otd_addr_include(l_ptr->op.addr,&addr)&&( (type && diff == 2) || (!type && diff == 1)))
          count++;
     ++l_ptr;
    }
  }
 return count;
}



