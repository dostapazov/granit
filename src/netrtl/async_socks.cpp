#include <wsockdef.h>

 void __fastcall TAsyncSocketsThread::set_user_handler(LPVOID user_data,UserHandlerFunc uf)
 {
  if(!IsRunning())
   {
    this->user_data = user_data;user_func = uf;
   }
 }


 bool      __fastcall TAsyncSocketsThread::BeforeExecute()
 {
   if(TThread::BeforeExecute())
   {
    std::vector<TSocketEvents>::iterator se = this->sockets.begin();
    while(se<sockets.end())//установка
    {
     WSAEventSelect(se->socket,se->event,se->event_mask);
     se++;
    }
   //Создаём очередь сообщений
    process_messages();
    return true;
   }
   return false;
 }


 void      __fastcall TAsyncSocketsThread::BeforeTerminate()
 {
  std::vector<TSocketEvents>::iterator se = sockets.begin();
  while(se< sockets.end())
    {
     WSACloseEvent(se->event);
     se++;
    }
  ss.Clear();
 }

 void      __fastcall TAsyncSocketsThread::setup_synchro_set()
 {
  ss.Clear();
  ss+=this->terminate_event();
  ss+=timer();
  this->first_socket_index = ss.GetWaitingCount();
  std::vector<TSocketEvents>::iterator se = sockets.begin();
  while(se< sockets.end())
     {
      ss+=(HANDLE)se->event;
      se++;
     }
 }


 int       __fastcall TAsyncSocketsThread::Execute()
 {
  int ret_code(0);
  setup_synchro_set();
  while(!TerminateRequest)
  {
   DWORD wait_code = ss.MsgWait(INFINITE,QS_ALLINPUT,false,false);
   if(wait_code == DWORD(ss.GetWaitingCount()))
     process_messages();
     else
     {
      if(wait_code == DWORD(-1))
      {
       this->handle_wait_error(GetLastError());
       Sleep(100);
      }
      else
      handle_wait_result(wait_code);
     }
  }
  process_messages();//Очистить
  return ret_code;
 }

#pragma warn -8057
 void      __fastcall TAsyncSocketsThread::handle_wait_error (DWORD error)
 {
  WSAGetLastError();
  Terminate();
 }
#pragma warn .8057

 void      __fastcall TAsyncSocketsThread::handle_wait_result(DWORD wait_code)
 {
      switch(wait_code-WAIT_OBJECT_0)
      {
       case 0:
         TThread::Terminate();
         break;
       case 1://Обработка таймера
        handle_timer();
        break;
       default://Обработка завершения по событию
             {
                DWORD   index = wait_code - WAIT_OBJECT_0 - 2;
                process_socket_events(index);
             }
             break;

      }

 }



 void      __fastcall TAsyncSocketsThread::process_socket_events(DWORD idx)
 {
    if(idx<sockets.size())
    {
          WSANETWORKEVENTS net_events;
          ZeroMemory(&net_events,sizeof(net_events));
          TSocketEvents & se = sockets.at(idx);
          LONG enum_code =  WSAEnumNetworkEvents(se.socket,se.event,&net_events);

         if(enum_code)
         {
          net_events.lNetworkEvents = 0;
          net_events.iErrorCode[0] = WSAGetLastError();
          Sleep(100);
         }

         for(int i = 0,mask=1;i<FD_MAX_EVENTS;i++,mask<<=1)
            {
             LONG fd_event = net_events.lNetworkEvents&(mask);
             if((i>=0 && fd_event) || (i== 0 && enum_code))
             {
             if(user_func == 0 || !user_func(user_data,se.socket,fd_event,net_events.iErrorCode[i]))
              handle_event(se.socket,fd_event,net_events.iErrorCode[i]);
             }
              if(net_events.lNetworkEvents == 0)
                break;
            }
     }       
 }
 void      __fastcall TAsyncSocketsThread::process_messages()
 {
  MSG msg;
  while(get_message(msg))
  {
   if(msg.message == ASTM_TERMINATE)
    TerminateRequest = true;
    handle_message(msg);
  }
 }

 bool      __fastcall TAsyncSocketsThread::get_message   (MSG & msg)
 {
	 return PeekMessage(&msg,0,0,-1,PM_REMOVE) ? true:false;
 }

 LRESULT   __fastcall TAsyncSocketsThread::handle_message(MSG & msg)
 {
   LRESULT ret(0);
   switch(msg.message)
   {
    case ASTM_TERMINATE     : TThread::Terminate();break;
    case ASTM_ADD_SOCKET    : do_add_socket    ((SOCKET)msg.lParam,(DWORD) msg.wParam);break;
    case ASTM_ENABLE_EVENTS : do_enable_events ((SOCKET)msg.lParam,(DWORD) msg.wParam);break;
    case ASTM_DISABLE_EVENTS: do_disable_events((SOCKET)msg.lParam,(DWORD) msg.wParam);break;
    case ASTM_REMOVE_SOCKET : do_remove_socket ((SOCKET)msg.lParam);break;
    case ASTM_START_TIMER   : do_start_timer(msg.lParam,msg.wParam ? true:false);break;
    case ASTM_STOP_TIMER    : do_stop_timer();break;
    default:
    break;
   }
   return ret;
 }

inline  int __fastcall can_read(SOCKET s)
 {
   DWORD len;
   return ioctlsocket(s,FIONREAD,&len) ? 0:int(len);
 }

#ifdef __BORLANDC__
#pragma warn -8004
#endif

 bool      __fastcall TAsyncSocketsThread::handle_event(SOCKET socket,LONG Event,LONG Error)
 {
  bool ret(false);
  if(Error)
  {
   int err;int sz = sizeof(err);
   getsockopt(socket,SOL_SOCKET,SO_ERROR,(char*)&err,&sz);
  }

  switch(Event)
  {
  case FD_READ:
   {
    int rx_len;
    while((rx_len = can_read(socket))!=0)
    {
     char buff[MAX_PATH<<1];
     recv(socket,buff,sizeof(buff),0);

    }
    ret = true;
   }
   break;
   }
   return ret;
 }
#ifdef __BORLANDC__
#pragma warn .8004
#endif


 void __fastcall TAsyncSocketsThread::do_add_socket    (SOCKET s,DWORD ev_mask)
 {
  TSocketEvents se(s,ev_mask,WSACreateEvent());
  sockets.push_back(se);
  WSAEventSelect(s,se.event,ev_mask);
  ss+=(HANDLE)se.event;
 }

 void __fastcall TAsyncSocketsThread::do_enable_events (SOCKET s,DWORD ev_mask)
 {
  for(std::vector<TSocketEvents>::iterator se = sockets.begin();se<sockets.end();se++)
  {
   if(se->socket == s)
   {
    se->event_mask|=ev_mask;
    WSAEventSelect(s,se->event,se->event_mask);
    break;
   }
  }
 }

 void __fastcall TAsyncSocketsThread::do_disable_events(SOCKET s,DWORD ev_mask)
 {
  for(std::vector<TSocketEvents>::iterator  se = sockets.begin();se<sockets.end();se++)
  {
   if(se->socket == s)
   {
    se->event_mask&=~ev_mask;
    WSAEventSelect(s,se->event,se->event_mask);
    break;
   }
  }
 }

 void __fastcall TAsyncSocketsThread::do_remove_socket (SOCKET s)
 {
  for(std::vector<TSocketEvents>::iterator  se = sockets.begin();se<sockets.end();se++)
  {
   if(se->socket == s)
   {
    ss-=(HANDLE)se->event;
    WSAEventSelect(s,se->event,0);
    WSACloseEvent (se->event);
    sockets.erase (se);
    break;
   }
  }
 }


 void __fastcall TAsyncSocketsThread::add_socket    (SOCKET s,DWORD ev_mask)
 {
  if(IsRunning() && GetCurrentThreadId()!= this->Id)
     PostMessage(ASTM_ADD_SOCKET,ev_mask,LPARAM(s));
   else
   do_add_socket(s,ev_mask);
 }

 void __fastcall TAsyncSocketsThread::enable_events (SOCKET  s,DWORD ev_mask)
 {
  if(IsRunning() && GetCurrentThreadId()!= this->Id)
     PostMessage(ASTM_ENABLE_EVENTS,ev_mask,LPARAM(s));
   else
   do_enable_events(s,ev_mask);
 }

 void __fastcall TAsyncSocketsThread::disable_events(SOCKET  s,DWORD ev_mask)
 {
  if(IsRunning() && GetCurrentThreadId()!= this->Id)
     PostMessage(ASTM_DISABLE_EVENTS,ev_mask,LPARAM(s));
   else
   do_disable_events(s,ev_mask);
 }
 void __fastcall TAsyncSocketsThread::remove_socket (SOCKET  s)
 {
  if(IsRunning() && GetCurrentThreadId()!= this->Id)
     PostMessage(ASTM_REMOVE_SOCKET,0,LPARAM(s));
   else
   do_remove_socket(s);
 }

 void __fastcall TAsyncSocketsThread::do_start_timer   (DWORD ms,bool Period )
 {
  if(ms)
    timer.StartPeriodTimer(ms,Period);
    else
    do_stop_timer();
 }

 void __fastcall TAsyncSocketsThread::do_stop_timer    ()
 {
  timer.StopTimer();
 }

 void __fastcall TAsyncSocketsThread::start_timer   (DWORD ms,bool Period )
 {
  if(IsRunning() && GetCurrentThreadId()!= this->Id)
     PostMessage(ASTM_START_TIMER,WPARAM(Period),LPARAM(ms));
     else
     do_start_timer(ms,Period);
 }
 void __fastcall TAsyncSocketsThread::stop_timer    ()
 {
  if(IsRunning() && GetCurrentThreadId()!= this->Id)
     PostMessage(ASTM_STOP_TIMER,0,0);
     else
     do_stop_timer();
 }

 DWORD __fastcall TAsyncSocketsThread::get_socket_events(SOCKET s)
 {
   TSocketEvents * se;
   DWORD ret = 0;
   for(std::vector<TSocketEvents>::iterator se = sockets.begin();se<sockets.end();se++)
      {
        if(se->socket == s)
           {ret = se->event_mask;break;}
      }
   return ret;   
 }









