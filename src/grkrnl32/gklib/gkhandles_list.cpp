#pragma hdrstop
#include <gkhandle.hpp>
#include <algorithm>



 void     __fastcall TGKHandlesList::for_each(DWORD cmd,LPARAM p1,LPARAM p2)
 {
  lock();
  iterator b = begin(),e = end();
  while(b<e)
     {call_gkhandle(GKHANDLE(*b++),cmd,p1,p2);}

  unlock();
 }

  void     __fastcall TGKHandlesList::free()
  {
   lock();
   erase(begin(),end());
   resize(0);
   unlock();
  }

  int      __fastcall TGKHandlesList::count()
  {
   int ret;
   lock();
   ret = int(size());
   unlock();
   return ret;
  }

  int      __fastcall TGKHandlesList::index (GKHANDLE h)
  {
   int ret(-1);
   lock();
   iterator i = std::lower_bound(begin(),end(),h,gkhandle_less());
   if(i<end() && gkhandle_equal()(*i,h))
      ret = distance(begin(),i);
   unlock();
   return ret;
  }

  int     __fastcall TGKHandlesList::add   (GKHANDLE h)
  {
   int ret(-1);
   lock();
   iterator i = std::lower_bound(begin(),end(),h,gkhandle_less());
   if(/*(i==NULL_PTR  ) ||*/ i == end() || !gkhandle_equal()(*i,h))
      {
      i = insert(i,h);
      ret = distance(begin(),i);
      };
   unlock();
   return ret;
  }

  bool     __fastcall TGKHandlesList::remove(GKHANDLE h)
  {
   bool ret(false);
   lock();
   iterator i = lower_bound(begin(),end(),h,gkhandle_less());
   if(i<end() && gkhandle_equal()(*i,h))
    {ret = true;erase(i);}
   unlock();
   return ret;
  }

  GKHANDLE __fastcall TGKHandlesList::at(int idx)
  {
   GKHANDLE ret =NULL;
   lock();
   if(idx<count())
      ret = GKHANDLE(*(begin()+idx));
   unlock();
   return ret;
  }


