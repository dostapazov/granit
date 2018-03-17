/*-----------------------------------------------------------------------------
 Unit Name: gktlmdb
 Author   : Остапенко Д.В.
 Purpose  :
 History  :
-----------------------------------------------------------------------------*/




#ifndef   _GKTLMDB_HPP
#define   _GKTLMDB_HPP


#include <gkmodules.hpp>
#include <gktlmdb.h>
#include <modemmer.h>
#include <otd_storage.hpp>



using namespace KeRTL;
using namespace std;
class changes_handler;

class otd_dblow:public otd_storage
{
 protected:
  TMrSw    locker;
 public:
 otd_dblow(){}
 bool __fastcall write_lock(DWORD to);
 bool __fastcall write_unlock();
 bool __fastcall read_lock(DWORD to);
 bool __fastcall read_unlock();
};


class TGKTlmDB:public TGKModule
{
 protected:
 TLMDB_SETTINGS      settings;
 TGKModuleInterface  modemmer;
 otd_dblow           main_db;


 WORD   __fastcall get_type(){return MT_TLMDB;}
 DWORD  __fastcall get_window_module (wchar_t * buf,DWORD bsz);
 void   __fastcall release(LPVOID);

 bool    __fastcall read_settings  ();
 bool    __fastcall write_settings ();
 DWORD   __fastcall start(DWORD reason,LPARAM p2);
 DWORD   __fastcall stop (DWORD reason);
 LRESULT __fastcall processing    (DWORD cmd,LPARAM p1,LPARAM p2);

 DWORD   __fastcall get_config_data(DWORD mask,LPVOID buf,DWORD bsz);
 bool    __fastcall set_config_data(DWORD mask,LPVOID buf,DWORD bsz);
 bool    __fastcall compare_config_data(DWORD mask,LPVOID buf,DWORD bsz,LPDWORD ch_mask,BOOL * restart);

 void     __fastcall oncm_settings(LPTLMDB_SETTINGS s,BOOL set);
 LRESULT  __fastcall handle_data  (LPMPROTO_HEADER mph,GKHANDLE src);
 public:
 TGKTlmDB();
};

inline bool __fastcall otd_dblow::write_lock(DWORD to)
{ return locker.LockForWrite(to);}

inline bool __fastcall otd_dblow::write_unlock()
{ return locker.WriteUnlock();}

inline bool __fastcall otd_dblow::read_lock(DWORD to)
{return locker.LockForRead(to);}

inline bool __fastcall otd_dblow::read_unlock()
{return locker.ReadUnlock();}


#endif
