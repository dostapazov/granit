#include <wsockdef.h>
#include <iphlpapi.h>

/*
определение МАС адреса по IP-адресу
*/

int __fastcall detect_mac_local (DWORD ip_addr,BYTE * mac_addr,int & msz)
{
 int ret;
 LPVOID           tmp_buff;
 PIP_ADAPTER_INFO adapt_info = NULL;
 DWORD            bsz        = 0;
 // Узнаём размер буфера
 ret = GetAdaptersInfo(adapt_info,&bsz);
 if(ret==ERROR_BUFFER_OVERFLOW)
 {
  adapt_info = PIP_ADAPTER_INFO(GlobalAlloc(GMEM_FIXED,bsz));
  tmp_buff   = adapt_info;//Сохранить указатель для последующего удаления
  ret = GetAdaptersInfo(adapt_info,&bsz);
  if(ret == ERROR_SUCCESS)
  {
    ret  =ERROR_NO_DATA;
    bool do_break = false;
    while(adapt_info)
    {
     PIP_ADDR_STRING addr_str = &adapt_info->IpAddressList;
     while(addr_str)
     {
      TIPAddress ipa(addr_str->IpAddress.String,0);
      if(ipa.sin_addr.S_un.S_addr == ip_addr)
      {
       do_break = true;
       if(msz>=int(adapt_info->AddressLength))
       {
        msz=adapt_info->AddressLength;
        memcpy(mac_addr,adapt_info->Address,msz);
        ret  =ERROR_SUCCESS;
       }
       else
       ret = ERROR_BUFFER_OVERFLOW; 
       break;
      }
      addr_str = addr_str->Next;
     }
      if(do_break)
         break;
     adapt_info = adapt_info->Next;
    }

  }
 }

 if(tmp_buff)
    GlobalFree(HGLOBAL(tmp_buff));

 return ret;
}

int __fastcall do_detect_mac_remote(DWORD ip_addr,BYTE * mac_addr,int & msz)
{
   int ret = ERROR_NO_DATA;
   char buffer[1024];
   PMIB_IPNETTABLE it = PMIB_IPNETTABLE(buffer);
   DWORD it_sz = sizeof(buffer);
   #ifdef _DEBUG_
   ZeroMemory(buffer,it_sz);
   #endif
   DWORD r_code = GetIpNetTable(it,&it_sz,FALSE);
   if(it_sz>sizeof(buffer))
   {
    it = (PMIB_IPNETTABLE)GlobalAlloc(GMEM_FIXED,it_sz);
    r_code = GetIpNetTable(it,&it_sz,TRUE);
   }

   if(it_sz && r_code == ERROR_SUCCESS)
   {
    PMIB_IPNETROW  nr =(PMIB_IPNETROW) it->table;
    for(DWORD i = 0;i<it->dwNumEntries;i++,nr++)
    {
     if(!(nr->dwType != 2 && ip_addr == nr->dwAddr))
      continue;
      if(msz>=int(nr->dwPhysAddrLen))
      {
       memcpy(mac_addr,nr->bPhysAddr,nr->dwPhysAddrLen);
       ret = ERROR_SUCCESS;
      }
      else
       ret = ERROR_BUFFER_OVERFLOW;
      break;
    }

   }
   if(buffer!=(char*)it)
      GlobalFree(HGLOBAL(it));
  return ret;    
}

int __fastcall detect_mac_remote(DWORD ip_addr,BYTE * mac_addr,int & msz)
{
  //Определение mac адреса удаленного компьютера
  int ret = do_detect_mac_remote(ip_addr,mac_addr,msz);
  if( ret == ERROR_NO_DATA)
  {
    //создаем UDP гнездо и отправляем некие данные на него
    SOCKET s     = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
    if(s && s!=(SOCKET)SOCKET_ERROR)
    {
    sockaddr_in addr;
    ZeroMemory(&addr,sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.S_un.S_addr = ip_addr;addr.sin_port=5555;
    sendto(s,(char*)&addr,sizeof(addr),0,(sockaddr*)&addr,sizeof(addr));
    sendto(s,(char*)&addr,sizeof(addr),MSG_DONTROUTE,(sockaddr*)&addr,sizeof(addr));
    Sleep(100);
    addr.sin_port=0;
    sendto(s,(char*)&addr,sizeof(addr),0,(sockaddr*)&addr,sizeof(addr));
    Sleep(100);
    ret = do_detect_mac_remote(ip_addr,mac_addr,msz);
    closesocket(s);
    }
    else
    ret = GetLastError();
  }
  return ret;
}

int __fastcall detect_mac_address(DWORD ip_addr,BYTE * mac_addr,int & msz)
{
 if(mac_addr && msz)
 {
 int ret = detect_mac_local (ip_addr,mac_addr,  msz);
 if (ret ==ERROR_NO_DATA)
     ret = detect_mac_remote(ip_addr,mac_addr,  msz);
 return ret;
 }
 return ERROR_INVALID_PARAMETER;
}

union s_byte
{
 struct{
        BYTE lo4:4;
        BYTE hi4:4;
       };
 BYTE  bt;
};

int  __fastcall str2macaddr(char * str,BYTE * ma,int masz)
{
 int ret = 0;
 if(str && ma && masz)
 {
 memset(ma,0,masz);
 s_byte * bt = (s_byte*)ma;
 while(*str)
 {
  DWORD dig = hexchar2digit(*(str++));
  if(dig == DWORD(-1))
     {bt++;ret++;continue;}
  bt->hi4 = bt->lo4;
  bt->lo4 = dig;
 }
 }
 return ret;
}


