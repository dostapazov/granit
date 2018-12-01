#include "grnet232.hpp"
#include <crcunit.h>
#include <gkwind.hpp>

//#pragma  hdrstop
using namespace KeWin;


  bool    TGrNetBase::UnderNT = (GetVersion()< 0x80000000) ? true:false;

  WSADATA TGrNetBase::wsadata ={0};
  int              TGrNetBase::WinSockUsageCount=0;

  bool  __fastcall TGrNetBase::InitWS()
  {
   if(WinSockUsageCount==0)
   {
    //Инициализация WinSock
    if(0==WSAStartup(MAKEWORD(2,0),&wsadata))
     WinSockUsageCount++;
   }
   else
    WinSockUsageCount++;
   return WinSockUsageCount ? true:false;
  }

  void  __fastcall TGrNetBase::DeinitWS()
  {
   if(WinSockUsageCount )
   {
    if(--WinSockUsageCount == 0)
      WSACleanup();
   }
  }



#ifdef __BORLANDC__
#pragma warn -8071
#endif

void TGRNetBuffer::CalcCheckSum()
 {
  CheckSum = 0;//Необходимо обнулять
  CheckSum = CalcKPK((BYTE*)this,GetTotalLen(),64709);
 }

#ifdef __BORLANDC__
#pragma warn .8071
#endif

WORD TGRNetBuffer::IsValid()
 {
  if(this->msg.IsValid())
  {
   WORD OldCheckSum = CheckSum;
   CalcCheckSum();
   return (CheckSum == OldCheckSum);
  }
  return 0;
 }

  TGrNetBase * __fastcall TGrNetBase::CAST(GRNET_OBJ obj)
  {
   TGrNetBase * gb = reinterpret_cast<TGrNetBase*>(obj);
   if(gb && gb->SecretDword == NetSecret)
      return gb;
   return 0;
  }


/*
 Реализация базового класса
*/

  TGrNetBase::TGrNetBase(WORD SockNumber,const char * name,TEventSimpleDataQueue* _queue):SecretDword(NetSecret),
  socket_number(SockNumber),
  close_event(true,false),
  secund_timer(false),
  socket_event(WSACreateEvent()),
  rx_queue(_queue),
  RetryCount(DEFAULT_RETRY_COUNT),
  WaitRcptTime(DEFAULT_WAIT_RECEIPT_TIME),
  WatchDogTime(DEFAULT_WDT_TIME),
  socket_window(0),
  socket_window_subclass_proc(0)
  {
   if(!rx_queue)
   {
    alloc_queue = true;
    rx_queue = new TEventSimpleDataQueue;
   }
   SetName(name);

  }

  void        __fastcall TGrNetBase::Release()
  {
   lock();
   __try{
   if(alloc_queue)
    delete rx_queue;
    }
   __finally
   {
   rx_queue = 0;
   alloc_queue = false;
   WSACloseEvent(socket_event);
   unlock();
   }
  }



  int          __fastcall TGrNetBase::Execute()
  {
   do{
      int WaitResult = int(synchro_set.MsgWait(INFINITE,QS_ALLINPUT)-WAIT_OBJECT_0);

      if(WaitResult>=0 && WaitResult <=synchro_set.GetWaitingCount())
      {
       if(WaitResult == synchro_set.GetWaitingCount())
         Messages();
       TerminateRequest = ! this->OnWaitEvent(DWORD(WaitResult));
      }
      else
      {
       Sleep(10);
      }
     }while(TerminateRequest==false);
     return 1;
  }

  bool         __fastcall TGrNetBase::BeforeExecute()
  {
   bool ret(false);
   if(socket_event && socket_event!=WSA_INVALID_EVENT)
   {
   lock();
   synchro_set.Clear();
   close_event.SetEvent(false);
   synchro_set+=close_event();
   synchro_set+=secund_timer();
   secund_timer.StartPeriodTimer(1000,true);
   synchro_set+=(HANDLE)this->socket_event;
   WSAResetEvent(socket_event);

   if(InitWS() && CreateSocket() )
   {
    //Создание гнезда
    input_buffer = new BYTE [max_frame_size+1];
    output_buffer = new BYTE[max_frame_size+1];
    if(alloc_queue)
       this->rx_queue->DropData();//Сброс приемного буфера если владелец
        Messages();

    ret = (input_buffer &&output_buffer) ? true:false;
   }
   unlock();
   }
   return ret;
  }

  void         __fastcall TGrNetBase::BeforeTerminate()
  {
   lock();
   secund_timer.StopTimer();
   synchro_set.Clear();
   DestroySocket();
   DeinitWS();
   if(input_buffer)
      delete [] input_buffer;
   if(output_buffer)
    delete [] output_buffer;
   input_buffer = output_buffer = 0;
   unlock();
  }

  bool        __fastcall TGrNetBase::CreateSocket()
  {
   bool ret(false);
   lock();
   socket_handle = socket(AF_IPX,SOCK_DGRAM,NSPROTO_IPX);
   if(socket_handle!=INVALID_SOCKET)
   {
    int addr_len = sizeof(local_address);
    this->local_address.sa_socket = htons(this->socket_number);
    if(bind(socket_handle,(sockaddr*)&local_address,addr_len))
     {
      GetSocketError();
      DestroySocket ();
     }
     else
     {

      getsockname(socket_handle,(sockaddr*)&local_address,&addr_len);
      addr_len = sizeof(max_frame_size);
      //Размер кадра
      if(getsockopt (socket_handle,NSPROTO_IPX,IPX_MAXSIZE,(char*)&max_frame_size,&addr_len))
         max_frame_size = 547;
      //Разрешить вещание
      BOOL bBroadcast = TRUE;
      setsockopt(socket_handle,SOL_SOCKET,SO_BROADCAST,(char*)&bBroadcast,sizeof(BOOL));
      if(UnderNT)
      {
       ret = WSAEventSelect(socket_handle,socket_event,FD_READ) ? false:true;

      }
      else
      {
       //создание окна
       socket_window = CreateWindow("STATIC","",0,0,0,0,0,0,0,0,0);
       if(socket_window)
        {
         SetWindowLong(socket_window,GWL_USERDATA,LONG(this));
         socket_window_subclass_proc = (WNDPROC)SetWindowLong(socket_window,GWL_WNDPROC,LONG(socket_window_proc));
         WSAAsyncSelect(socket_handle,socket_window,SOCKET_MSG,FD_READ);
		 ret = true;
        }
      }

     }
   }
   else
   {
    socket_handle = 0;
    GetSocketError();
   }
   unlock();
   return ret;
  }


  bool        __fastcall TGrNetBase::DestroySocket()
  {
   /*Уничтожение гнезда*/
   bool ret(false);
   lock();
   if(socket_handle)
   {
    if(UnderNT)
    {
     WSAEventSelect(socket_handle,socket_event,0);
     WSAResetEvent(socket_event);
    }
    else
    {
      WSAAsyncSelect(socket_handle,socket_window,0,0);
      DestroyWindow(socket_window);
      socket_window = 0;
      socket_window_subclass_proc = 0;
     //WSAASyncSelect()
     //DestroyWindow();
    }

    shutdown(socket_handle,SD_BOTH);
    if( closesocket(socket_handle))
    {
     GetSocketError();
    }
    else
    {
     ret = true;
     socket_handle = 0;
    }
   }
   unlock();
   return ret;

  }


  void        __fastcall TGrNetBase::OnSecundTimer()
  {
    WorkTime++;
  }

  bool        __fastcall TGrNetBase::OnWaitEvent(DWORD WaikupNumber)
  {
    switch(WaikupNumber)
        {
        case 0://Требование завершения
                return false;
        case 1: OnSecundTimer();
                break;
        case 2: {
                 //Обработка сетевых событий
                 WSANETWORKEVENTS nw_events={0};
                 WSAEnumNetworkEvents(socket_handle,socket_event,&nw_events);
                 if(nw_events.lNetworkEvents&FD_READ)
                    HandleSocketEvent(FD_READ,nw_events.iErrorCode[FD_READ_BIT]);
                 if(WSAResetEvent(socket_event))
                    GetSocketError();
                }
                break;

        }
        return true;
  }

  void        __fastcall TGrNetBase::GetSocketError()
  {
   if(socket_handle)
   {
     int len = sizeof(this->LastSocketError);
     getsockopt(socket_handle,SOL_SOCKET,SO_ERROR,(char*)&LastSocketError,&len);
   }
   LastWSAError = WSAGetLastError();
  }

 int        __fastcall TGrNetBase::Send(TIpxAddress& to  , LPVOID data, DWORD sz)
 {
   /*Отправка данных через гнездо*/
   int ret(0);
   ret = sendto(socket_handle,(char*)data,int(sz),0,(sockaddr*)&to,sizeof(to));
   if(!ret)
     GetSocketError();
   return ret;

 }

 int        __fastcall TGrNetBase::Recv(TIpxAddress& from, LPVOID data, DWORD sz )
 {
  int len = sizeof(from);
  DWORD AvailBytes(0);
  ioctlsocket(socket_handle,FIONREAD,&AvailBytes);
  if(AvailBytes)
  {
  int ret = recvfrom(socket_handle,(char*)data,sz,0,(sockaddr*)&from,&len);
  if(!ret)
    GetSocketError();
    return ret;
  }
  return 0;
 }

 void       __fastcall TGrNetBase::HandleSocketEvent(DWORD Event,DWORD Error)
 {
  if(Error)
   GetSocketError();
  if(Event & FD_READ && input_buffer)
  {
   TIpxAddress  from;
   int bytes;
   do{
      bytes = Recv(from,input_buffer,this->max_frame_size);
      if(bytes)
       this->HandleReceive(from,input_buffer,bytes);
     }while(bytes);
  }
 }

  void      __fastcall TGrNetBase::Messages()
  {
   MSG msg;
   while(PeekMessage(&msg,0,0,0,PM_REMOVE))
   {
    HandleMessage(msg);
   }
  }

 
  LRESULT    __fastcall TGrNetBase::HandleMessage(MSG & m)
  {
    TranslateMessage(&m);
    return DispatchMessage (&m);
  }



 bool       __fastcall TGrNetBase::PutDataToQueue(TSimpleDataQueue * _queue,
                                      DWORD ClientId,
                                      DWORD Command,
                                      DWORD KadrNumber,
                                      DWORD RetryCount,
                                      LPVOID data,
                                      DWORD sz  )
 {
  BYTE temp[2048];
  if(sz<= sizeof(temp)-sizeof(GRNET_QUEUEDDATA)+1)
  {
  LPGRNET_QUEUEDDATA qd = (LPGRNET_QUEUEDDATA)temp;
  qd->ClId       = ClientId;
  qd->Command    = Command;
  qd->KadrNumber = KadrNumber;
  qd->RetryCount = RetryCount;
  CopyMemory(qd->data,data,sz);
  if(_queue->QueueCount()>1024)
     _queue->DropFirst();//Ограничение на количество данных в очереди очереди
  return _queue->PutIntoQueue(qd,sz+sizeof(GRNET_QUEUEDDATA)-1);
  }
  return false;

 }

 DWORD      __fastcall TGrNetBase::GetDataFromQueue(TSimpleDataQueue * _queue,
                                      LPDWORD ClientId,
                                      LPDWORD Command,
                                      LPDWORD KadrNumber,
                                      LPDWORD RetryCount,
                                      LPVOID data,
                                      DWORD sz ,
                                      LPDWORD copy_bytes,
                                      LPDWORD RemindFrames)
 {
  //Возрат
  //DWORD(-3) - Ничего нет
  //DWORD(-2) - мало места во временном буфере
  //DWORD(-1) - мало места во выходном  буфере

  DWORD ret(0);
  BYTE temp[2048];
  LPGRNET_QUEUEDDATA qd = (LPGRNET_QUEUEDDATA)temp;
  _queue->Lock();
  int remind (_queue->QueueCount());
  if(_queue->PeekFromQueue(qd,sizeof(temp),(int*)&ret))
  {
   int datasz = int(ret)-sizeof(*qd)+1;
   if( datasz <= int(sz) && data)
   {
     ret = datasz;
     CopyMemory(data,qd->data,datasz);
     _queue->DropFirst();
	 remind--;
   }
   else
    ret = DWORD(-1);

    if(ClientId)
      *ClientId = qd->ClId;
    if(Command)
      *Command = qd->Command;
    if(KadrNumber)
     *KadrNumber = qd->KadrNumber;
    if(RetryCount)
      *RetryCount = qd->RetryCount;
     if(copy_bytes)
        *copy_bytes= datasz;
  }
  else
  {
   ret = remind ? DWORD(-2):DWORD(-3);
   
  }
   if(RemindFrames)
     *RemindFrames = remind;

  _queue->Unlock();

  return ret;
 }

  bool    __fastcall TGrNetBase::Read(LPDWORD From,LPDWORD Command,LPVOID data,
                                      DWORD cbSize,LPDWORD rdBytes,LPDWORD Remain )
  {

   int result = int(GetDataFromQueue(rx_queue,From,Command,0,0,data,cbSize,rdBytes,Remain));
   return result>=0 ? true:false;
  }

  void         __fastcall TGrNetBase::SetName(const char * _name,int len)
  {
   lock();
   if (_name)
   {
    if(len<1)
      len = lstrlenA(_name);
    lstrcpynA(name,_name,GRNET_NAMES_LEN);
    if(lstrlenA(name)==0)
      SetName(0);
   }
   else
   {
    char buffer[MAX_PATH];
    DWORD sz = sizeof(buffer);
    GetUserName(buffer,&sz);
    SetName(buffer,sz);
   }
   unlock();
  }

  DWORD        __fastcall TGrNetBase::GetName(char *out,DWORD sz)
  {
   DWORD ret(0);
   if(out)
   {
   lock();
   ret = lstrlenA(name);
   if(ret<sz)
    CopyMemory(out,name,ret+1);
   unlock();
   }
   return ret;
  }

  bool         __fastcall TGrNetBase::SameName(const char *_name)
  {
   bool ret(false);
   lock();
   /*Сравнение имен*/
   char n1[GRNET_NAMES_LEN+1],n2[GRNET_NAMES_LEN+1];
   lstrcpynA(n1,name,GRNET_NAMES_LEN);
   lstrcpynA(n2,_name,GRNET_NAMES_LEN);
   ret = (lstrcmpiA(n1,n2)==0) ? true:false;
   unlock();
   return ret;
  }

  HANDLE     __fastcall  TGrNetBase::GetRestartEvent()
  {
   static TEvent event(false,false);
   return event();
  }

  WORD       __fastcall  TGrNetBase::GetIpxSocket()
  {
    WORD ret;
    lock();
    ret = (this->State == tsRunning) ? htons(local_address.sa_socket) : socket_number;
    unlock();
    return ret;
  }

  DWORD        __fastcall TGrNetBase::OnException(DWORD ExceptionCode,LPEXCEPTION_POINTERS ep)
  {
   this->unlock();
   //ExitThread(DWORD(-2));
   _dbgprn(("Exception on GRNET Object"));
   return TThread::OnException(ExceptionCode,ep);
  }


/*****************************************************************************/

void * operator new (size_t sz)
{
  return LocalAlloc(LMEM_FIXED|LMEM_ZEROINIT,sz);
}

void operator delete(void* ptr)
{
   LocalFree((HLOCAL)ptr);
}

void * operator new[] (size_t sz)
{
 return LocalAlloc(LMEM_FIXED|LMEM_ZEROINIT,sz);
}

void operator delete[](void* ptr)
{
   LocalFree((HLOCAL)ptr);
}

#ifdef _DEBUG
static KeRTL::TLogFile lf;
void __stdcall grnet32_dbg_print(char * fmt,...)
{
 static bool lo = lf.OpenLog("grnet32dbg.log");
 char  text[2048];
 va_list a_list;
 va_start(a_list,fmt);
 wsprintf(text,fmt,a_list);
         va_end(a_list);

 lf.WriteToLog(text,0);
}
#endif




