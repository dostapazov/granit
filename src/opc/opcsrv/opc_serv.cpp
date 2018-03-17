#include <vcl.h>
#include <windows.h>

#pragma hdrstop
#pragma argsused

#include <tchar.h>
#include <stdio.h>
#include "opc_config_form.h"
#include <string.h>
#include <iostream>
#include <iomanip>
#include "opc_server_imp.h"
#include  <conio.h>


int  run_as_opc()
{
  unilog *log;                    /* logging entry */
//  log = unilog_Create("LOPC-exe", "|LOPC-exe", "%!T", -1,       /* Max filesize: -1 unlimited, -2 -don't change */
//                      ll_MESSAGE);        /* level [ll_FATAL...ll_DEBUG] */
//  unilog_Redirect("LOPC-exe", "LightOPC", 0);
//  unilog_Delete(log);
//  log = unilog_Create("Lopc-Sample-exe", "|Lopc-Sample-exe", "", -1,    /* Max filesize: -1 unlimited, -2 -don't change */
//                      ll_TRACE);        /* level [ll_FATAL...ll_DEBUG] */
//  UL_DEBUG((LOGID, "run as opc invoked..."));


    std::cout<<"Start in server mode"<<std::endl;
    //TOpcServerImp opc_server;
    //opc_server.Start(0,false);
    TGKOpcDriver & opcd = *new TGKOpcDriver;
    DWORD com_objid = 0;
    int terminated = 0;
    if(S_OK == opcd.service_create(1000,loDF_NOCOMP,250,1000,100,0))
      {

       GUID guid;
       IIDFromString(OPC_GUID,&guid);
       HRESULT res =CoRegisterClassObject(guid
                                        ,&opcd
                                        ,CLSCTX_LOCAL_SERVER |  CLSCTX_REMOTE_SERVER | CLSCTX_INPROC_SERVER, REGCLS_MULTIPLEUSE, &com_objid);
       if(SUCCEEDED(res))
          {
           opcd.LockServer(TRUE);
           opcd.AddRef();
          }


      }
      else
      terminated = 1;

    do{


      if(kbhit())
         terminated = getch() == 27;
       else
       Sleep(100);

      }while(!terminated);
    //opc_server.TerminateAndWait(5000);
    if(com_objid)
       CoRevokeClassObject(com_objid);
    delete & opcd;
    return 0;
}


void __fastcall show_console(bool show)
{
  HWND cwnd = GetConsoleWindow();
  ShowWindow (cwnd, show ? SW_SHOW : SW_HIDE);
}

int _tmain(int argc, _TCHAR* argv[])
{

    std::cout<<"GRKERNEL OPC server v"<<int(OPC_VER_MAJOR)<<"."<<int(OPC_VER_MINOR)<<std::endl
    <<"Ostapenko D.V. 2015"<<std::endl
    <<"Based on lightopc library"<<std::endl;

     HRESULT res = CoInitializeEx(NULL,COINIT_MULTITHREADED);
     if( res == S_OK || res ==  RPC_E_CHANGED_MODE)
     {

     wchar_t * cmd_line = GetCommandLineW();
     if(cmd_line && wcsstr(cmd_line,CMD_RUN_OPC))
     {
       run_as_opc();
     }
     else
     {
        lightopc::TLightOpcModule mod;
        mod.Load("lightopc.dll");
        GUID guid;
        IIDFromString(OPC_GUID,&guid);
        AnsiString s = OPC_PROG_ID;
        mod.loServerUnregister(&guid,s.c_str());
        return 0;
        std::cout<<"Start in configure mode"<<std::endl;
        show_console(false);
        TConfigForm * cfg_frm = new TConfigForm(NULL);
        cfg_frm->ShowModal();
        delete cfg_frm;
      }
       CoUninitialize();
     }
     else
     std::cout<<"Error initialize COM subsistem"<<std::endl;
     std::cout<<"shutdown"<<std::endl;
	return 0;
}
