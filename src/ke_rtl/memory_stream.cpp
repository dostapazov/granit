/*
  Реализация потока в память
*/

#include <KeSTRM.HPP>

#ifndef _NO_NAMESCPACE
namespace KeRTL{
#endif

 /*BYTE  *    __memptr;//
 DWORD      mem_size;//размер выделенной памяти
 DWORD      position;//Текущая позиция
 DWORD      inc_by;
 public:
 */

 bool       __fastcall TMemoryStream::resize()
 {
  #ifndef MEMSTRM_USE_REALLOC
  LPBYTE ptr = (LPBYTE)Alloc(stream_size+inc_by,HEAP_ZERO_MEMORY);
  if(ptr)
    {
     stream_size+=inc_by;
     if(using_size)
     memcpy(ptr,__memptr,using_size);
     Free(__memptr);
     __memptr = ptr;
     return true;
    }
   return false;
  #else
  LPBYTE ptr = (LPBYTE)Realloc(__memptr,stream_size+=inc_by,HEAP_ZERO_MEMORY);
  if(ptr)
    {
    __memptr = ptr;
    stream_size+=inc_by;
    }
    return ptr ? true:false;
  #endif
 }

 bool   __fastcall TMemoryStream::SetSize(DWORD size)
 {
   if(using_size<size)
   {
     if(stream_size<size)
      {
        DWORD nsize = size<inc_by ? inc_by:size;
        nsize+= inc_by%size;
        LPBYTE ptr = (LPBYTE)this->Alloc(nsize,HEAP_ZERO_MEMORY);
        if(ptr)
        {
         GetLastError();
         ptr = (LPBYTE)this->Alloc(nsize,HEAP_ZERO_MEMORY);
         memcpy(ptr,__memptr,using_size);
         Free(__memptr);
         __memptr = ptr;
         stream_size  = nsize;
        }
        else
        __memptr = NULL;
      }
   }
   if(__memptr)
   {
    using_size = size;
    Seek(0,smFromEnd);
   }
   return true;
 }

 TMemoryStream::TMemoryStream(DWORD   InitSize ,DWORD IncBy ):TStream(),
 THeap(InitSize+1024)
 {
   inc_by      = (IncBy);
   stream_size = (InitSize);
   stream_position   = (0);
   using_size = (0);
   __memptr =(LPBYTE) Alloc(stream_size,HEAP_ZERO_MEMORY	);
 }

 TMemoryStream::TMemoryStream(LPVOID  src,DWORD cbSize,   DWORD IncBy,bool no_alloc  ):
 THeap(cbSize+1024)
 {
   if(no_alloc)
   {
    __memptr = (LPBYTE)src;
    stream_size = using_size = cbSize;
    stream_position = 0;
   }
   else
   {
   inc_by = (IncBy), stream_size = (cbSize+1024) ,using_size = (cbSize),stream_position = (0);
   __memptr = (LPBYTE)Alloc(stream_size,HEAP_ZERO_MEMORY	);
   memcpy(__memptr,src,using_size);
   }
   this->Seek(0,smFromEnd);
 }

 int    __fastcall TMemoryStream::Write(void * data,DWORD size,bool )
 {
  if(stream_position+size>=stream_size)
     if(!resize())
         return 0;
  memcpy(__memptr+stream_position,data,size);
  using_size = MAX(using_size,stream_position+size);
  Seek(size,smFromCurr);
  return size;
 }

 DWORD  __fastcall TMemoryStream::Reserve(DWORD sz)
 {
  DWORD ret = this->Seek(0,smFromEnd);
  SetSize(using_size+sz);
  return ret;
 }

 int    __fastcall TMemoryStream::Read (void * buffer,DWORD sz,bool )
 {
  DWORD  bytes_to_read = MIN(sz,using_size-stream_position);
  memcpy(buffer,__memptr+stream_position,bytes_to_read);
  Seek(bytes_to_read,smFromCurr);
  return bytes_to_read;
 }

 DWORD  __fastcall TMemoryStream::Seek (DWORD pos,SeekMethod sm)
 {
  switch (sm)
  {
   case smFromBegin:
        stream_position = MIN(pos,using_size);
        break;
   case smFromCurr:
        stream_position = MIN(stream_position+pos,using_size);
        break;
   case smFromEnd:
        stream_position = using_size-MIN(pos,using_size);
        break;
  }

  return stream_position;
 }

 void   __fastcall TMemoryStream::Erase(DWORD pos,DWORD len)
 {
   if(using_size>pos)
    {
     if((pos+len)>using_size)
        len =(using_size-pos); 
     memmove(__memptr+pos,__memptr+pos+len,this->using_size-pos-len);
     using_size-=len;
     if(stream_position>using_size)
       stream_position=using_size;
    }
 }


#ifndef _NO_NAMESCPACE
}//namespace KeRTL{
#endif


