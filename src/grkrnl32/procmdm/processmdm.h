/*
 Параметры настойки модема обеспечивающего связь с другим процессом
*/

#ifndef _PROCESSMDM_INC
#define _PROCESSMDM_INC
#include <modemmer.h>

#ifndef RC_INVOKED

#pragma pack(push,1)
typedef struct _PMSETTINGS
{
 DWORD   dw_size;
 DWORD   modem_number;
 wchar_t proc_name[MAX_PATH<<1];
 wchar_t cmd_line [MAX_PATH<<1];
 wchar_t work_dir [MAX_PATH<<1];
 int     cmd_show;
 BOOL    auto_restart;
}PMSETTINGS,*LPPMSETTINGS;

#define CCFL_PROCNAME    0x08000000
#define CCFL_CMDLINE     0x04000000
#define CCFL_WORKDIR     0x02000000
#define CCFL_CMDSHOW     0x01000000
#define CCFL_AUTORESTART 0x00800000

#define PROCMDM_REGSTR_PROCNAME    L"ProcName"
#define PROCMDM_REGSTR_CMDLINE     L"CmdLine"
#define PROCMDM_REGSTR_WORKDIR     L"WorkDir"
#define PROCMDM_REGDW_CMDSHOW      L"CmdShow"
#define PROCMDM_REGDW_AUTORESTART  L"CmdShow"

#pragma pack(pop)

#endif // RC_INVOKED

#endif

