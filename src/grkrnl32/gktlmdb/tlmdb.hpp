#ifndef __TLMDB_HPP
#define __TLMDB_HPP

#include <KeStrm.hpp>
#include <otd.h>
#include <otd_proto.h>
#include <kedatas.hpp>
#include <otd_storage.hpp>


using namespace KeRTL;
using namespace std;

#define TLMSTRM_CURRVER 1


/*Базовый класс*/

class tlm_base
{
 protected:
 DWORD      alloc_sz;
 LPVOID     alloc_ptr;
 otd_proto  op;
         bool   __fastcall create (otd_proto_param & opp);
 public:
          tlm_base():alloc_sz(0),alloc_ptr(NULL){}
 virtual ~tlm_base(){release();}
         bool   __fastcall get_param(otd_proto_param & opp);
         DWORD  __fastcall get_addr()const {return alloc_ptr ? op.addr->addr:0;}
         DWORD  __fastcall get_data(LPVOID buff,DWORD bz,lpotd_proto_param  opp = NULL);
 virtual DWORD  __fastcall handle_data (lpotd_proto op,LPVOID ext_param) = 0;
 virtual void   __fastcall release(){if(alloc_ptr) delete [] (char*)alloc_ptr;alloc_sz = 0;}
 virtual DWORD  __fastcall strm_version(){return TLMSTRM_CURRVER;}
 virtual DWORD  __fastcall strm_write  (TStream & out);
 virtual DWORD  __fastcall strm_read   (TStream & in,DWORD ver);
 bool    operator <(const tlm_base & tb);
};

struct tlm_base_less
{
 sotd_addr addr;
 tlm_base_less(DWORD _addr = -1):addr(_addr){};
 bool operator()(const tlm_base & tb1,const tlm_base & tb2){return sotd_addr(tb1.get_addr())<sotd_addr(tb2.get_addr());}
 bool operator()(const tlm_base* tb1,const tlm_base* tb2){return sotd_addr(tb1? tb1->get_addr():addr.addr)<sotd_addr(tb2? tb2->get_addr():addr.addr);}
};


class tlm_module:public tlm_base
{
 protected:
    static __int64   curr_time;
    void   __fastcall init_diag();
    bool   __fastcall update_diag();
    bool   __fastcall set_diag(DWORD diag);
 public:
       tlm_module(){};
       bool   __fastcall create_discrete(DWORD addr,DWORD beg,DWORD count);
       bool   __fastcall create_analog (DWORD addr,DWORD beg,DWORD count,DWORD type = OTD_ANALOG_BYTE);
       DWORD  __fastcall handle_data (lpotd_proto op ,LPVOID ext_param);
static void   __fastcall prepare_handle();

};

class tlm_group:public tlm_base
{
 public:
 tlm_group(){};
 bool   __fastcall create(DWORD addr);
 DWORD  __fastcall handle_data (lpotd_proto op ,LPVOID ext_param);
 DWORD  __fastcall set_name(wchar_t * name);
 
 DWORD  __fastcall strm_write  (TStream & out);
 DWORD  __fastcall strm_read   (TStream & in,DWORD ver);
};

class tlm_list
{
 protected:
   vector<tlm_base*> list;
   TMrSw             locker;
         void   __fastcall release();
 public:
                           tlm_list(){};
 virtual                  ~tlm_list(){release();}

         DWORD  __fastcall insert(tlm_base*);
         tlm_base * __fastcall remove (const otd_addr & adr);
         tlm_base * __fastcall remove (const DWORD idx);
         tlm_base * __fastcall find   (const otd_addr &adr);
         DWORD      __fastcall get_idx(const otd_addr &adr);
         tlm_base * operator[](DWORD idx){return idx<list.size() ? list[idx] :NULL;}
         tlm_base * operator[](const otd_addr adr){return find(adr);}
         DWORD      __fastcall count(){return list.size();}
         bool __fastcall write_lock(bool lock, DWORD to = INFINITE);
         bool __fastcall read_lock (bool lock ,DWORD to = INFINITE);
};

 inline  bool __fastcall tlm_list::write_lock(bool lock,DWORD to )
 {return lock ? locker.LockForWrite(to):locker.WriteUnlock();}
 inline  bool __fastcall tlm_list::read_lock (bool lock,DWORD to )
 {return lock ? locker.LockForRead(to):locker.ReadUnlock();}




class tlm_db
{
 protected:
 tlm_list  module_list;//Cписок модулей
 tlm_list  spr_list;   //Список ПУ,КП
 DWORD     __fastcall write_list(TStream & out,tlm_list &);
 DWORD     __fastcall read_mod_list (TStream & in);
 DWORD     __fastcall read_spr_list (TStream & in);
 public:
  tlm_db(){};
 ~tlm_db(){};
 DWORD      __fastcall create (DWORD addr,int beg,int count,DWORD type = OTD_ANALOG_BYTE);
 tlm_base * __fastcall find(const otd_addr & addr);


 /*Функции обработки*/
 DWORD __fastcall handle_query(lpotd_proto op,TSimpleDataQueue * queue);
 DWORD __fastcall handle_data (lpotd_proto op);

 /*Запись / чтение в/из потока*/
 DWORD __fastcall strm_write(TStream & out);
 DWORD __fastcall strm_read (TStream & in);
};

#endif


