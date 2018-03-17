/*
 DLL для работы с GRANIT KERNEL HANDLE
 Остапенко Д.В. 1993-2001
 СДТУ Норильск
*/

/*
  Добавить анонимные хендлы
*/


#include <kertl.hpp>
#include <vector>
#include <algorithm>
#include <map>
#include <kedatas.hpp>
#include <gkhandle.h>
/*
#include <winerror.h>
#include <winnt.h>
#include <winbase.h>
*/



DWORD TLS_INDEX = 0;

#if _MSC_VER >=1200
#ifndef _DEBUG
#pragma comment(linker,"/opt:nowin98")
#endif
#endif




using namespace KeRTL;
//---------------------------------------------------------------------------
#define GKHANDLE_SECRET_DWORD 0x6C486B47

#pragma pack(push,8)
typedef struct _handle_struct
{
  DWORD                       size;
  DWORD                       SecretDword;
  DWORD                       Attributes;
  LARGE_INTEGER               tag;
  wchar_t *                   name;
  LPVOID                      handle_data;
  pfn_entry_function          entry;
  int                         usage_count;
  gkhandle_exception_filter   except_filter;
  TEventSimpleDataQueue    *  messages;
  //KeRTL::TFASTMutex locker;
   _handle_struct ();
   _handle_struct (LPVOID _data,pfn_entry_function _entry,DWORD _attr,const wchar_t *_name);
  ~_handle_struct ();
  BOOL    __fastcall lock(DWORD to = 0);
  BOOL    __fastcall unlock();
  DWORD   __fastcall full_unlock();
  BOOL    __fastcall restore_lock(DWORD);
  LRESULT __fastcall call(DWORD cmd,LPARAM p1,LPARAM p2);
}handle_struct,* lphandle_struct;
#pragma pack(pop)

  struct _handle_struct_less
  {
   bool operator() (const lphandle_struct & hs1,const lphandle_struct &hs2);
  };

  struct _handle_struct_equal
  {
   bool operator() (const lphandle_struct & hs1,const lphandle_struct &hs2);
  };


   inline _handle_struct::_handle_struct ()
   {
     size        = sizeof(*this);
     SecretDword = GKHANDLE_SECRET_DWORD;
     entry       = NULL;
     handle_data = NULL;
     Attributes  = 0;
     name        = NULL;
     usage_count = 0;
     except_filter = NULL;
     messages      = NULL;
     
   }

   _handle_struct::_handle_struct (LPVOID _data,pfn_entry_function _entry,DWORD _attr,const wchar_t * _name)
   
   {
     size          = sizeof(*this);
     SecretDword   = GKHANDLE_SECRET_DWORD;
     entry         = _entry;
     handle_data   = _data;
     Attributes    = _attr;
     usage_count   = 0;
     except_filter = NULL;
     name          = NULL;
     messages      = NULL;
     GetSystemTimeAsFileTime((LPFILETIME)&tag);
     
     int name_len = _name ? lstrlenW(_name):0;
     if( name_len )
     {
       name = new wchar_t[name_len+1];
       lstrcpyW(name,_name);
     }
   }

   _handle_struct::~_handle_struct ()
   {
    //lock();
    if(messages)
       {
        delete messages;
       }
    if(name)
     delete[] name;
    //unlock();
   }

  inline BOOL    __fastcall _handle_struct::lock(DWORD to)
  {
   return (BOOL)call(HANDLE_CMD,HANDLE_SUBCMD_LOCK,to);
  }

  inline BOOL    __fastcall _handle_struct::unlock()
  {
   return (BOOL)call(HANDLE_CMD,HANDLE_SUBCMD_UNLOCK,0);
  }

  DWORD   __fastcall _handle_struct::full_unlock()
  {
    return (DWORD) call(HANDLE_CMD,HANDLE_SUBCMD_FULLUNLOCK,0);
  }
  BOOL    __fastcall _handle_struct::restore_lock(DWORD cnt)
  {
    return (BOOL)call(HANDLE_CMD,HANDLE_SUBCMD_RESTORELOCK,cnt);
  }


  /*Сравнение : сравниваются имена, затем данные, затем точки входа*/
  bool _handle_struct_less::operator () (const lphandle_struct & hs1,const lphandle_struct &hs2)
  {
	if(hs1->name && hs2->name)
	   return lstrcmpiW(hs1->name , hs2->name)<0 ? true:false;
	else
	{
	 if(hs1->name == hs2->name)
	  {
		 if(hs1->handle_data == hs2->handle_data)
		   return hs1->entry<hs2->entry ? true:false;
		 return hs1->handle_data < hs2->handle_data ? true:false;
	  }
	  else
	   return hs1->name<hs2->name ? true:false;
	}
  }

  bool _handle_struct_equal::operator () (const lphandle_struct & hs1,const lphandle_struct &hs2)
  {
	if(hs1->name && hs2->name && lstrcmpiW(hs1->name , hs2->name)==0)
	   return (hs1->handle_data == hs2->handle_data && hs1->entry == hs2->entry) ?true:false;
	 return false;
  }




   static KeRTL::THeap           heap(0xF00);//Локальная куча для данных
   static KeRTL::TFASTMutex      locker;

   static DWORD first_handle_value = 0xF00000;
   /* Список  открытых хендлов */
   static std::vector<lphandle_struct>          handles_data;//Данные хендлов
   static std::map<DWORD,lphandle_struct>       handles_map; //Соответствие хендлов->данные
   static std::multimap<lphandle_struct,DWORD>  keys_map;    //Список данные->хендлы
   static std::vector<DWORD>                    free_handles; 


   lphandle_struct __fastcall get_gkhandle_struct(GKHANDLE handle)
   {
    if(IsValidGKHandle(handle))
	{
        locker.Lock();
        std::map<DWORD,lphandle_struct>::iterator i = handles_map.find(DWORD(handle));
        lphandle_struct ret = (i != handles_map.end() && i->first == DWORD(handle)) ? i->second:NULL;
	if(!ret)
	 SetLastError(ERROR_INVALID_HANDLE);

        locker.Unlock();
	return ret;
	}
	return NULL;
   }

   GKHANDLE winapi create_gkhandleA(LPVOID data,pfn_entry_function entry,DWORD attr,const char    * name)
   {
    return create_gkhandle_tagA(data,entry,attr,name,NULL);
   }

   GKHANDLE winapi create_gkhandle_tagA(LPVOID data,pfn_entry_function entry,DWORD attr,const char    * name,LARGE_INTEGER * tag)
   {
     GKHANDLE handle = NULL;
     if(entry)
     {
      int   len = name ? lstrlenA( name):0;
      wchar_t buff[MAX_PATH];
      wchar_t* _u_name = (wchar_t*)(len ? (len>=MAX_PATH ? new wchar_t[len+1]:buff):0);
      if(_u_name)
      KeRTL::Ansi2Unicode(_u_name,name);
      handle = create_gkhandle_tagW(data,entry,attr,_u_name,tag);
      if(_u_name && _u_name!=buff)
          delete[] _u_name;
     }
     return handle;
   }


   void __fastcall put_handles_record(GKHANDLE handle,lphandle_struct hs)
   {
      std::pair<GKHANDLE,lphandle_struct> p1;
      std::pair<lphandle_struct,GKHANDLE> p2;
      p1.first  = p2.second = (handle);
      p1.second = p2.first  = hs;
      handles_map.insert(p1);
      keys_map.insert(p2);

   }

   GKHANDLE winapi create_gkhandleW(LPVOID data,pfn_entry_function entry,DWORD attr,const wchar_t    * name)
   {
    return create_gkhandle_tagW(data,entry,attr,name,NULL);
   }

   GKHANDLE winapi create_gkhandle_tagW(LPVOID data,pfn_entry_function entry,DWORD attr,const wchar_t * name,LARGE_INTEGER * tag)
   {
     //создание
     GKHANDLE handle = open_gkhandleW(name);
     if(handle == NULL)
     {
      if(entry)
      {
      lphandle_struct  hs = new handle_struct(data,entry,attr,name);
      Sleep(100);
      if(tag)
         tag->QuadPart = hs->tag.QuadPart;
      if(hs)
      {
       locker.Lock();
       if(free_handles.size())
       {
		vector<DWORD>::iterator ptr = free_handles.begin();
        handle = (GKHANDLE)*ptr;
        free_handles.erase(ptr);
       }
       else
       handle = GKHANDLE(first_handle_value++);
       
       hs->usage_count++;
       std::vector<lphandle_struct>::iterator i = handles_data.begin();
       i = std::lower_bound(i,handles_data.end(),hs,_handle_struct_less());
       handles_data.insert(i,hs);
       put_handles_record(handle,hs);
       locker.Unlock();
      }
      }
      else
      SetLastError(ERROR_INVALID_FUNCTION);
     }
     else
     SetLastError(ERROR_ALREADY_EXISTS);
     return handle;
   }

   GKHANDLE winapi open_gkhandleA  (const char    * name  )
   {
     int   len = name ? lstrlenA( name):0;
      wchar_t buff[MAX_PATH];
      wchar_t* _u_name = (wchar_t*)(len ? (len>=MAX_PATH ? new wchar_t[len+1]:buff):0);
     if(_u_name)
      KeRTL::Ansi2Unicode(_u_name,name);
      GKHANDLE handle =  open_gkhandleW(_u_name);
      if(_u_name && _u_name !=buff)
        delete[] _u_name;
        return handle;
   }

   GKHANDLE winapi open_gkhandleW  (const wchar_t * name  )
   {
      GKHANDLE handle(0);
      if(name && lstrlenW(name))
      {
      locker.Lock();
      std::vector<lphandle_struct>::iterator i = handles_data.begin();
      _handle_struct _hs(0,0,0,name);
      i = std::lower_bound(i,handles_data.end(),&_hs,_handle_struct_less());
      if(i<handles_data.end() && lstrcmpiW((*i)->name,name)==0)
         {
          handle = dup_gkhandle(GKHANDLE(keys_map.find(*i)->second));
         }
      locker.Unlock();
      }
    return handle;
   }

   void __fastcall erase_handle(GKHANDLE hObj,lphandle_struct hs)
   {
     std::multimap<lphandle_struct,DWORD>::iterator k = keys_map.find(hs);
     while(k->first == hs)
     {
     if(k->second==DWORD(hObj))
      {
       keys_map.erase(k);break;
      }
       k++;
     }
   }

   LONG     winapi close_gkhandle  (GKHANDLE  hObj)
   {
    /*Закрытие хендла*/
    LONG ret = -1;
    locker.Lock();
     std::map<DWORD,lphandle_struct>::iterator h = handles_map.find(DWORD(hObj));
     if(h !=handles_map.end() && h->first == DWORD(hObj))
     {
     lphandle_struct hs = h->second;
     erase_handle(hObj,h->second);
     handles_map.erase(DWORD(hObj));
     free_handles.push_back(DWORD(hObj));
     ret = --hs->usage_count;
     if(ret <= 0)
       {

         std::vector<lphandle_struct>::iterator i
         = std::lower_bound(handles_data.begin(),handles_data.end(),hs,_handle_struct_less());
         if(*i == hs)
            {
             handles_data.erase(i);
			 hs->lock();
			 hs->call(HANDLE_SUBCMD_RELEASE,0,0);
			 delete hs;


           }
           else ret = -1;
       }

    }
    locker.Unlock();
    return ret;
   }

   DWORD    winapi get_gkhandle_usage(GKHANDLE hObj)
   {
    DWORD  ret(0);
    locker.Lock();
    lphandle_struct hs = get_gkhandle_struct(hObj);
    if(hs)
    {
     ret = hs->usage_count;
    }
    locker.Unlock();
    return ret;
   }

   BOOL     winapi find_gkhandleA     (const char    * name)
   {
    BOOL ret(FALSE);
     int   len = name ? lstrlenA( name):0;
      wchar_t buff[MAX_PATH];
      wchar_t* _u_name = (wchar_t*)(len ? (len>=MAX_PATH ? new wchar_t[len+1]:buff):0);
     if(_u_name)
      {
       KeRTL::Ansi2Unicode(_u_name,name);
       ret =  find_gkhandleW(_u_name);
       if(_u_name && _u_name!=buff)
       delete[] _u_name;
      }
      return ret;
   }

   BOOL     winapi find_gkhandleW     (const wchar_t * name)
   {
    BOOL ret(FALSE);
    if(name && lstrlenW(name))
    {
      locker.Lock();
      std::vector<lphandle_struct>::iterator i = handles_data.begin();
      handle_struct _hs(0,0,0,name);
      i = std::lower_bound(i,handles_data.end(),&_hs,_handle_struct_less());
      ret = (i<handles_data.end() && (*i)->name && lstrcmpiW(name,(*i)->name)==0) ? TRUE:FALSE;
      locker.Unlock();
    }
    return ret;
   }

   DWORD    winapi get_gkhandle_attr(GKHANDLE  hObj)
   {
     DWORD ret(0);
     locker.Lock();
     lphandle_struct hs = get_gkhandle_struct(hObj);
     if(hs)
     {
     ret = hs->Attributes;
     }
     locker.Unlock();
     return ret;
   }

   LPVOID   winapi get_gkhandle_data  (GKHANDLE hObj)
   {
     LPVOID ret = 0;
     locker.Lock();
     lphandle_struct hs = get_gkhandle_struct(hObj);
     if(hs)
     {
      ret = hs->handle_data;
     }
     locker.Unlock();
     return ret;
   }

   pfn_entry_function winapi gkhandle_subclass(GKHANDLE hObj,pfn_entry_function new_func,LPVOID new_data,LPVOID * old_data)
   {
     pfn_entry_function ret = NULL;
     locker.Lock();
     lphandle_struct hs = get_gkhandle_struct(hObj);
     if(hs)
     {
      if(old_data)
         *old_data = hs->handle_data;
      ret   = hs->entry;
      hs->entry       = new_func;
      hs->handle_data = new_data;

     }
     locker.Unlock();
     return ret;
   }


   DWORD    winapi get_owner_threadid(GKHANDLE h)
   {
     DWORD ret = 0;
     locker.Lock();
     lphandle_struct hs = get_gkhandle_struct(h);
     if(hs  )
     {
      ret = hs->call(HANDLE_SUBCMD_GET_OWNTHREADID,0,0);
     }
     locker.Unlock();
     return ret;
   }


   DWORD    winapi set_gkhandle_attr(GKHANDLE  hObj,DWORD attr)
   {
     DWORD ret(0);
     locker.Lock();
     lphandle_struct hs = get_gkhandle_struct(hObj);
     if(hs)
     {
      ret = hs->Attributes;
      hs->Attributes = attr;
     }
     locker.Unlock();
     return ret;
   }

   BOOL     winapi set_gkhandle_nameA (GKHANDLE hObj,const char   * name,LARGE_INTEGER tag)
   {
      int   len = name ? lstrlenA( name):0;
      wchar_t buff[MAX_PATH];
      wchar_t* _u_name = (wchar_t*)(len ? (len>=MAX_PATH ? new wchar_t[len+1]:buff):0);
      if(_u_name)
      KeRTL::Ansi2Unicode(_u_name,name);
      BOOL ret = set_gkhandle_nameW(hObj,_u_name,tag);
      if(_u_name && _u_name!=buff)
          delete[] _u_name;
      return ret;
   }

   BOOL     winapi set_gkhandle_nameW (GKHANDLE hObj,const wchar_t* name,LARGE_INTEGER tag)
   {
    BOOL ret = FALSE;
     locker.Lock();
     if (!find_gkhandleW(name))
     {
     lphandle_struct hs = get_gkhandle_struct(hObj);
     if(hs && hs->tag.QuadPart == tag.QuadPart)
     {
      if(hs->name)
       delete [] hs->name;
      hs->name = 0;
      int len = name ? lstrlenW(name):0;
      if(len)
       {
        hs->name = new wchar_t[len+1];
        lstrcpyW(hs->name,name);
       }
      sort(handles_data.begin(),handles_data.end(),_handle_struct_less());
      ret = TRUE;
     }
     }
     locker.Unlock();
    return ret;
   }





void __fastcall put_except_record(GKHANDLE  hObj,DWORD cmd,LPARAM p1,LPARAM p2)
{

  lpexcept_record  er = (lpexcept_record)TlsGetValue(TLS_INDEX);
  if(!er)
  {
   er = new except_record;
   if(er)
    {
     TlsSetValue(TLS_INDEX,er);
    }
    else
    return;
  }
  er->cmd     = cmd;
  er->handle  = hObj;
  er->p1=p1;er->p2=p2;
  ZeroMemory(&er->except_rec,sizeof(er->except_rec));
  ZeroMemory(&er->except_ctx,sizeof  (er->except_ctx));
}

LONG winapi put_except_pointers(LPEXCEPTION_POINTERS __ep)
{
  lpexcept_record  er = (lpexcept_record)TlsGetValue(TLS_INDEX);
  if(!er)
  {
   er = new except_record;
   er->dw_size = sizeof(*er);
   if(er)
   {
    TlsSetValue(TLS_INDEX,er);
   }
   else
   return EXCEPTION_EXECUTE_HANDLER;
  }

  CopyMemory(&er->except_rec,__ep->ExceptionRecord,sizeof(er->except_rec));
  CopyMemory(&er->except_ctx,__ep->ContextRecord  ,sizeof(er->except_ctx));
  #ifdef _WIN64
  CopyMemory(er->esp_dump,(LPVOID)__ep->ContextRecord->R13,sizeof(er->esp_dump));
  CopyMemory(er->ebp_dump,(LPVOID)__ep->ContextRecord->R12,sizeof(er->ebp_dump));
  #else
  CopyMemory(er->esp_dump,(LPVOID)__ep->ContextRecord->Esp,sizeof(er->esp_dump));
  CopyMemory(er->ebp_dump,(LPVOID)__ep->ContextRecord->Ebp,sizeof(er->ebp_dump));
  #endif
  return EXCEPTION_EXECUTE_HANDLER;
}

void     winapi get_gkhandle_except_record (lpexcept_record er)
{
  if(er)
  {
  lpexcept_record _er = (lpexcept_record)TlsGetValue(TLS_INDEX);
  if(_er)
    CopyMemory(er,_er,sizeof(*er));
  else
    ZeroMemory(er,sizeof(*er));
  }
}


DWORD winapi std_gkhandle_exception_filter(lpexcept_record )
{
  return EXCEPTION_EXECUTE_HANDLER;
}

gkhandle_exception_filter ef = 0;
KeRTL::TFASTMutex ef_locker;

LONG WINAPI UNH_EF(LPEXCEPTION_POINTERS)
{

 return EXCEPTION_EXECUTE_HANDLER;

}

gkhandle_exception_filter winapi set_gkhandle_exception_filter(gkhandle_exception_filter fltr)
{
  return set_gkhandle_exception_filterP(fltr,0);
}

gkhandle_exception_filter winapi set_gkhandle_exception_filterP(gkhandle_exception_filter fltr,GKHANDLE h)
{
  // Установка нового фильтра обработки исключений
  gkhandle_exception_filter old=NULL;

  if(fltr == std_gkhandle_exception_filter)
    fltr = 0;
  locker.Lock();
  handle_struct * hs = get_gkhandle_struct(h);
  if(hs)
   hs->except_filter = fltr;
  //if(old_unhandled_except_filter == 0)
     //old_unhandled_except_filter = SetUnhandledExceptionFilter(UNH_EF);
  locker.Unlock();
  if(!hs)
  {
   ef_locker.Lock();
   old = ef;
   ef  = fltr;
   ef_locker.Unlock();

  }
  return old ? old : std_gkhandle_exception_filter;
}

  LRESULT __fastcall _handle_struct::call(DWORD cmd,LPARAM p1,LPARAM p2)
  {
   //Вызов функции хендла
   LRESULT ret;
   #ifndef _DEBUG
    if(IsDebuggerPresent())
    {
      LPBYTE ptr = NULL;
      #ifdef _DEBUG_PROTECT
      for(;;)
        strcpy(ptr,"Under debugger");
      #endif
    }
   #endif
   __try{

         ret = entry ? entry(this->handle_data,cmd,p1,p2) :LRESULT(-1);

        }
     __except(put_except_pointers(GetExceptionInformation()))
     {

                  if(except_filter && !IsBadCodePtr((FARPROC)except_filter) )
                    {
                      except_record er;
                      get_gkhandle_except_record (&er);
                      except_filter(&er);
                    }
                    else
                    {
                      ef_locker.Lock();
                      if(ef && !IsBadCodePtr((FARPROC)ef))
                      {
                              except_record er;
                              get_gkhandle_except_record (&er);
                              ef(&er);
                      }
                      ef_locker.Unlock();
                     }
                  Sleep(1);
	          ret = LRESULT(-2);

      }

   return ret;
  }

  /*Вызов подрограммы хендла*/
   LRESULT  winapi call_gkhandle     (GKHANDLE  hObj,DWORD cmd,LPARAM p1,LPARAM p2)
   {

     LRESULT ret(-1);
     locker.Lock();
     lphandle_struct phs = get_gkhandle_struct(hObj);
     locker.Unlock();
     if(phs)
     {
      if(phs->entry && !IsBadCodePtr((FARPROC)phs->entry))
         {
          put_except_record(hObj,cmd,p1,p2);
          ret = phs->call(cmd,p1,p2);
          //put_except_record((GKHANDLE)NULL,0,0,0);//? Надо ли.
         }
      }
     return ret;
 }



BOOL     winapi lock_gkhandle    (GKHANDLE h)
{
     locker.Lock();
     lphandle_struct phs = get_gkhandle_struct(h);
     locker.Unlock();
     if(phs )
        return (BOOL)phs->call(HANDLE_CMD,HANDLE_SUBCMD_LOCK,INFINITE);
       return FALSE;
}

BOOL     winapi unlock_gkhandle  (GKHANDLE h)
{
     locker.Lock();
     lphandle_struct phs = get_gkhandle_struct(h);
     locker.Unlock();
     if(phs )
        return (BOOL)phs->call(HANDLE_CMD,HANDLE_SUBCMD_UNLOCK,0);
     return FALSE;    
}

DWORD    winapi fullunlock_gkhandle  (GKHANDLE h)
{
     locker.Lock();
     lphandle_struct phs = get_gkhandle_struct(h);
     locker.Unlock();
     DWORD ret = 0;
     if(phs)
         {ret = phs->call(HANDLE_CMD,HANDLE_SUBCMD_FULLUNLOCK,0); }
     return ret;
}

BOOL     winapi restorelock_gkhandle (GKHANDLE h,DWORD cnt)
{
     locker.Lock();
     lphandle_struct phs = get_gkhandle_struct(h);
     locker.Unlock();
     if(phs)
         {return phs->call(HANDLE_CMD,HANDLE_SUBCMD_RESTORELOCK,cnt); }
     return FALSE;

}

DWORD    winapi get_gkhandle_lockcount(GKHANDLE h)
{
     locker.Lock();
     lphandle_struct phs = get_gkhandle_struct(h);
     locker.Unlock();
     if(phs)
         {return phs->call(HANDLE_CMD,HANDLE_SUBCMD_LOCKCOUNT,0); }
     return 0;

}



BOOL     winapi try_lock_gkhandle(GKHANDLE h)
{
     locker.Lock();
     lphandle_struct phs = get_gkhandle_struct(h);
     locker.Unlock();
     if(phs)  
      return  (BOOL)phs->call(HANDLE_CMD,HANDLE_SUBCMD_LOCK,0);
    return FALSE;
}


BOOL     winapi get_gkhandle_nameA(GKHANDLE hObj,char  * buffer,int cb_size)
{
  BOOL ret(FALSE);
  if(buffer && cb_size)
  {
        *buffer = 0;
	wchar_t * _u_name = new wchar_t[cb_size];
        if(_u_name)
	ret = get_gkhandle_nameW(hObj,_u_name,cb_size);
	if(ret )
		KeRTL::Unicode2Ansi(buffer,_u_name);
	delete[] _u_name;
  }
  else
  SetLastError(ERROR_INVALID_PARAMETER);
	return ret;
}


BOOL     winapi get_gkhandle_nameW(GKHANDLE hObj,wchar_t* buffer,int cb_size)
{
  BOOL ret(0);
  if( buffer && cb_size)
  {
   *buffer = 0;
   locker.Lock();
   lphandle_struct hs = get_gkhandle_struct(hObj);
   if(hs)
   {
    *buffer = 0;
    if(hs->name  )
	{
         if(lstrlenW(hs->name)<cb_size)
         {
		lstrcpyW(buffer,hs->name);
		ret = TRUE;
         }
         else
         SetLastError(ERROR_INVALID_USER_BUFFER);// winerror
	}
   }
   locker.Unlock();
  }
  else
  SetLastError(ERROR_INVALID_PARAMETER);
  return ret;
}

 BOOL     winapi call_gkhandle_bynameA(const char    * name,DWORD cmd,LPARAM p1,LPARAM p2,LRESULT * ret_code)
 {
   BOOL ret = FALSE;
   int  nlen;
   if(name && (nlen=lstrlenA(name))>0)
   {
    wchar_t buff[MAX_PATH];
    wchar_t *wide_name = nlen >=MAX_PATH ? new wchar_t[nlen+1]:buff;
    KeRTL::Ansi2Unicode(wide_name,name);
    ret = call_gkhandle_bynameW(wide_name,cmd,p1,p2,ret_code);
    if(wide_name!=buff)
       delete wide_name;
   }
   return ret;
 }

 BOOL     winapi call_gkhandle_bynameW(const wchar_t * name,DWORD cmd,LPARAM p1,LPARAM p2,LRESULT * ret_code)
 {
    BOOL ret(FALSE);
    LRESULT _ret_c = -1;
    if(name && lstrlenW(name))
    {
      lphandle_struct phs = NULL;
      locker.Lock();
      std::vector<lphandle_struct>::iterator i = handles_data.begin();
      handle_struct _hs(0,0,0,name);
      i = std::lower_bound(i,handles_data.end(),&_hs,_handle_struct_less());
      ret = (i<handles_data.end() && (*i)->name && lstrcmpiW(name,(*i)->name)==0) ? TRUE:FALSE;
      if(ret)
       phs = *i;
      locker.Unlock();
      if(phs)
         {

          put_except_record(0,cmd,p1,p2);
         _ret_c = phs->call(cmd,p1,p2);
         }
    }
    if(ret_code)
       *ret_code = _ret_c;
    return ret;
 }


GKHANDLE winapi dup_gkhandle     (GKHANDLE  hObj)
{
        GKHANDLE handle(0);
	locker.Lock();
        lphandle_struct hs = get_gkhandle_struct(hObj);
	if(hs)
	{
         hs->usage_count++;
         if(free_handles.size())
         {
		  vector<DWORD>::iterator  ptr = free_handles.begin();
		  handle = (GKHANDLE)*ptr;
          free_handles.erase(ptr);
         }
         else
         handle = GKHANDLE(first_handle_value++);
         
         put_handles_record(handle,hs);
	}
	locker.Unlock();
	return handle;
}

BOOL     winapi is_same_gkhandle(GKHANDLE h1,GKHANDLE h2)
{
 BOOL ret(FALSE);
	locker.Lock();
	if(get_gkhandle_struct(h1) == get_gkhandle_struct(h2))
		ret = TRUE;
 	locker.Unlock();
	return ret;
}


int winapi gkhandle_compare(GKHANDLE h1,GKHANDLE h2)
{
 int ret ;
 locker.Lock();
 _handle_struct * hs1 = get_gkhandle_struct(h1),*hs2 = get_gkhandle_struct(h2);
 if(hs1 && hs2)
   ret = int((hs1->tag.QuadPart - hs2->tag.QuadPart)); 
  else
  ret = (int)hs1 - int(hs2);
 locker.Unlock();
 return ret;
}



void winapi release_all_gkhandles()
{
  //Опасный участок !!!
  //если сделать locker.Lock(); то можно остальные нити ввести в DeadLock !!!
  BOOL do_continue;
  DWORD curr_handle = first_handle_value-1;
  do{
     locker.Lock();
     close_gkhandle(GKHANDLE(curr_handle--));
     do_continue = handles_data.begin()<handles_data.end() ? TRUE:FALSE;
     locker.Unlock();
    }while(do_continue);

}

void     winapi enum_gkhandles(LPVOID  user_data,pfn_enum_handle_proc proc)
{
  if(proc && !IsBadCodePtr((FARPROC) proc))
  {
  BOOL do_continue(TRUE);
  locker.Lock();
  std::vector  <lphandle_struct>::iterator i = handles_data.begin(),e = handles_data.end();
  std::multimap<lphandle_struct,DWORD>::iterator k;

  while( do_continue && i< e)
	  {
           k = keys_map.find(*i);
           while(do_continue && k->first == *i)
               {
                do_continue = proc(user_data,GKHANDLE(k->second));
                k++;
               }
           i++;
          }

  locker.Unlock();
  }
}


 BOOL               winapi gkhandle_post_message(GKHANDLE handle,DWORD cmd,LPARAM p1,LPARAM p2)
 {
   gkh_message msg;
   msg.cmd = cmd;
   msg.p1  = p1;
   msg.p2  = p2;
   BOOL ret = FALSE;
   TEventSimpleDataQueue * messages;
   locker.Lock();
   lphandle_struct  hs = get_gkhandle_struct(handle);
   if(hs)
   {
     messages               = hs->messages;
     if(!messages)
        hs->messages = messages = new TEventSimpleDataQueue;
     messages->PutIntoQueue(&msg,sizeof(msg));
     ret = TRUE;
   }
   locker.Unlock();
   return ret;

 }

 BOOL               winapi gkhandle_peek_message (GKHANDLE handle ,lpgkh_message msg,DWORD timeout,BOOL remove)
 {
   BOOL ret = FALSE;
   lphandle_struct  hs = get_gkhandle_struct(handle);
   if(hs && hs->messages)
   {
    if(hs->messages->QueueCount() == 0)
       ret =  hs->messages->GetEvent().Wait(timeout) == WaitOk ? TRUE:FALSE;
       else
       ret = TRUE;
       if(ret)
         remove ? hs->messages->GetFromQueue(msg,sizeof(*msg),NULL):hs->messages->PeekFromQueue(msg,sizeof(*msg),NULL);
         
   }
   return ret;
 }


 void               winapi gkhandle_process_all_messages(GKHANDLE handle)
 {
   gkh_message msg;
   while(gkhandle_peek_message(handle,&msg,0,TRUE))
         call_gkhandle(handle,msg.cmd,msg.p1,msg.p2);
 }




/*Распределение памяти мз кучи*/

   void * operator new(size_t size)
   {
    return heap.Alloc(size,HEAP_ZERO_MEMORY);
   }

   void * operator new[](size_t size)
   {
    return heap.Alloc(size,HEAP_ZERO_MEMORY);
   }

   static int delete_counter = 0;

   void  operator delete(void * ptr)
   {
    
    if(ptr )
     if(heap.Free(ptr,0))
        {
         if(delete_counter++>32)
          {heap.HeapCompact(0);delete_counter -=delete_counter;}
        }
   }

   void  operator delete[](void * ptr)
   {
    if(ptr)
     if(heap.Free(ptr,0))
	 {
        if(delete_counter++>32)
          {heap.HeapCompact(0);delete_counter -=delete_counter;}

	 }
   }


/*
   Инициализация DLL
*/

#pragma warn -8057
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fwdreason, LPVOID lpvReserved)
{
	  if(fwdreason == DLL_PROCESS_ATTACH)
		{
		  TLS_INDEX = TlsAlloc();
		}

		if(fwdreason == DLL_THREAD_DETACH)
		{
			//Удалить содержимое TLS_INDEX
			lpexcept_record er =(lpexcept_record)TlsGetValue(TLS_INDEX);
			if(er)
			{
					TlsSetValue(TLS_INDEX,0);
					delete er;
			}

		}

		if(fwdreason == DLL_PROCESS_DETACH)
		{
		  //Завершение процесса
		  TlsFree(TLS_INDEX);

		}
		return 1;
}
#pragma warn .8057

#ifdef _WIN64
extern "C" int _DllMainCRTStartup(HINSTANCE i1,LRESULT i2,LRESULT i3)
{
 return  DllMain((HINSTANCE)i1,(DWORD)i2,(LPVOID)i3);
}
#else
extern "C" int _DllMainCRTStartup(int i1,int i2,int i3)
{
 return  DllMain((HINSTANCE)i1,(DWORD)i2,(LPVOID)i3);
}
#endif

