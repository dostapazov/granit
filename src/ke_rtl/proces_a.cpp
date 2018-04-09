#include <kertl.hpp>

#ifndef _NO_NAMESPACE
 namespace KeRTL {
#endif

bool __fastcall TProcess::Create(const char * exename, const char * cmd_line ,
                                 const char * environment ,const char * curdir ,
                                 DWORD c_flags ,bool inherit ,
                                 LPSTARTUPINFOA stpi ,
                                 LPSECURITY_ATTRIBUTES proc_sa,
                                 LPSECURITY_ATTRIBUTES thread_sa)
  {
    STARTUPINFOA startup_info;
    ZeroMemory(&startup_info,sizeof(startup_info));
    if(stpi) memcpy(&startup_info,stpi,sizeof(startup_info));
    startup_info.cb = sizeof(startup_info);
    PROCESS_INFORMATION pi;
    bool ret = CreateProcessA((char*)exename,(char*)cmd_line,proc_sa,thread_sa,
                             inherit? TRUE:FALSE,c_flags,
                             (char*)environment,(char*)curdir,&startup_info,&pi)
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
