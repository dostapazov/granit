#pragma hdrstop
#include "iec60870_modem.hpp"
#include <otd_diag.h>

   void __fastcall Tiec60870line::do_report(DWORD evt_type,wchar_t * rep_text,LPVOID data,DWORD dt_sz)
   {
      Tiec60870modem * modem = dynamic_cast<Tiec60870modem *>(get_owner());
      if(modem)
         modem->do_report(evt_type,rep_text,data,dt_sz);
   }




   int __fastcall Tiec60870line::read (KeRTL::TStream & strm,DWORD stream_ver)
   {
    int ret = strm.GetPosition();
    if(stream_ver < IEC60870_MOD_STREAM_VER_1)
    strm.Read(&line_config,sizeof(line_config)-sizeof(line_config.reserv));
    else
    strm.Read(&line_config,sizeof(line_config));

    set_line_number(line_config.cp_number);
    read_records(strm,ts_recs ,stream_ver);
    read_records(strm,tit_recs,stream_ver);
    read_records(strm,tii_recs,stream_ver);
    return strm.GetPosition()-ret;
   }

   int __fastcall Tiec60870line::read_records (KeRTL::TStream & strm,iec60870_records_t & recs,DWORD stream_ver)
   {
     DWORD recs_count = 0;
     iec60870_record  rec;
     strm>>recs_count;
     TLockHelper l(storage_locker);
     recs.clear() ;

     while(recs_count)
     {
	  strm>>rec.number>>rec.otd_fa>>rec.otd_group>>rec.otd_number>>rec.rc_command>>rec.rc_number>>rec.is_float;
	  if(stream_ver>IEC60870_MOD_STREAM_VER_2)
		 {
		  int qu;
		  strm>>qu>>rec.rc_timer;
                  rec.rc_ctrl.qu =(iec60870_command_quality) qu;
		 }
      recs.insert(recs.end(),rec);
      --recs_count;
     }
     otd_create_groups(recs);
     return 0;
   }


   int __fastcall Tiec60870line::write(KeRTL::TStream & strm)
   {
     int ret = strm.GetPosition();
     TLockHelper l(locker);
     strm.Write(&line_config,sizeof(line_config));
     write_records(strm,ts_recs );
     write_records(strm,tit_recs );
     write_records(strm,tii_recs );
     return strm.GetPosition() - ret;
   }

   int __fastcall Tiec60870line::write_records(KeRTL::TStream & strm, iec60870_records_t & recs)
   {
    DWORD save_position = strm.GetPosition();
    DWORD count = 0;
    strm<<count;
    TLockHelper l(storage_locker);
    iec60870_records_t::iterator ibeg = recs.begin();
    iec60870_records_t::iterator iend = recs.end  ();
    while(ibeg < iend)
    {
     if(ibeg->is_otd_assigned())
     {
	   strm<<ibeg->number<<ibeg->otd_fa<<ibeg->otd_group<<ibeg->otd_number<<ibeg->rc_command<<ibeg->rc_number<<ibeg->is_float;
       strm<<(int)ibeg->rc_ctrl.qu<<ibeg->rc_timer;
       ++count;
     }

     ++ibeg;
    }
    strm.Seek(save_position,TStream::smFromBegin);
    strm<<count;
    strm.Seek(0,TStream::smFromEnd);
    return 0;
   }


   bool __fastcall Tiec60870line::is_need_restart(const IEC60870_LINE_CONFIG & ln_conf)
   {
     TLockHelper l(locker);
     bool need_restart = (is_worked() &&
                          (ln_conf.cp_number != line_config.cp_number ||
                           ln_conf.host_port != line_config.host_port ||
                           _wcsicmp(ln_conf.host_addr,line_config.host_addr) ||
                           ln_conf.obj_addr_size != line_config.obj_addr_size
                          )) ? true:false;

    return need_restart ;
   }

   bool __fastcall Tiec60870line::set_config(const IEC60870_LINE_CONFIG & ln_conf)
   {

    bool need_restart =  is_need_restart(ln_conf);
    if(need_restart) stop();
    {
        if(get_number()!=ln_conf.cp_number)
        {
         if(this->owner )
         {
           if(!owner->renumber_line(get_number(),ln_conf.cp_number)) return false;
         }
         else
         set_line_number(line_config.cp_number);
        }
     TLockHelper l(locker);
     line_config = ln_conf;
    }

    if(need_restart) start();
    return true;
   }

   bool __fastcall Tiec60870line::get_config(IEC60870_LINE_CONFIG & ln_conf)
   {
      TLockHelper l(locker);
      ln_conf = this->line_config;
      return true;
   }


   DWORD __fastcall Tiec60870line::get_tx_queue_size ()
   {
     // TODO return tx_queue.size();
     return 0;
   }

   DWORD __fastcall Tiec60870line::get_line_text  (wchar_t * text,DWORD text_sz)
   {
    DWORD ret = 0;
    ret = snwprintf(text,text_sz,L"КП %d:%s",this->get_number() /*line_config.cp_number*/,line_config.cp_name);
    return ret;
   }


   void  __fastcall Tiec60870line::Terminate         ()
   {
      TGKThread::Terminate();
      if(term_event) term_event->SetEvent(true);
   }


   bool  __fastcall Tiec60870line::do_start ()
   {
      this->obj_addr_size = line_config.obj_addr_size;
      modem_line::do_start();
      return TGKThread::Start(8192);
   }

   bool  __fastcall Tiec60870line::do_stop  ()
   {
     modem_line::do_stop();
     if(GetID() == GetCurrentThreadId())
     {
         Terminate();
         return true;
     }
     else
      return TerminateAndWait(1000,true);

   }

   bool  __fastcall Tiec60870line::BeforeExecute     ()
   {

      sset.Clear();
      if(TGKThread::BeforeExecute())
      {
         term_event = new KeRTL::TEvent(true,false);
         if(term_event)
         {
          sset+=(*term_event)();
          wtimer = new TWaitableTimer(false);
          if(wtimer)
          {
           sset+=(*wtimer)();
           return true;
          }
         }
      }
    return false;
   }

   void  __fastcall Tiec60870line::BeforeTerminate   ()
   {
     if(term_event)
        delete term_event;
     term_event = NULL;
     if(wtimer)
        delete wtimer;
     wtimer = NULL;
     sset.Clear();
     rxbuf.clear();

   }

   DWORD __fastcall Tiec60870line::get_wait_ticks    (DWORD ms )
   {
      DWORD tc      =  GetTickCount();
      DWORD elapsed =  tc-last_timer_ticks;

      if(elapsed<ms )
      {
        DWORD remain = ms-elapsed;
        if(remain>10)
           return remain;
      }
      last_timer_ticks = GetTickCount();
      return ms;
   }



   int __fastcall Tiec60870line::Execute()
   {
     SetFlags(KERTL_THREAD_ENABLERESTART,true);
     int    ret = socket_execute();
     return ret;
   }

    void __fastcall Tiec60870line::handle_recv()
     {
       while(rxbuf.is_frame_complete())
       {
         lpiec60870_proto_header_t phdr = rxbuf.get_current_frame();
         owner->notify(MNF_LINE_RX,get_number(),(LPBYTE)phdr,phdr->get_frame_lenght());
         handle_frame(phdr);
         rxbuf.next_frame(true);

       }
       rxbuf.move_to_begin();
       WTimer_start(WT_RX_TIMER);
       WTimer_start(WT_TESTFR_TIMER);
     }


   DWORD __fastcall Tiec60870line::get_dev_addr()
   {
     TLockHelper l(locker);
     return  line_config.dev_addr;
   }

   DWORD __fastcall Tiec60870line::get_common_addr()
   {
     TLockHelper l(locker);
     return  line_config.common_addr;
   }

   DWORD __fastcall Tiec60870line::get_W_count()
   {
     TLockHelper l(locker);
     return  line_config.W_count;
   }

   DWORD __fastcall Tiec60870line::get_K_count()
   {
     TLockHelper l(locker);
     return  line_config.K_count;
   }

   void  __fastcall Tiec60870line::WTimer_start(int idx)
   {
     if(idx<MAX_WT_TIMERS)
     {
       TLockHelper l(locker);
       WT[idx] = line_config.WT[idx];
     }
   }

   void  __fastcall Tiec60870line::WTimer_stop(int idx)
   {
     if(idx<MAX_WT_TIMERS)
     {
       TLockHelper l(locker);
       WT[idx] = 0;
     }
   }

   bool  __fastcall Tiec60870line::WTimer_check(int idx)
   {
     if(idx<MAX_WT_TIMERS)
     {
       TLockHelper l(locker);
       int & wt = *(WT+idx);
       if(wt)
         {
           wt-=WT_RESOLUTION;
           if(wt<=0)
             {
              wt = 0;return true;
             }
         }
     }
    return false;
   }


    void __fastcall Tiec60870line::handle_frame(lpiec60870_proto_header_t phdr)
    {
      DWORD kadrs_count = 1;
      switch(phdr->get_format())
      {
        case acpi_type_I:
        handle_frame_typeI(phdr);
        break;
        case acpi_type_S:
        handle_frame_typeS(phdr);
        break;
        case acpi_type_U:
        handle_frame_typeU(phdr);
        break;
        default: kadrs_count = 0;
      }
      if(kadrs_count)
         this->update_stats_rx(0,kadrs_count,0);
    }

    void __fastcall Tiec60870line::__check_tx_number(DWORD number)
    {
       if(K_counter)
       {
           DWORD diff =  number - tx_number.get_number();
           K_counter-=diff;
           tx_number += diff;
           txbuf.erase_frames(diff);
       }
    }

    void __fastcall Tiec60870line::__check_rx_number(DWORD recv_number)
    {
       WORD num_diff = recv_number - rx_number.get_number() ;
       if(num_diff>1 && (0!=recv_number || 0x7FFF != rx_number.get_number()))
         {
          //Ошибка - нарушение последовательности
          wchar_t rep_text[256];

          snwprintf(rep_text,KERTL_ARRAY_COUNT(rep_text),L"%u.%u: Пропуск принятых кадров.Номер последнего нормального %u, принятого %u ",owner->get_modem_number(),get_number(), rx_number.get_number(),recv_number);
          do_report(REPORT_ERROR_TYPE,rep_text,NULL,0);
         }
        rx_number.set_number(recv_number);
        W_counter+=num_diff;
        if(DWORD(W_counter) >= get_W_count() )
           __do_receipt_rx();
    }


    void __fastcall Tiec60870line::handle_frame_typeS(lpiec60870_proto_header_t phdr)
    {
     __check_tx_number(phdr->acpiS.rx_number.get_number());
    }

    void __fastcall Tiec60870line::handle_frame_typeU(lpiec60870_proto_header_t phdr)
    {
      if(phdr->acpiU.startdt)
      {
        if(phdr->acpiU.startdt == at_con)
           {
            connect(true);
           }
           else
           {
            //Обобщающая процедура разрыва связи
           }
      }

      if(phdr->acpiU.stopdt)
      {
        if(phdr->acpiU.stopdt == at_con)
         {
          connect(false);
         }
      }

      if(phdr->acpiU.testfr)
      {
        //Проверка связи
        if(phdr->acpiU.testfr == at_act)
           {
            phdr->dw_ctrl      = acpi_type_U;
            phdr->acpiU.testfr = at_con;
            do_socket_send((LPBYTE)phdr,phdr->get_frame_lenght());
           }
      }
    }

   void __fastcall Tiec60870line::on_waitable_timer ()
   {
     int i ;
     if(WTimer_check(WT_TX_TIMER))
       {
         __send_next_frame();
       }

     if(WTimer_check(WT_RX_TIMER))
       {
         //Сквитировать
         __do_receipt_rx();
       }

     if(WTimer_check(WT_TESTFR_TIMER))
       {
         BYTE buffer[32];
         lpiec60870_proto_header_t phdr = iec60870_proto_testfr(buffer,sizeof(buffer),false);
         do_socket_send((LPBYTE)phdr,iec60870_proto_get_frame_length(phdr));
       }

     if(WTimer_check(3))
       {
       }
       tutr_timer();
       handle_record_changes();
   }

   inline bool  __fastcall rec_rc_timer_expired(const iec60870_record & rec)
   {
     return rec.rc_timer > 0 ? false : true;
   }

   void  __fastcall Tiec60870line::tutr_timer        ()
   {
     iec60870_records_t::iterator tutr_ptr = tutr_active.begin();
     iec60870_records_t::iterator tutr_end = tutr_active.end  ();
     int remove_count = 0;
     while(tutr_ptr < tutr_end)
     {
      tutr_ptr->rc_timer-=WT_RESOLUTION;
      if(tutr_ptr->rc_timer<=0)
        {
          ++remove_count;
          TLockHelper l(storage_locker);
          iec60870_records_t * recs = get_records_for(tutr_ptr->otd_fa);
          if(recs)
          {
            iec60870_records_t::iterator rec_ptr;
            if(recs->find(*tutr_ptr,rec_ptr))
            {
             rec_ptr->changes_mask = IEC60870_REC_FL_RC_STATE;
             rec_ptr->rc_state     = OTD_PDIAG_TUTR_FAIL;
             __record_changed(*rec_ptr);
            }
          }
        }
      ++tutr_ptr;
     }

     if(remove_count)
     {
        DWORD sz = tutr_active.size();
        tutr_active.erase(std::remove_if(tutr_active.begin(),tutr_active.end(),rec_rc_timer_expired));
        sz = tutr_active.size();
        sz = tutr_active.size();
     }
   }


   void  __fastcall Tiec60870line::on_connect        ()
   {
     timer_count    = 0;
     timer_counter  = 0;
     rxbuf.clear();
     txbuf.clear();
     bzero(WT,sizeof(WT));
     W_counter = 0;
     K_counter = 0;
     wtimer->StartPeriodTimer(WT_RESOLUTION,true);


     modem_line::on_connect();
     if(!line_config.WT[WT_TESTFR_TIMER])
         line_config.WT[WT_TESTFR_TIMER] = 10000;
     WTimer_start    (WT_TESTFR_TIMER);
   }

   void do_set_bad_diag(iec60870_record & rec)
   {
     rec.timestamp   = 0;
     rec.quality.quality_byte = 0;
     rec.quality.iv  = 1;
     rec.rc_state    = 0;

   }

   void  __fastcall Tiec60870line::__set_bad_diag      ()
   {
      TLockHelper l(storage_locker);
      std::for_each(ts_recs.begin () ,ts_recs.end (),do_set_bad_diag);
      std::for_each(tit_recs.begin() ,tit_recs.end(),do_set_bad_diag);
      std::for_each(tii_recs.begin() ,tii_recs.end(),do_set_bad_diag);
      proto_pointer pptr = storage.begin();
      proto_pointer pend = storage.end  ();
      while(pptr<pend)
      {
       *pptr->op.diag   = OTD_DIAG_CPMASK;
       DWORD diag_value = OTD_PDIAG_NODATA;
       otd_fill(pptr->op.personal_diag,&diag_value,sizeof(diag_value));
       ++pptr;
      }

   }


   void  __fastcall Tiec60870line::on_disconnect     ()
   {
      wtimer->StopTimer();
      tutr_active.clear();
      modem_line::on_disconnect();
      __set_bad_diag ();
   }

   void  __fastcall Tiec60870line::__handle_otd_query   (otd_proto * op)
   {
      if(((DWORD)op->addr->cp == get_number()) || ((DWORD)op->addr->cp == OTD_ADDR_MAXVALUE))
      {
          this->__do_recv_cp_data(true);
          TLockHelper l(storage_locker);
          otd_addr addr;
          addr.addr = op->addr->addr;
          addr.cp = addr.pu = 0;
          proto_pointer pptr = storage.begin();
          proto_pointer pend = storage.end  ();
          if(storage.get_range(addr,pptr,pend))
          {
              while(pptr<pend)
              {
                __queue_rxdata(&pptr->op);
                ++pptr;
              }
          }
      }
   }

  bool  __fastcall Tiec60870line::send        (LPMPROTO_HEADER mph,DWORD sz)
  {
	if(mph->fa == FA_OTD)
	{
	      sotd_proto sop;
	      sop.assign((LPBYTE)mph->data,mph->data_size);
          if(OTD_ADDR_ISQUERY(sop.op.addr))
           __handle_otd_query(&sop.op);
          else
          {
           if( get_number() == DWORD(sop.op.addr->cp) && OTD_FA_ISCTRL(sop.op.addr->fa) )
               __handle_otd_control(&sop.op);
          }
        }
     return true;
  }


void __fastcall get_tutr(otd_data * data,DWORD & object,otd_tutr & tutr)
{
    object = data->numbers.loN;
    tutr.command = 0;
    tutr.param   = 0;

    if(data->otd_type == OTD_TR_COMMAND_SET)
      otd_get_value(data,object,&tutr,sizeof(tutr));
      else
      otd_get_value(data,object,&tutr,sizeof(tutr));
}

  void  __fastcall 	iec60870_command_from_otd(const iec60870_record & rec, const otd_tutr &tutr)
  {
   switch(rec.rc_command)
   {
	case C_SC_NA_1:
	rec.rc_ctrl.var = tutr.command == OTD_TUOP_ON ? sp_on:sp_off;
	break;
	case C_DC_NA_1:
	rec.rc_ctrl.var = tutr.command == OTD_TUOP_ON ? dp_on:dp_off;
	break;
	case C_RC_NA_1:
        rec.rc_ctrl.var = tutr.command == OTD_TUOP_ON ? rcs_increment:rcs_decrement;
	break;
   }
  }

  void  __fastcall Tiec60870line::__handle_otd_control(otd_proto * op)
  {
	if(get_number() == DWORD(op->addr->cp) && OTD_FA_ISCTRL(op->addr->fa))
	  {
	   DWORD otd_fa = (op->addr->fa&OTD_FA_DATA_MASK);
	   iec60870_records_t * recs =  get_records_for(otd_fa);
	   if(recs && op->data)
		 {
		   DWORD object = -1;
		   otd_tutr tutr;
		   get_tutr(op->data,object,tutr);
		   iec60870_record _rec(otd_fa,op->addr->sb,object);
		   iec60870_records_t::index_iterator iptr;
		   TLockHelper l(this->storage_locker);
		   if(recs->find(_rec,iptr))
         	   {
                    iec60870_record & ref_rec = recs->at(*iptr);
                    ref_rec.changes_mask = IEC60870_REC_FL_RC_STATE;
                    if(tutr.command!=OTD_TUTROP_CANCEL)
                    {
                    if(ref_rec.rc_command !=(BYTE)-1 && ref_rec.rc_number != (DWORD)-1 )
                      {
                            ref_rec.rc_state  = tutr.command == OTD_TUOP_ON ? OTD_PSTATE_TUTR_ON_MORE : OTD_PSTATE_TUTR_OFF_LESS;

                            if(tutr.command_attr&OTD_TUTR_CMDATTR_INVERSE)
                               ref_rec.options|= IEC60870_REC_DYNOPT_INVERSE;
                                else
                               ref_rec.options&=~IEC60870_REC_DYNOPT_INVERSE;

                            if(tutr.command_attr&OTD_TUTR_CMDATTR_CHECK_SUCCESS)
                               ref_rec.options|= IEC60870_REC_DYNOPT_CHECK_RC_SUCCESS;
                                else
                               ref_rec.options&=~IEC60870_REC_DYNOPT_CHECK_RC_SUCCESS;


                            BYTE buf[256];
                            iec60870_command_from_otd(ref_rec,tutr);
                            ref_rec.rc_ctrl.se  = cs_select; //Выбрать
                            lpiec60870_proto_header_t phdr ;
                            phdr = iec60870_proto_control(buf,sizeof(buf),line_config.dev_addr,line_config.common_addr
                                                                                       ,ref_rec.rc_command,ref_rec.rc_number,line_config.obj_addr_size,ref_rec.rc_ctrl);
                            send(phdr);
                            if(!ref_rec.rc_timer)
                                ref_rec.rc_timer = 1000;
                            tutr_active.push_back(ref_rec);
                      }
                      else
                      {
                        //TODO
                        //взвести диагностику отсутствие описателя ТУ/ТР
                          ref_rec.rc_state = OTD_PDIAG_TUTR_DESCRIPT;
                       }
                    }
                    else
                    {
                     //Получена  команда отмены
                     ref_rec.rc_state &=~ (OTD_PSTATE_TUTR_PREPARE|OTD_PSTATE_TUTR_ACTIVE);
                    }
                      __record_changed(ref_rec);

               }
               else
               {
                // TODO в отчет нет такого объекта
               }
         }
      }
  }

   bool __fastcall Tiec60870line::is_countered_timer_fired()
   {
    if(timer_count)
    {
      if(++timer_counter>=timer_count)
        {
         timer_counter = 0;
         return true;
        }
    }
    else
    timer_counter = 0;
    return false;
   }

   DWORD __fastcall Tiec60870line::refresh(DWORD sbl)
   {
      BYTE buff[64] ;
      lpiec60870_proto_header_t hptr = iec60870_proto_interrogate(buff,sizeof(buff),0,get_dev_addr(),get_addr_size());
      send(hptr);
      return GKH_RET_SUCCESS;
   }


  void  __fastcall Tiec60870line::send(lpiec60870_proto_header_t phdr)
  {
    txbuf.add_bytes((LPBYTE)phdr,iec60870_proto_get_frame_length(phdr));
    if(!WT[WT_TX_TIMER])  WTimer_start(WT_TX_TIMER);
  }

  void  __fastcall Tiec60870line::__do_receipt_rx(bool force)
  {
      if(W_counter)
      {
        BYTE buf[32];
        lpiec60870_proto_header_t phdr = iec60870_proto_init_frame(buf,sizeof(buf),NULL,0);
        phdr->acpiS.format = acpi_type_S;
        WORD rx_num     = rx_number.get_number();
        ++rx_num;
        phdr->acpiS.rx_number.set_number(rx_num);
        do_socket_send((LPBYTE)phdr,iec60870_proto_get_frame_length(phdr));
        W_counter = 0;
      }
  }

   void  __fastcall Tiec60870line::__send_next_frame()
   {
     if(K_counter < get_K_count())
     {
         TLockHelper l(txbuf.get_locker());
         lpiec60870_proto_header_t phdr = txbuf.get_current_frame();
         if(phdr)
         {
              phdr->acpiI.tx_number.set_number(tx_number.get_number()+K_counter);
              phdr->acpiI.rx_number.set_number(rx_number.get_number());
              if(do_socket_send((LPBYTE)phdr,iec60870_proto_get_frame_length(phdr)))
              {
                W_counter = 0;
                ++K_counter;
                txbuf.next_frame(false);
                WTimer_start(WT_TX_TIMER);
              }
         }
     }
     else
     {
       K_counter = 0;
       txbuf.set_current_frame(0);
       WTimer_start(WT_TX_TIMER);
     }
   }



   iec60870_records_t * __fastcall Tiec60870line::get_records_for(DWORD otd_fa)
   {

     switch(otd_fa)
     {
      case OTD_FA_DISCRETE        :return &ts_recs ;
      case OTD_FA_ANALOG          :return  &tit_recs;
      case OTD_FA_ANALOG_INTEGR   :return  &tii_recs;
     }
     //SetLastError(UNKNOWN_OTD_FA);
     return 0;
   }


   DWORD __fastcall Tiec60870line::get_asdu_records_count(DWORD otd_fa)
   {
     iec60870_records_t * _recs = get_records_for(otd_fa);
     if( _recs )
         return _recs->size();
     return 0;
   }

   DWORD __fastcall Tiec60870line::enum_asdu_records     (lpiec60870_record rec)
   {
     iec60870_records_t * _recs = get_records_for(rec->otd_fa);
     if( _recs )
     {
       TLockHelper l(storage_locker);
       if( rec->number < _recs->size())
          {

          *rec = _recs->at(rec->number);
          return GKH_RET_SUCCESS;
          }
//          else
//          SetLastError(IEC60870_ERROR_NO_MORE_RECORDS);
     }
//     else
//     SetLastError(IEC60870_ERROR_NO_ASDU_TYPE);
    return GKH_RET_ERROR;
   }

   DWORD __fastcall Tiec60870line::get_asdu_record       (lpiec60870_record rec)
   {
     iec60870_records_t * _recs = get_records_for(rec->otd_fa);
     if(_recs)
     {
       TLockHelper l(storage_locker);
       iec60870_records_t::iterator ptr = std::lower_bound(_recs->begin(),_recs->end(),*rec,iec60870_record_less());
       if(ptr < _recs->end() && 0 == iec60870_record_comparer().compare(*ptr,*rec) )
          {
          *rec = *ptr;
          return GKH_RET_SUCCESS;
          }
//          else
//          SetLastError(IEC60870_ERROR_NOT_FOUND_RECORDS);

     }
//     else
//     SetLastError(IEC60870_ERROR_NO_ASDU_TYPE);
    return GKH_RET_ERROR;
   }


   DWORD    __fastcall Tiec60870line::get_group_min_max_numbers(DWORD otd_fa,DWORD otd_group,LPDWORD min_num,LPDWORD max_num)
   {
     DWORD ret = 0;
     if(max_num) *max_num = 0;
     if(min_num) *min_num = 0;
     iec60870_records_t * recs = this->get_records_for(otd_fa);
     if(recs)
     {
      TLockHelper l(storage_locker);
      recs->build_index() ;
      iec60870_records_t::index_iterator ibeg = recs->index_begin(),iend = recs->index_end();
      iec60870_record v1(otd_fa,otd_group,0);
      iec60870_record v2(otd_fa,otd_group,-1);
      if(recs->range(v1,v2,ibeg,iend))
      {
         if(max_num)
         {
         --iend;
         *max_num = recs->at(*iend).otd_number;
         }

         if(min_num)
          *min_num = recs->at(*ibeg).otd_number;

      }
     }
     return ret;

   }

   DWORD __fastcall Tiec60870line::get_group_count(DWORD otd_fa,DWORD otd_group)
   {
     DWORD ret = 0;
     iec60870_records_t * recs = this->get_records_for(otd_fa);
     if(recs)
     {
      TLockHelper l(storage_locker);
      recs->build_index() ;
      iec60870_records_t::index_iterator ibeg = recs->index_begin()  ,iend = recs->index_end();
      iec60870_record v1(otd_fa,otd_group,0);
      iec60870_record v2(otd_fa,otd_group,0);
      ret = recs->range(v1,v2,ibeg,iend);
     }
     return ret;
   }

   void  __fastcall Tiec60870line::__setup_group_values (otd_proto * op)
   {
     iec60870_records_t * recs = get_records_for(op->addr->fa);
     if(recs)
     {
        TLockHelper l(storage_locker);
        recs->build_index() ;
        iec60870_records_t::index_iterator ibeg = recs->index_begin()  ,iend = recs->index_end();
        iec60870_record v1(op->addr->fa,op->addr->sb,0);
        iec60870_record v2(op->addr->fa,op->addr->sb,-1);
        if(recs->range(v1,v2,ibeg,iend));
        {
          while(ibeg<iend)
          {
            lpiec60870_record record =   &recs->at(*ibeg);
            DWORD value = record->dw_value;
            otd_set_value(op->data,record->otd_number,&value,sizeof(value));
            value = record->get_otd_diag();
            otd_set_value(op->personal_diag,record->otd_number,&value,sizeof(value));
            ++ibeg;
          }
         if(is_connected())
            *op->diag = 0;
          *op->diag = otd_scan_personal_diag(op);

        }

     }
   }

   LRESULT  __fastcall Tiec60870line::otd_delete_group      (BYTE otd_fa,BYTE otd_group)
   {
     sotd_addr addr(0,0,otd_fa,otd_group);
     proto_pointer ptr;
     TLockHelper l(storage_locker);
     if(storage.find(addr,ptr))
     {
         if(ptr->op.diag && is_connected())
         {
          *ptr->op.diag = -1;
          this->__queue_rxdata(&ptr->op);
         }
         LPBYTE buf = (LPBYTE)ptr->op.addr;
         storage.erase(ptr);
         delete [] buf;
     }
     return GKH_RET_ERROR;
   }

   DWORD __fastcall Tiec60870line::otd_create_group(DWORD otd_fa,DWORD otd_group,DWORD first,DWORD count,DWORD otd_type)
   {
     sotd_addr addr(0,0,otd_fa,otd_group);
     proto_pointer ptr;
     TLockHelper l(storage_locker);
     if(storage.find(addr,ptr))
     {
       if(!ptr->op.data ||   count != (DWORD) OTD_GET_COUNT(&ptr->op.data->numbers))
         {
          LPBYTE buf = (LPBYTE)ptr->op.addr;
          storage.erase(ptr);
          delete [] buf;
          return otd_create_group(otd_fa,otd_group,first,count,otd_type);
         }
     }
     else
     {
        otd_proto       op;
        op.dw_size   = sizeof(op);

        otd_proto_param opp;
        bzero(&opp,sizeof(opp));
        opp.dw_size = sizeof(opp);
        opp.parts   = OTD_PROTO_PART_KPK|OTD_PROTO_PART_DIAG|OTD_PROTO_PART_DATA|OTD_PROTO_PART_PERSONAL_DIAG|OTD_PROTO_PART_PERSONAL_CHMASK|OTD_PROTO_PART_TIMESTAMP;
        opp.dt_param.type   = otd_type;
        opp.dt_param.first  = first;
        opp.dt_param.count  = count;
        opp.pd_param = opp.dt_param;

        opp.pd_param.type = OTD_BYTE;
        opp.cm_param = opp.pd_param;

        DWORD  need_sz = otd_proto_calc_size_ex(&opp,0);
        LPBYTE buf = new BYTE[need_sz];
        if(!buf)
          {
            SetLastError(0/*NO_MEMORY*/);
            return GKH_RET_ERROR;
          }

         otd_proto_format_ex(buf,need_sz,&opp,0,&op);
         op.addr->addr = 0;
         op.addr->fa   = otd_fa;
         op.addr->sb   = otd_group;
         *op.diag      = OTD_DIAG_NODATA|OTD_PART_DIAG_PARAM;
         __int64 val = 0;
         otd_fill(op.data,&val,sizeof(val));
         otd_fill(op.personal_chmask,&val,sizeof(val));
         val = OTD_PDIAG_NODATA;
         otd_fill(op.personal_diag,&val,sizeof(val));
         __setup_group_values  (&op);
         ptr = storage.insert(op);
         if(is_worked()) __queue_rxdata(&op);

     }
     return GKH_RET_SUCCESS;
   }



  LRESULT  __fastcall Tiec60870line::otd_create_group      (LPIEC60870_LINE_GROUP_PARAM lgc)
   {
     iec60870_records_t * recs = this->get_records_for(lgc->otd_fa);
     if(recs)
     {
       TLockHelper l(storage_locker);
        recs->build_index() ;
        iec60870_records_t::index_iterator idx_beg = recs->index_begin()  ,idx_end = recs->index_end();
        iec60870_record v1(lgc->otd_fa,lgc->otd_group,0);
        iec60870_record v2(lgc->otd_fa,lgc->otd_group,-1);
        DWORD count = recs->range  (v1,v2,idx_beg,idx_end);
        if(count)
        {
          lpiec60870_record prec = &recs->at(*idx_beg);
          DWORD min_number = prec->otd_number;
          --idx_end;
          DWORD max_number = recs->at(*idx_end).otd_number;
          count = 1+max_number-min_number;
          otd_create_group(lgc->otd_fa,lgc->otd_group,min_number,count,prec->get_otd_type());
          handle_record_changes();
          return GKH_RET_SUCCESS;
        }
     }
     return GKH_RET_ERROR;
   }

    void  __fastcall Tiec60870line::otd_create_groups  (iec60870_records_t & recs)
    {
       TLockHelper l(storage_locker);
       if(recs.size())
       {
          //Создание групп
           recs.build_index();
           iec60870_records_t::index_iterator range_beg   = recs.index_begin(),idx_end = recs.index_end();
           iec60870_records_t::index_iterator range_end = idx_end;
           while(range_beg<idx_end )
           {
             lpiec60870_record  rec = &recs.at(*range_beg);
             iec60870_record    rec1(rec->otd_fa,rec->otd_group,-1);
             if(rec && rec->is_otd_assigned())
             {

               DWORD group_count = recs.range(*rec,rec1,range_beg,range_end);
               if(group_count)
               {
                rec = &recs.at(*range_beg);
                DWORD first_num =  rec->otd_number;
                range_beg = range_end;
                --range_beg;
                rec = &recs.at(*range_beg);
                group_count = 1+rec->otd_number-first_num;
                otd_create_group(rec->otd_fa,rec->otd_group,first_num,group_count,rec->get_otd_type());
               }
               else
               range_beg = idx_end;
              range_beg = range_end;
              range_end = idx_end;
             }
             else
             break;
           }

       }

    }


   void  __fastcall Tiec60870line::__queue_rxdata(otd_proto * op)
   {
      op->addr->cp = line_number;
      queue_rxdata(FA_OTD,(LPBYTE)op->addr,op->proto_size,false,0);
      op->addr->cp = 0;
   }


   void  __fastcall Tiec60870line::__notify_record_changes(DWORD line_num,iec60870_records_t::container_t::iterator cbeg,iec60870_records_t::container_t::iterator cend)
   {
       while(cbeg<cend)
        {
          iec60870_record & rec = *cbeg;
          if(rec.number == 6)
             TRACE(_T("changed rec %d mask %X rc_state %d"),rec.number,rec.changes_mask,rec.rc_state);
          owner->notify(MNF_LINE_RECORD_CHANGED,line_num,&rec,sizeof(rec));
          ++cbeg;
        }
   }


   LRESULT __fastcall Tiec60870line::record_bind(lpiec60870_record rec)
   {

     iec60870_records_t * records = get_records_for(rec->otd_fa);
     if(records)
     {
       DWORD line_num = this->get_number();
       TLockHelper l(storage_locker);
       iec60870_records_t::iterator rec_ptr;

       if(rec->otd_group == BYTE(-1) || rec->otd_number == (DWORD)-1)
          rec->otd_group = BYTE(-1) , rec->otd_number = DWORD(-1);

       if(records->find(*rec,rec_ptr) && iec60870_record_comparer().compare_otd_number(*rec,*rec_ptr))
        {
          //Идет изменение привязки отд

          if(rec->otd_number != (DWORD)-1)
             {
              //Проверка того что адрес не сущтвует
              iec60870_records_t::index_iterator iptr ;
              if(records->find(*rec,iptr))
                 return GKH_RET_ERROR;
             }
          bool bad_diag = rec->otd_number == (DWORD)-1 ? true : false;
          __record_changed(*rec_ptr,bad_diag);
          handle_record_changes();

          BYTE prev_group = rec_ptr->otd_group;
          rec_ptr->otd_group  = rec->otd_group;
          rec_ptr->otd_number = rec->otd_number;
          rec_ptr->changes_mask = (IEC60870_REC_FL_OTD_GROUP|IEC60870_REC_FL_OTD_NUMBER);
          records->index_invalidate();
          records->build_index();
        __record_changed(*rec_ptr,bad_diag);

          if(bad_diag && prev_group != BYTE(-1))
          {
            iec60870_record r1(rec->otd_fa,prev_group,0);
            iec60870_record r2(rec->otd_fa,prev_group,-1);
            iec60870_records_t::index_iterator iptr1 = records->index_begin()  ,iptr2 = records->index_end() ;
            if(!records->range(r1,r2,iptr1,iptr2))
                  otd_delete_group(rec_ptr->otd_fa,prev_group);
          }
          return GKH_RET_SUCCESS;
        }
        else
        SetLastError(0/*RECORD NOT_FOUND*/);
     }
     return GKH_RET_ERROR;
   }

   LRESULT  __fastcall Tiec60870line::record_bind_rccmd(lpiec60870_record rec)
   {
     iec60870_records_t * records = get_records_for(rec->otd_fa);
     if(records)
     {
       //DWORD line_num = get_number();
       TLockHelper l(storage_locker);
       iec60870_records_t::iterator rec_ptr;
       if(records->find(*rec,rec_ptr))
        {
         if(rec_ptr->rc_command != rec->rc_command || rec_ptr->rc_number != rec->rc_number || rec_ptr->rc_ctrl.qu != rec->rc_ctrl.qu || rec_ptr->rc_timer != rec->rc_timer)
           {
             rec_ptr->rc_command = rec->rc_command;
             rec_ptr->rc_number  = rec->rc_number ;
             rec_ptr->rc_ctrl.qu = rec->rc_ctrl.qu;
             rec_ptr->rc_timer   = rec->rc_timer;
             rec_ptr->changes_mask   = IEC60870_REC_FL_RC_COMMAND|IEC60870_REC_FL_RC_NUMBER|IEC60870_REC_FL_RC_STATE|IEC60870_REC_FL_RC_PARAM;
             *rec = *rec_ptr;
             //owner->notify(MNF_LINE_RECORD_CHANGED,line_num,rec,sizeof(*rec));
             this->__record_changed(*rec);
             return GKH_RET_SUCCESS;
           }
        }
     }
     return GKH_RET_ERROR;
   }




   void  __fastcall Tiec60870line::__handle_record_changes_range(DWORD line_num,iec60870_records_t::container_t::iterator cbeg,iec60870_records_t::container_t::iterator cend)
   {
      proto_pointer proto_ptr;
      if(cbeg->otd_number!=(DWORD)-1)
       {
        sotd_addr addr(0,0,cbeg->otd_fa,cbeg->otd_group);
        if(storage.find(addr,proto_ptr))
        {

          DWORD zero_val = 0;
          otd_fill(proto_ptr->op.personal_chmask,&zero_val,sizeof(zero_val));
          *proto_ptr->op.diag = 0;
           DWORD ch_count = 0;

           while(cbeg<cend)
           {
               if(otd_proto_setvalue(&proto_ptr->op,cbeg->otd_number,&cbeg->dw_value,sizeof(cbeg->dw_value),cbeg->get_otd_diag(),1))
               {
                ++ch_count;
                *proto_ptr->op.time_stamp = cbeg->timestamp;
               }
           ++cbeg;
           }
         if(ch_count  )
           {
            *proto_ptr->op.diag = 0;
            *proto_ptr->op.diag = otd_scan_personal_diag(&proto_ptr->op);
            __queue_rxdata(&proto_ptr->op);
           }
          cbeg->changes_mask = 0;
        }
      }
   }

   void  __fastcall Tiec60870line::handle_record_changes ()
   {
      DWORD ln = get_number() ;
      bool wrk = is_worked();
      TLockHelper l(this->storage_locker);
      if(record_changes.size())
      {
          iec60870_records_t::container_t::iterator cbeg = record_changes.begin();
          iec60870_records_t::container_t::iterator cend = record_changes.end  ();
          __notify_record_changes(ln,cbeg,cend);
          if(wrk)
          {
              std::sort(cbeg,cend,iec60870_record_otd_less());
              cbeg = record_changes.begin();
              cend = record_changes.end  ();
              iec60870_records_t::container_t::iterator cptr = cbeg;
              while(cptr<cend)
              {
                cptr         = upper_bound(cbeg,cend,iec60870_record(cptr->otd_fa,cptr->otd_group,-1),iec60870_record_otd_less());
                DWORD  count = std::distance(cbeg,cptr);
                if(count)
                   __handle_record_changes_range(ln,cbeg,cptr);
                cbeg = cptr;
              }
          }
          record_changes.clear();
      }
   }



 DWORD    __fastcall  Tiec60870line::get_line_diag(bool rescan)
 {
    DWORD ret = 0;
    if(is_connected())
       {
          TLockHelper l(storage_locker);
          proto_pointer pptr = storage.begin();
          proto_pointer pend = storage.end();
          while(pptr<pend)
          {
           ++pptr;
          }
       }
       else
       ret = OTD_DIAG_CPMASK;
    return ret;

 }


 void    __fastcall  Tiec60870line::__do_recv_cp_data(bool inc_name )
 {
      TLockHelper l(locker);
      otd_proto_param opp;
      otd_proto       op;
      op.dw_size      = sizeof(op);
      ZeroMemory(&opp,sizeof(opp));
      opp.dw_size     = sizeof(opp);
      opp.parts       = OTD_PROTO_PART_DIAG;
      if(inc_name)
      {
       opp.parts       |= OTD_PROTO_PART_NAME;
       opp.name_sz     = lstrlenW(this->line_config.cp_name)+1;
      }
      BYTE   buffer [1024];
      DWORD need_size = otd_proto_calc_size_ex(&opp,0);
      LPBYTE  buf;
      buf = (need_size>sizeof(buffer)) ?    new BYTE[need_size] :   buffer;
      if(buf)
      {
      	 otd_proto_format_ex(buf,need_size,&opp,0,&op);
      	 op.addr->addr = -1;
      	 op.addr->pu   = 0;
         op.addr->cp   = 0;
         *op.diag      = get_line_diag(false);
      	 otd_text_set(op.name,line_config.cp_name);
      	 __queue_rxdata(&op);
      	 if(buf!=buffer) delete [] buf;
       }
  }

