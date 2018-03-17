#include <kedatas.hpp>

#ifndef _NO_NAMESPACE
namespace KeRTL
{
#endif

TSimpleDataQueue::~TSimpleDataQueue()
{
  DropData();
}
  int  __fastcall TSimpleDataQueue::QueueCount()
  {
   int ret;
   locker.Lock();
   ret = size();
   locker.Unlock();
   return ret;
  }

void __fastcall TSimpleDataQueue::DropFirst()
{
 locker.Lock();
  if(size())
  {
   LPSIMPLEDATA data = front();
   pop();
   ReleaseDataBuffer(data);
  } 
 locker.Unlock();

}

void __fastcall TSimpleDataQueue::DropData()
{
 locker.Lock();
 while(size())
   DropFirst();
 locker.Unlock();
}


 LPSIMPLEDATA __fastcall TSimpleDataQueue::pop()
 {

  LPSIMPLEDATA ret = NULL;
  if(size())
    {
     ret = *begin();
     erase(begin());
    }
    return ret;
 }

 void         __fastcall TSimpleDataQueue::push(LPSIMPLEDATA sd,bool to_front)
 {
  if(to_front)
    insert(begin(),sd);
   else
   push_back(sd);
 }


bool __fastcall TSimpleDataQueue::PutIntoQueue(LPVOID data,int sz,bool to_front)
{
 if(data && sz)
 {
  this->locker.Lock(INFINITE);
  LPSIMPLEDATA p = AllocDataBuffer(data,sz);
  push(p,to_front);
  locker.Unlock();
  return true;
 }
 return false;
}

 int  __fastcall TSimpleDataQueue::TopDataSize()
 {
  int ret(0);
  locker.Lock();
  if(!empty())
    ret = this->front()->size;
  locker.Unlock();
  return ret;
 }

 bool __fastcall TSimpleDataQueue::GetFromQueue(LPVOID data,int buf_sz,int*out)
 {
   bool ret(false);
   locker.Lock();
   if(PeekFromQueue(data,buf_sz,out))
   {
    ret = true;
    DropFirst();
   }
   locker.Unlock();
   return ret;
 }

 bool __fastcall TSimpleDataQueue::PeekFromQueue(LPVOID data,int buf_sz,int*out)
 {
  bool ret(false);
  if(data && buf_sz)
  {
  locker.Lock();
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
  locker.Unlock();
  }
  return ret;
 }

 void __fastcall TEventSimpleDataQueue::DropFirst()
 {
  locker.Lock();
  TSimpleDataQueue::DropFirst();
  if(empty())
     event.SetEvent(false);
  locker.Unlock();
 }
 TEvent &  TEventSimpleDataQueue::GetEvent()
 {
  return event;
 }



#ifndef _NO_NAMESPACE
//namespace KeRTL
}
#endif
