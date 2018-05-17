/*******************************************************************************
 *
 * Модуль    : gkmodule_loader
 * Описание  : Реализация загрузчика модуля
 * Автор     : Остапенко Д.В.
 * Версия    : 1
 * Создан    : 03.07.2002
 *
 ******************************************************************************/

#pragma hdrstop
#include <gkmodules.hpp>


  bool      __fastcall TGKModuleLoader::load_module  (const char    * dll_name)
  {
   bool ret = false;
   if(dll_name && *dll_name)
   {
    //wchar_t * _dll_name = new wchar_t[lstrlenA(dll_name)+1];
    std::wstring _dll_name(lstrlenA(dll_name)+1,ret);
    KeRTL::Ansi2Unicode(const_cast<std::wstring::pointer>(_dll_name.data()),dll_name);
    ret = load_module(_dll_name.data());

   }
   return ret;
  }

  bool      __fastcall TGKModuleLoader::load_module  (const wchar_t * dll_name)
  {
      bool ret = false;
      dll_proc1<DWORD,BOOL> spm;
      spm.assign(GetModuleHandleW(L"Kernel32.dll"),"SetSearchPathMode");
      if(spm.is_valid())
         spm(1); // SAFE_MODE
      module_dll = LoadLibraryW(dll_name);
      if(module_dll)
      {
        const char * entry_name;
        entry_name  = GKMODULE_ENTRY_NAME;
        if(!entry_main.assign(module_dll,entry_name))
          if(!entry_main.assign(module_dll,entry_name+1))
           {
             entry_name = GKMODULE_ENTRY_NAME_MS;
             entry_main.assign(module_dll,entry_name);
           }

        if(entry_main.is_valid())
           ret = true;
          else
          {
           unload_module();
           SetLastError(MERR_DLLNOT_MODULE);
          }
      }
      else
      {
       DWORD error = GetLastError();
       wchar_t text[MAX_PATH];
       KeRTL::GetErrorMessage(error,text,KERTL_ARRAY_COUNT(text),(HINSTANCE)NULL);
       MessageBoxW(NULL,text,L"Ошибка загрузки ядра",MB_ICONSTOP|MB_OK);
       //abort();
       SetLastError(error);
       ret = false;

      }
      return ret ;
  }

  void  __fastcall TGKModuleLoader::unload_module()
  {
         if(module_dll )
         {
          FreeLibrary(module_dll);
          module_dll   = NULL;
          entry_main.assign(NULL,NULL);
         }
  }

  DWORD WINAPI crh_expf(LPEXCEPTION_POINTERS exp)
  {
     TGKModule::report(RCM_EXCEPTION,(LPARAM)exp,(LPARAM)"Исключение при создании хендла");
     return EXCEPTION_EXECUTE_HANDLER;
  }

  GKHANDLE  __fastcall TGKModuleLoader::create_module(wchar_t * name ,LPARAM p)
  {
    GKHANDLE ret = NULL;
    if(name && find_gkhandleW(name))
       SetLastError(MERR_GKHANDLE_EXISTS);
     else
     {
      __try{
            ret = entry_main.is_valid() ? (GKHANDLE)entry_main(GKME_CMD_CREATE_HANDLE,LPARAM(name),p) : NULL;
           }
           __except(crh_expf(GetExceptionInformation()))
           {
            ret = NULL;
           }
     }
    return  ret;
  }


  

 
