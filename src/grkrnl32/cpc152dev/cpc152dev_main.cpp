#pragma hdrstop
#pragma argsused
#include "cpc152dev.hpp"

/*extern "C" int _libmain(unsigned long reason)
{
	return 1;
}
*/

#pragma warn -8057

LRESULT WINAPI module_main(DWORD cmd,LPARAM p1,LPARAM p2)
{
  static module_starter<Tcpc152controller> ms(MT_MODEM_CPC152);
  if(cmd==GKME_CMD_UNLOAD_MODULE) return 1;
     return ms(cmd,p1,p2);
}
#pragma warn .8057
