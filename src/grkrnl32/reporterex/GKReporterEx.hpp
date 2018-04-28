//---------------------------------------------------------------------------

#ifndef GKReporterExHPP
#define GKReporterExHPP

#include <gkreporter.hpp>
#include <GKReporterEx.h>
#include <map>
#include <string>
#include <system.hpp>
//---------------------------------------------------------------------------

using namespace KeRTL;

class TGKReporterEx: public TGKStdReporter
{
        LARGE_INTEGER tag;
        typedef vector<TFile *> Tlog_files;
        Tlog_files log_files;
        vector<LPVOID> log_funcs;       //файлы логов
        vector<string> reg_events;      //краткое название лога - имя файла
        vector<string> event_names;     //полное название лога
        BOOL sound;                     //выдавать звул при ошибке
protected:
        void  __fastcall close_all_logs();
        LRESULT __fastcall processing(DWORD cmd,LPARAM p1,LPARAM p2);
        DWORD __fastcall start(DWORD reason,LPARAM p2);
        DWORD __fastcall stop (DWORD reason);
        void __fastcall write_strings    (HANDLE fout,DWORD flags,char * str1,char * str2);
        void __fastcall write_struct     (HANDLE fout,DWORD flags,TReportMes *mes,LPARAM p2);
        void __fastcall write_exception  (DWORD flags,LPEXCEPTION_POINTERS,char * text);
        void __fastcall write_gkexception(lpexcept_record er,char * text);
        void __fastcall open_files(char * nt);
        DWORD __fastcall get_module_info(LPMODULE_INFO mi);
KeRTL::TFile *  __fastcall open_event_file(char *event,bool & bad_file);
        UINT __fastcall register_event(char event[MAX_PATH], char event_name[MAX_PATH]=0);
        void FillBuffer(byte *buf, DWORD length, TMess *event, ...);
        bool __fastcall read_settings();
        bool __fastcall write_settings();
virtual DWORD __fastcall license_inc_time(DWORD v){return -1;}
virtual DWORD   __fastcall check_license(){return MOD_LICENSE_REGISTERED; }
public:
        TGKReporterEx();
        ~TGKReporterEx();
        DWORD __fastcall get_window_module(wchar_t *buf, DWORD bsz);

private:
        bool __fastcall CheckLogFile(TFile *log, char *file_name);
        void __fastcall WriteOldLog();
        void __fastcall WriteOldErr();
        void __fastcall WriteToFile(UINT log_name, byte * buf, DWORD sz);
        DWORD __fastcall ClearEvent(UINT i);
        DWORD __fastcall GetMessageNameA(UINT event, TEventNameA * names);
        DWORD __fastcall GetMessageNameW(UINT event, TEventNameW * names);
        DWORD __fastcall SetRegisterFunc(UINT i, LPVOID func);
        LPVOID __fastcall GetRegisterFunc(UINT i, char * event_name);
        void __fastcall report(DWORD cmd,LPARAM p1,LPARAM p2);
        void __fastcall report(UINT log,int type,char * desc);
        void __fastcall report(UINT log,int type,wchar_t * desc);
        bool __fastcall ConvertVersion(DWORD version);
        DWORD __fastcall ChangeFileSize(UINT num, DWORD size);
};

inline void    __fastcall TGKReporterEx::report(DWORD cmd,LPARAM p1,LPARAM p2)
{
 processing(cmd,p1,p2);
}


int WINAPI ExceptionToStr(LPVOID Data, char *str, DWORD sz, int Code);
//---------------------------------------------------------------------------
#endif
