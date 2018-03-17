/*******************************************************************************
 *
 * Модуль    : grkernel
 * Описание  : Модуль ядра "Гранит-Н"
 * Автор     : Остапенко Д.В.
 * Версия    : 1
 * Создан    : 16.07.2002
 *
 ******************************************************************************/


#ifndef _GRKERNEL_INC_HPP_
#define _GRKERNEL_INC_HPP_

#include <GRKERNEL.H>
#include <GKMODULES.HPP>
#include <KeRTL.HPP>
#include <KeThreads.hpp>
#include <gkreporter.hpp>
#include <map>


using namespace KeRTL;

struct gktimer:public GKTIMER
{
 gktimer (){dw_size = sizeof(GKTIMER);tm_target = NULL;period = FALSE;};
 gktimer (const GKTIMER & );
 gktimer & operator = (const GKTIMER & );
 LRESULT operator()(DWORD id);
};

class TGKTimer:public TGKThread
{
 protected:
 typedef std::map<DWORD,gktimer> gktimer_list;
 TWaitableTimer      wt;
 TEvent              term_event;
 TFASTMutex          locker;
 __int64             resolution;
 DWORD               gen_id;
 gktimer_list        timers;
 DWORD   __fastcall  generate_tmid();
 int     __fastcall  Execute();
 void    __fastcall  Terminate();
 __int64 __fastcall  do_timers();
 const char  * __fastcall get_gkthread_name(){return "GKernel timer thread";}
 public:
 TGKTimer(DWORD res = 250):wt(false){resolution = MSEC_NS100(res);};
 DWORD   __fastcall set_timer   (LPGKTIMER);
 BOOL    __fastcall reset_timer (DWORD  id,__int64 * new_alert_time);
 BOOL    __fastcall cancel_timer(DWORD  id);
 BOOL    __fastcall get_timer   (DWORD  id,LPGKTIMER);
 BOOL    __fastcall start(DWORD resolution = 0);
 void    __fastcall clear();
 bool    __fastcall lock(bool try_lock = false);
 bool    __fastcall unlock();
};




struct report_names
{
 DWORD id;
 wchar_t * name;
 wchar_t * descr;
 report_names (){id = -1;name = descr = 0;}
 report_names (DWORD _id,wchar_t * _name,wchar_t * _descr):id(_id),name(newstr(_name)),descr(newstr(_descr)){}
 ~report_names(){if(name) delete[] name; if(descr) delete[] descr;}
};

struct report_names_less
{
 bool operator ()(const report_names & rn1 ,const report_names & rn2) {return rn1.id<rn2.id ?   true:false;}
 bool operator ()(const report_names * rn1 ,const report_names * rn2) {return rn1->id<rn2->id ? true:false;}
};

typedef vector<report_names*> TReports;

class TGRKernel:public TGKModulesSet
{
 HANDLE       terminate_event;
 GKHANDLE     reporter;
 TGKTimer     kernel_timer;
 TReports     reports;
 DWORD        license_timer_id;
 wchar_t      instance_name[MAX_PATH];
 std::wstring cwd;

 DWORD     __fastcall create_stdreporter();
 DWORD     __fastcall delete_stdreporter();
 void      __fastcall load_reporter   ();
 DWORD     __fastcall replace_reporter(wchar_t * image_path);

 WORD    __fastcall get_type  (){return MT_KERNEL;}
 bool    __fastcall check_module_type(WORD mt);
 LRESULT __fastcall processing(DWORD cmd,LPARAM p1,LPARAM p2);
 LRESULT __fastcall report_processing(DWORD cmd,LPARAM p1,LPARAM p2);

 BOOL    __fastcall can_start(DWORD reason,LPARAM p2);
 DWORD   __fastcall start(DWORD reason,LPARAM p2);
 DWORD   __fastcall stop (DWORD reason);

 bool    __fastcall init_module  (LPMODULE_INIT mi);
 void    __fastcall release      (LPVOID);

 DWORD   __fastcall get_config_data(DWORD mask,LPVOID buf,DWORD bsz);
 bool    __fastcall set_config_data(DWORD mask,LPVOID buf,DWORD bsz);
 bool    __fastcall compare_config_data(DWORD mask,LPVOID buf,DWORD bsz,LPDWORD ch_mask,BOOL * restart);


 LRESULT __fastcall on_notify_code(LPNOTIFY_CODE,DWORD mask);


 DWORD   __fastcall set_timer     (LPGKTIMER );
 DWORD   __fastcall reset_timer   (DWORD,__int64 * new_alert_time);
 DWORD   __fastcall cancel_timer  (DWORD);
 DWORD   __fastcall get_timer     (DWORD  id,LPGKTIMER);

 DWORD  __fastcall on_exception(lpexcept_record er);

 BOOL   __fastcall set_instance(wchar_t * _inst,DWORD len);
 DWORD  __fastcall get_instance(wchar_t * _inst,DWORD sz );
 virtual   DWORD   __fastcall check_license(){return MOD_LICENSE_REGISTERED; }

 protected:
 BOOL     __fastcall lock(DWORD to = INFINITE);
 BOOL     __fastcall unlock();
 void   __fastcall pre_load_modules();
 void   __fastcall start_reporter  ();
 void   __fastcall register_msg_type(DWORD id,wchar_t * name,wchar_t * descr);
 void   __fastcall register_msg_type(DWORD id,char    * name,char    * descr);
 DWORD  __fastcall get_mem_used   ();
 DWORD  __fastcall on_kernel_timer(DWORD timer_id);
 void   __fastcall trial_inc_time ();
 static int winapi trial_inc_proc (LPVOID ptr,GKHANDLE handle);
 virtual   DWORD   __fastcall license_inc_time(DWORD v){return -1;}

 public:
 TGRKernel();
 GKHANDLE __fastcall append_module(wchar_t * image_path,bool perm);
 bool     __fastcall remove_module(GKHANDLE h,bool erase = false);
 static TSysInfo   sys_info;
 static THeap      common_heap;

 static LPVOID  __fastcall alloc_mem     (DWORD sz);
 static LPVOID  __fastcall realloc_mem   (LPVOID ptr,DWORD new_sz);
 static BOOL    __fastcall free_mem      (LPVOID ptr);

};


inline bool    __fastcall TGRKernel::check_module_type(WORD mt)
{
 if(mt & MT_WINDOW)
    return false;   //Не окна
 if(mt &MT_MODEMMER)
    return mt == MT_MODEMMER ? true:false;//Только модеммер
 return true;
}

inline  DWORD   __fastcall TGRKernel::set_timer     (LPGKTIMER gktm)
{
 DWORD ret;
 unlock();
 ret = kernel_timer.set_timer(gktm);
 lock();
 return ret;
}

inline  DWORD   __fastcall TGRKernel::reset_timer   (DWORD id,__int64 * new_alert_time)
{
 DWORD ret;
 unlock();
 ret = kernel_timer.reset_timer(id,new_alert_time) ? GKH_RET_SUCCESS:GKH_RET_ERROR;
 lock();
 return ret;
}

inline  DWORD   __fastcall TGRKernel::cancel_timer  (DWORD id)
{
 DWORD ret;
 unlock();
 ret = kernel_timer.cancel_timer(id) ? GKH_RET_SUCCESS:GKH_RET_ERROR;
 lock();
 return ret;
}

inline  DWORD   __fastcall TGRKernel::get_timer     (DWORD  id,LPGKTIMER tm)
{
 DWORD ret;
 unlock();
 ret = kernel_timer.get_timer(id,tm) ? GKH_RET_SUCCESS:GKH_RET_ERROR;
 lock();
 return ret;
}




inline gktimer::gktimer (const GKTIMER & tm)
{  *this = tm; }

inline LRESULT gktimer::operator()(DWORD id)
{  return call_gkhandle(tm_target,tm_cmd,tm_p1,id); }

inline void    __fastcall TGKTimer::clear()
{
 TerminateAndWait(INFINITE);
 lock();
 timers.clear();
 unlock();
}

inline  bool    __fastcall TGKTimer::lock(bool try_lock)
{
  return locker.Lock(try_lock);
}

inline  bool    __fastcall TGKTimer::unlock()
{
 return locker.Unlock();
}



#endif

