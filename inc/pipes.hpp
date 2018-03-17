#ifndef _PIPES_INC_
#define _PIPES_INC_

#ifndef _KERTL_INC
 #include <kertlio.hpp>
#endif

#ifndef _NO_NAMESPACE
namespace KeRTL
{
#endif



 KERTL_CLASS TNamedPipe:public THandleBased
 {
  protected:
  DWORD  LastError;
  
  TNamedPipe()
  {LastError = 0;};
  public:
  //virtual ~TNamedPipe();

  struct TState
  {
    DWORD State;	        // address of flags indicating pipe state
    DWORD CurInstances;	        // address of number of current pipe instances
    DWORD MaxCollectionCount;	// address of max. bytes before remote transmission
    DWORD CollectDataTimeout;	// address of max. time before remote transmission
    TCHAR UserName[MAX_PATH];	// address of user name of client process

  };

  struct TInfo
  {
    DWORD Flags;	        // address of flags indicating type of pipe
    DWORD OutBufferSize;	// address of size, in bytes, of pipe's output buffer
    DWORD InBufferSize;	        // address of size, in bytes, of pipe's input buffer
    DWORD MaxInstances;	        // address of max. number of pipe instances
  };


  
virtual
  void     __fastcall Close();

  bool     __fastcall GetState   (TState & state);
  bool     __fastcall SetMode    (DWORD mode);
  bool     __fastcall SetMaxCount(DWORD count);
  bool     __fastcall SetTimeOut (DWORD TimeOut);
  bool     __fastcall GetInfo    (TInfo  & info);

  bool     __fastcall GetTotalBytesCount(DWORD &);
  bool     __fastcall GetLeftBytesCount(DWORD  &);
  bool     __fastcall PeekBytes(LPVOID buffer,DWORD cbBytes,LPDWORD ReadBytes);
  bool     __fastcall ReadBytes(LPVOID buffer,DWORD cbBytes,LPDWORD rdBytes = 0,LPOVERLAPPED = 0);
  bool     __fastcall WriteBytes(LPVOID buffer,DWORD cbWrite,LPDWORD wrBytes = 0,LPOVERLAPPED = 0);
  bool     __fastcall Transact  (LPVOID wrBuff,DWORD cbToWrite,LPVOID rdBuff,DWORD cbRead,DWORD& BytesRead ,LPOVERLAPPED =0);
  DWORD    __fastcall GetError(){return LastError;}
};


// Server edn of pipe

KERTL_CLASS TServerPipe:public TNamedPipe
{
  public:
  TServerPipe(){};
  TServerPipe(LPSTR name,DWORD OutSize,DWORD InSize,
              DWORD pType        = (PIPE_TYPE_BYTE|PIPE_READMODE_BYTE),
              DWORD pMode        = (PIPE_ACCESS_DUPLEX|FILE_FLAG_OVERLAPPED|WRITE_OWNER),
              DWORD MaxInstances = 1,
              DWORD DefTimeOut   = NMPWAIT_USE_DEFAULT_WAIT,
              LPSECURITY_ATTRIBUTES sa = 0
              );

  bool    __fastcall   Open(char * name,
                       DWORD OutSize,
                       DWORD InSize,
                       DWORD PipeMode = (PIPE_TYPE_BYTE|PIPE_READMODE_BYTE),
                       DWORD OpenMode = (PIPE_ACCESS_DUPLEX|FILE_FLAG_OVERLAPPED|WRITE_OWNER),
                       DWORD maxInst  = PIPE_UNLIMITED_INSTANCES,
                       DWORD DefTimeOut =NMPWAIT_USE_DEFAULT_WAIT ,
                       LPSECURITY_ATTRIBUTES = 0 ) ;

  bool    __fastcall   Open(wchar_t * name,
                       DWORD OutSize,
                       DWORD InSize,
                       DWORD PipeMode = (PIPE_TYPE_BYTE|PIPE_READMODE_BYTE),
                       DWORD OpenMode = (PIPE_ACCESS_DUPLEX|FILE_FLAG_OVERLAPPED|WRITE_OWNER),
                       DWORD maxInst  =  PIPE_UNLIMITED_INSTANCES,
                       DWORD DefTimeOut= NMPWAIT_USE_DEFAULT_WAIT ,
                       LPSECURITY_ATTRIBUTES = 0 ) ;
  bool    __fastcall Connect(LPOVERLAPPED =0);
  bool    __fastcall Disconnect();
  void    __fastcall Close();

};


class TClientPipe:public TNamedPipe
{
  public:
  TClientPipe(){}
  TClientPipe(char * name,
              DWORD Access,
              DWORD ShareMode,
              DWORD CreationDistribute,
              DWORD Flags,
              LPSECURITY_ATTRIBUTES sa =0);


  bool    __fastcall   Open(char * name,
                       DWORD Access,
                       DWORD ShareMode,
                       DWORD CreationDistribute,
                       DWORD Flags,
                       DWORD Reserved =0,
                       DWORD DefTimeOut = NMPWAIT_USE_DEFAULT_WAIT ,
                       LPSECURITY_ATTRIBUTES = 0 ) ;

  bool    __fastcall   Open(wchar_t * name,
                       DWORD Access,
                       DWORD ShareMode,
                       DWORD CreationDistribute,
                       DWORD Flags,
                       DWORD Reserved =0,
                       DWORD DefTimeOut = NMPWAIT_USE_DEFAULT_WAIT ,
                       LPSECURITY_ATTRIBUTES = 0 ) ;
};

//inline implementations


//inline  TNamedPipe::~TNamedPipe()
//{/*Close();*/}

inline  void __fastcall TNamedPipe::Close()
{
 if(IsValid())
 {
  CloseHandle(Handle);
  Handle = INVALID_HANDLE_VALUE;
 }
}


inline  bool    __fastcall TNamedPipe::GetState(TNamedPipe::TState & s)
{
  ZeroMemory(&s,sizeof(s));
  return GetNamedPipeHandleState(Handle,&s.State,&s.CurInstances,
                                 &s.MaxCollectionCount,
                                 &s.CollectDataTimeout,
                                 s.UserName,
                                 sizeof(s.UserName)/sizeof(TCHAR)) ? true:false;
}

inline  bool __fastcall TNamedPipe::GetInfo (TNamedPipe::TInfo  & info)
{
   ZeroMemory(&info,sizeof(info));
   if(GetNamedPipeInfo(Handle,&info.Flags,&info.OutBufferSize,&info.InBufferSize,&info.MaxInstances))
    return true;
    LastError = GetLastError();
    return false;
};

inline    bool __fastcall TNamedPipe::SetMode    (DWORD mode)
{
  if(SetNamedPipeHandleState(Handle,&mode,0,0))
    return true;
    LastError = GetLastError();
    return false;

}

inline    bool __fastcall TNamedPipe::SetMaxCount(DWORD count)
{
  if(SetNamedPipeHandleState(Handle,0,&count,0))
    return true;
    LastError = GetLastError();
    return false;
}

inline    bool __fastcall TNamedPipe::SetTimeOut (DWORD TimeOut)
{
  if(SetNamedPipeHandleState(Handle,0,0,&TimeOut))
    return true;
    LastError = GetLastError();
    return false;

}

inline      bool __fastcall TNamedPipe::GetTotalBytesCount(DWORD & bc)
{
 if(PeekNamedPipe(Handle,0,0,0,&bc,0))
    return true;
    LastError = GetLastError();
    return false;
}

inline      bool __fastcall TNamedPipe::GetLeftBytesCount(DWORD & bl)
{
 if(PeekNamedPipe(Handle,0,0,0,0,&bl))
    return true;
    LastError = GetLastError();
    return false;
}

inline      bool __fastcall TNamedPipe::PeekBytes(LPVOID buffer,DWORD cbBytes,LPDWORD ReadBytes)
{
 if(PeekNamedPipe(Handle,buffer,cbBytes,ReadBytes,0,0))
    return true;
    LastError = GetLastError();
    return false;

}


inline      bool __fastcall TNamedPipe::ReadBytes(LPVOID buffer,DWORD cbBytes,LPDWORD ReadBytes,LPOVERLAPPED ovrlp)
{
 if(ReadFile(Handle,buffer,cbBytes,ReadBytes,ovrlp))
    return true;
    LastError = GetLastError();
    return false;
}

inline   bool __fastcall TNamedPipe::WriteBytes(LPVOID buffer,DWORD cbWrite,LPDWORD wrBytes ,LPOVERLAPPED ovrlp)
{
 DWORD _wrBytes =0;
 bool ret(false);
 if(WriteFile(Handle,buffer,cbWrite,&_wrBytes,ovrlp))
    ret = true;
    LastError = GetLastError();
  if(wrBytes)
     *wrBytes = _wrBytes;
 return ret;
}

inline   bool __fastcall TNamedPipe::Transact  (LPVOID wrBuff,DWORD cbToWrite,LPVOID rdBuff,DWORD cbRead,DWORD& BytesRead ,LPOVERLAPPED ovrlp)
{
 if(TransactNamedPipe(Handle,wrBuff,cbToWrite,rdBuff,cbRead,&BytesRead,ovrlp))
    return true;
    LastError = GetLastError();
    return false;

}

inline TServerPipe::TServerPipe(LPSTR name,DWORD OutSize,DWORD InSize,DWORD PipeMode,DWORD OpenMode ,
                         DWORD maxInst,DWORD DefTimeOut, LPSECURITY_ATTRIBUTES sa):
             TNamedPipe ()
 {
  Open(name,OutSize,InSize,PipeMode,OpenMode,maxInst,DefTimeOut,sa);
 }


inline  void __fastcall TServerPipe::Close()
{
 Disconnect();
 TNamedPipe::Close();
}

inline bool __fastcall TServerPipe::Connect(LPOVERLAPPED ovrlp)
{
 if(ConnectNamedPipe(Handle,ovrlp) )
  return true;
  else
  LastError = GetLastError();
  return false;
}


inline  bool __fastcall TServerPipe::Disconnect()
{
 return DisconnectNamedPipe(Handle) ? true:false;
}


inline   TClientPipe::TClientPipe(LPSTR name,
              DWORD Access,
              DWORD ShareMode,
              DWORD CreationDistribute,
              DWORD Flags,
              LPSECURITY_ATTRIBUTES sa )
  {
    Open(name,Access,ShareMode,CreationDistribute,Flags,0,NMPWAIT_USE_DEFAULT_WAIT,sa);
  }


#ifndef _NO_NAMESPACE
}
 //Enf of namespace
#endif


#endif
