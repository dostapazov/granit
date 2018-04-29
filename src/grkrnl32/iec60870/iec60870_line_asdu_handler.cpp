 #pragma hdrstop
 #include "iec60870_modem.hpp"


    void __fastcall Tiec60870line::handle_frame_typeI(lpiec60870_proto_header_t phdr)
    {

       __check_rx_number(phdr->acpiI.tx_number.get_number());
       __check_tx_number(phdr->acpiI.rx_number.get_number());
       lpiec60870_asdu_header asdu_hdr = iec60870_proto_get_asdu_header(phdr);
       if(asdu_hdr)
       {
        handle_asdu(phdr);
       }
       else
       {
          wchar_t rep_text[256];
          snwprintf(rep_text,KERTL_ARRAY_COUNT(rep_text),L"%u.%u: Кадр №-%u без ASDU  ",owner->get_modem_number(),get_number(), phdr->acpiI.tx_number.get_number());
          do_report(REPORT_ERROR_TYPE,rep_text,phdr,iec60870_proto_get_frame_length(phdr));
       }
    }

   __int64  __fastcall Tiec60870line::get_timestamp(lpiec60870_cp24time2a tm)
   {
     __int64 timestamp;
     SYSTEMTIME st;
     GetLocalTime(&st);
     if(tm)
      {
        st.wSecond       = tm->msec /1000;
        st.wMilliseconds = tm->msec %1000;
      }
    SystemTimeToFileTime(&st,(LPFILETIME)&timestamp);
    return timestamp;
   }

   __int64  __fastcall Tiec60870line::get_timestamp(lpiec60870_cp56time2a tm)
   {
     __int64 timestamp;
     SYSTEMTIME st;
     if(tm)
      {
        st.wYear         = tm->year+2000;
        st.wMonth        = tm->month;
        st.wDay          = tm->mday;
        st.wHour         = tm->hour;
        st.wMinute       = tm->min;
        st.wSecond       = tm->msec /1000;
        st.wMilliseconds = tm->msec %1000;
      }
      else
      GetLocalTime(&st);
     SystemTimeToFileTime(&st,(LPFILETIME)&timestamp);
    return timestamp;
   }

   void     __fastcall Tiec60870line::set_timestamp(__int64 tmstamp,lpiec60870_cp56time2a tm)
   {
      bzero(tm,sizeof(*tm));
      if(tmstamp)
      {
       SYSTEMTIME st;
       FileTimeToSystemTime((LPFILETIME)&tmstamp,&st);
       tm->year  = st.wYear - 2000;
       tm->month = st.wMonth;
       tm->wday  = st.wDay;
       tm->hour  = st.wHour;
       tm->min   = st.wMinute;
       tm->msec  = (st.wSecond*1000) + st.wMilliseconds;
      }
   }

    void __fastcall Tiec60870line::__record_changed(iec60870_record & rec,bool bad_diag)
    {
      TLockHelper l(storage_locker);
      iec60870_records_t::iterator ptr = record_changes.insert(record_changes.end(),rec);
      if(bad_diag) {
         ptr->changes_mask |= IEC60870_REC_FL_QUALITY;
         ptr->quality.quality_byte = -1;
      }

      if(rec.changes_mask & IEC60870_REC_FL_VALUE)
      {
       // Изменилось значение
       if(rec.rc_state & OTD_PSTATE_TUTR_ACTIVE)
         {
         TCHAR text[1024];
         int len = snwprintf(text,sizeof(text),_T("CHANGE VALUE Num = %d "),rec.number);
         if(rec.is_float)
           len += snwprintf(text+len,sizeof(text)-len,_T("value = %.3f "),rec.fl_value);
          else
           len += snwprintf(text+len,sizeof(text)-len,_T("value = %d "),rec.dw_value);

           len += snwprintf(text+len,sizeof(text)-len,_T(" TU_TR operation - "),rec.dw_value);

           if(is_tu_success(rec))
              {
               len += snwprintf(text+len,sizeof(text)-len,_T("successed. timer removed"),rec.dw_value);
               __tutr_finish(rec,0);
              }
          TRACE(text,0);
         }
      }

      if(rec.changes_mask & IEC60870_REC_FL_RC_STATE)
         {
           TCHAR text[1024];
           snwprintf(text,sizeof(text),_T("RC_STATE changed Num = %d  RC_STATE - %04X"),rec.number,rec.rc_state);
           TRACE(text,0);
         }
    }


    void __fastcall Tiec60870line::__update_record(iec60870_record & src_rec)
    {
      iec60870_records_t * records = get_records_for(src_rec.otd_fa);
      if(records)
      {
           TLockHelper l(storage_locker);
           iec60870_records_t::iterator rec_ptr;
           if(records->find(src_rec,rec_ptr))
            {
             iec60870_record & rec = *rec_ptr;
             rec.changes_mask = rec ^ src_rec;
             if((rec.changes_mask&IEC60870_REC_FL_TIMESTAMP) && (rec.timestamp < src_rec.timestamp))
             {
              rec.dw_value = src_rec.dw_value;
              rec.quality.quality_byte = src_rec.quality.quality_byte;
              rec.timestamp = src_rec.timestamp;
              rec.asdu_type = src_rec.asdu_type;
              __record_changed(rec);
             }
            }
            else
            {
              rec_ptr = records->insert(rec_ptr,src_rec);
              rec_ptr->changes_mask = -1;
              __record_changed(*rec_ptr);
            }
       }
    }

    template <typename obj_t>
    void __fastcall Tiec60870line::handle_objects(lpiec60870_asdu_header asdu)
    {
        //DWORD  asdu_type = asdu->asdu_type;
        if(asdu->test)
           {
            //TODO test_handle
            return ;
           }
        DWORD obj_count = asdu->obj_count;
        LPBYTE ptr = (LPBYTE)asdu;
        ptr+=sizeof (*asdu);
        bool is_sequenced = asdu->seq ? true : false;
        DWORD addr_sz = get_addr_size();

        DWORD obj_num = iec60870_proto_get_obj_addr(ptr,addr_sz);
        ptr+=addr_sz;

        while(obj_count)
        {
          obj_t * obj = (obj_t *)ptr;
          ptr+=sizeof(*obj);

          iec60870_record rec(obj,obj_num,(DWORD)asdu->asdu_type);
          __update_record(rec);
          if(--obj_count)
             {
              if(is_sequenced)
                 ++obj_num;
                 else
                 {
                  obj_num = iec60870_proto_get_obj_addr(ptr,addr_sz);
                  ptr+=addr_sz;
                 }
             }
        }

	}

	void __fastcall Tiec60870line::handle_asdu(lpiec60870_proto_header_t phdr)
	{
		wchar_t rep_text[256];
		lpiec60870_asdu_header asdu = iec60870_proto_get_asdu_header(phdr);
		DWORD obj_count = asdu->obj_count;
		if(obj_count)
		{
		 switch(asdu->asdu_type)
		 {
		  case M_SP_NA_1: handle_objects<iec60870_type_M_SP_NA_1>(asdu);break;
		  case M_SP_TA_1: handle_objects<iec60870_type_M_SP_TA_1>(asdu);break;
		  case M_DP_NA_1: handle_objects<iec60870_type_M_DP_NA_1>(asdu);break;
		  case M_DP_TA_1: handle_objects<iec60870_type_M_DP_TA_1>(asdu);break;
                  case M_ST_NA_1: handle_objects<iec60870_type_M_ST_NA_1>(asdu);break;
                  case M_ST_TA_1: handle_objects<iec60870_type_M_ST_TA_1>(asdu);break;
		  case M_ME_NA_1: handle_objects<iec60870_type_M_ME_NA_1>(asdu);break;
		  case M_ME_TA_1: handle_objects<iec60870_type_M_ME_TA_1>(asdu);break;
		  case M_ME_NB_1: handle_objects<iec60870_type_M_ME_NB_1>(asdu);break;
                  case M_ME_TB_1: handle_objects<iec60870_type_M_ME_TB_1>(asdu);break;
		  case M_ME_NC_1: handle_objects<iec60870_type_M_ME_NC_1>(asdu);break;
		  case M_ME_TC_1: handle_objects<iec60870_type_M_ME_TC_1>(asdu);break;
		  case M_SP_TB_1: handle_objects<iec60870_type_M_SP_TB_1>(asdu);break;
		  case M_DP_TB_1: handle_objects<iec60870_type_M_DP_TB_1>(asdu);break;
		  case M_МЕ_ТD_1: handle_objects<iec60870_type_M_ME_TD_1>(asdu);break;
		  case M_МЕ_ТЕ_1: handle_objects<iec60870_type_M_ME_TE_1>(asdu);break;
		  case M_ME_TF_1: handle_objects<iec60870_type_M_ME_TF_1>(asdu);break;
		  case C_IC_NA_1: /*"Команда опроса"             */;break;
		  case C_CI_NA_1: /*"Команда опроса счетчиков"   */; break;
		  case C_RD_NA_1: /*"Команда чтения"             */; break;
		  case C_CS_NA_1: /*"Команда синхронизации часов"*/; break;
		  case C_TS_NA_1: /*"Тестовая команда"           */; break;
		  case C_RP_NA_1: /*"Команда установки процесса в исходное состояние Reset" */; break;
		  case C_CD_NA_1: /*"Команда задержки опроса"    */; break;
		  case C_TS_TA_1: /*"Тестовая команда с меткой времени СР56Время2а"*/         ; break;
		  case C_SC_NA_1:
		  case C_DC_NA_1:
		  case C_RC_NA_1: handle_remote_control(phdr);break;
		  default:
		  snwprintf(rep_text,KERTL_ARRAY_COUNT(rep_text),L"%u.%u: Кадр №-%u неизвестный тип ASDU-%d ",owner->get_modem_number(),get_number(), phdr->acpiI.tx_number.get_number(),asdu->asdu_type);
		  do_report(REPORT_ERROR_TYPE,rep_text,phdr,iec60870_proto_get_frame_length(phdr));
		  break;
		 }
		}
		else
		{
		  snwprintf(rep_text,KERTL_ARRAY_COUNT(rep_text),L"%u.%u: Кадр №-%u ASDU c нулевым количеством объектов ",owner->get_modem_number(),get_number(), phdr->acpiI.tx_number.get_number());
		  do_report(REPORT_ERROR_TYPE,rep_text,phdr,iec60870_proto_get_frame_length(phdr));
		}
	   handle_record_changes();
	}

  bool  __fastcall Tiec60870line::is_tu_success    (const iec60870_record & rec )
  {
    if(rec.otd_fa == OTD_FA_DISCRETE && (rec.rc_state & OTD_PSTATE_TUTR_ACTIVE))
    {
        bool current_pos = bool(rec.dw_value);
        if(rec.options & IEC60870_REC_DYNOPT_INVERSE)
           current_pos = !current_pos;
        bool command_pos = (rec.rc_state & OTD_PSTATE_TUTR_ACTIVE) == OTD_PSTATE_TUTR_ON_MORE  ? true : false;
        return current_pos == command_pos ? true : false;
    }
    return true;
  }

  void  __fastcall Tiec60870line::tutr_remove_timer(const iec60870_record & rec )
  {
    TRACE(_T("Удаление таймера ТУ rec %d "),rec.number);
    TLockHelper l(locker);
    iec60870_records_t::container_t::iterator ptr ;
    if(tutr_find_record(rec.rc_command,rec.rc_number,ptr))
       tutr_active.erase(ptr);


  }

  bool  __fastcall is_on_command(DWORD iec_rc_command,iec60870_rctrl & rc_ctrl)
  {
        if(iec_rc_command == C_SC_NA_1)
          return (rc_ctrl.var&1)   ? true : false;
        if(iec_rc_command == C_DC_NA_1)
          return  (rc_ctrl.var == dp_on)  ? true : false;
      return false;
  }

  bool  __fastcall Tiec60870line::tutr_need_check(const iec60870_record & rec )
  {
    bool ret = false;
    if(rec.otd_fa == OTD_FA_DISCRETE)
    {
        bool current_pos = bool(rec.dw_value);
        if(rec.options & IEC60870_REC_DYNOPT_INVERSE)
           current_pos = !current_pos;
        bool command_pos = current_pos;

        switch(rec.rc_command)
        {
          case C_SC_NA_1: command_pos =  (rec.rc_ctrl.var&1)         ? true : false;
          break;
          case C_DC_NA_1: command_pos =  (rec.rc_ctrl.var == dp_on)  ? true : false;
          break;
        }
      ret = current_pos != command_pos ? true : false;
    }
    return ret;
  }

  void  __fastcall Tiec60870line::tutr_start_timer(iec60870_record & rec )
  {
     iec60870_records_t * recs = get_records_for(rec.otd_fa);
     iec60870_records_t::iterator rptr;
     TLockHelper l(storage_locker);
     if(recs && recs->find(rec,rptr))
     {
        rec.rc_timer   = rptr->rc_timer;
        rptr->rc_state = rec.rc_state;
        TRACE(_T("Перезапуск таймера ТУ rec %d "),rec.number);
     }
  }

  bool  __fastcall Tiec60870line::tutr_find_record(BYTE command,DWORD obj_num,iec60870_records_t::iterator & ptr )
  {
     ptr = tutr_active.begin();
     iec60870_records_t::iterator end =  tutr_active.end();
     while(ptr<end)
     {
      if(ptr->rc_command == command && ptr->rc_number == obj_num)
         return true;
      ++ptr;
     }
     return false;
  }

  void  __fastcall Tiec60870line::__tutr_finish    (const iec60870_record & rec,DWORD rc_state)
  {
     TRACE(_T("Завершение ТУ rec %d "),rec.number);
     TLockHelper l(storage_locker);
     tutr_remove_timer(rec);
     iec60870_records_t * recs = get_records_for(rec.otd_fa);
     iec60870_records_t::iterator rptr;
     if(recs && recs->find(rec,rptr))
     {
       rptr->rc_state      = rc_state;
       rptr->changes_mask  = IEC60870_REC_FL_RC_STATE;
      __record_changed(*rptr);
     }
     handle_record_changes();
  }

  void  __fastcall Tiec60870line::handle_remote_control(lpiec60870_proto_header_t phdr )
  {
    lpiec60870_asdu_header asdu = iec60870_proto_get_asdu_header(phdr);
//    if(asdu->cause == tc_activation_cfrm && !asdu->pn)
//       ;
    LPBYTE ptr = (LPBYTE)asdu;
    ptr+=sizeof (*asdu);
    DWORD addr_sz = get_addr_size();
    DWORD obj_num = iec60870_proto_get_obj_addr(ptr,addr_sz);
    ptr+=addr_sz;
    iec60870_rctrl * rctrl = (iec60870_rctrl *)ptr;
    iec60870_records_t::iterator tutr_ptr;

    if(tutr_find_record(asdu->asdu_type,obj_num,tutr_ptr ))
     {
      // Нашли в активных ТУ
         iec60870_record  rec = *tutr_ptr;
         TCHAR text[1024];
         int  text_len = 0;
         text_len += snwprintf(text,KERTL_ARRAY_COUNT(text)-text_len,_T("asdu->cause = %d ** %d[%s-%d.%d][cmd-%d rc_num %d]")
                      ,(DWORD)asdu->cause
                      ,(DWORD)rec.number,(DWORD)rec.otd_fa ? _T("ТИТ"):_T("TC"),(DWORD)rec.otd_group,(DWORD)rec.otd_number
                      ,(DWORD)rec.rc_command,(DWORD)rec.rc_number
                      );

      TRACE(text,0);
      if(!asdu->pn && asdu->cause == tc_activation_cfrm)
       {
        if(rctrl->se == cs_execute)
         {
           // Здесь определить  надо ли перезапускать таймер ту для контроля успеха ту
           // таймер перезапускается в случае если отправлена противоположная команда
           // т.е. На вкл - отключить, на откл.- включить
           TRACE(_T("ТУ cs_execute rec %d "),rec.number);
           if(tutr_need_check(*tutr_ptr))
              {
               tutr_start_timer(*tutr_ptr);
              }
              else
              {
                 __tutr_finish(rec,0);
              }
         }
         else
         {
           TRACE(_T("ТУ cs_select отправка cs_execute rec %d "),rec.number);
           asdu->cause = tc_activation;
           rctrl->se   = cs_execute;
           send(phdr);

           tutr_ptr->changes_mask = IEC60870_REC_FL_RC_STATE;
           tutr_ptr->rc_state     = is_on_command(tutr_ptr->rc_command, *rctrl) ? OTD_PSTATE_TUTR_ON_MORE : OTD_PSTATE_TUTR_OFF_LESS;
           tutr_start_timer(*tutr_ptr);
         }
        }
        else
        {
          //Отрицательное подтверждение
          __tutr_finish(rec,OTD_PDIAG_TUTR_DESCRIPT);
        }

//      if(tutr_ptr->changes_mask)
//       {
//        tutr_ptr->timestamp     = GetTime(false);
//        tutr_ptr->changes_mask |= IEC60870_REC_FL_TIMESTAMP;
//        __record_changed(*tutr_ptr);
//       }

     }
  }





