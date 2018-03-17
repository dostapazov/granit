/*
 Интерфейсы классов ввода / вывода
*/

#ifndef _KERTLIO_INC
 #define _KERTLIO_INC

 #include <basetsd.h>
 #include <kertl.hpp>


#ifndef _NO_NAMESPACE
 namespace KeRTL
 {
#endif




 KERTL_CLASS TFile :public THandleBased
 {
   public:
   enum TFileSeekMethod {FromBegin = FILE_BEGIN,FromCurrent = FILE_CURRENT,FromEnd = FILE_END};
   protected:
   __int64  ReadPos ;
   __int64  WritePos;
   public :
                       TFile(const char * FileName,
                             DWORD cd  = OPEN_ALWAYS,
                             DWORD Share = FILE_SHARE_READ,
                             DWORD acc = GENERIC_READ|GENERIC_WRITE,
                             DWORD FlAttr = FILE_ATTRIBUTE_ARCHIVE,
                             LPSECURITY_ATTRIBUTES sa=0,HANDLE htmpl=0
                            );
                       TFile(const wchar_t * FileName,
                             DWORD cd  = OPEN_ALWAYS,
                             DWORD Share = FILE_SHARE_READ,
                             DWORD acc = GENERIC_READ|GENERIC_WRITE,
                             DWORD FlAttr = FILE_ATTRIBUTE_ARCHIVE,
                             LPSECURITY_ATTRIBUTES sa=0,HANDLE htmpl=0
                            );
                        TFile(){};
                        TFile(const TFile & file){*this = file;};
   virtual             ~TFile();
   virtual void  __fastcall   InitFile();
   virtual bool  __fastcall   Open(const char * FileName,
                              DWORD cd  = OPEN_ALWAYS,
                              DWORD Share = FILE_SHARE_READ,
                              DWORD acc = GENERIC_READ|GENERIC_WRITE,
                              DWORD FlAttr = FILE_ATTRIBUTE_ARCHIVE,
                              LPSECURITY_ATTRIBUTES sa=0,HANDLE htmpl=0);

   virtual bool  __fastcall   Open(const wchar_t * FileName,
                              DWORD cd  = OPEN_ALWAYS,
                              DWORD Share = FILE_SHARE_READ,
                              DWORD acc = GENERIC_READ|GENERIC_WRITE,
                              DWORD FlAttr = FILE_ATTRIBUTE_ARCHIVE,
                              LPSECURITY_ATTRIBUTES sa=0,HANDLE htmpl=0);
   virtual void __fastcall Close();

   bool     __fastcall Write(void * data,DWORD sz,LPOVERLAPPED = 0);
   bool     __fastcall Read (void * data,DWORD sz,DWORD & rd,LPOVERLAPPED = 0);
   void     __fastcall CancelIo();
   __int64  __fastcall GetSize();
   bool     __fastcall SetSize(__int64 new_size);
   __int64  __fastcall SeekWr (__int64 pos,DWORD);
   __int64  __fastcall SeekRd (__int64 pos,DWORD);
   __int64  __fastcall GetWritePos();
   __int64  __fastcall GetReadPos();
   void     __fastcall Flush();
   DWORD    __fastcall GetFileType();

   bool     __fastcall EofRd();
   bool     __fastcall EofWr();
   void     __fastcall Synchronize();
   TFile & operator = (const TFile & file){Close();Handle = file();Handle = Dup(false);Synchronize();return *this;};

 };

KERTL_CLASS TCommPort:protected TFile
{
  public:
   TCommPort(){ReadPos = WritePos = 0;};
   TCommPort(int number,bool overlapped=true){OpenPort(number,overlapped);};
   #ifdef _WIN64
   using TFile::CancelIo;
   using TFile::Open;
   using TFile::operator();
   using TFile::IsValid;
   #else
   TFile::CancelIo;
   TFile::Open;
   TFile::operator();
   TFile::IsValid;
   #endif

   void __fastcall Close();
   bool       __fastcall OpenPort(int number,bool overlapped=true);
   int        __fastcall GetSpeed();
   int        __fastcall SetSpeed(int);
   bool       __fastcall GetDCB(DCB & dcb);
   bool       __fastcall SetDCB(DCB & pdcb);
   bool       __fastcall Purge(DWORD Flag = PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);
   bool       __fastcall SetDTR(bool set);
   bool       __fastcall SetRTS(bool set);
   bool       __fastcall SetBreak(bool set);
   bool       __fastcall SetXOFF();
   bool       __fastcall SetXON();
   bool       __fastcall SetTxBuffer(int bytes);
   bool       __fastcall SetRxBuffer(int bytes);
   bool       __fastcall Wait(DWORD & event_mask,LPOVERLAPPED ovrlp = 0);
   bool       __fastcall SetMask(DWORD mask);
   bool       __fastcall GetMask(DWORD & mask);
   bool       __fastcall SetTimeouts(COMMTIMEOUTS & to);
   bool       __fastcall GetTimeouts(COMMTIMEOUTS & to);
   bool       __fastcall GetModemStatus(DWORD & status);
   bool       __fastcall GetProp(COMMPROP & prop);
   bool       __fastcall ClearError(DWORD & error,COMSTAT & cs);
   DWORD      __fastcall Write(LPVOID __buffer,DWORD __len,LPOVERLAPPED ov = 0);
   DWORD      __fastcall Read (LPVOID __buffer,DWORD __bsz,LPOVERLAPPED ov = 0);
};


class TMMapFile:public THandleBased
{
 protected:
 LPVOID   map_address;
 public:
 TMMapFile();
 TMMapFile(HANDLE hFile,const char    * name=NULL,unsigned __int64 map_size = 0,DWORD fprotect = PAGE_READWRITE|SEC_COMMIT,LPSECURITY_ATTRIBUTES sa = NULL);
 TMMapFile(HANDLE hFile,const wchar_t * name     ,unsigned __int64 map_size = 0,DWORD fprotect = PAGE_READWRITE|SEC_COMMIT,LPSECURITY_ATTRIBUTES sa = NULL);
 TMMapFile(const char    * name,DWORD fprotect = PAGE_READWRITE|SEC_COMMIT,BOOL inherit =FALSE);
 TMMapFile(const wchar_t * name,DWORD fprotect = PAGE_READWRITE|SEC_COMMIT,BOOL inherit =FALSE);
 virtual ~TMMapFile();
 bool     __fastcall Open  (const char    * name,DWORD fprotect ,BOOL inherit =FALSE);
 bool     __fastcall Open  (const wchar_t * name,DWORD fprotect ,BOOL inherit =FALSE);
 bool     __fastcall Create(HANDLE hFile,const char    * name=NULL,unsigned __int64 map_size = 0,DWORD fprotect = PAGE_READWRITE|SEC_COMMIT,LPSECURITY_ATTRIBUTES sa = NULL);
 bool     __fastcall Create(HANDLE hFile,const wchar_t * name,unsigned __int64 map_size = 0,DWORD fprotect = PAGE_READWRITE|SEC_COMMIT,LPSECURITY_ATTRIBUTES sa = NULL);
 LPVOID   __fastcall Map   (LPVOID base_addr=NULL,DWORD access = FILE_MAP_ALL_ACCESS,unsigned __int64 offset=0,DWORD size = 0);
 bool     __fastcall Unmap ();
 void     __fastcall Close ();
 bool     __fastcall Flush(DWORD sz = 0);
 DWORD    __fastcall GetSize();
 LPVOID   __fastcall GetMapAddress(){return map_address;}
};


  struct _OVERLAPPED_EX
  {
   OVERLAPPED    ovrlp;
   LPVOID        owner;
  };

 #define IO_NOTHING 0x0000
 #define IO_READ_C  0x0001
 #define IO_READ_O  0x0002
 #define IO_WRITE_C 0x0004
 #define IO_WRITE_O 0x0008
 #define IO_ANYREAD  (IO_READ_C|IO_READ_O)
 #define IO_ANYWRITE (IO_WRITE_C|IO_WRITE_O)

 KERTL_CLASS TOverlappedIO
 {
  public:
  protected:

  HANDLE     io_object;
  _OVERLAPPED_EX   rd_overlapped;
  _OVERLAPPED_EX   wr_overlapped;
  TEvent     rd_event;
  TEvent     wr_event;

  DWORD      WritingError;
  DWORD      WritingBytes;

  DWORD      ReadingError;
  DWORD      ReadingBytes;

  static     void WINAPI __IoCompletionWrite(DWORD Error,DWORD bytes,_OVERLAPPED_EX * ovrlp);
  static     void WINAPI __IoCompletionRead (DWORD Error,DWORD bytes,_OVERLAPPED_EX * ovrlp);
  void       __fastcall  BeginRead (bool UseIoCompletion);
  void       __fastcall  BeginWrite(bool UseIoCompletion);
  void       __fastcall  EndRead   ();
  void       __fastcall  EndWrite  ();

  LPOVERLAPPED_COMPLETION_ROUTINE __fastcall GetCompletionRoutine(DWORD Type);

  DWORD      OpInProgress;
  DWORD      OpDone;
  virtual    bool __fastcall OnIoCompletion(DWORD operation,DWORD Bytes,DWORD error);

  public:
        TOverlappedIO(HANDLE object =0);

  TEvent &     __fastcall GetRdEvent();
  TEvent &     __fastcall GetWrEvent();
  _OVERLAPPED_EX & __fastcall GetRdOverlapped();
  _OVERLAPPED_EX & __fastcall GetWrOverlapped();
  bool    __fastcall Read (__int64 OffsFrom,LPVOID buffer,DWORD size,bool UseIoCompletion = false);
  bool    __fastcall Write(__int64 OffsTo  ,LPVOID buffer,DWORD size,bool UseIoCompletion = false);
  bool    __fastcall WaitDone(bool read,DWORD TimeOut=INFINITE);
  bool    __fastcall CancelIo(){if(OpInProgress &&  ::CancelIo(io_object)){OpInProgress = 0;return true;}return false;};
  DWORD   __fastcall GetWritingBytes(){return WritingBytes;}
  DWORD   __fastcall GetWritingError(){return WritingError;};
  DWORD   __fastcall GetReadingBytes(){return ReadingBytes;}
  DWORD   __fastcall GetReadingError(){return ReadingError;};
  bool    __fastcall SetIoObject(HANDLE obj){return SetIoHandle(obj);}
  bool    __fastcall SetIoHandle(HANDLE obj)
  {
   if(OpInProgress == IO_NOTHING) {io_object = obj;return true;}return false;
  };
  bool __fastcall IsOperationInProgress(DWORD mask){return (OpInProgress&mask) ? true:false; }

 };


KERTL_CLASS TIoCompletionPort:public THandleBased
{
  public:
  TIoCompletionPort(int MaxConcurentThreads = 0);
  bool __fastcall AppendFileHandle(HANDLE hFile,DWORD CompletionKey,int MaxConcurentThreads=0);
  bool __fastcall GetStatus (ULONG_PTR &Key,LPOVERLAPPED & over,DWORD TimeOut = INFINITE,LPDWORD TransferredBytes=0);
  bool __fastcall PostStatus(ULONG_PTR Key,DWORD Bytes,LPOVERLAPPED overlapped);
};


KERTL_CLASS   TMappedFile
{
 protected:
 TFile       file;
 TMMapFile   map_file;
 static
 DWORD       sys_page_size;
 DWORD       inc_value;
 DWORD       mem_size;
 LPBYTE      beg_ptr;
 DWORD       position;
 DWORD       used_size;
 bool   __fastcall init_mm(char    * name,DWORD inc_val ,bool ro);
 bool   __fastcall init_mm(wchar_t * name,DWORD inc_val ,bool ro);
 bool   __fastcall growto (DWORD size);
 bool   __fastcall shrinkto(DWORD size);
 LPBYTE __fastcall get_curr_ptr(){return beg_ptr ? beg_ptr+position : NULL;}
 DWORD  __fastcall get_unused_size(){return mem_size-used_size;}
 virtual
 void   __fastcall after_mapping();
 public:
 TMappedFile();
 TMappedFile(wchar_t * file_name,DWORD inc_val = 4 ,bool ro = false);
 TMappedFile(char    * file_name,DWORD inc_val = 4 ,bool ro = false);
~TMappedFile(){close();};

 bool __fastcall   open  (wchar_t * file_name,DWORD inc_val = 4 ,bool ro = false);
 bool __fastcall   open  ( char   * file_name,DWORD inc_val = 4 ,bool ro = false);
 bool __fastcall   create(wchar_t * file_name,DWORD inc_val = 4 );
 bool __fastcall   create( char   * file_name,DWORD inc_val = 4 );

 void __fastcall   close();

 int    __fastcall write(void * data,DWORD size);
 int    __fastcall read (void * buffer,DWORD sz);
 DWORD  __fastcall seek(DWORD pos,DWORD sm);
 DWORD  __fastcall get_position() {  return position;  }
 DWORD  __fastcall get_size    () {  return used_size; }
 void   __fastcall flush ();
 bool   __fastcall reserve(DWORD size);
 bool   __fastcall is_good(){return beg_ptr ? true:false;};

};


///*******************   inline implementation ********************************




  inline bool     __fastcall TFile::SetSize(__int64 new_size)
  {
   LARGE_INTEGER li;li.QuadPart = new_size;
   SetFilePointer(Handle,li.LowPart,&li.HighPart,FILE_BEGIN);
   if(SetEndOfFile(Handle))
     {

      WritePos = KeRTL::MIN(WritePos,new_size);
      ReadPos  = KeRTL::MIN(ReadPos,new_size);
      return  true;
     } 
    return false;
  }


  inline  __int64  __fastcall TFile::GetWritePos()
   {return WritePos;}

  inline  __int64  __fastcall TFile::GetReadPos()
   {return ReadPos;}


 inline    DWORD    __fastcall TFile::GetFileType()
 {return ::GetFileType(Handle);}

 inline    bool     __fastcall TFile::EofRd()
 {return bool(GetSize()==GetReadPos());}

  inline   bool     __fastcall TFile::EofWr()
  { return bool(GetSize()==GetWritePos()); }

  //*Операторы *

  inline TFile & operator <<(TFile & f,int v)
  {   f.Write(&v,sizeof(v)); return f; }

  inline TFile & operator <<(TFile & f,double v)
  {   f.Write(&v,sizeof(v)); return f; }

  inline TFile & operator <<(TFile & f,float v)
  {   f.Write(&v,sizeof(v)); return f; }

  inline TFile & operator <<(TFile & f,long v)
  {   f.Write(&v,sizeof(v)); return f; }

  inline TFile & operator <<(TFile & f,long double v)
  {   f.Write(&v,sizeof(v)); return f; }

  inline TFile & operator <<(TFile & f,unsigned int v)
  {   f.Write(&v,sizeof(v)); return f; }

  inline TFile & operator <<(TFile & f,unsigned short v)
  {   f.Write(&v,sizeof(v)); return f; }

  inline TFile & operator <<(TFile & f,unsigned long v)
  {   f.Write(&v,sizeof(v)); return f; }

  inline TFile & operator <<(TFile & f,unsigned char v)
  {   f.Write(&v,sizeof(v)); return f; }


  inline TFile & operator <<(TFile & f, char v)
  {   f.Write(&v,sizeof(v)); return f; }



  inline TFile & operator >>(TFile & f,int &v)
  {DWORD rd;  f.Read(&v,sizeof(v),rd);  return f; }

  inline TFile & operator >>(TFile & f,double &v)
  {DWORD rd;  f.Read(&v,sizeof(v),rd);  return f; }

  inline TFile & operator >>(TFile & f,float &v)
  {DWORD rd;  f.Read(&v,sizeof(v),rd);  return f; }

  inline TFile & operator >>(TFile & f,long &v)
  {DWORD rd;  f.Read(&v,sizeof(v),rd);  return f; }

  inline TFile & operator >>(TFile & f,long double &v)
  {DWORD rd;  f.Read(&v,sizeof(v),rd);  return f; }

  inline TFile & operator >>(TFile & f,unsigned long &v)
  {DWORD rd;  f.Read(&v,sizeof(v),rd);  return f; }

  inline TFile & operator >>(TFile & f,unsigned int &v)
  {DWORD rd;  f.Read(&v,sizeof(v),rd);  return f; }

  inline TFile & operator >>(TFile & f,unsigned short &v)
  {DWORD rd;  f.Read(&v,sizeof(v),rd);  return f; }

  inline TFile & operator >>(TFile & f,unsigned char &v)
  {DWORD rd;  f.Read(&v,sizeof(v),rd);  return f; }

  inline TFile & operator >>(TFile & f, char &v)
  {DWORD rd;  f.Read(&v,sizeof(v),rd);  return f; }

//template <class T>
//inline TFile & operator <<(TFile & s,T & t){s.Write(&t,sizeof(t));return s;}
//
//template <class T>
//inline TFile & operator >>(TFile & s,T & t){s.Read(&t,sizeof(t));return s;}




inline  void       __fastcall  TOverlappedIO::BeginRead(bool UseIoCompletion)
{
  OpDone&=~(IO_READ_C|IO_READ_O);
  OpInProgress|= UseIoCompletion ? IO_READ_C:IO_READ_O;
}

inline  void       __fastcall  TOverlappedIO::BeginWrite(bool UseIoCompletion)
{
  OpDone&=~(IO_WRITE_C|IO_WRITE_O);
  OpInProgress|= UseIoCompletion ? IO_WRITE_C:IO_WRITE_O;
}

inline  void       __fastcall  TOverlappedIO::EndRead()
{
 OpDone|= (OpInProgress&IO_ANYREAD);
 OpInProgress&=~IO_ANYREAD;
}

inline  void       __fastcall  TOverlappedIO::EndWrite()
{
 OpDone|= (OpInProgress&IO_ANYWRITE);
 OpInProgress&=~IO_ANYWRITE;
}

  inline   TOverlappedIO::TOverlappedIO(HANDLE _obj):
           io_object(_obj),
           OpInProgress(0),
           OpDone(0),
           WritingError(0),
           WritingBytes(0),
           ReadingError(0),
           ReadingBytes(0)
           {
           ZeroMemory(&rd_overlapped,sizeof(rd_overlapped));rd_overlapped.ovrlp.hEvent = rd_event();rd_overlapped.owner = this;
           ZeroMemory(&wr_overlapped,sizeof(wr_overlapped));wr_overlapped.ovrlp.hEvent = wr_event();wr_overlapped.owner = this;
           }



  inline   TEvent & __fastcall TOverlappedIO::GetRdEvent()
  {return rd_event;};
  inline   TEvent &     __fastcall TOverlappedIO::GetWrEvent()
  {return wr_event;}
  inline   _OVERLAPPED_EX & __fastcall TOverlappedIO::GetRdOverlapped()
  {return rd_overlapped;}
  inline   _OVERLAPPED_EX & __fastcall TOverlappedIO::GetWrOverlapped()
  {return wr_overlapped;};

inline DWORD      __fastcall TCommPort::Write(LPVOID __buffer,DWORD __len,LPOVERLAPPED ov )
{
 DWORD ret;
 if(!WriteFile(Handle,__buffer,__len ,&ret,ov))
      ret = -1;
  return ret;
}

inline DWORD      __fastcall TCommPort::Read (LPVOID __buffer,DWORD __bsz,LPOVERLAPPED ov )
{
 DWORD ret;
 if(!ReadFile(Handle,__buffer,__bsz,&ret,ov))
      ret = -1;
 return ret;     
}

inline   bool       __fastcall TCommPort::SetDTR(bool set)
{ return EscapeCommFunction(Handle,set ? SETDTR :CLRDTR) ? true:false;}
inline   bool       __fastcall TCommPort::SetRTS(bool set)
{ return EscapeCommFunction(Handle,set ? SETRTS :CLRRTS) ? true:false;}
inline   bool       __fastcall TCommPort::SetBreak(bool set)
{ return EscapeCommFunction(Handle,set ? SETBREAK :CLRBREAK) ? true:false;}

inline   bool       __fastcall TCommPort::SetXOFF()
{ return EscapeCommFunction(Handle,SETXOFF) ? true:false;}
inline   bool       __fastcall TCommPort::SetXON()
{ return EscapeCommFunction(Handle,SETXON) ? true:false;}

inline   bool       __fastcall TCommPort::Wait(DWORD & event_mask,LPOVERLAPPED ovrlp )
{ return WaitCommEvent(Handle,&event_mask,ovrlp) ? true:false;}
inline   bool       __fastcall TCommPort::SetMask(DWORD  event_mask)
{return  ::SetCommMask(Handle,event_mask)? true:false;}
inline   bool       __fastcall TCommPort::GetMask(DWORD & event_mask)
{return  ::GetCommMask(Handle,&event_mask)? true:false;}

inline   bool       __fastcall TCommPort::GetDCB(DCB & dcb)
   {
    ZeroMemory(&dcb,sizeof(dcb));
    dcb.DCBlength = sizeof(dcb);
    return GetCommState(this->Handle,&dcb)? true:false;
   }

inline   bool       __fastcall TCommPort::SetDCB(DCB & dcb)
{    return SetCommState(this->Handle,&dcb) ? true:false;}

inline   bool       __fastcall TCommPort::SetTimeouts(COMMTIMEOUTS & to)
{
 return SetCommTimeouts(Handle,&to) ? true:false;
}
inline   bool       __fastcall TCommPort::GetTimeouts(COMMTIMEOUTS & to)
{
 return GetCommTimeouts(Handle,&to)? true:false;
}
inline   bool       __fastcall TCommPort::GetModemStatus(DWORD & status)
{
 return GetCommModemStatus(Handle,&status) ? true:false;
}
inline   bool       __fastcall TCommPort::GetProp(COMMPROP & prop)
{
 return GetCommProperties(Handle,&prop)? true:false;
}

inline bool       __fastcall TCommPort::Purge(DWORD Flag )
{
  return PurgeComm(Handle,Flag) ? true:false;
}

inline   bool       __fastcall TCommPort::ClearError(DWORD & error,COMSTAT & cs)
{
 return ClearCommError(Handle,&error,&cs) ? true:false;
}


inline    TMMapFile::TMMapFile():
THandleBased(NULL),map_address(NULL)
{}

inline    TMMapFile::TMMapFile(HANDLE hFile,const char    * name,unsigned __int64 map_size,DWORD fprotect , LPSECURITY_ATTRIBUTES sa):
THandleBased(NULL),map_address(NULL)
{
 Create(hFile,name,map_size,fprotect,sa);
}

inline    TMMapFile::TMMapFile(HANDLE hFile,const wchar_t * name,unsigned __int64 map_size,DWORD fprotect ,LPSECURITY_ATTRIBUTES sa ):
THandleBased(NULL),map_address(NULL)
{
 Create(hFile,name,map_size,fprotect,sa);
}
inline    TMMapFile::TMMapFile(const char    * name,DWORD fprotect ,BOOL inherit )
{
 Open(name,fprotect,inherit);
}

inline    TMMapFile::TMMapFile(const wchar_t * name,DWORD fprotect ,BOOL inherit )
{
 Open(name,fprotect,inherit);
}


inline bool     __fastcall TMMapFile::Create(HANDLE hFile,const char    * name, unsigned __int64 map_size, DWORD fprotect ,LPSECURITY_ATTRIBUTES sa )
{
 Close();
 Handle = CreateFileMappingA(hFile,sa,fprotect,((LARGE_INTEGER*)&map_size)->HighPart,((LARGE_INTEGER*)&map_size)->LowPart,name);
 return IsValid();
}

inline bool     __fastcall TMMapFile::Create(HANDLE hFile,const wchar_t * name, unsigned __int64 map_size, DWORD fprotect,LPSECURITY_ATTRIBUTES sa )
{
  Close();
  Handle = CreateFileMappingW(hFile,sa,fprotect,((LARGE_INTEGER*)&map_size)->HighPart,((LARGE_INTEGER*)&map_size)->LowPart,name);
  return IsValid();
}

inline bool     __fastcall TMMapFile::Open(const char    * name,DWORD fprotect ,BOOL inherit )
{
  Close();
  Handle = OpenFileMappingA(fprotect,inherit,name);
  return IsValid();
}

inline bool     __fastcall TMMapFile::Open(const wchar_t * name ,DWORD fprotect ,BOOL inherit  )
{
  Close();
    Handle = OpenFileMappingW(fprotect,inherit,name);
  return IsValid();
}

inline DWORD  __fastcall TMMapFile::GetSize()
{
  MEMORY_BASIC_INFORMATION mbi;
  mbi.RegionSize = 0;
  if(map_address)
     VirtualQuery(map_address,&mbi,sizeof(mbi));
  return mbi.RegionSize; 
}




inline    LPVOID   __fastcall TMMapFile::Map(LPVOID base_addr,DWORD access  ,unsigned __int64 offset,DWORD size )
{
 if(map_address == NULL)
 {
   LARGE_INTEGER * li = (LARGE_INTEGER*)&offset;
   map_address = MapViewOfFileEx(Handle,access,li->HighPart,li->LowPart,size,base_addr);
 }
 return map_address;
}

inline    bool     __fastcall TMMapFile::Unmap()
{
 if(map_address && UnmapViewOfFile(map_address))
    {map_address = NULL;return true;}
    return false;
}

inline    void     __fastcall TMMapFile::Close()
{
 Flush();
 Unmap();
 THandleBased::Close();
}

inline    bool     __fastcall TMMapFile::Flush(DWORD sz)
{
 if(map_address)
    return FlushViewOfFile(map_address,sz) ? true:false;
  return false;
}


inline    TMMapFile::~TMMapFile()
{
 Close();
}



#ifndef _NO_NAMESPACE
 }
#endif

#endif




