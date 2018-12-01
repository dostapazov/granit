#include "grnet232.hpp"

/*
  Окно для WinSocket under W9x 
*/


LRESULT CALLBACK  TGrNetBase::socket_window_proc(HWND w, UINT Msg ,WPARAM wp,LPARAM lp)
// Процедура окна
{
 LPVOID cast_ptr;
 cast_ptr = LPVOID(GetWindowLong(w,GWL_USERDATA));
 TGrNetBase * base =  TGrNetBase::CAST(cast_ptr);
 if(base)
 {
  if(Msg == SOCKET_MSG)
  {
   base->lock();
   base->HandleSocketEvent( WSAGETSELECTEVENT(lp),WSAGETSELECTERROR(lp));
   base->unlock();
   return 0;
  }

  if(base->socket_window_subclass_proc)
    return CallWindowProc(base->socket_window_subclass_proc,w,Msg,wp,lp);
 }

 return DefWindowProc(w,Msg,wp,lp); 

}
