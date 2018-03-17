#include "processmdm.hpp"


LRESULT WINAPI module_main(DWORD cmd ,LPARAM,LPARAM)
{
       LRESULT ret = NULL;

       switch(cmd)
       {
        case GKME_CMD_GET_MODULETYPE: ret = MT_MODEM;break;
        case GKME_CMD_CREATE_HANDLE :
        {
           {
            TProcessModem * modem = new TProcessModem;
            ret  = (LRESULT)(*modem)();
            if(!ret)
             delete modem;
           }
        }
        break;
       }

       return ret;

}

 
