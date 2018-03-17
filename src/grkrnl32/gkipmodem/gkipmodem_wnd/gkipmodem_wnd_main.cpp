#pragma hdrstop
#include "gkipmodem_frm.h"

wnd_module_starter<TGkIPModemFrm> wms(MT_MODEM_TCP|MT_WINDOW);

LRESULT WINAPI module_main(DWORD cmd,LPARAM p1,LPARAM p2)
{
  return wms(cmd,p1,p2);
}


