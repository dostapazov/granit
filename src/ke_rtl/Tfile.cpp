
#include <KeRTLio.hpp>

namespace KeRTL
{

   void __fastcall TFile::Flush()
   {
     FlushFileBuffers((*this)());
   }

   TFile::~TFile()
   {
    Close();
   }

   TFile::TFile(const char * FileName,
                             DWORD cd ,
                             DWORD Share ,
                             DWORD acc ,
                             DWORD FlAttr ,
                             LPSECURITY_ATTRIBUTES sa,HANDLE htmpl
                            )
   {
    Open(FileName,cd,Share,acc,FlAttr,sa,htmpl);
   }
   bool  __fastcall   TFile::Open(const char * FileName,
                              DWORD cd  ,
                              DWORD Share ,
                              DWORD acc ,
                              DWORD FlAttr ,
                              LPSECURITY_ATTRIBUTES sa,HANDLE htmpl)
   {


     Handle = CreateFileA(FileName,acc,Share,sa,cd,FlAttr|FILE_FLAG_RANDOM_ACCESS,htmpl);
     bool ret = IsValid();
     if(ret)
         InitFile();
     return ret;
   }

   TFile::TFile(const wchar_t * FileName,
                             DWORD cd ,
                             DWORD Share ,
                             DWORD acc ,
                             DWORD FlAttr ,
                             LPSECURITY_ATTRIBUTES sa,HANDLE htmpl
                            )
   {
    Open(FileName,cd,Share,acc,FlAttr,sa,htmpl);
   }
   bool  __fastcall   TFile::Open(const wchar_t * FileName,
                              DWORD cd  ,
                              DWORD Share ,
                              DWORD acc ,
                              DWORD FlAttr ,
                              LPSECURITY_ATTRIBUTES sa,HANDLE htmpl)
   {

     Handle = CreateFileW(FileName,acc,Share,sa,cd,FlAttr|FILE_FLAG_RANDOM_ACCESS,htmpl);
     bool ret = IsValid();
     if(ret)
         InitFile();
     return ret;
   }
   void __fastcall TFile::Close()
   {

       if(IsValid())
       {
        this->Flush();
        CloseHandle(Handle);
        Handle = INVALID_HANDLE_VALUE;
       }
   }

   void __fastcall   TFile::InitFile()
   {
    ZeroMemory(&ReadPos,sizeof(ReadPos));
    WritePos = GetSize();
   }

   __int64   __fastcall TFile::SeekWr (__int64 pos,DWORD seek )
   {
    __int64 Eof= GetSize();
    __int64 prev = WritePos;
    switch(seek)
    {
     case FromBegin  :
                      WritePos = pos;
                      WritePos = MAX(( __int64)0,MIN(WritePos,Eof));
                      break;
     case FromEnd    :
                      WritePos = Eof;
     case FromCurrent:
                     {
                      WritePos+=pos;
                      WritePos = MAX(( __int64)0,MIN(WritePos,Eof));
                     }
                     break;
    }
    return prev;
   }

   __int64   __fastcall TFile::SeekRd (__int64 pos,DWORD seek )
   {
    __int64 Eof  = GetSize();
    __int64 prev = ReadPos;
    switch(seek)
    {
     case FromBegin  :
                      ReadPos = pos;
                      ReadPos = MAX(( __int64)0,MIN(ReadPos,Eof));
                      break;
     case FromEnd    :
                      ReadPos = Eof;
     case FromCurrent:
                     {
                      ReadPos+=pos;
                      ReadPos = MAX(( __int64)0,MIN(ReadPos,Eof));
                     }
                     break;
    }
    return prev;
   }


   bool __fastcall TFile::Write(void * data,DWORD sz,LPOVERLAPPED ov)
   {
    DWORD WrBytes(0);
    LARGE_INTEGER wr_pos ;wr_pos.QuadPart=WritePos;
    if(ov)
       {
        ov->Offset     = wr_pos.LowPart;
        ov->OffsetHigh = wr_pos.HighPart;
       }
       else
       SetFilePointer(Handle,wr_pos.LowPart,&wr_pos.HighPart,FILE_BEGIN);

    bool Ret(WriteFile(Handle,data,sz,&WrBytes, ov)? true:false);

    #if defined  _DBG ||  defined_DEBUG
    if(!Ret)
    {
     DWORD LastError = GetLastError();
    }
    #endif

    if(Ret)
     WritePos+=WrBytes;
    return Ret;
   }


   bool __fastcall TFile::Read (void * data,DWORD sz,DWORD & rd,LPOVERLAPPED ov)
   {
     LARGE_INTEGER rd_pos; rd_pos.QuadPart = ReadPos;
    if(ov)
       {
        ov->Offset     = rd_pos.LowPart;
        ov->OffsetHigh = rd_pos.HighPart;
       }
       else
       SetFilePointer(Handle,rd_pos.LowPart,&rd_pos.HighPart,FILE_BEGIN);

	bool Ret(ReadFile(Handle,data,sz,&rd,ov)? true:false);
    #if defined  _DBG ||  defined_DEBUG
     if(!Ret)
     {
      DWORD LastError = GetLastError();
     }
     #endif
     if(Ret)
     ReadPos+=rd;
     return Ret;
   }



   void __fastcall TFile::CancelIo()
   {
    ::CancelIo(Handle);
   }


   __int64 __fastcall TFile::GetSize()
   {
    LARGE_INTEGER sz;
    sz.QuadPart = 0;
    if(this->IsValid())
      sz.LowPart =  GetFileSize(Handle,(ULONG*)&sz.HighPart);
    return sz.QuadPart;
   }

   void     __fastcall TFile::Synchronize()
   {
     LARGE_INTEGER pos ;
     pos.QuadPart = 0;
     pos.LowPart = SetFilePointer(Handle,0,&pos.HighPart,FILE_CURRENT);
     this->ReadPos = this->WritePos = pos.QuadPart;
   }

}
