#ifndef _TLM_VIEWER_MODULE
#define _TLM_VIEWER_MODULE

#include <gkmodules.hpp>
#include <modemmer.h>
#include <otdm_proto.h>
#include "tlm_viewer_mainform.h"


#define MT_TLM_VIEWER (MT_USER+1)

class tlm_viewer_mod:public TGKModule
{
 protected:
 TTlmViewerMainForm *     main_form;
 TGKModuleInterface       modemmer;
 WORD    __fastcall get_type(){return MT_TLM_VIEWER ;}
 void    __fastcall release (LPVOID p);

 DWORD   __fastcall get_config_data(DWORD mask,LPVOID buf,DWORD bsz);
 bool    __fastcall set_config_data(DWORD mask,LPVOID buf,DWORD bsz);
 bool    __fastcall compare_config_data(DWORD mask,LPVOID buf,DWORD bsz,LPDWORD ch_mask,BOOL * restart);
 DWORD   __fastcall start(DWORD reason,LPARAM p2);
 DWORD   __fastcall stop (DWORD reason);
 LRESULT __fastcall processing    (DWORD cmd,LPARAM p1,LPARAM p2);
 LRESULT __fastcall handle_data(LPMPROTO_HEADER mph,GKHANDLE h);

 public :
 tlm_viewer_mod();
 DWORD __fastcall send_command(bool begin,bool finish,DWORD cmd,DWORD err,LPVOID data,DWORD data_size );
 
};

#endif
 