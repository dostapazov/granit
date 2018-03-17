#include  <lua/lua_iface.hpp>

namespace lua
{

int LUAInstance::lua_bin_and (lua_State * L)
{
  __int64 arg1,arg2;
  int is_num = 0;
  LUAInstance lua(L,false);
  arg1 = lua.tonumberx(1,&is_num);
  arg2 = lua.tonumberx(2,&is_num);
  __int64 result = arg1&arg2;
  lua.push<__int64,void>(result);
  return 1;
}

int LUAInstance::lua_bin_or  (lua_State * L)
{

  __int64 arg1,arg2;
  int is_num = 0;
  LUAInstance lua(L,false);
  arg1 = lua.tonumberx(1,&is_num);
  arg2 = lua.tonumberx(2,&is_num);
  __int64 result = arg1|arg2;
  lua.push<__int64,void>(result);
  return 1;
}

int LUAInstance::lua_bin_xor (lua_State * L)
{
  __int64 arg1,arg2;
  int is_num = 0;
  LUAInstance lua(L,false);
  arg1 = lua.tonumberx(1,&is_num);
  arg2 = lua.tonumberx(2,&is_num);
  __int64 result = arg1^arg2;
  lua.push<__int64,void>(result);

  return 1;
}

int LUAInstance::lua_bin_not (lua_State * L)
{
  __int64 arg1;
  int is_num = 0;
  LUAInstance lua(L,false);
  arg1 = lua.tonumberx(1,&is_num);
  __int64 result = ~arg1;
  lua.push<__int64,void>(result);
  return 1;
}

int LUAInstance::lua_bin_shl (lua_State * L)
{
  __int64 arg1,arg2;
  int is_num = 0;
  LUAInstance lua(L,false);
  arg1 = lua.tonumberx(1,&is_num);
  arg2 = lua.tonumberx(2,&is_num);
  __int64 result = arg1<<arg2;
  lua.push<__int64,void>(result);
  return 1;
}

int LUAInstance::lua_bin_shr (lua_State * L)
{

  __int64 arg1,arg2;
  int is_num = 0;
  LUAInstance lua(L,false);
  arg1 = lua.tonumberx(1,&is_num);
  arg2 = lua.tonumberx(2,&is_num);
  __int64 result = arg1>>arg2;
  lua.push<__int64,void>(result);
  return 1;
}

} // end of namespace lua

