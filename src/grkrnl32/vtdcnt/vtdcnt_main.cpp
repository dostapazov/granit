#pragma hdrstop
#include "vtdcnt.hpp"

LRESULT WINAPI module_main(DWORD cmd,LPARAM p1,LPARAM p2)
{
  LRESULT ret = 0;
       switch(cmd)
       {
        case GKME_CMD_GET_MODULETYPE:
			ret = MT_MODEM;break;
        case GKME_CMD_CREATE_HANDLE :
        {
            TVtdCntModem * mod =new TVtdCntModem;
            ret  = (LRESULT)(*(TGKHandleBased*)(mod))();
            if(!ret)
             delete mod;
        }
        break;
       }
       return ret;
}

 