#pragma hdrstop
#include "granit.hpp"
#include <stdio.h>


sc_floor::sc_floor(DWORD number):modem_line(number),
term_event(true,false),floor_rx_event(false,false),floor_timer(false),send_event(false,false)
{
 this->floor_flags = 0;
 tx_speed    = rx_speed = 18000;
 ZeroMemory(cp_list,sizeof(cp_list));
}

void      __fastcall sc_floor::release()
{
 TLockHelper l(locker);
 for(int i = 0;i<GRSCFLOOR_MAX_CPCOUNT;i++)
  {
   if(cp_list[i])
	  delete cp_list[i];
  }
 modem_line::release();
}




DWORD     __fastcall sc_floor::OnException(DWORD ExceptionCode,LPEXCEPTION_POINTERS exp)
{
  
  if(locker.GetLockCount())
     locker.FullUnlock();
  DWORD ret = TGKThread::OnException(ExceptionCode,exp);
  Start(0);
  return ret;
}


void      __fastcall sc_floor::modem_notify(DWORD code,DWORD addr,DWORD proto_parts,DWORD loN,DWORD hiN,DWORD what )
{
       GRSC_ACR acr;
       acr.addr  = addr;
       acr.parts = proto_parts;
       acr.loN   = loN;
       acr.hiN   = hiN;
       acr.what  = what;
       modem_notify(MNF_SC,code,&acr,sizeof(acr));
}


void    __fastcall sc_floor::event_report(DWORD evt_type,wchar_t * rep_text,LPVOID data ,DWORD sz)
{

  TSCGranitModem * sc = dynamic_cast<TSCGranitModem *>(get_owner());
   if(sc)
      {
         //DWORD lock_count = locker.FullUnlock();
        sc->event_report((int)evt_type,rep_text,data,sz);
         //locker.RestoreLock(lock_count);

      } 
}

void    __fastcall sc_floor::tutr_report(DWORD evt_type,wchar_t * rep_text)
{

  TSCGranitModem * sc = dynamic_cast<TSCGranitModem *>(get_owner());
   if(sc)
      {
       //DWORD lock_count = locker.FullUnlock();
       sc->tutr_report((int)evt_type,rep_text);
       //locker.RestoreLock(lock_count);
      }
}


DWORD __fastcall  sc_floor::get_line_text     (wchar_t * text,DWORD text_sz)
{
  wchar_t floor_text[128];
  LoadStringW(owner->get_lang_instance(),IDS_SC_FLOOR_TEXT,floor_text,sizeof(floor_text)/sizeof(wchar_t));
  return snwprintf(text,text_sz,L"%s %d",floor_text,line_number);
}

bool  __fastcall  sc_floor::do_start ()
{
 return TGKThread::Start(16384);
}

bool  __fastcall  sc_floor::do_stop  ()
{
 if(TerminateAndWait(2000))
        connect(false);
  return !IsRunning();
}

bool  __fastcall  sc_floor::send     (LPMPROTO_HEADER mph,DWORD /*sz*/)
{
 if(mph->fa == FA_OTD && mph->data_size)
 {
         sotd_addr sa(*(lpotd_addr)mph->data);
         if(sa.cp == OTD_ADDR_MAXVALUE || (DWORD)GRPROTO_GET_FLOOR_NUM(sa.cp) == line_number)
         {
          send_queue.Lock();
          send_queue.put_data(mph,false);
          if(send_queue.queue_count() == 1)
             send_event.SetEvent(true);
          send_queue.Unlock();
         }
         return true;
 }
 return false;
}

DWORD     __fastcall sc_floor::enum_cp     (DWORD index,LPGR_CP_PROPS  cpp)
{
  DWORD ret = GKH_RET_ERROR;
  TLockHelper l(locker);
  int   idx = (int)index;
  for(DWORD i = 0;idx>=0 && i<GRSCFLOOR_MAX_CPCOUNT;i++)
  {
   gr_cp * cp = cp_list[i];
   if(cp && (--idx)<0)
	  ret = cp->get_props(cpp);
  }
  return  ret;
}


DWORD     __fastcall sc_floor::enum_grp    (DWORD index,LPGR_GRP_PROPS grp)
{
  DWORD ret = GKH_RET_ERROR;
  TLockHelper l(locker);
  gr_cp * cp = find_cp(grp->addr.cp,false);
  if(cp)
  {
   ret = cp->enum_grp(index,grp);
  }
  else
  SetLastError(GRSCERR_OBJECT_NOT_EXIST);
  return ret;
}


DWORD     __fastcall sc_floor::get_cp_count( bool /*dyn*/)
{
  DWORD count = 0;
  TLockHelper l(locker);
  for(int i = 1;i<GRSCFLOOR_MAX_CPCOUNT;i++)
  {
   if(this->cp_list[i])
     {
       count++;
     } 
  }
  return count;
}


DWORD     __fastcall sc_floor::get_props(DWORD addr, LPVOID ptr)
{
 DWORD ret = GKH_RET_ERROR;
 sotd_addr sa(addr);
 DWORD  lu_num = GRPROTO_GET_LU_NUM(sa.cp);
 TLockHelper l(locker);
 if(lu_num == 0)
 {
 if(owner->check_data_size(ptr,sizeof(GR_FLOOR_PROPS)))
  {
     LPGR_FLOOR_PROPS props = (LPGR_FLOOR_PROPS)ptr;
     if(props->mask & GRSC_FPRP_ADDR )
        {
         props->addr.addr = -1;
         props->addr.pu   =  0;
         props->addr.cp   =  GRPROTO_MK_CPNUM(line_number,0);
        }
     if(props->mask & GRSC_FPRP_FLAGS )
      props->flags    = this->floor_flags;
     if(props->mask & GRSC_FPRP_COUNTS )
      props->cp_count = get_cp_count();
   }
  }
  else
  {
   gr_cp * cp = find_cp(lu_num,false);
   if(cp)
     ret = cp->get_props(addr,ptr);
  }
 return  ret;
}

DWORD     __fastcall sc_floor::set_props(DWORD addr, LPVOID ptr)
{
 DWORD ret ;
 sotd_addr sa(addr);
 DWORD  lu_num = GRPROTO_GET_LU_NUM(sa.cp);

 if(lu_num == 0)
   ret = set_props((LPGR_FLOOR_PROPS)ptr);
  else
   ret = set_cp_props(addr,ptr);
 return  ret;
}



DWORD __fastcall sc_floor::set_cp_props(DWORD addr,LPVOID ptr)
{
    sotd_addr sa ( addr);
    DWORD ret = GKH_RET_ERROR;
   if(sa.addrtype() == OTD_ADDR_TYPE_SB)
    set_grp_props(addr,(LPGR_GRP_PROPS)ptr);
   else
   {
   DWORD  lu_num = GRPROTO_GET_LU_NUM(sa.cp);
   gr_cp * cp = find_cp(lu_num,false);
   if(cp)
      {
        ret = cp->set_props(addr,ptr);
        LPGR_CP_PROPS cpprp = (LPGR_CP_PROPS) ptr;
        if(ret == GKH_RET_SUCCESS && (cpprp->mask&GRSC_FPRP_ADDR) )
          {
            //Смена номера КП
            DWORD lu = GRPROTO_GET_LU_NUM(cpprp->addr.cp);
            if(cp_list[ lu ] == NULL)
            {

               modem_notify(MNCODE_SC_REMOVED,addr,-1,0,-1);
               cp_list[GRPROTO_GET_LU_NUM(sotd_addr(addr).cp)] = NULL;
               cp->set_number(cpprp->addr.cp);
               cp_list[lu] = cp;
               config_change();
               modem_notify(MNCODE_SC_ADDED,cpprp->addr.addr,-1);
            }
            else
            SetLastError(GRSCERR_OBJECT_ALREADY_EXIST),ret = GKH_RET_ERROR;

          }
     }
     }
   return ret;
}

DWORD __fastcall sc_floor::set_grp_props(DWORD addr,LPGR_GRP_PROPS grp_props)
{
  DWORD ret = GKH_RET_ERROR;
  if(grp_props->mask &GRSC_FPRP_TIMERS_TOALL)
   {
    gr_cp ** cp_b = cp_list;
    gr_cp ** cp_e = cp_list+GRSCFLOOR_MAX_CPCOUNT;
    while(cp_b<cp_e)
    {
     if(*cp_b)
       (*cp_b)->set_props(addr,grp_props);
      ++cp_b;
    }
  }
 else
 {
  DWORD  lu_num = GRPROTO_GET_LU_NUM(sotd_addr(addr).cp);
  gr_cp * cp = find_cp(lu_num,false);
  if(cp)
     ret = cp->set_props(addr,grp_props);
 }
 return ret;
}

DWORD     __fastcall sc_floor::set_props   (LPGR_FLOOR_PROPS  prp)
{
 DWORD ret = GKH_RET_ERROR;
 BOOL  ch  = FALSE;
 if(owner->check_data_size(prp,sizeof(GR_FLOOR_PROPS)))
  {

     ret = GKH_RET_SUCCESS;
     if((prp->mask&GRSC_FPRP_FLAGS) && floor_flags!=prp->flags)
     {
        floor_flags=prp->flags;
        ch = TRUE;
     }
    if(ch)
       config_change();

   }
 return ret;
}

void      __fastcall sc_floor::set_line_number(DWORD num)
{
 modem_line::set_line_number(num);
 TLockHelper l(locker);
 for(DWORD i = 1;i< GRSCFLOOR_MAX_CPCOUNT;i++)
 {
  gr_cp * cp = this->cp_list[i];
  if(cp)
	 cp->set_number(GRPROTO_MK_CPNUM(num,i));
 }
}





void    __fastcall  sc_floor::floor_rx(LPBYTE data,DWORD len)
{
  connect(true);
  TLockHelper l(floor_rx_queue.get_locker());
  floor_rx_queue.PutIntoQueue(data,len);
  if(floor_rx_queue.QueueCount() == 1)
    floor_rx_event.SetEvent(true);
}

bool    __fastcall  sc_floor::floor_tx(LPBYTE data,DWORD len)
{
 TSCGranitModem * sc = dynamic_cast<TSCGranitModem *>(get_owner());
 if(sc)
   {
    sc->sc_tx(data,len);
    update_stats_tx(len,1,0);
   }
  return  sc ? true:false;
}

DWORD     __fastcall sc_floor::refresh(DWORD /*sbl*/)
{
	  TLockHelper l(locker);
	  gr_cp ** first = cp_list,** last = first+GRSCFLOOR_MAX_CPCOUNT;
	  while(first<last)
	  {
		 if(*first)
			{
			 gr_cp * cp = (*first);
			 cp->refresh(-1);
			}
		   first++;
	  }

 return GKH_RET_SUCCESS;
}

DWORD     __fastcall sc_floor::floor_refresh(DWORD cpnum,DWORD fa)
{
 DWORD ret = GKH_RET_SUCCESS; 
 if(!GRPROTO_GET_LU_NUM(cpnum))
    cpnum = -1;
 if(cpnum>=OTD_ADDR_MAXVALUE)
   refresh(cpnum);
 else
 {
  TLockHelper l(locker);
  gr_cp * cp = this->find_cp(cpnum,false);
  if(cp)
   cp->refresh(fa);
 }
 return ret;
}

void      __fastcall sc_floor::on_connect()
{
 set_diag(OTD_DIAG_PUCONNECT,false);
 modem_line::on_connect();
 set_state(MODEM_LINE_STATE_TXREADY,true);
 refresh(-1);
}

void      __fastcall  sc_floor::on_disconnect()
{
 modem_line::on_disconnect();
 set_diag(OTD_DIAG_PUCONNECT|OTD_DIAG_PURESPOND,true);
}

void      __fastcall sc_floor::set_diag(DWORD df,bool set)
{
 TLockHelper l(locker);

	  gr_cp ** first = cp_list,** last = first+GRSCFLOOR_MAX_CPCOUNT;
	  while(first<last)
	  {
		 if(*first)
			{
			 gr_cp * cp = (*first);
			 cp->set_diag (df,set);
			}
		   first++;
	  }
}





void    __fastcall  sc_floor::Terminate()
{
 TGKThread::Terminate();
 term_event.SetEvent(true);
}

bool    __fastcall  sc_floor::BeforeExecute  ()
{
  TLockHelper l(floor_rx_queue.get_locker());
  floor_rx_queue.DropData();
  floor_rx_event.Wait(0); //Сброс события чтения
  floor_rx_queue.Unlock();
  term_event.SetEvent(false);
  synchro_set.Clear();
  synchro_set+=term_event;
  synchro_set+=floor_rx_event;
  synchro_set+=floor_timer();
  synchro_set+=send_event();
  floor_timer.StartPeriodTimer(GRSC_TIMER_RESOLUTION,true);
  return term_event.IsValid() ;

}



int     __fastcall  sc_floor::Execute()
{
   DWORD WaitResult = -1;
   while(!CheckTerminateRequest() && WaitResult)
   {
    WaitResult = synchro_set.Wait(INFINITE);
    switch(WaitResult)
    {
     case 1:  handle_floor_recv ()  ;break;
     case 2:  handle_floor_timer()  ;break;
     case 3:  handle_floor_send ()  ;break;
    }
   }
  return 0;
}

void    __fastcall  sc_floor::BeforeTerminate()
{
  TLockHelper l(floor_rx_queue.get_locker());
  floor_rx_queue.DropData();
  floor_rx_event.Wait(0); //Сброс события чтения
  floor_rx_queue.Unlock();
  term_event.SetEvent(false);
}

   void  __fastcall  sc_floor::handle_floor_timer()
   {
     /*Ежесекундный таймер этажа*/
     __int64 sys_time = KeRTL::GetTime(false);
     char text [MAX_PATH];

      TLockHelper l(locker);
      gr_cp ** first = cp_list,** last = first+GRSCFLOOR_MAX_CPCOUNT;
      while(first<last)
      {
         if(*first)
            (*first)->on_timer(sys_time);
           first++;
      }

   }

   void  __fastcall  sc_floor::handle_floor_recv ()
   {
     //Обработка принятых данных этажём
     BYTE buffer[128];
     int  len;
     gr_proto gp;
     gp.dw_size  = sizeof(gp);
     DWORD    proto_error;
     while(floor_rx_queue.GetFromQueue(buffer,sizeof(buffer),&len))
     {
       proto_error =  gr_proto_parser(buffer,len,&gp);
       if(!proto_error || proto_error ==(DWORD) GRPP_ERR_INVPART )
         {
            //КП ищется по номеру ЛУ в этаже
            DWORD cp_num = gp.cp->line;
             if(!cp_num)
                  cp_num =  gp.type->afb;
              TLockHelper l (locker);
              gr_cp * cp = this->find_cp(cp_num,false);
              if(cp)
                cp->handle(gp,proto_error,GetTime(false));
         }
         update_stats_rx(len,1,proto_error ? 1:0);
     }
   }

    gr_cp *   __fastcall  sc_floor::find_cp(DWORD number,bool create)
    {
      gr_cp * ret ;
      DWORD   lu  = GRPROTO_GET_LU_NUM(number);
      ret         = cp_list[lu];
      if(!ret && create && lu)
          {
           ret = new gr_cp(this,number);
           add_cp(ret);
          }
       if(!ret)
          SetLastError(GRSCERR_OBJECT_NOT_EXIST);
       return ret;
    }

    bool      __fastcall  sc_floor::add_cp(gr_cp * cp)
    {
      bool ret = false;
      if(cp)
      {
       DWORD   lu  = GRPROTO_GET_LU_NUM(cp->get_number());
       TLockHelper l(locker);
       if(NULL==cp_list[lu]  )
        {
         cp_list[lu] = cp;
         modem_notify(MNCODE_SC_ADDED,cp->get_otd_addr(),-1);
         ret = true;
        }
      }
      return ret;
    }

    void      __fastcall sc_floor::config_change()
    {
      TSCGranitModem * sc = dynamic_cast<TSCGranitModem *>(get_owner());
      if(sc)
       sc->config_change(this->get_number());
    }

    DWORD     __fastcall sc_floor::remove      (DWORD addr)
    {
      DWORD ret = GKH_RET_ERROR;
      sotd_addr sa(addr);
      DWORD lu  =GRPROTO_GET_LU_NUM(sa.cp);
	  TLockHelper l(locker);
	  gr_cp * cp = find_cp(lu,false);
	  if(cp)
	  {
	   if((sa.fa&OTD_FA_DATA_MASK)<OTD_FA_DATA_MASK)
		 ret = cp->remove(addr);
		 else
		 {
			BYTE kadr[256];
			cp_list[lu] = NULL;
			cp->set_diag(-1,true);
			otd_proto op;
			op.dw_size = sizeof(op);
			ret = cp->get_otd_data(addr,OTD_PROTO_PART_DIAG,kadr,sizeof(kadr),&op);
			this->queue_rxdata(FA_OTD,kadr,ret,false);
			delete cp;
			config_change();
			modem_notify(MNCODE_SC_REMOVED,addr,-1);
			ret = GKH_RET_SUCCESS;

		 }
	  }
	  return ret;
	}


    DWORD     __fastcall sc_floor::create_cp   (DWORD num,LPGR_CP_PROPS cpp)
    {
      //Создание КП
      DWORD ret = GKH_RET_ERROR;
      DWORD lu = GRPROTO_GET_LU_NUM(num);
      TLockHelper l(locker);
      if(NULL==cp_list[lu])
      {
        gr_cp * cp  = new gr_cp(this,num);
        cp->set_props(cpp);
        add_cp(cp);
        config_change();
        if(owner->is_running())
          cp->refresh(-1);
        ret = GKH_RET_SUCCESS;
      }
      else
      SetLastError(GRSCERR_OBJECT_ALREADY_EXIST);

      return ret;
    }

    DWORD     __fastcall sc_floor::create_group(DWORD cp,LPGR_GRP_PROPS grp)
    {
      DWORD ret = GKH_RET_ERROR;
      DWORD lu = GRPROTO_GET_LU_NUM(cp);
	  TLockHelper l(locker);
	  if(cp_list[lu])
	  {
		ret = cp_list[lu]->create_group(grp);
	  }
	  else
	  SetLastError(GRSCERR_OBJECT_NOT_EXIST);
	  return ret;
	}


	void      __fastcall sc_floor::write(TStream & strm)
	{
	  TLockHelper l(locker);
	  DWORD cp_count = this->get_cp_count(true);
	  strm<<line_number<<floor_flags<<cp_count;
	  for(DWORD i = 1;i<GRSCFLOOR_MAX_CPCOUNT  &&  cp_count;i++)
	  {
	   gr_cp * cp = cp_list[i];
	   if(cp )
		  {
		   strm<<cp->get_stream_ver();
		   cp->write(strm);
		   cp_count--;
		  }
	  }

	}

    void      __fastcall sc_floor::read (TStream & strm,DWORD /*ver*/)
    {
      DWORD  cp_count;
      TLockHelper l(locker);
      strm>>line_number>>floor_flags>>cp_count;
      for(DWORD i = 0;i<cp_count;i++)
      {
       gr_cp * cp = new gr_cp(this,i);
       DWORD ver;
       strm>>ver;
       cp->read(strm,ver);
       add_cp(cp);
      }

    }

 DWORD        __fastcall sc_floor::get_dynamic (DWORD addr,LPVOID dyn_data)
 {
   DWORD ret = GKH_RET_ERROR;
   sotd_addr sa(addr);
   DWORD      lu  = GRPROTO_GET_LU_NUM(sa.cp);
   TLockHelper l(locker);
   if(lu)
   {
   gr_cp * cp     = find_cp(lu,false);
   if(cp)
	  ret = cp->get_dynamic(addr,dyn_data);
   }
   else
   {
	//Получение динамических данных  этажа
   }

  return ret;
 }

 DWORD        __fastcall sc_floor::get_otd_data(DWORD addr,LPSC_OTDDATA scod)
 {
  DWORD ret = GKH_RET_SUCCESS;
  DWORD      lu  = GRPROTO_GET_LU_NUM(sotd_addr(addr).cp);
  TLockHelper l(locker);
   if(lu)
   {
   gr_cp * cp     = find_cp(lu,false);
   if(cp)
	  ret = cp->get_otd_data(addr,scod);
   }
   else
   {
	//Получение данных Готовностей по этажу
	 scod->data_len = get_otd_data(scod->data,scod->data_size,scod->parts,scod->lo_num,scod->hi_num);
	 ret = scod ->data_len <=scod->data_size ? GKH_RET_SUCCESS:GKH_RET_ERROR;
   }

  return ret;
 }

 DWORD __fastcall sc_floor::get_otd_data(LPBYTE buffer,DWORD bsz,DWORD parts,DWORD lo,DWORD hi)
 {
  otd_proto_param opp;
  otd_proto       op;
  op.dw_size     = sizeof(op);
  ZeroMemory (&opp,sizeof(opp));
  opp.dw_size = sizeof(opp);
  opp.parts   = OTD_PROTO_PART_KPK|(parts&(OTD_PROTO_PART_DIAG|OTD_PROTO_PART_PERSONAL_DIAG));
  opp.pd_param.type  = OTD_ANALOG_DWORD;
  opp.pd_param.first = lo;
  opp.pd_param.count = hi-lo+1;
  DWORD len = otd_proto_format_ex(buffer,bsz,&opp,0,&op);
  if(len)
  {
   DWORD diag = this->is_connected() ? 0:(OTD_DIAG_PUCONNECT|OTD_DIAG_PURESPOND);
   TLockHelper l(locker);
   for(int i = 1;i<GRSCFLOOR_MAX_CPCOUNT;i++)
   {
    gr_cp* cp = cp_list[i];
    if(cp)
    {
     DWORD cp_diag = cp->get_diag();
     if(cp_diag & OTD_DIAG_FULL_MASK)
     diag|=OTD_PART_DIAG_PARAM;
     if(op.personal_diag)
       otd_set_value(op.personal_diag,i,&cp_diag,sizeof(cp_diag));
    }
   }

   if(!is_connected())
     diag|=OTD_DIAG_PUCONNECT;
   if(op.diag)
      *op.diag = diag;

  }
  return len;
 }

  LRESULT __fastcall sc_floor::get_tu_entry(LPGRSC_TU_ENTRY tue)
  {
   LRESULT ret = GKH_RET_ERROR;
   DWORD      lu  = GRPROTO_GET_LU_NUM(tue->addr.cp);
   if(lu)
   {
   TLockHelper l(locker);
   gr_cp * cp     = find_cp(lu,false);
   if(cp)
	  ret = cp->get_tu_entry(tue);
	  else
	  SetLastError(GRSCERR_OBJECT_NOT_EXIST);
   }
   return ret;
  }

  LRESULT __fastcall sc_floor::add_tu_entry(LPGRSC_TU_ENTRY tue)
  {
   LRESULT ret = GKH_RET_ERROR;
   DWORD      lu  = GRPROTO_GET_LU_NUM(tue->addr.cp);
   if(lu)
   {
   TLockHelper l(locker);
   gr_cp * cp     = find_cp(lu,false);
   if(cp)
	  {
	   ret = cp->add_tu_entry(tue);
	   if(ret != GKH_RET_ERROR)
		  config_change();
	  }
	  else
	  SetLastError(GRSCERR_OBJECT_NOT_EXIST);
   }
   return ret;
  }

  LRESULT __fastcall sc_floor::set_tu_entry(LPGRSC_TU_ENTRY tue)
  {
   LRESULT ret = GKH_RET_ERROR;
   DWORD      lu  = GRPROTO_GET_LU_NUM(tue->addr.cp);
   if(lu)
   {
   TLockHelper l(locker);
   gr_cp * cp     = find_cp(lu,false);
   if(cp)
      {
       ret = cp->set_tu_entry(tue);
       if(ret != GKH_RET_ERROR)
          config_change();
      } 
      else
      SetLastError(GRSCERR_OBJECT_NOT_EXIST);
   }

   return ret;
  }

  LRESULT __fastcall sc_floor::del_tu_entry(LPGRSC_TU_ENTRY tue)
  {
   LRESULT ret = GKH_RET_ERROR;
   DWORD      lu  = GRPROTO_GET_LU_NUM(tue->addr.cp);
   if(lu)
   {
   TLockHelper l(locker);
   gr_cp * cp     = find_cp(lu,false);
   if(cp)
	  {
	   ret = cp->del_tu_entry(tue);
	   if(ret != GKH_RET_ERROR)
		  config_change();
	  }
	  else
	  SetLastError(GRSCERR_OBJECT_NOT_EXIST);
   }

   return ret;
  }

  LRESULT __fastcall sc_floor::enum_tu_entry(LPGRSC_TU_ENTRY tue,DWORD idx)
  {
   LRESULT ret = GKH_RET_ERROR;
   DWORD      lu  = GRPROTO_GET_LU_NUM(tue->addr.cp);
   if(lu)
   {
   TLockHelper l(locker);
   gr_cp * cp     = find_cp(lu,false);
   if(cp)
	  ret = cp->enum_tu_entry(tue,idx);
	  else
	  SetLastError(GRSCERR_OBJECT_NOT_EXIST);
   }

   return ret;
  }


 DWORD      __fastcall sc_floor::test_tutr   (LPSC_TEST_TUTR tt)
 {
 
  DWORD ret = GKH_RET_SUCCESS;
  DWORD lu  = GRPROTO_GET_LU_NUM(tt->addr.cp);
  TLockHelper l(locker);
   if(lu)
   {
   gr_cp * cp     = find_cp(lu,false);
   if(cp)
	  ret = cp->test_tutr(tt);
   }
  return ret;
 }

 DWORD    __fastcall sc_floor::reset_tu_error(otd_addr & addr,DWORD obj)
 {
  DWORD ret = GKH_RET_SUCCESS;
  DWORD lu  = GRPROTO_GET_LU_NUM(addr.cp);
  TLockHelper l(locker);
   if(lu)
   {
   gr_cp * cp     = find_cp(lu,false);
   if(cp)
	  ret = cp->reset_tu_error(addr,obj);
   }
  return ret;
 }

 void      __fastcall  sc_floor::handle_floor_send ()
 {
   BYTE buff[4096];
   LPMPROTO_HEADER mph ;
   int             rsz;
   otd_proto op;
   op.dw_size = sizeof(op);
   gr_cp * cp ;
   while((mph =send_queue.get_data(buff,sizeof(buff),&rsz))!=NULL)
   {
     if(rsz && mph->data_size && mph->fa == FA_OTD && otd_proto_parser((LPBYTE)mph->data,mph->data_size,&op,(LPDWORD)&rsz))
     {
       update_modemmer_tx(rsz,1);
       DWORD floor_num = GRPROTO_GET_FLOOR_NUM(op.addr->cp);
       DWORD lu_num    = GRPROTO_GET_LU_NUM   (op.addr->cp);
       if(!OTD_ADDR_ISQUERY(op.addr))
       {

		 if(floor_num == this->get_number())
		   {
            TLockHelper l(locker);
			cp = this->cp_list[lu_num];
			if(cp)
			   cp->modem_send(mph,&op);
		   }
	   }
	   else
	   {
		//Обработка запроса

        TLockHelper l(locker);
		if(op.addr->cp == OTD_ADDR_MAXVALUE)
		  {
			gr_cp ** cp_beg = cp_list,**cp_end = cp_beg+GRSCFLOOR_MAX_CPCOUNT;
			while(++cp_beg<cp_end) //С Первого КП
			{
			 cp = *cp_beg;
			 if(cp)
			    cp->modem_handle_query(mph,&op);
			}
		  }
		  else
		  {
			cp = this->cp_list[lu_num];
			if(cp)
			   cp->modem_handle_query(mph,&op);

		  }
	   }
	 }
   }
 }


 
