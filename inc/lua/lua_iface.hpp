#ifndef __LUA_INTERFACE__
#define __LUA_INTERFACE__

#define LUA_USE_WINDOWS
#define LUA_BUILD_AS_DLL
#include "lua.hpp"

namespace lua
{
    #define LUA_LIB_TYPE_BASE      0x0001
    #define LUA_LIB_TYPE_COROUTINE 0x0002
    #define LUA_LIB_TYPE_TABLE     0x0004
    #define LUA_LIB_TYPE_IO        0x0008
    #define LUA_LIB_TYPE_OS        0x0010
    #define LUA_LIB_TYPE_STRING    0x0020
    #define LUA_LIB_TYPE_UTF8      0x0040
    #define LUA_LIB_TYPE_BIT32     0x0080
    #define LUA_LIB_TYPE_MATH      0x0100
    #define LUA_LIB_TYPE_DEBUG     0x0200
    #define LUA_LIB_TYPE_PACKEGE   0x0400




/* open all previous libraries */
LUALIB_API void (luaL_openlibs) (lua_State *L);


    class LUAInstance
    {
     public:
      LUAInstance(lua_State * _st,bool _need_release = true):state ( _st ),need_release(_need_release)
      {}

      LUAInstance()
      {
       state = luaL_newstate();
       need_release = true;
       open_libs();
      }

      virtual ~LUAInstance()   {release();}
      void __fastcall assign   (lua_State * _st,bool _need_release = true){release();state = _st; need_release = _need_release;}
      void __fastcall release(){if(state && need_release) lua_close(state); state = NULL;}

      int __fastcall call(int nargs, int nresults = LUA_MULTRET , int errfunc = 0,lua_KContext ctx = 0, lua_KFunction k = NULL)
      { return lua_pcallk(state,nargs,nresults,errfunc,ctx,k); }

           int __fastcall load_string(const char * str) {return luaL_loadstring(state,str);}
           int __fastcall do_string(const char * str);

           int    __fastcall  gettop         (){return lua_gettop(state);}
           int    __fastcall  settop         (int top) {lua_settop(state,top); return gettop();}
           int    __fastcall  isnumber       (int idx){return lua_isnumber    (state,idx);}
           int    __fastcall  isstring       (int idx){return lua_isstring    (state,idx);}
           int    __fastcall  iscfunction    (int idx){return lua_iscfunction (state,idx);}
           int    __fastcall  isinteger      (int idx){return lua_isinteger   (state,idx);}
           int    __fastcall  isuserdata     (int idx){return lua_isuserdata  (state,idx);}
           bool   __fastcall  isnil          (int idx){return lua_isnil (state,idx) ? true : false;}
           bool   __fastcall  isnone         (int idx){return lua_isnone(state,idx) ? true : false;}
           int    __fastcall  type           (int idx){return lua_type        (state,idx);}
    const char*   __fastcall  type_name      (int tp) {return lua_typename    (state,tp);}

    lua_Number    __fastcall  tonumberx      (int idx, int *isnum){return lua_tonumberx  (state,idx,isnum);}
    lua_Integer   __fastcall  tointegerx     (int idx, int *isnum){return lua_tointegerx (state,idx,isnum);}
    int           __fastcall  toboolean      (int idx){return lua_toboolean (state,idx);}
    const char*   __fastcall  tolstring      (int idx, size_t *len){return lua_tolstring (state,idx,len);}
    size_t        __fastcall  rawlen         (int idx){return lua_rawlen      (state,idx);}
    lua_CFunction __fastcall  tocfunction    (int idx){return lua_tocfunction (state,idx);}
    void *        __fastcall  touserdata     (int idx){return lua_touserdata  (state,idx);}
    lua_State *   __fastcall  tothread       (int idx){return lua_tothread    (state,idx);}
    const void*   __fastcall  topointer      (int idx){return lua_topointer   (state,idx);}


    template <typename T ,  typename R  >
          R     __fastcall  push           (const T val);

    template <typename T ,  typename R  >
          R     __fastcall  pop            (T & val);


          void    __fastcall  global_set     (const char * name){lua_setglobal(state,name);}
          template <typename T>
          void    __fastcall  global_set     (const char * name, T val){push<T,void>(val);global_set(name);}
          int     __fastcall  global_get     (const char * name){ return lua_getglobal(state,name);}
          template <typename T>
          int     __fastcall  global_get     (const char * name, T & t)
          {
           int type = global_get(name);
           pop<T,int>(t);
           return type;
          }


          void    __fastcall reg_function     (const char * name ,lua_CFunction func) { lua_register(state,name,func);    }
          void    __fastcall reg_closure      (const char * name ,lua_CFunction func,int n) { lua_pushcclosure(state,func,n); global_set(name); }
          int     __fastcall getstack         (int level,lua_Debug * _dbg) { return lua_getstack(state,level,_dbg);}
          bool    __fastcall is_assigned      () {return state ? true : false;}
          int     __fastcall garbage_collect  (int w,int d){return lua_gc(state,w,d);}

         void __fastcall open_libs(unsigned what = (-1&(~LUA_LIB_TYPE_OS)))
            {
               ::luaL_openlibs(state);
               return;
               /*
               if(what&LUA_LIB_TYPE_BASE)
                luaopen_base     (state );
               if(what&LUA_LIB_TYPE_COROUTINE)
                luaopen_coroutine(state );
               if(what&LUA_LIB_TYPE_TABLE)
                luaopen_table    (state );
               if(what&LUA_LIB_TYPE_IO  )
                luaopen_io       (state );
               if(what&LUA_LIB_TYPE_OS  )
                luaopen_os       (state );
               if(what&LUA_LIB_TYPE_STRING)
                luaopen_string   (state );
               if(what&LUA_LIB_TYPE_UTF8)
                luaopen_utf8     (state );
               if(what&LUA_LIB_TYPE_BIT32)
                luaopen_bit32    (state );
               if(what&LUA_LIB_TYPE_MATH)
                luaopen_math     (state );
               if(what&LUA_LIB_TYPE_DEBUG)
                luaopen_debug    (state );
               if(what&LUA_LIB_TYPE_PACKEGE)
                luaopen_package  (state );
                */
            }


      void   __fastcall enable_bin_functions();
      private:
      lua_State   * state;
      bool          need_release;
      static    int lua_bin_and (lua_State * L);
      static    int lua_bin_or  (lua_State * L);
      static    int lua_bin_xor (lua_State * L);
      static    int lua_bin_not (lua_State * L);
      static    int lua_bin_shl (lua_State * L);
      static    int lua_bin_shr (lua_State * L);

    };

inline void   __fastcall LUAInstance::enable_bin_functions()
    {
       reg_function("bin_and",lua_bin_and);
       reg_function("bin_or" ,lua_bin_or );
       reg_function("bin_xor",lua_bin_xor);
       reg_function("bin_not",lua_bin_not);
       reg_function("bin_shl",lua_bin_shl);
       reg_function("bin_shr",lua_bin_shr);
    }




    template <>
    inline  void     __fastcall  LUAInstance::push           (const lua_Integer v)
     {
       lua_pushinteger(state,v);
     }

    template <>
    inline  void     __fastcall  LUAInstance::push           (const int v)
     {
       lua_pushinteger(state,(lua_Integer)v);
     }


    template <>
    inline  int     __fastcall  LUAInstance::pop           ( lua_Integer & v)
     {
       int top = gettop();
       int isnum = 0;
       if(isinteger(top))
          {
           v = tointegerx(top,&isnum);
          }
       return isnum ;
     }

    template <>
    inline  int     __fastcall  LUAInstance::pop           ( lua_Number & v)
     {
       int top = gettop();
       int isnum = 0;
       if(isnumber(top))
          {
           v = tonumberx(top,&isnum);
          }
       return isnum ;
     }

    template <>
    inline   int   __fastcall  LUAInstance::pop           ( const char * & str )
     {
       int top = gettop();
       if(isstring(top))
          {
           size_t  len = 0;
           str = tolstring(top,&len);
           return len ;
          }
       return 0;
     }

    template <>
    inline   int   __fastcall  LUAInstance::pop           ( const void * & ptr )
     {
       int top = gettop();
       if(isuserdata(top))
          {
           ptr = touserdata(top);
           return true;
          }
       return false;
     }





    template <>
    inline  void     __fastcall  LUAInstance::push           (const bool v)
     {
       lua_pushboolean(state,(int)v);
     }

    template <>
    inline  void     __fastcall  LUAInstance::push           (const lua_Number v)
     {
       lua_pushnumber(state,v);
     }

    template <>
    inline  void     __fastcall  LUAInstance::push           (const lua_CFunction v)
     {
      lua_pushcclosure(state,v,0);
     }

    template <>
    inline  void     __fastcall  LUAInstance::push           (const char * str)
     {
       lua_pushstring(state,str);
     }

    template <>
    inline  void     __fastcall  LUAInstance::push           ( char * str)
     {
       lua_pushstring(state,str);
     }

    template <>
    inline  void     __fastcall  LUAInstance::push           (const void * ptr)
     {
       lua_pushlightuserdata(state,(void*)ptr);
     }

    template <>
    inline  void     __fastcall  LUAInstance::push           ( void * ptr)
     {
       lua_pushlightuserdata(state,(void*)ptr);
     }



    // template <>
    // void    __fastcall  LUAInstance::global_set     (const char * name,lua_Integer val)
    // {
    //   push(val);
    //   global_set(name);
    // }
    //
    // template <>
    // void    __fastcall  LUAInstance::global_set     (const char * name,lua_Number val)
    // {
    //   push(val);
    //   global_set(name);
    // }

inline  int __fastcall LUAInstance::do_string(const char * str)
    {
      int res = load_string(str);
      if(res == LUA_OK)  res = call(0);
      return res;
    }

} //end of namespace lua
#endif

