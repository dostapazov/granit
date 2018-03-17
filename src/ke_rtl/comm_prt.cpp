/*Реализация COM - порта     */
/* Остапенко Д.В. АСДУ Норильскэнерго 2001*/


#include <Kertlio.hpp>
#include <stdlib.h>


namespace KeRTL
{

   bool __fastcall TCommPort::OpenPort(int number,bool overlapped)
   {
    char p_name[32];
    lstrcpy(p_name,"COM");
    itoa(number,p_name+3,10);
	DWORD flags=0;
	if(overlapped)
		flags = FILE_FLAG_OVERLAPPED;
    Handle = CreateFileA(p_name,GENERIC_WRITE|GENERIC_READ,0,0,OPEN_ALWAYS,
                         flags,0);
    DWORD err;
    COMSTAT cs;
    ClearError(err,cs);
    return IsValid();
   }

   void __fastcall TCommPort::Close()
   {
    Purge(PURGE_TXABORT|PURGE_TXCLEAR|PURGE_RXABORT|PURGE_RXCLEAR);
    THandleBased::Close();
   }

   int        __fastcall TCommPort::GetSpeed()
   {
    DCB dcb;
    GetDCB(dcb);
    return int(dcb.BaudRate);
   }

   int        __fastcall TCommPort::SetSpeed(int speed)
   {
    DCB dcb;
    GetDCB(dcb);
    dcb.BaudRate = speed;
    return int(SetDCB(dcb));
   }

bool       __fastcall TCommPort::SetTxBuffer(int bytes)
{
 COMMPROP prop;
 if(GetProp(prop))
  return   SetupComm(Handle,prop.dwCurrentRxQueue,bytes) ? true:false;
  return false;
}

bool       __fastcall TCommPort::SetRxBuffer(int bytes)
{
 COMMPROP prop;
 if(GetProp(prop))
  return   SetupComm(Handle,bytes,prop.dwCurrentTxQueue) ? true:false;
  return false;
}

}
