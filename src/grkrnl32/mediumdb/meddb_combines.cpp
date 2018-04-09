
#pragma hdrstop
#include "mediumdb.hpp"
#ifdef MAXINT
#undef MAXINT
#endif
#include <float.h>


 void __fastcall med_db::recalc_combines()
 {
     #if( FASTMUTEX_LOCKER != 0)
      TLockHelper l (locker);
     #else
       wr_lock    l (locker);
     #endif
  DWORD comb_id;
  DWORD comb_idx = 0;
  while((DWORD)-1!=(comb_id = combine.get_combine_id(comb_idx++)))
  {
    calc_combine(comb_id);
  }
  //unlock();
 }

inline bool __fastcall is_lua_script(const std::string & formula)
 {
  return strstr(formula.c_str(),"--Lua") ? true : false;
 }

#define MEDDB_CLASS "med_db"
#define MEDDB_GET_RECORD_MIN_DANGER "get_min_danger"
#define MEDDB_GET_RECORD_MAX_DANGER "get_max_danger"
#define MEDDB_GET_RECORD_MIN_ALARM  "get_min_alarm"
#define MEDDB_GET_RECORD_MAX_ALARM  "get_max_alarm"


void __fastcall med_db::_lua_prepare()
{
 if(!lua_prepared)
 {
  lua.open_libs();
  lua.global_set(MEDDB_CLASS,(void*)this);
  lua.enable_bin_functions();
  lua.reg_function        (MEDDB_GET_RECORD_MIN_DANGER,_lua_get_record_min_danger);
  lua.reg_function        (MEDDB_GET_RECORD_MAX_DANGER,_lua_get_record_max_danger);
  lua.reg_function        (MEDDB_GET_RECORD_MIN_ALARM ,_lua_get_record_min_alarm );
  lua.reg_function        (MEDDB_GET_RECORD_MAX_ALARM ,_lua_get_record_max_alarm );
  lua_prepared = TRUE;
 }
}


wstring & __fastcall wstring_from_str(wstring & str,const char * src)
{
  if(src && *src)
     {
       str.resize(strlen(src)+1);
       KeRTL::Ansi2Unicode(const_cast<wchar_t*>(str.c_str()),str.size(),src);
     }
   return str;
}

#pragma warn -8057
bool  __fastcall med_db::_lua_script_exec(const std::string & formula,int count,long double * result ,DWORD * ids,long double * args,LPWORD diags,LPDWORD error)
{
  long double ret = .0;
  if(!lua_prepared) _lua_prepare();
  lua.global_set("count",count);
  for(int i = 0;i<count;i++)
  {
     char var_name[32];
     sprintf(var_name,"x%d",i);
     lua.global_set(var_name,(double)args[i]);
     sprintf(var_name,"id%d",i);
     lua.global_set(var_name,(int)ids[i]);
  }
  if(LUA_OK == lua.do_string(formula.c_str()))
  {
   int is_num = 0;
   ret = lua.tonumberx(lua.gettop(),&is_num);
   lua.settop(0);
   lua.garbage_collect(LUA_GCCOLLECT,0);
   if(result) *result = ret;
   return true;
  }
  else
  {
     std::wstring err_str;
     wstring_from_str(err_str,lua.tolstring(-1,NULL));
     module->mdb_report_script(REPORT_ERROR_TYPE,err_str.c_str(),(LPVOID)formula.c_str(),formula.length());

  }
  return false;
}
#pragma warn .8057

 /*Вычисление комбинированного объекта*/
 DWORD        __fastcall med_db::calc_combine(mdb_record & rec,float & value,DWORD opt)
 {

   DWORD new_diag = MDBR_DIAG_GOOD;
   DWORD       _ids  [128] ;
   long double _args [128] ;
   WORD        _diags[128] ;
   DWORD       *ids   = _ids;
   long double *args  = _args;
   WORD        *diags = _diags;


   value = rec.value;
   combine_list::iterator low,hi;
   DWORD count = combine.get_entry_range(rec.rec_id,low,hi);
   std::string formula = combine.get_formula(rec.rec_id);
   bool enable_calc = true;

   //if(count)
   {
    //  Подготовка списка аргусентов
    if(count>sizeof(_args)/sizeof(long double))
       {
        ids   = new DWORD       [count+1];
        args  = new long double [count+1];
        diags = new WORD        [count+1];
       }

    if(args)
      {
        //Подготовка аргументов
        std::fill(args,args+count,(long double).0);
        DWORD bad_count = 0;
        DWORD arg_n     = 0;
        __int64 prev_time = 0;
        bool  calc_by_last = (rec.options&MDBR_OPT_COMBINE_CALC_BY_LAST) ? true:false;

        while(low<hi)
        {
         combine_entry &ce   = *low;
         DWORD offset = records.find_by_id(ce.obj_id);
         if(offset<records.size())
         {
          mdb_record & entry_rec = records.at(offset);
          ids  [arg_n ]  = entry_rec.rec_id;
          diags[arg_n ]  = entry_rec.diag;
          args [arg_n ]  = entry_rec.value;

          if(!(opt) && (entry_rec.diag))
             ++bad_count;
             else
             {
              //Вычисляем по изменению последнего а последний еще не обновился
              if(calc_by_last && prev_time && prev_time > entry_rec.time)
                enable_calc = false;
              if(opt)
                args[arg_n] = opt == 1 ? get_record_min_value(entry_rec) :this->get_record_max_value(entry_rec);
              else
              if(!(entry_rec.state&MDBR_STATE_WORKOUT)) //Если не выведен из работы
                  args[arg_n] = entry_rec.value;
              prev_time = entry_rec.time;
             }
             arg_n++;

         }
         ++low;
        }

       if(enable_calc || bad_count)
       {
            if(!bad_count)
            {
             //Все составляющие достоверны
             value = rec.value;
             DWORD calc_err = 0;
             if(is_lua_script(formula))
             {
               long double result ;
               if(_lua_script_exec(formula,count,&result,ids,args,diags,&calc_err))
                  value = result;
                  else
                  new_diag = MDBR_DIAG_BAD;
             }
             else
             {
                 __try{
                       value = (float)calculator->Calc(formula.c_str(),count,args);
                      }
                 __finally{
                  calc_err = calculator->GetError();
                  }
             }
             new_diag  = (calc_err==0) ? MDBR_DIAG_GOOD : (MDBR_DIAG_BAD,calc_err);
            }
            else
            new_diag = (bad_count == count) ? MDBR_DIAG_BAD : MDBR_DIAG_PARTIAL;
        }
      }
   }
   if(ids  != _ids  )   delete [] ids;
   if(args != _args )   delete [] args;
   if(diags!= _diags)   delete [] diags;
  return new_diag;
 }

 DWORD        __fastcall med_db::calc_combine(mdb_record &rec,__int64 time)
 {
   // Вычисление комбинированного объекта

   DWORD changes = 0;
   float new_value ;
   WORD  new_diag =  calc_combine(rec,new_value,0);
         if(rec.value!=new_value)
            {
             if(!time) time = GetTime();
             WORD new_state = rec.state&~(MDBR_STATE_DANGER|MDBR_STATE_ALARM);
             rec.last_value = rec.value;
             rec.value      = new_value;
             changes|=MDBR_FIELD_VALUE;
             if((rec.options & MDBR_OPT_DANGER) && (rec.value> rec.max_danger || rec.value<rec.min_danger ))
                 new_state |=MDBR_STATE_DANGER;

             if((rec.options & MDBR_OPT_ALARM) && (rec.value> rec.max_alarm || rec.value<rec.min_alarm) )
                 new_state |=MDBR_STATE_ALARM;
             if(rec.state != new_state)
                {
                 rec.state  = new_state;
                 changes   |=MDBR_FIELD_STATE;
                }
            }

        if(rec.diag!=(WORD)new_diag)
          {
            rec.diag   = new_diag;
            rec.otd_pd = new_diag ? OTD_PDIAG_ERROR : 0;
            changes|=MDBR_FIELD_DIAG|MDBR_FIELD_VALUE;
          }

         if(changes && rec.time!= time)
         {
          rec.last_time = rec.time;
          rec.time      = time;
          changes|=MDBR_FIELD_LASTVALUE|MDBR_FIELD_TIME;
         }

  return changes;
 }

 DWORD        __fastcall med_db::calc_combine(DWORD id)
 {

  DWORD ret = 0;
     #if( FASTMUTEX_LOCKER != 0)
      TLockHelper l (locker);
     #else
       wr_lock    l (locker);
     #endif
  DWORD offset = records.find_by_id(id);
  if(offset<records.size())
   {
    mdb_record & rec = records.at(offset);
    if(rec.options&MDBR_OPT_COMBINED)
       {
        ret = calc_record(rec,0);
       if(ret)
        record_changed(rec,ret);
       }
   }
  //unlock();
 return ret; 
 }

 void __fastcall med_db::recalc_combined(DWORD id)
 {
     //Проверка на вхождение в состав комбинированных
       obj_index::iterator low,hi;
       if(combine.get_combine_range(id,low,hi))
        {
         while(low<hi)
         {
          combine_entry & ce = combine.get_entry(*low);
          /*DWORD ch = */calc_combine(ce.comb_id);
          ++low;
         }
        }
 }

 DWORD      __fastcall med_db::add_to_combine(DWORD id_comb,DWORD id_obj)
 {
  DWORD ret ;
  DWORD offset;
     #if( FASTMUTEX_LOCKER != 0)
      TLockHelper l (locker);
     #else
       wr_lock    l (locker);
     #endif
  offset = records.find_by_id(id_comb);
  if(offset!=(DWORD)-1)
  {
  ret = combine.add_object(id_comb,id_obj);
  DWORD ch = update_minmax_value(records.at(offset));
  record_changed(id_comb,ch,0);
  calc_combine(id_comb);
  config_change(MDB_CCFL_COMBINE_TABLE);
  handle_record_changes();

  }
  else
  ret = -1;
  //unlock();
  return ret;
 }

 DWORD      __fastcall med_db::remove_from_combine(DWORD id_comb,DWORD id_obj)
 {
   DWORD ret = 0;
     #if( FASTMUTEX_LOCKER != 0)
      TLockHelper l (locker);
     #else
       wr_lock    l (locker);
     #endif
   DWORD offset = records.find_by_id(id_comb);
   if(offset!=(DWORD)-1)
   {
   ret = combine.remove_object(id_comb,id_obj);
   if(ret)
     {
      DWORD ch = update_minmax_value(records.at(offset));
      record_changed(id_comb,ch,0);
      calc_combine(id_comb);
      config_change(MDB_CCFL_COMBINE_TABLE);
      handle_record_changes();
     }
   }
   //unlock();
   return ret;
 }


DWORD __fastcall  med_db::get_combine_formula(DWORD comb_id,LPMDB_RECORD_TEXT text)
{
 DWORD ret = GKH_RET_ERROR;
     #if( FASTMUTEX_LOCKER != 0)
      TLockHelper l (locker);
     #else
       rd_lock    l (locker);
     #endif
 std::string formula =
  combine.get_formula(comb_id);
 //unlock();
 if(formula.length()<text->text_size)
 {
  text->text_len = formula.length();
  if(text->text_len)
  {

     Ansi2Unicode(text->text,formula.c_str());
  }
  ret = GKH_RET_SUCCESS;
 }
 return ret;
}

DWORD __fastcall  med_db::set_combine_formula(DWORD comb_id,LPMDB_RECORD_TEXT text)
{
 DWORD ret = GKH_RET_ERROR;
     #if( FASTMUTEX_LOCKER != 0)
      TLockHelper l (locker);
     #else
       wr_lock    l (locker);
     #endif
 std::string formula;
 formula.resize(text->text_len+1);
  Unicode2Ansi((char*)formula.c_str(),text->text);
 if(comb_id == combine.set_combine_formula(comb_id,(char*)formula.c_str()))
    {
     config_change(MDB_CCFL_COMBINE_TABLE);
     ret = GKH_RET_SUCCESS;
     DWORD ch;
     ch = calc_combine(comb_id);
     //ch |= update_minmax_value(records.get_record(comb_id));
     if(ch)
     {
      record_changed(comb_id,ch,0);
      handle_record_changes();
     }
    }
 //unlock();
 return ret;
}

 DWORD      __fastcall med_db::get_combine_count(DWORD id)
 {
   DWORD ret = 0;
     #if( FASTMUTEX_LOCKER != 0)
      TLockHelper l (locker);
     #else
       rd_lock    l (locker);
     #endif
    ret = id == (DWORD)-1 ? combine.size() : combine.get_entry_count(id);
   //unlock();
   return ret;
 }

 DWORD __fastcall  med_db::get_combine_list(DWORD comb_id, LPMDB_COMBINE_LIST comb_list)
 {
   DWORD ret = GKH_RET_ERROR;
   if(module->check_data_size(comb_list,sizeof(*comb_list)))
   {
     ret = GKH_RET_SUCCESS;
     comb_list->count = 0;
     #if( FASTMUTEX_LOCKER != 0)
      TLockHelper l (locker);
     #else
       rd_lock    l (locker);
     #endif
     combine_list::iterator low,hi;
     DWORD cnt = combine.get_entry_range(comb_id,low,hi);
     std::advance(low,comb_list->start);
     memset(comb_list->obj_id,-1,comb_list->capacity*sizeof(comb_list->obj_id[0]));
     while(cnt && comb_list->count<comb_list->capacity && low<hi)
     {
      comb_list->obj_id[comb_list->count++] = low->obj_id;
      ++low;
     }
      
     //unlock();
   }
  return ret;
 }

DWORD __fastcall  med_db::set_combine_list(DWORD comb_id, LPMDB_COMBINE_LIST comb_list)
{
  DWORD  ret = GKH_RET_ERROR;
  if(module->check_data_size(comb_list,sizeof(*comb_list)))
  {
   if(combine.is_combine(comb_id))
    {
     #if( FASTMUTEX_LOCKER != 0)
      TLockHelper l (locker);
     #else
       wr_lock    l (locker);
     #endif
      combine.clear_combine_list(comb_id);
      for(DWORD i = 0;i<comb_list->count;i++)
       {
        if(comb_list->obj_id[i]!=(DWORD)-1)
         combine.add_object(comb_id,comb_list->obj_id[i]);
       }
      DWORD ch = update_minmax_value(records.at(records.find_by_id(comb_id)));
      record_changed(comb_id,ch,0);
      calc_combine(comb_id);
      //unlock();
      config_change(MDB_CCFL_COMBINE_TABLE);
      handle_record_changes();
    }
  }
  return ret;
}


 DWORD      __fastcall med_db::enum_combine(DWORD idx,LPMDB_RECORD rec)
 {
   DWORD ret = -1;
     #if( FASTMUTEX_LOCKER != 0)
      TLockHelper l (locker);
     #else
       rd_lock    l (locker);
     #endif
   DWORD id = combine.get_combine_id(idx);
   if(id!=(DWORD)-1 && rec)
     get_record(id,rec);
   ret = id;
   //unlock();
   return ret;
 }

bool __fastcall is_valid_float(double value)
{
     int float_class = _fpclass(value);
     return (!(float_class & ~(_FPCLASS_NN|_FPCLASS_PN|_FPCLASS_PZ|_FPCLASS_NZ))) ? true : false;
}

int   med_db::_lua_get_record_limits    (lua_State * l, int lim_type)
{

  int ret      = 1;
  int success  = 0;
  double value = 0;
  med_db * mdb = NULL;
  lua::LUAInstance lua(l,false);
  lua.global_get(MEDDB_CLASS,(const void*&)mdb);
  if(mdb)
  {
   int stk_top = lua.gettop();
   int is_num  = 0;
   DWORD rec_id = lua.tonumberx(1,&is_num);
   mdb_record rec;
   if(is_num && GKH_RET_SUCCESS == mdb->get_record(rec_id,&rec))
    {
      switch(lim_type)
      {
       case 0 : value = rec.min_danger;break;
       case 1 : value = rec.max_danger;break;
       case 2 : value = rec.min_alarm ;break;
       case 3 : value = rec.max_alarm ;break;
      }
      if( !is_valid_float(value))
         {
          value = rec.value + (lim_type & 1) ? 1.0 : -1.0;
         }
     lua.push<double,void>(value);
     success = 1;
    }
  }
 lua.push<int,void>(success);
 if(success)
  {
   ++ret;
   lua.push<double,void>(value);
  }
 return ret;
}

int   med_db::_lua_get_record_min_danger(lua_State * l)
{
 return _lua_get_record_limits(l,0);
}

int   med_db::_lua_get_record_max_danger(lua_State * l)
{
 return _lua_get_record_limits(l,1);
}

int   med_db::_lua_get_record_min_alarm (lua_State * l)
{
 return _lua_get_record_limits(l,2);
}

int   med_db::_lua_get_record_max_alarm (lua_State * l)
{
 return _lua_get_record_limits(l,3);
}




