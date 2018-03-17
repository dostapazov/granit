/*-----------------------------------------------------------------------------
 Unit Name: gkreporter
 Author:    Dimka
 Purpose:   Регистратор сообщений об ошибках
 History:
-----------------------------------------------------------------------------*/



#ifndef GKREPORTER_HPP_
#define GKREPORTER_HPP_

#include <gkmodules.hpp>
#include <gkreporter.h>
#include <kertlio.hpp>

using namespace KeRTL;

#define REPFL_DATE     1
#define REPFL_TIME     2
#define REPFL_NEWLINES 4

class TGKStdReporter:public TGKModule
{
  WORD    __fastcall get_type(){return MT_REPORTER;}
protected:
  TFile   report_log;
  TFile   report_exception;
  void    __fastcall release(LPVOID);
  LRESULT __fastcall processing(DWORD cmd,LPARAM p1,LPARAM p2);
  DWORD   __fastcall start(DWORD reason,LPARAM p2);
  DWORD   __fastcall stop (DWORD reason);
  virtual void __fastcall write_strings    (HANDLE fout,DWORD flags,char * str1,char * str2);
  virtual void __fastcall write_struct     (HANDLE fout,DWORD flags,TReportMes *mes,LPARAM p2);
  virtual void __fastcall write_exception  (DWORD flags,LPEXCEPTION_POINTERS,char * text);
  virtual void __fastcall write_gkexception(lpexcept_record er,char * text);
  virtual void __fastcall open_files(char * nt);
  DWORD    __fastcall get_module_info(LPMODULE_INFO mi);
  virtual UINT __fastcall register_event(char *event, char *event_name=0){return 0;};
  DWORD   __fastcall get_config_data(DWORD mask,LPVOID buf,DWORD bsz){return GKH_RET_SUCCESS;};
  bool    __fastcall set_config_data(DWORD mask,LPVOID buf,DWORD bsz){return true;};
  bool    __fastcall compare_config_data(DWORD mask,LPVOID buf,DWORD bsz,LPDWORD ch_mask,BOOL * restart)
  {
   if(ch_mask) *ch_mask = 0; if(restart) * restart = FALSE;
   return false;
  };
  BOOL    __fastcall can_start(DWORD reason,LPARAM p2){return TRUE;};
public:
  TGKStdReporter(bool external = false);
};

#endif

