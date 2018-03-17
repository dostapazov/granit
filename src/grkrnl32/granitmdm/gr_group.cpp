#include <stdio.h>
#include <algorithm>
#pragma hdrstop
#include "granit.hpp"
#include <otd_diag.h>

gr_group::gr_group(gr_cp * own):
owner(own),number(-1),flags(0),inf(-1),mod_number(-1),grp_data(NULL),
work_timer_value(60),nowork_timer_value(120),respond_timer_value(10),call_send_time(0)
,tm_counter(0),respond_time(0),
nchd_counter(0),
chdbc_counter(0),
respond_smooth(1),
norespond_counter(0)
{
}

gr_group::gr_group(gr_cp * own,DWORD num,DWORD _inf,DWORD _mod_num):
owner(own),number(num),flags(0),inf(_inf),mod_number(_mod_num),grp_data(NULL),
work_timer_value(60),nowork_timer_value(120),respond_timer_value(10),call_send_time(0),
tm_counter(0),respond_time(0),
nchd_counter(0),
chdbc_counter(0),
respond_smooth(1),
norespond_counter(0)
{
 create();
}

gr_group::gr_group(gr_cp * own,LPGR_GRP_PROPS prp):
owner(own),flags(prp->flags),mod_number(prp->mod_number),grp_data(NULL),
work_timer_value(prp->work_timer),nowork_timer_value(prp->nowork_timer),
respond_timer_value(prp->respond_timer),call_send_time(0),
tm_counter(0),respond_time(0),
nchd_counter(0),
chdbc_counter(0),
respond_smooth(1),
norespond_counter(0)
{
 sotd_addr sa(prp->addr);
 number  = sa.sb;
 switch(sa.fa)
 {
  case OTD_FA_DISCRETE       : inf = GRINF_TS;break;
  case OTD_FA_ANALOG         : inf = GRINF_TIT;break;
  case OTD_FA_ANALOG_INTEGR  : inf = GRINF_TII;break;
 }
 create();
}


bool __fastcall gr_group::create()
{
 if(grp_data)
    delete [] (LPBYTE)grp_data;
 grp_data = NULL;
 if(this->number <255 && this->inf!=(DWORD)-1 && this->mod_number!=(DWORD)-1)
 {
 otd_proto_param opp;
 ZeroMemory(&opp,sizeof(opp));
 opp.dw_size    = sizeof(opp);
 opp.parts      = OTD_PROTO_PART_DATA|
                  OTD_PROTO_PART_DIAG|
                  OTD_PROTO_PART_TIMESTAMP|
                  OTD_PROTO_PART_PERSONAL_DIAG|
                  OTD_PROTO_PART_PERSONAL_CHMASK|
                  OTD_PROTO_PART_KPK;
 opp.dt_param.count =
 opp.pd_param.count =
 opp.cm_param.count = inf == GRINF_TS ? 64:16;
 opp.dt_param.type  = inf == GRINF_TS ? OTD_DISCRETE:OTD_ANALOG_BYTE;
 opp.pd_param.type  = opp.cm_param.type  = OTD_ANALOG_BYTE;
 DWORD  need_size   = otd_proto_calc_size_ex(&opp,0);
 LPBYTE ptr = new BYTE[need_size+sizeof(*grp_data)];
 ZeroMemory(ptr,sizeof(*grp_data));
 grp_data = (lpotd_proto) ptr;
 ptr+=sizeof(*grp_data);
 grp_data->dw_size = sizeof(*grp_data);
 otd_proto_format_ex(ptr,need_size,&opp,0,grp_data);
 *grp_data->diag = (owner->get_diag()&OTD_DIAG_MASK)|OTD_DIAG_MODRESPOND|OTD_DIAG_NODATA|OTD_PART_DIAG_PARAM;
  grp_data->addr->addr = owner->get_otd_addr();
  grp_data->addr->sb   = number;
   switch(inf)
   {
    case GRINF_TS  : grp_data->addr->fa= OTD_FA_DISCRETE      ;break;
    case GRINF_TIT : grp_data->addr->fa= OTD_FA_ANALOG        ;break;
    case GRINF_TII : grp_data->addr->fa= OTD_FA_ANALOG_INTEGR ;break;
   }
   otd_proto_protect(grp_data,grp_data->proto_size,OTD_DEF_POLINOM);
   call_send_time = 0;
  }
  return grp_data ? true:false;
}


gr_group::~gr_group()
{
 release();
}

void    __fastcall gr_group::release()
{
 tu_entry_list::iterator beg = tu_list.begin(),end = tu_list.end();
 while(beg<end)
 {
  if(*beg)
    delete *beg;
  beg++;
 }
 tu_list.resize(0);
 if(grp_data)
    delete [] (LPBYTE)grp_data;
}





DWORD     __fastcall gr_group::get_otd_addr()
{

 sotd_addr addr;
 if(grp_data)
   addr.addr = grp_data->addr->addr;
 else
 {
   addr.addr = (owner->get_otd_addr());
   switch(inf)
   {
    case GRINF_TS  : addr.fa   = OTD_FA_DISCRETE     ;break;
    case GRINF_TIT : addr.fa   = OTD_FA_ANALOG       ;break;
    case GRINF_TII : addr.fa   = OTD_FA_ANALOG_INTEGR ;break;
   }
   addr.sb   = number;
 }
 return addr.addr;
}

DWORD     __fastcall gr_group::get_call_afb()
{
 DWORD ret = GRKTAFB_CALLTS;
 switch(inf)
 {
  case GRINF_TIT : ret = GRKTAFB_CALLTIT  ;break;
  case GRINF_TII : ret = GRKTAFB_CALLTII  ;break;
 }
 return ret;
}

  bool      __fastcall gr_group::call_sended(__int64 send_time)
  {
   if(call_send_time)
      return false;
   call_send_time = send_time;
   tm_counter = respond_timer_value * 1000;
   return true;
  };




 DWORD     __fastcall gr_group::get_props(LPGR_GRP_PROPS props)
 {
   if(owner->get_owner()->get_owner()->check_data_size(props,sizeof(*props)))
   {
    props->addr.addr   = get_otd_addr();
    props->mod_number  = mod_number;
    props->flags       = flags;
    props->data_size   = grp_data->proto_size;
    props->param_count = OTD_GET_COUNT(&grp_data->data->numbers);
    props->tutr_count  = 0;//todo tutr_list.size();
    props->work_timer     = this->work_timer_value;
    props->nowork_timer   = this->nowork_timer_value;
    props->respond_timer  = this->respond_timer_value;
    props->respond_smooth = this->respond_smooth;
    return GKH_RET_SUCCESS;
   }
   return GKH_RET_ERROR;
 }

 DWORD     __fastcall gr_group::set_props(LPGR_GRP_PROPS  prp)
 {
   DWORD chm = 0;
   if((prp->mask & GRSC_FPRP_FLAGS) && prp->flags!= flags)
    {
     flags = prp->flags;
     chm  |= GRSC_FPRP_FLAGS;
    }

   if(prp->mask & GRSC_FPRP_TIMERS)
   {
    if(work_timer_value!=prp->work_timer)
       work_timer_value =prp->work_timer,chm |=GRSC_FPRP_TIMERS;

    if(nowork_timer_value!=prp->nowork_timer)
       nowork_timer_value =prp->nowork_timer,chm |=GRSC_FPRP_TIMERS;
    if(respond_timer_value!=prp->respond_timer)
       respond_timer_value =prp->respond_timer,chm |=GRSC_FPRP_TIMERS;

    if(call_send_time)
     tm_counter = KeRTL::MIN(respond_timer_value,tm_counter);
     else
     tm_counter = KeRTL::MIN(((get_diag()&OTD_DIAG_MASK) ? nowork_timer_value:work_timer_value),tm_counter/1000);
     tm_counter*=1000;//К миллисекундам

   }
   if(prp->mask& GRSC_FPRP_MODULE)
    {
     if(mod_number != prp->mod_number)
        {
         mod_number  = prp->mod_number;chm |= GRSC_FPRP_MODULE;
         set_diag(OTD_DIAG_NODATA|OTD_DIAG_MODRESPOND,true);
         call_send_time = 0;
         owner->send_call(get_call_afb());
         chm|= GRSC_FPRP_MODULE;
        }
    }
    if((prp->mask & GRSC_FPRP_RESPOND_SMOOTH) && respond_smooth != prp->respond_smooth)
       {
        respond_smooth = prp->respond_smooth;
       }

   if(chm)
     {
      owner->get_owner()->config_change();
      modem_notify(MNCODE_SC_CHANGED,get_otd_addr(),chm);
     }

   return GKH_RET_SUCCESS;
 }

  void      __fastcall gr_group::set_cp_number(DWORD num)
  {
   if(grp_data)
    grp_data->addr->cp = num;
  }


  DWORD      __fastcall gr_group::scan_diag ()
  {
   DWORD ret = OTD_DIAG_NODATA;
   if(grp_data && grp_data->diag)
     {
      ret = (*grp_data->diag);
      if(grp_data->personal_diag)
      {
       ret &=OTD_DIAG_MASK;
       DWORD start = (DWORD)grp_data->personal_diag->numbers.loN,end = (DWORD)grp_data->personal_diag->numbers.hiN;
       while(start<=end)
       {
        DWORD pd_value = 0;
        otd_get_value(grp_data->personal_diag,start,&pd_value,sizeof(pd_value));
        if(pd_value&OTD_PDIAG_MASK)
           ret|= OTD_PART_DIAG_PARAM;       // Недостовеоность параметра
        if(pd_value&OTD_PDIAG_TUTR_FAIL)
           ret|=OTD_DIAG_STATE_TUTR_ERROR;       //Неуспешное ТУ/ТР
           ret|=(pd_value&OTD_PSTATE_TUTR_MASK);
        start++;
       }
      }
     }
   return ret;
  }

  DWORD     __fastcall gr_group::get_diag(bool rescan )
  {
   DWORD ret;
   if(rescan)
      scan_diag();
   ret = grp_data ? *grp_data->diag : OTD_DIAG_NODATA;
   return ret;
  }

  bool       __fastcall gr_group::check_diag(DWORD df,bool all )
  {
    bool   ret = false;
    DWORD  diag = get_diag()&df;
    if(diag)
    {
      ret = all ? (diag==df):true;
    }
    return ret;
  }

  DWORD      __fastcall gr_group::set_diag(DWORD dfl,bool set,bool send_changes)
  {
    otd_diag _dv;
  
    _dv.diag = get_diag();
    DWORD changes = 0;
    if(grp_data)
    {
     if(set && (dfl&(OTD_DIAG_MASK)))
         dfl|= OTD_DIAG_MODRESPOND|OTD_PART_DIAG_PARAM;
     DWORD diag =  (*grp_data->diag) &(~OTD_DIAG_NODATA);
     if(set) diag|=dfl; else diag&=~dfl;
     changes = (*grp_data->diag)^diag;
     if(changes)
     {
       (*grp_data->diag)=diag;
       _dv.diag = get_diag();

       if(_dv.part_param && !_dv.mod_respond)
           _dv.diag = get_diag();

       if(set && (changes&OTD_PART_DIAG_PARAM))
           {
            call_send_time = 0;//Снять флаг об отправке вызова
            DWORD pdv  = OTD_PDIAG_NODATA;
            set_personal_diag(0,-1,pdv,true,false,0);
            if(!(diag&(OTD_DIAG_PUCONNECT|OTD_DIAG_PURESPOND|OTD_DIAG_CPCONNECT|OTD_DIAG_CPRESPOND))
               && (changes&(OTD_DIAG_MODRESPOND|OTD_DIAG_STATE_MASK|OTD_DIAG_PART_MASK))) // Остальную диагностику отправляет КП, ПУ
               {
                 log_diag(); //Запись о неисправности только при исправном КП
                if(send_changes)
                 modem_send_changes(OTD_PROTO_PART_DIAG|OTD_PROTO_PART_KPK,0,-1);
               }
           }
      modem_notify(MNCODE_SC_DATACHANGED,grp_data->addr->addr,OTD_PROTO_PART_DIAG|OTD_PROTO_PART_PERSONAL_DIAG);
     }
    }
    _dv.diag = get_diag();
    return changes;
  }

  bool       __fastcall gr_group::on_timer(__int64 /*sys_time*/)
  {
    /*Возврат true при изменении готовности т.е группа не ответила*/
    bool ret = false;
    if(grp_data)
    {
      if(!((*grp_data->diag)&OTD_DIAG_PUCONNECT) || call_send_time)
      {
      tm_counter-=GRSC_TIMER_RESOLUTION;
      if(int(tm_counter)<=0)
      {
        if(call_send_time)
           {
            //Был отправлен вызов и нет ответа
            call_send_time = 0;

            if(++norespond_counter > respond_smooth ) //Устанавливаем неработоспособность группы
              {
               ret = set_diag(OTD_DIAG_MODRESPOND,true);
               tm_counter = nowork_timer_value*1000;
              }
              else
              ret = true;

           }
           else
           owner->send_call(get_call_afb());//Отправить вызов
      }
      }
    }
    return ret;
  }

  DWORD      __fastcall gr_group::handle  (gr_proto & gp,__int64 h_time)
  {
   //Обработка
   //TODO  cброс таймера ответа, если вызов;
   //TODO  сброс таймера работоспособности/ обновления
   DWORD  changes        = 0;
   DWORD  loch = -1,hich = 0;

   if(grp_data)
   {
   DWORD  cur_param = 0;
   DWORD  cur_value,old_value,pd,pcm,new_pd;
   DWORD  param_count = OTD_GET_COUNT(&grp_data->data->numbers);
   DWORD  new_diag = (*grp_data->diag)&~OTD_DIAG_FULL_MASK;
   norespond_counter = 0;

    while(cur_param<param_count)
    {
     cur_value= old_value = pd = pcm = 0;
     cur_value = grp_data->addr->fa ?  gr_get_ti(gp.data,cur_param):gr_get_ts(gp.data,cur_param);
     otd_get_value(grp_data->data,cur_param,&old_value,sizeof(old_value));
     otd_get_value(grp_data->personal_diag,cur_param,&pd,sizeof(pd));

     if(cur_value!= old_value) //Установка нового значения
         {
          otd_set_value(grp_data->data,cur_param,&cur_value,sizeof(cur_value));
          changes|=OTD_PROTO_PART_DATA|OTD_PROTO_PART_DIAG|OTD_PROTO_PART_PERSONAL_CHMASK;
          pcm++;
         }

     new_pd  = pd;
     new_pd &=~ OTD_PDIAG_NODATA;

     if(cur_value>250) // Выход за границы шкалы
        {
         new_pd  |=OTD_PDIAG_OUTSCALE;
         new_diag|=OTD_PART_DIAG_PARAM;
        } 
        else
        new_pd&=~(OTD_PDIAG_OUTSCALE|OTD_PDIAG_ERROR);


     if(new_pd!=pd)
       {
        pcm+=2;
        changes  |=OTD_PROTO_PART_PERSONAL_DIAG;
        otd_set_value(grp_data->personal_diag,cur_param,&new_pd,sizeof(new_pd));
       }
     otd_set_value(grp_data->personal_chmask,cur_param,&pcm,sizeof(pcm));
     if(pcm)
     {
      loch =  loch == (DWORD)-1 ? cur_param:loch;
      hich =  cur_param;
     }
     cur_param++;
    }


     if(call_send_time)
     {
      //Был отправлен вызов
      respond_time = NS100_MSEC(h_time-call_send_time);
      call_send_time = 0;
      if(changes&OTD_PROTO_PART_DATA)
      {
        if((++chdbc_counter > 3) && (grp_data->addr->fa == OTD_FA_DISCRETE) )  //Изменения присутствуют значит модуль не выходит на передачу по изменениям
            new_diag|=OTD_DIAG_MODTXCHANGES;
      }
       else
      {
        chdbc_counter = 0; //Изменений нет всё ок
        new_diag&=~OTD_DIAG_MODTXCHANGES;
      }
     }
     else
     {
      /*Данные пришли без вызова т.е по инициативе КП*/
      /*Здесь ведется проверка блока на квитируемость*/
      if(!(changes&OTD_PROTO_PART_DATA))
      {
       if(++nchd_counter>3)  //Не было изменений
          new_diag|=OTD_DIAG_STATE_NOTRECEIPT;  //Модуль не квитируется
      }
       else
      {
       nchd_counter  = 0;
       chdbc_counter = 0;
       new_diag&=~(OTD_DIAG_STATE_NOTRECEIPT|OTD_DIAG_MODTXCHANGES); //Изменения присутствуют => блок был сквитирован
      }

   }

    if(new_diag!=(*grp_data->diag))
     {
     *grp_data->diag = new_diag;
      changes |= OTD_PROTO_PART_DIAG;
     }

     tm_counter = (new_diag&OTD_DIAG_FULL_MASK) ? nowork_timer_value:work_timer_value;
     tm_counter*=1000;
     }


   if(changes)//Уведомление об изменении
     {
      (*(__int64*)grp_data->time_stamp)   = h_time;
      changes|= OTD_PROTO_PART_TIMESTAMP;
      otd_proto_protect(grp_data,grp_data->proto_size,OTD_DEF_POLINOM);
      //DWORD notify_parts = OTD_PROTO_PART_DIAG|OTD_PROTO_PART_DATA|(changes/*&~OTD_PROTO_PART_PERSONAL_CHMASK*/);
      BYTE buffer[2048];
      int len =
      get_otd_data(OTD_PROTO_PART_DATA|OTD_PROTO_PART_DIAG|(changes&~OTD_PROTO_PART_PERSONAL_CHMASK),0,-1,buffer,sizeof(buffer),NULL);
      if(len>0)
        queue_rxdata(buffer,(DWORD)len,false);
      modem_notify (MNCODE_SC_DATACHANGED,grp_data->addr->addr,changes,loch,hich);
     }
   return changes;
  }

  DWORD        __fastcall gr_group::get_dynamic (LPVOID dyn_data)
  {
   DWORD ret = GKH_RET_ERROR;
   if(this->owner->get_owner()->get_owner()->check_data_size(dyn_data,sizeof(SC_DYNAMIC_GRP)))
   {
    LPSC_DYNAMIC_GRP grp_dyn = (LPSC_DYNAMIC_GRP)dyn_data;
    grp_dyn->call_sended  = call_send_time ? TRUE:FALSE;
    grp_dyn->respond_time = respond_time;
    grp_dyn->tm_counter   = tm_counter/1000;
    grp_dyn->chdbc_counter = chdbc_counter;
    grp_dyn->nchd_counter  = nchd_counter;
    grp_dyn->norespond_counter = norespond_counter;
    ret = GKH_RET_SUCCESS;
   }
   return ret;

  }

  DWORD     __fastcall gr_group::get_object_value(DWORD obj,bool inverce)
  {
   DWORD ret = 0;
   if(grp_data)
      {
       otd_get_value(grp_data->data,obj,&ret,sizeof(ret));
       if(inverce && grp_data->addr->fa == OTD_FA_DISCRETE)
         ret = !ret;
      } 
      
   return ret;
  }

  DWORD     __fastcall gr_group::get_otd_data(LPSC_OTDDATA scod)
  {
    scod->data_len = get_otd_data(scod->parts,scod->lo_num,scod->hi_num,scod->data,scod->data_size,NULL);
    return ((int)scod->data_len)>0 ? GKH_RET_SUCCESS:GKH_RET_ERROR;
  }

  DWORD     __fastcall gr_group::get_otd_data(DWORD parts,DWORD lo_num,DWORD hi_num,LPBYTE buf,DWORD bsz,lpotd_proto op)
  {
   DWORD ret ;
   otd_proto_param opp;
   opp.dw_size         = sizeof(opp);
   opp.parts           = (parts & (*grp_data->ver_parts))|OTD_PROTO_PART_KPK;
   opp.parts          |=  parts&OTD_PROTO_FLAGS_MASK;
   opp.dt_param.type   = grp_data->data->otd_type;
   
   opp.pd_param.first  =
   opp.cm_param.first  =
   opp.dt_param.first  = KeRTL::MAX((int)lo_num,(int)grp_data->data->numbers.loN);

   opp.pd_param.count  =
   opp.cm_param.count  =
   opp.dt_param.count  = KeRTL::MIN((DWORD)OTD_GET_COUNT(&grp_data->data->numbers),(DWORD)(1+hi_num-lo_num));

   opp.pd_param.type   = grp_data->personal_diag->otd_type;
   opp.cm_param.type   = grp_data->personal_chmask->otd_type;
   int   need_size     = otd_proto_calc_size_ex(&opp,0);
   if((DWORD)need_size<= bsz)
   {
    ret = otd_proto_getparts_ex(buf,bsz,&opp,grp_data,0);
    if(ret && op)
      otd_proto_parser(buf,ret,op,NULL);

   }
   else
    ret = (DWORD)(-need_size);
   return ret;
  }

    void      __fastcall gr_group::write(TStream & strm)
    {
     strm<<number<<flags<<inf<<mod_number<<work_timer_value<<nowork_timer_value<<respond_timer_value<<respond_smooth;
     int tue_count = (int)tu_list.size();
     strm<<tue_count ;
     tu_entry_list::iterator beg = tu_list.begin(), end = tu_list.end();
     while(beg<end)
     {
      LPGRSC_TU_ENTRY entry = *beg;
      strm<<entry->addr.addr<<entry->object<<entry->tu_module
           <<entry->tu_group<<entry->tu_object<<entry->cmd_on<<entry->cmd_off
           <<entry->tu_flags
           <<entry->tu_hold_time<<entry->tu_timeout<<entry->tu_select_timeout;
      beg++;
     }
    }

    void      __fastcall gr_group::read (TStream & strm,DWORD ver)
    {
     strm>>number>>flags>>inf>>mod_number>>work_timer_value>>nowork_timer_value>>respond_timer_value;
     if(ver>0)
       strm>>respond_smooth;
     create();
      //Чтение списка ТУ
      int tue_count ;
      strm>>tue_count;
      while(tue_count--)
      {
       LPGRSC_TU_ENTRY entry = new GRSC_TU_ENTRY;
       ZeroMemory(entry,sizeof(*entry));
       entry->dw_size = sizeof(*entry);
       strm>>entry->addr.addr>>entry->object>>entry->tu_module
           >>entry->tu_group>>entry->tu_object>>entry->cmd_on>>entry->cmd_off
           >>entry->tu_flags
           >>entry->tu_hold_time>>entry->tu_timeout>>entry->tu_select_timeout;
       tu_list.push_back(entry);

       }
    }


 void         __fastcall gr_group::log_diag()
 {
  wchar_t  log_text[MAX_PATH];
  wchar_t  log_templ[MAX_PATH];
  LoadStringW(ModuleInstance,IDS_SC_LOG2,log_templ,sizeof(log_templ)/sizeof(wchar_t));
  sotd_addr addr(get_otd_addr());
  swprintf(log_text,log_templ,(DWORD)addr.cp,(DWORD)addr.sb,(DWORD)addr.fa ? L"ТИТ":L"ТС",grp_data ? *grp_data->diag :0);
  event_report(REPORT_ERROR_TYPE,log_text,NULL,0);
 }

  tu_entry_list::iterator __fastcall gr_group::lower_bound(DWORD obj)
  {
   GRSC_TU_ENTRY tue;
   tue.dw_size   = sizeof(tue);
   tue.addr.addr = 0;
   tue.object    = obj;
   tu_entry_list::iterator ret = std::lower_bound(tu_list.begin(),tu_list.end(),&tue,tu_entry_comparer());
   return ret;
  }

  LPGRSC_TU_ENTRY         __fastcall gr_group::find_tu_entry(DWORD obj)
  {
    tu_entry_list::iterator ptr = lower_bound(obj);
    if(ptr<tu_list.end() && (*ptr)->object == obj)
      return *ptr;
    return NULL;
  }

  BOOL             __fastcall gr_group::get_tu_entry(DWORD obj,LPGRSC_TU_ENTRY entry)
  {
   if(entry && entry->dw_size == sizeof(*entry))
   {
    LPGRSC_TU_ENTRY ptr = find_tu_entry(obj);
    if(ptr)
      {
       memcpy(entry,ptr,sizeof(*entry));
       entry->dw_size      = sizeof(*entry);
       entry->addr.addr    = this->grp_data->addr->addr;
       return TRUE;
      }
   }
   return FALSE;
  }

  LRESULT          __fastcall gr_group::get_tu_entry(LPGRSC_TU_ENTRY tue)
  {
   return get_tu_entry(tue->object,tue) ? GKH_RET_SUCCESS:GKH_RET_ERROR;
  }

  bool        __fastcall gr_group::check_tu_duplicate(LPGRSC_TU_ENTRY tue)
  {
    bool ret = false;
    tu_entry_list::iterator b = tu_list.begin(),e = tu_list.end();
    while(!ret && (b<e))
    {
     LPGRSC_TU_ENTRY entry = *b;
     if(entry->tu_module == tue->tu_module &&
        entry->tu_group == tue->tu_group   &&
        entry->tu_object == tue->tu_object 
        )
        ret = true;
     ++b;
    }
    return ret;
  }
  
  LRESULT __fastcall gr_group::add_tu_entry(LPGRSC_TU_ENTRY tue)
  {
   //Добавление описателя ТУ
   LRESULT ret = GKH_RET_ERROR;
   LPGRSC_TU_ENTRY entry = find_tu_entry(tue->object);
   if(!entry)
     {
       if(tue->object < (DWORD)grp_data->data->numbers.hiN)
        {
          if(tue->tu_module>0 && tue->tu_module<GRCP_MAX_MODCOUNT)
          {
           /*Проверка дублирования записи ТУ */
           //if(!check_tu_duplicate(tue))
           {
           entry =  tue_init(new GRSC_TU_ENTRY,tue);
           entry->addr.addr = 0;
           tu_entry_list::iterator ptr = tu_list.insert(lower_bound(tue->object),entry);
           ret = distance(tu_list.begin(),ptr);
           modem_notify(MNCODE_SC_CHANGED,get_otd_addr(),0,ret,ret,GRSC_ACR_TUENTRY_ADDED);
           set_personal_diag(entry->object,entry->object,OTD_PDIAG_TUTR_DESCRIPT,false,true);
           }
           /*else
           SetLastError(GRSCERR_TU_EXIST);*/
          }
          else
          SetLastError(GRSCERR_INVALID_TUMODULE_NUMBER);
        }
        else
        SetLastError(GRSCERR_OBJECT_NOT_EXIST);
     }
     else
     {
     //if(!check_tu_duplicate(tue))
      {
       memcpy(entry,tue,sizeof(*entry));
       entry->addr.addr = 0;
       ret = distance(tu_list.begin(),lower_bound(entry->object));
       modem_notify(MNCODE_SC_CHANGED,get_otd_addr(),0,ret,ret,GRSC_ACR_TUENTRY_CHANGED);
      }
     //else
      //SetLastError(GRSCERR_TU_EXIST);
     }

   return ret;
  }

  LRESULT __fastcall gr_group::set_tu_entry(LPGRSC_TU_ENTRY tue)
  {
   return add_tu_entry(tue);
  }

  LRESULT __fastcall gr_group::del_tu_entry(LPGRSC_TU_ENTRY tue)
  {
   LRESULT ret = GKH_RET_ERROR;
   LPGRSC_TU_ENTRY entry = find_tu_entry(tue->object);
   if(entry)
   {
    tu_entry_list::iterator  ptr = lower_bound(tue->object);
    DWORD idx = distance(tu_list.begin(),ptr);
    tu_list.erase(ptr);
    delete entry;
    modem_notify(MNCODE_SC_CHANGED,get_otd_addr(),0,idx,idx,GRSC_ACR_TUENTRY_REMOVED);
    ret = GKH_RET_SUCCESS;
   }
   SetLastError(GRSCERR_OBJECT_NOT_EXIST);
   return ret;
  }

  LRESULT __fastcall gr_group::enum_tu_entry(LPGRSC_TU_ENTRY tue,DWORD idx)
  {
   LRESULT ret = GKH_RET_ERROR;
   if(idx< tu_list.size())
      {
       tu_entry_list::iterator ptr = tu_list.begin();
       advance(ptr,idx);
       memcpy(tue,*ptr,sizeof(*tue));
       tue->addr.addr = grp_data->addr->addr;
       ret = GKH_RET_SUCCESS;
      }
      else
      SetLastError(GRSCERR_OBJECT_NOT_EXIST);
   return ret;
  }

void      __fastcall gr_group::modem_send_changes(DWORD parts,DWORD low,DWORD hi,DWORD modem_addr)
{
  BYTE buffer[4096];
  otd_proto    op;
  op.dw_size = sizeof(op);
  int len =
  get_otd_data(parts|OTD_PROTO_PART_KPK,low,hi,buffer,sizeof(buffer),&op);
  if(len>0)
    {
     otd_proto_protect(&op,op.proto_size,OTD_DEF_POLINOM);
     queue_rxdata(buffer,(DWORD)len,false,modem_addr);
    }
}


DWORD   __fastcall gr_group::set_personal_diag(DWORD lo_obj,DWORD hi_obj,DWORD pd,bool set,bool send,DWORD modem_addr)
{
 //Возвращается маска изменеий
 //TODO Установка персональных состояний объектов
 DWORD ret = 0;
 if(this->grp_data && grp_data->personal_diag)
 {

  lo_obj = KeRTL::MAX(lo_obj,(DWORD)grp_data->personal_diag->numbers.loN);
  hi_obj = KeRTL::MIN(hi_obj,(DWORD)grp_data->personal_diag->numbers.hiN);
  DWORD lo_changes = hi_obj;
  DWORD hi_changes = lo_obj;
  while(lo_obj<=hi_obj)
  {
  DWORD old_value = 0;
  DWORD new_value;

  if(!otd_get_value(grp_data->personal_diag,lo_obj,&old_value,sizeof(old_value)))
  {
   new_value = set ? (old_value|pd) :(old_value&~pd);
   /*if(set)
      old_value&=~OTD_PDIAG_TUTR_DESCRIPT;*/
   if(new_value^old_value)
     {
      otd_set_value(grp_data->personal_diag,lo_obj,&new_value,sizeof(new_value));
      ret|= OTD_PROTO_PART_PERSONAL_DIAG|OTD_PROTO_PART_DIAG;
      lo_changes = KeRTL::MIN(lo_obj,lo_changes);
      hi_changes = KeRTL::MAX(hi_obj,hi_changes);
     }
  }
   lo_obj++;
  }
  *grp_data->diag = scan_diag();
  if(ret)
  {
   if(send )
      modem_send_changes(ret,lo_changes,hi_changes,modem_addr);
   modem_notify(MNCODE_SC_DATACHANGED,grp_data->addr->addr,ret,lo_changes,hi_changes);
   owner->update_diag();
  }
 }
 return ret;
}

  BOOL    __fastcall gr_group::get_first_change(LPDWORD chnum)
  {

   if(grp_data)
    {
     DWORD i = grp_data->data->numbers.loN;
     for(;i<(DWORD)grp_data->data->numbers.hiN;i++)
      {
       DWORD pc = 0;
       otd_get_value(grp_data->personal_chmask,i,&pc,sizeof(pc));
       if(pc&1)
         {
          if(chnum) *chnum = i;
          return TRUE;
         }
      }
    }
   return FALSE;
  }

  BOOL    __fastcall gr_group::get_next_change (DWORD i,LPDWORD chnum)
  {
   if(grp_data)
   {
     for(++i;i<(DWORD)grp_data->data->numbers.hiN;i++)
      {
       DWORD pc = 0;
       otd_get_value(grp_data->personal_chmask,i,&pc,sizeof(pc));
       if(pc&1)
         {
          if(chnum) *chnum = i;
          return TRUE;
         }
      }
    }
   return FALSE;
  }


void    __fastcall gr_group::reset_tu_error(DWORD lo,DWORD hi)
{
 static int _dbg = 0;
 if(grp_data && grp_data->personal_diag)
  {
   _dbg++;
   set_personal_diag(lo,hi,OTD_PDIAG_TUTR_FAIL,(_dbg&1)? true:false,true);
  }
}









