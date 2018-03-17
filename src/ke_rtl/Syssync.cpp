// Реализация системных объектов синхронизации
// TEvent,TMutex,TSemaphore
// TWaitableTimer,TFASTMutex(CriticalSection) TSynchroSet
// Ke_rtl.dll Остапенко Д.В. March 2000

#include <kertl.hpp>

#ifndef _NO_NAMESPACE
namespace KeRTL
{
#endif

    HANDLE  __fastcall THandleBased::Dup(bool inherit,HANDLE target ,DWORD Options )
    {
     HANDLE result;
      if(!DuplicateHandle(GetCurrentProcess(),Handle,target,&result,DUPLICATE_SAME_ACCESS,inherit,Options))
          result = INVALID_HANDLE_VALUE;
     return result;
    }

    bool __fastcall TMutex::Lock  (DWORD TimeOut,bool Alertable)
    {
      if(Wait(TimeOut,Alertable) == WaitOk )
        { OwnerThreadId = GetCurrentThreadId();return  true;}
        return false;
    }

    bool __fastcall TMutex::Unlock(bool doit )
    {
      if(doit)
      {
       DWORD tid = OwnerThreadId;
       OwnerThreadId = 0;
       if(ReleaseMutex(Handle))
         return true;
       OwnerThreadId = tid;
       return false;
      }

      if(OwnerThreadId == GetCurrentThreadId())
         {
          OwnerThreadId = 0;
            if(!ReleaseMutex(Handle))
             {
              OwnerThreadId = GetCurrentThreadId();return false;
             }
             return true;
         }
         return false;
    }


    DWORD __fastcall TFASTMutex::FullUnlock()
    {
     DWORD ret = 0;
     while(Unlock())
       ret++;
     return ret;  
    }

    bool  __fastcall TFASTMutex::RestoreLock(DWORD count)
    {
     bool ret = false;
     for(DWORD i = 0;i<count;i++)
        ret = Lock();
     return ret;
    }

    bool __fastcall TFASTMutex::TryLock()
    {
     if(TSysInfo::IsWinNT())
      {
       if(TryEnterCriticalSection(&rcs))
       {
          OwnerThreadId = GetCurrentThreadId();
          return true;
       }
      }
      else
       return Lock();
      return false;
    }


  bool   __fastcall TWaitableTimer::SetTimer(LARGE_INTEGER & due ,DWORD Period,bool Resume)
  {
	  Active = ::SetWaitableTimer(Handle,&due,Period,OnTimer,this,Resume) ? true:false;
   return Active;
  }

  bool   __fastcall TWaitableTimer::StartPeriodTimer(DWORD Period,bool Resume)
  {
   __int64 p(Period);
   p*=10000;
   LARGE_INTEGER li;
   li.QuadPart = -p;
   if(SetTimer(li,Period,Resume))
      Periodic = true;
      return Active;
  }

bool   __fastcall TWaitableTimer::StopTimer       ()
  {
   Active = bool(!CancelWaitableTimer(Handle));
   if(!Active)
      Periodic = false;
   return !Active;
  }

  void WINAPI TWaitableTimer::OnTimer(LPVOID arg,DWORD p1,DWORD p2)
  {
   TWaitableTimer * wt = static_cast<TWaitableTimer*>(arg);
   if(wt && wt->UserOnTimer)
     wt->UserOnTimer(wt->UserData,p1,p2);
  }

          TSynchroSet::TSynchroSet():ObjectsCount(0),InWait(false)
          {ZeroMemory(Objects,sizeof(Objects));}

          TSynchroSet::~TSynchroSet()
          {
           if(InWait)
              throw ("KeRTL::TSynchroSet wait in progress. Can't delete");
          }

  DWORD   __fastcall  TSynchroSet::Wait(DWORD TimeOut,bool All ,bool Alert)
  {
   if(ObjectsCount && !InWait )
     {
      InWait = true;
      DWORD Result = WaitForMultipleObjectsEx(ObjectsCount,Objects,All,TimeOut,Alert);
      InWait = false;
      return Result;
     }
   return WAIT_FAILED;
  }

  DWORD   __fastcall  TSynchroSet::MsgWait(DWORD TimeOut,DWORD MsgFlags,bool All ,bool Alert)
  {

   if( !InWait )
     {
      InWait = true;
      DWORD Result = MsgWaitForMultipleObjectsEx(ObjectsCount,ObjectsCount ? Objects:NULL,TimeOut,MsgFlags,((All ? MWMO_WAITALL :0)|(Alert ? MWMO_ALERTABLE:0)));
      InWait = false;
      return Result;
     }
   return WAIT_FAILED;
  }

  TSynchroSet & TSynchroSet::operator += (const HANDLE handle)
  {
   if(!InWait && ObjectsCount < MAXIMUM_WAIT_OBJECTS && handle && handle!=INVALID_HANDLE_VALUE)
    {
        Objects[ObjectsCount++]=handle;
    }

    return *this;
  }

  TSynchroSet & TSynchroSet::operator -= (const HANDLE handle)
  {
   if(!InWait && handle && handle!=INVALID_HANDLE_VALUE)
   {
     for(int i = 0;i<ObjectsCount;i++)
     {
       if(Objects[i] == handle)
         {
          ObjectsCount--;
          CopyMemory(Objects+i,Objects+i+1,sizeof(HANDLE)*(ObjectsCount-i));
          Objects[ObjectsCount] = 0;
          break;
         }
     }
   }
   return *this;
  }


    bool __fastcall TSemLocker::Lock  (DWORD TimeOut,bool Alertable)
    {
     DWORD tid = GetCurrentThreadId();
     if(this->OwnerThreadId != tid)
     {
      if(TSemaphore::Lock(TimeOut,Alertable))
         {
          OwnerThreadId = tid;
          LockCount++;
         }

     }
     else
     {
      if(tid == this->OwnerThreadId)
      {
       LockCount++;
      }

     }

     return bool (OwnerThreadId==tid && LockCount);
    }

    bool __fastcall TSemLocker::Unlock(bool)
    {

     DWORD tid = GetCurrentThreadId();
     bool ret(false);
     if(tid==OwnerThreadId)
     {
      if(--LockCount==0)
       {
        OwnerThreadId = 0;
        ret = TSemaphore::Unlock();
       }
     }
     return ret;
    }

#ifndef _NO_NAMESPACE
}// end of namespace KeRTL
#endif
