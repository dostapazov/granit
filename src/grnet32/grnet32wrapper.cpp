#include <grnet32wrapper.hpp>

LONG      TGRNET32DLL::usage = 0;
HINSTANCE TGRNET32DLL::hLib  = 0;

dll_proc3<LPCSTR,WORD,int,GRNET_OBJ>                        TGRNET32DLL::GRNET_CreateGranServer;
dll_proc5<LPCSTR,WORD,LPCSTR,WORD,int,GRNET_OBJ>            TGRNET32DLL::GRNET_CreateGranClient;
dll_proc1<GRNET_OBJ,BOOL>                                   TGRNET32DLL::GRNET_DestroyObj;
dll_proc7<GRNET_OBJ,DWORD ,WORD,LPVOID,WORD,BOOL,BOOL,BOOL> TGRNET32DLL::GRNET_Send;
dll_proc7<GRNET_OBJ,LPDWORD,LPDWORD,LPVOID,DWORD,LPDWORD,LPDWORD,BOOL> TGRNET32DLL::GRNET_Recv;
dll_proc3<GRNET_OBJ,LPCSTR,WORD ,BOOL>                      TGRNET32DLL::GRNET_Start;
dll_proc1<GRNET_OBJ,BOOL>                                   TGRNET32DLL::GRNET_Stop;
dll_proc1<GRNET_OBJ,HANDLE>                                 TGRNET32DLL::GRNET_GetRxEvent;
dll_proc1<GRNET_OBJ,HANDLE>                                 TGRNET32DLL::GRNET_GetRestartEvent;
dll_proc1<GRNET_OBJ,BOOL>                                   TGRNET32DLL::GRNET_IsClient;
dll_proc1<GRNET_OBJ,BOOL>                                   TGRNET32DLL::GRNET_IsServer;
dll_proc1<GRNET_OBJ,BOOL>                                   TGRNET32DLL::GRNET_Restart;
dll_proc1<GRNET_OBJ,DWORD>                                  TGRNET32DLL::GRNET_GetWSAError;
dll_proc3<GRNET_OBJ,LPSTR,DWORD,BOOL>                       TGRNET32DLL::GRNET_GetName;
dll_proc3<GRNET_OBJ,LPSTR,DWORD,BOOL>                       TGRNET32DLL::GRNET_GetServerName;
dll_proc1<GRNET_OBJ,WORD>                                   TGRNET32DLL::GRNET_GetIpxSocket;
dll_proc3<GRNET_OBJ,GRNET_EnumClientsProc,LPVOID >          TGRNET32DLL::GRNET_EnumClients;
dll_proc1<GRNET_OBJ,WORD>                                   TGRNET32DLL::GRNET_GetServerIpxSocket;

dll_proc2<GRNET_OBJ,DWORD,DWORD>                            TGRNET32DLL::GRNET_GetWaitReceiptTimeout;
dll_proc2<GRNET_OBJ,DWORD,DWORD>                            TGRNET32DLL::GRNET_GetSendRetryCount;
dll_proc3<GRNET_OBJ,DWORD,DWORD,BOOL>                       TGRNET32DLL::GRNET_SetWaitReceiptTimeout;
dll_proc3<GRNET_OBJ,DWORD,DWORD,BOOL>                       TGRNET32DLL::GRNET_SetSendRetryCount;
dll_proc2<GRNET_OBJ,BOOL,BOOL>                              TGRNET32DLL::GRNET_EnableReconnect  ;
dll_proc1<GRNET_OBJ,BOOL>                                   TGRNET32DLL::GRNET_DisconnectAllClients;



 void __fastcall TGRNET32DLL::AfterLoad()
 {
  //После загрузки
  GRNET_CreateGranServer.     assign(hLib,"GRNET_CreateGranServer");
  GRNET_CreateGranClient.     assign(hLib,"GRNET_CreateGranClient");
  GRNET_DestroyObj.           assign(hLib,"GRNET_DestroyObj");
  GRNET_Send.                 assign(hLib,"GRNET_Send");
  GRNET_Recv.                 assign(hLib,"GRNET_Recv");
  GRNET_Start.                assign(hLib,"GRNET_Start");
  GRNET_Stop.                 assign(hLib,"GRNET_Stop");
  GRNET_GetRxEvent.           assign(hLib,"GRNET_GetRxEvent");
  GRNET_GetRestartEvent.      assign (hLib,"GRNET_GetRestartEvent");
  GRNET_IsClient.             assign(hLib,"GRNET_IsClient");
  GRNET_IsServer.             assign(hLib,"GRNET_IsServer");
  GRNET_Restart.              assign(hLib ,"GRNET_Restart");
  GRNET_GetWSAError.          assign(hLib,"GRNET_GetWSAError");
  GRNET_GetName.              assign(hLib,"GRNET_GetName");
  GRNET_GetServerName.        assign(hLib,"GRNET_GetServerName");
  GRNET_GetIpxSocket.         assign(hLib ,"GRNET_GetIpxSocket");
  GRNET_GetServerIpxSocket.   assign(hLib,"GRNET_GetServerIpxSocket");
  GRNET_EnumClients.          assign(hLib,"GRNET_EnumClients");
  GRNET_GetWaitReceiptTimeout.assign(hLib,"GRNET_GetWaitReceiptTimeout");
  GRNET_GetSendRetryCount    .assign(hLib,"GRNET_GetSendRetryCount    ");
  GRNET_SetWaitReceiptTimeout.assign(hLib,"GRNET_SetWaitReceiptTimeout");
  GRNET_SetSendRetryCount    .assign(hLib,"GRNET_SetSendRetryCount");
  GRNET_EnableReconnect      .assign(hLib,"GRNET_EnableReconnect");
  GRNET_DisconnectAllClients .assign(hLib,"GRNET_DisconnectAllClients");
 }

 void __fastcall TGRNET32DLL::BeforeUnload()
 {
  GRNET_CreateGranServer.assign(0,0);
  GRNET_CreateGranClient.assign(0,0);
  GRNET_DestroyObj.assign(0,0);
  GRNET_Send.assign(0,0);
  GRNET_Recv.assign(0,0);
  GRNET_Start.assign(0,0);
  GRNET_Stop.assign(0,0);
  GRNET_GetRxEvent.assign(0,0);
  GRNET_GetRestartEvent.assign(0,0);
  GRNET_IsClient.assign(0,0);
  GRNET_IsServer.assign(0,0);
  GRNET_Restart.assign(0,0);
  GRNET_GetWSAError.assign(0,0);
  GRNET_GetName.assign(0,0);
  GRNET_GetServerName.assign(0,0);
  GRNET_GetIpxSocket.assign(0,0);
  GRNET_GetServerIpxSocket.assign(0,0);
  GRNET_EnumClients.assign(0,0);
  GRNET_GetWaitReceiptTimeout.assign(0,0);
  GRNET_GetSendRetryCount    .assign(0,0);
  GRNET_SetWaitReceiptTimeout.assign(0,0);
  GRNET_SetSendRetryCount    .assign(0,0);
  GRNET_EnableReconnect      .assign(0,0);
  GRNET_DisconnectAllClients .assign(0,0);
 }


bool        __fastcall TGRNET32DLL::load()
 {
  if(InterlockedIncrement(&usage)==1)
     {
      hLib = LoadLibrary("grnet32.dll");
      AfterLoad();
     }
   return hLib ? true:false;  
 }

void        __fastcall TGRNET32DLL::unload()
 {
  if(InterlockedDecrement(&usage)==0)
     {
      BeforeUnload();
      FreeLibrary(hLib);
     }
 }


