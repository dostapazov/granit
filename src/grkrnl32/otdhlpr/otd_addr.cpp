#ifdef __linux__
#include <gklib/otd_addr.h>
#else
#include <otd_addr.h>
#endif

//Сравнение адресов
/*int WINAPI otd_addr_compare(lpotd_addr a1,lpotd_addr a2)
{
 if(a1->addr != a2->addr)
 {
  int res = int(a1->pu)-int(a2->pu);
  if(!res)
  {
    res = int(a1->cp)-int(a2->cp);
    if(!res)
    {
     res = int(a1->fa)-int(a2->fa);
     if(!res)
      res = int(a1->sb)-int(a2->sb);
    }
  }
  return res;
 }
 return 0;//Адреса равны
}*/

DWORD WINAPI otd_addr_type(const lpotd_addr addr)
{
  if(addr)
  {
   if(OTD_ADDR_ISPU(addr))
      return OTD_ADDR_TYPE_PU;
   if(OTD_ADDR_ISCP(addr))
      return OTD_ADDR_TYPE_CP;
   if(OTD_ADDR_ISFA(addr))
      return OTD_ADDR_TYPE_FA;
   if(OTD_ADDR_ISSB(addr))
      return OTD_ADDR_TYPE_SB;
   return OTD_ADDR_TYPE_BROADCAST;
  }
  return OTD_ADDR_TYPE_UNDEF;
 }

int  WINAPI otd_addr_cmp_part(const lpotd_addr a1,const lpotd_addr a2,DWORD tp_lim)
{
 /*Частичное сравнение адресов*/
 int  cmp_res = 0;
 if(tp_lim >= OTD_ADDR_TYPE_BROADCAST)
    tp_lim = KeRTL::MIN(otd_addr_type(a1),otd_addr_type(a2));
 if(tp_lim<OTD_ADDR_TYPE_BROADCAST)
 {
  LPBYTE b1 = (LPBYTE)&a1->addr,b2 =(LPBYTE)&a2->addr;
  for(DWORD i = 0;i<=tp_lim && !cmp_res;i++,b1++,b2++)
   {
    int cmp_v1 = (int)((DWORD)(*b1)),cmp_v2 = (int)((DWORD)(*b2));
    //if(cmp_v1 == OTD_ADDR_MAXVALUE) cmp_v1 = -1;
    //if(cmp_v2 == OTD_ADDR_MAXVALUE) cmp_v2 = -1;
    cmp_res = cmp_v1-cmp_v2;
   }
 }
  return cmp_res;
}

BOOL  WINAPI otd_addr_include(lpotd_addr checked,lpotd_addr mask)
{
 /*
  Проверка включает ли один адрес другой
 */
 BOOL ret = TRUE;
 sotd_addr dwa1(checked->addr),dwa2(mask->addr);
 dwa1.fa&=OTD_FA_ALL;
 dwa2.fa&=OTD_FA_ALL;

 LPBYTE b1 = (LPBYTE)&dwa1.addr,b2 =(LPBYTE)&dwa2.addr;
 for(int i = 0;ret && i<OTD_ADDR_TYPE_BROADCAST;i++,b1++,b2++)
 {
  if(i == OTD_ADDR_TYPE_FA)
  {
  if((*b1 == *b2 )  /*|| *b1 == OTD_FA_ALL */|| *b2 == OTD_FA_ALL)
      continue;
  }
  else
  if( (*b1 == *b2 )  /*|| *b1 == OTD_ADDR_MAXVALUE*/ || *b2 == OTD_ADDR_MAXVALUE)
      continue;
  ret = FALSE;
 }
 return ret;
}



/*
DWORD __fastcall otd_fa2index(DWORD fanum)
{
 DWORD ret = -1;
 switch(fanum)
 {
  case  OTD_FA_DISCRETE    :ret =OTD_FA_DICRETE_IDX       ; break;
  case  OTD_FA_ANALOG      :ret =OTD_FA_ANALOG_IDX        ; break;
  case  OTD_FA_DICRETE_CTRL:ret =OTD_FA_DICRETE_CTRL_IDX  ; break;
  case  OTD_FA_ANALOG_CTRL :ret =OTD_FA_ANALOG_CTRL_IDX   ; break;
  case  OTD_FA_ANALOG_ADJ  :ret =OTD_FA_ANALOG_ADJ_IDX    ; break;
 }
 return ret;
}
*/

