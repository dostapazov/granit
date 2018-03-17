#pragma hdrstop
#include "mediumdb.hpp"
#include <otd_diag.h>
#include <list>
#include <scale.h>
#include <stdio.h>
#pragma warn -8017
#include <values.h>
#pragma warn .8017
#include <limits>



 DWORD      __fastcall med_db::handle(otd_proto & op)
 {
   //Обработчик данных
   DWORD ret = 0;
   if(op.addr)
   {
   DWORD addr_type = otd_addr_type(op.addr);
   if(addr_type == OTD_ADDR_TYPE_SB)
      ret = handle_sb(op);
      else
      ret = handle_group(op);

   }
   else
   {
    wchar_t rep_text[MAX_PATH];
    snwprintf(rep_text,KERTL_ARRAY_COUNT(rep_text),L"Заголовок ОТД без адреса");
    module->mdb_report(REPORT_ERROR_TYPE,rep_text);
   }

   return ret;
 }

 DWORD        __fastcall med_db::handle_sb     (otd_proto & op, bool sec_pass)
 {
   /*
    Обработка от сублока
   */
   DWORD ret = 0;
   DWORD lo_param = -1;
   DWORD hi_param = 0;
   if(op.personal_chmask)
   {
     DWORD locm = op.personal_chmask->numbers.loN;
     DWORD hicm = op.personal_chmask->numbers.hiN;
     DWORD cm = 0;
     while(locm<=hicm)
     {
       otd_get_value(op.personal_chmask,locm,&cm,sizeof(cm));
       if(cm)
       {
        if(lo_param == (DWORD)-1)
           lo_param = locm;
        hi_param = locm;
       }
       ++locm;
     }
   }
     else
     {
       if(op.data)
          {
           lo_param = KeRTL::MIN(lo_param,(DWORD)op.data->numbers.loN);
           hi_param = KeRTL::MAX(hi_param,(DWORD)op.data->numbers.hiN);
          }

       if(op.personal_diag)
          {
           lo_param = KeRTL::MIN(lo_param,(DWORD)op.personal_diag->numbers.loN);
           hi_param = KeRTL::MAX(hi_param,(DWORD)op.personal_diag->numbers.hiN);
          }

      }

   if(lo_param<=hi_param)
   {
    __int64 time = op.time_stamp ? *(__int64*)op.time_stamp : GetTime();
    sotd_param_addr lpa(*op.addr,lo_param);
    sotd_param_addr hpa(*op.addr,hi_param);

     #if( FASTMUTEX_LOCKER != 0)
      TLockHelper l (locker);
     #else
       wr_lock    l (locker);
     #endif

     trecords_index::iterator lo_range,hi_range;
     if(lpa.addr.cp == 2 && lpa.addr.fa == 0 && lpa.addr.sb == 0)
         time = time+0;
     if(records.get_addr_range(lpa,hpa,lo_range,hi_range))
       {
        while(lo_range<hi_range)
        {
         mdb_record & rec = records.at(*lo_range);
         if(!rec.is_combined() && !rec.is_pseudo())
            handle_record(rec,op,time);
         ++lo_range;
        }
       }
       else
       {

        if(!sec_pass && op.data && addr_mask.addr && otd_addr_include(op.addr,&addr_mask))
        {

          DWORD beg = op.data->numbers.loN;
          DWORD end = op.data->numbers.hiN+1;
          while(beg<end)
          {
           sotd_param_addr pa(*op.addr,beg);
           op.data;
           DWORD kvants = 250;
           if(op.data->otd_type == OTD_DISCRETE)
              kvants = 1;
           if((op.data->otd_type&OTD_TYPE_MASK) == OTD_BYTE_TYPE )
              {
                for(int i = 0 ; i < 4 && i<((op.data->otd_type&OTD_TYPE_LEN)-1);i++)
                   kvants = 0xFF|(0xFF<<(8));
                kvants &=0xFFFFFF00;
                kvants+=250;
              }

           mdb_record rec(pa,-1,0,kvants,float(kvants),float(.0));
           create(&rec,0,true);
           ++beg;

          }
          this->addr_mask_timeout = 1000;
          ret = handle_sb(op,true);
        }
       }
   }
   return ret;
 }

 DWORD        __fastcall med_db::handle_group  (otd_proto & op)
 {
   DWORD ret = 0;
   //Обработка групповых объектов
   sotd_param_addr low_addr(*op.addr,0);
   sotd_param_addr hi_addr (*op.addr,-1);
   DWORD offset = records.find_by_addr(hi_addr);
   if(offset<records.size())
   {
    mdb_record & rec  = records.at(offset);
    if(op.name)
      {
       wchar_t text[4096];
       text[otd_text_getwstr(op.name,text,sizeof(text)/sizeof(wchar_t))]=0;
       if(*text)
          {

              set_name(rec.rec_id,text,false);
           if(rec.name == (DWORD)-1)
              set_name(rec.rec_id,text,true);
            }
      }

   }

   if(op.diag && ((*op.diag)&OTD_DIAG_MASK))
   {

   if( low_addr.addr.sb == OTD_ADDR_MAXVALUE) low_addr.addr.sb = 0;
   if((low_addr.addr.fa&OTD_FA_ALL) == OTD_FA_ALL) {low_addr.addr.fa = 0;hi_addr.addr.fa = OTD_ADDR_MAXVALUE;}
   if( low_addr.addr.cp == OTD_ADDR_MAXVALUE)  low_addr.addr.cp = 0;
   if( low_addr.addr.pu == OTD_ADDR_MAXVALUE)  low_addr.addr.pu = 0;

   trecords_index::iterator lo_range,hi_range;
     #if( FASTMUTEX_LOCKER != 0)
      TLockHelper l (locker);
     #else
       wr_lock    l (locker);
     #endif
    if(records.get_addr_range(low_addr,hi_addr,lo_range,hi_range))
      {
       __int64 time = op.time_stamp ? *(__int64*)op.time_stamp : GetTime();
       while(lo_range<hi_range)
       {
         offset = *lo_range;
         mdb_record & rec = records.at(offset);
         if(hi_addr.is_include(rec.addr))
              {
               if(rec.is_pseudo())
               {
                rec.otd_pd = 0;
                rec.diag   = 0;
               }
               else
               {
                rec.otd_pd = OTD_PDIAG_NODATA;
                rec.diag   = MDBR_DIAG_BAD;
               }
               record_changed(rec,MDBR_FIELD_DIAG,time);
               //ret|=handle_record(rec,op,time);
              }
        ++lo_range;
       }
      }
   //unlock;
   }
   return ret;

 }

 DWORD        __fastcall med_db::record_changed(DWORD rec_id,DWORD chmask,__int64 time)
 {
   DWORD ret = 0;
   if(chmask)
   {
   if(!time)  time = GetTime();

    //Пересчитать комбинированные
    if(chmask&MDBR_FIELD_DIAG) chmask|= (MDBR_FIELD_VALUE|MDBR_FIELD_STATE);
    if(chmask & (MDBR_FIELD_DIAG | MDBR_FIELD_VALUE))
       recalc_combined(rec_id);
       record_changes[rec_id]|=chmask;
   if(module)
      {
       MDB_RECORD_CHANGED rc;
       rc.rec_id = rec_id;
       rc.mask   = chmask;
       if(chmask & MDBR_RECORD_REMOVED)
          module->notify(MDB_NOTIFY_MASK_RECORD,MDB_NOTIFY_CODE_RECORD_REMOVED,&rc,sizeof(rc));
       else
          module->notify(MDB_NOTIFY_MASK_RECORD,MDB_NOTIFY_CODE_RECORD_CHANGED,&rc,sizeof(rc));

      }
  }
  return  ret;
 }

 DWORD    __fastcall  med_db::calc_change_percent(mdb_record & rec,float new_value)
 {
  DWORD  ret = 0;
  float dif       = fabs(rec.value - new_value);
  float scale_begin,scale_end;
  if(rec.scale_id == DWORD(-1))
     rec.get_scale_info    (scale_begin,scale_end);
     else
     {
       //TODO
       rec.get_scale_info    (scale_begin,scale_end);
     }
  float scale_dif = fabs(scale_end - scale_begin);
  if(scale_dif>.0001)
  {
     float percent = fabs(dif/scale_dif)*100.0;
     percent*=100.0;
     ret = (DWORD)percent;
  }else
   ret = 100;
  return ret ;
 }

 DWORD        __fastcall med_db::record_changed(mdb_record & rec,DWORD chmask,__int64 time)
 {
   /*Обработка изменения записи*/
  DWORD ret =0;
  if(chmask)
  {
   if(chmask & MDBR_FIELD_DIAG)
      chmask|=MDBR_FIELD_VALUE|MDBR_FIELD_STATE;
   if(chmask&(MDBR_FIELD_VALUE|MDBR_FIELD_STATE|MDBR_FIELD_DIAG))
   {
     if(!time)  time = GetTime();
     chmask|= MDBR_FIELD_TIME;
     rec.last_time = time;
     rec.time = time;
   }

    wchar_t   rep_text[MAX_PATH<<2];
    wchar_t   name[MAX_PATH];

    /*
    if(rec.state & (MDBR_STATE_ALARM|MDBR_STATE_DANGER))
         report_alarm_danger(rec);
    */


   if((chmask&MDBR_FIELD_STATE)  && (rec.state &(MDBR_STATE_ALARM|MDBR_STATE_DANGER)))
     {
      if((rec.addr.param_number!=(DWORD)-1 || rec.options&MDBR_OPT_COMBINED) )
         report_alarm_danger(rec);
     }

    if(chmask&(MDBR_FIELD_DIAG|MDBR_FIELD_STATE) || ((rec.state&MDBR_STATE_DISCRETE) && (chmask&MDBR_FIELD_VALUE)))
       {
        record_changes[rec.rec_id]|=chmask;
        //update_owner_diag(rec,time);
       }

   if(rec.addr.param_number !=(DWORD)-1 && (chmask & MDBR_FIELD_VALUE) && (rec.state&MDBR_STATE_TUTR_ACTIVE))
   {
    if(!get_name(rec.rec_id,true,name,MAX_PATH))
       get_name(rec.rec_id,false,name,MAX_PATH);

    if(rec.state&MDBR_STATE_DISCRETE)
     wsprintfW(rep_text,L"ТУ - %s %03d.%03d.%03d.%03d-.%03d %s ",(DWORD)rec.value ? L"ВКЛ ":L"ОТКЛ",
              (DWORD)rec.addr.addr.pu,(DWORD)rec.addr.addr.cp,(DWORD)rec.addr.addr.fa,(DWORD)rec.addr.addr.sb,(DWORD)rec.addr.param_number,name);
     else
     wsprintfW(rep_text,L"ТР - %03.2f %03d.%03d.%03d.%03d-.%03d %s ",rec.value,
               (DWORD)rec.addr.addr.pu,(DWORD)rec.addr.addr.cp,(DWORD)rec.addr.addr.fa,(DWORD)rec.addr.addr.sb,(DWORD)rec.addr.param_number,name);
    module->mdb_report_tutr(REPORT_SUCCESS_TYPE,rep_text,0,0);
   }

   ret = record_changed(rec.rec_id,chmask,time);
   }
   return  ret;
 }


float __fastcall mdb_record::calc_value(float kv )
{
 float result;
 bool inv = options&MDBR_OPT_INVERSE_SCALE ? true:false;
 float _min_val = inv ? max_value:min_value;
 float _max_val = inv ? min_value:max_value;
 if(!kvants) {return  kv;}
 if(is_signed_kvants())
    {
     result =  ((_max_val)/(float)kvants)*(float)(kv);
    }
 else
   result =  _min_val+((float)kv)*(_max_val-_min_val)/((float)kvants) ;
 return result;
}


 DWORD __fastcall  med_db::handle_record(mdb_record & rec,otd_proto & op,__int64 & time)
 {
  //Другая обработка для КП и ПУ
  DWORD ret = 0;
  if(!rec.is_pseudo() && rec.addr.param_number<(DWORD)-1)
  {
  DWORD  otd_diag  = (op.diag ) ? *op.diag : 0;
  LONGLONG  otd_value = 0;
  //float  fl_value  ;

  DWORD  new_diag;
  new_diag       = (otd_diag&OTD_DIAG_MASK) ? MDBR_DIAG_BAD:MDBR_DIAG_GOOD;
  DWORD  otd_pd  = new_diag ? OTD_PDIAG_NODATA : 0;
  if(op.data && !otd_get_value(op.data,rec.addr.param_number,&otd_value,sizeof(otd_value))  )
     {
      DWORD new_opt =  rec.options;
      new_opt&=~(MDBR_OPT_FLOAT_TYPE|MDBR_OPT_DOUBLE_TYPE);
      if(op.data->otd_type == OTD_FLOAT || op.data->otd_type == OTD_SIGNED_FLOAT)
          new_opt|=MDBR_OPT_FLOAT_TYPE;
         else
         {
          if(op.data->otd_type == OTD_DOUBLE || op.data->otd_type == OTD_SIGNED_DOUBLE)
             new_opt|=MDBR_OPT_DOUBLE_TYPE;
         }
      if(rec.options ^ new_opt) {rec.options = new_opt; ret |= MDBR_FIELD_OPTIONS;}

      if(rec.otd_val != otd_value)
      {
       if(rec.is_signed_kvants() && !(rec.options&(MDBR_OPT_FLOAT_TYPE|MDBR_OPT_DOUBLE_TYPE)))
         rec.otd_val = rec.get_signed_kvants(otd_value);
         else
         {
          rec.otd_val=otd_value;
         }

       ret |=MDBR_FIELD_OTDVALUE;
      }

     }


    if(op.personal_diag)
         otd_get_value(op.personal_diag,rec.addr.param_number,&otd_pd,sizeof(otd_value));


     if (rec.otd_pd!=otd_pd)

     {
        ret|=(MDBR_FIELD_OTDPD|MDBR_FIELD_TIME);
        WORD new_state = rec.state & MDBR_STATE_MASK;
        // TODO сделать нормальный разбор флагов ТУ
        new_state |= (otd_pd&OTD_PSTATE_TUTR_MASK);

       if(otd_pd&(OTD_PDIAG_TUTR_FAIL|OTD_PDIAG_TUTR_DESCRIPT))
         new_state|=MDBR_STATE_TUTR_ERROR;
         else
         new_state&=~MDBR_STATE_TUTR_ERROR;
      if(rec.state!=new_state)
          rec.state =new_state; ret|=MDBR_FIELD_STATE;
     rec.otd_pd = otd_pd;
     if(otd_pd&OTD_PDIAG_MASK )  new_diag = MDBR_DIAG_BAD;
      rec.time  = time =  GetTime();
     }


   if(rec.diag!=(WORD)new_diag   )
    {
     rec.diag=new_diag;
     rec.last_value = rec.value;
     rec.time  = time =  GetTime();
     ret|=(MDBR_FIELD_DIAG|MDBR_FIELD_VALUE|MDBR_FIELD_STATE|MDBR_FIELD_LASTVALUE|MDBR_FIELD_TIME|MDBR_FIELD_OTDPD);
    }

     if(ret&(MDBR_FIELD_DIAG|MDBR_FIELD_OTDVALUE|MDBR_FIELD_OTDPD))
        {
         ret |= calc_record(rec,time);
        }
     if(ret)
        record_changed(rec,ret,time);
  }
  return ret;
 }


DWORD        __fastcall med_db::calc_record_kvants(mdb_record & rec,float kvants,float & result)
{
      /*Вычисление значения для кванта*/
      DWORD ret = 0;
      result = (float)kvants;
      if(rec.scale_id!=(DWORD)-1)
       {
        DWORD _kv = kvants;
        ret  = module->scale_calc(rec.scale_id,_kv,result,true);
        if(ret == (DWORD)GKH_RET_ERROR || ret == (DWORD)GKH_RET_EXCEPTION)
           {
            rec.scale_id = -1;
            ret = calc_record_kvants(rec,kvants,result);
           }

       }
       else
       {
         if(rec.kvants)
         {
          if(rec.state & MDBR_STATE_DISCRETE)
             kvants = std::min(kvants,(float)rec.kvants);
             result = rec.calc_value((float)kvants);
         }

        if(rec.state & MDBR_STATE_DISCRETE)
        {
          WORD normal_state = (rec.state & MDBR_STATE_NORMAL) ? 1 : 0;
          if( normal_state!= WORD(result))
            {
              if(rec.options & MDBR_OPT_DANGER)
                ret|= (normal_state ?  SCL_ERR_MINWARN  : SCL_ERR_MAXWARN );
              if(rec.options & MDBR_OPT_ALARM )
                ret|= (normal_state ?  SCL_ERR_MINALARM : SCL_ERR_MAXALARM);
            }
        }
        else
        {
          if(result<rec.min_danger && rec.options&MDBR_OPT_DANGER)
              ret |= SCL_ERR_MINWARN;
          if(result>rec.max_danger && rec.options&MDBR_OPT_DANGER)
              ret |= SCL_ERR_MAXWARN;
          if(result<rec.min_alarm && rec.options&MDBR_OPT_ALARM)
              ret |= SCL_ERR_MINALARM;
          if(result>rec.max_alarm && rec.options&MDBR_OPT_ALARM)
              ret |= SCL_ERR_MAXALARM;
        }


       }
  return ret;
}

DWORD        __fastcall med_db::calc_record   (mdb_record & rec,__int64 time)
{
  DWORD ret = 0 ;
  if(!time) time = GetTime();

  if(rec.options & MDBR_OPT_COMBINED)
     ret = calc_combine(rec,time);
     else
     {
      WORD new_state = rec.state&~(MDBR_STATE_DANGER|MDBR_STATE_ALARM);
      float new_value;
      float kvants;
      if(rec.options & MDBR_OPT_FLOAT_TYPE)
         kvants = rec.otd_val_flt;
         else
         {
         if(rec.options & MDBR_OPT_DOUBLE_TYPE)
           kvants = rec.otd_val_dbl;
           else
           kvants = rec.otd_val;
         }
      DWORD err =
      calc_record_kvants(rec,kvants,new_value);

      if(rec.options& (MDBR_OPT_DANGER|MDBR_OPT_ALARM))
        {

        if(err & (SCL_ERR_MINWARN | SCL_ERR_MAXWARN))
           {
            new_state|=MDBR_STATE_DANGER;
           }
         if(err&(SCL_ERR_MINALARM|SCL_ERR_MAXALARM))
            {
            new_state|=MDBR_STATE_ALARM;
            }
       }
     if(new_state!=rec.state)
        {
          ret|=MDBR_FIELD_STATE;
          rec.state = new_state;
        }
     if(new_value!=rec.value )
        {
         DWORD percent = 0;
         if(rec.state&(MDBR_STATE_ALARM|MDBR_STATE_ALARM))
            percent = rec.percent+1;
            else
            percent = calc_change_percent(rec,new_value);
         if(!rec.percent ||  (percent > rec.percent) )
         {
           ret|=MDBR_FIELD_VALUE;
           rec.last_value = rec.value;
           ret |=  MDBR_FIELD_LASTVALUE;
           rec.value = new_value;
         }
        }
     }

  return ret;
}

float        __fastcall med_db::get_record_min_value(mdb_record & rec)
{
 float ret = MAXFLOAT;
 if(rec.options&MDBR_OPT_CONST_MINMAX)
    ret = rec.min_value;
 else
 {
 if(rec.options&MDBR_OPT_COMBINED)
  {
   //Вычисление минимального значения комбинированного объекта
   calc_combine(rec,ret,1);
  }
  else
  {

       if(rec.addr.param_number!=(DWORD)-1)
       {
        if(rec.scale_id!=(DWORD)-1)  //scale.h
        {
            SC_MINMAX  sc_minmax;
            sc_minmax.id = rec.scale_id;
            module->scale_minmax(&sc_minmax);
            ret = sc_minmax.min_y;
        }
         else
         ret = rec.min_value;
       }
       else
       ret = .0;
  }
 }
  return ret;
}

float        __fastcall med_db::get_record_max_value(mdb_record & rec)
{
 float ret = MINFLOAT;
 if(rec.options&MDBR_OPT_CONST_MINMAX)
    ret = rec.max_value;
    else
 {
 if(rec.options&MDBR_OPT_COMBINED)
  {
   //Вычисление максимального значения комбинированного объекта
   calc_combine(rec,ret,2);
  }
  else
  {
       if(rec.addr.param_number!=(DWORD)-1)
       {
        if(rec.scale_id!=(DWORD)-1)  //scale.h
        {
            SC_MINMAX  sc_minmax;
            sc_minmax.id = rec.scale_id;
            module->scale_minmax(&sc_minmax);
            ret = sc_minmax.max_y;

        }
         else
         {
          ret = rec.max_value;

         }
       }
       else
       ret = 1.0;
  }
 }
  return ret;
}

DWORD        __fastcall med_db::update_minmax_value (mdb_record & rec)
{
 DWORD ret = 0;
 float new_min = this->get_record_min_value(rec);
 float new_max = this->get_record_max_value(rec);
 if(new_min>new_max)
    swap(new_min,new_max);
 if(rec.min_value!=new_min)
    {
     ret|=MDBR_FIELD_MIN_VALUE;
     rec.min_value = new_min;
    }
 if(rec.max_value!=new_max)
    {
     ret|=MDBR_FIELD_MAX_VALUE;
     rec.max_value = new_max;
    }
 return ret;
}


void  __fastcall get_owner_addr(sotd_param_addr & pa)
{

 if(pa.param_number == (DWORD)-1)
    pa.addr.addr = sotd_addr(pa.addr).owner_addr().addr;
    else
    pa.param_number = (DWORD)-1;

}

 void         __fastcall med_db::update_owner_diag(mdb_record & _rec,__int64 time)
 {
     #if( FASTMUTEX_LOCKER != 0)
      TLockHelper l (locker);
     #else
       wr_lock    l (locker);
     #endif
    if(_rec.addr.addr.cp<OTD_ADDR_MAXVALUE)
    {
     sotd_addr owner_addr = _rec.addr.addr;
     if(_rec.addr.param_number == DWORD (-1))
        {
         owner_addr = owner_addr.owner_addr();
         if(owner_addr.fa <OTD_ADDR_MAXVALUE)
            owner_addr = owner_addr.owner_addr();
        }

     DWORD offset  = records.find_by_addr(owner_addr,-1);
     if(offset<records.size())
     {
      mdb_record & owner_rec = records.at(offset);
      WORD new_diag = owner_rec.diag;
      new_diag  = _rec.diag ?  MDBR_DIAG_PARTIAL : MDBR_DIAG_GOOD;
      if(owner_rec.diag != new_diag)
      {
       owner_rec.diag   = new_diag;
       owner_rec.value  = new_diag ? .0 : 1.0;
       owner_rec.otd_pd = new_diag ? OTD_PART_DIAG_PARAM : 0;
       record_changed(owner_rec,MDBR_FIELD_DIAG|MDBR_FIELD_VALUE|MDBR_FIELD_OTDPD);
       update_owner_diag(owner_rec,time);
      }
     }
    }
 }

 void         __fastcall med_db::update_owner_diag(DWORD id,__int64 time)
 {
  //Обновление диагностики владельца
     #if( FASTMUTEX_LOCKER != 0)
      TLockHelper l (locker);
     #else
       wr_lock    l (locker);
     #endif
  DWORD offset =  records.find_by_id(id);
  if(offset<records.size())
  {
    mdb_record & _rec = records.at(offset);
    update_owner_diag(_rec,time);
  }
 }

DWORD      __fastcall med_db::record_scale_changed(DWORD scale_id)
{
 DWORD ret = 0;
     #if( FASTMUTEX_LOCKER != 0)
      TLockHelper l (locker);
     #else
       wr_lock    l (locker);
     #endif
 DWORD count = records.size();
 __int64 time = GetTime();
 for(DWORD i = 0;i<count;i++)
 {
  mdb_record & rec = records.at(i);

  if(rec.scale_id == scale_id)
  {
   DWORD ch = update_minmax_value(rec);
         ch |= calc_record(rec,time);
   if(ch)
     {
      record_changed(rec,ch,time);
      ++ret;
     }
  }
 }
 //unlock;
 return ret;

}

void         __fastcall med_db::handle_record_changes()
{
     #if( FASTMUTEX_LOCKER != 0)
      TLockHelper l (locker);
     #else
       rd_lock    l (locker);
     #endif
 entry_index::iterator  low,hi;
 TChangesMap::iterator ch_beg = record_changes.begin(),ch_end = record_changes.end();
 std::distance(ch_beg,ch_end);
 //Формируем список кадров которым принадлежат изменившиеся объекты
 while(ch_beg!=ch_end)
 {
  DWORD rec_id  = ch_beg->first;
  DWORD ch_mask = ch_beg->second;
  if(ch_mask &MDBR_FIELD_OPTIONS)
     ch_mask|=MDBR_FIELD_STATE;
  if(ch_mask &MDBR_FIELD_STATE)
     ch_mask|=MDBR_FIELD_OPTIONS;
  if(ch_mask & (MDBR_FIELD_VALUE|MDBR_FIELD_DIAG|MDBR_FIELD_STATE|MDBR_FIELD_NAME|MDBR_FIELD_SNAME|MDBR_RECORD_REMOVED | MDBR_FIELDS_DANGERS | MDBR_FIELDS_ALARMS | MDBR_FIELD_OPTIONS))
  {
    if( kadrs.get_kadr_range(rec_id,low,hi))
    {
     while(low<hi)
      {
       DWORD kadr_id = kadrs.entry_at(*low)->kadr_id;
       TChangesMap::iterator ci;
       ci = kadr_changes.find(kadr_id);
       if(ci==kadr_changes.end())
          kadr_changes[kadr_id]=MDB_KADR_FIELD_VALUES;
          else
          ci->second |=MDB_KADR_FIELD_VALUES;
       ++low;
      }
    }
  }
  if(ch_mask & (MDBR_FIELD_DIAG|MDBR_ALARMS_CHANGED))
    {
     //При восстановлении работоспособности Отправить на железо аварийные уставки
     mdb_record & rec = records.get_record(rec_id);

     if(rec.is_pseudo() && rec.diag)
     {
      rec.diag = MDBR_DIAG_GOOD; ch_mask|=MDBR_FIELD_DIAG;
     }

     wchar_t rep_text[MAX_PATH];
     int  rep_len;
        if(rec.diag == MDBR_DIAG_GOOD &&  !(ch_mask & MDBR_ALARMS_CHANGED) && rec.addr.param_number == (DWORD)-1 && OTD_ADDR_TYPE_CP == sotd_addr(rec.addr.addr).addrtype())
            {
             //Восстановление работоспособности
             //mediumdb.h
             send_alarm_condition(rec);
             module->print_rct(rep_text,sizeof(rep_text),MDB_RCSTR_ALARM_RESTORE,(DWORD)rec.addr.addr.pu,(DWORD)rec.addr.addr.cp);
             module->mdb_report(REPORT_INFORMATION_TYPE,rep_text,NULL);
            }

        if((ch_mask & MDBR_ALARMS_CHANGED) && rec.addr.param_number != (DWORD)-1)
            {
             wchar_t addr_text[64];
             swprintf(addr_text,L"%02hu.%02hu.%02hu.%02hu-%d",(WORD)rec.addr.addr.pu,(WORD)rec.addr.addr.cp,(WORD)rec.addr.addr.fa,(WORD)rec.addr.addr.sb,rec.addr.param_number);
             if(rec.options&MDBR_OPT_ALARM)
                module->print_rct(rep_text,sizeof(rep_text),MDB_RCSTR_ALARM_CHANGED,rec.rec_id,addr_text,rec.min_alarm,rec.max_alarm);
                else
                module->print_rct(rep_text,sizeof(rep_text),MDB_RCSTR_ALARM_OFF,rec.rec_id,addr_text);
                module->mdb_report(REPORT_INFORMATION_TYPE,rep_text,NULL);
                send_alarm_condition(rec);
            }
    }

  ++ch_beg;
 }
 //Обновление  диагностики кадра
 ch_beg = kadr_changes.begin(),ch_end = kadr_changes.end();
 while(ch_beg!=ch_end)
  {
     ch_beg->second        |= kadr_update_diag(ch_beg->first);
     ++ch_beg;
  }

   //TODO 2 send_kadrs_changed  подписчикам
 handle_kadr_changes();
 ch_beg = kadr_changes.begin(),ch_end = kadr_changes.end();
 while(ch_beg!=ch_end)
  {
     mdb_record & rec = records.get_record(ch_beg->first);
     rec.state&=~MDBR_STATE_FLASH;
     ++ch_beg;
  }

 record_changes.clear();
 //unlock;
}

void         __fastcall med_db::handle_kadr_changes()
{
 /* Отправка данных подписчикам*/
     #if( FASTMUTEX_LOCKER != 0)
      TLockHelper l (locker);
     #else
       rd_lock    l (locker);
     #endif
 TChangesMap::iterator ch_beg = kadr_changes.begin(),ch_end = kadr_changes.end();

 MDB_KADR_CHANGED kc;
 //sended_recs sr,sr_common;

 while(ch_beg!=ch_end)
 {
  DWORD kadr_id = ch_beg->first;
  DWORD ch_mask = ch_beg->second;
  order_vector::iterator modem_low,modem_hi;
  kadrs_list::iterator _kadr = kadrs.find_kadr(kadr_id);
  if(kadrs.is_valid_kadr_ptr(_kadr))
  {
   __int64 image_time = get_kadr_image_time(_kadr);
   if((ch_mask&MDB_KADR_FIELD_IMAGE) || image_time!=_kadr->image_time)
      {
       ch_mask|=MDB_KADR_FIELD_IMAGE;
       _kadr->image_time = image_time;
       // NEW CHANGES
       module->notify(MDB_NOTIFY_MASK_KADR,MDB_NOTIFY_CODE_KADR,&kc,sizeof(kc));
       module->set_ccflags(MDB_CCFL_KADRS_TABLE,true);
     }
  }
  /*Отправка данных всем подписчикам*/

  if(orders.get_modem_range(kadr_id,modem_low,modem_hi))
     {
      mproto_send_kadr_changes(kadr_id,ch_mask,modem_low,modem_hi,NULL/*&sr*/);
     }
    //Это особый случай. Передача тем кто подписался на всё
    if(orders.get_modem_range(-1,modem_low,modem_hi))
       {
        if(ch_mask&(MDBR_FIELD_VALUE |MDBR_FIELD_DIAG|MDBR_FIELD_STATE|MDBR_FIELD_KVANTS|MDBR_FIELD_OTDPD ))
           ch_mask|=(MDBR_FIELD_VALUE |MDBR_FIELD_DIAG|MDBR_FIELD_STATE|MDBR_FIELD_KVANTS|MDBR_FIELD_OTDPD );
        mproto_send_kadr_changes(kadr_id,ch_mask,modem_low,modem_hi,NULL/*&sr_common*/);
       }

  kc.kadr_id = ch_beg->first;
  DWORD mask = ch_beg->second;
  kc.mask    =mask & MDB_KADR_FIELD_MASK;
  if(kc.mask )
    module->notify(MDB_NOTIFY_MASK_KADR,MDB_NOTIFY_CODE_KADR,&kc,sizeof(kc));
  kc.mask = mask & MDB_KADR_FIELD_ENTRYES;
  if(kc.mask )
     module->notify(MDB_NOTIFY_MASK_KADR,MDB_NOTIFY_CODE_KADR_CONTENT,&kc,sizeof(kc));

  ++ch_beg;
 }
 kadr_changes.clear();
 //unlock;
}

void __fastcall med_db::report_alarm_danger(mdb_record & rec)
{
  wchar_t rep_text[MAX_PATH<<2];
  wchar_t rec_name[MAX_PATH];
  get_name(rec.rec_id,true,rec_name,MAX_PATH);
  if(!rec_name[0])
    get_name(rec.rec_id,false,rec_name,MAX_PATH);
    swprintf(rec_name+lstrlenW(rec_name),L" %d.%d.%d.%d-%d",(DWORD)rec.addr.addr.pu,(DWORD)rec.addr.addr.cp,(DWORD)rec.addr.addr.fa,(DWORD)rec.addr.addr.sb,(DWORD)rec.addr.param_number);
    swprintf(rep_text,L" %s сотояние объекта  %s  значение %.2f",(rec.state & MDBR_STATE_ALARM)? L"Аварийное":L"Предупредительное",  rec_name,rec.value);
    module->mdb_report(REPORT_ERROR_TYPE,rep_text,0,0);


}


DWORD        __fastcall med_db::get_count_opts(DWORD opt_mask,trecords_index::iterator low,trecords_index::iterator hi)
{
  DWORD ret = 0;
  while(low<hi)
  {
   mdb_record & rec = records.at(*low);
   if(rec.options&opt_mask)
       ++ret;
   ++low;
  }
  return ret;

}

DWORD        __fastcall med_db::get_count_alarms(trecords_index::iterator low,trecords_index::iterator hi)
{
  DWORD ret = 0;
  while(low<hi)
  {
   mdb_record & rec = records.at(*low);
   if(rec.options&MDBR_OPT_ALARM)
   {
       if(rec.addr.addr.fa == OTD_FA_DISCRETE)
        ++ret;
        else
        {
          if(rec.has_max_alarm())
             ++ret;
          if(rec.has_min_alarm())
             ++ret;

        }
   }
   ++low;
  }
  return ret;
}


void         __fastcall get_alarm_param(mdb_record & rec,lpotd_alarm_param & ap)
{


  if(rec.addr.addr.fa == OTD_FA_DISCRETE)
    {
      ap->type   = OTD_ANALOG_DWORD;
      ap->param_addr = rec.addr;
      if(rec.options&MDBR_OPT_INVERSE_SCALE)
         {
           ap->value = 1;
           ap->cmd   = OTD_ALARM_CMD_LESS|OTD_ALARM_CMD_SET;
         }
         else
         {
           ap->value = 0;
           ap->cmd   = OTD_ALARM_CMD_MORE|OTD_ALARM_CMD_SET;
         }
       ++ap;
    }
    else
    {
     if(rec.has_min_alarm())
       {
         ap->param_addr = rec.addr;
         ap->type   = OTD_ANALOG_DWORD;
         ap->cmd    = OTD_ALARM_CMD_LESS|OTD_ALARM_CMD_SET;
         ap->value  = rec.calc_kvants(rec.min_alarm);
         ++ap;
       }

     if(rec.has_max_alarm())
       {
         ap->param_addr = rec.addr;
         ap->type   = OTD_ANALOG_DWORD;
         ap->cmd    = OTD_ALARM_CMD_MORE|OTD_ALARM_CMD_SET;
         ap->value  = rec.calc_kvants(rec.max_alarm);
         ++ap;
       }
    }

}


bool         __fastcall med_db::send_alarm_condition(mdb_record & rec)
{
 //Отправка на сервер уставок по авариям
     #if( FASTMUTEX_LOCKER != 0)
      TLockHelper l (locker);
     #else
       rd_lock    l (locker);
     #endif
 otd_param_addr lo_addr = rec.addr;
 //bool is_group = false;
 if(lo_addr.param_number == (DWORD)-1)
    {
     lo_addr.param_number = 0;
     //is_group = true;
    }
 if(lo_addr.addr.sb == 0xFF)
    lo_addr.addr.sb = 0;
 if(lo_addr.addr.fa == 0xFF)
    lo_addr.addr.fa = 0;

 trecords_index::iterator low,hi;
 if(records.get_addr_range(lo_addr,rec.addr,low,hi))
 {
   DWORD alarm_count = get_count_alarms(low,hi)+1;
   otd_proto op;
   DWORD sz =sizeof(otd_alarm_param);
   otd_proto_format_alarm(NULL,0,&sz,&rec.addr.addr,alarm_count,&op);
   //std::vector<BYTE> buff(sz+sizeof(MPROTO_HEADER)) ;
   std::unique_ptr<BYTE,array_deleter_t<BYTE> > buff(new BYTE[sz+sizeof(MPROTO_HEADER)]);
   LPMPROTO_HEADER mph = reinterpret_cast<LPMPROTO_HEADER>(buff.get());
   sz = otd_proto_format_alarm(mph->data,sz,&sz,&rec.addr.addr,alarm_count,&op);
   lpotd_alarm_param ap = (lpotd_alarm_param)op.extension->block_data;
    ap->param_addr = rec.addr;
    ap->param_addr.addr.fa&=OTD_FA_DATA_MASK;
    //ap->param_addr.param_number = -1;
    ap->cmd  = OTD_ALARM_CMD_CLEAR;
    ++ap;
   while(low<hi)
   {
     mdb_record & rec = records.at(*low);
     if(rec.options&MDBR_OPT_ALARM)
        get_alarm_param(rec,ap);
     ++low;
   }
   op.addr->addr = rec.addr.addr.addr;
   op.addr->fa  &= OTD_FA_DATA_MASK;
   op.addr->fa  |= OTD_FA_SETUP;
   otd_proto_protect(&op,0,0);
   mproto_init(mph,0,FA_OTD,sz);
   module->send(mph);


 }
 return false;
}




