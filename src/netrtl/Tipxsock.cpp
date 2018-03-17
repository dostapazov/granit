
#include <wsockdef.h>



bool __fastcall GetLocalIPXAddress(SOCKADDR_IPX & adr,int AdapterNum )
{
 SOCKADDR_IPX ipxaddr;
 memset(&ipxaddr,0,sizeof(SOCKADDR_IPX));
 memset(&adr,0,sizeof(SOCKADDR_IPX));

 adr.sa_family = ipxaddr.sa_family = AF_IPX;
 SOCKET s = socket(AF_IPX,SOCK_DGRAM,NSPROTO_IPX);
 if(s!=(SOCKET)SOCKET_ERROR)
  {
   int cb = sizeof(IPX_ADDRESS_DATA);
   IPX_ADDRESS_DATA AddrData;
   memset(&AddrData,0,cb);

   bind(s,(SOCKADDR*)&ipxaddr,sizeof(adr));

   cb = sizeof(int);
   if(0==getsockopt(s,NSPROTO_IPX,IPX_MAX_ADAPTER_NUM,(char*)&AddrData.adapternum,&cb))
   {
   if(AddrData.adapternum <=AdapterNum)
      AdapterNum = 0;//Любая доступная карта
      AddrData.adapternum = AdapterNum;
      
   cb = sizeof(IPX_ADDRESS_DATA);
   if(getsockopt(s,NSPROTO_IPX,IPX_ADDRESS,(char*)&AddrData,&cb)==0)
     {
       memcpy(adr.sa_netnum,AddrData.netnum,sizeof(adr.sa_netnum));
	   memcpy(adr.sa_nodenum,AddrData.nodenum,sizeof(adr.sa_nodenum));
       closesocket(s);
       return true;
    }
   }
    closesocket(s);
  }   
  return false;
}



  bool __fastcall TIpxSocket::CreateIpxSocket(WORD SocketNumber,DWORD PktType ,DWORD flags,LPWSAPROTOCOL_INFO lpi )
  {
    if(CreateSocket( AF_IPX,SOCK_DGRAM,DWORD(NSPROTO_IPX+PktType),flags,0,lpi))
    {
    EnableReuseAddress(true);
    //if(GetLocalIPXAddress(LocalAddress,0))
    {
    //GetSocketName((sockaddr*)&LocalAddress);
    LocalAddress.sa_socket = WORD(htons(SocketNumber));
    for(int i = 1;i<1024;i++)
        {
          if(Bind((sockaddr*)&LocalAddress,sizeof(LocalAddress)))
             {
              GetSocketName((sockaddr*)&LocalAddress);
              GetLastError();
              break;
             }
             else
             LocalAddress.sa_socket = WORD(htons(SocketNumber+WORD(i)));
        }
      return true;
     }
    }
    return false;
  }

  bool __fastcall TIpxSocket::CreateSpxSocket(WORD SocketNumber,DWORD flags,LPWSAPROTOCOL_INFO lpi )
  {
    if(CreateSocket(AF_IPX,SOCK_STREAM,DWORD(NSPROTO_SPX),flags,0,lpi))
    {
    //if(GetLocalIPXAddress(LocalAddress ,0))
    {
    GetSocketName((sockaddr*)&LocalAddress);
    LocalAddress.sa_socket = WORD(htons(SocketNumber));
    for(int i = 1;i<1024;i++)
        {
          if(Bind((sockaddr*)&LocalAddress,sizeof(LocalAddress)))
             {
              GetSocketName((sockaddr*)&LocalAddress);
              this->GetLastError();
              break;
             }
             else
             LocalAddress.sa_socket = WORD(htons(SocketNumber+WORD(i)));
        }
      return true;
     }
    }
    return false;
  }


  int  __fastcall TIpxSocket::Send(void * buf,int len,LPWSAOVERLAPPED ovrlp)
  {
    DWORD  send_bytes;
    WSABUF wsabuf;
    wsabuf.len   = len;
    wsabuf.buf   =(char*) buf;
    return WSASendTo(&wsabuf,1,&send_bytes,0/*MSG_DONTROUTE*/,(sockaddr*)&RemoteAddress,sizeof(RemoteAddress),ovrlp);
  }

  int  __fastcall TIpxSocket::SendBroadcast(WORD ipxSocket,void * buf,int len,LPWSAOVERLAPPED ovrlp)
  {
    TIpxAddress BroadcastAddress(ipxSocket,true);
    DWORD  send_bytes;
    WSABUF wsabuf;
    wsabuf.len   = len;
    wsabuf.buf   =(char*) buf;
    return WSASendTo(&wsabuf,1,&send_bytes,0/*MSG_DONTROUTE*/,(sockaddr*)&BroadcastAddress,sizeof(BroadcastAddress),ovrlp);
  }

  #ifdef __BORLANDC__
  #pragma warn -par
  #endif

  int  __fastcall TIpxSocket::Receive(void * buf,int len,LPWSAOVERLAPPED ovrlp)
  {
    int ret = 0;
    GetSocketError();
    if(ovrlp || CanRead())
     {
      static int flen = sizeof(RemoteAddress);
      WSABUF wsabuf;
      wsabuf.len  = len;
      wsabuf.buf  = (char*)buf;
      static DWORD flags = 0;
      static DWORD rd_bytes;
      if(WSARecvFrom(&wsabuf,1,&rd_bytes,&flags,(sockaddr*)&RemoteAddress,&flen,ovrlp,NULL))
         ret = rd_bytes;
     }
     return ret;
  }
  #ifdef __BORLANDC__
  #pragma warn .par
  #endif


