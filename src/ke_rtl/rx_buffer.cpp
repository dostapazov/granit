
#ifdef __linux__
#include <gklib/rx_buffer.hpp>
#else
#include <rx_buffer.hpp>
#endif

#include <stdlib.h>



   DWORD    __fastcall rx_buffer::find (DWORD offset,DWORD find_value,DWORD end_offset)
   {
     if(buff && offset<len && end_offset>offset)
     {
      end_offset = KeRTL::MIN(end_offset,len);
      LPBYTE ptr = (LPBYTE) memchr(buff+offset, find_value,(end_offset-offset)) ;
      if(ptr)
        return distance(ptr);
     }
     return -1;
   }

   void     __fastcall rx_buffer::move_to_begin  (DWORD offset_from,DWORD offset_to)
   {
      if(offset_to!=offset_from)
      {
      if(buff && offset_to<offset_from && offset_from<len)
      {
       DWORD move_len = len-offset_from;
       len= len-offset_from+(offset_to);
       memmove(buff+offset_to,buff+offset_from,move_len);
      }
      else
      len = 0;
      }
   }

   bool     __fastcall rx_buffer::set_size(DWORD new_sz)
   {
    //Установка размера буфера
    DWORD page_size = 4096;
    div_t dt = div(new_sz,page_size);
    if(dt.rem)
       new_sz+=page_size-dt.rem;
    bool ret = false;
    LPBYTE old_ptr = buff;
    if(new_sz)
    {
      buff =  new BYTE[new_sz] ;
      if(buff)
      {
        if(old_ptr)
        memcpy(this->buff,old_ptr,len);
        ret = true;
        _size = new_sz;
      }
      else
        buff = old_ptr;
    }
    else
      _size = len = 0,buff = NULL,ret = true;
    if(old_ptr)
      delete [] old_ptr;
      return ret ;
   }


   bool     __fastcall rx_buffer::reserve (DWORD sz)
   {
    bool ret = true;
    DWORD free_sz = this->get_free_size();
    if(sz>free_sz)
      ret = grow(sz-free_sz);
    if(ret)
    {
     len +=sz;
    }
    return ret;
   }

   bool     __fastcall rx_buffer::add   (LPBYTE src,DWORD sz)
   {
    bool ret = true;
    DWORD free_sz = this->get_free_size();
    if(sz>free_sz)
      ret = grow(sz-free_sz);
    if(ret)
    {
     #ifdef _Windows
     if(!end() || !src)  DebugBreak();
     //char text[256];
     //wsprintf(text,"sz = %d rxb free = %d",sz,this->get_free__size());
     //OutputDebugString(text);
     #endif

     memcpy(this->end(),src,sz);
     len+=sz;
    }
    return ret;
   }


