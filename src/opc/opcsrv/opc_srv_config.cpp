//---------------------------------------------------------------------------
#include <kertl.hpp>
#include <tregstry.hpp>

#pragma hdrstop
#include "opc_srv_config.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)


   TOpcSrvConfig::TOpcSrvConfig()
   {
    config_reg_key = new char [KERTL_ARRAY_COUNT(REG_OPC_SERVER_KEY_TMPL)+KERTL_ARRAY_COUNT(OPC_PROG_ID) ];
    wsprintf(config_reg_key,REG_OPC_SERVER_KEY_TMPL,OPC_PROG_ID);
    opc_server_name[0] = 0;
    otd_server_addr[0] = 0;
    otd_server_port = 0;
    RefreshRate     =  500;
    RefreshRateMin  =  100;
    MaxQueueCount   =  256;
    TagsCount       = 1000;
    no_clients_terminate_delay = 10;
    tn_mode = tag_long_name;
    use_console     = FALSE;
    memset(BranchSep,0,sizeof(BranchSep));
   };

   TOpcSrvConfig::~TOpcSrvConfig()
   {
     if(config_reg_key) delete []config_reg_key;
   }



   bool __fastcall TOpcSrvConfig::write()
   {
     KeRTL::TRegsWriter rwr(HKEY_LOCAL_MACHINE);
     if(rwr.OpenKey(config_reg_key) && rwr.OpenKey(REG_OPC_SERVER_CONFIG,true))
     {
       rwr.WriteString(REGSTR_OPC_SERVER_NAME,opc_server_name);
       rwr.WriteDword (REGDW_OPC_REFRESH_RATE,RefreshRate);
       rwr.WriteDword (REGDW_OPC_REFRESH_RATE_MIN,RefreshRateMin);
       rwr.WriteDword (REGDW_OPC_MAX_QUEUE_COUNT,MaxQueueCount);
       rwr.WriteDword (REGDW_OPC_BRANCH_SEP,DWORD(BranchSep[0]));
       rwr.WriteDword (REGDW_OPC_TAGS_COUNT,TagsCount);
       rwr.WriteDword (REGDW_OPC_NO_CLIENT_TERMINATE_DELAY,this->no_clients_terminate_delay);
       rwr.WriteString(REGSTR_OTD_SERVER_ADDR,otd_server_addr);
       rwr.WriteDword (REGDW_OTD_SERVER_PORT,otd_server_port);
       rwr.WriteDword (REGDW_OPC_TAG_NAME_MODE,(DWORD)tn_mode);
       rwr.WriteDword (REGDW_OPC_USE_CONSOLE,(DWORD)use_console);
       void * m_ptr =  &order_kadrs.front();
       rwr.WriteBytes(REGBIN_ORDER_KADRS,m_ptr,order_kadrs.size()*sizeof(Torder_kadrs::value_type));
       return true;
     }
     return false;
   }


   bool __fastcall TOpcSrvConfig::read ()
   {
     KeRTL::TRegsReader rrd(HKEY_LOCAL_MACHINE);
     bool can_create =
     #ifdef _DEBUG
     IsDebuggerPresent();
     #else
     false;
     #endif

     if(rrd.OpenKey(config_reg_key) )
     {
      if(rrd.OpenKey(REG_OPC_SERVER_CONFIG,can_create))
      {

       opc_server_name[rrd.ReadString(REGSTR_OPC_SERVER_NAME,opc_server_name,KERTL_ARRAY_COUNT (opc_server_name))] = 0;

       TagsCount     = rrd.ReadDword (REGDW_OPC_TAGS_COUNT,1000);
       RefreshRate   = rrd.ReadDword (REGDW_OPC_REFRESH_RATE,500);
       RefreshRateMin= rrd.ReadDword (REGDW_OPC_REFRESH_RATE_MIN,100);
       MaxQueueCount = rrd.ReadDword (REGDW_OPC_MAX_QUEUE_COUNT ,256);
       BranchSep[0]  = (char) rrd.ReadDword (REGDW_OPC_BRANCH_SEP,DWORD(0));

       no_clients_terminate_delay = rrd.ReadDword (REGDW_OPC_NO_CLIENT_TERMINATE_DELAY,10);

       otd_server_addr [rrd.ReadString(REGSTR_OTD_SERVER_ADDR,otd_server_addr,KERTL_ARRAY_COUNT(otd_server_addr))] = 0;
       otd_server_port = rrd.ReadDword(REGDW_OTD_SERVER_PORT,0);

       tn_mode     = (tag_name_mode)rrd.ReadDword(REGDW_OPC_TAG_NAME_MODE,tag_name_mode::tag_short_name);
       use_console =  rrd.ReadDword(REGDW_OPC_USE_CONSOLE,FALSE);

       DWORD vect_size = 0;
       rrd.GetValueType(REGBIN_ORDER_KADRS,NULL,&vect_size);
       order_kadrs.clear();
       int vect_count = vect_size/sizeof(Torder_kadrs::value_type);
       order_kadrs.resize(vect_count);
       order_kadrs.reserve(vect_count);
       rrd.ReadBytes(REGBIN_ORDER_KADRS,&order_kadrs.front(),vect_size);
       std::stable_sort(order_kadrs.begin(),order_kadrs.end(),std::less<int>());

       return true;
       }
     }
     else
     KeRTL::ShowErrorMessage(NULL,GetLastError());

     return false;
   }

