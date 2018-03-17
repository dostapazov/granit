#include <KeRTL.hpp>

#ifndef _NO_NAMESPACE
 namespace KeRTL {
#endif


DWORD   __fastcall THeap::GetMemoryUsed()
{
 DWORD result = 0;
 PROCESS_HEAP_ENTRY he;
 ZeroMemory(&he,sizeof(he));
 while(HeapWalk(&he))
  {
   if(he.wFlags&PROCESS_HEAP_ENTRY_BUSY)
      result += he.cbData;
  }
 return result;  
}

#ifndef _NO_NAMESPACE
} //namespace KeRTL {
#endif

 
