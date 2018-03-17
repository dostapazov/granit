#include <algorithm>
#include <stdio.h>
#pragma hdrstop
#include "granit.hpp"


 gr_cp::gr_cp(sc_floor * own,DWORD num):
              owner(own),
              number(num),
              flags(0),
              send_wait_time(1000),
              diag(OTD_DIAG_NODATA|OTD_DIAG_PUCONNECT|OTD_DIAG_PURESPOND|OTD_DIAG_CPCONNECT|OTD_DIAG_CPRESPOND),
              tx_time(0),
              rx_timer(90*1000),
              rx_timer_value(90),
              time_stamp(0),
              tx_deliver_time(0),
              tx_current_len(0),
              tx_byte_time(0)

 {

   name[0]  = 0;
   ZeroMemory(tu_active,sizeof(tu_active));
 }

 gr_cp::~gr_cp()
 {
  release();
 }

 void       __fastcall gr_cp::release ()
 {
  tx_queue.DropData();

  //TODO delete all groups and sublocks
  gr_group_list::iterator begin = grp_list.begin(),end = grp_list.end();
  while(begin<end)
  {
   delete *begin;
   begin++;
  }

 }


 DWORD      __fastcall gr_cp::set_diag  (DWORD df,bool set)
 {
   //Установка диагностики
   otd_diag _dv;
   _dv.diag = diag;
   if(set && (df&(OTD_DIAG_PUCONNECT|OTD_DIAG_PURESPOND)))
                df|=OTD_DIAG_CPRESPOND|OTD_DIAG_CPCONNECT|OTD_PART_DIAG_PARAM;
   DWORD new_diag =  diag&(~OTD_DIAG_NODATA);
   new_diag  = set ? (new_diag|df) : (new_diag&~df);


   DWORD changes  = new_diag^diag;
   _dv.diag = new_diag;
   if(changes )
   {
     diag = new_diag;
     if(changes & OTD_DIAG_MASK)
     {
      gr_group_list::iterator gbeg = grp_list.begin(),gend = grp_list.end();
      while(gbeg<gend)
       {
        (*gbeg)->set_diag(df&OTD_DIAG_MASK,set);
         gbeg++;
       }

      if(changes)
      {
       if(set)
       {
        if(changes&OTD_DIAG_MASK)
         tx_queue.DropData();
        if(/*!(diag&(OTD_DIAG_PUCONNECT|OTD_DIAG_PURESPOND))&&*/
         (changes&(OTD_DIAG_CPCONNECT|OTD_DIAG_CPRESPOND|OTD_DIAG_PUCONNECT|OTD_DIAG_PURESPOND)) )
        {
         DWORD len;BYTE diag_kadr[MAX_PATH];
         len = get_otd_data(get_otd_addr(),OTD_PROTO_PART_DIAG,diag_kadr,sizeof(diag_kadr),NULL);
         queue_rxdata (FA_OTD,diag_kadr,len);
         log_diag();
        }
       }
       else
       {
        scan_diag();
        this->refresh(-1);
       } 
       modem_notify(MNCODE_SC_DATACHANGED,get_otd_addr(),OTD_PROTO_PART_DIAG|OTD_PROTO_PART_PERSONAL_DIAG);
      }
     }
   }
   return changes;
 }


 void       __fastcall gr_cp::update_diag()
 {
  if(scan_diag())
     modem_notify(MNCODE_SC_DATACHANGED,get_otd_addr(),OTD_PROTO_PART_DIAG|OTD_PROTO_PART_PERSONAL_DIAG);
 }

 DWORD      __fastcall gr_cp::scan_diag ()
 {
  DWORD new_diag=(diag&OTD_DIAG_MASK);

  gr_group_list::iterator begin = grp_list.begin(),end = grp_list.end();
  while(begin<end)
  {
    DWORD grp_diag =(*begin)->get_diag();
    new_diag|=(grp_diag&(OTD_STATE_MASK|OTD_DIAG_STATE_TUTR));

    if(grp_diag&OTD_DIAG_MASK)
       new_diag|=(OTD_PART_DIAG_PARAM);

    begin++;
  }

  DWORD changes = new_diag^diag;
  diag = new_diag;
  return changes;
 }

 bool       __fastcall gr_cp::check_diag(DWORD df,bool all)
 {
   DWORD res = diag&df;
   if(res)
    {
      if(all)  if(res !=df)   return false;
       return true;
    }
    return false;
 }




 bool       __fastcall gr_cp::floor_tx(LPBYTE data,DWORD len,bool  first)
 {
  if(is_tx_ready())
     return  do_floor_tx(data,len);
     else
     tx_queue.PutIntoQueue(data,len,first);

   return false;
 }

 bool       __fastcall gr_cp::do_floor_tx(LPBYTE data,DWORD len)
 {
   if(owner)
   {
    tx_current_len = len;
    tx_time = GetTime(false);
    owner->floor_tx(data,len);

    return true;
   }
   return false;
 }


 void       __fastcall gr_cp::on_timer(__int64 sys_time )
 {
   if(tx_time )
   {
    /*
      Если не пришла готовность  то таймаут отправки устанавливается
      4-x кратному времени отправки
    */
    DWORD cmp_time = send_wait_time < 0 ? (tx_current_len * tx_byte_time)*abs(send_wait_time) :(DWORD)send_wait_time;//
    if(!cmp_time || cmp_time > 5000)
        cmp_time = 1000;
    if( NS100_MSEC(sys_time-tx_time)>cmp_time)
       continue_floor_tx(sys_time);
   }

   rx_timer-=GRSC_TIMER_RESOLUTION;
   if(int(rx_timer)<=0)
    {
      //refresh(-1);
      set_diag(OTD_DIAG_CPRESPOND,true);
      rx_timer = rx_timer_value; // к миллисекундам
      rx_timer *=1000;
    }
    group_timer   (sys_time);
    tutr_timer    (sys_time);
 }

 void         __fastcall gr_cp::group_timer(__int64 sys_time)
 {
  bool diag_scan = false;
  gr_group_list::iterator grp_beg = grp_list.begin(),grp_end = grp_list.end();
  bool  recall_afb[15] = {false};
  while(grp_beg<grp_end)
  {
    gr_group * grp = *grp_beg;
    if(grp->on_timer(sys_time))
      {
       diag_scan=true;
       DWORD cafb = grp->get_call_afb();
       if(cafb<sizeof(recall_afb)/sizeof(bool)) //Защита от ошибочного CALL_AFB
          recall_afb[cafb]=true;
      }
    grp_beg++;
  }

  if(diag_scan)
    {
     if(scan_diag())
        modem_notify(MNCODE_SC_DATACHANGED,get_otd_addr(),OTD_PROTO_PART_DIAG);
     for(DWORD i = 0;i<sizeof(recall_afb)/sizeof(bool);i++)
        {
         if(recall_afb[i])
            {
             //event_report(REPORT_INFORMATION_TYPE,L"Повторный вызов по отказу",0,0);
             send_call(i,true);
            } 
        }
    }
 }


 void         __fastcall gr_cp::refresh (DWORD  fa)
 {
      if(fa == OTD_FA_DISCRETE || ((fa&OTD_FA_DATA_MASK) == OTD_FA_DATA_MASK) )
        send_call(GRKTAFB_CALLTS,true);
      if(fa == OTD_FA_ANALOG   || ((fa&OTD_FA_DATA_MASK) == OTD_FA_DATA_MASK))
        send_call(GRKTAFB_CALLTIT,true);
      if(fa == OTD_FA_ANALOG_INTEGR  || ((fa&OTD_FA_DATA_MASK) == OTD_FA_DATA_MASK))
        send_call(GRKTAFB_CALLTII,true);
 }


 void       __fastcall gr_cp::continue_floor_tx(__int64 sys_time)
 {
       BYTE buf[128];
       int  len;
       if(tx_time )
          {
           tx_deliver_time      = NS100_MSEC(sys_time-tx_time);
           DWORD middle_tx_time = tx_deliver_time/tx_current_len;
           tx_byte_time    = (tx_byte_time) ?(tx_byte_time+middle_tx_time)/2 : middle_tx_time;
          }
       if(tx_queue.QueueCount())
        {
          tx_queue.GetFromQueue(buf,sizeof(buf),&len);
          do_floor_tx(buf,len);
        }
        else
        tx_time = 0;

 }

 void       __fastcall gr_cp::send_call(DWORD call_afb,bool no_check)
 {
   DWORD     cnt = 0;
   BYTE buf[128];
   gr_proto gp;gp.dw_size = sizeof(gp);
   gr_group_list::iterator  gptr =grp_list.begin(),end = grp_list.end();
   __int64 c_time = GetTime(false);
   while(gptr<end)
   {
    if((*gptr)->get_call_afb() == call_afb && (*gptr)->call_sended(c_time))
       cnt++;
       gptr++;
   }

   if((cnt || no_check) && gr_proto_format (number,GRPROTO_MKTYPE(GRKT_CALL,call_afb),0,buf,sizeof(buf),&gp) == GRPP_NOERROR)
     {
      gr_proto_protect(&gp);
      floor_tx(buf,gp.kadr_len,false);
     }

 }


 void       __fastcall gr_cp::receipt(gr_proto & gp)
 {
  BYTE buf[128];
  DWORD len;
  /*if((flags & GRSC_FLAG_CP_NUMBERED_TS) && gp.info->inf == GRINF_TS)
  {
   //TODO Нумерованную квитанцию
   gr_proto_format(gp.cp->cp_number,
  }
  else*/
  len = gr_proto_receipt(gp.cp->cp_number,gp.type->afb,buf,sizeof(buf));
  floor_tx(buf,len,true);

 }

 DWORD      __fastcall gr_cp::handle(gr_proto & gp,DWORD proto_error,__int64 h_time)
 {
  DWORD ret = 0;
  if(!proto_error || proto_error==(DWORD)GRPP_ERR_INVPART)
  {
    set_diag(OTD_DIAG_NODATA|OTD_DIAG_PUCONNECT|OTD_DIAG_PURESPOND,false);
    switch(gp.type->ktype)
    {
    case  GRKT_DATAIN : //Обработка данных
          ret = handle_data(gp,proto_error,h_time);
          break;
    case  GRKT_READY  :
          handle_ready(gp,h_time);
          break;
    case  GRKT_ERROR  :
          handle_error(gp,h_time);
          break;
    }
  }
  return ret;
 }

 DWORD      __fastcall gr_cp::handle_data  (gr_proto & gp,DWORD proto_error,__int64 h_time)
 {
  //Обработка данных
  DWORD ret = 0;
  if(!proto_error)
  {
    switch(gp.info->inf)
    {
     case 0         :
                      ret|= set_diag(OTD_DIAG_CPCONNECT|OTD_DIAG_CPRESPOND,true) ? OTD_PROTO_PART_DIAG:0;
                      break;//Готовность
     case GRINF_TS  :
     case GRINF_TIT :
     case GRINF_TII : ret = handle_group(gp,h_time);break;
    }
   }
  if(ret)
    time_stamp = h_time;
  return ret;
 }

 DWORD      __fastcall gr_cp::handle_group    (gr_proto & gp,__int64 h_time)
 {
  DWORD ret = 0;
  set_diag(OTD_DIAG_NODATA|OTD_DIAG_PUCONNECT|OTD_DIAG_PURESPOND|OTD_DIAG_CPCONNECT|OTD_DIAG_CPRESPOND,false);
  rx_timer = rx_timer_value; // TODO
  rx_timer *=1000;

  DWORD grp_num = gp.info->grp;
 if(gp.info->inf == GRINF_TS &&( flags & GRSC_FLAG_CP_NUMBERED_TS))
         grp_num &=0x03;
  gr_group * grp = find_group(gp.info->inf,grp_num,gp.type->afb);
  if(grp)
   {
  if( gp.info->inf && !(gp.info->inf&GRINF_ERRFL) && !(flags&GRSC_FLAG_CP_NOTRECEIPT))
      receipt(gp);
     ret = grp->handle(gp,h_time);
     if(ret & OTD_PROTO_PART_DIAG)
        {
         scan_diag(); //Изменилось состояние диагностики
         modem_notify(MNCODE_SC_DATACHANGED,get_otd_addr(),OTD_PROTO_PART_DIAG,0,-1,0);
        }
     if((ret&OTD_PROTO_PART_DATA) && tu_list.size())
     {
      DWORD ch_idx ;
      BOOL  do_check =grp->get_first_change(&ch_idx);
      while(do_check && tu_list.size())
      {
       //TODO проверка ТУ находящихся в списке на успешность
       tutr_check_success(grp,ch_idx);
       do_check = grp->get_next_change(ch_idx,&ch_idx);
      }
     }
   }
   return ret;
 }




 DWORD      __fastcall gr_cp::handle_ready (gr_proto & gp,__int64 h_time)
 {
  DWORD ret = 0;
   if(gp.cp->line == 0)
      continue_floor_tx(h_time);
      else
      {
       /*Готовность блока ТУ*/
       DWORD mod_number = gr_proto_poscode2val(gp.data,gp.data_len);
       if(mod_number == (DWORD)gp.type->afb)
          {
           tutr_next(mod_number);
          } 
          
      }
  return ret;
 }

 DWORD      __fastcall gr_cp::handle_error (gr_proto & gp,__int64 /*h_time*/)
 {
  DWORD ret = 0;
   //TODO
   /*

   */
   /*Если ошибка  от блока ТУ*/
    if(this->flags&GRSC_FLAG_CP_LOGHARDWARE_ERROR)
     {
      //Ошибка Контроллера
        wchar_t rep_text[128];
        wchar_t rep_templ[128];
        get_owner()->get_owner()->get_lang_string(IDS_SC_LOG3,rep_templ,sizeof(rep_templ)/sizeof(wchar_t));
        wsprintfW(rep_text,rep_templ,gp.cp->cp_number);
        event_report(REPORT_ERROR_TYPE,rep_text,gp.cp,gp.kadr_len);
      }
  return ret;
 }


 void        __fastcall gr_cp::update_rxtimer_value()
 {
   rx_timer_value = 0;
   gr_group_list::iterator ptr = grp_list.begin(),end = grp_list.end();
   while(ptr<end)
   {
    gr_group * grp = *ptr;
    grp->get_respond_time();
    DWORD grp_tmv  = grp->get_work_timer_value();
    DWORD grp_rtmv = grp->get_respond_time_value()* (grp->get_norespond_counter_value()+1);
    if(!grp_rtmv)
        grp_rtmv = 15;

    rx_timer_value = KeRTL::MAX(rx_timer_value,grp_tmv+grp_rtmv);
    ptr++;
   }
   if(!rx_timer_value)
       rx_timer_value = 90; // 1,5 минута обнаружения неисправности КП
       else
       rx_timer_value+=15;

   rx_timer = KeRTL::MIN(rx_timer,rx_timer_value*1000);  
 }


 DWORD        __fastcall gr_cp::add_group (gr_group * grp,gr_group_list::iterator ptr )
 {
  if(grp )
  {
   sotd_addr a(grp->get_otd_addr());
   ptr = lower_group(a);
  ptr = grp_list.insert(ptr,grp);
  owner->modem_notify(MNCODE_SC_ADDED,grp->get_otd_addr(),-1);
  rx_timer_value = KeRTL::MAX(rx_timer_value,grp->get_work_timer_value()+10);
  return distance(grp_list.begin(),ptr);
  }
  return -1;
 }

void         __fastcall gr_cp::log_diag()
{
 wchar_t text[512];
 wchar_t rep_templ[512];
 LoadStringW(ModuleInstance,IDS_SC_LOG0,rep_templ,sizeof(rep_templ)/sizeof(wchar_t));
 swprintf(text,rep_templ,this->number,this->diag);
 event_report(REPORT_ERROR_TYPE,text,NULL,0);
}

 gr_cp::gr_group_list::iterator   __fastcall gr_cp::lower_group(otd_addr & addr)
 {

  return  std::lower_bound(grp_list.begin(),grp_list.end(),(gr_group *) NULL,*&gr_group_less(addr.addr));
 }


 gr_group* __fastcall gr_cp::find_group(otd_addr & addr)
 {
   gr_group * ret = NULL;
   sotd_addr sa(addr);
   sa.pu=0;
   sa.fa&=OTD_FA_DATA_MASK;
   gr_group_list::iterator ptr = lower_group(sa);
  if(ptr!=NULL && ptr<grp_list.end() && *ptr)
    {
     ret = (*ptr)->get_otd_addr() == sa.addr ? *ptr :ret;
    }
  if(!ret)
    SetLastError(GRSCERR_OBJECT_NOT_EXIST);


  return ret;
 }

 gr_group* __fastcall gr_cp::find_group(DWORD inf,DWORD num,DWORD mod,bool create)
 {
   sotd_addr a(get_otd_addr());
   a.pu  = 0;
   a.sb  = num;
   a.fa  = (inf == GRINF_TS ? OTD_FA_DISCRETE:(inf == GRINF_TIT ? OTD_FA_ANALOG:OTD_FA_ANALOG_INTEGR)) ;
   gr_group * ret = find_group(a);
   if(!ret && create)
     {
      ret = new gr_group(this,num,inf,mod);
      add_group(ret);
     } 
  if(ret && ret->get_mod_number()!=mod)
     {ret = NULL;SetLastError(GRSCERR_OBJECT_NOT_EXIST);}
  return ret;     
 }




 DWORD     __fastcall gr_cp::get_props(DWORD addr,LPVOID ptr)
 {
  DWORD ret = GKH_RET_ERROR;
  sotd_addr sa(addr);
  DWORD type = sa.addrtype();
  if(type == OTD_ADDR_TYPE_CP)
  {
    ret = get_props((LPGR_CP_PROPS)ptr);
  }
  else
  {
    LPGR_GRP_PROPS grp_props = (LPGR_GRP_PROPS)ptr;
    if(owner->get_owner()->check_data_size(ptr,sizeof(*grp_props)))
    {
     gr_group * grp = find_group(sa);
     if(grp) {grp->get_props(grp_props);ret = GKH_RET_SUCCESS;}
    }

  }
  return ret;
 }
 DWORD     __fastcall gr_cp::get_props(LPGR_CP_PROPS prp)
 {
     if(owner->get_owner()->check_data_size(prp,sizeof(*prp)))
     {
      prp->addr.addr = this->get_otd_addr();
      prp->flags     = this->flags;
      prp->grp_cnt   = this->grp_list.size();
      prp->send_wait_time = this->send_wait_time;
      lstrcpynW(prp->name,this->name,sizeof(prp->name)/sizeof(wchar_t));
      return GKH_RET_SUCCESS;
     }
    return GKH_RET_ERROR;
 }


 DWORD     __fastcall gr_cp::set_props(DWORD addr,LPVOID ptr)
 {
  DWORD ret = GKH_RET_ERROR;
  sotd_addr sa(addr);
  DWORD type = sa.addrtype();
  if(type == OTD_ADDR_TYPE_CP)
  {
     LPGR_CP_PROPS props = (LPGR_CP_PROPS)ptr;
     if(owner->get_owner()->check_data_size(ptr,sizeof(*props)))
     {
      ret = set_props(props);
     }
  }
  else
  {
    LPGR_GRP_PROPS grp_props = (LPGR_GRP_PROPS)ptr;
    if(owner->get_owner()->check_data_size(ptr,sizeof(*grp_props)))
    {
     sotd_addr sa(addr);
     if((DWORD)sa.cp == number)
     {
     if(grp_props->mask&GRSC_FPRP_ADDR)
     {
      if(!find_group(grp_props->addr))
        {
         remove(sa.addr);
         ret = create_group(grp_props);
        }
        else
        SetLastError(GRSCERR_OBJECT_ALREADY_EXIST),ret = GKH_RET_ERROR;
     }
     else
     {
     gr_group * grp = find_group(sa);
     if(grp)
        {
         ret = grp->set_props(grp_props);
         update_rxtimer_value();
        }
     }
     }

     if(grp_props->mask&GRSC_FPRP_TIMERS_TOALL)
       {  //Установка параметров диагностики для всех групп
          gr_group_list::iterator b = grp_list.begin(),e = grp_list.end();
          while(b<e)
          {
           (*b)->set_props(grp_props);
           ++b;
          }
        update_rxtimer_value();
       }

    }

  }
  return ret;
 }



 DWORD     __fastcall gr_cp::set_props(LPGR_CP_PROPS prp)
 {
  DWORD chm = 0;
  DWORD chfl = 0;
  if((prp->mask & GRSC_FPRP_FLAGS))
    {
      chfl = flags^prp->flags;
      flags = prp->flags;
      chm |= GRSC_FPRP_FLAGS;
    }

  if((prp->mask & GRSC_FPRP_NAME) && lstrcmpW( name,prp->name))
    {
     lstrcpynW(name,prp->name,sizeof(name)/sizeof(wchar_t)),chm |=GRSC_FPRP_NAME;
     BYTE      kadr[1024];
     DWORD len = get_otd_data(get_otd_addr(),OTD_PROTO_PART_NAME,kadr,sizeof(kadr),NULL);
     this->queue_rxdata(FA_OTD,kadr,len,false);
     chm|= GRSC_FPRP_NAME;
    }
   if((prp->mask&GRSC_FPRP_SWT) && send_wait_time!=prp->send_wait_time)
   {
    send_wait_time=prp->send_wait_time;
    if(!send_wait_time) send_wait_time = 1000;
    if(send_wait_time>2000)
       send_wait_time = 2000;
    chm|=GRSC_FPRP_SWT;
   }

  if(chfl&GRSC_FLAG_TEST)
    {
     reset_tu_error();
    }
  if(chm)
     {
      owner->config_change();
      owner->modem_notify(MNCODE_SC_CHANGED,get_otd_addr(),chm);
     }

  return GKH_RET_SUCCESS;
 }

 void         __fastcall gr_cp::reset_tu_error()
 {
   //Сброс неуспеха ТУ
   gr_group_list::iterator beg = grp_list.begin(),end = grp_list.end();
   while(beg<end)
   {
    (*beg)->reset_tu_error(0,-1);
    ++beg;
   }
   update_diag();
 }

 DWORD   __fastcall gr_cp::reset_tu_error(otd_addr & sa,DWORD obj)
 {
  DWORD ret = GKH_RET_ERROR;
  gr_group * grp = this->find_group(sa);
  if(grp)
     grp->reset_tu_error(obj,obj),ret = GKH_RET_SUCCESS;
  return ret;  
 }


 void         __fastcall gr_cp::set_number(DWORD num)
 {
  if(this->number!=num)
  {
    set_diag(OTD_DIAG_NODATA|OTD_DIAG_CPCONNECT|OTD_DIAG_CPRESPOND,true);
    number = num;
    gr_group_list::iterator beg = grp_list.begin(),end = grp_list.end();
    while(beg<end)
    {
     (*beg)->set_cp_number(num);
     beg++;
    }

    refresh(-1);
  }
 }


 DWORD     __fastcall gr_cp::enum_grp  (DWORD index,LPGR_GRP_PROPS grp)
 {
  if(index< grp_list.size())
     return grp_list[index]->get_props(grp);
   return GKH_RET_ERROR;
 }

    DWORD     __fastcall gr_cp::get_group_count(bool /*dyn*/)
    {
     DWORD ret = this->grp_list.size();
     return ret;
    }

 DWORD        __fastcall gr_cp::create_group(LPGR_GRP_PROPS prp)
 {
   DWORD ret = GKH_RET_ERROR;
   sotd_addr sa(prp->addr);
   gr_group * grp = this->find_group(sa);
   if(!grp)
   {
    grp = new gr_group(this,prp);
    ret = GKH_RET_SUCCESS;
    add_group(grp);
    update_diag();
    owner->config_change();
    if(get_owner()->is_worked())
      send_call(grp->get_call_afb());
   }
   else
   SetLastError(GRSCERR_OBJECT_ALREADY_EXIST);
   return ret;
 }

 DWORD        __fastcall gr_cp::get_dynamic (DWORD addr,LPVOID dyn_data)
 {
  DWORD ret = GKH_RET_ERROR;
  sotd_addr sa(addr);
  if(sa.sb<OTD_ADDR_MAXVALUE)
   {
      gr_group * grp = this->find_group(sa);
      if(grp)
         ret = grp->get_dynamic(dyn_data);
   }
   else
   {
    //TODO получение динамических данных КП
    LPSC_DYNAMIC_CP dyn_cp = (LPSC_DYNAMIC_CP) dyn_data;
    if(dyn_cp->dw_size == sizeof(*dyn_cp))
      {
       ret = GKH_RET_SUCCESS;
       dyn_cp->tx_deliver_time = tx_deliver_time;
       dyn_cp->tx_byte_time    = tx_byte_time;
       dyn_cp->rx_timer        = rx_timer;
      }
   }
  return ret;

 }

 DWORD        __fastcall gr_cp::get_otd_data(DWORD addr,LPSC_OTDDATA scod)
 {
  DWORD ret = GKH_RET_ERROR;
  sotd_addr sa(addr);
  if(sa.sb<OTD_ADDR_MAXVALUE)
   {
      gr_group * grp = this->find_group(sa);
      if(grp)
         ret = grp->get_otd_data(scod);
   }
   else
   {

    scod->data_len = get_otd_data(addr,scod->parts,scod->data,scod->data_size,NULL);
    if(((int)scod->data_len)>0)
       ret = GKH_RET_SUCCESS;
   }
  return ret;
 }

  DWORD     __fastcall gr_cp::remove(DWORD addr)
  {
    DWORD ret = GKH_RET_ERROR;
    sotd_addr sa(addr);
    BYTE  diag_pos [256];
    DWORD diag_len;
    gr_group * grp = this->find_group(sa);
    if(grp)
    {
      //Отправляем диагностический кадр  otd_diag.h
      grp->set_diag(-1,true);
      diag_len = grp->get_otd_data(OTD_PROTO_PART_DIAG,0,0,diag_pos,sizeof(diag_pos),NULL);
      queue_rxdata(FA_OTD,diag_pos,diag_len,false);
      owner->config_change();
      grp_list.erase(lower_group(sa));
      delete grp;
      update_rxtimer_value();
      update_diag();
      owner->modem_notify(MNCODE_SC_REMOVED,addr,-1);
      ret   = GKH_RET_SUCCESS;
      //TODO remove all tutr_entry
    }
    return ret;
  }

    void      __fastcall gr_cp::write(TStream & strm)
    {
      DWORD grp_cnt = this->get_group_count(true);
      DWORD name_len = lstrlenW(name);
      strm<<number<<flags<<name_len<<grp_cnt<<this->send_wait_time;
      strm.Write(name,name_len*sizeof(wchar_t));
      //TODO  Запись значений таймеров и таблиц ТУ/ТР
      gr_group_list::iterator beg = grp_list.begin(),end = grp_list.end();
      while(beg<end)
      {
       gr_group * grp = *beg++;
       strm<<grp->get_stream_ver();
       grp->write(strm);
      }
    }

    void      __fastcall gr_cp::read (TStream & strm,DWORD ver)
    {
      DWORD grp_cnt  ;
      DWORD name_len ;
      strm>>number>>flags>>name_len>>grp_cnt;
      if(ver>0)
        strm>>send_wait_time;
      strm.Read(name,name_len*sizeof(wchar_t));
      name[name_len] = 0;
      rx_timer_value = 0;
      while(grp_cnt--)
      {
        gr_group * grp = new gr_group(this);
        DWORD ver;
        strm>>ver;
        grp->read(strm,ver);
        this->add_group(grp);
      }
      diag = OTD_DIAG_NODATA|OTD_DIAG_PUCONNECT|OTD_DIAG_PURESPOND|OTD_DIAG_CPCONNECT|OTD_DIAG_CPRESPOND;
      update_rxtimer_value();
      rx_timer = rx_timer_value;
      rx_timer *=1000;
    }

DWORD        __fastcall gr_cp::get_otd_data (DWORD addr,DWORD parts,LPBYTE buff,DWORD bsz,lpotd_proto op)
{
 otd_proto_param opp;
 otd_proto       _op;
 _op.dw_size   = sizeof(_op);
 if(!op) op = & _op;
 op->proto_size = 0;

 ZeroMemory(&opp,sizeof(opp));
 opp.dw_size = sizeof(opp);
 parts|=OTD_PROTO_PART_KPK;
 opp.parts   = parts&~(OTD_PROTO_PART_DATA|OTD_PROTO_PART_PERSONAL_CHMASK);

 if(grp_list.size() &&( parts&OTD_PROTO_PART_PERSONAL_DIAG))
 {
    opp.pd_param.type  = OTD_ANALOG_QWORD;
    opp.pd_param.first = 0;
    opp.pd_param.count = grp_list.size();
 }
 if(parts & OTD_PROTO_PART_NAME)
  opp.name_sz = lstrlenW(name);

  if(otd_proto_format_ex(buff,bsz,&opp,0,op))
  {
   op->addr->addr = addr;
   if(op->diag) *op->diag = this->get_diag();
   if(op->name)
      otd_text_set(op->name,name);

   if(op->personal_diag)
   {
      DWORD addr_pd[2];
      gr_group_list::iterator beg =  grp_list.begin(),end = grp_list.end();
      DWORD idx = 0 ;
      while(beg<end)
      {
       addr_pd[0] = (*beg)->get_otd_addr();
       addr_pd[1] = (*beg)->get_diag();
       otd_set_value(op->personal_diag,idx,addr_pd,sizeof(addr_pd));
       beg++;
       idx++;
      }

   }
   if(op->time_stamp)
    {
      *((__int64*)op->time_stamp) = this->get_timestamp();
    }
   otd_proto_protect(op,op->proto_size,OTD_DEF_POLINOM);
  }
  return op->proto_size;
}

__int64      __fastcall gr_cp::get_timestamp()
{
      __int64 tms = 0;
      gr_group_list::iterator beg =  grp_list.begin(),end = grp_list.end();
      while(beg<end)
      {
       tms = KeRTL::MAX(tms,(*beg)->get_timestamp());
       beg++;
      }
    return tms;  
}


 DWORD        __fastcall gr_cp::modem_send        (LPMPROTO_HEADER mph,lpotd_proto op)
 {
  //Операция ТУ / ТР
  if(op && op->proto_size && OTD_FA_ISCTRL(op->addr->fa))
  {
   if(op->data && OTD_GET_COUNT(&op->data->numbers)==1)
      return tutr_send(mph,op);
   else
   {
    wchar_t log_text[MAX_PATH];
    wchar_t log_templ[MAX_PATH];
    LoadStringW(ModuleInstance,IDS_SC_LOG1,log_templ,sizeof(log_templ)/sizeof(wchar_t));
    wsprintfW(log_text,log_templ,(DWORD)mph->addr_from.sta,(DWORD)mph->addr_from.modem,(DWORD)mph->addr_from.line);
    event_report(REPORT_ERROR_TYPE,log_text,op->addr,op->proto_size);
   }
  }
  return GKH_RET_SUCCESS;
 }


 void         __fastcall gr_cp::queue_group_data   (gr_group * grp,DWORD modem_addr,bool /*_call*/)
 {
   if(grp)
   {
    DWORD parts = OTD_PROTO_PART_DIAG|OTD_PROTO_PART_KPK|OTD_PROTO_FLAG_QUERYRESPOND;
    DWORD grp_diag = grp->get_diag();
    if(!(grp_diag&OTD_DIAG_MASK) )
    {
      parts|=OTD_PROTO_PART_DATA;
      if(grp_diag&(OTD_DIAG_PART_MASK|OTD_DIAG_STATE_TUTR)) //otd_diag
         parts|=OTD_PROTO_PART_PERSONAL_DIAG;


    }
    grp->modem_send_changes(parts,0,-1,modem_addr);
   }
 }

 DWORD        __fastcall gr_cp::modem_handle_query(LPMPROTO_HEADER mph,lpotd_proto op)
 {
    //Обработка вызова запроса
    sotd_addr sa(op->addr->addr);
    sa.pu = -1;
    bool _call = (sa.fa&OTD_FA_CALL) ? true:false;
    gr_group_list::iterator ptr = this->grp_list.begin(),end =grp_list.end() ;
    //refresh(sa.fa);
    sa.fa&=OTD_FA_ALL;
    if((sa.fa&OTD_FA_DATA_MASK) ==  OTD_FA_DATA_MASK)
    {
     DWORD len;BYTE kadr[MAX_PATH];
     if((diag &(OTD_DIAG_PURESPOND|OTD_DIAG_PUCONNECT|OTD_DIAG_CPRESPOND|OTD_DIAG_CPCONNECT)))
     {
        //Неисправно ПУ или КП
         len = get_otd_data(get_otd_addr(),OTD_PROTO_PART_DIAG|((*op->ver_parts)&OTD_PROTO_PART_NAME),kadr,sizeof(kadr),NULL);
         queue_rxdata (FA_OTD,kadr,len);
     }
     else
     {
      if((*op->ver_parts)&OTD_PROTO_PART_NAME)
      {
         len = get_otd_data(get_otd_addr(),OTD_PROTO_PART_NAME,kadr,sizeof(kadr),NULL);
         queue_rxdata (FA_OTD,kadr,len);
      }
      while(ptr<end)
      {
       sotd_addr grp_addr((*ptr)->get_otd_addr());
       grp_addr.pu = -1;
       if(otd_addr_include(&grp_addr,&sa))
          queue_group_data(*ptr,mph->addr_from.addr,_call);
       ptr++;
      }
     } 
    }
    else
    {
     //Запрос на 1 фа
     ptr = lower_group(sa);
     while(*ptr && ptr<end  && otd_addr_include(&sotd_addr((*ptr)->get_otd_addr()),&sa))
     {
      queue_group_data(*ptr,mph->addr_from.addr);
      ptr++;
     }
    }
    return GKH_RET_SUCCESS;
 }

  LRESULT __fastcall gr_cp::get_tu_entry(LPGRSC_TU_ENTRY tue)
  {
   LRESULT ret = GKH_RET_ERROR;
   gr_group * grp = this->find_group(tue->addr);
   if(grp)
      ret = grp->get_tu_entry(tue);
      else
      SetLastError(GRSCERR_OBJECT_NOT_EXIST);
   return ret;
  }

  LRESULT __fastcall gr_cp::add_tu_entry(LPGRSC_TU_ENTRY tue)
  {
   LRESULT ret = GKH_RET_ERROR;
   gr_group * grp = this->find_group(tue->addr);
   if(grp)
      ret = grp->add_tu_entry(tue);
      else
      SetLastError(GRSCERR_OBJECT_NOT_EXIST);
   return ret;
  }

  LRESULT __fastcall gr_cp::set_tu_entry(LPGRSC_TU_ENTRY tue)
  {
   LRESULT ret = GKH_RET_ERROR;
   gr_group * grp = this->find_group(tue->addr);
   if(grp)
      ret = grp->set_tu_entry(tue);
      else
      SetLastError(GRSCERR_OBJECT_NOT_EXIST);
   return ret;
  }

  LRESULT __fastcall gr_cp::del_tu_entry(LPGRSC_TU_ENTRY tue)
  {
   LRESULT ret = GKH_RET_ERROR;
   gr_group * grp = this->find_group(tue->addr);
   if(grp)
      ret = grp->del_tu_entry(tue);
      else
      SetLastError(GRSCERR_OBJECT_NOT_EXIST);
   return ret;
  }

  LRESULT __fastcall gr_cp::enum_tu_entry(LPGRSC_TU_ENTRY tue,DWORD idx)
  {
   LRESULT ret = GKH_RET_ERROR;
   if(tue->addr.sb == OTD_ADDR_MAXVALUE)
   {
    if(idx< this->tu_list.size())
    {
     memcpy(tue,tu_list.at(idx),sizeof(*tue));
     ret = GKH_RET_SUCCESS;
    }
   }
   else
   {
   gr_group * grp = this->find_group(tue->addr);
   if(grp)
      ret = grp->enum_tu_entry(tue,idx);
      else
      SetLastError(GRSCERR_OBJECT_NOT_EXIST);
   }   
   return ret;
  }





  











