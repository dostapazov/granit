#pragma hdrstop
#include "mediumdb.hpp"

#define PAINTVARS_STREAM_VER_1 1
#define PAINTVARS_STREAM_VER 2

DWORD      __fastcall med_db::paint_vars_stream_ver()
{
  return PAINTVARS_STREAM_VER;
}


void __fastcall read_paint_variant1(KeRTL::TStream &strm,mdb_paint_variant & pv)
{
    LPMDB_PAINT_VARIANT  ptr = &pv;
    strm>>ptr->idx>>ptr->name_id>>ptr->size.cx>>ptr->size.cy
        >>ptr->switched
        >>ptr->image_id>>ptr->image_time
        >>ptr->mask_id>>ptr->mask_time;

}

void __fastcall read_paint_variant2(KeRTL::TStream &strm,mdb_paint_variant & pv)
{
    LPMDB_PAINT_VARIANT  ptr = &pv;
    strm>>ptr->idx>>ptr->name_id>>ptr->size.cx>>ptr->size.cy
        >>ptr->switched
        >>ptr->image_id>>ptr->image_time
        >>ptr->mask_id>>ptr->mask_time>>ptr->multi_position;

}


void       __fastcall med_db::paint_vars_read(KeRTL::TStream & strm,  DWORD ver ,DWORD count)
{
      paint_vars.resize(count);
      Tmdb_paint_variants::iterator ptr = paint_vars.begin();
      Tmdb_paint_variants::iterator end = paint_vars.end();
      while(ptr<end)
      {
        (*ptr).dw_size = sizeof(MDB_PAINT_VARIANT);
        switch(ver)
        {
         case PAINTVARS_STREAM_VER_1:      read_paint_variant1(strm,*ptr); break;
         default                    :      read_paint_variant2(strm,*ptr); break;
        }
        ++ptr;
      }
}

KeRTL::TStream & operator << (KeRTL::TStream & strm ,const mdb_paint_variant & pv)
{
    LPMDB_PAINT_VARIANT  ptr = &pv;
    strm<<ptr->idx<<ptr->name_id<<ptr->size.cx<<ptr->size.cy
        <<ptr->switched
        <<ptr->image_id<<ptr->image_time
        <<ptr->mask_id<<ptr->mask_time<<ptr->multi_position;
    return strm;
}

DWORD    __fastcall med_db::paint_vars_write(KeRTL::TStream & strm)
{
      DWORD ret = 0;
      Tmdb_paint_variants::iterator ptr = paint_vars.begin();
      Tmdb_paint_variants::iterator end = paint_vars.end();
      while(ptr<end)
      {
        strm<<*ptr;
        ++ptr;
        ++ret;
      }
  return ret;
}


 DWORD      __fastcall med_db::paint_vars_enum   (DWORD idx,LPMDB_PAINT_VARIANT pv)
 {
   DWORD ret = GKH_RET_ERROR;
   if(module->check_data_size(pv,sizeof(*pv)))
   {
     TLockHelper l(locker);
     if(idx<this->paint_vars.size())
     {
      mdb_paint_variant & src = paint_vars.at(idx);
      memcpy(pv,&src,sizeof(*pv));
      ret = GKH_RET_SUCCESS;
     }
   }
   return ret;
 }

 DWORD      __fastcall med_db::paint_vars_add    (LPMDB_PAINT_VARIANT pv)
 {
   DWORD ret = GKH_RET_ERROR;
   if(module->check_data_size(pv,sizeof(*pv)))
   {
    TLockHelper l(locker);
    pv->idx        = paint_vars.size();
    paint_vars.push_back(mdb_paint_variant(pv));
    ret = paint_vars_update(pv,MDB_NOTIFY_CODE_PAINTVAR_ADD);
    module->set_ccflags(MDB_CCFL_PAINT_VARS,true);
   }
   return ret;
 }

 DWORD      __fastcall med_db::paint_vars_clear        ()
 {
    TLockHelper l(locker);
    paint_vars.clear();
    module->set_ccflags(MDB_CCFL_PAINT_VARS,true);
    module->notify(MDB_NOTIFY_MASK_PAINTVAR,MDB_NOTIFY_CODE_PAINTVAR_CLEAR,0,0);
    return GKH_RET_SUCCESS;
 }

 DWORD      __fastcall med_db::paint_vars_end_update        (DWORD pv_count)
 {
   module->notify(MDB_NOTIFY_MASK_PAINTVAR,MDB_NOTIFY_CODE_PAINTVAR_END_UPDATE,0,0);
  //Разослать всем подписчикам  изменения варианта отрисовки
   order_vector modems;
     {
      TLockHelper l(locker);
      Tmdb_paint_variants::iterator ptr = paint_vars.begin();
      advance(ptr,pv_count);
      paint_vars.erase(ptr,paint_vars.end());
      module->set_ccflags(MDB_CCFL_PAINT_VARS,true);
      orders.get_modem_list(modems);
     }
     if(modems.size())
     {
      OTDM_PAINT_VARIANTS rpv;
      bzero(&rpv,sizeof(rpv));
      rpv.count = 1;
      rpv.pvars->dw_size = sizeof(rpv.pvars->dw_size);
      rpv.pvars->idx     = -1;
      order_vector::iterator ptr = modems.begin();
      order_vector::iterator end = modems.end  ();
      while(ptr<end)
      {
        mproto_get_paint_variants(ptr->dest_addr,OTDMPROTO_CMD_GET_PAINT_VARIANTS,&rpv,sizeof(rpv));
        ++ptr;
      }
     }
     return GKH_RET_SUCCESS;
     //mproto_get_paint_variants(mph->addr_from,omph->command&OTDM_COMMAND_MASK,mph->data,mph->data_size);

 }

 DWORD      __fastcall med_db::paint_vars_update (LPMDB_PAINT_VARIANT pv,DWORD  notify_code )
 {
   DWORD ret = GKH_RET_ERROR;
   if(module->check_data_size(pv,sizeof(*pv)))
   {
    TLockHelper l(locker);
    mdb_paint_variant p(pv);
    Tmdb_paint_variants::iterator ptr = std::lower_bound(paint_vars.begin(),paint_vars.end(),p);
    if(ptr<paint_vars.end() && ptr->idx == pv->idx)
      {
       p.image_time = pv->image_time = get_file_time(pv->image_id);
       p.mask_time = pv->mask_time   = get_file_time(pv->mask_id);

       mdb_paint_variant &paint_var = *ptr;
       DWORD ch_mask  = 0;
       if(paint_var.size.cx != p.size.cx || paint_var.size.cy != p.size.cy)
          ch_mask|= MDB_PAINT_VARIANT_SIZE_MASK;
       if(paint_var.image_time!=pv->image_time)
          ch_mask|= MDB_PAINT_VARIANT_IMAGE_MASK;
       if(paint_var.mask_time!=pv->mask_time)
          ch_mask|= MDB_PAINT_VARIANT_MASK_MASK;
       if(paint_var.switched!=pv->switched)
          ch_mask|= MDB_PAINT_VARIANT_SWITCHED_MASK;
       if(paint_var.multi_position !=pv->multi_position)
          ch_mask|= MDB_PAINT_VARIANT_MULTIPOS_MASK;
       paint_var = p;
       ret = GKH_RET_SUCCESS;
       module->notify(MDB_NOTIFY_MASK_PAINTVAR,notify_code,&paint_var,sizeof(mdb_paint_variant));
       p = paint_var;
       module->set_ccflags(MDB_CCFL_PAINT_VARS,true);
      }
   }
   return ret;
 }

 bool       __fastcall med_db::paint_vars_search       (DWORD idx,Tmdb_paint_variants::iterator * ret_ptr)
 {
   bool ret = false;
   mdb_paint_variant p;
   p.idx = idx;
   Tmdb_paint_variants::iterator ptr = std::lower_bound(paint_vars.begin(),paint_vars.end(),p);
   if(ret_ptr) *ret_ptr = ptr;
   if(ptr<paint_vars.end() && ptr->idx == idx)
       ret = true;
  return ret;
 }

 #pragma warn -8057
 void       __fastcall med_db::paint_vars_handle_change(mdb_paint_variant & paint_var,DWORD ch_mask)
 {
     //Разослать всем подписчикам  изменения варианта отрисовки
     //TLockHelper l(locker);
 }
 #pragma warn .8057

