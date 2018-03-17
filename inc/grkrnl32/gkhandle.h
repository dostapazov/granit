/*
 DLL для работы с GRANIT KERNEL HANDLE
 Остапенко Д.В. 1993-2001
 СДТУ Норильск
*/

#ifndef  _GKHANDLE_INC
#define  _GKHANDLE_INC
 #ifndef  KE_DEFS_INC
 #include <ke_defs.h>
 #endif


/*Команда уведомляющая об уничтожении хендла p2 = TRUE - принудительное освободение*/
#define HANDLE_CMD  0
//p1 = SubCommand
//p2 = arguments
#define HANDLE_SUBCMD_RELEASE     0
#define HANDLE_SUBCMD_LOCK        1 //p2 =  BOOL Try;
#define HANDLE_SUBCMD_UNLOCK      2
#define HANDLE_SUBCMD_LOCKCOUNT   3
#define HANDLE_SUBCMD_FULLUNLOCK  4
#define HANDLE_SUBCMD_RESTORELOCK 5 // p2 LockCount;
#define HANDLE_SUBCMD_GET_OWNTHREADID  6


/*коды возврата*/
#define GKH_RET_ERROR      ((LRESULT)-1)
#define GKH_RET_EXCEPTION  ((LRESULT)-2)
#define GKH_RET_CANCEL     ((LRESULT)-3)
#define GKH_RET_SUCCESS     0
#define GKH_SUCCESS(ret)   ((ret == GKH_RET_SUCCESS) ? TRUE : FALSE)
#define GKH_ERROR  (ret)   ((ret != GKH_RET_SUCCESS) ? TRUE : FALSE)

#define IS_RET_EXCEPTION(x) (x == GKH_RET_EXCEPTION)
#define IS_RET_ERROR(x) ((x == GKH_RET_ERROR || IS_RET_EXCEPTION(x)))
    
//typedef HANDLE GKHANDLE;

typedef unsigned __int64    GKHANDLE;
typedef LRESULT  winapi        ( * pfn_entry_function)   (LPVOID h_data,DWORD cmd,LPARAM p1,LPARAM p2);
typedef BOOL     winapi        ( * pfn_enum_handle_proc) (LPVOID user_data,GKHANDLE handle);
#define INVALID_GKHANDLE_VALUE ((GKHANDLE)-1)
#define IsValidGKHandle(h) ( h !=0 && h!=INVALID_GKHANDLE_VALUE )


#pragma pack(push,8)

typedef struct _gkh_message
{
 DWORD    cmd;
 LPARAM    p1,p2;
}gkh_message,*lpgkh_message;

//Контекст SEH исключения



typedef struct _except_record
{
 DWORD    dw_size;
 GKHANDLE handle;
 DWORD   cmd;
 LPARAM   p1,p2;
#ifdef _Windows
 EXCEPTION_RECORD except_rec;
 CONTEXT          except_ctx;
#endif

 ULONG             esp_dump[16];
 ULONG             ebp_dump[16];
}except_record,* lpexcept_record;




#pragma pack(pop)


typedef DWORD  (winapi * gkhandle_exception_filter)(lpexcept_record);


#ifdef __cplusplus
extern "C" {
#endif

   GKHANDLE winapi create_gkhandleA     (LPVOID data,pfn_entry_function entry,DWORD attr,const char    * name);
   GKHANDLE winapi create_gkhandleW     (LPVOID data,pfn_entry_function entry,DWORD attr,const wchar_t * name);
   GKHANDLE winapi create_gkhandle_tagA (LPVOID data,pfn_entry_function entry,DWORD attr,const char    * name,LARGE_INTEGER * tag);
   GKHANDLE winapi create_gkhandle_tagW (LPVOID data,pfn_entry_function entry,DWORD attr,const wchar_t * name,LARGE_INTEGER * tag);
   BOOL     winapi get_gkhandle_nameA   (GKHANDLE hObj,char   * buffer,int cb_size);
   BOOL     winapi get_gkhandle_nameW   (GKHANDLE hObj,wchar_t* buffer,int cb_size);
   BOOL     winapi set_gkhandle_nameA   (GKHANDLE hObj,const char   * buffer,LARGE_INTEGER tag);
   BOOL     winapi set_gkhandle_nameW   (GKHANDLE hObj,const wchar_t* buffer,LARGE_INTEGER tag);
   GKHANDLE winapi open_gkhandleA       (const char    * name  );
   GKHANDLE winapi open_gkhandleW       (const wchar_t * name  );
   LONG     winapi close_gkhandle       (GKHANDLE  hObj);
   GKHANDLE winapi dup_gkhandle         (GKHANDLE  hObj);
   LRESULT  winapi call_gkhandle        (GKHANDLE  hObj,DWORD cmd,LPARAM p1,LPARAM p2);
   //LRESULT  winapi call_gkhandle_safe (GKHANDLE  hObj,DWORD cmd,LPARAM p1,LPARAM p2);
   BOOL     winapi call_gkhandle_bynameA(const char    * name,DWORD cmd,LPARAM p1,LPARAM p2,LRESULT * ret_code);
   BOOL     winapi call_gkhandle_bynameW(const wchar_t * name,DWORD cmd,LPARAM p1,LPARAM p2,LRESULT * ret_code);
   BOOL     winapi find_gkhandleA       (const char    * name);
   BOOL     winapi find_gkhandleW       (const wchar_t * name);

   LPVOID   winapi get_gkhandle_data    (GKHANDLE hObj);

   DWORD    winapi get_gkhandle_attr    (GKHANDLE  hObj);
   DWORD    winapi set_gkhandle_attr    (GKHANDLE  hObj,DWORD attr);
   DWORD    winapi get_gkhandle_usage   (GKHANDLE);
   void     winapi release_all_gkhandles();
   void     winapi enum_gkhandles       (LPVOID  user_data,pfn_enum_handle_proc);
   BOOL     winapi is_same_gkhandle     (GKHANDLE h1,GKHANDLE h2);
   BOOL     winapi lock_gkhandle        (GKHANDLE h);
   BOOL     winapi unlock_gkhandle      (GKHANDLE h);
   BOOL     winapi try_lock_gkhandle    (GKHANDLE h);
   DWORD    winapi fullunlock_gkhandle  (GKHANDLE h);
   BOOL     winapi restorelock_gkhandle (GKHANDLE h,DWORD cnt);
   DWORD    winapi get_gkhandle_lockcount(GKHANDLE h);
   DWORD    winapi get_owner_threadid   (GKHANDLE h);
   int      winapi gkhandle_compare     (GKHANDLE h1,GKHANDLE h2);
   /*Установка глобального фильтра исключений*/
   gkhandle_exception_filter winapi set_gkhandle_exception_filter (gkhandle_exception_filter fltr);
   /*Установка персонального фильтра исключений*/
   gkhandle_exception_filter winapi set_gkhandle_exception_filterP(gkhandle_exception_filter fltr,GKHANDLE h);
   void     winapi get_gkhandle_except_record (lpexcept_record er);

   pfn_entry_function winapi gkhandle_subclass(GKHANDLE hObj,pfn_entry_function new_func,LPVOID new_data,LPVOID * old_data);
   BOOL               winapi gkhandle_post_message(GKHANDLE,DWORD cmd,LPARAM p1,LPARAM p2);
   BOOL               winapi gkhandle_peek_message(GKHANDLE,lpgkh_message,DWORD timeout,BOOL remove);
   void               winapi gkhandle_process_all_messages(GKHANDLE);


#ifdef UNICODE
  #define create_gkhandle      create_gkhandleW
  #define open_gkhandle        open_gkhandleW
  #define find_gkhandle        find_gkhandleW
  #define get_gkhandle_name    get_gkhandle_nameW
  #define call_gkhandle_byname call_gkhandle_bynameW
#else
  #define create_gkhandle      create_gkhandleA
  #define open_gkhandle        open_gkhandleA
  #define find_gkhandle        find_gkhandleA
  #define get_gkhandle_name    get_gkhandle_nameA
  #define call_gkhandle_byname call_gkhandle_bynameA
#endif

#ifdef __cplusplus
}// end of extern C section
#endif

#endif