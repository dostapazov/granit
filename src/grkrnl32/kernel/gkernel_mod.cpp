/*******************************************************************************
 *
 * Модуль    : gkernel
 * Описание  : Реализация Ядра "Гранит-Н"
 * Автор     : Остапенко Д.В.
 * Версия    : 1
 * Создан    : 17.07.2002
 *
 ******************************************************************************/


#include "gkernel_mod.hpp"
#include <gkreporter.h>
#include <tregstry.hpp>
#include <algorithm>
#include <KeRTLio.hpp>
#include <kfname.hpp>


#if _MSC_VER >=1200
#ifndef _DEBUG
#pragma comment(linker,"/opt:nowin98")
#endif
#endif

 DWORD  winapi GK_global_except_filter (lpexcept_record er)
 {
  TGKModule::report(RCM_GKH_EXCEPTION,(LPARAM)er,(LPARAM)"GRKernel global Exception filter");
  return GKH_RET_EXCEPTION;
 }

 TSysInfo    TGRKernel::sys_info;
 THeap       TGRKernel::common_heap;

 LPVOID  __fastcall TGRKernel::alloc_mem(DWORD sz)
{
 if(!common_heap.IsValid())
	 common_heap.Create(sys_info.GetPageSize()<<8);

 LPVOID ret =  common_heap.Alloc(sz);
 if(ret)
 {
	LONG mem_sz =  common_heap.MemSize(ret,0);
 }
 return ret;
}

LPVOID  __fastcall TGRKernel::realloc_mem(LPVOID ptr,DWORD new_sz)
{
 if(common_heap.IsValid())
 {
 LONG mem_sz = 0;
  LPVOID ret =  common_heap.Realloc(ptr,new_sz);
  return ret;
 }
 return NULL;
}

BOOL    __fastcall TGRKernel::free_mem   (LPVOID ptr)
{
 if(ptr && common_heap.IsValid() && common_heap.Validate(ptr,0))
 {
   return common_heap.Free(ptr);
 }
 return FALSE;
}


// void * operator new(size_t sz)
// {
//  return TGRKernel::alloc_mem(sz);
// }
// void * operator new [] (size_t sz)
// {
//  return TGRKernel::alloc_mem(sz);
// }
//
// void operator delete (void * ptr)
// {
//  TGRKernel::free_mem(ptr);
// }
//
// void operator delete [] (void * ptr)
// {
//   TGRKernel::free_mem(ptr);
// }


 TGRKernel::TGRKernel()
 {
  reporter = 0;
  lock_param = GKHB_AUTO_LOCK_OFF;//Убрать автоблокировку
  terminate_event = 0;
  alloc_gkhandle(GRKERNEL_HANDLE_NAME);
  set_gkhandle_exception_filter(GK_global_except_filter);
  license_timer_id = -1;
 }


 BOOL     __fastcall TGRKernel::lock(DWORD to)
 {
   BOOL ret = TGKModulesSet::lock(to);
   return ret;
 }

 BOOL     __fastcall TGRKernel::unlock()
 {
   BOOL ret = TGKModulesSet::unlock();
   return ret;
 }

bool    __fastcall TGRKernel::init_module  (LPMODULE_INIT mi)
{
  return TGKModule::init_module(mi);
}


void    __fastcall TGRKernel::release      (LPVOID release_var)
{
  TReports::iterator  rep_beg = reports.begin(),rep_end = reports.end();
  while(rep_beg<rep_end)
  {
   delete (*rep_beg);
   rep_beg++;
  }
  reports.erase(reports.begin(),rep_end);
  TGKModulesSet::release(release_var);
}


LRESULT __fastcall TGRKernel::processing(DWORD cmd,LPARAM p1,LPARAM p2)
{
 LRESULT ret = GKH_RET_ERROR;
 switch(cmd)
 {
  case CMGK_ALLOC_MEM        : ret = (LRESULT) alloc_mem((DWORD)p1);break;
  case CMGK_REALLOC_MEM      : ret = (LRESULT) realloc_mem(LPVOID(p1),DWORD(p2));break;
  case CMGK_FREE_MEM         : ret = (LRESULT) free_mem(LPVOID(p2)) ? GKH_RET_SUCCESS:GKH_RET_ERROR ;break;
  case CMGK_SET_TIMER        : if(check_data_size((LPVOID)p1,sizeof(GKTIMER)))
                                  ret = set_timer((LPGKTIMER)p1);
                                  break;
  case CMGK_RESET_TIMER      : ret = reset_timer ((DWORD)p1,(__int64*)p2);break;
  case CMGK_CANCEL_TIMER     : ret = cancel_timer((DWORD)p1);break;
  case CMGK_GET_TIMER        : if(check_data_size((LPVOID) p2,sizeof(GKTIMER)))
                                  ret = get_timer(p1,(LPGKTIMER)p2);
                                  break;
  case CMGK_ONTIMER          : ret = on_kernel_timer(p1);break;
  case CMGK_CREATE_STDREPORTER:
  case CMGK_DELETE_SDTREPORTER:
                               wchar_t name[MAX_PATH<<1];
                               get_gkhandle_nameW((GKHANDLE)p1,name,sizeof(name)/sizeof(wchar_t));
                               if (wcscmp(name,REPORTEREX_NAME_W)==0)
                                 ret=((cmd==CMGK_CREATE_STDREPORTER)?create_stdreporter():delete_stdreporter());
                               break;
  case MCMD_APPEND_MODULE    : ret =(LRESULT)append_module((wchar_t*)p1,p2 ? true:false); break;
  case CMGK_SET_INSTANCE     : ret = set_instance((wchar_t * )p1,(DWORD) p2) == TRUE ? GKH_RET_SUCCESS : GKH_RET_ERROR ;
                               break;
  case CMGK_GET_INSTANCE     : ret = get_instance((wchar_t * )p1,(DWORD) p2);
                               break;
  default                    :
                               ret = (MCMD_TYPE(cmd)==MT_REPORTER) ? report_processing(cmd,p1,p2):TGKModulesSet::processing(cmd,p1,p2);
 }
 return ret;
}


LRESULT __fastcall TGRKernel::report_processing(DWORD cmd,LPARAM p1,LPARAM p2)
{
 LRESULT ret = GKH_RET_ERROR;
 if(reporter)
 {
   ret = call_gkhandle(reporter,cmd,p1,p2);
 if(cmd == RCM_REGISTER_MSG_TYPE_A)
     register_msg_type(ret,(char*)p1,(char*)p2);
  if(cmd == RCM_REGISTER_MSG_TYPE_W)
     register_msg_type(ret,(wchar_t*)p1,(wchar_t*)p2);
 }
 return ret;
}



void __fastcall TGRKernel::pre_load_modules()
{
  TGKModulesSet::pre_load_modules();
}

//bool __fastcall check_valid_copy(bool as_service);

#pragma warn -8057

BOOL    __fastcall TGRKernel::can_start(DWORD reason,LPARAM p2)
{
   load_reporter();
   start_reporter();
   report_processing(RCM_REGISTER_MSG_TYPE_W,(LPARAM)L"GKSystem",(LPARAM)L"События системы");
   report_processing(RCM_REGISTER_MSG_TYPE_W,(LPARAM)L"Exceptions",(LPARAM)L"Исключения");
   BOOL  ret = TGKModulesSet::can_start(reason,p2);
   license_type = MOD_LICENSE_REGISTERED;
   return ret;
}

DWORD   __fastcall TGRKernel::start(DWORD reason,LPARAM p2)
{
 //Не нулевой p2 указывает о том , что приложение запущено
 //не как служба


  if(reason!=MODULE_START_RECONFIGURE)
     {
      kernel_timer.start(0);
     }

  if(reason == MODULE_START_PROGRAM_LAUNCH)
  {
   if(!cwd.length())
     {
        int cwd_size = GetCurrentDirectoryW(0,NULL);
        cwd.resize(cwd_size+1);
        GetCurrentDirectoryW(cwd_size,const_cast<wchar_t*>(cwd.data()));
     }
   load_modules();
   report(EVENT_SYSTEM,REPORT_INFORMATION_TYPE,p2 ?L"Запуск ядра в режиме службы " : L"Запуск ядра");
   terminate_event     = CreateEventW(0,TRUE,FALSE,GRKERNEL_TERMINATE_EVENT);
  }

//     GKTIMER tm;
//     ZeroMemory(&tm,sizeof(tm));
//     tm.dw_size    =  sizeof(tm);
//     tm.tm_target  =  this->_handle;
//     tm.alert_time = GetTime()+MSEC_NS100(10*1000);
//     kernel_timer.set_timer(&tm);
//     tm.alert_time = -MSEC_NS100(2000);
//     tm.period     = TRUE;
//     kernel_timer.set_timer(&tm);


  TGKModulesSet::start(reason,p2);
 if(license_timer_id==(DWORD)-1)
  {
//      GKTIMER tmr;
//	  ZeroMemory(&tmr,sizeof(tmr));
//	  tmr.dw_size = sizeof(tmr);
//	  tmr.tm_target  = _handle;
//	  tmr.tm_cmd     = CMGK_ONTIMER;
//	  tmr.tm_p1      = TIMER_ID_TRIAL;
//	  tmr.alert_time = -MSEC_NS100(1000);
//	  tmr.period     = TRUE;
//	  license_timer_id = kernel_timer.set_timer(&tmr);
  }

  return GKH_RET_SUCCESS;
}

#pragma warn .8057

DWORD   __fastcall TGRKernel::stop (DWORD reason)
{
 report(EVENT_SYSTEM,REPORT_INFORMATION_TYPE,L"Завершение работы ядра");
 SetEvent(terminate_event);
 //останавливаем репортер последним
 if(license_timer_id!=(DWORD)-1)
  {
	 kernel_timer.cancel_timer(license_timer_id);
	 license_timer_id = -1;
  }

 if(reason !=MODULE_STOP_RECONFIGURE)
 {
      kernel_timer.TerminateAndWait(INFINITE);
      kernel_timer.clear();
 }

 TGKModuleInterface * rep_iface = modules_list.at(modules_list.get_interface_index(reporter));
 if(rep_iface)
    modules_list.remove(rep_iface);

 DWORD ret=TGKModulesSet::stop(reason);
 ResetEvent(terminate_event);

 if(reason == MODULE_STOP_PROGRAM_SHUTDOWN)
 {

   //DWORD lock_cnt = full_unlock();
   unload_modules();
   if(terminate_event)
    CloseHandle(terminate_event);
   if(rep_iface)
      {
       rep_iface->stop(reason);
       free_interface(rep_iface);
      }
   //this->restore_lock(lock_cnt);
 }
 else
  modules_list.add(rep_iface);

 return ret;
}

#pragma warn -8057

DWORD   __fastcall TGRKernel::get_config_data(DWORD mask,LPVOID buf,DWORD bsz)
{
 return GKH_RET_SUCCESS;
}

bool    __fastcall TGRKernel::set_config_data(DWORD mask,LPVOID buf,DWORD bsz)
{
   return  true;
}

bool    __fastcall TGRKernel::compare_config_data(DWORD mask,LPVOID buf,DWORD bsz,LPDWORD ch_mask,BOOL * restart)
{
   if(ch_mask) *ch_mask = 0; if(restart) * restart = FALSE;
   return false;
}

#pragma warn .8057

LRESULT __fastcall TGRKernel::on_notify_code(LPNOTIFY_CODE,DWORD )
{
 return 0;
}


DWORD    __fastcall      TGRKernel::on_exception(lpexcept_record er)
{
  //MessageBox(main_window,"Kernel Handle Exception",0,0);
  return TGKModulesSet::on_exception(er);
}


LRESULT WINAPI module_main(DWORD cmd ,LPARAM,LPARAM)
{
       LRESULT ret = NULL;

       switch(cmd)
       {
        case GKME_CMD_GET_MODULETYPE: ret = MT_KERNEL;break;
        case GKME_CMD_CREATE_HANDLE :
        {
         if(!find_gkhandleW(GRKERNEL_HANDLE_NAME))
           {
            TGRKernel * grkernel = new TGRKernel;
            ret  = (LRESULT)(*grkernel)();
            if(!ret)
             delete grkernel;
           }
        }
        break;
        case GKME_CMD_UNLOAD_MODULE: ret = TRUE;break;
       }

       return ret;

}

bool     __fastcall TGRKernel::remove_module(GKHANDLE h,bool erase )
{
 if(is_same_gkhandle(h,reporter) && erase)
 {
  TRegsWriter rw(DupKey(reg_key));
  rw.OpenKey(MREG_KEY_SETTINGS);
  rw.DeleteKey(GRKERNEL_REPORTER_KEY);
  erase = false;
 }
 return  TGKModulesSet::remove_module(h,erase);
}

GKHANDLE __fastcall TGRKernel::append_module(wchar_t * image_path, bool permanent)
{
  //проверить, установлен ли внешний репортёр
  GKHANDLE ret=(GKHANDLE)GKH_RET_ERROR;
  TGKModuleLoader  mldr;
  mldr.load_module(image_path);
  if(mldr.is_loaded() && mldr.is_module())
  {
   if(mldr.get_module_type() == MT_REPORTER)
    replace_reporter(image_path);
   else
    ret=TGKModulesSet::append_module(image_path,permanent);
  }
  else
  GetLastError();
  return ret;
}

 DWORD     __fastcall TGRKernel::replace_reporter(wchar_t * image_path)
 {
   delete_stdreporter();
   TGKModuleInterface * iface = do_load_module(image_path);
   if(iface)
   {
    TRegsWriter rw;
    rw.SetRootKey(DupKey(reg_key));
    rw.OpenKey(MREG_KEY_SETTINGS,true);
    rw.OpenKey(GRKERNEL_REPORTER_KEY,true);
    rw.WriteString(MREG_VALUE_IMAGEPATH,image_path);
    accept_loaded_module(iface,MT_REPORTER);
    reporter = (*iface)();
   }

   if(!reporter)
      create_stdreporter();
      start_reporter();
   return 0;
 }


DWORD __fastcall TGRKernel::create_stdreporter()
{
  DWORD ret(GKH_RET_ERROR);
  if(!find_gkhandleW(REPORTER_NAME_W))//Если незапущен внешний репортёр - запускаем внутренний
  {
    TGKStdReporter * std_rep = new TGKStdReporter();
    reporter = (*std_rep)();
    TGKModuleInterface * mi = new TGKModuleInterface(reporter);
    MODULE_INIT mod_init;
    ZeroMemory(&mod_init,sizeof(mod_init));
    mod_init.dw_size = sizeof(mod_init);
    mod_init.owner   = _handle;
    mi->module_init(&mod_init);
    if (accept_loaded_module(mi,mi->get_type()))
        ret=mi->start(0,0);
  }
  return ret;
}

DWORD __fastcall TGRKernel::delete_stdreporter()
{
  DWORD ret(GKH_RET_SUCCESS);
  if(reporter)
  {
    if (remove_module(reporter,false))
      reporter=NULL;
    else
      ret=GKH_RET_ERROR;
  }
  return ret;
}

void __fastcall TGRKernel::load_reporter()
{
    //Загрузка  отчётчика
    TRegsReader rr(DupKey(reg_key));
    if(rr.OpenKey(MREG_KEY_SETTINGS) && load_module(rr(), const_cast<wchar_t*>(GRKERNEL_REPORTER_KEY)))
    {
      reporter = (*modules_list.at(0))();
    }
    else
    create_stdreporter();

}

 void   __fastcall TGRKernel::start_reporter()
 {
   TReports::iterator b = reports.begin(),e = reports.end();
   call_gkhandle(reporter,MCMD_START,MODULE_START_REQUEST,0);
   while(b<e)
   {
    report_names * rn = *b;
     call_gkhandle(reporter,RCM_REGISTER_MSG_TYPE_W,LPARAM(rn->name),LPARAM(rn->descr));
    b++;
   }
 }

 void  __fastcall TGRKernel::register_msg_type(DWORD id,wchar_t * name,wchar_t * descr)
 {
   if(id!=(DWORD)GKH_RET_ERROR)
   {
    report_names * rn = new report_names(id,name,descr);
    TReports::iterator ins_ptr = std::lower_bound(reports.begin(),reports.end(),rn,report_names_less());
    reports.insert(ins_ptr,rn);
   }
 }

 void  __fastcall TGRKernel::register_msg_type(DWORD id,char    * name,char    * descr)
 {

   if(name && *name)
   {
     wchar_t * wname  = new wchar_t[lstrlenA(name)+1];
     wchar_t * wdescr = (descr && *descr) ? new wchar_t[lstrlenA(descr)+1]:new wchar_t[1];
     *wdescr = 0;
     Ansi2Unicode(wname,name);
     if(descr && *descr)
     Ansi2Unicode(wdescr,descr);
     register_msg_type(id,wname,wdescr);
     if(wdescr) delete [] wdescr;
     if(wname)  delete [] wname;
   }
 }

 DWORD  __fastcall TGRKernel::get_mem_used()
 {
   return (common_heap.IsValid()) ? common_heap.GetMemoryUsed() : GKH_RET_ERROR;
 }

 DWORD  __fastcall TGRKernel::on_kernel_timer(DWORD timer_id)
 {
   switch(timer_id)
   {
     case TIMER_ID_TRIAL: trial_inc_time();
     break;
   }
   return GKH_RET_SUCCESS;
 }

int   winapi     TGRKernel::trial_inc_proc (LPVOID ptr,GKHANDLE handle)
 {
   if(ptr)
     {
      std::vector<GKHANDLE> * hlist = (std::vector<GKHANDLE> *)ptr;
      hlist->push_back(handle);
     }
   return TRUE;
 }

 void   __fastcall TGRKernel::trial_inc_time ()
 {

   std::vector<GKHANDLE> hlist;
   enum_gkhandles(&hlist,trial_inc_proc);
   std::vector<GKHANDLE>::iterator beg = hlist.begin(),end = hlist.end();
   while(beg<end)
   {
    TGKModuleInterface mod_iface(*beg,true);

     if(!is_same_gkhandle(mod_iface(),(*this)()) &&  mod_iface.is_running())
      mod_iface.call(MCMD_LICENSE_INCTIME,1,0);
    ++beg;
   }
 }

 BOOL   __fastcall TGRKernel::set_instance(wchar_t * _inst,DWORD len)
 {
   if(_inst && *_inst && len < KERTL_ARRAY_COUNT(instance_name))
     {
      wcscpy(instance_name,_inst);
      return true;
     }
    return FALSE;
 }

 DWORD  __fastcall TGRKernel::get_instance(wchar_t * _inst,DWORD sz )
 {
   DWORD ilen = wcslen(instance_name);
   if(_inst && sz > ilen)
      {
       wcscpy(_inst,instance_name);
       return ilen;
      }
  return 0;
 }

