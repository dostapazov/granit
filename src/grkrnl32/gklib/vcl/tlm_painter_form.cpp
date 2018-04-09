//---------------------------------------------------------------------------
#pragma hdrstop
#include <vcl.h>
#include "tlm_painter_form.h"
#include <vector_index.hpp>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"


TTlmPainterForm *TlmPainterForm;
//---------------------------------------------------------------------------
__fastcall TTlmPainterForm::TTlmPainterForm(TComponent* Owner)
	: TForm(Owner)

{
  Feditable        = false;
  Fcurrent_over    = -1;
  Fcaptured        = false;
  DoubleBuffered   = true;
  AlphaBlend       = false;
  Fkadr            = NULL;
  Fon_get_record   = NULL;
  Fon_get_text     = NULL;
  paint_context    = 0;
  Ffixed           = false;
  Fmulty_selected  = true;
  Fsel_rect_lt.x   =  Fsel_rect_lt.y  = Fsel_rect_br.x =  Fsel_rect_br.y = 0;
  Fflash_counter   = 0;
  Fpaint_locker    = 0;
}
//---------------------------------------------------------------------------

bool      __fastcall TTlmPainterForm::get_record   (DWORD rec_id,MDB_RECORD & rec,bool * rec_visible) const
{
  if(!on_get_record || !on_get_record(rec_id,rec,rec_visible ))
     {
      rec.diag = -1;
      if(rec_visible) *rec_visible = false;
      return false;

     }
     return true;

}
//---------------------------------------------------------------------------

  UnicodeString    __fastcall TTlmPainterForm::get_text     (DWORD str_id) const
  {
    if(on_get_text)
       return on_get_text(str_id);
    return UnicodeString();
  }
//---------------------------------------------------------------------------

  const wchar_t * __fastcall TTlmPainterForm::get_name     (MDB_RECORD & rec,bool short_name) const
  {
    const_cast<TTlmPainterForm*>(this)->_gts = get_text(short_name ? rec.sname : rec.name);
    return _gts.c_str();
  }
//---------------------------------------------------------------------------
UnicodeString   __fastcall TTlmPainterForm::get_record_name(DWORD  rec_id,bool short_name) const
{
    UnicodeString str;
    mdb_record rec;
    if(get_record(rec_id,rec,NULL))
      str = get_name(rec,short_name);
    return str;
}
//---------------------------------------------------------------------------

UnicodeString   __fastcall TTlmPainterForm::get_kadr_caption ()
{
  UnicodeString str;
  if(Fkadr)
  {
    str = get_text(Fkadr->kadr.name_id);
    if(str.IsEmpty())
       str = get_text(Fkadr->kadr.sname_id);
    if(str.IsEmpty())
       str.printf(L"Кадр  %d [Без имени]",Fkadr->kadr.kadr_id);
  }
  else
  str = L"Unassigned";
  return str;
}
//---------------------------------------------------------------------------

UnicodeString __fastcall TTlmPainterForm::get_image_file()
{
 return get_text(Fkadr->kadr.image_id);
}
//---------------------------------------------------------------------------

void      __fastcall TTlmPainterForm::reset_kadr  (DWORD mask)
{
  Caption = get_kadr_caption();
  if(Fkadr)
  {
    if(mask & MDB_KADR_FIELD_FLAGS)
     set_bk_mode  (tlmpaint::TBmpBkgnd::bkgnd_mode_t(Fkadr->kadr.kadr_flags&MDB_KADR_FLAG_IMAGE_BKGND_MASK));
    if(mask & MDB_KADR_FIELD_IMAGE)
     set_bk_image (get_image_file());
    if(mask & MDB_KADR_FIELD_SIZE)
    {
     set_kadr_size(Fkadr->kadr.kadr_size.cx,Fkadr->kadr.kadr_size.cy);
     Fkadr->position_index_build();
    }
  }
  refresh_kadr();
}
//---------------------------------------------------------------------------

void      __fastcall TTlmPainterForm::set_kadr     (single_kadr * kadr)
{
    if(Fkadr!=kadr)
      Fkadr = kadr;
    reset_kadr();
}
//---------------------------------------------------------------------------

void __fastcall  TTlmPainterForm::set_kadr_size(int cx,int cy)
{
  int dx = ClientWidth  - paint_area->ClientWidth;
  int dy = ClientHeight - paint_area->ClientHeight;
  ClientWidth   = cx +   dx;
  ClientHeight  = cy +   dy;
}
//---------------------------------------------------------------------------

void __fastcall TTlmPainterForm::CreateParams(TCreateParams & params)
{
  TForm::CreateParams(params);
  //params.WindowClass.style |= CS_BYTEALIGNWINDOW|CS_BYTEALIGNCLIENT|CS_DBLCLKS|CS_OWNDC|CS_VREDRAW|CS_HREDRAW;
}
//---------------------------------------------------------------------------

TBitmap * __fastcall TTlmPainterForm::get_bitmap()
{
   if(!bk_bmp.get())
       bk_bmp.reset(new TBitmap);
    return bk_bmp.get();
}
//---------------------------------------------------------------------------

void __fastcall TTlmPainterForm::FormResize(TObject *Sender)
{
   bkgnd.set_size(paint_area->Width,paint_area->Height);
   refresh_kadr();
}
//---------------------------------------------------------------------------

void      __fastcall TTlmPainterForm::set_bk_grid  (int grid_size)
{
   bkgnd.set_grid(grid_size);
   refresh_kadr();
}
//---------------------------------------------------------------------------

void __fastcall TTlmPainterForm::set_bk_mode(tlmpaint::TBmpBkgnd::bkgnd_mode_t mode)
{
 bkgnd.set_mode(mode);
 paint_area->Repaint() ;
}
//---------------------------------------------------------------------------

void __fastcall TTlmPainterForm::set_bk_image(UnicodeString image_name)
{
    HBITMAP   bmp  = tlmpaint::load_image(image_name.c_str());
    bkgnd.set_bitmap(bmp);
    refresh_kadr();
}
//---------------------------------------------------------------------------

void __fastcall TTlmPainterForm::paint_selected_entryes(bool erase,bool flash_erase)
{
   paint_action_t paint_action(paint_context,paint_area->Canvas->Handle,paint_area->Canvas->Font->Handle,bkgnd(),erase,flash_erase,this);
   index_paint_action_t pa(Fkadr->entryes,paint_action);
   std::for_each(Fkadr->selected.begin(),Fkadr->selected.end(),pa);
   paint_area->Update();
}
//---------------------------------------------------------------------------

void __fastcall TTlmPainterForm::paint_entryes(bool erase,bool flash_erase)
{
   if(Fkadr)
   {
    tlmpaint::paint_action_t<TTlmPainterForm>
    paint_action(paint_context,paint_area->Canvas->Handle,paint_area->Canvas->Font->Handle,bkgnd(),erase,flash_erase,this);
    std::for_each(Fkadr->entryes.begin(),Fkadr->entryes.end(),paint_action);
   }
   paint_area->Update();
}
//---------------------------------------------------------------------------
void __fastcall  TTlmPainterForm::set_flash_counter     (DWORD  fc)
{
   if(Fflash_counter != fc)
      {
       Fflash_counter = fc;
       paint_area->Repaint();
      }

}

void __fastcall TTlmPainterForm::paint_bkgnd ()
{
   RECT r;
   SetRect(&r,0,0,paint_area->Width,paint_area->Height);
   bkgnd.paint(paint_area->Canvas->Handle,r);
}

void __fastcall TTlmPainterForm::paint_areaPaint(TObject *Sender)
{
   paint_bkgnd();
   bool flash_erase = ((Fflash_counter&1) && ((Fkadr->ready_state & rds_state_flashing) || Fkadr->sel_count()) ) ? true : false;
   paint_entryes(false,flash_erase);
   if(Fsel_rect_br.x && Fsel_rect_br.y)
      paint_select_rect(paint_area->Canvas->Handle,Fsel_rect_lt,&Fsel_rect_br,NULL);

}
//---------------------------------------------------------------------------

void __fastcall TTlmPainterForm::update_status_bar ()
{
  if(Fkadr)
    {
      mdb_kadr_entry * entry ;
      if(current_over < Fkadr->entryes.size() )
         entry = &Fkadr->entryes.at(current_over);
         else
          entry = Fkadr->get_last_selected();
      show_entry_info(entry);
    }
}
//---------------------------------------------------------------------------
void  __fastcall TTlmPainterForm::set_captured(bool val)
{
    if(captured != val)
    {
      if(val)
         SetCapture(Handle);
      else
         {
          SetCapture(NULL);
          paint_area->Repaint();
         }
      Fcaptured = val;

    }
}
//---------------------------------------------------------------------------

void __fastcall TTlmPainterForm::do_move_selected(int dx,int dy)
{
  if(Fkadr && Fkadr->sel_count())
  {
      paint_selected_entryes(true,false);
      index_move_action_t ma(Fkadr->entryes,*&tlmpaint::move_action_t(dx,dy));
      for_each(Fkadr->selected.begin(),Fkadr->selected.end(),ma);
      paint_entryes(false,false);
  }
}
//---------------------------------------------------------------------------

void __fastcall TTlmPainterForm::do_size_selected(int dx,int dy)
{
  if(Fkadr &&  Fkadr->sel_count())
  {
   index_size_action_t sa(Fkadr->entryes,*&tlmpaint::size_action_t(dx,dy));
   for_each(Fkadr->selected.begin(),Fkadr->selected.end(),sa);
  }
}
//---------------------------------------------------------------------------

void __fastcall  TTlmPainterForm::set_current_over(DWORD over)
{
 if(Fcurrent_over != over)
   {
    Fcurrent_over  = over;
    update_status_bar();
   }
}
//---------------------------------------------------------------------------

 void         __fastcall TTlmPainterForm::paint_select_rect(HDC dc,POINT & spot_pt,LPPOINT  new_pt,LPPOINT old_pt )
 {
     int old_rop = ::SetROP2(dc ,R2_NOTXORPEN);
     if(old_pt)
     {
      MoveToEx(dc,spot_pt.x,spot_pt.y,NULL);
      LineTo  (dc,old_pt->x,spot_pt.y );
      LineTo  (dc,old_pt->x,old_pt->y);
      LineTo  (dc,spot_pt.x ,old_pt->y);
      LineTo  (dc,spot_pt.x ,spot_pt.y );
     }

     if(new_pt)
     {
     MoveToEx(dc,spot_pt.x ,spot_pt.y,NULL);
     LineTo  (dc,new_pt->x,spot_pt.y );
     LineTo  (dc,new_pt->x,new_pt->y);
     LineTo  (dc,spot_pt.x ,new_pt->y);
     LineTo  (dc,spot_pt.x ,spot_pt.y );
     }

     SetROP2(dc,old_rop);

 }
//---------------------------------------------------------------------------

void __fastcall  TTlmPainterForm::captured_mouse_move( TShiftState Shift,int X, int Y)
{
    if(mstate.mb_left.is_down())
    {
    if(current_over!=DWORD(-1))
    {
     if(Feditable)
     {
     int dx = mstate.mb_left.get_dx(X);
     int dy = mstate.mb_left.get_dy(Y);
     mstate.mb_left.set_state(true,X,Y);
     if(Shift.Contains(ssShift))
       {
       do_size_selected(dx,dy);
       refresh_kadr();
       }
       else
       {
        do_move_selected (dx,dy);
       }
      }
     }
     else
     {
      Fsel_rect_lt = mstate.mb_left.get_switch_spot();
      POINT old_pt = mstate.mb_left.get_spot();
      mstate.mb_left.set_state(true,X,Y);
      Fsel_rect_br = mstate.mb_left.get_spot();
      paint_select_rect(paint_area->Canvas->Handle,Fsel_rect_lt,&Fsel_rect_br,&old_pt);
     }
    }

}
//---------------------------------------------------------------------------

void __fastcall TTlmPainterForm::on_mouse_move(TShiftState Shift,int X, int Y)
{
  if(!Fkadr) return;
  if(captured)
     captured_mouse_move(Shift,X,Y);
  else
  {
   if(mstate.mb_left.is_down())
   {
     int capt_delta = get_captured_delta();
     if(abs(mstate.mb_left.get_dx(X))>capt_delta  || abs(mstate.mb_left.get_dy(Y))>capt_delta )
     {
        captured = true;
        if(current_over!=DWORD(-1))
        {
          if(!Shift.Contains(ssCtrl))
             select_entry(-1,sel_mode_t::clear);
             select_entry(current_over,sel_mode_t::select);
        }
        captured_mouse_move(Shift,X,Y); return;
     }
   }
   DWORD over = Fkadr->get_at_position(X,Y);
   set_current_over(over);
  }
}
//---------------------------------------------------------------------------

void __fastcall TTlmPainterForm::paint_areaMouseMove(TObject *Sender, TShiftState Shift,
          int X, int Y)
{
  on_mouse_move(Shift,X,Y);
}
//---------------------------------------------------------------------------



void __fastcall TTlmPainterForm::on_mouse_down(TMouseButton Button,TShiftState Shift, int X, int Y)
{
  if(!Fkadr) return;
  if(Button == mbLeft)
  {
   mstate.mb_left.set_state(true,X,Y);
  }

}

void __fastcall TTlmPainterForm::paint_areaMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y)
{
  on_mouse_down(Button,Shift,X,Y);
}
//---------------------------------------------------------------------------
void      __fastcall  TTlmPainterForm::select_entry    (DWORD offset,sel_mode_t sel_mode)
{
  if(sel_mode == sel_mode_t::clear)
     {
      paint_lock();
      while(Fkadr->sel_count())
      {
        offset = Fkadr->get_last_selected_offset();
        select_entry                (offset,sel_mode_t::deselect);
      }
      paint_unlock();
      return;
     }

  kadr_entryes_table::iterator ptr ;
  if( Fkadr->get_by_offset(offset,ptr))
    {
     switch(sel_mode)
     {
      case sel_mode_t::toggle   : Fkadr->toggle_select(offset);break;
      case sel_mode_t::select   : /*if(!ptr->is_selected())*/      Fkadr->select(offset) ;break;
      case sel_mode_t::deselect : /*if( ptr->is_selected())*/      Fkadr->deselect(offset);break;
     }
     select_changed         (offset,ptr->is_selected());
    }
}
//---------------------------------------------------------------------------

void      __fastcall  TTlmPainterForm::select_in_rect (bool add)
{
    paint_lock();

   if(!add)
       selections_clear();
   RECT r;
   int x1 = Fsel_rect_lt.x;
   int x2 = Fsel_rect_br.x;
   int y1 = Fsel_rect_lt.y;
   int y2 = Fsel_rect_br.y;
   if(x2<x1) std::swap(x1,x2);
   if(y2<y1) std::swap(y1,y2);
   SetRect(&r,x1,y1,x2,y2);

   kadr_entryes_table::iterator ptr = Fkadr->entryes.begin();
   kadr_entryes_table::iterator end = Fkadr->entryes.end  ();
   while(ptr<end)
   {
     POINT pt1,pt2;
     pt1.x = pt2.x = ptr->position.x;
     pt1.y = pt2.y = ptr->position.y;
     pt2.x += ptr->size.cx;
     pt2.y += ptr->size.cy;
     if(PtInRect(&r,pt1) || PtInRect(&r,pt2))
        {
         if(is_entry_selectable(&(*ptr)))
            select_entry(Fkadr->get_offset(ptr),sel_mode_t::select);
        }
     ++ptr;
   }
  paint_unlock();
}
//---------------------------------------------------------------------------
void __fastcall TTlmPainterForm::on_mouse_up(TMouseButton Button,TShiftState Shift, int X, int Y)
{
  if(!Fkadr) return;
  if(Button == mbLeft)
  {
    if(captured )
    {
     captured = false;
     if(current_over == DWORD(-1)  )
     {

      Fsel_rect_br.x = X;Fsel_rect_br.y = Y;
      select_in_rect(Shift.Contains(ssCtrl) ? true : false);
      Fsel_rect_lt.x =  Fsel_rect_lt.y  = Fsel_rect_br.x =  Fsel_rect_br.y = 0;
     }
     else
     Fkadr->position_index_resort() ;
     paint_area->Repaint();
    }
    else
    {
      if(current_over!=(DWORD)-1 )
      {
        bool ctrl = Shift.Contains(ssCtrl);
        if(ctrl)
        {
            select_entry(current_over,sel_mode_t::toggle);
        }
        else
        {
         if(Fkadr->is_selected(current_over))
           {
            if(Fkadr->sel_count() == 1)
               select_entry(current_over,sel_mode_t::deselect);//Fkadr->deselect(current_over);
               else
               {
                selections_clear();
                select_entry(current_over,sel_mode_t::select);//Fkadr->select(current_over);
               }
           }
            else
            {
              selections_clear();
              select_entry(current_over,sel_mode_t::select);//Fkadr->select(current_over);
            }
        }
        refresh_kadr();
      }
      else
      {
        if(Fkadr->sel_count())
        {
          selections_clear();
        }
      }
    }
    mstate.mb_left.set_state(false,X,Y);
  }
  else
  captured = false;


}
//---------------------------------------------------------------------------

void __fastcall TTlmPainterForm::paint_areaMouseUp(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y)
{
 on_mouse_up(Button,Shift,X,Y);
}
//---------------------------------------------------------------------------

void __fastcall TTlmPainterForm::FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)

{

  int dx = 0,dy = 0;
  bool shift = Shift.Contains(ssShift);
  bool alt   = Shift.Contains(ssAlt  );
  bool ctrl  = Shift.Contains(ssCtrl );

  switch(Key)
  {
    case VK_LEFT   :  dx = -1; break;
    case VK_RIGHT  :  dx =  1; break;
    case VK_UP     :  dy = -1; break;
    case VK_DOWN   :  dy =  1; break;
    case L'A':case L'a':case L'ф':case L'Ф':
    if(ctrl && !shift)
       miSelectAllClick(miSelectAll);
    if(!ctrl && shift)
       miSelectAllClick(this->miInvertSelect);
    break;
    case L'D':case L'd':case L'в':case L'В':
    if(ctrl && !shift)
       miSelectDiscreteClick(miSelectDiscrete);
    if(!ctrl && shift)
       miSelectDiscreteClick(miSelectDiscreteInvert);
    break;
    case L'I':case L'i':case L'ш':case L'Ш':
    if(ctrl && !shift)
       miSelectAnalogsClick(miSelectAnalogs);
    if(!ctrl && shift)
       miSelectAnalogsClick(miSelectAnalogsInvert);
    break;

  }

  if(Fkadr && Fkadr->get_entryes_count() && ( dx+dy ))
  {
    if(Feditable && (shift || alt))
    {
        if(bkgnd.get_grid()) {dx*=bkgnd.get_grid();dy*=bkgnd.get_grid();}
        if(alt)
            do_move_selected(dx,dy);
        else
        {
        if(shift)
            do_size_selected(dx,dy);
        }
         Fkadr->position_index_resort();
    }
    else
    {
     bool ctrl = Shift.Contains(ssCtrl);
     DWORD prev_sel = Fkadr->get_last_selected_offset();
     DWORD over = prev_sel;
     mdb_kadr_entry * entry;
     int count = 0;
     do{
        over = Fkadr->get_next(over,(dx+dy)>0);
        entry = Fkadr->get_by_offset(over);
        ++count;
       }while(!is_entry_selectable(entry) && count<(int)Fkadr->get_entryes_count());
       current_over = over;
     if(!ctrl && prev_sel!=(DWORD)-1)
         select_entry(prev_sel,sel_mode_t::deselect);
         select_entry(over,sel_mode_t::select);
     //select_changed(current_over,true);
    }
     refresh_kadr();
  }
}
//---------------------------------------------------------------------------

void __fastcall TTlmPainterForm::WndProc(TMessage & msg)
{
   switch(msg.Msg)
     {
      case WM_RESET_KADR  :reset_kadr(msg.WParam);
                           break;
      case WM_SHOW_REFRESH:refresh_kadr();
                           msg.Result = 0;
                           break;
      case WM_FLASH_KADR:  set_flash_counter(msg.WParam);
      break;
      case WM_SYSCOMMAND:
         switch(msg.WParam)
         {
           case SC_MOVE  :
           case SC_MOVE|2:
           case SC_CLOSE :
           case SC_SIZE  :    if(Ffixed) return;
           default       :    TForm::WndProc(msg); break;
         }
         break;
      default: TForm::WndProc(msg);
      break;
     }

}

//---------------------------------------------------------------------------

void __fastcall TTlmPainterForm::FormShow(TObject *Sender)
{
  PostMessage(Handle,WM_SHOW_REFRESH,0,0);
}
//---------------------------------------------------------------------------

class select_action_t
{
  TTlmPainterForm & form;
  TTlmPainterForm::sel_mode_t sel_mode;
  bool discrete;
  bool all;
  DWORD offset;
  public:
  select_action_t(TTlmPainterForm & _form,TTlmPainterForm::sel_mode_t _sel_mode,
                  bool _discrete,bool _all)
                  :form(_form),sel_mode(_sel_mode),discrete(_discrete),all(_all),offset(0)
  {}
  select_action_t(const select_action_t & src):form(src.form),sel_mode(src.sel_mode),discrete(src.discrete),all(src.all),offset(src.offset)
  {}

  void operator ()(const mdb_kadr_entry & e);

};
 void select_action_t::operator ()(const mdb_kadr_entry & e)
 {
   mdb_record rec;
   bool is_visible;
   form.get_record(e.rec_id,rec,&is_visible);
   if(is_visible)
     {
       if(all || discrete == rec.is_discrete())
          form.select_entry(offset,sel_mode);
     }
   ++offset;
 }
//---------------------------------------------------------------------------

void __fastcall TTlmPainterForm::miSelectAllClick(TObject *Sender)
{
  paint_lock();
  sel_mode_t sm   = (Sender == miInvertSelect) ? sel_mode_t::toggle : sel_mode_t::select;
  select_action_t sel_action(*this,sm,true,true);
  std::for_each(Fkadr->entryes.begin(),Fkadr->entryes.end(),sel_action);
  paint_unlock();
}
//---------------------------------------------------------------------------

void __fastcall TTlmPainterForm::miSelectDiscreteClick(TObject *Sender)
{
  paint_lock();
  sel_mode_t sm = (Sender == miSelectDiscreteInvert) ? sel_mode_t::toggle : sel_mode_t::select;
  select_action_t sel_action(*this,sm,true,false);
  std::for_each(Fkadr->entryes.begin(),Fkadr->entryes.end(),sel_action);
  paint_unlock();
}
//---------------------------------------------------------------------------

void __fastcall TTlmPainterForm::miSelectAnalogsClick(TObject *Sender)
{
  paint_lock();
  sel_mode_t sm = (Sender == miSelectAnalogsInvert) ? sel_mode_t::toggle : sel_mode_t::select;
  select_action_t sel_action(*this,sm,false,false);
  std::for_each(Fkadr->entryes.begin(),Fkadr->entryes.end(),sel_action);
  paint_unlock();
}
//---------------------------------------------------------------------------


