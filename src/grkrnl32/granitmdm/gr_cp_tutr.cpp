#include <algorithm>
#include <stdio.h>
#pragma hdrstop
#include "granit.hpp"



 DWORD      __fastcall gr_cp::test_tutr(LPSC_TEST_TUTR tt)
 {
     static int ivar = 0;
     
     DWORD ret =  GKH_RET_SUCCESS;
     DWORD error ;
  if(this->flags &GRSC_FLAG_TEST)
  {  //TODO поиск и перекодировка
     GRSC_TU_ENTRY tu_entry ;
       otd_tutr cmd;
       cmd.command = tt->tu_command;
       cmd.param   = 0;
       error = tutr_command (-1,tt->addr.addr,tt->object,cmd,&tu_entry);
 }
 else
    error = GRSCERR_TEST_TUTR_DISABLED;
   if(error)
      {ret = GKH_RET_ERROR; SetLastError(error);}
   ivar++;   
   return ret;
 }

 bool       __fastcall gr_cp::tutr_is_ready    (DWORD mod_number)
 {
   /*Готовность К ТУ / ТР*/
   return tu_active[mod_number] == 0 ? true:false;
 }



  tu_entry_list::iterator __fastcall gr_cp::tutr_lower_bound(DWORD addr,DWORD obj)
  {
   GRSC_TU_ENTRY tue;
   tue.dw_size   = sizeof(tue);
   tue.addr.addr = addr;
   tue.addr.fa&= OTD_FA_DATA_MASK;
   tue.object    = obj;
   tu_entry_list::iterator ret = std::lower_bound(tu_list.begin(),tu_list.end(),&tue,tu_entry_comparer());
   return ret;
  }

  DWORD          __fastcall gr_cp::tutr_find_entry(DWORD addr,DWORD obj)
  {
    DWORD ret = -1;
    sotd_addr sa(addr);
    sa.fa&=OTD_FA_DATA_MASK;
    sa.pu = 0;
    tu_entry_list::iterator ptr = tutr_lower_bound(sa.addr,obj);
    if(ptr<tu_list.end() && (*ptr)->addr.addr == sa.addr && (*ptr)->object == obj)
      return distance(tu_list.begin(),ptr);
    return ret;
  }

 DWORD                   __fastcall gr_cp::tutr_add        (gr_group * grp,LPGRSC_TU_ENTRY entry,DWORD modem_addr)
 {
  DWORD ret = -1;
  if(entry)
  {
   entry =   tue_init(new GRSC_TU_ENTRY,entry);
   if(entry)
   {
     entry->owner_addr = modem_addr;
     entry->tu_state   = GRSC_TUTR_STATE_SELECTED;
     if(!entry->tu_select_timeout)  entry->tu_select_timeout = 10000;
     if(!entry->tu_hold_time  )  entry->tu_hold_time   = 2500;
     if(!entry->tu_timeout    )  entry->tu_timeout     = 10000;
     if(is_RA(entry))
        entry->tu_timeout =   entry->tu_hold_time-500;
        entry->tu_hold_time+=grp->get_respond_time()/2;
     entry->tu_time = GetTime(false); // Запомнить время выбора для таймера автосброса
     tu_entry_list::iterator ptr;
     ptr = tutr_lower_bound(entry->addr.addr,entry->object);
     ptr = tu_list.insert(ptr,entry);
     ret =  distance(tu_list.begin(),ptr);
     modem_notify(MNCODE_SC_DATACHANGED,get_otd_addr(),0,ret,ret,GRSC_ACR_TUENTRY_ADDED);
     if((DWORD)-1 != modem_addr)
      {
        BYTE buffer[MAX_PATH];
        DWORD len = otd_proto_format_tutr(buffer,sizeof(buffer),&entry->addr,entry->object,OTD_TUTR_CMDENABLE,NULL,NULL,0,NULL);
        queue_rxdata(FA_OTD,buffer,len,true,modem_addr);
      }
     grp->set_personal_diag(entry->object,entry->object,OTD_PSTATE_TUTR_PREPARE,true,true);
   }
  }
  return ret;
 }

 BOOL                    __fastcall gr_cp::tutr_remove     (DWORD idx)
 {
  if(idx<tu_list.size())
  {
   tu_entry_list::iterator ptr = tu_list.begin();
   advance(ptr,idx);
   LPGRSC_TU_ENTRY entry = *ptr;
   if(entry==tu_active[entry->tu_module])
     tu_active[entry->tu_module] = NULL;
    tu_list.erase(ptr);
    gr_group * grp = find_group(entry->addr);
    if(grp)
       grp->set_personal_diag(entry->object,entry->object,OTD_PSTATE_TUTR_MASK,false,true);
    delete(entry);
    modem_notify(MNCODE_SC_DATACHANGED,get_otd_addr(),0,idx,idx,GRSC_ACR_TUENTRY_REMOVED);
    return TRUE;
  }
  return FALSE;
 }



 DWORD      __fastcall gr_cp::tutr_select      (DWORD modem_addr,DWORD addr,DWORD obj,LPGRSC_TU_ENTRY tu_entry)
 {
  /*Выбор объекта для ТУ/ТР*/
  DWORD error = GRSCERR_TUTROBJECT_LOCKED; //granit_modem_err.h
  DWORD idx       = this->tutr_find_entry(addr,obj);
  if(idx>tu_list.size())
  {
   gr_group * grp = find_group(addr);
   if(!grp)
     error = GRSCERR_OBJECT_NOT_EXIST;
   else
   {
    GRSC_TU_ENTRY entry;
    entry.dw_size = sizeof(entry);
    if(!grp->get_tu_entry(obj,&entry))
       {
        error = GRSCERR_TUTR_DESCRIPT_NOT_EXIST;
        grp->set_personal_diag(obj,obj,OTD_PDIAG_TUTR_DESCRIPT,true,true);
       }
       else
       {
         if(tutr_add(grp,&entry,modem_addr)<tu_list.size())
         {
          error = 0;
          if(tu_entry) tue_init(tu_entry,&entry);
         }
       }
   }
  }
  return error;
 }

 DWORD      __fastcall gr_cp::tutr_deselect    (DWORD modem_addr,DWORD  addr,DWORD obj,LPGRSC_TU_ENTRY tu_entry)
 {
  DWORD error = GRSCERR_TUTROBJECT_NOT_SELECTED;
  DWORD idx   = this->tutr_find_entry(addr,obj);
  if(idx<tu_list.size())
  {
   LPGRSC_TU_ENTRY entry = tu_list.at(idx);
   if(tu_entry) tue_init(tu_entry,entry);
   if(entry->owner_addr != modem_addr)
      error = GRSCERR_TUTROBJECT_LOCKED;
      else
      {
        if(entry->tu_state& GRSC_TUTR_STATE_SELECTED )
           {
            error = 0;
            entry->tu_state&=~GRSC_TUTR_STATE_SELECTED;
            if(!(entry->tu_state& (GRSC_TUTR_STATE_ACTIVE_MASK |GRSC_TUTR_STATE_QUEUED) ))
                tutr_remove(idx);
            else
            modem_notify(MNCODE_SC_DATACHANGED,get_otd_addr(),0,idx,idx,GRSC_ACR_TUENTRY_CHANGED);

           }
      }
  }
  return error;
 }

  //otd_types.h  otd_diag.h

 DWORD        __fastcall gr_cp::tutr_send(LPMPROTO_HEADER mph,lpotd_proto op)
 {
  DWORD ret   = GKH_RET_ERROR;
  DWORD error ;
  otd_tutr tutr ;
  DWORD object = op->data->numbers.loN;
  tutr.command = 0;
  if(op->data->otd_type == OTD_TR_COMMAND_SET)
     otd_get_value(op->data,object,&tutr,sizeof(tutr));
     else
     otd_get_value(op->data,object,&tutr.command,sizeof(tutr.command));
  GRSC_TU_ENTRY tu_entry;
  ZeroMemory(&tu_entry,sizeof(tu_entry));
  tu_entry.dw_size = sizeof(tu_entry);
  switch(tutr.command&OTD_TUTR_CMDMASK)
  {
   case OTD_TUTR_CMDSELECT  : error   = tutr_select(mph->addr_from.addr,op->addr->addr,object ,&tu_entry);break;
   case OTD_TUTR_CMDDESELECT: error   = tutr_deselect(mph->addr_from.addr,op->addr->addr,object,&tu_entry);break;
   default:
    error   =  tutr_command(mph->addr_from.addr,op->addr->addr,object,tutr, &tu_entry);
    break;
  }
   
   if(error)
     {
      tutr_log(mph->addr_from.addr,op->addr->addr,object,tutr,&tu_entry,error);
      SetLastError(error);
     }
   else
   ret = GKH_RET_SUCCESS;

  return ret;
 }

 inline DWORD __fastcall get_error_facility(DWORD err)
 {return  ((err>>30)&0x3); }

 void __fastcall gr_cp::tutr_log(DWORD modem_addr,DWORD  addr,DWORD obj,otd_tutr & tutr,LPGRSC_TU_ENTRY tu_entry,DWORD error)
 {
   /*Формирование записи в LOG об операции ТУ,ТР*/
    DWORD   log_type = REPORT_SUCCESS_TYPE;
    MODEM_ADDR mfrom;
    mfrom.addr = modem_addr;
    sotd_addr sa(addr);
    wchar_t rep_text [MAX_PATH<<1];
    wchar_t rep_templ[MAX_PATH/2];
    int     rep_len ;
    TGKModule::get_lang_string(IDS_SC_TULOG,rep_templ,sizeof(rep_templ)/sizeof(wchar_t));
    rep_len = wsprintfW(rep_text,rep_templ,mfrom.sta,mfrom.modem,mfrom.line);

    TGKModule::get_lang_string(IDS_SC_TUTR_BEGIN+tutr.command,rep_templ,sizeof(rep_templ)/sizeof(wchar_t));
    rep_len +=wsprintfW(rep_text+rep_len,L"%s",rep_templ);

    TGKModule::get_lang_string(IDS_SC_TU_OBJ,rep_templ,sizeof(rep_templ)/sizeof(wchar_t));
    rep_len +=wsprintfW(rep_text+rep_len,rep_templ,(DWORD)sa.cp,(DWORD)(sa.fa&OTD_FA_DATA_MASK),(DWORD)sa.sb,obj);

    if(tu_entry)
    {
     TGKModule::get_lang_string(tu_entry->addr.fa ? IDS_SC_TR_GRPOBJ : IDS_SC_TU_GRPOBJ,rep_templ,sizeof(rep_templ)/sizeof(wchar_t));
     rep_len +=wsprintfW(rep_text+rep_len,rep_templ,(DWORD)tu_entry->tu_group,(DWORD)tu_entry->tu_object);
    }
   if(error)
      {
       owner->get_owner()->get_error_text(error,rep_text+rep_len,sizeof(rep_text)/sizeof(wchar_t)-rep_len);
       switch(get_error_facility(error))
       {
        case 1: log_type = REPORT_INFORMATION_TYPE;break;
        case 2: log_type = REPORT_WARNING_TYPE;break;
        case 3: log_type = REPORT_ERROR_TYPE;break;
       }
      }
   if(rep_len)
     tutr_report(log_type,rep_text);

 }


 DWORD      __fastcall gr_cp::tutr_activate    (DWORD idx)
 {
 DWORD error = GRSCERR_TUTROBJECT_LOCKED;
 /*Активизация команды ТУ*/
 if(idx<tu_list.size())
  {
   LPGRSC_TU_ENTRY entry = tu_list.at(idx);
   if(!(entry->tu_state&GRSC_TUTR_STATE_ACTIVE_MASK))
    {
      DWORD cmd   = get_tu_command(entry);
      BYTE  buf[128];
      DWORD len = gr_proto_tu(this->number,cmd,entry->tu_group,entry->tu_object,buf,sizeof(buf));
      if(len)
      {
        floor_tx(buf,len,true);
        entry->tu_time  = GetTime(false);
        entry->tu_state&=~GRSC_TUTR_STATE_QUEUED;//Снять признак помещения в очередь 
        entry->tu_state|= GRSC_TUTR_STATE_ACTIVE|GRSC_TUTR_STATE_HOLDING;
        tu_active[entry->tu_module] = entry;
        gr_group * grp = find_group(entry->addr);
        if(grp)
          {


           //Снять признак подготовки
           grp->set_personal_diag(entry->object,entry->object,OTD_PSTATE_TUTR_PREPARE,false,false );

           grp->set_personal_diag(entry->object,entry->object,
                                  (entry->tu_state & GRSC_TUTR_STATE_CMDON) ? OTD_PSTATE_TUTR_ON_MORE : OTD_PSTATE_TUTR_OFF_LESS
                                  ,true,true
                                  );
          }
        if(is_RA(entry))
         {
          //для ТР
          entry->end_value = -1;
         }
        else
        {
        entry->curr_value = entry->end_value = 0;
        //Успех ТУ/ТР проверяется при  не тестовом КП и неустановленном флаге не проверке успеха
        if(grp && !( (flags&GRSC_FLAG_TEST) || (entry->tu_flags&GRSC_TUFL_NOCHECK) ) )
        {
         entry->curr_value = grp->get_object_value(entry->object,(entry->tu_flags&GRSC_TUFL_TSINVERSE) ? true:false);
         if(((entry->tu_state&GRSC_TUTR_STATE_CMDON) && !entry->curr_value) || ((entry->tu_state&GRSC_TUTR_STATE_CMDOFF) && entry->curr_value))
           {
            entry->tu_state|=GRSC_TUTR_STATE_WAITING;
            entry->end_value = (!entry->curr_value)&1;
           }
        }
       }
       modem_notify(MNCODE_SC_DATACHANGED,get_otd_addr(),0,idx,idx,GRSC_ACR_TUENTRY_CHANGED);
       error = 0;
       otd_tutr _tc;
       _tc.command = entry->tu_state&GRSC_TUTR_STATE_CMDON ? OTD_TUOP_ON:OTD_TUOP_OFF;
       tutr_log(entry->owner_addr,entry->addr.addr,entry->object,_tc,entry,GRSCINFO_TUTR_SENDED);
      }
      else
      error = 0;//Error Format Tu Kadr
    }
    else
    error = GRSCERR_TUTRCOMMAND_ACTIVE;
  }
  return error;
 }

 DWORD      __fastcall gr_cp::tutr_deactivate    (DWORD idx)
 {
 DWORD error = GRSCERR_TUTRCOMMAND_NOT_ACTIVE;
 /*Активизация команды ТУ*/
 if(idx<tu_list.size())
  {
   LPGRSC_TU_ENTRY entry = tu_list.at(idx);
   error = 0;
   entry->tu_time = GetTime(false);
   if(entry->tu_state&GRSC_TUTR_STATE_HOLDING)
    {
      DWORD cmd   = GRKTAFB_TRCANCEL;
      BYTE  buf[128];
      DWORD len = gr_proto_tu(this->number,cmd,entry->tu_group,entry->tu_object,buf,sizeof(buf));
      floor_tx(buf,len,true);
    }
   gr_group * grp = find_group(entry->addr);
   if(grp)
      {
      grp->set_personal_diag(entry->object,entry->object,OTD_PSTATE_TUTR_ACTIVE,false,true);
      if(entry->tu_state&GRSC_TUTR_STATE_ACTIVE)
        send_call(grp->get_call_afb(),true);
      }
   entry->tu_state&=~(GRSC_TUTR_STATE_ACTIVE|GRSC_TUTR_STATE_WAITING);
   modem_notify(MNCODE_SC_DATACHANGED,get_otd_addr(),0,idx,idx,GRSC_ACR_TUENTRY_CHANGED);

   }
 return error;
 }

 DWORD      __fastcall gr_cp::tutr_command (DWORD modem_addr,DWORD addr,DWORD obj,otd_tutr & cmd,LPGRSC_TU_ENTRY tu_entry)
 {
 DWORD error = GRSCERR_TUTROBJECT_NOT_SELECTED;
 BOOL  auto_select = FALSE;
 DWORD idx = tutr_find_entry(addr,obj);

 if(!(idx<tu_list.size()) && cmd.command!=OTD_TUTROP_CANCEL)
   {
    //При отправке команды ТУ/ТР Remote Control / Remote Adjust
    //Производится автовыбор
    error = tutr_select(modem_addr,addr,obj,tu_entry);
    if(error == ERROR_SUCCESS)
       {
        idx = tutr_find_entry(addr,obj);
        auto_select = TRUE;
       }
   }

 if(idx<tu_list.size())
 {
  LPGRSC_TU_ENTRY entry = tu_list.at(idx);
  if(modem_addr!=entry->owner_addr )
    error = GRSCERR_TUTROBJECT_LOCKED;
    else
    {
      if(cmd.command == OTD_TUTROP_CANCEL)
      {
       entry->tu_state&=~GRSC_TUTR_STATE_QUEUED|GRSC_TUTR_STATE_CMDON|GRSC_TUTR_STATE_CMDOFF;
       entry->tu_state|= GRSC_TUTR_STATE_CMDCANCEL;
       if(entry->tu_state&GRSC_TUTR_STATE_ACTIVE)
          error = tutr_deactivate(idx);
          else
          modem_notify(MNCODE_SC_DATACHANGED,get_otd_addr(),0,idx,idx,GRSC_ACR_TUENTRY_CHANGED);
      }
      else
      {
        if(!(entry->tu_state&GRSC_TUTR_STATE_ACTIVE_MASK))
        {
         entry->tu_state|= GRSC_TUTR_STATE_QUEUED;
         entry->tu_state|= ( cmd.command == OTD_TUOP_ON ? GRSC_TUTR_STATE_CMDON:GRSC_TUTR_STATE_CMDOFF);
         if(tutr_is_ready(entry->tu_module))
            error = tutr_activate(idx);
            else
            {
             entry->tu_state|= GRSC_TUTR_STATE_QUEUED;
             modem_notify(MNCODE_SC_DATACHANGED,get_otd_addr(),0,idx,idx,GRSC_ACR_TUENTRY_CHANGED);
             error = GRSCWRN_TUCOMMAND_IS_QUEUED;
            }

        }
        else error = GRSCERR_TUTROBJECT_LOCKED;
      }
      
     if(auto_select)
       tutr_deselect(modem_addr,addr,obj,tu_entry);
    }
 }
 return error;
 }


void       __fastcall gr_cp::tutr_timer       (__int64 sys_time)
{
 if(tu_list.size())
 {
 
 LPGRSC_TU_ENTRY entry;
 tu_entry_list::iterator beg = tu_list.begin(),end = tu_list.end(),ptr;
 ptr = beg;
 DWORD elapsed_time;

 while(ptr<end)
 {
  entry = *beg;
  elapsed_time = NS100_MSEC(abs(sys_time-entry->tu_time));
  if(entry->tu_state &GRSC_TUTR_STATE_ACTIVE_MASK)
  {
     if(entry == tu_active[entry->tu_module])
      {
       //Удержание контактов
       if(elapsed_time> entry->tu_hold_time)
          tutr_next(entry->tu_module);
      }
      if(entry->tu_state&GRSC_TUTR_STATE_WAITING)  //Ожидание завершения ТУ
        {
         if(elapsed_time>entry->tu_timeout)
          {
          entry->tu_state &=~GRSC_TUTR_STATE_HOLDING;
          tutr_deactivate(distance(beg,ptr));
          gr_group * grp = find_group(entry->addr);
          if(grp)
             grp->set_personal_diag(entry->object,entry->object,OTD_PDIAG_TUTR_FAIL,true,true);
          //Запись в лог о неуспешном ТУ
          otd_tutr _tc;DWORD error;
          if(entry->tu_state & GRSC_TUTR_STATE_CMDON )
             _tc.command = OTD_TUOP_ON,error = GRSCERR_TUON;
             else
             _tc.command = OTD_TUOP_OFF,error = GRSCERR_TUOFF;
          tutr_log(entry->owner_addr,entry->addr.addr,entry->object,_tc,entry,error);
          }
        }
        else
        {
          if(!(entry->tu_state &GRSC_TUTR_STATE_HOLDING))
              {
               tutr_deactivate(distance(beg,ptr));
              }
        }
  }
  ptr++;
 }
 tutr_autoreset(sys_time);
 }
}

void __fastcall gr_cp::tutr_autoreset(__int64 sys_time)
{
 LPGRSC_TU_ENTRY entry;
 tu_entry_list::iterator beg = tu_list.begin(),end = tu_list.end(),ptr;
 ptr = beg;
 DWORD elapsed_time;
 while(ptr < end)
 {
  entry = * ptr;
  elapsed_time = NS100_MSEC(sys_time-entry->tu_time);
  if(!(entry->tu_state&GRSC_TUTR_STATE_ACTIVE_MASK))
  {
   if (elapsed_time>=entry->tu_select_timeout || !(entry->tu_state & (GRSC_TUTR_STATE_SELECTED|GRSC_TUTR_STATE_QUEUED)))
    {
       if(entry->tu_state & GRSC_TUTR_STATE_SELECTED)
        {
         //Это автосброс
        otd_tutr tutr;
        tutr.command = OTD_TUTR_CMDDESELECT;
        tutr_log(entry->owner_addr,entry->addr.addr,entry->object,tutr,entry,GRSCWRN_TUTR_AUTORESET);
       }
       tutr_remove(distance(beg,ptr));
       ptr--;
       end = tu_list.end();
     }
   }  
  ptr++;
 }

}


void  __fastcall gr_cp::tutr_next(DWORD mod_num)
{
 //Выполнение следующей команды
 DWORD    idx = -1;
 __int64  queued_time;
 LPGRSC_TU_ENTRY entry = tu_active[mod_num];
 if(entry)
 {
  entry->tu_state&=~(GRSC_TUTR_STATE_HOLDING|GRSC_TUTR_STATE_ACTIVE);
  DWORD idx  = tutr_find_entry(entry->addr.addr,entry->object);
  modem_notify(MNCODE_SC_DATACHANGED,get_otd_addr(),0,idx,idx,GRSC_ACR_TUENTRY_CHANGED);
  tu_active[mod_num] = NULL;
  gr_group * grp = find_group(entry->addr);
  if(grp)
   send_call(grp->get_call_afb(),true);

 }
 //Поиск 
 for(DWORD  i = 0; i < tu_list.size() && tutr_is_ready(mod_num);i++)
 {
  entry = tu_list.at(i);
  if(!(entry->tu_state&GRSC_TUTR_STATE_ACTIVE_MASK) && entry->tu_module == mod_num && (entry->tu_state&GRSC_TUTR_STATE_QUEUED) )
  {
   if(idx>tu_list.size() || queued_time > entry->tu_time)
    {
     idx = i;
     queued_time = entry->tu_time;
    }
  }
 }
 if(idx<tu_list.size())
    {
     tutr_activate(idx);
    }
}


void  __fastcall gr_cp::tutr_check_success(gr_group * grp,DWORD obj)
{
  DWORD idx = tutr_find_entry(grp->get_otd_addr(),obj);
  if(idx<tu_list.size())
  {
   LPGRSC_TU_ENTRY   entry = tu_list.at(idx);
   entry->curr_value = grp->get_object_value(obj,(entry->tu_flags& GRSC_TUFL_TSINVERSE) ? true:false);
   if((entry->tu_state&GRSC_TUTR_STATE_WAITING) && entry->curr_value == entry->end_value )
    {
     grp->set_personal_diag(obj,obj,OTD_PDIAG_TUTR_FAIL,false,true);
     entry->tu_state&=~GRSC_TUTR_STATE_WAITING;//Снять ожидание
     tutr_deactivate(idx);
    }
  }
}



/******************************************************************************/



