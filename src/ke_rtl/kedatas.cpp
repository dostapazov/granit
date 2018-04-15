#include <kedatas.hpp>

#ifndef _NO_NAMESPACE
namespace KeRTL
{
#endif


#define SIMPLE_DATA_SW 0x534D5044
LPSIMPLEDATA          __fastcall TSimpleDataQueue::alloc_data_buffer(int sz)
{
 LPSIMPLEDATA buf = (LPSIMPLEDATA)new unsigned char[sz+sizeof(SIMPLEDATA)-sizeof(buf->data[0])];
 buf->size = sz;
 buf->sw   = SIMPLE_DATA_SW;
 return buf;
}

LPSIMPLEDATA   __fastcall TSimpleDataQueue::alloc_data_buffer(LPVOID data,int sz)
{
 LPSIMPLEDATA buf = alloc_data_buffer(sz);
 CopyMemory(buf->data,data,sz);
 return buf;
}

void __fastcall TSimpleDataQueue::release_data_buffer(LPSIMPLEDATA b)
{
 if(b &&  b->sw == SIMPLE_DATA_SW)
    delete[](unsigned char*)b;
}

TSimpleDataQueue::~TSimpleDataQueue()
{
  DropData();
}
  int  __fastcall TSimpleDataQueue::QueueCount()
  {
   int ret;
   TLockHelper l(locker);
   ret = size();
   return ret;
  }

void __fastcall TSimpleDataQueue::DropFirst()
{
  TLockHelper l(locker);
  if(size())
  {
   LPSIMPLEDATA data = pop();
   release_data_buffer(data);
  } 

}

void __fastcall TSimpleDataQueue::DropData()
{
 TLockHelper l(locker);
 while(size())
       DropFirst();
}


 LPSIMPLEDATA __fastcall TSimpleDataQueue::pop()
 {

  TLockHelper l(locker);
  LPSIMPLEDATA ret = NULL;
  if(size())
    {
     data_iterator ptr = begin();
     ret = *ptr;
     erase(ptr);
    }
    return ret;
 }

 int         __fastcall TSimpleDataQueue::push(LPSIMPLEDATA sd,bool to_front)
 {
  TLockHelper l(locker);
  if(to_front)
    insert(begin(),sd);
   else
    push_back(sd);
    return size();
 }


bool __fastcall TSimpleDataQueue::PutIntoQueue(LPVOID data,int sz,bool to_front)
{
 if(data && sz)
 {
  LPSIMPLEDATA p = alloc_data_buffer(data,sz);
  if(p)
    {
     push(p,to_front);
     return true;
    }
 }
 return false;
}

 int  __fastcall TSimpleDataQueue::TopDataSize()
 {
  int ret(0);
  TLockHelper l(locker);
  if(!empty())
    ret = front()->size;
  return ret;
 }

 bool __fastcall TSimpleDataQueue::GetFromQueue(LPVOID data,int buf_sz,int*out)
 {
   bool ret(false);
   TLockHelper l(locker);
   if(PeekFromQueue(data,buf_sz,out))
   {
    ret = true;
    DropFirst();
   }
   return ret;
 }

 int  __fastcall TSimpleDataQueue::move_to(TSimpleDataQueue & dest,int count)
 {
   TLockHelper l(locker);
   int mv_count = std::min(count,this->QueueCount());
   count = mv_count;
   while(count--)
   {
    dest.push(pop());
   }
   return mv_count;
 }

 bool __fastcall TSimpleDataQueue::PeekFromQueue(LPVOID data,int buf_sz,int*out)
 {
  bool ret(false);
  if(data && buf_sz)
  {
   TLockHelper l(locker);
   if(!empty())
    {
     LPSIMPLEDATA pd = front();
     if(pd->size<=buf_sz)
     {
      ret = true;
      memcpy(data,pd->data,pd->size);
      if(out)
         *out = pd->size;
     }
    }
    else
      if(out)
         *out = 0;
  }
  return ret;
 }

// void __fastcall TEventSimpleDataQueue::DropFirst()
// {
//  TLockHelper l(locker);
//  TSimpleDataQueue::DropFirst();
//  if(empty())
//     event.SetEvent(false);
//  locker.Unlock();
// }

 TEvent &  TEventSimpleDataQueue::GetEvent()
 {
  return event;
 }



#ifndef _NO_NAMESPACE
//namespace KeRTL
}
#endif
