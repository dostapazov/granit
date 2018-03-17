//---------------------------------------------------------------------------

#include <windows.h>
#pragma hdrstop

#include "ReporterEx_frm.h"

#pragma argsused
//---------------------------------------------------------------------------
wnd_module_starter<TReporterExFrm> wms(MT_REPORTER|MT_WINDOW);
LRESULT WINAPI module_main(DWORD cmd,LPARAM p1,LPARAM p2)
{
 return wms(cmd,p1,p2);
}

