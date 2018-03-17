#include <kertlio.hpp>
#include <winerror.h>
//#include <winbase.h>

#ifndef _NO_NAMESPACE
 namespace KeRTL
 {
#endif

  LPOVERLAPPED_COMPLETION_ROUTINE __fastcall TOverlappedIO::GetCompletionRoutine(DWORD Type)
  {
    if(Type & IO_ANYREAD)
    {
     return LPOVERLAPPED_COMPLETION_ROUTINE(__IoCompletionRead);
    }
    return  LPOVERLAPPED_COMPLETION_ROUTINE(__IoCompletionWrite);

  }


//Процедура завершения
void WINAPI TOverlappedIO::__IoCompletionRead(DWORD Error,DWORD bytes,_OVERLAPPED_EX * ovrlp)
{
 TOverlappedIO * io = reinterpret_cast<TOverlappedIO*>( ovrlp->owner);
 if(io)
 {
 io->ReadingError = Error;
 io->ReadingBytes +=bytes;
 if(io->OnIoCompletion(io->OpInProgress&IO_ANYREAD,bytes,Error))
    io->rd_event.SetEvent(true);
 io->EndRead();
 }

}

void WINAPI TOverlappedIO::__IoCompletionWrite(DWORD Error,DWORD bytes,_OVERLAPPED_EX * ovrlp)
{
 TOverlappedIO * io = reinterpret_cast<TOverlappedIO*>( ovrlp->owner);
 if(io)
 {
 io->WritingError = Error;
 io->WritingBytes  +=bytes;
 if(io->OnIoCompletion(io->OpInProgress&IO_ANYWRITE,bytes,Error))
    io->wr_event.SetEvent(true);
 io->EndWrite();   
 }

}

 #pragma warn -8057
  bool __fastcall TOverlappedIO::OnIoCompletion(DWORD Operation,DWORD Bytes,DWORD error)
  {return true;}
 #pragma warn .8057

  bool    __fastcall TOverlappedIO::Read (__int64 from, LPVOID buffer, DWORD size, bool UseIoCompletion)
  {
   bool ret(false);
   if((OpInProgress&IO_ANYREAD) == IO_NOTHING)
   {
     rd_event.SetEvent(false);
     BeginRead(UseIoCompletion);
     this->ReadingBytes = 0;
     LARGE_INTEGER li;
     li.QuadPart = from;
     rd_overlapped.ovrlp.Offset = li.LowPart;
     rd_overlapped.ovrlp.OffsetHigh = li.HighPart;
     if(UseIoCompletion)
     ret = ReadFileEx(io_object,buffer,size,&this->rd_overlapped.ovrlp,this->GetCompletionRoutine(OpInProgress&IO_ANYREAD))?true:false;
     else
     ret = ReadFile(io_object,buffer,size,&ReadingBytes,&this->rd_overlapped.ovrlp)?true:false;

     ReadingError = GetLastError();
     DWORD Error = ReadingError;

     if(ret || Error == ERROR_HANDLE_EOF)
      {
       //Завершено!
       if(UseIoCompletion)
           {
            if(SleepEx(0,true)!=WAIT_IO_COMPLETION)// Дать возможность выполнится Queued APC
            {
               ret = false;ReadingError = GetLastError();
            }
           }
           else
           this->__IoCompletionRead(Error,0,&rd_overlapped);
           return ret;
      }



     if( Error != ERROR_IO_PENDING)
       {
       //Чето не так
        OpInProgress&=~IO_ANYREAD;
       }
   }
    return ret;
  }

  bool    __fastcall TOverlappedIO::Write(__int64 OffsTo,LPVOID buffer,DWORD size,bool UseIoCompletion )
  {
   bool ret(false);
   if((OpInProgress&IO_ANYWRITE) == IO_NOTHING)
   {
     BeginWrite(UseIoCompletion);
     this->WritingBytes = 0;
     wr_event.SetEvent(false);
     LARGE_INTEGER li;
     li.QuadPart = OffsTo;
     wr_overlapped.ovrlp.Offset = li.LowPart;
     wr_overlapped.ovrlp.OffsetHigh = li.HighPart;
     if(UseIoCompletion)
     ret = WriteFileEx(io_object,buffer,size,&wr_overlapped.ovrlp,this->GetCompletionRoutine(OpInProgress&IO_ANYWRITE))?true:false;
     else
     ret = WriteFile(io_object,buffer,size,&WritingBytes,&wr_overlapped.ovrlp) ? true:false;

     DWORD Error  = GetLastError();
     WritingError = Error;

     if(ret)
     {
      //Операция записи завершена
       if(UseIoCompletion)
       {
        if(SleepEx(0,true)!=WAIT_IO_COMPLETION)
          {ret = false;WritingError=GetLastError();}
       }
       else
        this->__IoCompletionWrite(Error,WritingBytes,&wr_overlapped);

     }
     else
     {
      if(Error!=ERROR_IO_PENDING)
       {
        OpInProgress&=~IO_ANYWRITE;
       }
     }

   }
     return ret;
  }

  bool    __fastcall TOverlappedIO::WaitDone(bool read,DWORD TimeOut)
  {
   bool ret(false);

   if(this->OpInProgress &(read ? IO_ANYREAD:IO_ANYWRITE) )
   {
     DWORD WaitResult;
     if( OpInProgress& (IO_WRITE_C|IO_READ_C))
        {
         WaitResult = SleepEx(TimeOut,true);
         ret = ((WaitResult == WAIT_IO_COMPLETION) && (OpDone & (read? IO_ANYREAD:IO_ANYWRITE))) ? true:false;
        }
        else
        {
         ret = WaitForSingleObject(read ? rd_event():wr_event(),TimeOut) == WAIT_OBJECT_0 ? true:false;
         if(ret)
            {
             DWORD Bytes;
             GetOverlappedResult(io_object,read ? &rd_overlapped.ovrlp:&wr_overlapped.ovrlp,&Bytes,false);
             if(read)
              __IoCompletionRead(GetLastError(),Bytes,&rd_overlapped);
              else
              __IoCompletionWrite(GetLastError(),Bytes,&wr_overlapped);
            }

        }

   }
   if(!ret)
   {
	if(read)
	ReadingError = GetLastError();
	else
	WritingError = GetLastError();
   }

   return ret;
  }


#ifndef _NO_NAMESPACE
 }
#endif



 
