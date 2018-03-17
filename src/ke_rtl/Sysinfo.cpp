//Реализация TSysInfo
//Ke_rtl.dll Остапенко Д.В. March 2000
#include <kertl.hpp>

#ifndef _NO_NAMESPACE
namespace KeRTL
{
#endif

 TSysInfo si;

 OSVERSIONINFO TSysInfo::osvi = {0};
 SYSTEM_INFO   TSysInfo::si   = {0};


 TSysInfo::TSysInfo()
 {
  osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
  GetVersionEx(&osvi);
  GetSystemInfo(&si);
 }

 int __fastcall TSysInfo::GetMajorVersion   ()
 {return int(osvi.dwMajorVersion);}

   int __fastcall TSysInfo::GetMinorVersion   ()
 {return int(osvi.dwMinorVersion);}

   int __fastcall TSysInfo::GetBuildNumber    ()
 {return int(osvi.dwBuildNumber&(IsWin95() ? 0xFFFF : 0xFFFFFFFF));}


 bool  __fastcall TSysInfo::IsWinNT    ()
 {
  return bool(GetPlatform()==pWinNT);
 }

    bool  __fastcall TSysInfo::IsWin95 ()
 {return bool(GetPlatform()==pWin95);}

    bool  __fastcall TSysInfo::IsWin32s()
 {return bool(GetPlatform()==pWin32s);}

   TSysInfo::TPlatform __fastcall TSysInfo::GetPlatform()
 {  return TPlatform(osvi.dwPlatformId); }

#pragma warning(disable:858)
 const char  *  __fastcall TSysInfo::GetCSDVersion()
 {return ( char  * )osvi.szCSDVersion;}


 DWORD               __fastcall TSysInfo::GetPageSize         ()
 {return si.dwPageSize;}

 DWORD               __fastcall TSysInfo::GetProcessorCount   ()
 {return si.dwNumberOfProcessors;}
 DWORD               __fastcall TSysInfo::GetActiveProcessorMask ()
 {return si.dwActiveProcessorMask;}
  WORD               __fastcall TSysInfo::GetProcessorLevel   ()
 {return si.wProcessorLevel;}

  WORD               __fastcall TSysInfo::GetProcessorRevision()
 {return si.wProcessorRevision;}

 LPVOID              __fastcall TSysInfo::GetMinAppAddress    ()
 {return si.lpMinimumApplicationAddress;}

 LPVOID              __fastcall TSysInfo::GetMaxAppAddress    ()
 {return si.lpMaximumApplicationAddress;}

 DWORD               __fastcall TSysInfo::GetAllocationGranularity()
 {return si.dwAllocationGranularity;}

 TSysInfo::TProcessor   __fastcall TSysInfo::GetProcessorArchitecture()
 {return TProcessor(si.wProcessorArchitecture);}

 DWORD               __fastcall TSysInfo::GetProcessorType    ()
 {
  if(IsWin95())
    return si.dwProcessorType;
    return si.dwProcessorType;

 }

#ifndef _NO_NAMESPACE
}
#endif

