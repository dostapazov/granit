//---------------------------------------------------------------------------
#ifndef _wsockdef_H
#define _wsockdef_H
//---------------------------------------------------------------------------


#ifndef _WINSOCKAPI_
#define _WINSOCKAPI_   /* Prevent inclusion of winsock.h in windows.h !!*/
#endif

#include <kertl.hpp>
#include <kethreads.hpp>
#include <winsock2.h>
#include <wsipx.h>
#include <wsnwlink.h>
#include <nspapi.h>
#include <mswsock.h>
#include <vector>

using namespace KeRTL;
using namespace std;

#pragma pack(push,4) //Be shure that align = 8;

#define WM_SOCKETEVENT (WM_USER+10000)

#ifdef NETRTL_DLL
#define NETRTL_CLASS __declspec(dllexport) class
#else
#define NETRTL_CLASS class
#endif


struct TMacAddress
{
  char   mac[8];
  int    addr_len;
  TMacAddress(){ZeroMemory(mac,sizeof(mac));addr_len = 0;}
  TMacAddress(const char * _mac,int len=6){addr_len = len,memcpy(mac,_mac,len);}
  TMacAddress(const TMacAddress & ma){*this = ma;};
  TMacAddress & operator = (const TMacAddress & ma){addr_len = ma.addr_len; memcpy(mac,ma.mac,addr_len);return *this;}
  int   operator <(const TMacAddress & ma){return addr_len == ma.addr_len ? (memcmp(mac,ma.mac,addr_len)<0 ? TRUE:FALSE) :(addr_len<ma.addr_len);};
  bool __fastcall is_valid(){return addr_len !=0; }
 };

inline int   operator <(const TMacAddress & ma1,const TMacAddress & ma2)
{return ma1.addr_len == ma2.addr_len ? (memcmp(ma1.mac,ma2.mac,ma1.addr_len)<0 ? TRUE:FALSE) :(ma1.addr_len<ma2.addr_len);};

inline  int   operator == (const TMacAddress & ma1,const TMacAddress & ma2)
{   return (ma1.addr_len == ma2.addr_len && memcmp(ma1.mac,ma2.mac,ma1.addr_len)==0);  }

inline  int   operator != (const TMacAddress & ma1,const TMacAddress & ma2)
{return !(ma1==ma2);}


NETRTL_CLASS TBaseSocket
{
 protected:
  static  int UsageCount;
  static  WSAData wsadata;
  static  bool    UnderNT;

  SOCKET Socket;

  int     fLastError;
  int     fAddressFamily;
  int     fType;         //Тип Гнезда SOCK_STREAM;SOCK_DGRAMM
  int     fProtocol;     //Протокол
  DWORD   fFlags;        //
  GROUP   fGroup;        //Группа
  DWORD   EventMask;

public:
          static bool __fastcall InitWS(WORD Vers);         //Инициализация
          static void __fastcall DeinitWS();       //Деинициализация
          static bool __fastcall IsInited(){return UsageCount ? true : false;}
protected:
  virtual WORD __fastcall VersionRequest(); //Возврат требуемой версии

 public:
 enum NetEvent { evRead   = FD_READ  , evWrite   = FD_WRITE  , evOob = FD_OOB,
                 evAccept = FD_ACCEPT, evConnect = FD_CONNECT,
                 evClose  = FD_CLOSE , evQos     = FD_QOS    , evGroupQos = FD_GROUP_QOS
				 #if WINVER > 0x0500
				 ,
                 evRoutingInterfaceChange        = FD_ROUTING_INTERFACE_CHANGE,
                 evAddressListChange             = FD_ADDRESS_LIST_CHANGE
				 #endif
				};


                  TBaseSocket ():Socket(0),fAddressFamily(0),fType(0),fProtocol(0),fFlags(0),fGroup(0),EventMask(0){}
                  TBaseSocket (int af,int type,int proto,DWORD flg=0,GROUP g =0);
                  TBaseSocket (SOCKET  s);
   virtual       ~TBaseSocket();
           bool __fastcall CreateSocket(LPWSAPROTOCOL_INFO lp_pi=NULL);   //Создание гнезда
           bool __fastcall CreateSocket(int af,int type,int proto,DWORD flags,GROUP g = 0,LPWSAPROTOCOL_INFO lp_pi=NULL);
   virtual bool __fastcall DestroySocket();  //Удаление гнезда
   SOCKET __fastcall GetSocket() const;
    void  __fastcall GetOptions();
    int   __fastcall GetLastError   ();
    int   __fastcall GetSocketError ();
    int   __fastcall GetAddressFamily();
    int   __fastcall GetSocketType  ();
    int   __fastcall GetSocketProtocol();
    DWORD __fastcall GetSocketFlags ();
    GROUP __fastcall GetSocketGroup ();
    int   __fastcall GetSocketName  (sockaddr * addr);
    int   __fastcall GetPeerName    (sockaddr * addr);
    bool  __fastcall EnableOob      (bool Val);
    bool  __fastcall EnableReuseAddress(bool Val);
    bool  __fastcall EnableBroadcast(bool Val);
    bool  __fastcall EnableDebug    (bool Val);
    bool  __fastcall EnableKeepAlive(bool Val);
    bool  __fastcall SetSendTimeout (int  val);
    bool  __fastcall SetRecvTimeout (int  val);
    bool  __fastcall SetDontRoute   (bool Val);
    bool  __fastcall SetRecvBuffSize(int  Val);
    bool  __fastcall SetSendBuffSize(int  Val);
    int   __fastcall GetRecvBuffSize();
    int   __fastcall GetSendBuffSize();
    virtual
    int   __fastcall  GetMaxMsgSize();
    bool  __fastcall  GetProtocolInfo(WSAPROTOCOL_INFO&);
    SOCKET __fastcall WSAAccept (struct sockaddr  * addr,int * addrlen,LPCONDITIONPROC lpfnCondition, DWORD dwCallbackData =0);
    int    __fastcall WSAConnect(sockaddr* name, int len, WSABUF * cdata1, WSABUF * cdata2, QOS * qs1, QOS * qs2);

    int __fastcall WSARecv(WSABUF * buf, int bufcount, DWORD * ReadBytes, DWORD * Flags, WSAOVERLAPPED * lpOvrlp=0, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionROUTINE=0);

    int __fastcall WSARecvFrom(WSABUF* lpBuffers,DWORD dwBufferCount,DWORD *lpNumberOfBytesRecvd,
    DWORD *lpFlags,struct sockaddr * lpFrom,INT *lpFromlen,WSAOVERLAPPED* lpOverlapped=0,
    LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionROUTINE=0);

    int __fastcall WSARecvDisonnect(WSABUF * bufs);

    int __fastcall WSASend(LPWSABUF lpBuffers,DWORD dwBufferCount,DWORD *lpNumberOfBytesSent,
    DWORD dwFlags,
    LPWSAOVERLAPPED lpOverlapped=0,
    LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionROUTINE=0);

    int __fastcall WSASendDisconnect(WSABUF * bufs);

    int __fastcall WSASendTo( WSABUF * lpBuffers, DWORD dwBufferCount,
    DWORD *lpNumberOfBytesSent,DWORD dwFlags,
    const struct sockaddr FAR * lpTo, int iToLen,
    LPWSAOVERLAPPED lpOverlapped=0,
    LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionROUTINE=0);

    int __fastcall WSAGetOverlappedResult(LPWSAOVERLAPPED lpOverlapped,
    DWORD *lpcbTransfer,BOOL fWait,DWORD *lpdwFlags );

    int __fastcall WSAIoctl(DWORD dwIoControlCode,void *lpvInBuffer,
    DWORD cbInBuffer,void *lpvOUTBuffer,DWORD cbOUTBuffer,DWORD* lpcbBytesReturned,
    LPWSAOVERLAPPED lpOverlapped,LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionROUTINE);

    int __fastcall WSANtohl(DWORD NetLong, DWORD * hostlong);
    int __fastcall WSAHtonl(DWORD NetLong, DWORD * hostlong);
    int __fastcall WSAGetQOSByName(WSABUF * lpQOSName,QOS *lpQOS);
    int __fastcall WSAJoinLeaf(const struct sockaddr FAR * name,int namelen,
    WSABUF * lpCallerData,WSABUF * lpCalleeData,QOS * lpSQOS,QOS * lpGQOS,DWORD dwFlags);

    SOCKET __fastcall Accept  (struct sockaddr *addr,int * addrlen);
    int    __fastcall Bind    (struct sockaddr *addr,int  addrlen);
    int    __fastcall Connect (struct sockaddr *addr,int  addrlen);
    int    __fastcall Recv    (void * buff,int len,int flg);
    int    __fastcall RecvFrom(void * buff,int len,int flg,struct sockaddr * From,int * fromlen);
    int    __fastcall Send    (void * buff,int len,int flags);
    int    __fastcall SendTo  (void * buff,int len,int flg,struct sockaddr * To,int  tolen);
    int    __fastcall Select  ();
    int    __fastcall Listen  (int Backlog);
    int    __fastcall IoctlSocket(int cmd, DWORD * arg);
    int   __fastcall  CanRead();

    bool  __fastcall IsReuseAddress ();
    bool  __fastcall IsBroadcast    ();
    bool  __fastcall IsStream       ();
    bool  __fastcall IsDebug        ();
    bool  __fastcall IsKeepAlive    ();
    bool  __fastcall IsDontRoute    ();
    bool  __fastcall Disconnect     (int how = SD_BOTH){return shutdown(Socket,how) ? false:true;}
    bool  __fastcall EnableLinger   (int linger);
    bool  __fastcall DisableLinger  ();
    bool  __fastcall IsOobEnabled   ();
 };


//Сообщение окну об асинхронном событии на гнезде
#define  WM_SOCKETMSG (WM_USER+1000)
#define  FD_ASYNC_TIMER 0x40000000

//Этот класс смело можно выносить в общую реализацию...

typedef bool ( __fastcall * SocketExternHandler )(LPVOID data,SOCKET s,LONG Event,LONG ErrorCode);

NETRTL_CLASS TSockAsyncThread:public KeRTL::TThread
{
  protected:
  HWND wnd;
  WNDPROC PrevProc;
  static LRESULT CALLBACK AsyncWndProc(HWND w,UINT msg,WPARAM,LPARAM);

  KeRTL::TWaitableTimer wt;
  WSAEVENT              NetEvent;
  KeRTL::TEvent         CloseEvent;
  SOCKET     socket;
  LONG       EventMask;
  WSANETWORKEVENTS EventsOccured;
  //KeRTL::TSemLocker      Locker;
  KeRTL::TFASTMutex      Locker;

  void __fastcall EnumEvents();

  LPVOID     UserData;
  SocketExternHandler     UserHandler;
  virtual bool __fastcall DefaultEventHandler(SOCKET,DWORD,DWORD);
          bool __fastcall SelectAsyncEvent(bool);
  public:

  TSockAsyncThread (SOCKET s);
  ~TSockAsyncThread();
  int    __fastcall Execute   ();
  void   __fastcall Terminate ();
  bool   __fastcall Lock  (DWORD Timeout = INFINITE);
  bool   __fastcall Unlock();
  bool   __fastcall SetUserHandler(SocketExternHandler,LPVOID);
  bool   __fastcall SetTimer(DWORD mSek,bool Period);
  void   __fastcall StopTimer();
  bool   __fastcall EnableEvents(DWORD Mask,bool Enable);
  SOCKET __fastcall SetSocket(SOCKET s);
  SOCKET operator()(){return socket;}
};

/******************************************************************************/
//Набор асинхронных гнезд
//
// ASTM AsyncSocketsThreadMessage
#define ASTM_TERMINATE      (WM_USER+ 99)
#define ASTM_ADD_SOCKET     (WM_USER+100) // LPARAM (TBaseSocket*),WPARAM(EventMask)
#define ASTM_ENABLE_EVENTS  (WM_USER+101) // LPARAM (TBaseSocket*),WPARAM(EventsToEnabale)
#define ASTM_DISABLE_EVENTS (WM_USER+102) // LPARAM (TBaseSocket*),WPARAM(EventsToDisable)
#define ASTM_REMOVE_SOCKET  (WM_USER+103) // LPARAM (TBaseSocket*)
#define ASTM_START_TIMER    (WM_USER+104) // Запуск таймера LPARAM(time),WPARAM(BOOL periodic)
#define ASTM_STOP_TIMER     (WM_USER+105)

struct TSocketEvents
{
 SOCKET        socket;
 DWORD         event_mask;
 WSAEVENT      event;
 TSocketEvents():socket(NULL),event_mask(0){};
 TSocketEvents(SOCKET s, DWORD em,WSAEVENT e):socket(s),event_mask(em),event(e){}
 TSocketEvents(const TSocketEvents& se){*this = se;}
 TSocketEvents & operator = (const TSocketEvents & se)
 {socket = se.socket;event_mask = se.event_mask; event = se.event;return *this;}
};

#define MAX_ASYNC_SOCKETS (MAXIMUM_WAIT_OBJECTS - 2)

class TAsyncSocketsThread:public KeRTL::TThread
{
 public:
 typedef bool (WINAPI * UserHandlerFunc)(LPVOID data,SOCKET,LONG Event,LONG Error);
 protected:

 std::vector<TSocketEvents> sockets;//Список гнезд и событий
 LPVOID                     user_data;
 UserHandlerFunc            user_func;
 KeRTL::TWaitableTimer      timer;
 KeRTL::TEvent              terminate_event;
 KeRTL::TSynchroSet         ss;
 int                        first_socket_index;

 void      __fastcall Terminate(){PostMessage(ASTM_TERMINATE,0,0);Terminate();}

 bool      __fastcall BeforeExecute();
 void      __fastcall BeforeTerminate();
 int       __fastcall Execute();

 void      __fastcall process_socket_events(DWORD index);
 void      __fastcall process_messages();
 virtual
 bool      __fastcall get_message   (MSG & msg);
 virtual
 LRESULT   __fastcall handle_message(MSG & msg);
 virtual
 bool      __fastcall handle_event  (SOCKET s,LONG Event,LONG Error);
 virtual
 void      __fastcall handle_timer(){};
 virtual
 void      __fastcall setup_synchro_set();
 virtual
 void      __fastcall handle_wait_result(DWORD code);
 virtual
 void      __fastcall handle_wait_error(DWORD error);


 void __fastcall do_add_socket    (SOCKET s,DWORD ev_mask);
 void __fastcall do_enable_events (SOCKET s,DWORD ev_mask);
 void __fastcall do_disable_events(SOCKET s,DWORD ev_mask);
 void __fastcall do_remove_socket (SOCKET s);
 void __fastcall do_start_timer   (DWORD ms,bool Period = true);
 void __fastcall do_stop_timer    ();

 public:
 TAsyncSocketsThread(const char* te_name = 0):terminate_event(true,false,te_name),user_data(NULL),user_func(NULL),timer(false)
 {
  sockets.reserve(64);
 }
 TAsyncSocketsThread(const wchar_t* te_name ):terminate_event(true,false,te_name),user_data(NULL),user_func(NULL),timer(false)
 {  sockets.reserve(64);}
 TAsyncSocketsThread(HANDLE te ):terminate_event(te),user_data(NULL),user_func(NULL),timer(false)
 {}
 ~TAsyncSocketsThread(){terminate_event.Close();timer.Close();}

 void __fastcall set_user_handler(LPVOID user_data,UserHandlerFunc uf);

 void __fastcall  add_socket    (SOCKET s,DWORD ev_mask);
 void __fastcall  enable_events (SOCKET s,DWORD ev_mask);
 void __fastcall  disable_events(SOCKET s,DWORD ev_mask);
 void __fastcall  remove_socket (SOCKET s);
 int  __fastcall  get_sockets_count(){return sockets.size();}
 void __fastcall  start_timer   (DWORD ms,bool Period = true);
 void __fastcall  stop_timer    ();

 DWORD __fastcall get_socket_events(SOCKET s);


};

/******************************************************************************/


 NETRTL_CLASS TIpxAddress:public SOCKADDR_IPX
 {
  public:
    TIpxAddress(int ipx_socket,bool Broadcast=false,bool FullBroadcast = false);
    TIpxAddress();
   SOCKADDR * operator()();
   void operator = (SOCKADDR & s);
 };

bool __fastcall GetLocalIPXAddress(SOCKADDR_IPX & adr,int AdapterNum );

NETRTL_CLASS TIpxSocket:public TBaseSocket
{
  protected:
  TIpxAddress LocalAddress;
  TIpxAddress RemoteAddress;
  public:
                  TIpxSocket(){};
  bool __fastcall CreateIpxSocket(WORD socket_number=0,DWORD PktType = 0,DWORD flags=0,LPWSAPROTOCOL_INFO lpi = NULL);
  bool __fastcall CreateSpxSocket(WORD socket_number=0,DWORD flags=0,LPWSAPROTOCOL_INFO lpi = NULL);
  int  __fastcall Send   (void * buf,int len,LPWSAOVERLAPPED ovrlp = 0);
  int  __fastcall SendBroadcast(WORD ipxSocket,void * buf,int len,LPWSAOVERLAPPED ovrlp =0);
  int  __fastcall Receive(void * buf,int len,LPWSAOVERLAPPED ovrlp = 0);
  const TIpxAddress & GetLocalAddress();
  const TIpxAddress & GetRemoteAddress();
  void  MakeRemoteAddrBroadcast(WORD sn){RemoteAddress = TIpxAddress(sn,true);} 
};

NETRTL_CLASS TIPAddress:public sockaddr_in
{
 public:
  TIPAddress(){sin_family= AF_INET;sin_addr.s_addr = 0;sin_port = 0;ZeroMemory(sin_zero,sizeof(sin_zero));};
  TIPAddress(const char * ip_addr,int port);
  TIPAddress(DWORD ip_addr,int port);
  SOCKADDR * operator()();
  TIPAddress & operator = (TIPAddress &);
  WORD __fastcall get_port();
};


int  __fastcall detect_mac_address(DWORD ip_addr,BYTE * mac_addr,int &  bsz);
int  __fastcall str2macaddr(char * str,BYTE * ma,int masz);


NETRTL_CLASS TIPSocket:public TBaseSocket
{
  public:
  TIPSocket (SOCKET s):TBaseSocket(s){};
  TIPSocket(bool Stream = true,bool ovrlp = false);
  TIPSocket(TIPAddress & bind_addr,bool Stream = true,bool ovrlp = false);
  

};


//----------------------------- inlines for TIpxAddress ------------------------

 #ifdef __BORLANDC__
 #pragma warn -sig
 #endif

 inline  TIpxAddress::TIpxAddress(int ipx_socket,bool Broadcast,bool FullBroadcast)
 {
  ::FillMemory(sa_nodenum,sizeof(sa_nodenum),(Broadcast) ? 255:0);
  ::FillMemory(sa_netnum,sizeof(sa_netnum),(FullBroadcast) ? 255:0);
  sa_family = AF_IPX;
  sa_socket = WORD(htons(ipx_socket));
 }
 inline  TIpxAddress::TIpxAddress()
 {  ::FillMemory(this,sizeof(*this),0);sa_family = AF_IPX; }

 #ifdef __BORLANDC__
 #pragma warn .sig
 #endif

 inline  SOCKADDR * TIpxAddress::operator()()
 { return (SOCKADDR*)this; }

 inline void TIpxAddress::operator = (SOCKADDR & s)
 {  ::CopyMemory(this,&s,sizeof(*this)); }

 inline int operator ==(const TIpxAddress & a1,const TIpxAddress & a2)
 {
  return (memcmp(&a1,&a2,sizeof(TIpxAddress))==0);
 }

//---------------------- inlines for TBaseSocket -------------------------------


inline int __fastcall TBaseSocket::GetLastError()
{
   int Error = ( fLastError!=0 ) ? fLastError:WSAGetLastError();
   fLastError = 0;
   return Error;
}


inline int __fastcall TBaseSocket::GetAddressFamily()
{ return fAddressFamily;}


inline int __fastcall TBaseSocket::GetSocketType()
{ return fType;}


inline int __fastcall TBaseSocket::GetSocketProtocol()
{ return fProtocol;}

inline  DWORD __fastcall TBaseSocket::GetSocketFlags()
{return fFlags;}

inline  GROUP __fastcall TBaseSocket::GetSocketGroup()
{ return fGroup; }

inline SOCKET __fastcall TBaseSocket::GetSocket() const
 {
  return Socket;
 }

inline  int   __fastcall TBaseSocket::GetSocketName  (sockaddr * addr)
{
 int len;
 return  (getsockname(Socket,addr,&len) ) ? 0: len;
}

inline  int   __fastcall TBaseSocket::GetPeerName  (sockaddr * addr)
{
 int len;
 return  (getpeername(Socket,addr,&len) ) ? 0: len;
}

inline int   __fastcall TBaseSocket::GetSocketError ()
{
 int err = 0,sz = sizeof(int);
 getsockopt(Socket,SOL_SOCKET,SO_ERROR,(char*)&err,&sz);
 return err;
}

 //inline TBaseSocket:: SOCKET operator() const
 //{return Socket;}

inline SOCKET __fastcall TBaseSocket::WSAAccept (struct sockaddr  * addr,int * addrlen,LPCONDITIONPROC lpfnCondition, DWORD dwCallbackData )
{
 return ::WSAAccept(Socket,addr,addrlen,lpfnCondition,dwCallbackData);
}



#if  __BORLANDC__ > 0x0520
#pragma warn -8084
#endif

inline WORD __fastcall TBaseSocket::VersionRequest()
{return (MAKEWORD(2,2));}

#if  __BORLANDC__ > 0x0520
#pragma warn .8084
#endif


  inline  TBaseSocket::TBaseSocket (int af,int type,int proto,DWORD flg,GROUP g )
  {
   fAddressFamily = af;fType = type; fProtocol = proto;
   fFlags = flg;fGroup = g;
   Socket = SOCKET_ERROR;
   fLastError = 0;
   
   if(InitWS(VersionRequest()))
      CreateSocket();
   EventMask = 0;
  }

inline  TBaseSocket::~TBaseSocket()
 {
    if(Socket!= (SOCKET)SOCKET_ERROR)
      DestroySocket();
      DeinitWS();
 }

inline   void __fastcall  TBaseSocket::GetOptions()
 {
  IsStream();
 }


inline  bool  __fastcall TBaseSocket::IsOobEnabled   ()
{
   BOOL tBool = FALSE;
   int  plen  = sizeof(tBool);
   int ret = getsockopt(Socket,SOL_SOCKET,SO_OOBINLINE,(char*)&tBool,&plen);
   return (tBool ? true:false);
}

inline  bool  __fastcall TBaseSocket::EnableOob      (bool Val)
{
   BOOL tBool = BOOL(Val);
   int ret = setsockopt(Socket,SOL_SOCKET,SO_OOBINLINE,(char*)&tBool,sizeof(BOOL));
   return bool(ret == 0);
}

inline bool __fastcall TBaseSocket::EnableReuseAddress(bool Val)
{
   BOOL tBool = BOOL(Val);
   int ret = setsockopt(Socket,SOL_SOCKET,SO_REUSEADDR,(char*)&tBool,sizeof(BOOL));
   return bool(ret == 0);
}


inline bool __fastcall TBaseSocket::EnableBroadcast(bool Val)
{
   BOOL tBool = BOOL(Val);
   int ret = setsockopt(Socket,SOL_SOCKET,SO_BROADCAST,(char*)&tBool,sizeof(BOOL));
   return bool(ret == 0);
}


inline bool __fastcall TBaseSocket::EnableDebug(bool Val)
{
   BOOL tBool = BOOL(Val);
   int ret = setsockopt(Socket,SOL_SOCKET,SO_DEBUG,(char*)&tBool,sizeof(BOOL));
   return bool(ret == 0);
}


inline bool __fastcall TBaseSocket::SetDontRoute(bool Val)
{
   BOOL tBool = BOOL(Val);
   int ret = setsockopt(Socket,SOL_SOCKET,SO_DONTROUTE,(char*)&tBool,sizeof(BOOL));
   return bool(ret == 0);
}


inline bool __fastcall TBaseSocket::EnableKeepAlive(bool Val)
{
   BOOL tBool = BOOL(Val);
   int ret = setsockopt(Socket,SOL_SOCKET,SO_KEEPALIVE,(char*)&tBool,sizeof(BOOL));
   return bool(ret == 0);
}



inline bool __fastcall TBaseSocket::SetRecvBuffSize(int Val)
{
   int ret = setsockopt(Socket,SOL_SOCKET,SO_RCVBUF,(char*)&Val,sizeof(int));
   return bool(ret == 0);

}

inline bool  __fastcall TBaseSocket::SetSendTimeout (int  val)
{
   int ret = setsockopt(Socket,SOL_SOCKET,SO_SNDTIMEO,(char*)&val,sizeof(int));
   return bool(ret == 0);
}

inline bool  __fastcall TBaseSocket::SetRecvTimeout (int  val)
{
   int ret = setsockopt(Socket,SOL_SOCKET,SO_RCVTIMEO,(char*)&val,sizeof(int));
   return bool(ret == 0);
}



inline bool __fastcall TBaseSocket::SetSendBuffSize(int Val)
{
   int ret = setsockopt(Socket,SOL_SOCKET,SO_SNDBUF,(char*)&Val,sizeof(int));
   return bool(ret == 0);
}

inline   bool  __fastcall TBaseSocket::IsReuseAddress ()
   {
     BOOL tBool;
     int optsz = sizeof(BOOL);
     int ret = getsockopt(Socket,SOL_SOCKET,SO_REUSEADDR,(char*)&tBool,&optsz);
     return (ret == 0 && (tBool))? true:false;
    }

inline    bool  __fastcall TBaseSocket::IsBroadcast    ()
    {
     BOOL tBool;
     int optsz = sizeof(BOOL);
     int ret = getsockopt(Socket,SOL_SOCKET,SO_BROADCAST,(char*)&tBool,&optsz);
     return  (ret == 0 && (tBool)) ? true:false;
    }

inline    bool  __fastcall TBaseSocket::IsDebug        ()
    {
     BOOL tBool;
     int optsz = sizeof(BOOL);
     int ret = getsockopt(Socket,SOL_SOCKET,SO_DEBUG,(char*)&tBool,&optsz);
     return (ret == 0 && (tBool)) ? true:false;
    }

inline    bool  __fastcall TBaseSocket::IsKeepAlive    ()
    {
      BOOL tBool;
      int optsz = sizeof(BOOL);
      int ret = getsockopt(Socket,SOL_SOCKET,SO_KEEPALIVE,(char*)&tBool,&optsz);
      return (ret == 0 && (tBool)) ? true:false;
    }

inline    bool  __fastcall TBaseSocket::IsDontRoute    ()
    {
     BOOL tBool;
     int optsz = sizeof(BOOL);
     int ret = getsockopt(Socket,SOL_SOCKET,SO_DONTROUTE,(char*)&tBool,&optsz);
     return (ret == 0 && (tBool)) ? true:false;
    }

inline    bool  __fastcall TBaseSocket::IsStream       ()
    {

     int optsz = sizeof(int);
     int ret = getsockopt(Socket,SOL_SOCKET,SO_DONTROUTE,(char*)&fType,&optsz);
     return bool(ret == 0 && (fType == SOCK_STREAM));

    }

inline   int   __fastcall TBaseSocket::GetRecvBuffSize()
    {
      int bsz = -1;
      int optsz = sizeof(int);
      getsockopt(Socket,SOL_SOCKET,SO_RCVBUF,(char*)&bsz,&optsz);
      return bsz;
    }

inline   int   __fastcall TBaseSocket::GetSendBuffSize()
    {
      int bsz = -1;
      int optsz = sizeof(int);
      getsockopt(Socket,SOL_SOCKET,SO_SNDBUF,(char*)&bsz,&optsz);
      return bsz;
    }

inline    bool  __fastcall TBaseSocket::EnableLinger   (int to)
    {
     linger l;
     l.l_onoff = 1;
     l.l_linger = to;
     return setsockopt(Socket,SOL_SOCKET,SO_LINGER,(char*)&l,sizeof(l)) ? false:true;
    }

inline  bool  __fastcall TBaseSocket::DisableLinger  ()
    {
     linger l;
     l.l_onoff = 0;
     l.l_linger = 0;
     return setsockopt(Socket,SOL_SOCKET,SO_LINGER,(char*)&l,sizeof(l)) ? false:true;
    }

inline bool __fastcall TBaseSocket::CreateSocket(LPWSAPROTOCOL_INFO lp_pi)
 {
  return CreateSocket(fAddressFamily,fType,fProtocol,fFlags,fGroup,lp_pi);
 }

inline bool __fastcall TBaseSocket::CreateSocket(int af,int type,int proto,DWORD flags,GROUP g ,LPWSAPROTOCOL_INFO lp_pi)
 {
  fAddressFamily = af;  fType = type;  fProtocol = proto;  fFlags = flags;  fGroup = g;
  Socket = WSASocket(fAddressFamily,fType,fProtocol,lp_pi,fGroup,fFlags);
  fLastError = WSAGetLastError();
  GetOptions();
  return bool(Socket!=(SOCKET)SOCKET_ERROR);
 }



inline int __fastcall TBaseSocket::WSAConnect(sockaddr* name, int len, WSABUF * cdata1, WSABUF * cdata2, QOS * qs1, QOS * qs2)
{
    //TODO: Add your source code here
    int Ret = ::WSAConnect(Socket,name,len,cdata1,cdata2,qs1,qs2);
    if(Ret)
      fLastError = WSAGetLastError();
    return (Ret==0);
}


inline int __fastcall TBaseSocket::WSARecv(WSABUF * buf, int bufcount, DWORD * ReadBytes, DWORD * Flags, WSAOVERLAPPED * lpOvrlp, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionROUTINE)
{
    //TODO: Add your source code here
    int Ret = ::WSARecv(Socket,buf,bufcount,ReadBytes,Flags,lpOvrlp,lpCompletionROUTINE);
    if(Ret)
      fLastError = WSAGetLastError();
    return  (Ret==0);
}


inline int __fastcall TBaseSocket::WSARecvFrom
(   WSABUF* lpBuffers,DWORD dwBufferCount,DWORD *lpNumberOfBytesRecvd,
    DWORD *lpFlags,struct sockaddr * lpFrom,INT *lpFromlen,WSAOVERLAPPED* lpOverlapped,
    LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionROUTINE
)
{
    //TODO: Add your source code here
    int Ret = ::WSARecvFrom(Socket,lpBuffers,dwBufferCount,lpNumberOfBytesRecvd,
                                   lpFlags,lpFrom,lpFromlen,
                                   lpOverlapped,lpCompletionROUTINE);
    if(Ret)
      fLastError = WSAGetLastError();
    return  (Ret==0);
}


inline int __fastcall TBaseSocket::WSARecvDisonnect(WSABUF * bufs)
{
    //TODO: Add your source code here
    int Ret = ::WSARecvDisconnect(Socket,bufs);
    if(Ret)
      fLastError = WSAGetLastError();
    return  (Ret==0);
}


inline int __fastcall TBaseSocket::WSASend(LPWSABUF lpBuffers,DWORD dwBufferCount,DWORD *lpNumberOfBytesSent,
    DWORD dwFlags,LPWSAOVERLAPPED lpOverlapped,
    LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionROUTINE)
{
    //TODO: Add your source code here
    int Ret = ::WSASend(Socket,lpBuffers,dwBufferCount,lpNumberOfBytesSent,
    dwFlags,lpOverlapped,lpCompletionROUTINE);
    if(Ret)
      fLastError = WSAGetLastError();
    return  (Ret==0);
}

inline int __fastcall TBaseSocket::WSASendDisconnect(WSABUF * bufs)
{
    int Ret = ::WSASendDisconnect(Socket,bufs);
    if(Ret)
      fLastError = WSAGetLastError();
    return  (Ret==0);
}

inline int __fastcall TBaseSocket::WSASendTo( WSABUF * lpBuffers, DWORD dwBufferCount,
    DWORD *lpNumberOfBytesSent,DWORD dwFlags,
    const struct sockaddr FAR * lpTo,
    int iToLen,
    LPWSAOVERLAPPED lpOverlapped,
    LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionROUTINE)
{
    //TODO: Add your source code here
    int Ret = ::WSASendTo(Socket,lpBuffers,dwBufferCount,lpNumberOfBytesSent,dwFlags,
    lpTo,iToLen,lpOverlapped,lpCompletionROUTINE);
    if(Ret)
      fLastError = WSAGetLastError();
    return  (Ret==0);
}


inline int __fastcall TBaseSocket::WSAGetOverlappedResult( LPWSAOVERLAPPED lpOverlapped,
    DWORD *lpcbTransfer,BOOL fWait,DWORD *lpdwFlags )
{
    //TODO: Add your source code here
    int Ret = ::WSAGetOverlappedResult(Socket,lpOverlapped,lpcbTransfer,fWait,lpdwFlags );
    if(Ret)
      fLastError = WSAGetLastError();
    return  (Ret==0);

}


inline int __fastcall TBaseSocket::WSAIoctl(DWORD dwIoControlCode,void *lpvInBuffer,
    DWORD cbInBuffer,void *lpvOUTBuffer,
    DWORD cbOUTBuffer,DWORD* lpcbBytesReturned,
    LPWSAOVERLAPPED lpOverlapped,LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionROUTINE)
{
    //TODO: Add your source code here
    int Ret = ::WSAIoctl(Socket,dwIoControlCode,lpvInBuffer,cbInBuffer,lpvOUTBuffer,
    cbOUTBuffer,lpcbBytesReturned,lpOverlapped,lpCompletionROUTINE);
    if(Ret)
      fLastError = WSAGetLastError();
    return  (Ret==0);
}


inline int __fastcall TBaseSocket::WSANtohl(DWORD NetLong, DWORD * hostlong)
{
    int Ret = ::WSANtohl(Socket,NetLong,hostlong);
    if(Ret)
      fLastError = WSAGetLastError();
    return  (Ret==0);
    //TODO: Add your source code here
}


inline int __fastcall TBaseSocket::WSAHtonl(DWORD NetLong, DWORD * hostlong)
{
    //TODO: Add your source code here

    int Ret = ::WSAHtonl(Socket,NetLong,hostlong);
    if(Ret)
      fLastError = WSAGetLastError();
    return  (Ret==0);

}



inline int __fastcall TBaseSocket::WSAGetQOSByName( WSABUF * lpQOSName,QOS *lpQOS)
{
    //TODO: Add your source code here
    int Ret = ::WSAGetQOSByName(Socket,lpQOSName,lpQOS);
    if(Ret)
      fLastError = WSAGetLastError();
    return  (Ret==0);

}


inline int __fastcall TBaseSocket::WSAJoinLeaf(const struct sockaddr FAR * name,
    int namelen,
    WSABUF * lpCallerData,
    WSABUF * lpCalleeData,
    QOS * lpSQOS,
    QOS * lpGQOS,
    DWORD dwFlags)
{
    //TODO: Add your source code here
    int Ret = ::WSAJoinLeaf(Socket,name,namelen,lpCallerData,lpCalleeData,
                            lpSQOS,lpGQOS,dwFlags);
    if(Ret)
      fLastError = WSAGetLastError();
    return  (Ret==0);
}


inline SOCKET __fastcall TBaseSocket::Accept( struct sockaddr *addr,int * addrlen )
{
    //TODO: Add your source code here
    SOCKET s = ::accept(Socket,addr,addrlen);
    if(s == INVALID_SOCKET)
      fLastError = WSAGetLastError();
    return  s;
}


inline int __fastcall TBaseSocket::Bind(struct sockaddr *addr,int  addrlen)
{
    //TODO: Add your source code here
    int Ret = ::bind(Socket,addr,addrlen);
    if(Ret)
      fLastError = WSAGetLastError();
    return  (Ret==0);

}


inline int __fastcall TBaseSocket::Connect(struct sockaddr *addr,int  addrlen)
{
    //TODO: Add your source code here
    int Ret = ::connect(Socket,addr,addrlen);
    if(Ret)
      fLastError = WSAGetLastError();  
    return  (Ret==0);
}


inline int __fastcall TBaseSocket::Recv(void * buff,int len,int flg)
{
    //TODO: Add your source code here
    int Ret = ::recv(Socket,(char*)buff,len,flg);
    if(Ret<1)
       fLastError = WSAGetLastError();
    return  (Ret>0) ? Ret:0;

}


inline int __fastcall TBaseSocket::RecvFrom(void * buff,int len,int flg,struct sockaddr * From,int * fromlen)
{
    int Ret = ::recvfrom(Socket,(char*)buff,len,flg,From,fromlen);
    if(Ret<1)
       fLastError = WSAGetLastError();
    return  (Ret>0)? Ret:0;
}


inline int __fastcall TBaseSocket::Send(void * buff,int len,int flg)
{
    //TODO: Add your source code here

    int Ret = (Socket) ? ::send(Socket,(char*)buff,len,flg) : 0;
    if( Ret ==SOCKET_ERROR)
       fLastError = WSAGetLastError();
    return  (Ret==SOCKET_ERROR)? 0 :Ret;
}


inline int __fastcall TBaseSocket::SendTo(void * buff,int len,int flg,struct sockaddr * to,int  tolen)
{
    //TODO: Add your source code here
    int Ret = (Socket) ? ::sendto(Socket,(char*)buff,len,flg,to,tolen) : 0;
    if( Ret ==SOCKET_ERROR)
	{
		fLastError = WSAGetLastError();
		int sz = sizeof(fLastError);
		::getsockopt(Socket,SOL_SOCKET,SO_ERROR,(char*)&fLastError,&sz);

	}
    return  (Ret==SOCKET_ERROR)? 0 :Ret;
}




inline int    __fastcall TBaseSocket::Listen(int Backlog)
{

    int Ret = ::listen(Socket,Backlog);
    if(Ret)
	{
		fLastError = WSAGetLastError();
		int sz = sizeof(fLastError);
		::getsockopt(Socket,SOL_SOCKET,SO_ERROR,(char*)&fLastError,&sz);
	}
    return  (Ret==0);
}



inline int __fastcall TBaseSocket::IoctlSocket(int cmd, DWORD * arg)
{
    int Ret = ::ioctlsocket(Socket,long(cmd),arg);
    if(Ret)
      fLastError = WSAGetLastError();
    return  (Ret==0);
    //TODO: Add your source code here
}


inline int __fastcall TBaseSocket::Select()
{
    //TODO: Add your source code here
    return 0;
}


inline int __fastcall TBaseSocket::CanRead()
{

   unsigned long rec_len = 0;
   int Ret = ioctlsocket(Socket,FIONREAD,&rec_len);
       if(Ret)
         {rec_len = 0;fLastError = WSAGetLastError();}
   return int (rec_len);
}



inline  const TIpxAddress & TIpxSocket::GetLocalAddress()
  {return *&LocalAddress;}

inline  const TIpxAddress & TIpxSocket::GetRemoteAddress()
{ return *&RemoteAddress; }


inline  void __fastcall TSockAsyncThread::Terminate()
 {
   TerminateRequest = true;
   CloseEvent.SetEvent(true);

 }

inline  bool __fastcall TSockAsyncThread::Lock  (DWORD Timeout )
  {
   return Locker.Lock(Timeout);
  }

inline  bool __fastcall TSockAsyncThread::Unlock()
{
   return Locker.Unlock();
}

inline  TIPAddress::TIPAddress(const char * ip_addr,int port)
{
 sin_family = AF_INET;
 DWORD addr = inet_addr(ip_addr);
 if(addr == INADDR_NONE)
 {
  hostent * he = gethostbyname(ip_addr);
  if(he)
  {
    addr = *(LPDWORD(*he->h_addr_list));
  }
  else
  addr = 0;
 }
 sin_addr.S_un.S_addr = addr;
 sin_port = htons(port);
 ZeroMemory(sin_zero,sizeof(sin_zero));
}

inline  TIPAddress::TIPAddress(DWORD ip_addr,int port)
{
 sin_family = AF_INET;
 sin_addr.S_un.S_addr = ip_addr;
 sin_port = htons(port);
 ZeroMemory(sin_zero,sizeof(sin_zero));
}

inline WORD __fastcall TIPAddress::get_port()
{
 return htons(sin_port);
}

inline  SOCKADDR *  TIPAddress::operator()()
{ return (SOCKADDR *)this;}

inline  TIPAddress& TIPAddress::operator = (TIPAddress & a)
{
 sin_family           = a.sin_family;
 sin_addr.S_un.S_addr = a.sin_addr.S_un.S_addr;
 sin_port             = a.sin_port;
 CopyMemory(sin_zero,a.sin_zero,sizeof(sin_zero));
 return *this;
}

inline int operator ==( TIPAddress & i1,TIPAddress & i2)
{return i1.sin_addr.S_un.S_addr == i2.sin_addr.S_un.S_addr;}

inline int operator !=( TIPAddress & i1,TIPAddress & i2)
{return !(i1 == i2);}

inline int operator <( TIPAddress & i1,TIPAddress & i2)
{return i1.sin_addr.S_un.S_addr < i2.sin_addr.S_un.S_addr;}


inline  TIPSocket::TIPSocket(bool Stream,bool ovrlp)
  :TBaseSocket(AF_INET,Stream ? SOCK_STREAM:SOCK_DGRAM,Stream ? IPPROTO_TCP:IPPROTO_UDP,ovrlp ? WSA_FLAG_OVERLAPPED : 0 )
  {}
inline TIPSocket::TIPSocket(TIPAddress & bind_addr,bool Stream,bool ovrlp):
TBaseSocket(AF_INET,Stream ? SOCK_STREAM:SOCK_DGRAM,Stream ?IPPROTO_TCP:IPPROTO_UDP,ovrlp ? WSA_FLAG_OVERLAPPED : 0)
{
  Bind((sockaddr*)&bind_addr,sizeof(TIPAddress));
}

#pragma pack(pop)
#endif



