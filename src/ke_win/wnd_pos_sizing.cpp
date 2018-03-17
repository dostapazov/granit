/*
 Изменение положения и/или размеров дочерних окон
  взависимости от выбранных флагов
*/


#include <gkwind.hpp>

#ifndef _NO_NAMESPACE
namespace KeWin
{
#endif


KEWIN_FUNC void __fastcall TrackWndSizePosition(HWND parent, HWND control,TRect & old_r,TRect & new_r,DWORD flags,bool redraw)
{
 
	int dx = new_r.Width () - old_r.Width ();
	int dy = new_r.Height() - old_r.Height();
        TPoint top_left;
	TRect  ctrl_rect;

	DWORD Flags = SWP_NOMOVE| (redraw ? (SWP_DRAWFRAME|SWP_NOCOPYBITS):0);
        GetWindowRect((::HWND)control,&ctrl_rect);

	if(flags&FLWND_ANCHOR_BOTTOM)
       ctrl_rect.bottom+=dy;
	if(flags& FLWND_ANCHOR_RIGHT)
       ctrl_rect.right+=dx;

	if(flags&FLWND_ANCHOR_TOP)
		ctrl_rect.top+=dy;
	if(flags& FLWND_ANCHOR_LEFT)
       ctrl_rect.left+=dx;

	if(flags&(FLWND_ANCHOR_BOTTOM|FLWND_ANCHOR_RIGHT))
		Flags&=~SWP_NOSIZE;

       if(flags&(FLWND_ANCHOR_TOP|FLWND_ANCHOR_LEFT))
		Flags&=~SWP_NOMOVE;

        top_left = ctrl_rect.TopLeft();
	ScreenToClient((::HWND)parent,&top_left);
	SetWindowPos((::HWND)control,0,top_left.x,top_left.y,ctrl_rect.Width(),ctrl_rect.Height(),Flags);

}

#ifndef _NO_NAMESPACE
}
#endif
