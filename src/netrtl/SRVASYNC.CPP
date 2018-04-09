/*-----------------------------------------------------------------------------
 Unit Name: SRVASYNC
 Author:    Администратор
 Purpose:
 History:
-----------------------------------------------------------------------------*/


#include <wsockdef.h>

#ifndef HWND_MESSAGE
#define HWND_MESSAGE 0
#endif


  TSockAsyncThread::TSockAsyncThread(SOCKET s):
  wt(false),
  NetEvent(WSACreateEvent()),
  CloseEvent(false,false,(char*)0)
  {
   UserData    = 0;
   UserHandler = 0;
   EventMask = 0;
   PrevProc  = 0;
   socket = s;
   wnd =0;
   if(KeRTL::TSysInfo::IsWinNT()==false)
   {
    //Не под NT Будем использовать окно
	wnd = CreateWindow(L"STATIC",L"Asynchronous socket  helper Window",0,0,0,0,0,HWND_MESSAGE,0,0,0);
	#ifdef _WIN64
	PrevProc = (WNDPROC)SetWindowLongPtr(wnd,GWLP_WNDPROC,LONG(AsyncWndProc));
	SetWindowLongPtr(wnd,GWLP_USERDATA,LONG(this));
	#else
	PrevProc = (WNDPROC)SetWindowLong(wnd,GWL_WNDPROC,LONG(AsyncWndProc));
	SetWindowLong(wnd,GWL_USERDATA,LONG(this));
	#endif
   }
 }

  TSockAsyncThread::~TSockAsyncThread()
  {
   if(this->State == tsRunning)
     TerminateAndWait(1000,true);
   WSACloseEvent(NetEvent);
   if(wnd)
     {
      DestroyWindow(wnd);
     }
  }


  int  __fastcall TSockAsyncThread::Execute  ()
  {
   //SetThreadPriority(tpHigest);
   //SetThreadPriority(tpAboveNormal);
   SetThreadPriority(tpTimeCritical);//Опасно
   CloseEvent.SetEvent(false);
   HANDLE Events[3];
   Events[0] = CloseEvent();
   Events[1] = HANDLE(NetEvent);
   Events[2] = wt();

   //Начало работы
    SelectAsyncEvent(true);
    do{
       //Ожидание события
       DWORD WaitResult = WSAWaitForMultipleEvents(3,(LPWSAEVENT)Events,FALSE,INFINITE,FALSE);
       if(WaitResult!=WSA_WAIT_FAILED)
        {
          WaitResult-=WSA_WAIT_EVENT_0;
          switch(WaitResult)
          {
           case 1:
                 //Это событие от сети
                 if(Locker.Lock(1000)) //- не надо ?????
                 {
                   //Вызов процедуры внешнего обработчика
                   EnumEvents();
                   WSAResetEvent(NetEvent);//Сбросить события
                   DWORD Mask = 1;
                   for(int i = 0;i<FD_MAX_EVENTS;i++)
                   {
                    if(EventsOccured.lNetworkEvents & Mask)
                    {
                     if(!UserHandler || UserHandler(UserData,socket,Mask,EventsOccured.iErrorCode[i]))
                         DefaultEventHandler(socket,Mask,EventsOccured.iErrorCode[i]);
                    }
                     Mask<<=1;
		     
                   }
                   Locker.Unlock();
                 }
                 break;
           case 2:
                 //Это таймер;
                 if(Locker.Lock(1000))
                 {
                  if(UserHandler)
                     UserHandler(UserData,socket,FD_ASYNC_TIMER,0);
                  Locker.Unlock();
                 }
                 break;
          }
        }
        else
        Sleep(10);

      }while(this->TerminateRequest==false);


   //Завершение работы
    SelectAsyncEvent(false);

   return 0;
  }

  bool __fastcall TSockAsyncThread::DefaultEventHandler(SOCKET socket,DWORD Event,DWORD Error)
  {
    char  buf[2048*2];
	int sz;
    if(Error)
	{
		sz = sizeof(int);
		getsockopt(socket,SOL_SOCKET,SO_ERROR,buf,&sz);
	}
	if(Event == FD_READ)
    {
      while(recv(socket,buf,sizeof(buf),0)!=-1)
         ;
    }
    return true;
  }


  LRESULT CALLBACK TSockAsyncThread::AsyncWndProc(HWND w,UINT msg,WPARAM wp,LPARAM lp)
  {

	LPVOID  vptr = (LPVOID)
	#ifdef _WIN64
	 GetWindowLongPtr(w,GWLP_USERDATA)
	#else
	GetWindowLong(w,GWL_USERDATA)
	#endif
	 ;
	TSockAsyncThread * thread = reinterpret_cast<TSockAsyncThread*>(vptr);

	if(thread)
    {
      if(msg == WM_SOCKETMSG)
      {
       DWORD nevent = LOWORD(lp);
       DWORD Error  = HIWORD(lp);
       if(thread->Lock(1000))
        {
         thread->EventsOccured.lNetworkEvents = nevent;
         DWORD Mask = 1;
         for(int i = 0;i<FD_MAX_EVENTS;i++)
          {
           if(nevent&Mask)
              thread->EventsOccured.iErrorCode[i] = Error;
           Mask<<=1;
          }
          WSASetEvent   (thread->NetEvent);
          thread->Unlock();
        }
      }
      else
      return thread->PrevProc(w,msg,wp,lp);
    }
    return DefWindowProc(w,msg,wp,lp);
  }

  void __fastcall TSockAsyncThread::EnumEvents()
  {
   if(KeRTL::TSysInfo::IsWinNT())
   {
    WSAEnumNetworkEvents(socket,NetEvent,&this->EventsOccured);
   }
  }

  bool __fastcall TSockAsyncThread::SetUserHandler(SocketExternHandler handler,LPVOID data)
  {
   if(Locker.Lock(1000))
   {
    this->UserHandler = handler;
    UserData          = data;
    Locker.Unlock();
   }
   return false;
  }

  bool __fastcall TSockAsyncThread::SetTimer(DWORD mSek,bool Period)
  {
    return wt.StartPeriodTimer(mSek,Period);
  }

  void  __fastcall TSockAsyncThread::StopTimer()
  {
    wt.StopTimer();
  }

  bool __fastcall TSockAsyncThread::EnableEvents(DWORD Mask,bool Enable)
  {
    if(Enable)
      EventMask|=Mask;
      else
      EventMask&=~Mask;
      return SelectAsyncEvent(true);

  }

       bool __fastcall TSockAsyncThread::SelectAsyncEvent(bool enable)
       {
          int ret;
          if(wnd)
            ret = WSAAsyncSelect(socket,wnd,WM_SOCKETMSG   ,enable ? EventMask : 0) ? false:true;
             else
            ret = WSAEventSelect(socket,NetEvent,enable ? EventMask : 0)? false:true;
            if(ret)
             WSAGetLastError();
            return ret ? true:false; 
       }

  SOCKET __fastcall TSockAsyncThread::SetSocket(SOCKET s)
  {
   SOCKET _s = this->socket;
   socket = s;
   return _s;
  }


