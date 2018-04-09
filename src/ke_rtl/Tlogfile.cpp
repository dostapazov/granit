#include <TLogfile.hpp>

//Задача лог файла
//В текстовый файл писать   стоки типа

// Пример для guard.log (см. guard.dll, guardhlp.dll)

//*****************************************************************************
// 04.04.2000 20:23:11
// Исключение нарушение прав доступа
// по адресу 045D6F7Eh в модуле 'ke_rtl.dll' из нити "Unnamed" ID 0200h
// нить перезапущена  // или // нить остановлена.

// 05.04.200 11:32:55
// При завершении нити "MaxSubDriver" модуль 'mxdrv.sdr'
// остались неосвобожденные системные ресурсы
//           1) Устройство ..\.\hdlc$1 открыто из модуля 'mxdrv.sdr'
//           2) Событие    'Threads shutdown event' создано из модуля 'mxdrv.sdr'


namespace KeRTL
{

TLogEntry::TLogEntry(const char * aHeader,const char * aText)
{
  Next = 0;
  GetLocalTime(&s_time);
  int h_len = aHeader ? lstrlenA(aHeader)+1 : 1;
  int t_len = aText ? lstrlenA(aText)+1 : 1;
  try{
      Header = ::new char[h_len+t_len+1];
     }
     catch(...)
     {Header = Text = 0;}

   if(Header)
   {
     if(aHeader)
      lstrcpyA(Header,aHeader);
       else
        *Header = 0;
     Text = Header + h_len;
     if(aText)
      lstrcpyA(Text,aText);
       else
        *Text = 0;
   }
}





    bool   __fastcall TLogFile::OpenLog(LPCSTR LogFile,DWORD LimitSize,bool Threading)
    {
     this->LimitSize = LimitSize;
     bool ret(Locker.Lock(2000));
     if(ret)
     {
     if(File)//Если лог открыт то закрыть
       CloseLog();

       try{
            File = ::new TFile(LogFile);
            if(Threading)
             {
               #ifdef __BORLANDC__
               LogQueue    = ::new TLogQueue;
               #endif
               wrlg_thread = ::new TWriteLogThread(this);
               wrlg_thread->Start(4096,true);
               wrlg_thread->SetThreadPriority(DWORD(TThread::tpIdle));
               wrlg_thread->Resume();
             }

             if(!File || !(*File)())
                throw int(-1);
          }
          catch(...)
          {
           ret = false;
           CloseLog();
          }
      Locker.Unlock();
     }

     return ret;
    }

    bool   __fastcall TLogFile::CloseLog()
    {
      bool ret(this->Locker.Lock(2000));
      if(ret)
      {
        if(this->wrlg_thread)
        {
          wrlg_thread->Terminate();
          ::delete wrlg_thread;
        }
        #ifdef __BORLANDC__
        if(this->LogQueue)
           {
            FlushLogQueue();
            ::delete LogQueue;
           }
        #else
        FlushLogQueue();
        #endif

        if(this->File)
           ::delete File;
        wrlg_thread = 0;
#ifdef __BORLANDC__
        LogQueue    = 0;
#else
		First = Last = 0;
#endif
        File        = 0;
        Locker.Unlock();
      }
      return ret;
    }

    bool   __fastcall TLogFile::WriteToLog(LPCSTR Header,LPCSTR Text)
    {
		bool Ret(File ? true:false);

     if(Ret && Locker.Lock(2000))
     {
      TLogEntry le(Header,Text);
      if(wrlg_thread)
        {
         #ifdef __BORLANDC__
         LogQueue->push_back(le);
         #else
         //Под MSVC
         if(Last)
           {
            Last->Next = new TLogEntry(le);
            Last = Last->Next;
           }
           else
           First = Last = new TLogEntry(le);
         #endif
         wrlg_thread->SignalWrite();
        }
        else
          {
           WriteEntry(le);
           le.Release();
          }

       Locker.Unlock();
     }
      else
       Ret = false;
     return Ret;
    }

    #ifndef __BORLANDC__
    bool   __fastcall TLogFile::RemoveFirst()
    {
     if(First )
     {
      TLogEntry * le = First;
      First = le->Next;
      delete  le;
      if(First)
         {
          Last = First->Next ? First->Next : First;
          return true;
         }
         else
         Last = 0;

     }

     return false;
    }
    #endif

    void __fastcall SpaceZeroCh(char * str)
    {
     while(*str)
     {     if(*str==' ')
                *str = '0';
             str++;   
     }
    }

    bool __fastcall TLogFile::WriteEntry(TLogEntry & e)
    {
		bool ret(File ? true:false);

      if(ret && Locker.Lock())
      {
      if(LimitSize!=DWORD(-1) && File->GetSize()>=LimitSize && File->GetWritePos()>=LimitSize)
         File->SeekWr(0,FILE_BEGIN);
      try{
          char * CRLF  = (char*) "\r\n";
          char   Eol   = 0x1A;
          char   td_str[32];

          int len =
          wsprintfA(td_str,"%2d.%2d.%2d",e.s_time.wDay,e.s_time.wMonth,e.s_time.wYear);
          SpaceZeroCh(td_str);
          len+=wsprintfA(td_str+len,"  ");
          wsprintfA(td_str+len,"%2d:%2d:%2d",e.s_time.wHour,e.s_time.wMinute,e.s_time.wSecond);
          SpaceZeroCh(td_str+len);
          File->Write(td_str,lstrlenA(td_str));
          File->Write(CRLF,2);
          File->Write(e.Header,lstrlenA(e.Header));
          File->Write(CRLF,2);
          File->Write(e.Text,lstrlenA(e.Text));
          File->Write(CRLF,2);
          File->Write(&Eol,1);
          File->Write(CRLF,2);

         }
         catch(...)
         {ret = false;}
        Locker.Unlock();
      }
      return ret;
    }

 void  __fastcall TLogFile::FlushLogQueue()
 {
  if(Locker.Lock(2000))
  {
   #ifdef __BORLANDC__
   TLogEntry le;
   while (LogQueue && !LogQueue->empty())
   {
    le = LogQueue->at(0);
    LogQueue->erase(LogQueue->begin());
    WriteEntry(le);
    le.Release();
   }
   #else
    TLogEntry * le = First;
    while(le)
    {
     WriteEntry(*le);
     RemoveFirst();
     le = First;
    }
   #endif

   Locker.Unlock();
  }
 }


     //Реализация нитевидной формы записи в LOG
      TLogFile::TWriteLogThread::TWriteLogThread (TLogFile* lfile):
                           log_file(lfile),
                           TerminateEvent(CreateEvent(0,FALSE,FALSE,0)),
                           WriteEvent    (CreateEvent(0,FALSE,FALSE,0))
      {}

      TLogFile::TWriteLogThread::~TWriteLogThread()
      {
      }

      int   __fastcall TLogFile::TWriteLogThread::Execute()
      {
       TSynchroSet ss;
       ss+=WriteEvent;
       ss+=TerminateEvent;
       DWORD WaitRes;
       while(!TerminateRequest)
       {
        WaitRes = ss.Wait(INFINITE,false);
        if(WaitRes == WAIT_OBJECT_0  )
         {
           if(log_file->Locker.Lock(100))
           {
             ResetEvent(WriteEvent);
             #ifdef __BORLANDC__
             while(!log_file->LogQueue->empty())
             {
              TLogEntry le;
              le = log_file->LogQueue->at(0);
              log_file->LogQueue->erase(log_file->LogQueue->begin());
              log_file->Locker.Unlock();
              log_file->WriteEntry(le);
              le.Release();
             }
             #else
              TLogEntry * le = log_file->First;
              while(le)
              {
               log_file->WriteEntry(*le);
               log_file->RemoveFirst();
               le = log_file->First;
              }
             #endif

           }
         }
       }

       return 0;
      }

      void  __fastcall TLogFile::TWriteLogThread::Terminate()
      {
       TerminateRequest = true;
       SetEvent(TerminateEvent);
      }

      void  __fastcall TLogFile::TWriteLogThread::SignalWrite()
      {
       SetEvent(WriteEvent);
      }


}//end of namespace KeRTL;

