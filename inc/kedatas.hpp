/*
  Очередь данных
  TSimpleDataQueue
  TEventSimpleDataQueue
  Ostapenko D.V. ASDU Norilsk 1993-2001
*/

#ifndef _KE_DATAQUEUE_INC
#define _KE_DATAQUEUE_INC

#include <KeRTL.hpp>
#include <vector>
#include <queue>
using namespace std;

#ifndef _NO_NAMESPACE
namespace KeRTL
{
#endif


typedef struct {
                 int  size;
                 unsigned char  data[1];
                }SIMPLEDATA,*LPSIMPLEDATA;

inline LPSIMPLEDATA   __fastcall AllocDataBuffer(LPVOID data,int sz)
{
 LPSIMPLEDATA buf = (LPSIMPLEDATA)new unsigned char[sz+sizeof(SIMPLEDATA)];
 buf->size = sz;
 CopyMemory(buf->data,data,sz);
 return buf;
}

inline void __fastcall ReleaseDataBuffer(LPSIMPLEDATA b)
{
 delete[](unsigned char*)b;
}


KERTL_CLASS TSimpleDataQueue:protected vector<LPSIMPLEDATA>
{
 protected:
 TFASTMutex locker;
 LPSIMPLEDATA __fastcall pop();
 void         __fastcall push(LPSIMPLEDATA,bool first = false);
 public:
 TSimpleDataQueue ():std::vector<LPSIMPLEDATA>(){}
 ~TSimpleDataQueue();
 virtual bool __fastcall PutIntoQueue (LPVOID data,int sz,bool to_front = false);
 virtual bool __fastcall GetFromQueue (LPVOID data,int buf_sz,int*out=0);
 virtual bool __fastcall PeekFromQueue(LPVOID data,int buf_sz,int*out=0);
 virtual void __fastcall DropData();
 virtual void __fastcall DropFirst();
 int  __fastcall TopDataSize();
 int  __fastcall QueueCount();
 bool __fastcall Lock()  {return locker.Lock();  };
 bool __fastcall Unlock(){return locker.Unlock();};
 void __fastcall full_unlock(){locker.FullUnlock();}
 TFASTMutex & __fastcall get_locker(){return locker;}
 LPSIMPLEDATA __fastcall __at(DWORD idx);

};

 inline LPSIMPLEDATA   __fastcall TSimpleDataQueue::__at(DWORD idx)
 {
   return idx<size() ? at(idx) : NULL;
 }


KERTL_CLASS TEventSimpleDataQueue:public TSimpleDataQueue
{
 KeRTL::TEvent event;
 public:
 TEventSimpleDataQueue(const char * ev_name=0):event(true,false,ev_name){};
 TEventSimpleDataQueue(const wchar_t * ev_name):event(true,false,ev_name){};
 virtual bool __fastcall PutIntoQueue(LPVOID data,int sz,bool to_front = false);
 virtual bool __fastcall GetFromQueue(LPVOID data,int buf_sz,int*out=0);
 virtual void __fastcall DropFirst();
 TEvent &  GetEvent();
};


/*inine implementations*/


 inline  bool __fastcall TEventSimpleDataQueue::PutIntoQueue(LPVOID data,int sz,bool to_front)
 {
   bool ret = false;
   Lock();
   if(TSimpleDataQueue::PutIntoQueue(data,sz,to_front))
   {
     if(QueueCount()==1)
       event.SetEvent(true);
     ret = true;
   }
   Unlock();
     return ret;
 }

 inline  bool __fastcall TEventSimpleDataQueue::GetFromQueue(LPVOID data,int buf_sz,int*out)
 {
   bool ret = false;
   Lock();
   if(TSimpleDataQueue::GetFromQueue(data,buf_sz,out))
   {
      if(empty()) event.SetEvent(false);
      ret =  true;
   }
   Unlock();
   return ret;
 }

#ifndef _NO_NAMESPACE
}
#endif

#endif
