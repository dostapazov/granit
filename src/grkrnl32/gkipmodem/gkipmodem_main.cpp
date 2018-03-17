#pragma hdrstop
#include "gkipmodem_mod.hpp"
#include <alloc.h>

static THeap heap;
static DWORD total_alloc_size = 0;

void * operator new [](size_t sz)
{
 void * ptr = NULL;
 if(!heap.IsValid()) heap.Create(TSysInfo().GetPageSize()<<8);
 if(heap.IsValid())
 {
  ptr = heap.Alloc(sz);
 }
 else
 ptr = malloc(sz);
 return ptr;
}

void operator delete( void * ptr)
{
  if(ptr && _USEDENTRY == heapchecknode(ptr))
  {
   __try{
     free(ptr);
   }
   __finally{}
   return;
  }
  if(ptr && heap.IsValid() && heap.Validate(ptr))
    {
     heap.Free(ptr);
    }
}

void operator delete[]( void * ptr)
{

  if(ptr && heap.Validate(ptr))
    {
     heap.Free(ptr);
    }
   else
   {
    if(ptr && _USEDENTRY == heapchecknode(ptr)) free(ptr);
   }
}


 DWORD       __fastcall TGkIPModem::get_mem_used()
 {
  return heap.IsValid() ? heap.GetMemoryUsed() : GKH_RET_ERROR;
  //total_alloc_size;
 }

#pragma warn -8057

module_starter<TGkIPModem> ms(MT_MODEM_TCP);

LRESULT WINAPI module_main(DWORD cmd,LPARAM p1,LPARAM p2)
{
  return ms(cmd,p1,p2);
//  LRESULT ret = 0;
//       switch(cmd)
//       {
//        case GKME_CMD_GET_MODULETYPE:
//			ret = MT_MODEM;break;
//        case GKME_CMD_CREATE_HANDLE :
//        {
//            TGkIPModem * mod =new TGkIPModem;
//            ret  = (LRESULT)(*(TGKHandleBased*)(mod))();
//            if(!ret)
//             delete mod;
//        }
//        break;
//       }
//       return ret;
}

 #pragma warn .8057



 
