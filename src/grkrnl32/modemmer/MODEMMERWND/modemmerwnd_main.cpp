//---------------------------------------------------------------------------
#pragma hdrstop
#include <vcl.h>
#include "modemmer_frm.h"

wnd_module_starter<TModemmerFrm> wms(MT_MODEM|MT_WINDOW);

LRESULT WINAPI module_main(DWORD cmd,LPARAM p1,LPARAM p2)
{
  return wms(cmd,p1,p2);
}

