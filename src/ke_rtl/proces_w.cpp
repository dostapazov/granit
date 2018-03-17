#include <kertl.hpp>

#ifndef _NO_NAMESPACE
 namespace KeRTL {
#endif

bool __fastcall TProcess::Create(const wchar_t * exename, const wchar_t * cmd_line ,
                                 const wchar_t * environment ,const wchar_t * curdir ,
                                 DWORD c_flags ,bool inherit ,
                                 LPSTARTUPINFOW stpi ,
                                 LPSECURITY_ATTRIBUTES proc_sa,
                                 LPSECURITY_ATTRIBUTES thread_sa)
  {
    STARTUPINFOW startup_info;
    ZeroMemory(&startup_info,sizeof(startup_info));
    if(stpi) memcpy(&startup_info,stpi,sizeof(startup_info));
    startup_info.cb = sizeof(startup_info);
    PROCESS_INFORMATION pi;
    bool ret = CreateProcessW((wchar_t*)exename,(wchar_t*)cmd_line,proc_sa,thread_sa,
                             inherit? TRUE:FALSE,c_flags,
                             (wchar_t*)environment,(wchar_t*)curdir,&startup_info,&pi)
                             ? true:false;
    if(ret)
    {
     this->Handle         = pi.hProcess;
     this->process_thread = pi.hThread;
     this->process_id     = pi.dwProcessId;
     this->thread_id      = pi.dwThreadId;
    }
    return ret;
  }


#ifndef _NO_NAMESPACE
 }
#endif

 
