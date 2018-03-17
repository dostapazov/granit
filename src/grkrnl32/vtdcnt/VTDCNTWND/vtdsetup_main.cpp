#pragma hdrstop
#include "vtdsetup.h"


wnd_module_starter<TVTDSetupFrm> wms(MT_MODEM_TCP|MT_WINDOW);

LRESULT WINAPI module_main(DWORD cmd,LPARAM p1,LPARAM p2)
{
 return wms(cmd,p1,p2);
}

