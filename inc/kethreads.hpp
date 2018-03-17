#ifndef __KETHREADS_HPP
#define __KETHREADS_HPP

#include <KeRTL.hpp>
#include <vector>

#ifndef _NO_NAMESPACE
 namespace KeRTL {
#endif
using namespace std;

 #define KERTL_THREAD_ENABLERESTART  0x00000001
 #define KERTL_THREAD_SELFDESTRUCT   0x00000002
 KERTL_CLASS  TThread:public THandleBased
 {

    protected:
     int          State;
     LONG         TerminateRequest;
     DWORD        Id;
     DWORD        ExceptionCode;

    protected:
    virtual  int   __fastcall Execute  ()=0;
    virtual  bool  __fastcall BeforeExecute();
    virtual  void  __fastcall BeforeTerminate();
    virtual  DWORD __fastcall OnException(DWORD ExceptionCode,LPEXCEPTION_POINTERS);
             void  __fastcall Release();
    public:
    enum TThreadState    {tsCreating,tsRunning,tsSuspended,tsSleeping,tsTerminating,tsTerminated};
    enum TThreadPriority
    {tpAboveNormal  = THREAD_PRIORITY_ABOVE_NORMAL,
     tpBelowNormal  = THREAD_PRIORITY_BELOW_NORMAL,
     tpHighest      = THREAD_PRIORITY_HIGHEST,
     tpIdle         = THREAD_PRIORITY_IDLE,
     tpLowest       = THREAD_PRIORITY_LOWEST,
     tpNormal       = THREAD_PRIORITY_NORMAL,
     tpTimeCritical = THREAD_PRIORITY_TIME_CRITICAL
    };

                TThread();
               ~TThread();
              bool  __fastcall TerminateAndWait(DWORD TimeOut = INFINITE,bool =false);
     virtual  void  __fastcall Terminate();
     virtual  bool  __fastcall CheckTerminateRequest();
              bool  __fastcall Start(DWORD StackSize = 0,bool Suspended = false,LPSECURITY_ATTRIBUTES  sa = 0);
              bool  __fastcall Suspend();
              bool  __fastcall Resume();
              bool  __fastcall SetThreadPriority(DWORD);
              DWORD __fastcall GetThreadPriority();
              DWORD __fastcall SetAffinityMask(DWORD);
              DWORD __fastcall SetIdealProcessor(DWORD);
              int   __fastcall GetExitCode();
              DWORD __fastcall Sleep(DWORD to,bool Alert = false);
              int   __fastcall GetState();
              bool  __fastcall PostMessage(UINT msg,WPARAM wp,LPARAM lp);
              void  __fastcall EnableRestart(bool r=false);
              DWORD __fastcall GetExceptionCode();
              bool  __fastcall IsRunning(){return (State>TThread::tsCreating && State<TThread::tsTerminating) ? true:false;}
              bool  __fastcall WaitFor(DWORD time_out=INFINITE);
              DWORD __fastcall GetID() const{return Id;}
     protected:
     static       DWORD WINAPI ThreadStarter(TThread*);
 };


 struct thread_less
 {
  bool __fastcall operator ()(const TThread & th1,const TThread & th2)
  {return (th1.GetID()<th2.GetID()) ? true:false;}
  bool __fastcall operator ()(const TThread * th1,const TThread * th2)
  {return (th1->GetID()<th2->GetID()) ? true:false;}

 };

 /****************************************************************************
                          Список нитей
 *****************************************************************************/
 typedef void (__fastcall * pfn_thread_del)(TThread * );

 KERTL_CLASS TThreadsList
 {
  protected:
  TFASTMutex      locker;
  vector<TThread*> list;
  public:
  TThreadsList(){};
  ~TThreadsList(){terminate_all();clear();}
  bool      __fastcall lock(bool tr = false);
  bool      __fastcall unlock();
  TThread * __fastcall pop_back();
  DWORD     __fastcall push_back (TThread*  th);
  TThread * __fastcall get_front(bool remove = false);
  DWORD     __fastcall add(TThread * th);
  TThread * __fastcall remove(DWORD idx);
  bool      __fastcall remove(TThread * th);
  DWORD     __fastcall get_count();
  virtual
  bool      __fastcall terminate_all(bool wait = true,DWORD wto=INFINITE);
  void      __fastcall clear(bool del = true,pfn_thread_del = NULL);
 };

/******************************************************************************

    Пул потоков

*******************************************************************************/

KERTL_CLASS TThreadsPool;
/*Абстрактный класс  нити входящей в состав пула потоков*/
KERTL_CLASS TPooledThread:public TThread
{
 protected:
 TThreadsPool * owner;
 HANDLE         term_event;/*Событие завершения*/
 bool           __fastcall need_terminate();/*Проверка необходимости завершения*/
 int            __fastcall Execute();
 virtual
 void           __fastcall do_work()=0;
 virtual
 void           __fastcall done_work(){/*Предназначена для освобождения рабочих данных*/}
 DWORD __fastcall OnException(DWORD ExceptionCode,LPEXCEPTION_POINTERS);
 public:
 TPooledThread(TThreadsPool * _own):owner(_own),term_event(NULL){};
 void   __fastcall set_term_event(HANDLE te);
 virtual
 bool           __fastcall begin_work(DWORD p,LPVOID arg1,LPVOID arg2) = 0;
};

KERTL_CLASS TQueuedPoolData
{
 public:
 virtual ~TQueuedPoolData(){release();}
 virtual bool __fastcall push_data(DWORD p ,LPVOID  arg1,LPVOID  arg2)      = 0;
 virtual bool __fastcall pop_data (DWORD& p,LPVOID& arg1,LPVOID& arg2)      = 0;
 virtual bool __fastcall free_data(DWORD p ,LPVOID  arg1,LPVOID  arg2)      = 0;
 void __fastcall release() ;
};

#define PT_UNLIMITED ((DWORD)-1)

typedef TPooledThread* (WINAPI * pfn_thread_creator)(TThreadsPool *);


KERTL_CLASS TThreadsPool
{
 public:
 enum    start_result {sr_success,sr_pending,sr_fail};
 protected:
 DWORD                 limit; /*Ограничитель кол_ва одновременно работающих нитей*/
 DWORD                 grow_size;
 TEvent                common_term_event;/*Событие завершения для всех*/
 TThreadsList          worked_threads;
 TThreadsList          ready_threads;
 TThreadsList          _free_threads;
 pfn_thread_creator    pfn_new_thread;
 pfn_thread_del        pfn_del_thread;
 LPSECURITY_ATTRIBUTES sa;
 bool                  terminating;

 TQueuedPoolData      *queue;
 TPooledThread * __fastcall get_free_thread();

 public:
 TThreadsPool(pfn_thread_creator fc,TQueuedPoolData* qd = 0,LPSECURITY_ATTRIBUTES _sa = NULL,DWORD gs = 4,DWORD limit = PT_UNLIMITED);
~TThreadsPool(){release();};
 virtual start_result __fastcall start(DWORD p,LPVOID arg1,LPVOID arg2);
 virtual bool         __fastcall make_free(TPooledThread*);
         HANDLE       __fastcall get_term_event(){return common_term_event();}
 bool            __fastcall terminate(DWORD wto);
 void            __fastcall release();
 void            __fastcall set_delete_proc(pfn_thread_del pfn_dt){pfn_del_thread = pfn_dt;}

};



/*inline implementation*/

 inline  TThread::TThread():
 State(tsCreating),TerminateRequest(0)
 {
 }

 inline void   __fastcall TThread::Release()
 {
   TerminateAndWait(1000,true);
   THandleBased::Release();
 }

 inline bool __fastcall TThread::Suspend()
 {
  if(State==tsRunning)
     {
      State = tsSuspended;
      SuspendThread(Handle);
     };
   return bool(State == tsSuspended);
 }

 inline bool __fastcall TThread::Resume()
 {
  if(State==tsSuspended)
     {if(ResumeThread(Handle)) State = tsRunning;};
   return bool(State == tsRunning);
 }

  inline int   __fastcall TThread::GetState()
  {return State;}


 inline bool   __fastcall TThread::SetThreadPriority(DWORD tp)
 {  return ::SetThreadPriority(Handle,tp)? true:false;}

 inline DWORD  __fastcall TThread::GetThreadPriority()
 { return ::GetThreadPriority(Handle);}

 inline DWORD __fastcall TThread::SetAffinityMask(DWORD mask)
 {
   mask = TSysInfo::IsWinNT() ? mask:1;
   return ::SetThreadAffinityMask(Handle,mask);
 }


 inline DWORD __fastcall TThread::SetIdealProcessor(DWORD ProcNum)
 {
  #ifdef __BORLANDC__
   ProcNum = MIN(TSysInfo::GetProcessorCount(),ProcNum);
   return TSysInfo::IsWinNT() ? ::SetThreadIdealProcessor(Handle,ProcNum):1;
  #else
   //::SetThreadIdealProcessor(Handle,ProcNum);
    return 0;
  #endif

 }

 inline  int   __fastcall TThread::GetExitCode()
 {
  int ec = STILL_ACTIVE;
  ::GetExitCodeThread(Handle,LPDWORD(&ec));
  return ec;
 }

 inline  DWORD __fastcall TThread::Sleep(DWORD to,bool Alert)
 {
  int old_state = -1;
  DWORD cid = GetCurrentThreadId();
  if(cid==Id)
     {
      old_state = int(State);
      State     = tsSleeping;
     }
  DWORD ret = ::SleepEx(to,Alert);
  if(cid==Id)
      State     = old_state;
  return ret;
 }


 inline   TThread::~TThread()
 {
  if(State>tsCreating
	 && State < tsTerminating
	 && GetExitCode() == int( STILL_ACTIVE )
	 )
    {
     TerminateAndWait(2000,true);
    }
   Close();
 }

inline bool  __fastcall TThread::Start(DWORD StackSz,bool Suspended,LPSECURITY_ATTRIBUTES sa)
 {
   if(State==tsCreating || State == tsTerminated)
   {
      if(IsValid())
         CloseHandle(Handle);
      Handle = CreateThread
      (sa,StackSz,(LPTHREAD_START_ROUTINE)ThreadStarter,this,Suspended ? CREATE_SUSPENDED:0,&Id);
   }


   if(IsValid())
       State = Suspended ? tsSuspended:tsRunning;
       else
       State = tsTerminated;
   return IsValid();
 }

inline void  __fastcall TThread::EnableRestart(bool r)
{
   SetFlags(KERTL_THREAD_ENABLERESTART,r);

}


inline    bool      __fastcall TThreadsList::lock(bool tr )
  {return locker.Lock(tr);}

inline    bool      __fastcall TThreadsList::unlock()
  {return locker.Unlock();}

inline    DWORD     __fastcall TThreadsList::get_count()
  {
   lock();
   DWORD ret = list.size();
   unlock();
   return ret;
  }
inline  TThread * __fastcall TThreadsList::pop_back()
{
 TThread * ret = NULL;
 lock();
 if(list.size())
   {
    ret = list.back();list.pop_back();
   }
 unlock();
 return ret;
}

inline  DWORD     __fastcall TThreadsList::push_back (TThread*  th)
{
  DWORD ret;
  lock();
  ret = list.size();
  list.push_back(th);
  unlock();
  return ret;
}

inline  TThread * __fastcall TThreadsList::get_front(bool remove )
{
 TThread * ret = NULL;
 lock();
 if(list.size())
   {ret = list.front();list.erase(list.begin());}
 unlock();
 return ret;
}

inline  TThreadsPool::TThreadsPool(pfn_thread_creator fc,TQueuedPoolData* qd,LPSECURITY_ATTRIBUTES _sa,DWORD gs,DWORD _limit ):
limit(_limit),common_term_event(true,false),pfn_new_thread(fc),sa(_sa),grow_size(gs),queue(qd),terminating(false),pfn_del_thread(NULL)
{};


#ifndef _NO_NAMESPACE
}
#endif


#endif
