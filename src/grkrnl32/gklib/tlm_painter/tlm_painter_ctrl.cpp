#pragma hdrstop

#include <kertl.hpp>
#include <pictdlgs.h>
#include <kfname.hpp>
#include <pictdlgs.h>
#include "tlmpainter.hpp"
#include <stdio.h>
#include <algorithm>
#include <iterator>
#include <functional>

 TGKModuleLoader TTlmPainter::painter_dll;
 DWORD           TTlmPainter::flash_counter = 0;


 const char * __fastcall TTlmPainter::GetClassName()
 {
  static char class_name[32];
  sprintf(class_name,"TTlmPainter#%08X",DWORD(this));
  return TWindow::GetClassName();

 }

 DWORD  __fastcall TTlmPainter::set_bkgnd_mode(DWORD mode)
 {
   
   if(kadr)
     {
      DWORD new_fl = (kadr->kadr_flags& ~MDB_KADR_FLAG_IMAGE_BKGND_MASK)|(mode&MDB_KADR_FLAG_IMAGE_BKGND_MASK);
      if(kadr->kadr_flags!=new_fl)
        {
         kadr->kadr_flags=new_fl;
         bkgnd.set_mode((tlmpaint::TBmpBkgnd::bkgnd_mode_t)(kadr->get_image_mode()));
         Repaint(true);
        }
     }
   return this->get_bkgnd_mode();
 }

void         __fastcall TTlmPainter::enable_edit(bool enable)
 {
  if(bool(editable)!=enable)
  {
   editable = enable ? TRUE:FALSE;
   bkgnd.set_grayed(is_bkgnd_grayed());
   //do_setup_bkgnd();
   Repaint(true);
  }
 }

 mdb_kadr *   __fastcall TTlmPainter::set_kadr(mdb_kadr * _kadr,DWORD ch_mask)
 {
   if(_kadr)
   {
    kadr=_kadr;
    //mediumdb.h
    if(ch_mask &(MDB_KADR_FIELD_SIZE|MDB_KADR_FIELD_DIAG|MDB_KADR_FIELD_IMAGE|MDB_KADR_FIELD_FLAGS))
       {
        //release_bkgnd();

        if(ch_mask&MDB_KADR_FIELD_IMAGE)
           set_bkgnd(this->bkgnd_image);
        do_setup_bkgnd();
        SetSize(kadr->kadr_size.cx,kadr->kadr_size.cy);
        Repaint(true);
       }
   }
   return 0;
 }

 int          __fastcall TTlmPainter::MessageHandler(MSG & msg)
 {
  int ret = 0;
  //DWORD keys = msg.wParam;

  DWORD      mouse_over_entry;
  int        dx,dy;
  switch(msg.message)
  {
   case   WM_GETDLGCODE : ret = DLGC_WANTALLKEYS; break;
   case   WM_LBUTTONDOWN:
   case   WM_LBUTTONUP:
          on_lmouse(*&KeWin::TPoint (LOWORD(msg.lParam),HIWORD(msg.lParam)),
                    (msg.message == WM_LBUTTONDOWN)?true:false,msg.wParam);
          break;

   case   WM_RBUTTONDOWN:
   case   WM_RBUTTONUP:
          on_rmouse(*&KeWin::TPoint (LOWORD(msg.lParam),HIWORD(msg.lParam)),
                    (msg.message == WM_RBUTTONDOWN)?true:false,msg.wParam);
          break;

   case   WM_MOUSEMOVE:
          on_mouse_move(*&KeWin::TPoint (LOWORD(msg.lParam),HIWORD(msg.lParam)),msg.wParam);
          break;
   case   WM_ACTIVATE:

          break;
   case   WM_KEYDOWN :
          on_keydown(msg.wParam,msg.lParam);
          break;

   default:ret = TWindow::MessageHandler(msg);break;
  }
  return ret;
 }

 void         __fastcall TTlmPainter::paint_select_rect(HDC dc,LPPOINT  pt)
 {
     int old_rop = ::SetROP2(dc ,R2_NOTXORPEN);

     if(pt)
     {
      MoveToEx(dc,spot_pt.x,spot_pt.y,NULL);
      LineTo  (dc,spot_pt1.x,spot_pt.y );
      LineTo  (dc,spot_pt1.x,spot_pt1.y);
      LineTo  (dc,spot_pt.x ,spot_pt1.y);
      LineTo  (dc,spot_pt.x ,spot_pt.y );
      spot_pt1 = *pt;
     }

     MoveToEx(dc,spot_pt.x ,spot_pt.y,NULL);
     LineTo  (dc,spot_pt1.x,spot_pt.y );
     LineTo  (dc,spot_pt1.x,spot_pt1.y);
     LineTo  (dc,spot_pt.x ,spot_pt1.y);
     LineTo  (dc,spot_pt.x ,spot_pt.y );
     SetROP2(ClientDC,old_rop);

 }

 void         __fastcall TTlmPainter::on_lmouse(POINT & pt,bool down,DWORD keys)
 {
    //Обработка левой кнопки мыши
    if(down)
    mouse_buttons |= MOUSE_BUTTON_LEFT;
    else
    mouse_buttons &= ~MOUSE_BUTTON_LEFT;

    if(on_mouse_buttons_callback.HaveMethod())
       {
        Tmouse_data md;
        md.mbuttons = MOUSE_BUTTON_LEFT;
        md.pt       = pt;
        md.down     = down;
        md.keys     = keys;
        if(on_mouse_buttons_callback(this,&md))
           return;
       }

       if(down)
       {
           if(mouse_over < ent_list.size())
           {
            spot_pt = pt;
            //Мышь над объектом
            if(!is_selected(mouse_over))
              {
               //on_down_select = true;
               select_entry(mouse_over,(keys&MK_CONTROL)? true:false);
              }
              else
              on_down_select = true;
           }
           else
           {
            if(!(keys&MK_CONTROL))
               select_entry(mouse_over,false);
            captured = true;
            SetCapture(hWnd);
            spot_pt.x = spot_pt1.x = pt.x;
            spot_pt.y = spot_pt1.y = pt.y;

           }
       }
       else
       {
          if(captured)
          {
           ReleaseCapture();
           captured = FALSE;
           on_down_select = false;
           RECT r;
           POINT pt1,pt2;
           pt1.x = std::min(spot_pt.x,spot_pt1.x);
           pt2.x = std::max(spot_pt.x,spot_pt1.x);
           pt1.y = std::min(spot_pt.y,spot_pt1.y);
           pt2.y = std::max(spot_pt.y,spot_pt1.y);

           SetRect(&r,pt1.x,pt1.y,pt2.x,pt2.y);
           select_in_rect(r,(keys&MK_CONTROL)? true : false );
          }
          else
          {
           if(on_down_select)
              select_entry(mouse_over,(keys&MK_CONTROL)? true:false);
              on_down_select = false;
              

          }
           if(GetFocus()!=hWnd)
              this->SetFocus();
       }
 }

 void         __fastcall TTlmPainter::set_mouse_buttons_callback(Tmouse_closure::closure_method cm,LPVOID user_data)
 {
   on_mouse_buttons_callback.SetClosure(user_data,cm);
 }

 void         __fastcall TTlmPainter::set_mouse_move_callback(Tmouse_closure::closure_method cm,LPVOID user_data)
 {
   on_mouse_move_callback.SetClosure(user_data,cm);
 }

 void         __fastcall TTlmPainter::set_keydown_closure(Tkeyboard_closure::closure_method cm,LPVOID user_data)
 {
   on_keydown_closure.SetClosure(user_data,cm);
 }


 void         __fastcall TTlmPainter::on_rmouse(POINT & pt,bool down,DWORD keys)
 {
    if(down)
    mouse_buttons |= MOUSE_BUTTON_RIGHT;
    else
    mouse_buttons &= ~MOUSE_BUTTON_RIGHT;

    if(on_mouse_buttons_callback.HaveMethod())
       {
        Tmouse_data md;
        md.mbuttons = MOUSE_BUTTON_RIGHT;
        md.pt       = pt;
        md.down     = down;
        md.keys     = keys;
        on_mouse_buttons_callback(this,&md);
       }

 }

 void         __fastcall TTlmPainter::on_mouse_move(POINT & pt,DWORD keys)
 {
        if(pt.x<1 || pt.y <1)
        {
             //DebugBreak();
        }

        long dx = pt.x - spot_pt.x;
        long dy = pt.y - spot_pt.y;
        if(on_mouse_move_callback.HaveMethod())
        {
          Tmouse_data md;
          md.mbuttons = mouse_buttons;
          md.pt       = pt;
          md.down     = false;
          md.keys     = keys;
          if(on_mouse_move_callback(this,&md)) return;
        }

        bool sizing = (keys&MK_SHIFT) ? true:false;
          if(captured)
          {
           if(this->editable && this->get_selected_count())
              {
               move_size_selected(dx,dy,sizing);
               spot_pt = pt;
              }
              else
              paint_select_rect(ClientDC,&pt);
          }
          else
          {
           set_mouse_over_entry(find_entry(pt));
           if(is_selected(mouse_over))
           {
            if(keys&MK_LBUTTON)
            {
             on_down_select = true;
             if(!captured && (abs(dx) > 5 || abs(dy) >5))
                {
                 captured = TRUE;
                 SetCapture(hWnd);
                }

            //move_size_selected(dx,dy,sizing);
            //spot_pt = pt;
            }
            else
            on_down_select = false;
           }

          }

 }

 void __fastcall TTlmPainter::select_in_rect(RECT &r,bool add)
 {
 if(!add)
    select_clean(false);
 Tentryes_list::iterator ptr  = ent_list.begin();
 Tentryes_list::iterator eptr = ent_list.end();
 int i = 0;
 while(ptr<eptr)
 {
   POINT pt1,pt2;
   pt1.x = ptr->position.x;
   pt1.y = ptr->position.y;
   pt2 = pt1;
   pt2.x += ptr->size.cx;
   pt2.y += ptr->size.cy;


   if( PtInRect(&r,pt1) || PtInRect(&r,pt2) )
   {
    this->select_entry(i,true);
   }
   ++ptr; ++i;
 }
  this->Repaint(false,NULL);

 }

 void         __fastcall TTlmPainter::on_keydown(DWORD v_key,DWORD key_data )
 {
  if(this->on_keydown_closure.HaveMethod())
  {
     Tkeyboard_data kd;
     kd.v_key    = v_key;
     kd.key_data = key_data;
     if(on_keydown_closure(this,&kd))
        return;
  }


  bool sizing = ((GetAsyncKeyState(VK_RSHIFT)|GetAsyncKeyState(VK_LSHIFT))&0x8000) ? true:false;
  int m_dist =  sizing ||( (GetAsyncKeyState(VK_RCONTROL)|GetAsyncKeyState(VK_LCONTROL))&0x8000) ? 1:KeRTL::MAX(1,(int)grid_step);
  switch(v_key)
  {
   case VK_LEFT :
        move_size_selected(-m_dist,0,sizing);
        break;
   case VK_RIGHT:
        move_size_selected(m_dist,0,sizing);
        break;
   case VK_DOWN :
        move_size_selected(0,m_dist,sizing);
        break;
   case VK_UP   :
        move_size_selected(0,-m_dist,sizing);
        break;
   default:
       this->DefaultProcessing();
       break;

  }

 }


 void         __fastcall TTlmPainter::release_bkgnd_bitmaps()
 {
  this->release_bkgnd();
  if(mem_dc)
       DeleteDC(mem_dc);
  mem_dc = NULL;    
 }

 void         __fastcall TTlmPainter::restore_bkgnd_bitmaps()
 {
  if(!mem_dc)
     {
      mem_dc = CreateCompatibleDC(ClientDC);
      set_bkgnd(this->bkgnd_image);
     }    
 }

 void         __fastcall TTlmPainter::OnWMDestroy()
 {
  release_bkgnd_bitmaps();
  if(mem_dc)
   {
    DeleteDC(mem_dc);
    mem_dc = NULL;
   }
  TWindow::OnWMDestroy();

 }


 KeWin::TSize __fastcall get_bitmap_size(HBITMAP hBmp)
 {
     BITMAP  _bmp;
     GetObject(hBmp,sizeof(_bmp),&_bmp);
     return KeWin::TSize(_bmp.bmWidth,_bmp.bmHeight);
 }

 int          __fastcall TTlmPainter::EraseBkgnd(HDC dc)
 {
//  if(bkgnd_bmp)
//    {
//     return  1;
//    }
  if(bkgnd()) return 1;
  return TWindow::EraseBkgnd(dc);
 }


 bool         __fastcall TTlmPainter::is_selected(DWORD offs)
 {
  Tselected_list::iterator end =sel_list.end(),
      ptr = std::lower_bound(sel_list.begin(),end,offs);
  return ptr<end && *ptr == offs ? true:false;    
 }

 DWORD __fastcall TTlmPainter::get_selected_recid(DWORD num)
 {
  if(num<this->get_selected_count())
  {
   DWORD ent_num = sel_list.at(num);
   return ent_list.at(ent_num).rec_id;
  }
  return (DWORD)-1;
 }

 DWORD        __fastcall TTlmPainter::get_selected_recid(Tselected_list & lst)
 {
   Tselected_list::iterator ptr = sel_list.begin(),end = sel_list.end();
   while(ptr<end)
     {
        mdb_kadr_entry & ke = ent_list.at(*ptr);
        lst.push_back(ke.rec_id);
        ++ptr;
     }
  return lst.size();
 }




 void         __fastcall TTlmPainter::calc_min_entry_size(mdb_kadr_entry &e ,KeWin::TSize & min_size)
 {
  TLMOBJ_PAINT_PARAM  pp;
  ZeroMemory(&pp,sizeof(pp));
  pp.dw_size = sizeof(pp);
  pp.paint_dc = ClientDC;
  pp.position = e.position;
  pp.size     = e.size;
  pp.bkgnd_color = e.options.bkgnd_color;
  pp.color       = e.options.color;
  pp.font        = this->Font;
  pp.paint_selected = false;
  pp.erase          = false;
  //pp.bkgnd_bitmap   = this->bkgnd_bmp;
  pp.bkgnd_bitmap   = bkgnd();
  pp.precision      = e.options.prec;
  pp.paint_variant  = e.options.paint_as;
  pp.paint_options  = e.options.paint_flags;
  mdb_record_ex  rec;
  rec.rec_id = e.rec_id;
  _get_record(this,&rec);
  pp.record         = mdb_short_record(rec);
  wstring str       = _get_text(this,rec.sname);
  if(str.length())
  pp.short_name     = str.c_str();
  painter_dll(PMCMD_CALC_SIZE,(LPARAM)&pp,0);
  min_size.cx = pp.size.cx;
  min_size.cy = pp.size.cy;

 }

 void         __fastcall TTlmPainter::calc_min_entry_size(Tentryes_list::iterator ptr,KeWin::TSize & min_size)
 {
  calc_min_entry_size(*ptr,min_size);
 }



 void         __fastcall TTlmPainter::paint_entry  (HDC dc,Tentryes_list::iterator ptr,bool sel,bool  erase,bool flash)
 {
  TLMOBJ_PAINT_PARAM  pp;
  ZeroMemory(&pp,sizeof(pp));
  pp.dw_size = sizeof(pp);
  pp.paint_dc = dc;
  pp.position = ptr->position;
  pp.size     = ptr->size;
  pp.bkgnd_color = ptr->options.bkgnd_color;
  pp.color       = ptr->options.color;
  pp.font        = Font;
  pp.paint_selected = sel;
  //pp.bkgnd_bitmap   = this->bkgnd_bmp;
  pp.bkgnd_bitmap   = this->bkgnd();
  pp.precision      = MAKELONG(ptr->options.prec,ptr->options.digits);
  pp.paint_variant  = ptr->options.paint_as;
  pp.paint_options  = ptr->options.paint_flags;
  mdb_record_ex  rec;
  rec.rec_id = ptr->rec_id;
  _get_record(this,&rec);
  pp.record         = mdb_short_record(rec);
  pp.changed        = rec.get_changes();
  pp.normal_value   = rec.get_normal_value();
  wstring str;
  if(erase)
     pp.erase = TLMOBJ_PAINT_FLAG_ERASE;
  else
  {

    if(rec.is_visible() || Fshow_hidden)
    {

      if(flash)
      {
        if(rec.is_flashed())
           pp.erase = TLMOBJ_PAINT_FLAG_ERASE_FLASH;
           else
           return;
      }
      else
      {
       if(rec.is_flashed()  && (flash_counter&1) )
         {
          if(rec.is_discrete())
             return;
         }
      }
      if(pp.erase != TLMOBJ_PAINT_FLAG_ERASE)
      {
          if(rec.sname != (DWORD)-1)
            str       = _get_text(this,rec.sname);
            else
            str.length();
          if(!str.length() && rec.addr.param_number == (DWORD)-1)
          {
           wchar_t text[MAX_PATH];
           swprintf(text,L"%03d.%03d.%03d.%03d",(DWORD)rec.addr.addr.pu,(DWORD)rec.addr.addr.cp,(DWORD)rec.addr.addr.fa,(DWORD)rec.addr.addr.sb);
           str = text;
          }
      }
     pp.short_name = str.data();
    }
    else
    {
       pp.erase = TLMOBJ_PAINT_FLAG_ERASE ;
    }
  }
     painter_dll(PMCMD_PAINT,(LPARAM)&pp,0);

 }

 void         __fastcall TTlmPainter::paint_entry  (HDC dc,DWORD idx,bool erase)
 {
   if(idx<ent_list.size())
     {
      Tentryes_list::iterator ptr = ent_list.begin();
      advance(ptr,idx);
      paint_entry(dc,ptr,is_selected(idx),erase);
     }
 }

 void         __fastcall TTlmPainter::paint_entryes(HDC dc,RECT & r,bool erase)
 {
   Tentryes_list::iterator ptr = ent_list.begin(),end = ent_list.end();
   DWORD   offs = 0;
   KeWin::TRect ur;
   while(ptr<end)
    {
     KeWin::TRect er(*&KeWin::TPoint(ptr->position.x,ptr->position.y),*&KeWin::TSize(ptr->size.cx,ptr->size.cy));
     if(UnionRect(&ur,&r,&er) && !IsRectEmpty(&ur))
      paint_entry(dc,ptr,is_selected(offs),erase);
     ++ptr;
     ++offs;
    }
 }

 int          __fastcall TTlmPainter::Paint(HDC dc,RECT & r,bool erase)
 {

  KeWin::TRect cr = GetClientRect();
  bkgnd.paint(dc,r);
  paint_entryes(dc,r);
  if(captured)
     paint_select_rect(dc,NULL);
  return 0;
 }


 void         __fastcall TTlmPainter::GetWndClass(WNDCLASSEX & wc)
 {
  TWindow::GetWndClass(wc);
  wc.style =CS_BYTEALIGNWINDOW|CS_BYTEALIGNCLIENT|CS_DBLCLKS|CS_OWNDC|CS_VREDRAW|CS_HREDRAW;

 }
 void         __fastcall TTlmPainter::Create
    ( HWND Parent,const char * Caption,RECT & r,int Id ,
      HMENU Menu ,DWORD Style,DWORD ExStyle,const char * ClassName)

{
 if(kadr)
 {
  r.right   = r.left+kadr->kadr_size.cx;
  r.bottom  = r.top+kadr->kadr_size.cy;
 }
 TWindow::Create(Parent,Caption ,r,Id,Menu,Style,ExStyle,ClassName);
}


 DWORD        __fastcall TTlmPainter::GetStyle(bool Ex )
 {
  if(Ex)
    return WS_EX_CLIENTEDGE;
  return WS_CHILD|WS_VISIBLE;
 }

 bool         __fastcall TTlmPainter::OnWMCreate (LPCREATESTRUCT cs)
 {
  mouse_over    = -1;
  mouse_buttons = 0;
  spot_pt.x = spot_pt.y = 0 ;
  spot_pt1 = spot_pt;
  on_down_select= FALSE;
  captured      = FALSE;
  show_grid = this->hide_bkgnd = FALSE;
  grid_step  =  4;
  sel_leader = -1;
  mem_dc   = CreateCompatibleDC((::HDC)ClientDC);
  SetBkColor(GetSysColor(COLOR_BTNFACE));
  if(!painter_dll.is_loaded() && this->paint_dll_name.length())
     painter_dll.load_module(this->paint_dll_name.c_str());
  return TWindow::OnWMCreate(cs);
 }

 HBRUSH       __fastcall TTlmPainter::OnCtlColor(HWND wnd,HDC dc,UINT CtrlType)
 {
   HBRUSH br = TWindow::OnCtlColor(wnd,dc,CtrlType);
   return br;
 }

 void         __fastcall TTlmPainter::OnSize(int how,int cx,int cy)
 {
  //Изменение размеров окна
//  if(bkgnd_bmp)
//    {
//     DeleteObject(bkgnd_bmp);
//     bkgnd_bmp = 0;
//     this->do_setup_bkgnd();
//    }
   bkgnd.set_size(cx,cy);
 }



bool         __fastcall TTlmPainter::set_bkgnd(const wstring & img_name,bool force)
{
  if(bkgnd_image != img_name || force )
  {
   bkgnd_image = img_name ;
   HBITMAP bmp = tlmpaint::load_image(img_name.c_str());
   bkgnd.set_bitmap(bmp,true);
   do_setup_bkgnd();
   Repaint(true);
  }
  return true;
}


void    __fastcall TTlmPainter::release_bkgnd()
{
    bkgnd.set_bitmap(NULL,true);
}

DWORD   __fastcall TTlmPainter::get_bkgnd_mode()
{
 return kadr ? kadr->get_image_mode() : MDB_KADR_FLAG_IMAGE_BKGND_STRETCH;
}



bool    __fastcall TTlmPainter::do_setup_bkgnd()
{
   bkgnd.set_size  (kadr->kadr_size.cx,kadr->kadr_size.cy);
   bkgnd.set_grayed(is_bkgnd_grayed());
   bkgnd.set_mode  (tlmpaint::TBmpBkgnd::bkgnd_mode_t(kadr->get_image_mode()));
   bkgnd.set_grid( this->show_grid ? this->grid_step : 0 );
   return true;
}


bool         __fastcall TTlmPainter::is_bkgnd_grayed()
{
 if(not_gray_bkgnd)
    return false;
 return (!editable && ( !kadr || kadr->kadr_diag) ) ? true:false;
}


 void         __fastcall TTlmPainter::set_getter_text  (getter_text::closure_method method,LPVOID user_data)
 {
   _get_text.SetClosure(user_data,method,this,-1);
   Repaint(true,NULL);
 }

 void         __fastcall TTlmPainter::set_getter_record(getter_record::closure_method method,LPVOID user_data)
 {
  _get_record.SetClosure(user_data,method,this,NULL);
  Repaint(true,NULL);

 }

 struct check_limit:public std::unary_function<DWORD,bool>
 {
  DWORD v;
  check_limit(DWORD _v):v(_v){}
  check_limit(const  check_limit & src){*this = src;}
  check_limit& operator = (const  check_limit & src){v = src.v; return *this;}
  bool operator()(const DWORD & cv) { return cv>=v ? true : false;}

 };


 DWORD        __fastcall TTlmPainter::set_entry(DWORD idx,mdb_kadr_entry & e)
 {
  if(!(idx<ent_list.size()))
      set_entryes_count(idx+1);
   Tentryes_list::iterator ptr = ent_list.begin();
   advance(ptr,idx);
   /*if(is_selected(ptr) && ptr->rec_id != e.rec_id )
      select_entry(idx,true);
   */
   *ptr = e;
   return idx;
 }


 DWORD        __fastcall TTlmPainter::set_entryes_count(DWORD count)
 {
   ent_list.resize(count);
   if(count)
   std::remove_if(sel_list.begin(),sel_list.end(),check_limit(count));
   else
   sel_list.clear();
   Repaint();
   return TRUE;
 }

DWORD __fastcall get_max_enry_number(Tentryes_list & ent_list)
{
  DWORD ret = 0;
  Tentryes_list::iterator ptr = ent_list.begin();
  Tentryes_list::iterator end = ent_list.end  ();
  while(ptr<end)
  {
    if(ret != ptr->number)
     ret = std::max(ret,ptr->number);
     else
     ++ret;
    ++ptr;
  }
  return ret;
}

 DWORD        __fastcall TTlmPainter::add_entry(mdb_kadr_entry & e)
 {
  if(e.number == (DWORD)-1)
     {
      e.number = get_max_enry_number(ent_list);
     }
  ent_list.push_back(e);
  Repaint();
  return ent_list.size();
 }

 DWORD        __fastcall TTlmPainter::remove_entry(mdb_kadr_entry & e)
 {
  Tentryes_list::iterator ptr = ent_list.begin();
  Tentryes_list::iterator end = ent_list.end  ();
  while(ptr<end)
  {
    if(*ptr == e)
      {

       ent_list.erase(ptr);
       break;
      }
      else
      ++ptr;
  }
  Repaint();
  return ent_list.size();
 }

 DWORD        __fastcall TTlmPainter::update_entry(mdb_kadr_entry & e)
 {
  Tentryes_list::iterator ptr = ent_list.begin();
  Tentryes_list::iterator end = ent_list.end  ();

  while(ptr<end)
  {

    if(*ptr == e)
      {
       (*ptr) = e;
        ptr = end;
        HDC dc = this->get_paint_dc();
        //if(!(flash_counter&1))
        bool is_sel = is_selected(ptr);
        paint_entry(dc,ptr,is_sel,false,false);
        refresh(ClientDC);
        return 1;
      }
      else
      ++ptr;
  }
  return 0;
 }


 DWORD        __fastcall TTlmPainter::find_entry(POINT & pt)
 {
  DWORD ret = -1;
  Tentryes_list::reverse_iterator r_beg = this->ent_list.rbegin(),r_end = ent_list.rend();
  while(r_beg<r_end )
  {
   KeWin::TRect r(r_beg->position.x,r_beg->position.y,r_beg->size.cx,r_beg->size.cy);
   if(PtInRect(&r,pt))
     {
      ret = distance(ent_list.rbegin(),r_beg);
      ret = ent_list.size()-ret-1;
      break;
     }
   ++r_beg;
  }
  return ret;
 }


bool __fastcall TTlmPainter::select_entry(DWORD offs,bool added_select)
{
 // возврат true - элемент выделен, false - снят с выделения
 // Выделение / снятие элемента
 // added_select  - накопительное выделение
 // Установка лидирующего элемента в групповой выборке

 bool ret = false;
 Tselected_list::iterator s_beg = sel_list.begin(),s_end = sel_list.end();
 s_beg = lower_bound(s_beg,s_end,offs);
 if(s_beg<s_end && *s_beg == offs)
  {

    if(!added_select)
     {
//      sel_list.clear();
//      sel_leader = -1;
//      select_entry(offs,false);
     }
     else
     {
      sel_list.erase(s_beg);
      if(sel_list.size())
      {
       if(sel_leader == offs)
          sel_leader =  *(s_beg<sel_list.end()? s_beg:sel_list.begin()) ;
      }
      else
      sel_leader = -1;
     }
  }
  else
  {
   if(offs<this->ent_list.size())
     {
       if(!Fshow_hidden)
        {
         mdb_record_ex rec;
         get_record(get_entry_rec_id(offs),rec);
         if(!rec.is_visible())
            return false;
        }

      if(!added_select)
       {
        sel_list.clear();
        s_beg = sel_list.begin();
        sel_leader = offs;
       }
       else
       {
        if(sel_leader == (DWORD)-1)
        sel_leader = offs;
       }
      sel_list.insert(s_beg,offs);
      ret = true;
     }
     else
     {
     sel_list.clear();
     sel_leader = -1;
     }
  }
  Repaint(true);
  _on_select_notify.CallMethod(this,offs);
  return ret;
}


void         __fastcall TTlmPainter::move_size_entry(Tentryes_list::iterator ptr,int dx,int dy,bool sizing)
{
  //Проверить на выход за границы видимости ?

  //1 стереть старое изображение
  KeWin::TRect r1 (ptr->position.x,ptr->position.y,ptr->size.cx,ptr->size.cy);
  bkgnd.paint(get_paint_dc(),r1);
  if(sizing)
  {

   KeWin::TSize new_size(ptr->size.cx + dx,ptr->size.cy + dy);
   KeWin::TSize min_size;

   if(dx<0 || dy <0)
   {
    //Если уменьшение то запретить уменьшаться меньше необходимого
    calc_min_entry_size(ptr,min_size);
    if(min_size.cx<=(new_size.cx))
      ptr->size.cx = new_size.cx;
    if(min_size.cy<=(new_size.cy))
      ptr->size.cy = new_size.cy;
   }
   else
    ptr->size << *&new_size;
  }
  else
  {
   ptr->position.x += dx;
   ptr->position.y += dy;
  }
  KeWin::TRect r2 (ptr->position.x,ptr->position.y,ptr->size.cx,ptr->size.cy);
  //paint_entry(ClientDC,ptr,this->is_selected(ptr));
  _on_moving_sizing(this,distance(ent_list.begin(),ptr));
  KeWin::TRect r;
  UnionRect(&r,&r1,&r2);
  paint_entryes(get_paint_dc(),r);
  refresh(ClientDC);

}

 void         __fastcall TTlmPainter::move_size_selected(int dx,int dy,bool sizing)
 {
  if(this->editable)
  {
   Tselected_list::iterator s_ptr = sel_list.begin(),s_end = sel_list.end();
   while(s_ptr<s_end)
   {
    move_size_entry(*s_ptr,dx,dy,sizing);
    ++s_ptr;
   }
  }
 }

 void         __fastcall TTlmPainter::arrange_left  (mdb_kadr_entry & leader,mdb_kadr_entry & entry)
 {
   int dx =  leader.position.x - entry.position.x;
   move_size_entry(&entry,dx,0,false);

 }

 void         __fastcall TTlmPainter::arrange_top   (mdb_kadr_entry & leader,mdb_kadr_entry & entry)
 {
   int dy = leader.position.y - entry.position.y;
   move_size_entry(&entry,0,dy,false);
 }

 void         __fastcall TTlmPainter::arrange_right (mdb_kadr_entry & leader,mdb_kadr_entry & entry)
 {
  int dx =  leader.position.x + leader.size.cx - entry.size.cx -entry.position.x;
  move_size_entry(&entry,dx,0,false);
 }

 void         __fastcall TTlmPainter::arrange_bottom(mdb_kadr_entry & leader,mdb_kadr_entry & entry)
 {
  int dy =  leader.position.y + leader.size.cy - entry.size.cy - entry.position.y ;
  move_size_entry(&entry,0,dy,false);
 }

 void         __fastcall TTlmPainter::arrange_size  (mdb_kadr_entry & leader,mdb_kadr_entry & entry,how_arrange ha)
 {
  KeWin::TSize min_size;
  KeWin::TSize size(leader.size.cx,leader.size.cy);
  calc_min_entry_size(&entry,min_size);
  int dx = 0;
  int dy = 0;

  switch(ha)
  {
   case arr_size_width:
        if(size.cx>=min_size.cx)
          dx = size.cx-entry.size.cx;
   break;
   case arr_size_height:
        if(size.cy>=min_size.cy)
          dy = size.cy-entry.size.cy;
   break;
   case arr_size_both:
        if(size.cx>=min_size.cx)
          dx = size.cx-entry.size.cx;
        if(size.cy>=min_size.cy)
          dy = size.cy-entry.size.cy;
   break;
  }
  if(dx || dy)
     this->move_size_entry(&entry,dx,dy,true);
 }


void         __fastcall TTlmPainter::arrange_selected(how_arrange ha)
{
  if(this->editable && sel_list.size()>1)
  {
  Tselected_list::iterator s_ptr = sel_list.begin(),s_end = sel_list.end();
  Tentryes_list::iterator  e_ptr = ent_list.begin();
  advance(e_ptr,sel_leader); // Все операции выполняются по лидирующему объекту
  while(s_ptr<s_end)
   {
    if(*s_ptr != sel_leader)
    {
      switch(ha)
      {
      case arr_left        :
         arrange_left(*e_ptr,ent_list.at(*s_ptr));
      break;
      case arr_top         :
         arrange_top(*e_ptr,ent_list.at(*s_ptr));
      break;
      case arr_right       :
         arrange_right(*e_ptr,ent_list.at(*s_ptr));
      break;
      case arr_bottom      :
         arrange_bottom(*e_ptr,ent_list.at(*s_ptr));
      break;
      case arr_size_width  :
           arrange_size(*e_ptr,ent_list.at(*s_ptr),arr_size_width);
      break;
      case arr_size_height :
           arrange_size(*e_ptr,ent_list.at(*s_ptr),arr_size_height);
      break;
      case arr_size_both   :
           arrange_size(*e_ptr,ent_list.at(*s_ptr),arr_size_both);

      break;
      }
    }
    ++s_ptr;
   }
  this->Repaint(true);
  }
}

 void         __fastcall TTlmPainter::clear_entryes()
 {
  select_entry(-1,false);
  ent_list.clear();
  Repaint(true);
 }

bool  __fastcall TTlmPainter::get_kadr_entry(DWORD offs,mdb_kadr_entry & ke)
{
 if(offs<ent_list.size())
   {
    ke = ent_list.at(offs);
    return true;
   }
   ke = mdb_kadr_entry();
   return false;

}

 void         __fastcall TTlmPainter::get_record(DWORD rec_id,mdb_record_ex & rec)
 {
   rec.rec_id = rec_id;
   _get_record(this,&rec);
 }

 wstring      __fastcall TTlmPainter::get_wtext(DWORD id)
 {
   wstring ret ;
   ret = _get_text(this,id);
   return ret;
 }

void   __fastcall TTlmPainter::flash_repaint()
{
 if(!this->editable)
 {
   KeWin::TRect cr = this->GetClientRect();
   HDC dc = get_paint_dc();
   Tentryes_list::iterator ptr = ent_list.begin();
   Tentryes_list::iterator end = ent_list.end();
   while(ptr<end)
   {

    paint_entry(dc,ptr,is_selected(ptr),false,(flash_counter&1) ? true:false);
    ++ptr;
   }
   refresh(ClientDC);
 }
}

DWORD        __fastcall TTlmPainter::get_selected()
{
 if(get_selected_count())
    return *sel_list.begin();
 else
 return (DWORD)-1;

}

DWORD        __fastcall TTlmPainter::get_selected(Tselected_list & dst)
{
 std::copy(sel_list.begin(),sel_list.end(),std::back_inserter(dst));
 return dst.size();
}



void    __fastcall TTlmPainter::set_selected(Tselected_list & src,bool repaint)
{
  sel_list.clear();
  std::copy(src.begin(),src.end(),back_inserter(sel_list));
  if(repaint)
     Repaint();
}

 void         __fastcall TTlmPainter::select_all  (bool repaint )
 {
    select_clean(false);
    for(DWORD i = 0;i<this->ent_list.size() ;i++)
    {
      select_entry(i,true);
    }
  if(repaint)
     Repaint();
 }

void         __fastcall TTlmPainter::select_discrete(bool repaint )
{
    select_clean(false);
    Tentryes_list::iterator ptr = ent_list.begin();
    for(DWORD i = 0;i<this->ent_list.size() ;i++)
    {
      mdb_record_ex rec;
      get_record(ptr->rec_id,rec);
      if(rec.addr.addr.fa == OTD_FA_DISCRETE)
          select_entry(i,true);
      ++ptr;
    }
  if(repaint)
     Repaint();
 }



void         __fastcall TTlmPainter::select_analogs (bool repaint )
{
    select_clean(false);
    Tentryes_list::iterator ptr = ent_list.begin();
    for(DWORD i = 0;i<this->ent_list.size() ;i++)
    {
      mdb_record_ex rec;
      get_record(ptr->rec_id,rec);
      if(rec.addr.addr.fa != OTD_FA_DISCRETE)
          select_entry(i,true);
      ++ptr;
    }
  if(repaint)
     Repaint();
}


void         __fastcall TTlmPainter::select_clean(bool repaint )
{
   sel_list.clear();
   if(_on_select_notify.HaveMethod())
      _on_select_notify(this,-1);
   if(repaint)
      Repaint();
}

DWORD        __fastcall TTlmPainter::get_entry_rec_id  (DWORD entry_num)
{
  if(entry_num<this->ent_list.size())
     return ent_list.at(entry_num).rec_id;
     else
     return (DWORD)-1;
}

void         __fastcall TTlmPainter::SetFlags(DWORD32 flags, bool Set)
{
   TWindow::SetFlags(flags,Set);
   if(flags&FLWND_DOUBLE_BUFFERED)
     {
        release_dbl_buffered();
        if(Set)
           create_dbl_buffered();
        Repaint();
     }
}

 bool         __fastcall TTlmPainter::lower_entry(DWORD rec_id,DWORD number, Tentryes_list::iterator & ptr)
 {
  mdb_kadr_entry ke(this->kadr->kadr_id,rec_id,number);
  ptr = std::lower_bound (ent_list.begin(),ent_list.end(),ke);
  return (ptr->rec_id == rec_id && ptr->number == number) ? true:false;
 }

 DWORD        __fastcall TTlmPainter::range_entry(DWORD rec_id,Tentryes_list::iterator & lo_ptr,Tentryes_list::iterator & hi_ptr)
 {
   if(kadr)
   {
    mdb_kadr_entry ke(this->kadr->kadr_id,rec_id,0);
    lo_ptr    = std::lower_bound (ent_list.begin(),ent_list.end(),ke);
    ke.number = (DWORD)-1;
    hi_ptr    = std::upper_bound(lo_ptr,ent_list.end(),ke);
    return std::distance(lo_ptr,hi_ptr);
   }
   else
   return 0;
 }


void         __fastcall TTlmPainter::repaint_record(DWORD rec_id)
{
 Tentryes_list::iterator  lo_ptr,hi_ptr;
 if(range_entry(rec_id,lo_ptr,hi_ptr))
 {
     HDC dc = get_paint_dc();
     while(lo_ptr<hi_ptr)
     {
       paint_entry(dc,lo_ptr,is_selected(lo_ptr),false);
       ++lo_ptr;
     }
   refresh(ClientDC);
 }
}

 void         __fastcall TTlmPainter::show_hidden(BOOL show)
 {
   if(Fshow_hidden!=show)
   {
     Fshow_hidden = show;
     if(!show)
        {
         //Снять с выделения скрытые
         Tselected_list::iterator ptr = sel_list.begin(),end = sel_list.end();
         mdb_record_ex rec;

         while(ptr<end)
         {
            DWORD id = *ptr;
            id = ent_list.at(id).rec_id;
            get_record(id,rec);
            if(!rec.is_visible())
            {
             sel_list.erase(ptr);
             end = sel_list.end();
            }
            else
            ++ptr;
         }
        }
     Repaint();
   }
 }

 void         __fastcall TTlmPainter::set_not_gray_bkgnd(BOOL ngray)
 {
   if(not_gray_bkgnd != ngray)
   {
      not_gray_bkgnd = ngray;
      bkgnd.set_grayed( is_bkgnd_grayed() );
      Repaint(true);
   }
 }

 void         __fastcall add_one(DWORD & val, bool plus,DWORD limit)
 {

   if(plus)
   {
     if((++val)>limit) val = 0;
   }
   else
   {
    if((--val) == (DWORD)-1) val = limit;
   }
 }

 void         __fastcall TTlmPainter::select_next(bool next)
 {
  if(this->ent_list.size())
   {
     DWORD offs = 0;
     if(get_selected_count())
        offs = get_selected();
     DWORD limit = ent_list.size();
     for(DWORD i = 0;i<ent_list.size();i++)
     {
       add_one(offs,next,limit-1);
       if(select_entry(offs,false)) break;
     }
   }
 }




