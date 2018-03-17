#pragma hdrstop
#include "iec60870_modem.hpp"


  bool __fastcall iec60870_buffer_t::grow    (DWORD grow_size)
  {
    DWORD cur_hdr_offset = 0;
    if(current_hdr)
       cur_hdr_offset = ((LPBYTE)current_hdr) - begin();
    bool ret = rx_buffer::grow(grow_size);
    if(ret && current_hdr)
         current_hdr = lpiec60870_proto_header_t(begin()+cur_hdr_offset);
     return ret;
  }

 int __fastcall iec60870_buffer_t::add_bytes(LPBYTE ptr,DWORD length)
 {
   if(ptr && length)
   {
    TLockHelper l(locker);
    if(get_free_size()<length) if(!grow(length<<1)) throw std::bad_alloc();
    while(length && !current_hdr)
     {
     if((*ptr) != IEC60870_PROTO_START_MARKER)
         {
          ++ptr;--length;
         }
         else
         break;
     }
     if(length)
        {
         if(!current_hdr && *ptr == IEC60870_PROTO_START_MARKER)
             current_hdr = (lpiec60870_proto_header_t)end();
         add(ptr,length);
        }
   }
   return this->get_data_len();
 }

 bool __fastcall iec60870_buffer_t::is_frame_complete()
 {
   TLockHelper l(locker);
   if(current_hdr)
   {
     DWORD len = end() - (LPBYTE)current_hdr;
     return (len>sizeof(current_hdr->start_marker)) && iec60870_proto_get_frame_length(current_hdr) <= len;
   }
   return false;
 }

 lpiec60870_proto_header_t __fastcall next_frame_from( lpiec60870_proto_header_t phdr)
 {
    LPBYTE ptr = (LPBYTE)phdr;
    ptr += iec60870_proto_get_frame_length(phdr);
    phdr = (lpiec60870_proto_header_t)ptr;
    if( phdr->start_marker == IEC60870_PROTO_START_MARKER )
       return phdr;
    return NULL;
 }

 bool __fastcall iec60870_buffer_t::next_frame       (bool _erase)
 {
   TLockHelper l(locker);
   if(is_frame_complete())
     {
       LPBYTE ptr = (LPBYTE)current_hdr;
       ptr+=iec60870_proto_get_frame_length(current_hdr);
       int remain_len = int(end()- ptr);
       if(remain_len && (*ptr) == IEC60870_PROTO_START_MARKER)
       {
        current_hdr = (lpiec60870_proto_header_t)ptr ;
        if(_erase && remain_len<current_hdr->lenght)
           {
             len = end()-(LPBYTE)current_hdr;
             memmove(begin(),current_hdr,len);
             current_hdr = (lpiec60870_proto_header_t)begin();
           }
        return is_frame_complete();
       }
       else
       {
        current_hdr = NULL;
        if(_erase)  clear();
       }
     }
   return false;
 }

 void __fastcall iec60870_buffer_t::move_to_begin    ()
 {
   TLockHelper l(locker);
   if(current_hdr)
   {
       int before_len = ((LPBYTE)current_hdr) - begin();
       memmove(begin(),current_hdr,before_len);
       this->len -= before_len;
       current_hdr = (lpiec60870_proto_header_t)begin();
   }
   else
   len = 0;
 }

 void __fastcall iec60870_buffer_t::insert_first    (lpiec60870_proto_header_t hptr)
 {
  if(iec60870_proto_is_valid(hptr))
  {
    DWORD frame_len = iec60870_proto_get_frame_length(hptr);
    TLockHelper l(locker);
    if(get_free_size()<frame_len) grow(frame_len<<1);
    if(current_hdr)
    {
      memmove(begin()+frame_len,begin(),this->len);
      this->len+=frame_len;
      memcpy(begin(),hptr,frame_len);
    }
    else
     add((LPBYTE)hptr,frame_len);
  }
 }

 bool   __fastcall iec60870_buffer_t::is_frame_complete(lpiec60870_proto_header_t phdr)
{
   DWORD frame_offset = get_frame_offset(phdr);
   DWORD dlen     = get_data_len();
   DWORD rem_len  = dlen - frame_offset;
   if(rem_len>= (sizeof(phdr->start_marker)+sizeof(phdr->lenght)) && iec60870_proto_get_frame_length(phdr)<=rem_len)
      return true;
   return false;
}


 int  __fastcall iec60870_buffer_t::get_frames_count()
 {
    int count = 0;
    TLockHelper l(locker);
    lpiec60870_proto_header_t phdr =  this->current_hdr;
    while(phdr && is_frame_complete(phdr))
    {
     ++count;
     phdr = next_frame_from(phdr);
    }
   return count;
 }

 bool __fastcall iec60870_buffer_t::set_current_frame(DWORD number)
 {
   bool ret = false;
   TLockHelper l(locker);
   if(len >= sizeof(*current_hdr))
   {
    current_hdr = (lpiec60870_proto_header_t)begin();
    while(number && is_frame_complete(current_hdr))
    {
      lpiec60870_proto_header_t phdr = next_frame_from(current_hdr);
      if(is_frame_complete(phdr))
        {
          current_hdr = phdr;
          --number;
        }
        else
        {
         current_hdr = NULL;
         break;
        }
    }
   }
   return ret;
 }

 bool __fastcall iec60870_buffer_t::erase_frames(DWORD count)
 {
   TLockHelper l(locker);
   set_current_frame(count);
   move_to_begin();
   return  true;
 }





