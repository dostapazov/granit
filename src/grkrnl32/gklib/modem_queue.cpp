#pragma hdrstop
#include <modem.hpp>



 int    __fastcall  TModemDataQueue::put_data (LPMPROTO_HEADER hdr,bool first)
 {
  if(hdr)
  {
  int sz = sizeof(*hdr)+hdr->data_size-sizeof(hdr->data[0]);
  return PutIntoQueue(hdr,sz,first);
  }
  return -1;
 }


 int    __fastcall  TModemDataQueue::put_data (LPMPROTO_HEADER hdr,LPVOID data,DWORD dsz,bool first)
 {
     int ret = -1;
     if(hdr )
     {
       dsz = data ? dsz:0;
       DWORD need_size = sizeof(*hdr)-sizeof(hdr->data[0])+dsz;
       LPSIMPLEDATA sd = alloc_data_buffer(need_size) ;
       if(sd)
       {

        LPMPROTO_HEADER dst_hdr = (LPMPROTO_HEADER)sd->data;
        memcpy(dst_hdr,hdr,sizeof(*hdr));
        dst_hdr->data_size = dsz;
        memcpy(dst_hdr->data,data,dsz);
        Lock();
        if(first)
         insert(this->begin(),sd);
         else
         push_back(sd);
        ret = size();
        Unlock();
       }
     }
     return ret;
 }


int    __fastcall  TModemDataQueue::put_data (LPMODEM_ADDR addr_to,
                                              LPMODEM_ADDR addr_from,
                                              WORD kadr_num,WORD fa,
                                              LPVOID data,DWORD sz,bool first)
{
 /*Помещение данных в очередь*/
 int    ret ;
 MPROTO_HEADER hdr;
 ZeroMemory(&hdr,sizeof(hdr));
 hdr.addr_to.addr  =  addr_to->addr;
 hdr.addr_from.addr= addr_from->addr;
 hdr.pkt_num       = kadr_num;
 hdr.fa            = fa;
 mproto_protect(&hdr);
 ret               = put_data(&hdr,data,sz,first);
 return ret;
}

LPMPROTO_HEADER __fastcall  TModemDataQueue::peek_data(LPVOID buffer,DWORD bsz,int * rsz)
{
 LPMPROTO_HEADER ret_hdr = NULL;
 int copy_sz = 0;
 Lock();
 if(size())
 {
  LPSIMPLEDATA sd = this->front();
  LPMPROTO_HEADER src_hdr = (LPMPROTO_HEADER)sd->data;
  copy_sz = sizeof(*src_hdr)-sizeof(src_hdr->data[0])+src_hdr->data_size;
  if((DWORD)copy_sz<=bsz)
  {
   CopyMemory(buffer,src_hdr,copy_sz);
   ret_hdr = (LPMPROTO_HEADER) buffer;
  }
  else
  copy_sz =-copy_sz;
 }
 Unlock();
  if(rsz)
    *rsz = copy_sz;
 return ret_hdr;

}

LPMPROTO_HEADER __fastcall  TModemDataQueue::get_data (LPVOID buffer,DWORD bsz,int * rsz)
{
 LPMPROTO_HEADER ret_hdr;
 Lock();
 ret_hdr = peek_data(buffer,bsz,rsz);
 if(ret_hdr)
    DropFirst();
 Unlock();
 return ret_hdr;   


}


bool   __fastcall  TModemDataQueue::peek_data(LPMODEM_ADDR addr_to,LPMODEM_ADDR addr_from,LPWORD kadr_num,LPWORD fa,LPVOID data,DWORD bsz,int * out )
{
  bool ret  = false;
  int  ret_size = 0;
  Lock();
  if(size())
   {
    LPSIMPLEDATA sd = (LPSIMPLEDATA)this->front();
    LPMPROTO_HEADER src_hdr = (LPMPROTO_HEADER)sd->data;
    if(addr_to)     addr_to  ->addr = src_hdr->addr_to.addr;
    if(addr_from)   addr_from->addr = src_hdr->addr_from.addr;
    if(kadr_num)   *kadr_num = src_hdr->pkt_num;
    if(fa)         *fa       = src_hdr->fa;
    ret_size = src_hdr->data_size;
    if((int)bsz>=sd->size)
    {
     CopyMemory(data,src_hdr->data,ret_size);
     ret = true;
    }
    else
    ret_size = -ret_size;
   }
  Unlock();
  if(out)
     *out = ret_size;
  return ret;

}


