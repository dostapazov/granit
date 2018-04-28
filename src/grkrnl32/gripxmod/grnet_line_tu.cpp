
#pragma hdrstop
#include "gripxmod_main.hpp"
#include <stdlib.h>
#include <otd_proto.h>


 void     __fastcall grnet_line::tu_query   (lp_gratony_kadr kadr)
 {
  if(this->tu_enabled)
  {
   lpobject_tu otu = (lpobject_tu) kadr;
   otu->remote     = TRUE;
  //send_data(CmEnableTU,otu,sizeof(*otu));
   BYTE  buf[1024];
   sotd_addr sa(pu_number,otu->kp,otu->tlm_type,otu->sb);
   DWORD len = otd_proto_format_tutr(buf,sizeof(buf),&sa,otu->obj-1, OTD_TUTR_CMDSELECT ,0, NULL,0,0,NULL);
   queue_rxdata(FA_OTD,buf,len,false,0);
  }
 }

 void     __fastcall grnet_line::tu_begin   (lp_gratony_kadr kadr)
 {
  /*
    BYTE  otd_kadr[1024];
   lpobject_tu otu = (lpobject_tu) kadr;
   otu->remote     = TRUE;
   send_data(CmEnableTU,otu,sizeof(*otu));
  */
 }

 void     __fastcall grnet_line::tu_reset   (lp_gratony_kadr kadr)
 {
  lpobject_tu otu = (lpobject_tu) kadr;
  //send_data(CmResetTU,otu,sizeof(*otu));
  sotd_addr sa(pu_number,otu->kp,otu->tlm_type,otu->sb);
  BYTE  buf[1024];
  DWORD len = otd_proto_format_tutr(buf,sizeof(buf),&sa,otu->obj-1, OTD_TUTR_CMDDESELECT ,0, NULL,0,0,NULL);
  queue_rxdata(FA_OTD,buf,len,false,0);
 }


/**********************************************************
 Правильный вариант для перекодировки ТУ в модуле СЦ-Гранит
*************************************************************/


void     __fastcall grnet_line::tu_on_more (lp_gratony_kadr kadr)
 {
  if(this->tu_enabled)
  {
  lpobject_tu otu = (lpobject_tu) kadr;
  otu->remote     = TRUE;
  //Сформировать кадр для отправки  модеммеру
  lock();
  sotd_addr sa(pu_number,otu->kp,otu->tlm_type,otu->sb);
  unlock();
  char  buf[1024];    // otd.h
  DWORD len = otd_proto_format_tutr((LPBYTE)buf,sizeof(buf),&sa,otu->obj-1,OTD_TUOP_ON,0, NULL,0,0,NULL);
  queue_rxdata(FA_OTD,(LPBYTE)buf,len,false,0);
  }
 }

 void     __fastcall grnet_line::tu_off_less(lp_gratony_kadr kadr)
 {
  if(this->tu_enabled)
  {
  lpobject_tu otu = (lpobject_tu) kadr;
  otu->remote     = TRUE;

  //Сформировать кадр для отправки  модеммеру
  lock();
  sotd_addr sa(pu_number,otu->kp,otu->tlm_type,otu->sb);
  unlock();
  char  buf[1024];    // otd.h
  DWORD len = otd_proto_format_tutr((LPBYTE)buf,sizeof(buf),&sa,otu->obj-1,OTD_TUOP_OFF,0, NULL,0,0,NULL);
  this->queue_rxdata(FA_OTD,(LPBYTE)buf,len,false,0);
  }
 }

 void     __fastcall grnet_line::tu_cancel  (lp_gratony_kadr kadr)
 {
  if(this->tu_enabled)
  {
  lpobject_tu otu = (lpobject_tu) kadr;
  otu->remote     = TRUE;
  //Сформировать кадр для отправки  модеммеру
  lock();
  sotd_addr sa(pu_number,otu->kp,otu->tlm_type,otu->sb);
  unlock();
  char  buf[1024];    // otd.h
  DWORD len = otd_proto_format_tutr((LPBYTE)buf,sizeof(buf),&sa,otu->obj-1,OTD_TUTROP_CANCEL,0 , NULL,0,0,NULL);
  this->queue_rxdata(FA_OTD,(LPBYTE)buf,len,false,0);
  }
 }

  DWORD __fastcall grnet_line::get_tuobj_active_state (otd_addr & addr,DWORD param)
  {
   DWORD ret = 0;
   sotd_param_addr pa(addr,param);
   if(active_tu.count(pa))
     ret = active_tu[pa];
   return ret;
  }

  void  __fastcall grnet_line::set_tuobj_active_state (otd_addr & addr,DWORD param,DWORD state)
  {
   sotd_param_addr pa(addr,param);
    if(state)
       active_tu[pa] = state;
    else
    {
     if(active_tu.count(pa))
        active_tu.erase(pa);
    }
  }



