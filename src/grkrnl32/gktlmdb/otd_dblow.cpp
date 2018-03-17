#pragma hdrstop
#include "otd_dblow.hpp"
#include <gktlmdb.h>



otd_dblow::otd_dblow()
{

};


void    __fastcall otd_dblow::delete_sop(proto_pointer ptr)
{
  delete [] (LPBYTE)ptr->op.addr;
}

void  __fastcall otd_dblow::release()
{
  wr_lock(INFINITE);
  proto_pointer  pbeg = begin(),pend = end();
  while(pbeg<pend)
  {
   delete_sop(pbeg);
   pbeg++;
  }
  clear();
  wr_unlock();
}

 proto_pointer   __fastcall otd_dblow::create_group(otd_addr & addr)
 {
    //Создание групповых объектов КП и ПУ

    wr_lock();
    sotd_addr sa(addr);
    proto_pointer ret = otd_list.end();

  DWORD addr_type = sa.addrtype();
 if(addr_type<OTD_ADDR_TYPE_BROADCAST)
 {
    while(addr_type>OTD_ADDR_TYPE_CP)
        {
         sa = sa.owner_addr();
         addr_type = sa.addrtype();
        }
    ret = this->find(sa);
    if(ret>=otd_list.end())
    {
     DWORD name_len = TLMDBLOW_MAXNAMELEN;
     DWORD need_sz  =  otd_proto_calc_size(OTD_PROTO_PART_DIAG|OTD_PROTO_PART_NAME|OTD_PROTO_PART_KPK,0,0,0,0,name_len);
     LPBYTE ptr = new BYTE[need_sz];
     otd_proto op;
     op.dw_size = sizeof(op);
     if(ptr && otd_proto_format(ptr,need_sz,OTD_PROTO_PART_DIAG|OTD_PROTO_PART_NAME|OTD_PROTO_PART_KPK,0,0,0,0,0,name_len,&op))
     {
       op.addr->addr = sa.addr;
       op.addr->fa   = -1;
       switch(sa.addrtype())
       {
        case OTD_ADDR_TYPE_CP: *op.diag  = OTD_DIAG_CPMASK;break;
        case OTD_ADDR_TYPE_PU: *op.diag  = OTD_DIAG_PUMASK;break;
       }
       *op.diag  |= OTD_DIAG_NODATA;
       ret = insert(op);
       item_changed(ret,0,-1,0);
       if(addr_type>OTD_ADDR_TYPE_PU)
       {
          create_group(*&sa.owner_addr());
          ret = find(sa);
       }
       update_diag(*&sa.owner_addr());
     }
    }
  }
    wr_unlock();
    return ret;
 }


 proto_pointer   __fastcall otd_dblow::create(otd_addr & addr,otd_proto_param & src_opp)
 {
   wr_lock();
   proto_pointer ret  = end();
   DWORD  need_size = otd_proto_calc_size_ex(&src_opp,0);
   LPBYTE ptr       = new BYTE[need_size];
   if(ptr)
   {
   otd_proto op;
   op.dw_size = sizeof(op);
   src_opp.parts|=OTD_PROTO_PART_KPK;
   if(otd_proto_format_ex(ptr,need_size,&src_opp,0,&op))
    {
     op.addr->addr = addr.addr;
     *op.diag      = OTD_DIAG_FULL_MASK;
     ret = insert(op);
     item_changed(ret,0,-1,0);
    }
   }
   wr_unlock();
   return ret;
 }

 DWORD  __fastcall otd_dblow::create (otd_proto & src)
 {
   /*Создание и нового элемента и его копирование*/
   DWORD ret = -1;
   otd_proto_param opp;
   ZeroMemory(&opp,sizeof(opp));
   opp.dw_size = sizeof(opp);
   if(otd_proto_getparam(&src,&opp))
   {
    DWORD addr_type = otd_addr_type(src.addr);
    if(addr_type == OTD_ADDR_TYPE_SB)
    {
    opp.parts|=OTD_PROTO_PART_DIAG|OTD_PROTO_PART_TIMESTAMP|OTD_PROTO_PART_KPK;
    opp.parts|=(OTD_PROTO_PART_PERSONAL_DIAG|OTD_PROTO_PART_PERSONAL_CHMASK);
    opp.cm_param = opp.pd_param = opp.dt_param;
    opp.cm_param.type = opp.pd_param.type = OTD_ANALOG_WORD;
    wr_lock();

    proto_pointer ptr = create(*src.addr,opp);
    ret = distance(otd_list.begin(),ptr);
    wr_unlock();
    }
    create_group(*src.addr);
   }
   return ret;
 }

 DWORD   __fastcall otd_dblow::recreate  (proto_pointer  ptr,DWORD lo_obj,DWORD hi_obj,wchar_t * name)
 {
   //Пересоздание 
   DWORD ret = 0;
   wr_lock();
   sotd_proto sop(ptr->op);
   otd_proto_param  opp;
   ZeroMemory(&opp,sizeof(opp));
   opp.dw_size = sizeof(opp);
   otd_proto_getparam  (&sop.op,&opp);
   opp.pd_param.first = opp.cm_param.first = opp.dt_param.first = lo_obj;
   opp.pd_param.count = opp.cm_param.count = opp.dt_param.count = hi_obj-lo_obj+1;
   wchar_t old_name[MAX_PATH];

   if(!name && sop.op.name)
      {
       otd_text_getwstr(sop.op.name,old_name,sizeof(old_name)/sizeof(wchar_t));
       name = old_name;
      } 

   if(name && *name)
     opp.parts |=OTD_PROTO_PART_NAME, opp.name_sz = KeRTL::MAX(opp.name_sz,(DWORD)lstrlenW(name));
   opp.parts   |= OTD_PROTO_PART_PROTOEXTENSION;
   opp.ext_size = sizeof(dblow_ext);
   opp.ext_type = OTD_BYTE|sizeof(dblow_ext);
   DWORD new_sz = otd_proto_calc_size_ex(&opp,0);
   LPBYTE buf = new BYTE[new_sz];
   if(buf)
    {
     otd_proto_format_ex(buf,new_sz,&opp,0,&ptr->op);
     ptr->op.addr->addr = sop.op.addr->addr;
     *ptr->op.diag = *sop.op.diag;
     if(ptr->op.data)
     otd_copy(ptr->op.data,sop.op.data,opp.dt_param.first,opp.dt_param.count);
     if(ptr->op.personal_diag)
     otd_copy(ptr->op.personal_diag,sop.op.personal_diag,opp.dt_param.first,opp.dt_param.count);
     if(ptr->op.personal_chmask)
     otd_copy(ptr->op.personal_chmask,sop.op.personal_chmask,opp.dt_param.first,opp.dt_param.count);
     if(ptr->op.name)
      {
       if(name && *name)
         otd_text_set(ptr->op.name,name);
         else
         otd_text_copy(ptr->op.name,sop.op.name);
      }
      if(ptr->op.time_stamp)
         *(__int64*)ptr->op.time_stamp = GetTime(false);
     this->make_static(ptr,TRUE);
     update_diag(*ptr->op.addr);
     item_changed(ptr,opp.parts,0,-1);
     delete [] (LPBYTE)sop.op.addr;
     ret = opp.parts;
     ptr = get_owner(ptr);
     if(ptr<end() && is_dynamic(ptr))
      {
        recreate(ptr,0,0,NULL);
      }
    }
    else
    {
     //Востатовить
     memcpy(&ptr->op,&sop.op,sizeof(sop.op));
    }
   wr_unlock();
   return ret;
 }

 DWORD   __fastcall otd_dblow::make_static   (proto_pointer  ptr,BOOL st)
 {
   DWORD ret = 0;
   wr_lock();
   if(!ptr->op.extension)
   {
       dblow_ext * de = (dblow_ext*) ptr->op.extension->block_data;
       if(st)
       {
        if(de)
          {
           if(!de->is_static)
           {
           de->is_static = TRUE;
           ret = TRUE;
           }
          }
         else
          ret = recreate(ptr,ptr->op.data->numbers.loN,ptr->op.data->numbers.hiN,NULL);
       }
       else
       {
        if(de && de->is_static)
        {
         de->is_static = FALSE;
         ret = TRUE;
        }
       }
   }
   wr_unlock();
   return ret;
 }


 DWORD  __fastcall otd_dblow::find_byaddr         (otd_addr & addr)
 {
  DWORD ret = -1;
  rd_lock();
   if(this->otd_list.size())
   {
     proto_pointer ptr = find(addr);
     if(ptr<end())
       ret = distance(otd_list.begin(),ptr);
   }
  rd_unlock();
  return ret;
 }

 DWORD  __fastcall otd_dblow::items_enum(otd_addr & addr,DWORD index)
 {
  DWORD ret = -1;
  proto_pointer low,high;
  rd_lock();
  if(get_range(addr,low,high))
  {
   advance(low,index);
   if(low<high )
   {
     ret = distance(begin(),low);
   }
  }
  rd_unlock();
  return ret;
 }

 DWORD  __fastcall otd_dblow::get_item  (DWORD offset,DWORD parts,DWORD low,DWORD hi,LPBYTE buffer,DWORD buffer_sz)
 {
  //Получение объекта БД по смещению от начала
  DWORD ret = 0;
  rd_lock();

    //get_parts_addr()
   if(offset<otd_list.size())
   {
    proto_pointer ptr = otd_list.begin();
    advance(ptr,offset);

    if(OTD_ADDR_ISSB(ptr->op.addr))
       ret = get_parts_idx(offset,parts,low,hi,buffer,buffer_sz);
    else
    {
     //Подучение данных группы объектов
      proto_pointer low_ptr = otd_list.begin(),hi_ptr = ptr;
      if(get_range(*ptr->op.addr,low_ptr,hi_ptr))
      {
       otd_proto       op;
       ZeroMemory(&op,sizeof(op));
       op.dw_size   = sizeof(op);
       otd_proto_param opp;
       ZeroMemory(&opp,sizeof(opp));
       opp.dw_size   = sizeof(opp);
       opp.parts = OTD_PROTO_PART_DIAG;
       if(parts&(OTD_PROTO_PART_PERSONAL_DIAG))
        {
         opp.pd_param.type  = OTD_GROUP_PD;
         opp.pd_param.first = 0;
         opp.pd_param.count = get_count(*ptr->op.addr);
         opp.parts|= OTD_PROTO_PART_PERSONAL_DIAG;
        }
       if((parts&OTD_PROTO_PART_NAME) && ptr->op.name )
        {
         opp.parts|=OTD_PROTO_PART_NAME;
         opp.name_sz = OTD_GET_TEXT_LENGTH(ptr->op.name);
        }
        if((parts&OTD_PROTO_PART_PROTOEXTENSION) && ptr->op.extension)
           {
            opp.parts|=OTD_PROTO_PART_PROTOEXTENSION;
            opp.ext_size = ptr->op.extension->block_size;
            opp.ext_type = ptr->op.extension->block_type;
           }
            
        ret = otd_proto_format_ex(buffer,buffer_sz,&opp,0,&op);
        if(int(ret)>0)
        {
          op.addr->addr = ptr->op.addr->addr;

          if(opp.name_sz)
             otd_text_copy(op.name,ptr->op.name);
          if(op.extension)
            memcpy(op.extension,ptr->op.extension,OTD_BLOCK_GET_SIZE(op.extension));
          if(op.personal_diag)
          {
            lpotd_group_pd_entry pd_entry = (lpotd_group_pd_entry) op.personal_diag->data;
            DWORD addr_type = otd_addr_type(op.addr);
            while(low_ptr<hi_ptr)
            {
             DWORD type_diff = otd_addr_type(low_ptr->op.addr) - addr_type;
             if((addr_type == OTD_ADDR_TYPE_CP && type_diff == 2) ||(!addr_type && type_diff==1))
                {
                 pd_entry->addr.addr =   low_ptr->op.addr->addr;
                 pd_entry->diag      =   get_diag(low_ptr,false); 
                 ++pd_entry;
                }
             ++low_ptr;
            }
          }

          *op.diag      = get_diag(ptr,op.personal_diag ? false:true); // get_diag_range(low_ptr,hi_ptr);
        }
      }
    }
   }
  rd_unlock();
  return ret;
 }

 #pragma warn -8057

 DWORD  __fastcall otd_dblow::copy_from      (otd_addr & mask,otd_storage & src,DWORD parts,DWORD lo_obj,DWORD hi_obj)
 {
  //Копирование из БД источника по маске
  DWORD ret = 0;
  wr_lock();
  proto_pointer low,hi;
  if(src.get_range(mask,low,hi))
  {
    while(low<hi)
    {
    sotd_proto sop(*low);
    if(otd_addr_include(sop.op.addr,&mask))
    {
     create(sop());
     ret++;
    }
    low++;
    }
  }
  wr_unlock();
  return ret;
 }

 #pragma warn .8057

 DWORD  __fastcall otd_dblow::copy_to      (otd_addr  & mask,otd_storage & dest,DWORD parts,DWORD lo_obj,DWORD hi_obj)
 {
   DWORD ret = 0;
   sotd_addr addr = mask;
   if((addr.fa&=OTD_FA_ALL)==OTD_FA_ALL)
      addr.fa = OTD_ADDR_MAXVALUE;
   parts&=~OTD_PROTO_PART_PROTOEXTENSION;
   parts|=OTD_PROTO_PART_KPK;
   rd_lock();
   proto_pointer l_ptr = otd_list.begin(),h_ptr = otd_list.end();
   DWORD count = get_range(mask,l_ptr,h_ptr);
   if(count)
     {
      dest.reserve(count);
      while(l_ptr<h_ptr)
      {
       if(otd_addr_include(l_ptr->op.addr,&addr))
       {
        if((*l_ptr->op.ver_parts)&(parts&OTD_PROTO_PARTSMASK))
          {
           DWORD sz   = abs(get_parts(l_ptr,parts,lo_obj,hi_obj,0,0));
           LPBYTE copy_buff = new BYTE[sz];
           if(copy_buff)
           {
            get_parts(l_ptr,parts,lo_obj,hi_obj,copy_buff,sz);
            sotd_proto sop(copy_buff,sz);
            if(sop.op.kpk)
               otd_proto_protect(&sop.op,0,OTD_DEF_POLINOM);
            dest.insert(sop.op);
           }
          } 
        ret++;
       }
       ++l_ptr;
      }
     }
   rd_unlock();
   return ret;
 }

 DWORD  __fastcall otd_dblow::remove (otd_addr & mask)
 {
  //Удаление по маске
  vector<proto_pointer> vptr;
  DWORD ret = 0;
  wr_lock();
  proto_pointer low,hi;
  //Составление списка
  if(get_range(mask,low,hi))
  {
    while(low<hi )
    {
     if(otd_addr_include(low->op.addr,&mask))
       vptr.push_back(low);
      ++low;
    }
  }

   //Реальное удаление
   vector<proto_pointer>::iterator b = vptr.begin(),e = vptr.end();
   if(vptr.size())
   {
     while(b<e)
     {
      this->delete_sop(*b);
      otd_storage os;
      erase(*b);
      ++b;
     }
   }
  wr_unlock();
  return ret;
 }


#pragma warn -8057
DWORD   __fastcall otd_dblow::item_changed  (proto_pointer  ptr,DWORD ch_parts,DWORD lo_obj,DWORD hi_obj)
{
 lpotd_block save_ext = ptr->op.extension;
 ptr->op.extension = NULL;
 otd_proto_protect(&ptr->op,ptr->op.proto_size,OTD_DEF_POLINOM);
 ptr->op.extension = save_ext;
 return 0;
}
 #pragma warn .8057

bool    __fastcall otd_dblow::is_static     (proto_pointer ptr)
{
 //return ((ptr<end()) && ptr->op.extension && ((dblow_ext*)ptr->op.extension->block_data)->is_static) ? true:false;
 return !is_dynamic(ptr);
}

DWORD   __fastcall otd_dblow::stream_write  (TStream & strm)
{
  DWORD ret = 0;
  DWORD save_position = strm.GetPosition();
  strm<<ret;
  rd_lock();
  proto_pointer beg_ptr = begin(), end_ptr = this->end();
  while(beg_ptr<end_ptr)
  {
   if(is_static(beg_ptr))
   {
      ++ret;
      otd_proto_param  opp = {0};
      opp.dw_size = sizeof(opp);
      otd_proto_getparam(&beg_ptr->op,&opp);
      strm<<beg_ptr->op.addr->addr;
      strm.Write(&opp,sizeof(opp));
      if(opp.parts&OTD_PROTO_PART_NAME)
      {
        wchar_t  name[MAX_PATH];
        *name = 0;
        otd_text_getwstr(beg_ptr->op.name,name,sizeof(name)/sizeof(wchar_t));
        strm.write_string(name);
      }
      if(beg_ptr->op.extension!=NULL)
      {
       dblow_ext* ext = (dblow_ext*)beg_ptr->op.extension->block_data;
       strm.Write(ext,sizeof(*ext));

      }
   }
   ++beg_ptr;
  }
  rd_unlock();
  strm.Seek(save_position,TStream::smFromBegin);
  strm<<ret;
  strm.Seek(0,TStream::smFromEnd);
  return ret;
}

DWORD   __fastcall otd_dblow::stream_read   (TStream & strm,DWORD ver)
{
    DWORD count = 0;
    strm>>count;
    otd_proto_param  opp;
    sotd_proto sop;
    opp.dw_size   = sizeof(opp);
    wr_lock();
    while(count--)
    {
     wchar_t  name[MAX_PATH];
    *name = 0;
     otd_addr   addr;
     LPBYTE  buf;
     DWORD   size;
     strm>>addr.addr;
     if(addr.fa == OTD_FA_ALL)
        addr.fa = -1;
     strm.Read(&opp,sizeof(opp));
      if(opp.parts&OTD_PROTO_PART_NAME)
      {
        wchar_t * _name = name;
        strm.read_string(_name);
      }
     size = otd_proto_calc_size_ex(&opp,0);
     buf  = new BYTE [size];
     if(buf)
     {
       otd_proto_format_ex(buf,size,&opp,0,&sop());
       sop().addr->addr = addr.addr;
       if(sop().name)
         otd_text_set(sop().name,name);
       if(sop().extension!=NULL)
       {
        dblow_ext* ext = (dblow_ext*)sop().extension->block_data;
        if(ver>0)
        {
         strm.Read(ext,sizeof(*ext));
        }
        else
        ext->is_static = true;
       }
       insert(sop());
     }
    }
    wr_unlock();
    return count;
}

bool   __fastcall otd_dblow::is_dynamic    (proto_pointer ptr)
{

  bool ret = true;
  rd_lock();
  if( ptr<otd_list.end())
   {
      ret = ptr->op.extension == NULL ? true:false;
      if(!ret)
      {
       dblow_ext * dbext = (dblow_ext *)ptr->op.extension->block_data;
       ret = dbext->is_static ? false:true;
      }
   }
  rd_unlock();
  return ret;
 }

         DWORD    __fastcall otd_dblow::get_owner_addr(otd_addr & addr  )
         {
          sotd_addr sa(addr);
          sa = sa.owner_addr();
          if(sa.addrtype()>OTD_ADDR_TYPE_CP)
             sa = sa.owner_addr();
          return sa.addr;
         }

 proto_pointer    __fastcall otd_dblow::get_owner     (proto_pointer ptr)
 {
   sotd_addr sa(get_owner_addr(*ptr->op.addr));
   return this->find(sa);
 }









