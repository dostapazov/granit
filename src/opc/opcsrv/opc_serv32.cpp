

#pragma hdrstop
#pragma argsused
#include <lightopc.hpp>
#include "opc_srv_service.h"
#include <iostream>
//#include <map>


#ifdef _WIN32
#include <tchar.h>

#else
  typedef char _TCHAR;
  #define _tmain main
#endif


#include <stdio.h>

#include "opc_serv32_consts.h"
#include <string>
#include <dll_procs.hpp>



using namespace lightopc;

typedef std::basic_string<char> ansi_string;

ansi_string wstr2str(wchar_t * str)
{
  ansi_string res;
  if(str && *str)
  {
   char * _tmp = new char[wcslen(str)+1];
   KeRTL::Unicode2Ansi(_tmp,str);
   res = _tmp;
   delete [] _tmp;
  }
  return res;
}

std::wstring str2wstr(char * str)
{
  std::wstring res;
  if(str && *str)
  {
   wchar_t * _tmp = new wchar_t[strlen(str)+1];
   KeRTL::Ansi2Unicode(_tmp,str);
   res = _tmp;
   delete [] _tmp;
  }
  return res;
}


int __fastcall register_server(GUID & guid)
{

  char exe_name[8192];
  int len = GetModuleFileName(GetModuleHandle(0),exe_name,sizeof(exe_name));
  sprintf(exe_name+len," %s",CMD_RUN_OPC);
  return loServerRegister(&guid,OPC_PROG_ID,"OPC-server TCP-client GRKERNEL",exe_name,NULL);
}

int __fastcall unregister_server(GUID & guid)
{
  return loServerUnregister(&guid,OPC_PROG_ID);
}

int __fastcall config_server(GUID & guid)
{
  FreeConsole();
  KeRTL::TModule mod;
  mod.Load("opc_serv32_params.dll");
  if(mod.IsLoaded())
  {
    dll_proc1<GUID*,int> show_config;
    show_config.assign(mod.GetInstance(),"show_config");
    return show_config(&guid);
  }
  return 0;
}


 int _tmain(int argc, _TCHAR* argv[])
{
  //char * cmd_line = GetCommandLineA();
   GUID guid;
   IIDFromString(str2wstr(OPC_GUID).c_str(),&guid);
  if(argc > 1)
  {

      if(0==stricmp(argv[1],CMD_RUN_OPC))
       {
        run_as_service(guid);
        return 0;
       }

       if(0==stricmp(argv[1],CMD_REG_OPC))
       {
         register_server(guid);
         return 0;
       }

       if( 0 == stricmp(argv[1],CMD_UNREG_OPC))
       {
         unregister_server(guid);
         return 0;
       }

       if(!argv[1] || 0 == stricmp(argv[1],CMD_CONFIG_OPC))
       {
         config_server(guid);
         return 0;
       }
   }
   else
   config_server(guid);
  return 0;
}


//int WINAPI WinMain(HINSTANCE hinst,LPCSTR cmdLine,int)
//{
//    return 0;
//}


