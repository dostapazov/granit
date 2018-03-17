//---------------------------------------------------------------------------

#ifndef opc_srv_configH
#define opc_srv_configH
//---------------------------------------------------------------------------
#include <ke_defs.h>
#include <guiddef.h>
#include <string>
#include <vector>
#include <algorithm>
#include <functional>
#include "opc_serv32_consts.h"

#pragma hdrstop



struct TOpcSrvConfig
{
   enum    tag_name_mode { tag_name_id, tag_name_otd_addr, tag_short_name, tag_long_name };
   typedef std::vector<int> Torder_kadrs;
   char          otd_server_addr[MAX_PATH];
   int           otd_server_port;
   Torder_kadrs  order_kadrs;
   char          opc_server_name [MAX_PATH];
   int           RefreshRate;
   int           RefreshRateMin;
   int           MaxQueueCount;
   char          BranchSep[2];
   int           TagsCount;
   int           no_clients_terminate_delay;
   tag_name_mode tn_mode;
   int           use_console;
   char     *   config_reg_key;

    TOpcSrvConfig();
 virtual ~TOpcSrvConfig();
           bool __fastcall write();
           bool __fastcall read();

};


#endif
