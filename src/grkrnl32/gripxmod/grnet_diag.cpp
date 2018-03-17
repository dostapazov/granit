#pragma hdrstop
#include "gripxmod_main.hpp"

  /*Формат слова диагностики Гранита*/
  /*
  0x8000 КП не работает
  0x7F00   признаки неработоспособности 7и ГРУПП тит
  0x00FF   признаки неработоспособности 8и ГРУПП тc
  */

  void __fastcall TGRNetModule::diag_notify(DWORD cp_num,DWORD count)
  {
   const DWORD DIAG_NOTIFY_BUFF_SZ = (sizeof(NOTIFY_CODE)+sizeof(grdiag_map));
   if(need_notify(GRNET_NOTIFY_READY))
   {
    BYTE    buffer[DIAG_NOTIFY_BUFF_SZ];
    LPNOTIFY_CODE nc = (LPNOTIFY_CODE)buffer;
    nc->dw_size      = sizeof(*nc)-sizeof(nc->notify_data)+count*sizeof(grdiag_item);
    nc->from_module  = _handle;
    nc->notify_code  = MAKELONG((WORD)cp_num,(WORD)count);
    lp_grdiag_item  gdi =(lp_grdiag_item)nc->notify_data;
    diag_table.get_items(gdi,cp_num,count);
    notify_handles(nc,GRNET_NOTIFY_READY);
   }
  }



void    __fastcall TGRNetModule::diag_reset()
{
  diag_table.reset();
  diag_notify(0,MAX_CP_COUNT);
}

LRESULT __fastcall TGRNetModule::diag_get_items (lp_grdiag_item gdi, DWORD begin,DWORD cnt)
{
   return diag_table.get_items(gdi,begin,cnt) ? GKH_RET_SUCCESS:GKH_RET_ERROR;
}


 void __fastcall diag_map::reset()
 {
  /*Сброс готовности*/
  TLockHelper l(locker);
  for(DWORD i = 0;i <MAX_CP_COUNT;i++)
       {
        gd_init_item(gdmap.items+i);
       }
 }

 bool __fastcall diag_map::get_items(lp_grdiag_item gdi,DWORD begin ,DWORD count )
 {
  if(gdi && (begin+count)<=MAX_CP_COUNT)
  {
     TLockHelper l(locker);
     memcpy(gdi,gdmap.items+begin,count*sizeof(*gdi));
     return true;
  }
  return false;
 }

 DWORD __fastcall diag_map::set_grdiag(DWORD cp_num,WORD diag)
 {
  DWORD ret = 0;
  if(cp_num<OTD_ADDR_MAXVALUE)
  {
    TLockHelper l(locker);
    ret = gd_set_diag(gdmap.items+cp_num,diag);
  }
  return ret;
 }

 bool  __fastcall diag_map::get_grdiag(DWORD cp_num,lp_grdiag_item gdi)
 {
  bool ret = false; 
  if(gdi && cp_num<OTD_ADDR_MAXVALUE)
  {
    TLockHelper l(locker);
    gdi->state = gdmap.items[cp_num].state;
    gdi->mask  = gdmap.items[cp_num].mask;
  }
  return ret;
 }

 WORD  __fastcall diag_map::get_grdiag(DWORD cp_num)
 {
  WORD ret = -1;
  if(cp_num<OTD_ADDR_MAXVALUE)
  {
    TLockHelper l(locker);
    ret = gdmap.items[cp_num].state|(~gdmap.items[cp_num].mask);
  }
  return ret;
 }

 WORD  __fastcall diag_map::get_grmask(DWORD cp_num)
 {
  WORD ret = 0;
  if(cp_num<OTD_ADDR_MAXVALUE)
  {
    TLockHelper l(locker);
    ret = gdmap.items[cp_num].state ;
  }
  return ret;
 }

DWORD __fastcall diag_map::get_otd_diag(DWORD cp,DWORD fa,DWORD grp)
{
 DWORD ret = OTD_DIAG_NODATA;
 if(cp<OTD_ADDR_MAXVALUE)
 {
  TLockHelper l(locker);
  if(gdmap.items[0].state&GRDIAG_CP)
    ret = OTD_DIAG_PUCONNECT|OTD_DIAG_PURESPOND;
   else
   {
    lp_grdiag_item gdi = gdmap.items+cp;
     ret = gd_get_otd_diag(gdi,fa,grp);
   }
 }
 return ret;
}

DWORD __fastcall diag_map::mark_exist(DWORD cp,DWORD fa,DWORD grp)
{
 DWORD ret = 0;
 if(cp<OTD_ADDR_MAXVALUE)
 {
  TLockHelper l(locker);
  ret = gd_mark_exist(gdmap.items+cp,fa,grp);
 }
 return ret;
}

DWORD __fastcall diag_map::set_otd_diag(lpotd_addr addr,DWORD otd_diag)
{
  //Установка диагностики Гранита из OTD_DIAG
  //Возрат маска изменений
  DWORD ret = 0;
  if(addr->cp<OTD_ADDR_MAXVALUE )
  {
      TLockHelper l(locker);
      ret = gd_set_otd_diag(gdmap.items+addr->cp,addr->fa,addr->sb,otd_diag);
  }
  else
  {
   grdiag_item * gdi_beg = gdmap.items, *gdi_end = gdi_beg+ MAX_CP_COUNT;
   gdi_beg++; //  с первого КП
   for(;gdi_beg<gdi_end;gdi_beg++)
   {
    ret|= gd_set_otd_diag(gdi_beg,addr->fa,addr->sb,otd_diag);
   }
  }
  return ret;
}








