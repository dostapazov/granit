/*
 KeRTL::TIoCompletionPort
 Ostapenko D.V.

*/

#include <kertlio.hpp>

#ifndef _NO_NAMESPACE
namespace KeRTL
{
#endif

  TIoCompletionPort::TIoCompletionPort(int MaxConcurentThreads ):THandleBased(0)
  {
   AppendFileHandle(HANDLE(-1),0,MaxConcurentThreads);
  }

  bool __fastcall TIoCompletionPort::AppendFileHandle(HANDLE hFile,DWORD CompletionKey,int MaxConcurentThreads)
  {
    HANDLE port =
    CreateIoCompletionPort(hFile,IsValidHandle(Handle) ? Handle : 0,CompletionKey,MaxConcurentThreads);
    if(IsValidHandle(port) && Handle!=port)
       Handle = port;
    return IsValidHandle(port);    
  }

  bool __fastcall TIoCompletionPort::GetStatus(ULONG_PTR &Key,LPOVERLAPPED & over,DWORD TimeOut ,LPDWORD TransferredBytes)
  {


   bool ret;
   DWORD _Bytes(0);
   ret  = ::GetQueuedCompletionStatus(Handle,&_Bytes,&Key,&over,TimeOut) ? true:false;

   if(TransferredBytes )
	  *TransferredBytes = _Bytes;
   return ret;   
  }

  bool __fastcall TIoCompletionPort::PostStatus(ULONG_PTR Key,DWORD Bytes,LPOVERLAPPED overlapped)
  {
   return PostQueuedCompletionStatus(Handle,Bytes,Key,overlapped) ? true:false;
  }


#ifndef _NO_NAMESPACE
//namespace KeRTL
}
#endif
