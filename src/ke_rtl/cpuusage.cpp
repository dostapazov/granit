
#include <kertl.hpp>
//#include <dll_procs.hpp>
#include <winternl.h>

//static dll_proc4<DWORD,PVOID,ULONG,PULONG,DWORD> NtQuerySystemInformation(::GetModuleHandle("ntdll.dll"),"NtQuerySystemInformation");

bool   __fastcall GetCPUTimes(LPDWORD  idle_time,LPDWORD  user_time,LPDWORD  kernel_time)
{
    static SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION lastInfo = {0};
    SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION curInfo = {0};
    DWORD retsize;
    if(NtQuerySystemInformation(SystemProcessorPerformanceInformation,&curInfo, sizeof(curInfo), &retsize))
     return false;
    if(idle_time)
     *idle_time   = NS100_MSEC(curInfo.IdleTime.QuadPart   - lastInfo.IdleTime.QuadPart);
    if(kernel_time)
     *kernel_time = NS100_MSEC(curInfo.KernelTime.QuadPart - lastInfo.KernelTime.QuadPart);
    if(user_time)
     *user_time   = NS100_MSEC(curInfo.UserTime.QuadPart   - lastInfo.UserTime.QuadPart);
     CopyMemory(&lastInfo ,&curInfo,sizeof(lastInfo));

     PSYSTEM_PROCESS_INFORMATION  spi;
     BYTE buffer[128000] ={0};
     spi =(PSYSTEM_PROCESS_INFORMATION)buffer;
     retsize = GetCurrentProcessId();
     spi->UniqueProcessId = (PVOID) retsize;
     spi->NextEntryOffset = 248;
     NtQuerySystemInformation(SystemProcessInformation,buffer, sizeof(buffer), &retsize);
     spi =(PSYSTEM_PROCESS_INFORMATION)buffer;
     while(spi->NextEntryOffset)
           spi = (PSYSTEM_PROCESS_INFORMATION)(((LPBYTE)spi)+spi->NextEntryOffset);

    return true;
}

double  __fastcall CPUUsage   (DWORD & idle_time,DWORD & user_time,DWORD & kernel_time)
{
 DWORD total_time = user_time+kernel_time;
 double ret = .0;
 if(total_time)
    ret = 100.0-100.0*double(idle_time)/double(total_time);
 return ret;
}

double  __fastcall CPUUsage   ()
{
 DWORD  it,ut,kt;
 if(GetCPUTimes(&it,&ut,&kt))
    return  CPUUsage(it,ut,kt);
 return .0;
}



 
