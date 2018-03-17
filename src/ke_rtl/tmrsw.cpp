/*
 Объект синхронизации Много читателей/один писатель
 ex:Писано по Дж. Рихтеру
 16.09.04 Свой алгоритм
 Фишка в двух событиях с автосбросом
 Для Блокировки на записть ждёмс  сигнального состояния 2 событий
 Для блокировки на чтение  ждёмс  любого сигнала , что автоматически заблокирует запись.
*/

#include <Kertl.hpp>

#ifdef __BORLANDC__
//#include <values.h>
#endif



#ifndef _NO_NAMESPACE
namespace KeRTL
{
#endif

  #ifdef _DEBUG
  LONG    TMrSw::dboe = 0;
  void    TMrSw::enable_dbo( bool e){ if(e) InterlockedIncrement(&dboe); else InterlockedDecrement(&dboe);};
  #endif

  inline  LONG atom_cmp_xchg(LPLONG val,LONG xc_val,LONG cmp_val)
  {
    return
    #ifdef __BORLANDC__
		InterlockedCompareExchange(val,xc_val,cmp_val);
    #else
	   (LONG) InterlockedCompareExchange((LPVOID*)val,(LPVOID)xc_val,(LPVOID)cmp_val);
    #endif
  }


  TMrSw::TMrSw(LPSECURITY_ATTRIBUTES sa):owner_tid(0),writer(0),reader(0),wr_waiting(0),rd_waiting(0)
  {
   sync[0] = CreateSemaphoreA(sa,1,1,NULL);
   sync[1] = CreateEventA    (sa,FALSE,TRUE,NULL);
  };

  TMrSw::~TMrSw()
  {
   CloseHandle(sync[1]);
   CloseHandle(sync[0]);
  }

  #pragma warn -8004
  bool  __fastcall  TMrSw::LockForRead(DWORD timeout ,bool Alertable )
  {
   /*Блокировка на чтение*/
   bool ret;
   long  tid      = GetCurrentThreadId();
   LONG  rdw      = InterlockedIncrement(&rd_waiting);
   if(atom_cmp_xchg(&owner_tid,tid,tid) != tid)
   {
   DWORD wr       = WaitForMultipleObjectsEx(2,sync,atom_cmp_xchg(&wr_waiting,wr_waiting,wr_waiting)? TRUE:FALSE,timeout,Alertable ? TRUE:FALSE);
   switch(wr)
   {
    case (WAIT_OBJECT_0+1) : SetEvent(sync[1]);
    case  WAIT_OBJECT_0    : if(InterlockedIncrement(&reader)==1)
                                owner_tid = tid; //added
                             ret = true;
                             break;
   }
   }
   else
   {InterlockedIncrement(&reader);ret = true;}
   rdw = InterlockedDecrement(&rd_waiting);

   #ifdef _DEBUG
   if(ret && dboe)
    {
//     char _text[MAX_PATH];
//     wsprintf(_text,"LockForRead %8X",GetCurrentThreadId());
//     OutputDebugString(_text);
    }
   #endif

   return ret;
  }
  #pragma warn .8004

  #pragma warn -8004
  bool  __fastcall  TMrSw::ReadUnlock()
  {
   bool ret = false;
   /*Разблокировка на чтение*/
   long  tid      = GetCurrentThreadId();
   if(atom_cmp_xchg(&reader,reader,reader) && InterlockedDecrement(&reader)==0)
   {
     if(atom_cmp_xchg(&writer,0,0)==0 )
     {
        owner_tid = 0;
        ReleaseSemaphore(sync[0],1,NULL);
        SetEvent(sync[1]);
     }
    ret = true;
   }
   #ifdef _DEBUG
//   if(dboe)
//    {
//    char _text[MAX_PATH];
//    wsprintf(_text,"ReadUnlock %8X %d %d %d",GetCurrentThreadId(),reader,rd_waiting,wr_waiting);
//    OutputDebugString(_text);
//    }
   #endif
   return ret;
  }
  #pragma warn .8004


  bool  __fastcall  TMrSw::LockForWrite(DWORD timeout ,bool Alertable )
  {
   long tid = GetCurrentThreadId();
   bool ret = false;
   InterlockedIncrement(&wr_waiting);
   if(atom_cmp_xchg(&owner_tid,tid,tid)==tid ||
      WaitForMultipleObjectsEx(2,sync,TRUE,timeout,Alertable ?  TRUE:FALSE) != WAIT_TIMEOUT)
     {
       ret = true;
       writer++;
       owner_tid = tid;
     }
   InterlockedDecrement(&wr_waiting);
   #ifdef _DEBUG
//   if(ret && dboe)
//    {
//    char _text[MAX_PATH];
//    wsprintf(_text,"LockForWrite %8X",GetCurrentThreadId());
//    OutputDebugString(_text);
//    }
   #endif
   return ret;
  }

  bool  __fastcall  TMrSw::WriteUnlock()
  {

   bool ret = false;
   long tid = GetCurrentThreadId();
   if(atom_cmp_xchg(&owner_tid,tid,tid)==tid)
   {
    if((--writer) == 0)
      {
       ret = true;
       owner_tid = 0;
       if(atom_cmp_xchg(&reader,0,0)==0)
          ReleaseSemaphore(sync[0],1,NULL);
       SetEvent(sync[1]);
      }
   }
   #ifdef _DEBUG
//   if(dboe)
//    {
//    char _text[MAX_PATH];
//    wsprintf(_text,"WriteUnlock() %8X",GetCurrentThreadId());
//    OutputDebugString(_text);
//    }
   #endif
   return ret;
  }


#ifndef _NO_NAMESPACE
//namespace KeRTL
}
#endif
