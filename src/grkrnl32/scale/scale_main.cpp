//---------------------------------------------------------------------------
#pragma hdrstop
#include <windows.h>
#include "scale.hpp"

#pragma argsused

static THeap heap(TSysInfo().GetPageSize()<<8);
static DWORD total_mem_alloc = 0;

void * operator new(size_t sz)
{
  void * ptr = heap.Alloc(sz);
  total_mem_alloc+= heap.MemSize(ptr,0);
  return ptr;
}

void * operator new [](size_t sz)
{
  void * ptr = heap.Alloc(sz);
  total_mem_alloc+= heap.MemSize(ptr,0);
  return ptr;
}

void operator delete( void * ptr)
{
  if(ptr && heap.Validate(ptr))
  {
    total_mem_alloc-= heap.MemSize(ptr,0);
    heap.Free(ptr);
  }
}

void operator delete[]( void * ptr)
{
  if(ptr && heap.Validate(ptr))
  {
    total_mem_alloc-= heap.MemSize(ptr,0);
    heap.Free(ptr);
  }
}


//---------------------------------------------------------------------------
LRESULT WINAPI module_main(DWORD cmd,LPARAM p1,LPARAM p2)
{
  LRESULT ret = 0;
  switch(cmd)
  {
    case GKME_CMD_GET_MODULETYPE:
      ret = MT_SCALE; break;
    case GKME_CMD_CREATE_HANDLE :
    {
      TGKScale * scale =new TGKScale;
      ret  = (LRESULT)(*(TGKHandleBased*)(scale))();
      if(!ret)
        delete scale;
    }
    break;
  }
  return ret;
}

