#ifndef _GRNET32WRAPPER_HPP
#define _GRNET32WRAPPER_HPP

#ifndef STRICT
#define STRICT
#endif

#if __BORLANDC__>= 0x0560
#include <grnet32.h>
#include <KeRTL.hpp>
#else
#include <KeRTL.hpp>
#include <grnet32.h>
#endif

#include <dll_procs.hpp>

using namespace KeRTL;

/*Враппер  GRNET32.DLL если я правильно понимаю значение слова "wrapper"*/
class TGRNET32DLL
{
 public:
 static dll_proc3<LPCSTR,WORD,int,GRNET_OBJ>                        GRNET_CreateGranServer;
 static dll_proc5<LPCSTR,WORD,LPCSTR,WORD,int,GRNET_OBJ>            GRNET_CreateGranClient;
 static dll_proc1<GRNET_OBJ,BOOL>                                   GRNET_DestroyObj;
 static dll_proc7<GRNET_OBJ,DWORD ,WORD,LPVOID,WORD,BOOL,BOOL,BOOL> GRNET_Send;
 static dll_proc7<GRNET_OBJ,LPDWORD,LPDWORD,LPVOID,DWORD,LPDWORD,LPDWORD,BOOL> GRNET_Recv;
 static dll_proc3<GRNET_OBJ,LPCSTR,WORD ,BOOL>                      GRNET_Start;
 static dll_proc1<GRNET_OBJ,BOOL>                                   GRNET_Stop;
 static dll_proc1<GRNET_OBJ,HANDLE>                                 GRNET_GetRxEvent;
 static dll_proc1<GRNET_OBJ,HANDLE>                                 GRNET_GetRestartEvent;
 static dll_proc1<GRNET_OBJ,BOOL>                                   GRNET_IsClient;
 static dll_proc1<GRNET_OBJ,BOOL>                                   GRNET_IsServer;
 static dll_proc1<GRNET_OBJ,BOOL>                                   GRNET_Restart;
 static dll_proc1<GRNET_OBJ,DWORD>                                  GRNET_GetWSAError;
 static dll_proc3<GRNET_OBJ,LPSTR,DWORD,BOOL>                       GRNET_GetName;
 static dll_proc3<GRNET_OBJ,LPSTR,DWORD,BOOL>                       GRNET_GetServerName;
 static dll_proc1<GRNET_OBJ,WORD>                                   GRNET_GetIpxSocket;
// typedef   BOOL   (CALLBACK * GRNET_EnumClientsProc)(LPVOID UserData,LPCLIENT_INFO);
 static dll_proc3<GRNET_OBJ,GRNET_EnumClientsProc,LPVOID >          GRNET_EnumClients;
 static dll_proc1<GRNET_OBJ,WORD>                                   GRNET_GetServerIpxSocket;

 static dll_proc2<GRNET_OBJ,DWORD,DWORD>                            GRNET_GetWaitReceiptTimeout;
 static dll_proc2<GRNET_OBJ,DWORD,DWORD>                            GRNET_GetSendRetryCount;
 static dll_proc3<GRNET_OBJ,DWORD,DWORD,BOOL>                       GRNET_SetWaitReceiptTimeout;
 static dll_proc3<GRNET_OBJ,DWORD,DWORD,BOOL>                       GRNET_SetSendRetryCount;
 static dll_proc2<GRNET_OBJ,BOOL,BOOL>                              GRNET_EnableReconnect  ;
 static dll_proc1<GRNET_OBJ,BOOL>                                   GRNET_DisconnectAllClients;

 TGRNET32DLL(){};
 bool        __fastcall load  ();
 void        __fastcall unload();
 bool        __fastcall is_loaded(){return hLib ? true:false;}
 protected:
 static LONG      usage;
 static HINSTANCE hLib;
 void __fastcall AfterLoad    ();
 void __fastcall BeforeUnload ();
};



#endif

