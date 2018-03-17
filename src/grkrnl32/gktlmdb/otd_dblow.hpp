/*
 Класс ОТД базы данных
 Thread sync : моного читателей - один писатель
 #define FASTMUTEX_LOCKER Для блокировки критической секцией в случае
 непоняток с TMrSw

   Данные храняться в виде протокола разобранного на части sotd_proto
   Упорядочены в порядке возрастания адресов
   ПУ 10 КП 1   ФА 0(ТС ) модуль 1
   ПУ 10 КП 1   ФА 1(ТИТ) модуль 1
   ПУ 10 КП 1   ФА 255    Модуль 255  - описатель КП с именем и диагностикой КП
   ПУ 10 КП 255 ФА 255    Модуль 255  - описатель ПУ с именем и диагностикой ПУ

   Обработка  подразделяется на  обработку одиночную и групповую

   1) Одиночная Если пришли данные
*/

#include <otd_storage.hpp>
#include <map>
#include <modem_proto.h>
#include <kestrm.hpp>


#ifndef FASTMUTEX_LOCKER
#define FASTMUTEX_LOCKER 1
#endif

using namespace KeRTL;
using namespace std;


class otd_dblow:protected otd_storage
{
  protected:
  #if( FASTMUTEX_LOCKER != 0)
  TFASTMutex  locker;
  #else
  TMrSw       locker;
  #endif
  virtual void    __fastcall release();
  virtual void    __fastcall delete_sop  (proto_pointer ptr);
  virtual
  proto_pointer   __fastcall create      (otd_addr & addr,otd_proto_param & src_opp);
  virtual
  proto_pointer   __fastcall create_group(otd_addr & addr);
  virtual DWORD   __fastcall handle_sb   (otd_proto & op,LPDWORD lo_ch,LPDWORD hi_ch);
  virtual DWORD   __fastcall handle_sb   (proto_pointer ptr,otd_proto & src,__int64 chtime,LPDWORD lo_ch,LPDWORD hi_ch);
  virtual DWORD   __fastcall handle_group(otd_proto & op,LPDWORD lo_ch,LPDWORD hi_ch);
  virtual DWORD   __fastcall handle_ctrl (otd_proto & op,modem_addr & from,LPDWORD,LPDWORD);
  virtual DWORD   __fastcall handle_diag (proto_pointer ptr,DWORD diag,LPDWORD lo_ch,LPDWORD hi_ch);
  virtual DWORD   __fastcall handle_name (proto_pointer ptr,otd_block & name);

          DWORD   __fastcall get_diag      (proto_pointer  ptr,bool rescan);
          DWORD   __fastcall get_diag      (otd_addr & addr);
          DWORD   __fastcall set_pu_diag   (proto_pointer ptr, DWORD diag,bool set);
          DWORD   __fastcall set_cp_diag   (proto_pointer ptr, DWORD diag,bool set);
          DWORD   __fastcall set_sb_diag   (proto_pointer ptr, DWORD diag,bool set);
          DWORD   __fastcall set_diag      (proto_pointer , DWORD diag,bool set);
          DWORD   __fastcall set_diag      (otd_addr & addr , DWORD diag,bool set);
          DWORD   __fastcall update_diag   (otd_addr & addr) ;
  virtual DWORD   __fastcall item_changed  (proto_pointer  ptr,DWORD ch_parts,DWORD lo_obj,DWORD hi_obj);
          DWORD   __fastcall recreate      (proto_pointer  ptr,DWORD lo_obj,DWORD hi_obj,wchar_t * name);
          DWORD   __fastcall make_static   (proto_pointer  prt,BOOL st);
          bool    __fastcall is_static     (proto_pointer ptr );
          DWORD   __fastcall get_owner_addr(otd_addr & addr  );
 proto_pointer    __fastcall get_owner     (proto_pointer ptr);
 bool             __fastcall is_dynamic    (proto_pointer ptr);


  public:
  explicit otd_dblow();
  ~otd_dblow(){release();}

          DWORD  __fastcall create       (otd_proto & src);
          DWORD  __fastcall items_enum   (otd_addr & addr,DWORD index);
          DWORD  __fastcall get_item     (DWORD offset,DWORD parts,DWORD lo,DWORD hi,LPBYTE buffer,DWORD buffer_sz);
          DWORD  __fastcall find_byaddr  (otd_addr & addr);

  virtual DWORD  __fastcall copy_from    (otd_addr  & mask,otd_storage & src,DWORD parts,DWORD lo,DWORD hi);
  virtual DWORD  __fastcall copy_to      (otd_addr  & mask,otd_storage & dest,DWORD parts,DWORD lo,DWORD hi);
  virtual DWORD  __fastcall remove       (otd_addr  & mask);
  virtual DWORD  __fastcall handle       (otd_proto & op,modem_addr & from,LPDWORD lo_ch,LPDWORD hi_ch);
          BOOL   __fastcall get_item     (otd_param_addr &opa,otd_param_value & pv);

          bool    __fastcall wr_lock     (DWORD time_out = INFINITE);
          bool    __fastcall wr_unlock   ();
          bool    __fastcall rd_lock     (DWORD time_out = INFINITE);
          bool    __fastcall rd_unlock   ();

  virtual DWORD   __fastcall stream_write  (TStream & strm);
  virtual DWORD   __fastcall stream_read   (TStream & strm,DWORD ver);
  class wr_locker
  {
    otd_dblow * obj;
    wr_locker(){}
    public:
    wr_locker(otd_dblow & _obj,DWORD to = INFINITE):obj(&_obj)
    {
      _obj.wr_lock(to);
    }
    ~wr_locker(){obj->wr_unlock();}

  };
};



#if (FASTMUTEX_LOCKER )
inline  bool __fastcall otd_dblow::wr_lock(DWORD time_out)
{ return locker.Lock();}

inline  bool __fastcall otd_dblow::wr_unlock()
{ return locker.Unlock();}

inline  bool __fastcall otd_dblow::rd_lock(DWORD time_out)
{ return locker.Lock();}

inline  bool __fastcall otd_dblow::rd_unlock()
{ return locker.Unlock();}

#else
inline  bool __fastcall otd_dblow::wr_lock(DWORD time_out)
{ return locker.LockForWrite(time_out);}

inline  bool __fastcall otd_dblow::wr_unlock()
{ return locker.WriteUnlock();}

inline  bool __fastcall otd_dblow::rd_lock(DWORD time_out)
{ return locker.LockForRead(time_out);}

inline  bool __fastcall otd_dblow::rd_unlock()
{ return locker.ReadUnlock();}



#endif

