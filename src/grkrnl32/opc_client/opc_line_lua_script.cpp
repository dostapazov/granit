
#pragma hdrstop
#include "gkopc_client.hpp"

wstring & __fastcall wstring_from_str(wstring & str,const char * src)
{
  if(src && *src)
     {
       str.resize(strlen(src)+1);
       KeRTL::Ansi2Unicode(const_cast<wchar_t*>(str.c_str()),str.size(),src);
     }
   return str;
}

string & __fastcall string_from_wstr(string & str,const wchar_t * src)
{
  if(src && *src)
     {
       str.resize(wcslen(src)+1);
       KeRTL::Unicode2Ansi(const_cast<char*>(str.c_str()),str.size(),src);
     }
   return str;
}



 DWORD __fastcall opc_line::get_script   (DWORD script_idx,LPSCRIPT_TEXT st)
 {
   TLockHelper l(locker);
   if(script_idx<KERTL_ARRAY_COUNT(lua_scripts))
   {
   string & str = lua_scripts[script_idx];
   st->text_len = str.size();
   strncpy(st->text,str.c_str(),std::min((DWORD)st->text_len,st->text_capacity));
   if(st->text_len>(LONG)st->text_capacity)
      st->text_len = -st->text_len;
    return GKH_RET_SUCCESS;
   }
   return GKH_RET_ERROR;
 }

 DWORD __fastcall opc_line::set_script   (DWORD script_idx,LPSCRIPT_TEXT st)
 {
   TLockHelper l(locker);
   if(script_idx<KERTL_ARRAY_COUNT(lua_scripts))
   {
    string & str = lua_scripts[script_idx];
    if(st->text_len)
      {
       str = string(st->text,st->text_len);
      }
      else
      str.clear();
   return GKH_RET_SUCCESS;
   }
   return GKH_RET_ERROR;
 }



const char *  __fastcall lua_get_item_id(lua_State * L,int stack_num)
{
 const char * ret = NULL;
 lua::LUAInstance lua(L,false);
 if(stack_num <= lua.gettop() && lua.isstring(stack_num))
 {
   unsigned int len = 0;
   ret = lua.tolstring(stack_num,&len);
 }
 else
 lua.global_get(LUA_OPC_ITEMID,ret);
 return ret;
}

int opc_line::lua_opc_item_set_value(lua_State *L)
{
  bool result = false;
  lua::LUAInstance lua(L,false);
  opc_line * line = NULL;
  lua.global_get(LUA_CLASS_NAME,(const void*&)line);
  if(line && lua.gettop()>1)
   {
    int isnum = 0;
    lua_Number value = lua.tonumberx(1,&isnum);
    const char * str = lua_get_item_id(L,2);
    if(str)
    {
     wstring item_id;
     wstring_from_str(item_id,str);
     result = line->opc_item_set_value(item_id,value);
    }
   }
   lua.push<bool,void>(result);
   return 1;
}


int opc_line::lua_opc_item_get_value(lua_State *L)
{
  lua::LUAInstance lua(L,false);
  opc_line * line = NULL;
  lua.global_get(LUA_CLASS_NAME,(const void*&)line);
  lua_Number value(0);
  int quality     (0);
  int success     (0);
  if(line )
   {
    unsigned int len = 0;
    const char * str = lua_get_item_id(L,1);
    if(str)
      {
        gkopc_items_t::iterator ptr;
        TLockHelper l(line->locker);
        wstring wstr;
        if(line->opc_items.find(*&gkopc_item(wstring_from_str(wstr,str)),ptr))
        {
          value    = (double)ptr->item_state;
          quality  = (int)ptr->item_state.wQuality;
          success  = 1;
        }
      }
   }
   lua.push<int,void>       (success);
   lua.push<lua_Number,void>(value);
   lua.push<int,void>       (quality);
  return 3;
}

int opc_line::lua_opc_set_items_quality  (lua_State *L)
{
 //Установка качества для элементов
  int result = 0;
  lua::LUAInstance lua(L,false);
  opc_line * line = NULL;
  lua.global_get(LUA_CLASS_NAME,(const void*&)line);
  if(line && lua.gettop()>0)
  {
  }
  lua.push<int,void>(result);
  return 1;
}

int opc_line::lua_opc_set_group_quality  (lua_State *L)
{
 //Установка качества для группы
  int result = 0;
  lua::LUAInstance lua(L,false);
  opc_line * line = NULL;
  lua.global_get(LUA_CLASS_NAME,(const void*&)line);
  int top = lua.gettop();
  if(line && top >1 && lua.isinteger(1))
  {
     int is_num = 0;
     lua_Integer quality = quality = lua.tointegerx(1,&is_num);
     lua_Integer fa      = (top>2) ? lua.tointegerx(2,&is_num) : -1;
     lua_Integer grp     = (top>3) ? lua.tointegerx(3,&is_num) : -1;
     result = line->opc_set_group_quality(fa,grp,quality);
  }
  lua.push<int,void>(result);
  return 1;
}

int opc_line::lua_opc_set_global_quality_mask  (lua_State *L)
{
  lua::LUAInstance lua(L,false);
  opc_line * line = NULL;
  lua.global_get(LUA_CLASS_NAME,(const void*&)line);
  int top = lua.gettop();
  if(line && top >2 && lua.isinteger(1) && lua.isinteger(2))
  {
    int is_num = 0;
    lua_Integer quality  = lua.tointegerx(1,&is_num);
    lua_Integer bool_set = lua.tointegerx(2,&is_num);
    line->opc_set_global_quality_mask  (quality,bool(bool_set));
  }
  return 0;
}

int opc_line::lua_opc_item_set_rc_state(lua_State *L)
  {
      int result = 0;
      lua::LUAInstance lua(L,false);
      opc_line * line = NULL;
      lua.global_get(LUA_CLASS_NAME,(const void*&)line);
      if(line && lua.gettop()>0)
      {
        int          is_num = 0;
        lua_Integer  state_arg = lua.tointegerx(1,&is_num);
        const char * str = lua_get_item_id(L,2);
        if(str)
        {
         gkopc_items_t::iterator ptr;
         TLockHelper l(line->locker);
         wstring wstr;
         if(line->opc_items.find(*&gkopc_item(wstring_from_str(wstr,str)),ptr))
         {
          result = 1;
          DWORD new_state = (ptr->rc_state&= ~ OTD_PSTATE_TUTR_MASK);
          new_state      |= (state_arg&OTD_PSTATE_TUTR_MASK);
          if(ptr->rc_state!=new_state || !new_state)
            {
             line->__set_opc_item_values(*ptr,NULL,NULL,NULL,&new_state);
            }
         }
        }
      }
   lua.push<int,void>(result);
   return 1;
  }

  int opc_line::lua_opc_item_set_rc_prepare(lua_State *L)
  {
      //Установка сброс подготовки
      int result = 0;
      lua::LUAInstance lua(L,false);
      opc_line * line = NULL;
      lua.global_get(LUA_CLASS_NAME,(const void*&)line);
      if(line && lua.gettop()>0)
      {
        int          is_num = 0;
        lua_Integer  set_arg = lua.tointegerx(1,&is_num);
        const char * str = lua_get_item_id(L,2);
        if(str)
        {
         gkopc_items_t::iterator ptr;
         TLockHelper l(line->locker);
         wstring wstr;
         if(line->opc_items.find(*&gkopc_item(wstring_from_str(wstr,str)),ptr))
         {
          result = 1;
          DWORD new_state = (ptr->rc_state& ~OTD_PSTATE_TUTR_MASK);
          if(set_arg) new_state |=OTD_PSTATE_TUTR_PREPARE;

          if(ptr->rc_state!=new_state)
            {
             __int64 _time = GetTime();
             line->__set_opc_item_values(*ptr,NULL,NULL,&_time,&new_state);
            }
         }
        }
      }
   lua.push<int,void>(result);
   return 1;
  }

  int opc_line::lua_opc_item_set_rc_error(lua_State *L)
  {
      //Установка сброс ошибки
      int result = 0;
      lua::LUAInstance lua(L,false);
      opc_line * line = NULL;
      lua.global_get(LUA_CLASS_NAME,(const void*&)line);
      if(line && lua.gettop()>0)
      {
        int          is_num = 0;
        lua_Integer  set_arg = lua.tointegerx(1,&is_num);
        const char * str = lua_get_item_id(L,2);
        if(str)
        {
         gkopc_items_t::iterator ptr;
         TLockHelper l(line->locker);
         wstring wstr;
         if(line->opc_items.find(*&gkopc_item(wstring_from_str(wstr,str)),ptr))
         {
          result = 1;
          DWORD new_state = (ptr->rc_state& ~OTD_PDIAG_MASK);
          if(set_arg) new_state |=OTD_PDIAG_TUTR_FAIL;

          if(ptr->rc_state!=new_state )
            {
             __int64 _time = GetTime();
             line->__set_opc_item_values(*ptr,NULL,NULL,&_time,&new_state);
            }
         }
        }
      }
   lua.push<int,void>(result);
   return 1;
  }


  int opc_line::lua_opc_item_set_rc_active(lua_State *L)
  {
      //Установка сброс активности ТУ
      int result = 0;
      lua::LUAInstance lua(L,false);
      opc_line * line = NULL;
      lua.global_get(LUA_CLASS_NAME,(const void*&)line);
      int  lua_stack_top = lua.gettop();
      if(line && lua_stack_top>0)
      {
        int          is_num = 0;
        lua_Integer  set_arg = lua.tointegerx(1,&is_num);
        const char * str = lua_get_item_id(L,2);
        if(str)
        {
         gkopc_items_t::iterator ptr;
         TLockHelper l(line->locker);
         wstring wstr;
         if(line->opc_items.find(*&gkopc_item(wstring_from_str(wstr,str)),ptr))
         {
          result = 1;
          DWORD new_state = (ptr->rc_state&~ OTD_PSTATE_TUTR_MASK);
          if   (set_arg)
          {
              lua_Integer rc_command = 0;
              lua.global_get(LUA_RC_COMMAND,rc_command);
              switch(rc_command)
              {
               case OTD_TUOP_ON  : new_state |=OTD_PSTATE_TUTR_ON_MORE ;break;
               case OTD_TUOP_OFF : new_state |=OTD_PSTATE_TUTR_OFF_LESS;break;
               case OTD_TROP_SET : new_state |=OTD_PSTATE_TUTR_ACTIVE  ;break;
              }
          }

          if(ptr->rc_state!=new_state || !new_state)
            {
             __int64 _time = GetTime();
             line->__set_opc_item_values(*ptr,NULL,NULL,&_time,&new_state);
            }
         }
        }
      }
   lua.push<int,void>(result);
   return 1;
  }



  int opc_line::lua_check_terminate      (lua_State *L)
  {
      // проверка необходимости завершения
      lua::LUAInstance lua(L,false);
      opc_line * line = NULL;
      lua.global_get(LUA_CLASS_NAME,(const void*&)line);
      int need_terminate = 1;
      if(line)
      {
       need_terminate = line->CheckTerminateRequest() ;
       if(!need_terminate)
          {
            TLockHelper l(line->locker);
            need_terminate = line->rc_undo_request ? TRUE : FALSE;
          }
      }
      lua.push<int,void>(need_terminate);
      return 1;
  }


int opc_line::lua_thread_sleep      (lua_State *L)
{
  lua::LUAInstance lua(L,false);
  int v = 0;
  int top = lua.gettop();
  if(top>0) v = lua.tointegerx(1,NULL);
  ::SleepEx(v,FALSE);
  return 0;
}

int opc_line::lua_trace      (lua_State *L)
{
   lua::LUAInstance lua(L,false);
   opc_line * line = NULL;
   lua.global_get(LUA_CLASS_NAME,(const void*&)line);
   if(lua.gettop())
     {
       wstring str;
       unsigned int len = 0;
       wstring_from_str(str,lua_tolstring(L,1,&len));
       line->owner->notify(MNF_NOTIFY_SCRIPT_TRACE,line->get_number(),(void*)str.c_str(),str.length()*sizeof(wstring::value_type));
       //TRACE(str.c_str(),0);
     }
   return 0;
}


  void __fastcall opc_line::rc_prepare_execute(lua::LUAInstance & lua)
  {
    // otd.h
    lua.global_set  (LUA_CLASS_NAME,(const void*)this);

    lua.global_set  (LUA_CONST_COMMAND_ON_MORE ,OTD_TUOP_ON);
    lua.global_set  (LUA_CONST_COMMAND_OFF_LESS,OTD_TUOP_OFF);
    lua.global_set  (LUA_CONST_COMMAND_SET     ,OTD_TROP_SET);
    lua.global_set  (LUA_CONST_COMMAND_UNDO    ,OTD_TUTROP_CANCEL);

    lua.global_set  (LUA_CONST_TUTR_STATE_PREPARE       , OTD_PSTATE_TUTR_PREPARE  );
    lua.global_set  (LUA_CONST_TUTR_STATE_ONMORE_ACTIVE , OTD_PSTATE_TUTR_ON_MORE  );
    lua.global_set  (LUA_CONST_TUTR_STATE_OFFLESS_ACTIVE, OTD_PSTATE_TUTR_OFF_LESS );
    lua.global_set  (LUA_CONST_TUTR_STATE_RC_ERROR      , OTD_PDIAG_TUTR_FAIL );


    lua.reg_function(LUA_TRACE                       ,lua_trace                  );
    lua.reg_function(LUA_FUNC_OPC_ITEM_GET_VALUE     ,lua_opc_item_get_value     );
    lua.reg_function(LUA_FUNC_OPC_ITEM_SET_VALUE     ,lua_opc_item_set_value     );
    lua.reg_function(LUA_FUNC_OPC_ITEM_SET_RC_STATE  ,lua_opc_item_set_rc_state  );
    lua.reg_function(LUA_FUNC_OPC_ITEM_SET_RC_PREPARE,lua_opc_item_set_rc_prepare);
    lua.reg_function(LUA_FUNC_OPC_ITEM_SET_RC_ACTIVE ,lua_opc_item_set_rc_active );
    lua.reg_function(LUA_FUNC_OPC_ITEM_SET_RC_ERROR  ,lua_opc_item_set_rc_error  );
    lua.reg_function(LUA_FUNC_CHECK_TERMINATE        ,lua_check_terminate        );
    lua.reg_function(LUA_FUNC_SLEEP                  ,lua_thread_sleep           );
    lua.enable_bin_functions();
  }

  void __fastcall opc_line::rc_run_script     (lua::LUAInstance & lua,gkopc_item & item)
  {
     std::string str;
     string_from_wstr(str,item.id.c_str());
     //TRACE(L"Begin LUA script",0);
     lua.global_set((const char*)LUA_OPC_ITEMID,(char*)str.c_str());

     string_from_wstr(str,item.rc_tag.c_str());
     lua.global_set((const char*)LUA_RC_TAG,(char*)str.c_str());

     lua.global_set(LUA_RC_OBJECT ,(lua_Integer)item.rc_object  );
     lua.global_set(LUA_RC_COMMAND,(lua_Integer)item.rc_current.command);
     lua.global_set(LUA_RC_VALUE  ,(lua_Number)item.rc_current .param  );


     int ret = lua.do_string(item.rc_script.c_str());
     //TRACE(L"End LUA script ret = %d",ret);
     if(ret)
     {
         TGkOpcModem * modem = dynamic_cast<TGkOpcModem*>(owner);
        if(modem)
        {
          wstring error_str;
          wstring_from_str(error_str,lua.tolstring(-1,NULL));
          modem->do_report(REPORT_ERROR_TYPE,(wchar_t*)error_str.c_str(),(void*)L"Ошибка исполнения скрипта",0);
          modem->notify   (MNF_NOTIFY_SCRIPT_ERROR,get_number(),(void*)error_str.c_str(),error_str.length()*sizeof(wstring::value_type));
        }

     }
  }

  int  opc_line::rc_thread_proc()
  {
     //Процедура
     lua::LUAInstance  lua;
     rc_prepare_execute(lua);
     DWORD rc_size = 0;
     TLockHelper l (locker);
     gkopc_items_t::iterator ptr  = rc_queue.begin();
     DWORD    rc_state = 0;
     __int64  cur_time;
     while(!CheckTerminateRequest() && rc_queue.size())
     {
       rc_undo_request    = FALSE;
       gkopc_item  & item = rc_queue.at(0);
       locker.unlock();
       rc_run_script(lua,item);
       locker.Lock();
       rc_queue.erase(rc_queue.begin());
     }
     rc_queue.clear();
     rc_thread = boost::thread();
     return 0;
  }




  void __fastcall opc_line::rc_start()
  {
    TLockHelper l(locker);
    if(rc_queue.size() &&  !rc_thread.joinable())  //(rc_thread.native_handle() == INVALID_HANDLE_VALUE || WaitForSingleObject(rc_thread.native_handle(),0) != WAIT_TIMEOUT))
    {
       rc_thread = boost::thread(&opc_line::rc_thread_proc,this);
    }
  }


void  __fastcall opc_line::__prepare_calc_scripts()
{
   calc_scripts.assign      (luaL_newstate(),true);
   calc_scripts.open_libs   ();
   calc_scripts.global_set  (LUA_CLASS_NAME,(const void*)this);
   calc_scripts.reg_function(LUA_TRACE                       ,lua_trace                  );
   calc_scripts.reg_function(LUA_FUNC_OPC_ITEM_GET_VALUE     ,lua_opc_item_get_value     );
   calc_scripts.reg_function(LUA_FUNC_OPC_ITEM_SET_VALUE     ,lua_opc_item_set_value     );
   calc_scripts.reg_function(LUA_FUNC_OPC_SET_ITEMS_QUALITY  ,lua_opc_set_items_quality  );
   calc_scripts.reg_function(LUA_FUNC_OPC_SET_GROUP_QUALITY  ,lua_opc_set_group_quality  );
   calc_scripts.reg_function(LUA_FUNC_OPC_SET_GLOBAL_QUALITY_MASK,lua_opc_set_global_quality_mask);

   calc_scripts.enable_bin_functions();
}

void  __fastcall opc_line::__calc_item_value(gkopc_item & item)
{
  item.data_value  = item.item_state.vDataValue;
  if(!item.calc_script.empty())
     {
       item.data_value = item.item_state.vDataValue;
       if(!calc_scripts.is_assigned()) __prepare_calc_scripts();

       std::string str;
       string_from_wstr(str,item.id.c_str());
       calc_scripts.global_set((const char*)LUA_OPC_ITEMID,(char*)str.c_str());

       if(item.item_state.is_value_float())
         {
           lua_Number lval = (double)item.item_state;
           calc_scripts.global_set(LUA_OPC_ITEM_VALUE,lval);
         }
         else
         {
           lua_Integer   lval = (double)item.item_state;
           calc_scripts.global_set(LUA_OPC_ITEM_VALUE,lval);

         }
       calc_scripts.global_set(LUA_OPC_ITEM_QUALITY,lua_Integer(item.item_state.wQuality));
       if( LUA_OK == calc_scripts.do_string(item.calc_script.c_str()))
         {
           int top = calc_scripts.gettop();
           if(top)
             {
               int is_num = 0;
               lua_Number ln = calc_scripts.tonumberx(top,&is_num);
               item.data_value = ln;
               calc_scripts.pop<lua_Number,int>(ln);
               calc_scripts.settop(0);
               calc_scripts.garbage_collect(LUA_GCCOLLECT,0);
             }
         }
         else
         {
            wstring error_str;
            wstring_from_str(error_str,calc_scripts.tolstring(-1,NULL));
            TGkOpcModem * modem = dynamic_cast<TGkOpcModem*>(owner);
            if(modem)
            {
              //modem->do_report(REPORT_ERROR_TYPE,(wchar_t*)error_str.c_str(),(void*)L"Ошибка исполнения скрипта",0);
              modem->notify   (MNF_NOTIFY_SCRIPT_ERROR,get_number(),(void*)error_str.c_str(),error_str.length()*sizeof(wstring::value_type));
            }
         }


     }
}


