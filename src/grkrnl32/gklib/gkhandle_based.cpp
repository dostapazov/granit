/***************************************************************
 *
 * Модуль    : Реализация класса TGKHandleBased (основан на GKHANDLE)
 * Описание  :
 * Автор     :Остапенко Д.В.
 * Версия    :1
 * Создан    :20.06.2002 10:02:42
 *
 ****************************************************************/

#include <gkhandle.hpp>

#define GKHANDLE_SECRET_DWORD 0x6C486B47

TGKHandleBased::TGKHandleBased()
 :secret_dword(GKHANDLE_SECRET_DWORD)
 ,lock_param(GKHB_AUTO_LOCK_OFF)
 ,_handle(NULL)
 ,_prev_except_handler(NULL)
{}


TGKHandleBased *  __fastcall TGKHandleBased::CAST(LPVOID ptr)
{
   TGKHandleBased * ghb =  reinterpret_cast<TGKHandleBased *>(ptr);
   return (ghb && !IsBadReadPtr(&ghb->secret_dword,sizeof(DWORD))  && ghb->secret_dword == GKHANDLE_SECRET_DWORD) ? ghb: NULL;
}


     void     __fastcall TGKHandleBased::release(LPVOID){};

     LRESULT      WINAPI TGKHandleBased::free(TGKHandleBased* ghb,LPVOID pv)
     {
      __try{
            ghb->release(pv);
            delete ghb;
           }
      __finally{

               }
       return 0;
     }

  #pragma warn -8057
	 LRESULT  __fastcall TGKHandleBased::processing(DWORD cmd,LPARAM p1,LPARAM p2)
	 {
	  LRESULT ret(-1);
		SetLastError(STG_E_UNIMPLEMENTEDFUNCTION);
	  return ret;
	 }

  #pragma warn .8057

     LRESULT      WINAPI TGKHandleBased::handle_function(LPVOID data,DWORD cmd,LPARAM p1,LPARAM p2)
     {
      LRESULT ret(-1);
	  TGKHandleBased * ghb =  CAST(data);
	  if(ghb)
        {
        if(cmd == HANDLE_CMD)
        {
          switch(p1)
          {
           case  HANDLE_SUBCMD_RELEASE     : ret = free(ghb,LPVOID(p2)) ;break;
           case  HANDLE_SUBCMD_LOCK        : ret = ghb->lock((DWORD)p2) ;break;
           case  HANDLE_SUBCMD_UNLOCK      : ret = ghb->unlock()        ;break;
           case  HANDLE_SUBCMD_LOCKCOUNT   : ret = ghb->get_lock_count();break;
           case  HANDLE_SUBCMD_FULLUNLOCK  : ret = ghb->full_unlock()   ;break;
           case  HANDLE_SUBCMD_RESTORELOCK : ret = ghb->restore_lock((DWORD)p2);break;
           case  HANDLE_SUBCMD_GET_OWNTHREADID: ret = ghb->locker.GetOwnerThreadId();break;
          } 
        }  
          else
          {
           if(!ghb->lock_param)
               ghb->lock();
            ret = ghb->processing(cmd,p1,p2);
           if(!ghb->lock_param)
              ghb->unlock();
          }
        }
      return ret;
     }

    GKHANDLE __fastcall TGKHandleBased::alloc_gkhandle(const wchar_t * name, LARGE_INTEGER *tag)
    {
     if(NULL == _handle && !find_gkhandleW(name))
         {
          _handle = create_gkhandle_tagW(this,handle_function,0,name,tag);
          this->_prev_except_handler = set_gkhandle_exception_filterP(exception_handler,_handle);
         }
       return _handle;
    }

    GKHANDLE __fastcall TGKHandleBased::alloc_gkhandle(const char * name, LARGE_INTEGER *tag)
    {
     if(NULL == _handle && !find_gkhandleA(name))
         {
          _handle = create_gkhandle_tagA(this,handle_function,0,name,tag);
          this->_prev_except_handler = set_gkhandle_exception_filterP(exception_handler,_handle);
         }
       return _handle;
    }


    void     __fastcall TGKHandleBased::release_lock(DWORD tid)
    {
      if(locker.GetOwnerThreadId() ==tid )
         locker.FullUnlock();
  
    }

    DWORD    __fastcall TGKHandleBased::on_exception(lpexcept_record er)
    {
      release_lock(GetCurrentThreadId());
      if(_prev_except_handler)
         this->_prev_except_handler(er);
       return DWORD(GKH_RET_EXCEPTION);
    }

   DWORD        WINAPI TGKHandleBased::exception_handler(lpexcept_record er)
   {
    TGKHandleBased * gkhb = CAST(get_gkhandle_data(er->handle));
    if(gkhb)
    {
      
      return gkhb->on_exception(er);
    }
    return DWORD(-2);
   }


