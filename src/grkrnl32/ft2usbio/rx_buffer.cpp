#include "ft2usbio.hpp"

   bool     __fastcall rx_buffer::set_size(DWORD new_sz)
   {
    if(new_sz != this->_size)
    {
     if(buff)
        {
         delete [] buff;
         buff = NULL;
        } 
      _size = new_sz;
      if(_size)
        buff = new BYTE [_size];
    }
    return buff ? true:false;
   }

   void     __fastcall rx_buffer::move_to_begin  (LPBYTE from)
   {
     int dist = from-buff;
     if(dist  >=0 && (DWORD)dist <size)
      {
       len  -= dist;
       memmove(buff,from,len);
      }
     else
     len = 0;
   }
   

 
