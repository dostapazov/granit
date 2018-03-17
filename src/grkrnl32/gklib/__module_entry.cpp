#pragma hdrstop
#include <gkmodules.hpp>



HINSTANCE   ModuleInstance;

__declspec(dllexport) LRESULT WINAPI gkmodule_entry(DWORD cmd,LPARAM p1, LPARAM p2)
{
 return module_main(cmd,p1,p2);
}




extern "C" BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fwdreason, LPVOID /*lpvReserved*/)

{
   if(fwdreason == DLL_PROCESS_ATTACH)
   {
     ModuleInstance = hinstDLL;
     DisableThreadLibraryCalls(hinstDLL);
     TGKModule::init_instance(hinstDLL);
   }
   if(fwdreason == DLL_PROCESS_DETACH)
    {
      module_main(GKME_CMD_UNLOAD_MODULE,0,0);
    }
   return 1;
}

extern "C" BOOL WINAPI DllEntryPoint(HINSTANCE hinstDLL, DWORD fwdreason, LPVOID lpvReserved)
{
  return DllMain(hinstDLL,fwdreason,lpvReserved);
}






#ifdef _MSC_VER

extern "C" int __DllMainCRTStartup(int i1,int i2,int i3)
{

        ModuleInstance = (HINSTANCE)i1;
        DllMain(ModuleInstance,i2,(LPVOID)i3);
 return  1;//DllMain((HINSTANCE)i1,(DWORD)i2,(LPVOID)i3);
}


#endif

