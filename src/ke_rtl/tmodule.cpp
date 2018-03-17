#include <kertl.hpp>
#pragma hdrstop

namespace KeRTL
{
  FARPROC    __fastcall TModule::GetProcAddress(const wchar_t * proc_name)
  {
    char ansi_name[MAX_PATH];
    *(ansi_name +
    WideCharToMultiByte(CP_ACP,0,
                        proc_name,lstrlenW(proc_name)+1,
                        ansi_name,sizeof(ansi_name),0,0)
     )=0;
     return GetProcAddress(ansi_name);                   
  }
}


 
