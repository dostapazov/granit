#include <kertl.hpp>
#include <dll_procs.hpp>


extern "C" int WINAPI wWinMain(::HINSTANCE hi1,::HINSTANCE hi2, TCHAR * cmd_line ,int cmd_show)
{
  dll_stdc_proc4<HINSTANCE,HINSTANCE,TCHAR*,int>  kernel_main;
  KeRTL::TModule mod;
  if(mod.Load(L"gklaunch32.dll"))
    {
      kernel_main.assign(mod(),"kernel_main");
      if(kernel_main.is_valid())
         return kernel_main(hi1,hi2,cmd_line,cmd_show);
    }
   return GetLastError();
}

