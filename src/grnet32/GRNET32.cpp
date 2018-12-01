/*Версия 2 GRNET232.DLL*/

#include "grnet232.hpp"
#include <kertl.hpp>

#if _MSC_VER >=1200
#ifndef _DEBUG
#pragma comment(linker,"/opt:nowin98")
//#pragma comment(linker,"/opt:ref")
#endif
#endif
 

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fwdreason, LPVOID lpvReserved)
{
        return 1;
}

 GRNET_OBJ WINAPI GRNET_CreateGranClient(LPCSTR SName,WORD sSocket,LPCSTR CName,WORD cSocket,int adapter_num)
 {
  //return (GRNET_OBJ)new TGrNetBase(sSocket,SName);
  return (GRNET_OBJ)new TGrNetClient(SName,sSocket,CName,cSocket);
 }

 BOOL      WINAPI GRNET_DestroyObj (GRNET_OBJ obj)
 {
  TGrNetBase * base = TGrNetBase::CAST(obj);
  if(base && GRNET_Stop(obj))
    {
     //KeRTL::TKeRTLBase::FreeObject(*base);
	 base->Release();
	 delete base;
     return TRUE;
    }
    return FALSE;
 }


 BOOL      WINAPI GRNET_Start(GRNET_OBJ obj,LPCSTR sName,WORD socket)
 {
  TGrNetBase * base = TGrNetBase::CAST(obj);
  if(base)
    {
     if(sName)
       base->SetName(sName);
     if(socket)
       base->SetSocketNumber(socket);
     
     base->Start();
     return TRUE;
    }
    return FALSE;
 }

 BOOL      WINAPI GRNET_Stop(GRNET_OBJ obj)
 {
  TGrNetBase * base = TGrNetBase::CAST(obj);
  if(base)
    {
     #ifdef _DEBUG 
     base->TerminateAndWait(base->GetTerminateTime()*10,true);
     #else
     base->TerminateAndWait(base->GetTerminateTime(),true);
     #endif
     return TRUE;
    }
    return FALSE;

 }

 DWORD    WINAPI GRNET_GetWaitReceiptTimeout(GRNET_OBJ obj, DWORD ClId)
 {
  TGrNetBase * base = TGrNetBase::CAST(obj);
  if(base)
    {
     return base->GetWaitReceiptTimeout(ClId);
    }
    return 0;
 }

 BOOL     WINAPI GRNET_SetWaitReceiptTimeout(GRNET_OBJ obj, DWORD ClId, DWORD to)
 {
  TGrNetBase * base = TGrNetBase::CAST(obj);
  if(base && to>10 && to<1000)
    {
     base->SetWaitReceiptTimeout(ClId,to);
     return TRUE;
    }
    return FALSE;
 }

 DWORD    WINAPI GRNET_GetSendRetryCount(GRNET_OBJ obj, DWORD ClId)
 {
  TGrNetBase * base = TGrNetBase::CAST(obj);
  if(base)
    {
     return base->GetRetryCount(ClId);
    }
    return 0;
 }

 BOOL     WINAPI GRNET_SetSendRetryCount(GRNET_OBJ obj, DWORD ClId, DWORD Count)
 {
  TGrNetBase * base = TGrNetBase::CAST(obj);
  if(base && Count >1 && Count < 32)
    {
     base->SetRetryCount(ClId,Count);
     return TRUE;
    }
   return FALSE;
 }

#ifndef __BORLANDC__
#pragma warning(disable:4800)
#endif

 BOOL      WINAPI GRNET_Send(GRNET_OBJ obj,DWORD ClientId,WORD Command,LPVOID data,WORD length,BOOL NeedReceipt,BOOL First)
 {
  BOOL ret(FALSE);
  TGrNetBase * base = TGrNetBase::CAST(obj);
  if(base )
    {
     ret = base->SendTo(ClientId,Command,data,length,NeedReceipt,First) ? true:false ;
    }
   return ret;
 }

 BOOL      WINAPI GRNET_Recv(GRNET_OBJ obj,LPDWORD From,LPDWORD Command,LPVOID data,DWORD cbSize,LPDWORD rdBytes,LPDWORD Remain)
 {
  BOOL ret(FALSE);
  TGrNetBase * base = TGrNetBase::CAST(obj);
  if(base )
    {
     ret = BOOL(base->Read(From,Command,data,cbSize,rdBytes,Remain ));
    }
   return ret;
 }


 BOOL      WINAPI GRNET_IsClient(GRNET_OBJ obj)
 {
  BOOL ret(FALSE);
  TGrNetBase * base = TGrNetBase::CAST(obj);

   if(base && dynamic_cast<TGrNetClient*>(base))
     ret = TRUE;

  return ret;
 }
 BOOL      WINAPI GRNET_IsServer(GRNET_OBJ obj)
 {
  BOOL ret(FALSE);
  TGrNetBase * base = TGrNetBase::CAST(obj);

   if(base && dynamic_cast<TGrNetServer*>(base))
     ret = TRUE;

  return ret;
 }

 HANDLE    WINAPI GRNET_GetRxEvent(GRNET_OBJ obj)
 {
   TGrNetBase * base = TGrNetBase::CAST(obj);
   if(base)
     return base->GetRxEvent();
   return INVALID_HANDLE_VALUE;
 }

 BOOL      WINAPI GRNET_GetName(GRNET_OBJ obj,LPSTR out,DWORD cbSize)
 {
   TGrNetBase * base = TGrNetBase::CAST(obj);
   if(base)
   {
    base->GetName(out,cbSize);
    return TRUE;
   }
   return FALSE;
 }

 WORD      WINAPI GRNET_GetIpxSocket(GRNET_OBJ  obj)
 {
   TGrNetBase * base = TGrNetBase::CAST(obj);
   if(base)
   {
    //return base->GetSocketNumber();
   }
   return 0;
 }

 BOOL      WINAPI GRNET_GetServerName     (GRNET_OBJ  obj,LPSTR out,DWORD cbSize)
 {
   TGrNetBase * base = TGrNetBase::CAST(obj);
   TGrNetClient * cl = dynamic_cast<TGrNetClient *>(base);
   if(cl)
   {
    cl->GetServerName(out,cbSize);
    return TRUE;
   }
   else
   return GRNET_GetName(obj,out,cbSize);
 }

 WORD      WINAPI GRNET_GetServerIpxSocket(GRNET_OBJ obj)
 {
   TGrNetBase * base = TGrNetBase::CAST(obj);
   TGrNetClient * cl = dynamic_cast<TGrNetClient *>(base);
   if(cl)
     {
      return cl->GetServerIpxSocket();
     }

   return GRNET_GetIpxSocket(obj);

 }

 GRNET_OBJ WINAPI GRNET_CreateGranServer(LPCSTR name,WORD Socket,int adapter_num)
 {
  //Создание сервера
  return (GRNET_OBJ) (new TGrNetServer(name,Socket));
 }

 void      WINAPI GRNET_EnumClients(GRNET_OBJ obj,GRNET_EnumClientsProc proc,LPVOID UserData)
 {
   TGrNetBase * base = TGrNetBase::CAST(obj);
   if(base)
     {
      base->EnumClients(proc,UserData);
     }

 }

 DWORD     WINAPI GRNET_GetWSAError(GRNET_OBJ obj)
 {
   TGrNetBase * base = TGrNetBase::CAST(obj);
   if(base)
     return base->GetWsaError();
   return 0;
 }

 HANDLE    WINAPI GRNET_GetRestartEvent(GRNET_OBJ obj)
 {
   TGrNetBase * base = TGrNetBase::CAST(obj);
   if(base)
     {
     return base->GetRestartEvent();
     }
    return NULL; 
 }

 BOOL      WINAPI GRNET_Restart (GRNET_OBJ)
 {
  //ПОКА Так
  return TRUE;
 }


BOOL  WINAPI GRNET_EnableReconnect(GRNET_OBJ obj,BOOL enable)
{
	TGrNetClient * client = dynamic_cast<TGrNetClient*>(TGrNetBase::CAST(obj));
	if(client)
	{
		client->lock();
		client->SetFlags(GRNET_FLAGS_ENABLE_RECONNECT,enable ? true:false);
		client->unlock();
		return TRUE;
	}

	return FALSE;
}


BOOL WINAPI GRNET_DisconnectAllClients(GRNET_OBJ obj)
{
   BOOL ret    = FALSE;
   TGrNetBase   * base = TGrNetBase::CAST(obj);
   if(base )
   {
    TGrNetServer * srv  = dynamic_cast<TGrNetServer*>(base);
    if(srv)
    {
     ret = TRUE;
     srv->DisconnectClients();
    }
   }
     return ret;
}
