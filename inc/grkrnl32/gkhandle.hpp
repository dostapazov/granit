/***************************************************************
 *
 *
 * Описание  :Интерфейс классов работающий с GKHANDLE
 * Автор     :Остапенко Д.В.
 * Версия    :1
 * Создан    :19.06.2002 13:21:17
 *
 ****************************************************************/
#ifndef _GKHANDLE_INC_HPP
#define _GKHANDLE_INC_HPP

#ifndef _KERTL_INC
 #include <kertl.hpp>
#endif

#ifndef  _GKHANDLE_INC
 #include <gkhandle.h>
#endif

#include <vector>

using namespace KeRTL;
using namespace std;


    struct gkhandle_less
    {
     bool operator()(GKHANDLE h1,GKHANDLE h2) const
     {return (gkhandle_compare(h1,h2)<0) ? true:false;}
    };

    struct gkhandle_equal
    {
     bool operator()(GKHANDLE h1,GKHANDLE h2) const
     {return (is_same_gkhandle((GKHANDLE)h1,(GKHANDLE)h2)) ? true : false;}
    };

  class TGKHandleInterface
  {
   protected:
   GKHANDLE _handle;
   bool     no_close;
   public:
    TGKHandleInterface();
    TGKHandleInterface(const char   * name);
    TGKHandleInterface(const wchar_t* name);
    TGKHandleInterface(GKHANDLE     handle,bool _no_close);
    TGKHandleInterface(GKHANDLE     handle);
    virtual ~TGKHandleInterface();
    virtual
    int     __fastcall close();
    bool    __fastcall open(const char   * name);
    bool    __fastcall open(const wchar_t* name);
    bool    __fastcall open(GKHANDLE     handle,bool no_close = false);
    bool    __fastcall get_handle_name(char   * buff,int bsz);
    bool    __fastcall get_handle_name(wchar_t* buff,int bsz);


    LRESULT __fastcall call(DWORD cmd,LPARAM p1,LPARAM p2);
    LRESULT __fastcall call(DWORD cmd,LPARAM p1,LPARAM p2) const;
    GKHANDLE operator()(){return _handle;}
    LRESULT  operator()(DWORD cmd,LPARAM p1,LPARAM p2){return call(cmd,p1,p2);}
    bool     __fastcall lock(bool try_lock = false);
    bool     __fastcall unlock();
    DWORD    __fastcall full_unlock();
    bool     __fastcall restore_lock(DWORD cnt);
    DWORD    __fastcall get_lock_count();

    DWORD   __fastcall  get_owner_threadid();
    DWORD   __fastcall  get_attr();
    DWORD   __fastcall  set_attr(DWORD attr);
    int     __fastcall  compare_handle(GKHANDLE h_cmp);
    void    __fastcall  get_except_record(lpexcept_record er);
    DWORD   __fastcall  get_usage_counter();
    bool    __fastcall  is_valid()
    {
     return (_handle && _handle!=INVALID_GKHANDLE_VALUE) ? true:false;
    }
  };


  #define GKHB_AUTO_LOCK_ON     0
  #define GKHB_AUTO_LOCK_OFF    1

  class TGKHandleBased
  {
   private:
   DWORD   secret_dword;
   static  LRESULT      WINAPI free(TGKHandleBased* ghb,LPVOID pv);
   static  DWORD        WINAPI exception_handler(lpexcept_record er);
   static  LRESULT      WINAPI handle_function(LPVOID data,DWORD cmd,LPARAM p1,LPARAM p2);

   protected:
   DWORD                     lock_param;
   TFASTMutex                locker;
   GKHANDLE                  _handle;
   gkhandle_exception_filter _prev_except_handler;
   virtual void     __fastcall release(LPVOID);

   virtual LRESULT  __fastcall processing(DWORD cmd,LPARAM p1,LPARAM p2);
   virtual DWORD    __fastcall on_exception(lpexcept_record er);
           GKHANDLE __fastcall alloc_gkhandle(const wchar_t * name = 0, LARGE_INTEGER *tag=NULL);
           GKHANDLE __fastcall alloc_gkhandle(const char * name       , LARGE_INTEGER *tag=NULL);
   TGKHandleBased ();
   virtual ~TGKHandleBased(){};
   public:
   GKHANDLE operator()(){return _handle;}
   virtual BOOL     __fastcall lock(DWORD to = INFINITE);
   virtual BOOL     __fastcall unlock();
   virtual void     __fastcall release_lock(DWORD tid);
   DWORD    __fastcall full_unlock();
   bool     __fastcall restore_lock(DWORD cnt);
   DWORD    __fastcall get_lock_count();

   static
   TGKHandleBased *  __fastcall CAST(LPVOID ptr);
  };


  inline BOOL    __fastcall TGKHandleBased::lock(DWORD to)
  {
   if(to ) return locker.Lock();
      return locker.TryLock();
  }

  inline BOOL    __fastcall TGKHandleBased::unlock()
  {
     return locker.Unlock();
  }

  inline DWORD    __fastcall TGKHandleBased::full_unlock()
  {
    return locker.FullUnlock();
  }

  inline bool     __fastcall TGKHandleBased::restore_lock(DWORD cnt)
  {
    //return restorelock_gkhandle(_handle,cnt) ? true:false;
    return locker.RestoreLock(cnt);
  }

  inline DWORD    __fastcall TGKHandleBased::get_lock_count()
  {
   //return get_gkhandle_lockcount(_handle);
    return locker.GetLockCount();
  }



    inline TGKHandleInterface::TGKHandleInterface():_handle(0),no_close(false){}
    inline TGKHandleInterface::TGKHandleInterface(const char   * name):_handle(0),no_close(false){open(name);}
    inline TGKHandleInterface::TGKHandleInterface(const wchar_t* name):_handle(0),no_close(false){open(name);}
    inline TGKHandleInterface::TGKHandleInterface(GKHANDLE     handle):_handle(0),no_close(false){_handle = handle;}
    inline TGKHandleInterface::TGKHandleInterface(GKHANDLE     handle,bool _no_close):_handle(handle),no_close(_no_close){if(!no_close) _handle = dup_gkhandle(handle);}
    inline TGKHandleInterface::~TGKHandleInterface(){close();}

    inline int    __fastcall TGKHandleInterface::close()
    {
      int ret = -1;
      if(!no_close && _handle)
           ret = close_gkhandle(_handle);
        _handle = 0;
      return ret;
    }
    
    inline    DWORD   __fastcall TGKHandleInterface::get_owner_threadid()
    {     return 0;    }

    inline    DWORD   __fastcall TGKHandleInterface::get_attr()
    {     return get_gkhandle_attr(_handle);    }

    inline    DWORD   __fastcall TGKHandleInterface::set_attr(DWORD attr)
    {    return set_gkhandle_attr(_handle,attr); }

    inline    int     __fastcall TGKHandleInterface::compare_handle(GKHANDLE h_cmp)
    {  return gkhandle_compare(_handle,h_cmp);   }

    inline DWORD   __fastcall  TGKHandleInterface::get_usage_counter()
    {
     return get_gkhandle_usage(_handle);
    }


    inline bool    __fastcall TGKHandleInterface::open(const char   * name)
    {
      close();
     _handle = open_gkhandleA((char*)name);
     return _handle ? true:false;
    }
    inline bool    __fastcall TGKHandleInterface::open(const wchar_t* name)
    {
     close();
     _handle = open_gkhandleW((wchar_t*)name);
     return _handle ? true:false;
    }
    inline bool    __fastcall TGKHandleInterface::open(GKHANDLE     handle,bool _no_close)
    {
      close();
      no_close = _no_close;
     _handle   = _no_close ? handle:dup_gkhandle(handle);
     return _handle ? true:false;
    }

    inline LRESULT __fastcall TGKHandleInterface::call(DWORD cmd,LPARAM p1,LPARAM p2 )
    {
      return  call_gkhandle(_handle,cmd,p1,p2);
    }

    inline LRESULT __fastcall TGKHandleInterface::call(DWORD cmd,LPARAM p1,LPARAM p2 ) const
    {
      return  call_gkhandle(_handle,cmd,p1,p2);
    }


    inline bool    __fastcall TGKHandleInterface::get_handle_name(char   * buff,int bsz)
    {
     return get_gkhandle_nameA(_handle,buff,bsz) ? true:false;
    }
    inline bool    __fastcall TGKHandleInterface::get_handle_name(wchar_t* buff,int bsz)
    {
     return get_gkhandle_nameW(_handle,buff,bsz) ? true:false;
    }

    inline bool     __fastcall TGKHandleInterface::lock(bool try_lock)
    {
        return (try_lock ? try_lock_gkhandle(_handle) : lock_gkhandle(_handle))? true:false;
    }

    inline bool     __fastcall TGKHandleInterface::unlock()
    {
     return unlock_gkhandle(_handle) ? true:false;
    }
    inline DWORD    __fastcall TGKHandleInterface::full_unlock()
    {
      return fullunlock_gkhandle(_handle);
    }

    inline bool     __fastcall TGKHandleInterface::restore_lock(DWORD cnt)
    {
      return restorelock_gkhandle(_handle,cnt) ? true:false;
    }

    inline DWORD    __fastcall TGKHandleInterface::get_lock_count()
    {
     return get_gkhandle_lockcount(_handle);
    }


    inline void    __fastcall  TGKHandleInterface::get_except_record(lpexcept_record er)
    {
     get_gkhandle_except_record(er);
    }

  class TGKHandlesList:protected vector<GKHANDLE>
  {
   protected:
   TFASTMutex locker;
   public:
   TGKHandlesList(){};
   ~TGKHandlesList(){free();};
   int      __fastcall add   (GKHANDLE h);
   int      __fastcall index (GKHANDLE h);
   bool     __fastcall remove(GKHANDLE h);
   GKHANDLE __fastcall at(int idx);
   int      __fastcall count();
   void     __fastcall free();
   bool     __fastcall lock();
   bool     __fastcall unlock();
   void     __fastcall for_each(DWORD cmd,LPARAM p1,LPARAM p2);
  };



  inline  bool     __fastcall TGKHandlesList::lock()
  {
   return locker.Lock();
  }
  inline  bool     __fastcall TGKHandlesList::unlock()
  {
   return locker.Unlock();
  }

#endif


 
