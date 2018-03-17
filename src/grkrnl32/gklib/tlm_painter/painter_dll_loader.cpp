#pragma hdrstop
#include <tlm_painter.hpp>
#include <otd_diag.h>

namespace tlmpaint
{
          TGKModuleLoader  painter_dll::dll;
    painter_dll * painter_dll::instance = NULL;

    painter_dll * __fastcall painter_dll::get_instance()
    {
       if(!instance)
           instance = new painter_dll;
       return instance;
    }

    void    __fastcall  painter_dll::release_instance ()
    {
      if(instance)
        {
          instance->unload_library();
          delete instance;
          instance = NULL;
        }
    }


    bool   __fastcall painter_dll::load_library(wstring & _lib_name)
    {
      if(!dll.is_loaded() || lib_name != _lib_name )
      {
        if(dll.is_loaded())
          dll.unload_module();
       lib_name = _lib_name;
       dll.load_module(lib_name.c_str());
      }
      return dll.is_loaded();
    }

    bool   __fastcall painter_dll::unload_library()
    {
     if(dll.is_loaded())
        dll.unload_module();
      return true;
    }

    LRESULT    __fastcall  painter_dll::call(DWORD cmd,LPARAM p1,LPARAM p2)
    {
      if(!dll.is_loaded())
         {
           wstring default_lib_name(L"tlmobj_painter.dll");
           load_library(default_lib_name);
         }
       return dll.module_entry(cmd,p1,p2);
    }





}//end of namespace



