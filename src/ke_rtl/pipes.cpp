#include <pipes.hpp>


#ifndef _NO_NAMESPACE
namespace KeRTL
{
#endif




 
  bool    __fastcall   TServerPipe::Open
                       (char * name,
                       DWORD OutSize,
                       DWORD InSize,
                       DWORD PipeMode   ,
                       DWORD OpenMode   ,
                       DWORD maxInst    ,
                       DWORD DefTimeOut ,
                       LPSECURITY_ATTRIBUTES sa )
{
  if(!IsValid())
  {
   Handle = CreateNamedPipeA(name,OpenMode,PipeMode,maxInst,OutSize,InSize,DefTimeOut,sa);
   LastError = GetLastError();

  }
  return IsValid();
}

  bool    __fastcall   TServerPipe::Open
                       (wchar_t * name,
                       DWORD OutSize,
                       DWORD InSize,
                       DWORD PipeMode   ,
                       DWORD OpenMode   ,
                       DWORD maxInst    ,
                       DWORD DefTimeOut ,
                       LPSECURITY_ATTRIBUTES sa )
{
  if(!IsValid())
  {
   Handle = CreateNamedPipeW(name,OpenMode,PipeMode,maxInst,OutSize,InSize,DefTimeOut,sa);
   LastError = GetLastError();
   
  }
  return IsValid();
}

 #pragma warn -8057
  bool    __fastcall   TClientPipe::Open(char * name,
					   DWORD Access,
					   DWORD ShareMode,
					   DWORD CreationDistribute,
					   DWORD Flags,
					   DWORD Reserved ,
					   DWORD DefTimeOut ,
					   LPSECURITY_ATTRIBUTES sa )

  {
   if(WaitNamedPipeA(name,DefTimeOut))
	 Handle = CreateFileA(name,Access,ShareMode,sa,CreationDistribute,Flags,0);
	 LastError = GetLastError();
	 return IsValid();

  }
 #pragma warn .8057

  bool    __fastcall   TClientPipe::Open(wchar_t * name,
					   DWORD Access,
					   DWORD ShareMode,
                       DWORD CreationDistribute,
                       DWORD Flags,
                       DWORD /*Reserved */,
                       DWORD DefTimeOut ,
                       LPSECURITY_ATTRIBUTES sa )

  {
   if(WaitNamedPipeW(name,DefTimeOut))
     Handle = CreateFileW(name,Access,ShareMode,sa,CreationDistribute,Flags,0);
     LastError = GetLastError();
     return IsValid();
    
  }

#ifndef _NO_NAMESPACE
}
#endif

