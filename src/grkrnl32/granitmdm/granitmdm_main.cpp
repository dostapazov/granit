#pragma hdrstop
#include "granit_modem.hpp"
#include <alloc.h>

static THeap heap(TSysInfo().GetPageSize()<<8);
static DWORD total_alloc_size = 0;

void * operator new (size_t sz)
{
 void * ptr = NULL;

 if(heap.IsValid())
 {
  ptr = heap.Alloc(sz);
 }
 else
 ptr = malloc(sz);
 return ptr;
}

void * operator new [](size_t sz)
{
 void * ptr = NULL;

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
  if(ptr && heap.Validate(ptr))
    {
     heap.Free(ptr);
    }
    else
    {
    if(ptr && _USEDENTRY == heapchecknode(ptr)) free(ptr);
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

DWORD   __fastcall TSCGranitModem::get_mem_used()
{
 return heap.IsValid() ? heap.GetMemoryUsed() : GKH_RET_ERROR;
}

LRESULT WINAPI module_main(DWORD cmd,LPARAM /*p1*/,LPARAM /*p2*/)
{
  LRESULT ret = 0;
       switch(cmd)
       {
        case GKME_CMD_GET_MODULETYPE:
			ret = MT_MODEM;break;
        case GKME_CMD_CREATE_HANDLE :
        {
            TSCGranitModem * mod =new TSCGranitModem;
            ret  = (LRESULT)(*(TGKHandleBased*)(mod))();
            if(!ret)
             delete mod;
        }
        break;
       }
       return ret;
}

 
