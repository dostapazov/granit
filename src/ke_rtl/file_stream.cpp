#pragma hdrstop
#include <kestrm.hpp>

#ifndef _NO_NAMESPACE
namespace KeRTL
{
#endif

  int    __fastcall TFileStream::Write(void * data,DWORD size,bool)
 {
   DWORD  ret =TFile::Write(data,size);
   TFile::Synchronize();
   stream_position = TFile::GetWritePos(); 
   using_size  = KeRTL::MAX(using_size,stream_position);
   stream_size = KeRTL::MAX(stream_position,stream_size);
   return ret;
 }
  int    __fastcall TFileStream::Read (void * buffer,DWORD sz,bool)
 {
  DWORD rdBytes(0);
  TFile::Read(buffer,sz,rdBytes);
  TFile::Synchronize();
  stream_position = TFile::GetReadPos();
  using_size  = KeRTL::MAX(using_size,stream_position);
  stream_size = KeRTL::MAX(stream_position,stream_size);
  return int(rdBytes);
 }

  DWORD  __fastcall TFileStream::Seek (DWORD pos,SeekMethod sm)
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

  TFile::SeekWr(stream_position,smFromBegin);
  TFile::SeekRd(stream_position,smFromBegin); 
  return stream_position;
 }

 bool   __fastcall TFileStream::SetSize(DWORD size)
{
 if(TFile::SetSize(size))
 {
  stream_size     =  size;
  using_size      =  KeRTL::MIN(stream_size,using_size);
  stream_position =  KeRTL::MIN(using_size,stream_position);
  return true;
 }
 return false;
 
}

 DWORD  __fastcall TFileStream::Reserve(DWORD sz)
{
 // Резервирование места
 DWORD ret = using_size;
 using_size += sz;
 if(using_size> stream_size)
    {
     SetSize(using_size+inc_by);
    }
 return ret;
}

#ifndef _NO_NAMESPACE
//namespace KeRTL
}
#endif
