//---------------------------------------------------------------------------
#pragma hdrstop

#include <wsockdef.h>

//---------------------------------------------------------------------------
#ifdef __BORLANDC__
#pragma link "ws2_32.lib"
#endif

    int TBaseSocket::UsageCount=0;
    WSAData TBaseSocket::wsadata = {0};
    bool TBaseSocket::UnderNT = bool(GetVersion()<0x80000000);

    bool __fastcall TBaseSocket::InitWS(WORD Version)     //Инициализация
    {
       if(WSAStartup(Version,&wsadata))
           return false;
        UsageCount++;
        return true;

    }


    void __fastcall TBaseSocket::DeinitWS()       //Деинициализация
    {
     UsageCount--;
     if(0==UsageCount)
        WSACleanup();
    }


 TBaseSocket::TBaseSocket(SOCKET s)
 {
   EventMask = 0;
  if(UsageCount)
     UsageCount++;
     else
     s = SOCKET_ERROR;
  Socket = s;
  if(Socket!=(SOCKET)SOCKET_ERROR)
  {
   GetOptions();
   int sz = sizeof(int);
   getsockopt(Socket,SOL_SOCKET,SO_TYPE,(char*)&fType,&sz);
  }
 }

 bool __fastcall TBaseSocket::GetProtocolInfo(WSAPROTOCOL_INFO & pinfo)
 {
     int ProtocolQuery[2];
     DWORD cb = sizeof(pinfo);
     ProtocolQuery[0] = fProtocol,ProtocolQuery[1]=0;
     return bool(WSAEnumProtocols(ProtocolQuery,&pinfo,&cb)!=SOCKET_ERROR);

 }
 bool __fastcall TBaseSocket::DestroySocket()
 {
  fLastError = 0;
  if(Socket!=(SOCKET)SOCKET_ERROR)
  {
  if(shutdown(Socket,SD_BOTH))
    fLastError = WSAGetLastError();
    closesocket(Socket);
    Socket    = 0;
  }
   return bool(fLastError == 0);
 }

   int __fastcall TBaseSocket::GetMaxMsgSize()
   {
    int tInt = 0;
    WSAPROTOCOL_INFO info;
    if(GetProtocolInfo(info))
        tInt = info.dwMessageSize;
    else
    {
     int optsz = sizeof(tInt);
     getsockopt(Socket,SOL_SOCKET,SO_MAX_MSG_SIZE,(char*)&tInt,&optsz);
    }
    return tInt;
   }



