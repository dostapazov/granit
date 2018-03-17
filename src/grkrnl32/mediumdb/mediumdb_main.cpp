#pragma hdrstop
#include "Mediumdb.hpp"


void * operator new(size_t sz)
{
  return TMediumDB::mem_alloc(sz);
}

void * operator new [](size_t sz)
{
 return TMediumDB::mem_alloc(sz);
}

void operator delete( void * ptr)
{
 TMediumDB::mem_free(ptr);
}


void operator delete[]( void * ptr)
{
 TMediumDB::mem_free(ptr);
}


#pragma warn -8057
static module_starter<TMediumDB> ms(MT_MEDIUMDB);
LRESULT WINAPI module_main(DWORD cmd,LPARAM p1,LPARAM p2)
{


       return ms(cmd,p1,p2);
}
#pragma warn .8057

