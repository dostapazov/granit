//---------------------------------------------------------------------------
#pragma hdrstop
#include "mediumdb_frm.h"

wnd_module_starter<TMediumDBFrm> wms(MT_WINDOW|MT_MEDIUMDB);

LRESULT WINAPI module_main(DWORD cmd,LPARAM p1,LPARAM p2)
{
  return wms(cmd,p1,p2);
}

