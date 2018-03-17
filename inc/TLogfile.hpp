//Интерфейс класса реализующий текстовый лог.
//Остапенко Д.В. АСДУ April 2000
#ifndef __TLOGFILE_HPP_INC__
 #define __TLOGFILE_HPP_INC__

 #include <vector>
 #include <KeRtlio.hpp>
 #include <kethreads.hpp>
 
 using namespace std;


namespace KeRTL
{
      class TLogEntry
      {
	   public:
	   SYSTEMTIME s_time;
       char * Header;
       char * Text;
              TLogEntry();
              TLogEntry(const char * Header,const char * text);
			  TLogEntry(const TLogEntry & le){*this = le;}
       void __fastcall Release();
       TLogEntry & operator = (const TLogEntry & le);
       TLogEntry * Next;
      };

      #ifdef __BORLANDC__
      typedef vector<TLogEntry> TLogQueue;
      #endif

  KERTL_CLASS TLogFile
  {

   class  TWriteLogThread:public TThread
    {
      HANDLE   TerminateEvent;
      HANDLE   WriteEvent;
      TLogFile * log_file;
      public:
             TWriteLogThread (TLogFile* file);
            ~TWriteLogThread();
      int   __fastcall Execute();
      void  __fastcall Terminate();
      void  __fastcall SignalWrite();
    };
    TSemLocker   Locker;
    TFile      * File;
    DWORD        LimitSize;
    #ifdef __BORLANDC__
    TLogQueue  * LogQueue;
    #else
    //Описатель очереди
    TLogEntry  * First;
    TLogEntry  * Last;
    bool   __fastcall RemoveFirst();
    #endif
    TWriteLogThread   * wrlg_thread;
    bool __fastcall  WriteEntry(TLogEntry & e);
    void  __fastcall FlushLogQueue();
    public:
           TLogFile();
           ~TLogFile();

    bool   __fastcall OpenLog(LPCSTR LogFile,DWORD LimitSize = DWORD(-1),bool Threading = false);
    bool   __fastcall CloseLog();
    bool   __fastcall WriteToLog(LPCSTR Header,LPCSTR Text);
    friend class TWriteLogThread;

  };


inline    TLogFile::TLogFile():wrlg_thread(0),File(0)
#ifdef __BORLANDC__
,LogQueue(0)
#else
,First(0),
Last(0)
#endif

 {}

inline    TLogFile::~TLogFile()
 {  CloseLog(); }

inline  TLogEntry::TLogEntry()
        {ZeroMemory(this,sizeof(*this));}

inline void __fastcall TLogEntry::Release()
{
    if(Header)
     delete [] Header;
     Header = 0;
}

inline    TLogEntry & TLogEntry::operator = (const TLogEntry & le)
   {
    Release();
    CopyMemory(this,&le,sizeof(*this));
    Next = 0;
    return *this;
   }


}// End of namespace



#endif


