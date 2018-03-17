#pragma hdrstop
#include "gktlmdb_mod.hpp"

void * operator new(size_t sz)
{
 return TGKTlmDB::mem_alloc(sz);
}

void * operator new [](size_t sz)
{
 return TGKTlmDB::mem_alloc(sz);
}

void operator delete( void * ptr)
{
  TGKTlmDB::mem_free(ptr);
}

void operator delete[]( void * ptr)
{
  TGKTlmDB::mem_free(ptr);
}



module_starter<TGKTlmDB> ms(MT_TLMDB);

LRESULT WINAPI module_main(DWORD cmd ,LPARAM p1,LPARAM p2)
{
  return ms(cmd,p1,p2);
//       LRESULT ret = NULL;
//       switch(cmd)
//       {
//        case GKME_CMD_GET_MODULETYPE: ret = MT_TLMDB;break;
//        case GKME_CMD_CREATE_HANDLE :
//        {
//         if(!find_gkhandleW(GKTLMDB_HANDLE_NAME_W))
//           {
//            TGKTlmDB * db = new TGKTlmDB;
//            ret  = (LRESULT)(*db)();
//            if(!ret)
//             delete db;
//           }
//        }
//        break;
//       }
//
//       return ret;
}




