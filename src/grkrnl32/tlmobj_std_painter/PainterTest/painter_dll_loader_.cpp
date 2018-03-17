
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

    bool   __fastcall painter_dll::load_library(wstring & _lib_name)
    {
      if(lib_name != _lib_name )
      {
        if(dll.is_loaded())
        {
          dll.module_entry(GKME_CMD_UNLOAD_MODULE,0,0);
          dll.unload_module();
        }
       lib_name = _lib_name;
       dll.load_module(lib_name.c_str());
      }
      return dll.is_loaded();
    }

    int    __fastcall  painter_dll::call(DWORD cmd,LPARAM p1,LPARAM p2)
    {
      if(!dll.is_loaded())
           load_library(*&wstring(L"tlmobj_painter.dll"));
       return dll.module_entry(cmd,p1,p2);
    }





}//end of namespace



