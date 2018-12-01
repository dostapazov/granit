#ifndef   _GRNET3_32_INC_
#define   _GRNET3_32_INC_
#ifndef STRICT
#define STRICT
#endif

#include <winsock2.h>
#include <grnet32.h>
#include <kertl.hpp>
#include <kedatas.hpp>
#include <wsipx.h>
#include <wsnwlink.h>

#ifdef _DEBUG
#include <tlogfile.hpp>
 void __stdcall grnet32_dbg_print(char * fmt,...);
 #define _dbgprn( _x_ ) grnet32_dbg_print _x_

#else
 #define _dbgprn(x)
#endif

/*
#include <nspapi.h>
#include <mswsock.h>
*/


#define PROTO_VER_1   1

#define GRNET_FLAGS_ENABLE_WATCHDOG  0x0000100
#define GRNET_FLAGS_ENABLE_RECONNECT 0x0000200

using namespace KeRTL;


const long NetSecret = 0x5850494EuL;    //NIPX

const WORD NoReceipt   = 0,
           WaitReceipt = 1
           ;
const DWORD stNotConnect      = 0x80000000,
            stFree            = 0x00000000,
            stWaitReceipt     = 0x00000100,
            stWaitConnect     = 0x00000200,
            stWaitDisconnect  = 0x00000400
           ;


const WORD CLIENT_SOCKET      = 0x6544;
const WORD SERVER_SOCKET      = 0x6543;
const WORD NO_HANDLE          = 0,  //Не обработано
            HANDLE_OK          = 1,
            REC_QUEUE_FULL     = 2,  //Приемный буфер заполнен
            REC_QUEUE_OVERFLOW = 3,  //Приемный буфер перполнен
            REC_CRC_ERROR      = 4   //Ошибка контрольной суммы
           ;

#pragma pack (push,1)


struct TNetMessage
{
 DWORD secretWord;
 WORD  Receipt;
 WORD  NeedReceipt;
 DWORD command;
 WORD  ID;
 WORD  KadrNum;
 WORD  RetryNum;
 TNetMessage(DWORD cmd,WORD id = WORD(-1),WORD NR=0,WORD kdr = 0,WORD retry=0);
 TNetMessage & operator = (TNetMessage & msg);
 WORD IsValid()
 { return (secretWord == NetSecret);}
 WORD IsReceipt();
 void ReceiptMessage()
 { Receipt=1;}
};


struct TGRNetBuffer
{
 TNetMessage msg;
 WORD Len;
 long CheckSum;
 BYTE DATA[IPX_DATA_LEN+sizeof(TNetMessage)+sizeof(WORD)+sizeof(long)];
 TGRNetBuffer();
 TGRNetBuffer & operator = (TNetMessage& m);
 TGRNetBuffer & operator = (TGRNetBuffer & b);
 TGRNetBuffer & SetData(void * data,WORD len);
 WORD GetTotalLen();
 WORD GetDataLen();
 WORD GetData(void * data,WORD Len);
 void CalcCheckSum();
 WORD IsValid();
 void Receipt();
 TNetMessage & GetMessage();

};


struct TIpxAddress:public sockaddr_ipx
{
 TIpxAddress(WORD sck=0,short family = AF_IPX);
 TIpxAddress(const TIpxAddress & addr);
 TIpxAddress & operator = (const TIpxAddress & addr);
 void __fastcall SetBroadcast(short sck,bool Full = false);
};

inline bool          operator == (const TIpxAddress & addr1,const TIpxAddress & addr2)
 {
  return (memcmp(addr1.sa_netnum,addr2.sa_netnum,sizeof(addr1.sa_netnum)) == 0 &&
          memcmp(addr1.sa_nodenum,addr2.sa_nodenum,sizeof(addr1.sa_nodenum)) == 0
          && addr1.sa_socket == addr2.sa_socket
         ) ? true:false;
 }


struct TBeginConnection
{
 char ServerName[GRNET_NAMES_LEN]; // ..
 char ClientName[GRNET_NAMES_LEN]; // ..
 WORD ClientLevel;    // .. устанавливается клиентом
 /*************************/
 WORD ID;              // ..
 WORD ErrorConnection; // ..Устанавливается сервером

};

typedef struct  _GRNET_QUEUEDDATA
{
 DWORD ClId;
 DWORD Command;
 DWORD KadrNumber;//Номер кадра
 DWORD RetryCount;//Номер попытки
 BYTE  data[1];
}GRNET_QUEUEDDATA,*LPGRNET_QUEUEDDATA ;

#pragma pack (pop)

#define DEFAULT_WAIT_RECEIPT_TIME  300 //мсек
#define DEFAULT_RETRY_COUNT   10        //раз
#define DEFAULT_WDT_TIME      5        //сек

#define SOCKET_MSG (WM_USER+1000)

/*Базовый класс */
class TGrNetBase:public KeRTL::TThread
{
  protected:
  DWORD                   SecretDword; //Секретное слово
  KeRTL::TFASTMutex       locker;      //Блокиратор
  KeRTL::TEvent           close_event; //Событие о закрытии
  KeRTL::TWaitableTimer   secund_timer;//Секундный таймер
  KeRTL::TSynchroSet      synchro_set;

  WORD                    socket_number; //Номер гнезда
  SOCKET                  socket_handle;//Гнездо
  WSAEVENT                socket_event; //Событие на гнезде

  TIpxAddress             local_address;

  DWORD                   LastWSAError; //Последние ошибки
  DWORD                   LastSocketError;

  HWND                    socket_window;
  WNDPROC                 socket_window_subclass_proc;

  static
  LRESULT CALLBACK        socket_window_proc(HWND, UINT Msg,WPARAM,LPARAM);// Процедура окна

  DWORD                   WaitRcptTime;//Время ожидания квитанции  ms
  DWORD                   RetryCount;  //Количество повторов до обрыва связи
  DWORD                   WatchDogTime;//Время проверки связи

  KeRTL::TEventSimpleDataQueue * rx_queue;//Приемная очередь
  bool                    alloc_queue;

  int                     max_frame_size;//Максимальный размер данных для записи в гнездо
  BYTE*                   input_buffer;  //буфер на прием
  BYTE*                   output_buffer; //буфер на передачу
  DWORD                   WorkTime;//Время работы


  char                    name[GRNET_NAMES_LEN+1]; //Имя клиента
  

           int        __fastcall Execute();
           bool       __fastcall BeforeExecute();
           void       __fastcall BeforeTerminate();

  virtual  void       __fastcall OnSecundTimer();
  virtual  bool       __fastcall OnWaitEvent  (DWORD WaikupNumber);
  virtual  bool       __fastcall CreateSocket ();
  virtual  bool       __fastcall DestroySocket();
  virtual  void       __fastcall HandleReceive(TIpxAddress & from,LPVOID data,int bytes){/*Abstract*/};
           void       __fastcall GetSocketError();
           int        __fastcall Send(TIpxAddress& to  , LPVOID data, DWORD sz);
           int        __fastcall Recv(TIpxAddress& from, LPVOID data, DWORD sz );

  virtual  void       __fastcall HandleSocketEvent(DWORD Event,DWORD Error);
  virtual  LRESULT    __fastcall HandleMessage(MSG & m);
  static   bool       __fastcall PutDataToQueue(TSimpleDataQueue * _queue,
                                                DWORD ClientId,
                                                DWORD Command,
                                                DWORD KadrNumber,
                                                DWORD RetryCount,
                                                LPVOID data,
                                                DWORD sz  );

  static   DWORD      __fastcall GetDataFromQueue(TSimpleDataQueue * _queue,
                                                  LPDWORD ClientId,
                                                  LPDWORD Command,
                                                  LPDWORD KadrNumber,
                                                  LPDWORD RetryCount,
                                                  LPVOID data,
                                                  DWORD sz ,
                                                  LPDWORD copy_bytes,
                                                  LPDWORD RemindFrames);

  void         __fastcall Messages();                                              

  public:
  TGrNetBase(WORD SockNumber,const char * name=0,TEventSimpleDataQueue* _queue=0);

  void         __fastcall SetName(const char * _name,int len=-1);
  DWORD        __fastcall GetName(char *out,DWORD sz);
  bool         __fastcall SameName(const char *name);


  void         __fastcall SetSocketNumber(WORD sn);
  WORD         __fastcall GetSocketNumber();
  DWORD        __fastcall GetWsaError();
  void         __fastcall lock(){locker.Lock();}
  void         __fastcall unlock(){locker.Unlock();}
  void         __fastcall Terminate(){close_event.SetEvent(true);TThread::Terminate();}

  virtual  DWORD       __fastcall GetTerminateTime();
  HANDLE      __fastcall GetRxEvent(){return rx_queue ? rx_queue->GetEvent()():INVALID_HANDLE_VALUE; };

  void        __fastcall Release();
  bool        __fastcall Read(LPDWORD From,LPDWORD Command,LPVOID data, DWORD cbSize,LPDWORD rdBytes,LPDWORD Remain );
  virtual
  bool        __fastcall SendTo(DWORD ClientId,DWORD Command, LPVOID data, DWORD length, bool NeedReceipt,bool First)=0;
  virtual
  DWORD      __fastcall  GetWaitReceiptTimeout(DWORD);
  virtual
  bool       __fastcall  SetWaitReceiptTimeout(DWORD,DWORD);
  virtual
  DWORD      __fastcall  GetRetryCount(DWORD);
  virtual
  bool       __fastcall  SetRetryCount(DWORD,DWORD);
  HANDLE     __fastcall  GetRestartEvent();
  virtual
  WORD       __fastcall  GetIpxSocket();
  virtual
  void        __fastcall EnumClients(GRNET_EnumClientsProc proc,LPVOID UserData){/*Abstract*/};


  static
  TGrNetBase * __fastcall CAST(GRNET_OBJ obj);

  protected:
  static bool             UnderNT;
  static WSADATA          wsadata;
  static int              WinSockUsageCount;
  static bool  __fastcall InitWS();
  static void  __fastcall DeinitWS();
  DWORD        __fastcall OnException(DWORD ExceptionCode,LPEXCEPTION_POINTERS);

};

class TGrNetClient:public TGrNetBase
{
 protected:
 friend        class TGrNetServer;

 KeRTL::TWaitableTimer receipt_timer;//Таймер ожидания квитанции
 WORD          server_socket_number;          //номер гнезда сервера
 char          server_name[GRNET_NAMES_LEN+1];//имя   сервера
 TIpxAddress   host_addr;// IPX адрес партнера
 DWORD  ClientState;     // флаги определяющие состояние клиента
 WORD   ClientId;        //Идентификатор клиента присвоенный сервером
 TSimpleDataQueue        tx_queue;//Очередь на передачу
 TEvent                  tx_queue_event;//Событие на передачу

 TGrNetServer *          owner;//Сервер владелец
 TGRNetBuffer *          out_net_buffer;// буфер посылки


 DWORD                   LastRxKadrNumber;
 DWORD                   LastTxKadrNumber;
 DWORD                   WatchDogTimer;

 void         __fastcall Terminate      ();
 bool         __fastcall BeforeExecute  ();
 void         __fastcall BeforeTerminate();

 void         __fastcall OnSecundTimer();
 void         __fastcall OnReceiptTimer ();//Обработчтк таймера ожидания квитанции
 bool         __fastcall OnWaitEvent(DWORD WaikupNumber);

 void         __fastcall HandleReceive(TIpxAddress & from,LPVOID data,int bytes);
 virtual
 void         __fastcall HandleRx     (TGRNetBuffer & buff);
 virtual
 void         __fastcall HandleReceipt(TGRNetBuffer & buff);
 virtual
 void         __fastcall ReceiptMessage (TGRNetBuffer & buff);
 virtual
 void         __fastcall HandleRxCommand(TGRNetBuffer & buff);
 virtual
 void         __fastcall ContinueSend();
 virtual
 void         __fastcall SetClientState(DWORD StateMask, bool set);
 bool         __fastcall send_data(DWORD Command,void * data,int sz,bool nr = true,bool real=true);
 void         __fastcall BeginConnection();
 void         __fastcall MakeClientInfo(LPCLIENT_INFO info=0);
 virtual
 void         __fastcall AcceptConnection(TGRNetBuffer & buff);

 TGrNetClient(TGrNetServer * _owner,WORD id,TEventSimpleDataQueue * _queue);
 public:
 TGrNetClient(const char * ServerName,DWORD ServerSocket,const char * name,WORD socket);
 void        __fastcall SetServerName(const char * s_name,int len=-1);
 DWORD       __fastcall GetServerName(char *out,DWORD sz);
 bool        __fastcall IsConnected  (){return ((ClientState&stNotConnect) == 0) ? true:false;}
 bool        __fastcall IsWaitReceipt(){return ((ClientState&stWaitReceipt)) ? true:false;}
 bool        __fastcall IsWaitConnect(){return ((ClientState&stWaitConnect)) ? true:false;}
 bool        __fastcall IsFree       (){return ((ClientState==stFree      )) ? true:false;}
 bool        __fastcall SendTo(DWORD ClientId,DWORD Command, LPVOID data, DWORD length, bool NeedReceipt,bool First);
 WORD        __fastcall GetServerIpxSocket(){return server_socket_number;}
 void        __fastcall EnumClients(GRNET_EnumClientsProc proc,LPVOID UserData);

};

#define DEFAULT_CLIENTS_COUNT 8

class TGrNetServer:public TGrNetBase
{
 protected:
  DWORD                   MaxClients; //Максимальное количество клиентов
  TGrNetClient**          clients_pool;//Пул клиентов
  bool   __fastcall       BeforeExecute ();
  void   __fastcall       BeforeTerminate();
  void   __fastcall       HandleReceive(TIpxAddress & from,LPVOID data,int bytes);
  void   __fastcall       Terminate();
  DWORD  __fastcall       GetTerminateTime();
  virtual
  bool   __fastcall       CreateClients();
  virtual

  void   __fastcall       RegisterClient(TIpxAddress &,TBeginConnection & bc,TGRNetBuffer & buff);
  public:
  TGrNetServer(const char * name,WORD socket);
  void        __fastcall DeleteClients();
  bool        __fastcall SendTo(DWORD ClientId,DWORD Command, LPVOID data, DWORD length, bool NeedReceipt,bool First);
  void        __fastcall EnumClients(GRNET_EnumClientsProc proc,LPVOID UserData);
  DWORD       __fastcall GetWaitReceiptTimeout(DWORD);
  bool        __fastcall SetWaitReceiptTimeout(DWORD,DWORD);
  DWORD       __fastcall GetRetryCount(DWORD);
  bool        __fastcall SetRetryCount(DWORD,DWORD);
  void        __fastcall DisconnectClients();

};


/*inline implementation*/
inline TNetMessage::TNetMessage(DWORD cmd,WORD id ,WORD nr,WORD kdr ,WORD retry):
secretWord(NetSecret),
Receipt    (    0   ),
NeedReceipt(   nr   ),
command    (   cmd  ),
ID         (   id   ),
KadrNum    (   kdr  ),
RetryNum   (  retry )
{}

inline TNetMessage & TNetMessage::operator = (TNetMessage & msg)
{
 memcpy(this,&msg,sizeof(TNetMessage));
 return *this;
}


inline WORD TNetMessage::IsReceipt()
{ return (Receipt!=0);}

inline void TGRNetBuffer::Receipt()
{
 msg.ReceiptMessage();
}

inline TGRNetBuffer::TGRNetBuffer():msg(0,0,0)
{
 Len = 0;
 CheckSum = 0;
 memset(DATA,0,sizeof(DATA));
}

inline TGRNetBuffer & TGRNetBuffer::operator = (TNetMessage& m)
{ msg = m;return *this;}

inline TGRNetBuffer & TGRNetBuffer::operator = (TGRNetBuffer & b)
{memcpy(this,&b,b.GetTotalLen());return *this;}

inline TGRNetBuffer & TGRNetBuffer::SetData(void * data,WORD len)
{
  Len = ((len > WORD(sizeof(DATA))) ? WORD(sizeof(DATA)) : len);
  memcpy(DATA,((data!=0) ? data:DATA),Len);
  return *this;
}

inline WORD TGRNetBuffer::GetTotalLen()
{return WORD(Len+sizeof(TNetMessage)+sizeof(Len)+sizeof(CheckSum));}

inline WORD TGRNetBuffer::GetDataLen()
{return Len;};

inline TNetMessage & TGRNetBuffer::GetMessage()
{ return msg;}

inline WORD TGRNetBuffer::GetData(void * data,WORD len)
{
 memcpy(data,DATA,(len <= Len) ? len : Len);
 return Len;
}




inline TIpxAddress::TIpxAddress(WORD sck,short family)
{
 ZeroMemory(sa_netnum,sizeof(sa_netnum));
 ZeroMemory(sa_nodenum,sizeof(sa_nodenum));;
 sa_family = family;sa_socket = sck;
}

inline TIpxAddress::TIpxAddress(const TIpxAddress & addr)
{
 *this = addr;
}

inline TIpxAddress & TIpxAddress::operator = (const TIpxAddress & addr)
{
 sa_socket = addr.sa_socket;
 memcpy(sa_netnum ,addr.sa_netnum,sizeof(sa_netnum));
 memcpy(sa_nodenum,addr.sa_nodenum,sizeof(sa_nodenum));
 return *this;
}

inline void __fastcall TIpxAddress::SetBroadcast(short sck , bool Full)
{
 sa_socket = htons(sck);
 FillMemory(sa_nodenum,sizeof(sa_nodenum),BYTE(-1));
 FillMemory(sa_netnum,sizeof(sa_netnum),Full ? BYTE(-1):BYTE(0));
}

  inline void         __fastcall TGrNetBase::SetSocketNumber(WORD sn)
  {
   socket_number = sn;
  }

  inline WORD         __fastcall TGrNetBase::GetSocketNumber()
  {
   return socket_handle ? socket_number:local_address.sa_socket;
  }

  inline DWORD   __fastcall TGrNetBase::GetWsaError()
  {     return LastWSAError;  }

  inline DWORD       __fastcall TGrNetBase::GetTerminateTime()
  {
   lock();
   DWORD ret =  (WaitRcptTime*RetryCount)*2;
   unlock();
   return ret;
  }

  inline DWORD      __fastcall  TGrNetBase::GetWaitReceiptTimeout(DWORD clid)
  {
   DWORD Ret;
   lock();
   Ret = WaitRcptTime;
   unlock();
   return Ret;
  }

  inline bool       __fastcall  TGrNetBase::SetWaitReceiptTimeout(DWORD clid,DWORD to)
  {
   bool ret(false);
   lock();
   if(to>=10 && to <=1000)
    {WaitRcptTime = to;ret = true;}
   unlock();
   return ret;
  }

  inline DWORD      __fastcall  TGrNetBase::GetRetryCount(DWORD clid)
  {
   DWORD Ret;
   lock();
   Ret = RetryCount;
   unlock();
   return Ret;
  }

  inline bool       __fastcall  TGrNetBase::SetRetryCount(DWORD clid,DWORD rc)
  {
   bool ret(false);
   lock();
   if(rc>=1 && rc <=100)
    {RetryCount = rc;ret = true;}
   unlock();
   return ret;
  }


#endif
