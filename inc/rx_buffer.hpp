#ifndef __RX_BUFFER_HPP
#define __RX_BUFFER_HPP

#ifdef __linux__
#include <lin_ke_defs.h>
#else
#include <ke_defs.h>
#endif // __WIN32__

class rx_buffer
{
   protected:

   LPBYTE     buff;
   DWORD      _size;
   DWORD      len;
   public:
   DWORD    __fastcall get_free_size(){return _size-len;}
   LPBYTE   __fastcall begin()        {return buff;}
   LPBYTE   __fastcall end  ()        {return buff+len;}
   DWORD    __fastcall find (DWORD offset,DWORD find_value,DWORD end_offset = -1);
   void     __fastcall move_to_begin  (DWORD offset_from,DWORD offset_to = 0);
   DWORD    __fastcall distance(LPBYTE ptr){return ptr-buff;}
   bool     __fastcall grow    (DWORD grow_size){ return set_size(_size+grow_size);}
   bool     __fastcall set_size(DWORD new_sz);
   bool     __fastcall add     (LPBYTE src,DWORD sz);
   DWORD    __fastcall get_data_len(){return len;}
   bool     __fastcall reserve (DWORD len);
   void     __fastcall clear   (){len = 0;}
   rx_buffer (DWORD _size = 0):buff(NULL),_size(0),len(0){set_size(_size);}
   ~rx_buffer(){set_size(0);}
};

#endif

