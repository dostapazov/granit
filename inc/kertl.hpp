 //Интерфейсы системных классов

#ifndef _KERTL_INC
 #define _KERTL_INC

 #define KERTL_VERSION 001 //Определение версии
 #include <ke_defs.h>
 #include <winbase.h>

/*

  Определение _NO_NAMESPACE отменяет пространство имен KeRTL, KrnlWin
  Определение _NATIVE_W2K   Реализует нормальную поддержку Win2k
  
*/


#ifndef _NO_NAMESPACE
 namespace KeRTL {
#endif

 KERTL_FUNC DWORD      __fastcall GetKeRTLVersion();
 KERTL_FUNC bool       __fastcall GetPtrModuleName(LPVOID  ptr,char  * dest,int ccMax);
 KERTL_FUNC bool       __fastcall GetPtrModuleName(LPVOID  ptr,wchar_t * dest,int ccMax);
 KERTL_FUNC HINSTANCE  __fastcall GetPtrModuleHandle(LPVOID ptr);
 KERTL_FUNC char *     __fastcall GetFileNameFromPath(LPCSTR  FullPath);
 KERTL_FUNC wchar_t *  __fastcall GetFileNameFromPath(LPCWSTR  FullPath);
 KERTL_FUNC int        __fastcall Unicode2Ansi(LPSTR  out, LPCWSTR in);
 KERTL_FUNC int        __fastcall Ansi2Unicode(LPWSTR out, LPCSTR  in);
 KERTL_FUNC int        __fastcall Unicode2Ansi(LPSTR  out,int sz, LPCWSTR in);
 KERTL_FUNC int        __fastcall Ansi2Unicode(LPWSTR out,int sz, LPCSTR  in);
 KERTL_FUNC __int64    __fastcall GetTime(bool System = false);
 KERTL_FUNC int        __fastcall GetErrorMessage (DWORD Error,char * buffer,int ccMax,HINSTANCE fromModule = 0);
 KERTL_FUNC int        __fastcall GetErrorMessage (DWORD Error,char * buffer,int ccMax,char * module );
 KERTL_FUNC int        __fastcall GetErrorMessage (DWORD Error,wchar_t * buffer,int ccMax,HINSTANCE module );
 KERTL_FUNC int        __fastcall ShowErrorMessage(HWND parent,DWORD ErrCode,int Flags = MB_OK,wchar_t * caption = NULL,HINSTANCE from_mod = NULL);
 KERTL_FUNC BOOL       __fastcall EnableAnimation (BOOL e);
 KERTL_FUNC bool       __fastcall ForceDirs( wchar_t * target,LPSECURITY_ATTRIBUTES sa=0);
 KERTL_FUNC bool       __fastcall ForceDirs( char    * target,LPSECURITY_ATTRIBUTES sa=0);
 KERTL_FUNC bool       __fastcall ForceDeleteDir(char * dir_name);
 KERTL_FUNC bool       __fastcall ForceDeleteDir(wchar_t * dir_name);
 KERTL_FUNC bool       __fastcall ClearDir      (char * dir_name,char * wild_card);
 KERTL_FUNC bool       __fastcall ClearDir      (wchar_t * dir_name,wchar_t * wild_card);

 KERTL_FUNC HANDLE     __fastcall DupHandle(HANDLE src,BOOL = false,DWORD =  GENERIC_READ|GENERIC_WRITE);
 KERTL_FUNC char       __fastcall digit2hexchar(DWORD dig);
 KERTL_FUNC DWORD      __fastcall hexchar2digit(char ch);
 KERTL_FUNC DWORD      __fastcall hexchar2digit(wchar_t ch);
 KERTL_FUNC DWORD      __fastcall hextoi(const wchar_t* ch);
 KERTL_FUNC DWORD      __fastcall hextoi(const char   * ch);
 KERTL_FUNC DWORD      __fastcall octtoi(const wchar_t* ch);
 KERTL_FUNC DWORD      __fastcall octtoi(const char   * ch);
 KERTL_FUNC int        __fastcall bytes2hexstr(LPBYTE ma,int ma_sz,char * outs,int out_sz,char delim ='-');
 KERTL_FUNC char*      __fastcall newstr(char * __s);
 KERTL_FUNC wchar_t*   __fastcall newstr(wchar_t * __s);
 KERTL_FUNC void       __fastcall secunds2HMS(DWORD secs, char    * text,int tsz); 
 KERTL_FUNC void       __fastcall secunds2HMS(DWORD secs, wchar_t * text,int tsz);
 KERTL_FUNC int        __fastcall str2addr(char * str,LPBYTE addr,DWORD addr_sz,int delim);
 KERTL_FUNC int        __fastcall str2addr(wchar_t * str,LPBYTE addr,DWORD addr_sz,int delim);
 KERTL_FUNC void*      __fastcall safe_memcpy(void * dest, const void * src,size_t sz);
 KERTL_FUNC wchar_t *  __fastcall safe_strcpy(wchar_t * dest,const wchar_t * src);
 KERTL_FUNC char    *  __fastcall safe_strcpy(char    * dest,const  char   * src);
 KERTL_FUNC wchar_t *  __fastcall safe_strcpyn(wchar_t * dest,const wchar_t * src,size_t lim);
 KERTL_FUNC char    *  __fastcall safe_strcpyn(char    * dest,const  char   * src,size_t lim);

 KERTL_FUNC char    *  __fastcall itoa_radix(int value, char    * out,int radix);
 KERTL_FUNC wchar_t *  __fastcall itoa_radix(int value, wchar_t * out,int radix);
 KERTL_FUNC int        __fastcall atoi_radix(char *    __str,int radix);
 KERTL_FUNC int        __fastcall atoi_radix(wchar_t * __str,int radix);
 KERTL_FUNC WORD           WINAPI calc_kpk  (void * Buffer,unsigned short len,unsigned short Del);

inline KERTL_FUNC wchar_t *  __fastcall safe_strstr(wchar_t * s1,const wchar_t * s2)
{
  return wcsstr(s1,s2);
}

inline KERTL_FUNC char    *  __fastcall safe_strstr(char    * s1,const  char   * s2)
{
    return strstr(s1,s2);
}

inline KERTL_FUNC wchar_t *  __fastcall safe_strcat(wchar_t * s1,const wchar_t * s2)
{
  return wcscat(s1,s2);
}

inline KERTL_FUNC char    *  __fastcall safe_strcat(char    * s1,const  char   * s2)
{
    return strcat(s1,s2);
}

inline KERTL_FUNC int       __fastcall safe_strlen(const wchar_t * s)
{
  return wcslen(s);
}

inline KERTL_FUNC int       __fastcall safe_strlen(const  char   * s)
{
  return strlen(s);
}


 
  


 KERTL_CLASS  TSysInfo
 {
   protected:
    static OSVERSIONINFO osvi;
    static SYSTEM_INFO   si;
    public   :
    TSysInfo();
    enum TPlatform {pWin32s = VER_PLATFORM_WIN32s ,
                    pWin95  = VER_PLATFORM_WIN32_WINDOWS,
                    pWinNT  = VER_PLATFORM_WIN32_NT};

    enum TProcessor {prIntel,prMips,prAlpha,prPPC,prSHX,prARM,prUnknown=0xFFFF};

    static int                 __fastcall GetMajorVersion   ();
    static int                 __fastcall GetMinorVersion   ();
    static int                 __fastcall GetBuildNumber    ();
    static TPlatform           __fastcall GetPlatform       ();
    static const char  *       __fastcall GetCSDVersion     ();
    static bool                __fastcall IsWinNT    ();
    static bool                __fastcall IsWin95 ();
    static bool                __fastcall IsWin32s();

    static TProcessor          __fastcall GetProcessorArchitecture();
    static DWORD               __fastcall GetPageSize             ();
    static DWORD               __fastcall GetProcessorCount       ();
    static DWORD               __fastcall GetActiveProcessorMask  ();
    static DWORD               __fastcall GetProcessorType        ();
    static  WORD               __fastcall GetProcessorLevel       ();
    static  WORD               __fastcall GetProcessorRevision    ();
    static LPVOID              __fastcall GetMinAppAddress        ();
    static LPVOID              __fastcall GetMaxAppAddress        ();
    static DWORD               __fastcall GetAllocationGranularity();

 };





 //Базовый класс для Events,Semaphore,Mutex,Thread
enum TWaitResult {WaitFailed = WAIT_FAILED , WaitOk = WAIT_OBJECT_0,
                  WaitTimeOut= WAIT_TIMEOUT, WaitAbandoned = WAIT_ABANDONED};

 #define IsValidHandle(h) ( h !=0 && h!=INVALID_HANDLE_VALUE )


 KERTL_CLASS THandleBased: public TKeRTLBase
 {
  protected:
      HANDLE Handle;
      void __fastcall Release(){Close();};
  public:
      THandleBased(HANDLE Handle=INVALID_HANDLE_VALUE);
      virtual  ~THandleBased();
      virtual   void __fastcall Close();
      HANDLE  operator ()()const;
      HANDLE       __fastcall Dup(bool inherit,HANDLE target=GetCurrentProcess(),DWORD Options = DUPLICATE_SAME_ACCESS );
      bool         __fastcall IsValid()
      { return (Handle!=0 && Handle!=INVALID_HANDLE_VALUE) ? true:false;}
      HANDLE  operator = (THandleBased & hb);
      bool __fastcall  IsSignaledState();
 };

 KERTL_CLASS  THeap : public THandleBased
 {
    public:
             THeap(DWORD InitSize ,DWORD MaxSize=0,DWORD Options=0);
             THeap(HANDLE heap);
			 THeap();
             ~THeap(){Close();};
    void __fastcall SetHeap(HANDLE);
    bool __fastcall Create(DWORD InitSize = 0,DWORD MaxSize=0,DWORD Options=0);
    void __fastcall Close();
    LPVOID  __fastcall Alloc  (DWORD szBytes,DWORD Flags = 0);
    LPVOID  __fastcall Realloc(LPVOID memptr,DWORD szNewSize,DWORD Flags = 0);
    BOOL    __fastcall Free   (LPVOID memptr,DWORD Flag  = 0);
    BOOL    __fastcall Validate(LPVOID memptr,DWORD Flag = 0);
    DWORD   __fastcall MemSize (LPVOID memptr,DWORD Flags = 0);

    BOOL    __fastcall HeapLock   ();
    BOOL    __fastcall HeapUnlock ();
    UINT    __fastcall HeapCompact(DWORD Flags = 0);
    BOOL    __fastcall HeapWalk   (LPPROCESS_HEAP_ENTRY);
    DWORD   __fastcall GetMemoryUsed();

 };

 KERTL_CLASS  TSysSync:public THandleBased
 {
  protected:
        DWORD OwnerThreadId;
        TSysSync(HANDLE Handle = INVALID_HANDLE_VALUE);
  public:
        virtual bool __fastcall Lock  (DWORD TimeOut = INFINITE,bool Alertable = false) = 0;
        virtual bool __fastcall Unlock(bool = false) = 0;
        TWaitResult  __fastcall Wait  (DWORD TimeOut = INFINITE,bool Alertable=false);
        DWORD        __fastcall GetOwnerThreadId(){return OwnerThreadId;}
 };


 KERTL_CLASS TEvent:public TSysSync
 {
  public :
         TEvent();
         TEvent(bool ManualReset,bool State ,const char * name = 0,
                           LPSECURITY_ATTRIBUTES sa = 0);
         TEvent(bool ManualReset,bool State ,const wchar_t * name  ,
                           LPSECURITY_ATTRIBUTES sa = 0);
         TEvent(const char    * name,DWORD Access,bool inherit = false);
         TEvent(const wchar_t * name,DWORD Access,bool inherit = false);
         TEvent(HANDLE src,bool dup_handle = true);

    bool __fastcall SetEvent(bool state);
    bool __fastcall Lock  (DWORD TimeOut  = INFINITE,bool Alertable = false);
    bool __fastcall Unlock(bool=false);
    bool __fastcall PulseEvent();
    TEvent & operator  = (TEvent & src){Close();Handle = DupHandle(src());return *this;}
 };

 //template <typename lock_type>
 class TLockHelper
 {
   TSysSync & lock_object;
   public:
   TLockHelper (TSysSync & lo ):lock_object(lo) { lock  ();}
   ~TLockHelper(  ) { unlock();}
       void __fastcall  lock  (){lock_object.Lock()  ;}
       void __fastcall  unlock(){lock_object.Unlock();}

 };

 KERTL_CLASS TMutex:public TSysSync
 {
  public:
          TMutex();
          TMutex(bool Locked ,const char * name=0,LPSECURITY_ATTRIBUTES  sa = 0);
          TMutex(bool Locked ,const wchar_t * name =0,LPSECURITY_ATTRIBUTES  sa = 0);
          TMutex(const char * name,DWORD Access,bool inherit = true);
          TMutex(const wchar_t * name,DWORD Access,bool inherit = true);
    bool __fastcall Lock  (DWORD TimeOut  = INFINITE,bool Alertable = false);
    bool __fastcall Unlock(bool = false);
    bool __fastcall lock(){return Lock();}
    bool __fastcall unlock(){return Unlock();}
 };



 KERTL_CLASS TFASTMutex:public TSysSync
 {
  protected:
  _RTL_CRITICAL_SECTION rcs;
  public:
     TFASTMutex ();
     ~TFASTMutex();
    bool  __fastcall Lock  (DWORD TimeOut  = INFINITE,bool Alertable = false);
    bool  __fastcall Unlock(bool=false);
    bool  __fastcall TryLock();
    int   __fastcall GetLockCount();
    DWORD __fastcall FullUnlock();
    bool  __fastcall RestoreLock(DWORD count);
    bool __fastcall lock(){return Lock();}
    bool __fastcall unlock(){return Unlock();}

    _RTL_CRITICAL_SECTION & operator()(){return rcs;}
 };


 KERTL_CLASS TSemaphore:public TSysSync
 {
   public:
          TSemaphore();
          TSemaphore(int InitCount,int MaxCount,const char * name=0,LPSECURITY_ATTRIBUTES sa = 0);
          TSemaphore(const char * name,DWORD Access,bool inherit = true);
          TSemaphore(int InitCount,int MaxCount,const wchar_t * name,LPSECURITY_ATTRIBUTES sa = 0);
          TSemaphore(const wchar_t * name,DWORD Access,bool inherit = true);
    bool __fastcall Lock  (DWORD TimeOut  = INFINITE,bool Alertable = false);
    bool __fastcall Unlock(bool=false);
    bool __fastcall ReleaseSem(int count );
 };

 KERTL_CLASS TSemLocker :public TSemaphore
 {
  int LockCount;
  public:
   TSemLocker();
    bool __fastcall Lock  (DWORD TimeOut  = INFINITE,bool Alertable = false);
    bool __fastcall Unlock(bool=false);
 };



 KERTL_CLASS TWaitableTimer:public TSysSync
 {
  static void WINAPI OnTimer(LPVOID,DWORD,DWORD);
  LPVOID UserData;
  PTIMERAPCROUTINE UserOnTimer;
  bool   Active;
  bool   Periodic;
  public:
           TWaitableTimer (bool ManualReset,const char * name = 0,LPSECURITY_ATTRIBUTES sa = 0);
           TWaitableTimer (const char * name,DWORD Access,bool inherit = true);
           TWaitableTimer (bool ManualReset,const wchar_t * name ,LPSECURITY_ATTRIBUTES sa = 0);
           TWaitableTimer (const wchar_t * name,DWORD Access,bool inherit = true);
           virtual ~TWaitableTimer();
  bool   __fastcall SetTimer        (LARGE_INTEGER & due ,DWORD Period,bool Resume = false);
  bool   __fastcall StartPeriodTimer(DWORD Period,bool Resume = false);
  bool   __fastcall StopTimer       ();
  bool   __fastcall SetTimerAPC     (PTIMERAPCROUTINE apc = 0,LPVOID data = 0);
  bool   __fastcall IsActive  ();
  bool   __fastcall IsPeriodic();
  bool   __fastcall Lock  (DWORD TimeOut,bool Alertable = false){return Wait(TimeOut,Alertable) ? true:false;};
  bool   __fastcall Unlock(bool=false){return false;};

 };


 KERTL_CLASS TSynchroSet
 {
  protected:
  HANDLE  Objects[MAXIMUM_WAIT_OBJECTS+1];
  int     ObjectsCount;
  bool    InWait;
  public:
            TSynchroSet();
           ~TSynchroSet();
  DWORD   __fastcall  Wait(DWORD TimeOut,bool All = false,bool Alert = false);
  DWORD   __fastcall  MsgWait(DWORD TimeOut,DWORD msgFlags,bool All = false,bool Alert = false);
  TSynchroSet & operator += (const HANDLE);
  TSynchroSet & operator += (const THandleBased &);
  TSynchroSet & operator -= (const HANDLE);
  TSynchroSet & operator -= (const THandleBased &);
  int     __fastcall GetWaitingCount(){return ObjectsCount;};
  HANDLE  & operator [](int i);
  bool    __fastcall Clear();
  HANDLE * operator()(){return Objects;}
 };

 KERTL_CLASS TMrSw
 {
  protected:
  LONG        owner_tid;
  LONG        writer;     //Счётчик блокировок на запись
  LONG        reader;     //Счётчик читателей
  LONG        wr_waiting; //счётчик ожидающих записи
  LONG        rd_waiting; //Счётчик ожидающих чтения
  HANDLE      sync[2];

  public:
   TMrSw(LPSECURITY_ATTRIBUTES sa = NULL);
  ~TMrSw();
  bool  __fastcall  LockForRead(DWORD timeout = INFINITE,bool Alertable = false);
  bool  __fastcall  ReadUnlock();
  bool  __fastcall  LockForWrite(DWORD timeout = INFINITE,bool Alertable = false);
  bool  __fastcall  WriteUnlock();
  long  __fastcall  GetReadCount () {return InterlockedExchangeAdd(&reader,0);}
  long  __fastcall  GetWriteCount() {return InterlockedExchangeAdd(&writer,0);}
  void  __fastcall  Reset()
  {rd_waiting = wr_waiting = owner_tid = writer = reader = 0;}

  #ifdef _DEBUG
  static LONG   dboe;
  static void   enable_dbo( bool e);
  #endif

 };

 class rd_lock
 {
  TMrSw * locker;
  bool    locked;
  rd_lock(const TMrSw & ){locker = NULL,locked = false;}
  public:
  rd_lock(TMrSw & _locker,DWORD to = INFINITE):locker(&_locker),locked(false){ locked = locker->LockForRead(to);}
  ~rd_lock(){if(locked && locker) locker->ReadUnlock();}
  bool operator()(){return locked;}
 };

 class wr_lock
 {
  TMrSw * locker;
  bool    locked;
  wr_lock(const TMrSw & ){locker = NULL,locked = false;}
  public:
  wr_lock(TMrSw & _locker,DWORD to = INFINITE):locker(&_locker),locked(false){ locked = locker->LockForWrite(to);}
  ~wr_lock(){if(locked && locker) locker->ReadUnlock();}
  bool operator()(){return locked;}
 };






 KERTL_CLASS TModule :public TKeRTLBase
 {
   protected:
   HINSTANCE hModule;
   public   :
   TModule():hModule(NULL){}
   TModule(HINSTANCE mod ):hModule(mod){SetFlags(FLBASE_NODELETE,mod? true:false);};
   TModule(const char    * name,DWORD Flags = 0){Load(name,Flags);}
   TModule(const wchar_t * name,DWORD Flags = 0){Load(name,Flags);}
   ~TModule(){Unload();}
   bool       __fastcall IsLoaded(){return hModule ? true : false;};
   HINSTANCE  __fastcall GetInstance(){return hModule;}
   virtual   void __fastcall AfterLoad(){};
   virtual   void __fastcall BeforeUnload(){};
   virtual   bool __fastcall CanUnload();
   bool       __fastcall Load(const char       * name,DWORD Flags = 0);
   bool       __fastcall Load(const wchar_t    * name,DWORD Flags = 0);
   bool       __fastcall Unload();
   bool       __fastcall UnloadAndExitThread(DWORD ExitCode);
   void       __fastcall Release();
   FARPROC    __fastcall GetProcAddress(const char    * proc_name);
   FARPROC    __fastcall GetProcAddress(const wchar_t * proc_name);
   HRSRC      __fastcall FindResoure(const char * rType,const char * rName,WORD Lang = MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL));
   HRSRC      __fastcall FindResoure(const wchar_t * rType,const wchar_t * rName,WORD Lang = MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL));
 };



 /*Интерфейс класса TProcess*/
 KERTL_CLASS TProcess:public THandleBased
 {
  protected:
  DWORD  process_id;
  DWORD  thread_id;
  HANDLE process_thread;
  public:
  TProcess():THandleBased(NULL) ,process_id(0),thread_id(0),process_thread(INVALID_HANDLE_VALUE)
  {};
  TProcess(HANDLE hProcess):THandleBased(hProcess) ,process_id(0),thread_id(0),process_thread(INVALID_HANDLE_VALUE)
  {
   if(IsValid())
   {
     process_id    = GetProcessId(hProcess);
   }
  };
  virtual ~TProcess(){Release();}
  bool __fastcall Create(const char * exename, const char * cmd_line = NULL,
                         const char * environment = NULL,const char * curdir = NULL,
                         DWORD c_flags = NORMAL_PRIORITY_CLASS ,bool inherit = false,
                         LPSTARTUPINFOA stpi = NULL,
                         LPSECURITY_ATTRIBUTES proc_sa=NULL,
                         LPSECURITY_ATTRIBUTES thread_sa = NULL);
  bool __fastcall Create(const wchar_t * exename,const wchar_t * cmd_line = NULL,
                         const wchar_t * environment = NULL,const wchar_t * curdir = NULL,
                         DWORD c_flags = NORMAL_PRIORITY_CLASS ,bool inherit = false,
                         LPSTARTUPINFOW stpi = NULL,
                         LPSECURITY_ATTRIBUTES proc_sa=NULL,
                         LPSECURITY_ATTRIBUTES thread_sa = NULL);
  void  __fastcall Close();
  bool  __fastcall GetProcessInfo  (PROCESS_INFORMATION * pi);
  bool  __fastcall WaitForTerminate(DWORD TimeOut,bool alert=false){return WaitForSingleObjectEx(Handle,TimeOut,alert) ? false:true;}
  bool  __fastcall WaitStartup(DWORD TimeOut = INFINITE){ return WaitForInputIdle(Handle,TimeOut) ? false:true;}
  DWORD __fastcall GetExitCode() { DWORD ret = -1;if(IsValid()) GetExitCodeProcess(Handle,&ret); return ret; }
 };

/***************************** Inline inplementation **************************/

 inline      THandleBased:: THandleBased(HANDLE aHandle):Handle(aHandle)
 {}


 inline      THandleBased::~THandleBased()
 {Close();}

 inline     HANDLE     THandleBased::operator ()()const
 {return Handle==INVALID_HANDLE_VALUE ? 0:Handle;}

 inline     HANDLE  THandleBased::operator = (THandleBased & hb)
 {
  if(IsValidHandle(Handle))
     Close();
  Handle = hb.Dup(true);
  return Handle;
 }

 inline  bool __fastcall  THandleBased::IsSignaledState()
 {
  return IsValid() &&  WaitForSingleObject(Handle,0) == WAIT_OBJECT_0;
 }



inline    THeap::THeap(DWORD InitSize ,DWORD MaxSize,DWORD Options)
{
 Create(InitSize,MaxSize,Options);
}

inline THeap::THeap(HANDLE heap )
{
 SetHeap(heap);
}

inline THeap::THeap():THandleBased(INVALID_HANDLE_VALUE)
{
}

inline void __fastcall THeap::SetHeap(HANDLE heap)
{
 if(IsValid())
  Close();
  Handle = heap;
  if(IsValid())
   SetFlags(FLBASE_NODELETE,true);

}

inline  bool __fastcall  THeap::Create(DWORD InitSize ,DWORD MaxSize,DWORD Options)
{
 if(InitSize == 0)   InitSize = TSysInfo::GetPageSize();
 if(MaxSize!=0 && MaxSize < InitSize)  MaxSize = InitSize;
 Handle = HeapCreate(Options,InitSize,MaxSize);
 return IsValid();
}

inline void __fastcall THeap::Close()
{
  if(!this->IsFlagsSet(FLBASE_NODELETE) && IsValid())
  {
   if(TSysInfo::IsWinNT()) ::HeapUnlock(Handle);
    HeapDestroy(Handle);
  }
  Handle = INVALID_HANDLE_VALUE;
  SetFlags(FLBASE_NODELETE,false);

}


inline    LPVOID  __fastcall THeap::Alloc  (DWORD szBytes,DWORD Flags )
{
 LPVOID ret = HeapAlloc(Handle,Flags,szBytes);
 #ifdef _DEBUG
  if(!ret)
    DebugBreak();
 #endif
 return ret;
}


inline    BOOL    __fastcall THeap::Free   (LPVOID memptr,DWORD Flag  )
   {return HeapFree(Handle,Flag,memptr);}

inline    LPVOID  __fastcall THeap::Realloc(LPVOID memptr,DWORD szNewSize,DWORD Flags )
   {return ::HeapReAlloc(Handle,Flags,memptr,szNewSize);}

inline    BOOL    __fastcall THeap::Validate(LPVOID memptr,DWORD Flag )
   {return (memptr && ::HeapValidate(Handle,Flag,memptr)) ? TRUE:FALSE; }

inline    BOOL    __fastcall THeap::HeapLock   ()
   {return ::HeapLock(Handle);}

inline    BOOL    __fastcall THeap::HeapUnlock ()
   {return ::HeapUnlock(Handle);}

inline    UINT    __fastcall THeap::HeapCompact(DWORD Flags )
   { return ::HeapCompact(Handle,Flags); }

inline    BOOL    __fastcall THeap::HeapWalk   (LPPROCESS_HEAP_ENTRY he)
   { return ::HeapWalk(Handle,he); }

inline    DWORD   __fastcall THeap::MemSize   (LPVOID memptr,DWORD Flags )
    { return ::HeapSize(Handle,Flags,memptr);}

inline               TSysSync::TSysSync(HANDLE Handle ):THandleBased(Handle),
                         OwnerThreadId(0){}

inline TWaitResult  __fastcall TSysSync::Wait  (DWORD TimeOut,bool Alertable)
 {
   if(Handle && Handle!=INVALID_HANDLE_VALUE)
     return TWaitResult(WaitForSingleObjectEx(Handle,TimeOut,Alertable));
    return WaitFailed;
 }


inline        TEvent::TEvent():
                    TSysSync(CreateEvent(0,FALSE,FALSE,0)){}

inline        TEvent::TEvent(bool ManualReset,bool State ,const char * name ,
                                   LPSECURITY_ATTRIBUTES sa ):
                    TSysSync(CreateEventA(sa,ManualReset,State,name)){}

inline        TEvent::TEvent(const char * name,DWORD Access,bool inherit):
                    TSysSync(OpenEventA(Access,inherit,name)){}

inline        TEvent::TEvent(bool ManualReset,bool State ,const wchar_t * name ,
                                   LPSECURITY_ATTRIBUTES sa ):
                    TSysSync(CreateEventW(sa,ManualReset,State,name)){}

inline        TEvent::TEvent(const wchar_t * name,DWORD Access,bool inherit):
                    TSysSync(OpenEventW(Access,inherit,name)){}

inline        TEvent::TEvent(HANDLE src,bool dup_handle ){Handle = src; if(dup_handle) Handle = Dup(false);}



inline  bool __fastcall TEvent::SetEvent(bool state)
{return (state ? ::SetEvent(Handle) : ::ResetEvent(Handle)) ? true:false; }

inline  bool __fastcall TEvent::Lock  (DWORD TimeOut,bool Alertable)
    {return this->Wait(TimeOut,Alertable) == WaitOk? true : false; }

inline  bool __fastcall TEvent::Unlock(bool)
    {return SetEvent(true) ;}

inline  bool __fastcall TEvent::PulseEvent()
    {return ::PulseEvent(Handle) ? true : false;}


inline   TMutex::TMutex():TSysSync(CreateMutex(0,FALSE,0)){}

inline   TMutex::TMutex(bool Locked ,const char * name,LPSECURITY_ATTRIBUTES  sa ):
         TSysSync(CreateMutexA(sa,Locked,name))
         { OwnerThreadId = Locked ? GetCurrentThreadId():0;}

inline   TMutex::TMutex(const char * name,DWORD Access,bool inherit ):
         TSysSync(OpenMutexA(Access,inherit,name)) {}

inline   TMutex::TMutex(bool Locked ,const wchar_t * name,LPSECURITY_ATTRIBUTES  sa ):
         TSysSync(CreateMutexW(sa,Locked,name))
         { OwnerThreadId = Locked ? GetCurrentThreadId():0;}

inline   TMutex::TMutex(const wchar_t * name,DWORD Access,bool inherit ):
         TSysSync(OpenMutexW(Access,inherit,name)) {}

inline   TSemaphore::TSemaphore():
         TSysSync(CreateSemaphore(0,1,1,0)){};

inline   TSemaphore::TSemaphore(int InitCount,int MaxCount,const char * name,LPSECURITY_ATTRIBUTES sa ):
         TSysSync(CreateSemaphoreA(sa,InitCount,MaxCount,name)){};

inline   TSemaphore::TSemaphore(const char * name,DWORD Access,bool inherit):
                     TSysSync(OpenSemaphoreA(Access,inherit,name)){}

inline   TSemaphore::TSemaphore(int InitCount,int MaxCount,const wchar_t * name,LPSECURITY_ATTRIBUTES sa ):
         TSysSync(CreateSemaphoreW(sa,InitCount,MaxCount,name)){};

inline   TSemaphore::TSemaphore(const wchar_t * name,DWORD Access,bool inherit):
                     TSysSync(OpenSemaphoreW(Access,inherit,name)){}

inline    bool __fastcall TSemaphore::Lock  (DWORD TimeOut,bool Alertable)
{return bool(Wait(TimeOut,Alertable)==WaitOk);}

inline    bool __fastcall TSemaphore::Unlock(bool)
{return ::ReleaseSemaphore(Handle,1,0)? true : false;}

inline    bool __fastcall TSemaphore::ReleaseSem(int count)
{return ::ReleaseSemaphore(Handle,count,0)? true : false;}


inline     TWaitableTimer::TWaitableTimer (bool ManualReset,const char * name ,LPSECURITY_ATTRIBUTES sa ):
                     TSysSync(CreateWaitableTimerA(sa,ManualReset,name)),
                     UserData(0),UserOnTimer(0),Active(false),Periodic(false)
         {}

inline     TWaitableTimer::TWaitableTimer (const char * name,DWORD Access,bool inherit ):
                     TSysSync(OpenWaitableTimerA(Access,inherit,name)),
                     UserData(0),UserOnTimer(0),Active(false),Periodic(false)
         {}

inline     TWaitableTimer::TWaitableTimer (bool ManualReset,const wchar_t * name ,LPSECURITY_ATTRIBUTES sa ):
                     TSysSync(CreateWaitableTimerW(sa,ManualReset,name)),
                     UserData(0),UserOnTimer(0),Active(false),Periodic(false)
         {}

inline     TWaitableTimer::TWaitableTimer (const wchar_t * name,DWORD Access,bool inherit ):
                     TSysSync(OpenWaitableTimerW(Access,inherit,name)),
                     UserData(0),UserOnTimer(0),Active(false),Periodic(false)
         {}

inline   TWaitableTimer::~TWaitableTimer ()
         { StopTimer();}


inline  bool   __fastcall TWaitableTimer::SetTimerAPC     (PTIMERAPCROUTINE apc ,LPVOID data )
  {
   if(!Active)
     {UserData = data; UserOnTimer = apc; }
     return bool(!Active);
  }

inline  bool   __fastcall TWaitableTimer::IsActive  ()
  {return Active;}

inline  bool   __fastcall TWaitableTimer::IsPeriodic()
  {return Periodic;}

inline  TSynchroSet & TSynchroSet::operator += (const THandleBased & hb)
{return *this+=hb();}

inline  TSynchroSet & TSynchroSet::operator -= (const THandleBased & hb)
{return *this-=hb();}

inline    TSemLocker::TSemLocker():TSemaphore(1,1,(char*)0),LockCount(0)
{}

inline   HANDLE & TSynchroSet::operator[](int i)
{
 if(!InWait && i>=0 && i<ObjectsCount)
     return Objects[i];
     return Objects[MAXIMUM_WAIT_OBJECTS];
}

inline bool    __fastcall TSynchroSet::Clear()
{
 if(!InWait)
    {
     ZeroMemory(Objects,sizeof(Objects));
     ObjectsCount = 0;return true;
    }
    return false;
}

/* TFastMutex inline implementation*/

 inline   TFASTMutex::TFASTMutex():TSysSync(INVALID_HANDLE_VALUE)
    {
     ZeroMemory(&rcs,sizeof(rcs));
     OwnerThreadId = -1;
     InitializeCriticalSection(&rcs);
    }

 inline    TFASTMutex::~TFASTMutex()
    {
     FullUnlock();
     DeleteCriticalSection(&rcs);
    }


 inline    bool __fastcall TFASTMutex::Lock  (DWORD tmo,bool)
    {
         if(!rcs.DebugInfo)
            DebugBreak();
          DWORD tid = GetCurrentThreadId();
//      #ifdef _DEBUG
//          int time_out = 50;
//          while(!TryEnterCriticalSection(&rcs))
//          {
//            if(--time_out<0)
//               {
//                TRACE("Can't lock TFASTMutex : locked Thread %X",OwnerThreadId);
//                //DebugBreak();
//                return false;
//               }
//               else
//               Sleep(100);
//          }
//      #else
          EnterCriticalSection(&rcs);
//      #endif
#ifdef _MSC_VER
		  InterlockedCompareExchange((LPVOID*)&OwnerThreadId,(LPVOID)tid,(LPVOID)-1);
#else
		  InterlockedCompareExchange((LPLONG)&OwnerThreadId,tid,-1);
#endif
        //#ifdef _DEBUG
        //  if(this->rcs.RecursionCount>1)  TRACE("Reenter TFASTMutex ",0);
        //#endif
         return true;
    }

 inline    bool __fastcall TFASTMutex::Unlock(bool )
    {
       DWORD tid = GetCurrentThreadId();
#ifdef _MSC_VER
	   if(((DWORD)InterlockedCompareExchange((LPVOID*)&OwnerThreadId,(LPVOID)tid,(LPVOID)tid))==tid)  //Ошибка Исправлено 1.03.2001
#else
       #pragma warn -8012
       if(InterlockedCompareExchange((LPLONG)&OwnerThreadId,tid,tid)==tid)
       #pragma warn .8012

#endif
       {
         if(rcs.RecursionCount ==1)
            OwnerThreadId = -1;

         LeaveCriticalSection(&rcs);
         return true;
       }
       return false;
    }

 inline    int   __fastcall TFASTMutex::GetLockCount()
    {

     if(GetOwnerThreadId() == GetCurrentThreadId())
      return  rcs.RecursionCount;
     return 0;
    }



/*************************** TThread implementation ***************************/

/*TModule*/

  inline  bool       __fastcall TModule::CanUnload()
  {
   return !IsFlagsSet(FLBASE_NODELETE);
  }

  inline  bool       __fastcall TModule::Load(const char       * name,DWORD Flags )
  {
   hModule = LoadLibraryExA(name,0,Flags);
   if(hModule)
     AfterLoad();
   return IsLoaded();
  }

  inline  bool       __fastcall TModule::Load(const wchar_t    * name,DWORD Flags )
  {
   hModule = LoadLibraryExW(name,0,Flags);
   if(hModule)
     AfterLoad();
   return IsLoaded();
  }
  inline  bool       __fastcall TModule::Unload()
  {
   if( IsLoaded() && CanUnload() )
       {
        BeforeUnload();
        FreeLibrary(hModule);
        hModule = 0;return true;
       }
       return false;
  }
  inline  bool       __fastcall TModule::UnloadAndExitThread(DWORD ExitCode)
  {
   if( IsLoaded() && CanUnload() )
    {
     BeforeUnload();
     FreeLibraryAndExitThread(hModule,ExitCode);hModule = 0;return true;
    }
    return false;
  }

  inline  void       __fastcall TModule::Release()
  {   Unload();  }

  inline  FARPROC    __fastcall TModule::GetProcAddress(const char    * proc_name)
  {
   return ::GetProcAddress(hModule,proc_name);
  }

  inline  HRSRC      __fastcall TModule::FindResoure(const char * rType,const char * rName,WORD Lang )
  {   return ::FindResourceExA(hModule,rType,rName,Lang);  }

  inline  HRSRC      __fastcall TModule::FindResoure(const wchar_t * rType,const wchar_t * rName,WORD Lang )
  {   return ::FindResourceExW(hModule,rType,rName,Lang);  }


inline   void __fastcall TProcess::Close()
{
 THandleBased::Close();
 if(IsValidHandle(process_thread))
    {CloseHandle(process_thread);process_thread = INVALID_HANDLE_VALUE;};
}

inline  bool __fastcall TProcess::GetProcessInfo(PROCESS_INFORMATION * pi)
  {
   if(IsValidHandle(this->Handle) && pi)
    {
     pi->hProcess = Handle;
     pi->hThread  = this->process_thread;
     pi->dwProcessId = this->process_id;
     pi->dwThreadId  = this->thread_id;
     return true;
    }
    return false;
  }


inline  HANDLE     __fastcall DupHandle(HANDLE src,BOOL Inherit ,DWORD acc )
{
 HANDLE ret;
 HANDLE process = GetCurrentProcess();
 if(!DuplicateHandle(process,src,process,&ret,acc,Inherit,DUPLICATE_SAME_ACCESS))
      ret = INVALID_HANDLE_VALUE;
 return ret;     
}

inline HKEY __fastcall DupKey(HKEY key)
{
  HKEY nk = (HKEY) DupHandle(HANDLE(key),FALSE);
  return nk == INVALID_HANDLE_VALUE ? key:nk;
}


inline  char*      __fastcall newstr(char * __s)
{
 int len    = 0;
 char * ret = NULL;
 if(__s ) len = lstrlenA(__s);
 if(len ) {ret = new char[len+1];lstrcpyA(ret,__s);}
 return ret;
}

inline  wchar_t*   __fastcall newstr(wchar_t * __s)
{
 int len       = NULL;
 wchar_t * ret = NULL;
 if(__s ) len = lstrlenW(__s);
 if(len ) {ret = new wchar_t[len+1];lstrcpyW(ret,__s);}
 return ret;
}



#ifndef _NO_NAMESPACE
 }// end of Namespace
#endif

#endif



