//---------------------------------------------------------------------------

#pragma hdrstop

#include <vcl.h>
#include <dibs.h>
#include "kadr_painter.h"

#include <functional>
#include <algorithm>

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TKadrPainter *KadrPainter;
//---------------------------------------------------------------------------
__fastcall TKadrPainter::TKadrPainter(TComponent* Owner)
        : TForm(Owner)
{
 Fdiag_off     = false;
 painter_dll   = NULL;
 Fkadr_data    = NULL;
 grayed_bkgnd  = FALSE;
 Fgrid_step     = 4;
 get_text      = 0;
 get_name      = 0;
 get_record    = 0;
 mem_dc        = CreateCompatibleDC(Canvas->Handle);

 flash_counter = 0;
 Fover_entry    = 0;
 Fon_select_entry = NULL;
 Fon_over_entry   = NULL;
 Fset_kadr_change = NULL;


 rmb_down =  lmb_down = false;
 is_captured = false;
 select_on_down = false;
 Fhide_bkgnd = FALSE;

 show_grid  = FALSE;
 mouse_sel_mode = FALSE;

  painter_dll = tlmpaint::painter_dll::get_instance();
}

void __fastcall TKadrPainter::CreateParams(Vcl::Controls::TCreateParams &Params)
{
  TForm::CreateParams(Params);
  //Params.Style   |=WS_CHILD|WS_TABSTOP;
  //Params.ExStyle = WS_EX_CONTROLPARENT;
}

WideString         __fastcall TKadrPainter::stub_get_text(DWORD rid)        {return WideString();}
WideString         __fastcall TKadrPainter::stub_get_name(DWORD rid,bool )  {return WideString();}

//---------------------------------------------------------------------------

DWORD    __fastcall TKadrPainter::get_changes()
{
  return  0;
}

void     __fastcall TKadrPainter::set_changes(DWORD mask)
{
  if(Fkadr_data && set_kadr_change)
     set_kadr_change(Fkadr_data->kadr.kadr_id,mask);
}


void     __fastcall TKadrPainter::set_get_text  (get_text_method   __gm)
{
 if(!__gm)
    __gm = this->stub_get_text;
 if(Fget_text!=__gm)
    Fget_text= __gm;

}

void     __fastcall TKadrPainter::set_get_name  (get_name_method   __gm)
{
 if(!__gm)
    __gm = this->stub_get_name;
 if(Fget_name!=__gm)
    Fget_name= __gm;
}

void     __fastcall TKadrPainter::set_get_record(get_record_method __gm)
{
 if(!__gm)
    __gm = this->stub_get_record;
 if(Fget_record!=__gm)
    Fget_record= __gm;

}



void     __fastcall TKadrPainter::set_kadr_data    (single_kadr * kd)
{
  Fover_entry = 0;
  if(on_over_entry)
    on_over_entry(over_entry);
  if(Fkadr_data    != kd)
  {
   Fkadr_data     = kd;
   //TODO Загрузка фонового изображения
   bkgnd_image  = kd ? get_text(kadr_data->kadr.image_id):WideString();

  }
}

void  __fastcall TKadrPainter::reload_bkgnd_image()
{
  AnsiString img_name = Fbkgnd_image;
  Fbkgnd_image = AnsiString();
  release_bkgnd();
  bkgnd_image = img_name;


}

void     __fastcall TKadrPainter::set_bkgnd_image( UnicodeString img)
{
 if(Fbkgnd_image!=img)
 {
  Fbkgnd_image = img;
  HBITMAP bmp  = tlmpaint::load_image(img.c_str());
  bkgnd.set_bitmap(bmp);
  bkgnd.set_size(Width,Height);
  if(kadr_data)
     bkgnd.set_mode(tlmpaint::TBmpBkgnd::bkgnd_mode_t(this->kadr_data->kadr.kadr_flags&MDB_KADR_FLAG_IMAGE_BKGND_MASK) );
 }
  Repaint();
}

 TSize __fastcall get_bitmap_size(HBITMAP hBmp)
 {
     BITMAP  _bmp;
     GetObject(hBmp,sizeof(_bmp),&_bmp);
     TSize ret;
     ret.cx = _bmp.bmWidth;
     ret.cy = _bmp.bmHeight;
     return ret;
 }


 bool         __fastcall TKadrPainter::is_bkgnd_grayed()
 {
  return grayed_bkgnd ? true:false;
 }

TSize   __fastcall calc_img_size(TSize & img_sz,TSize & wnd_sz)
{
 TSize  ret;

 float kw = (float)wnd_sz.cx/(float)img_sz.cx;
 float kh = (float)wnd_sz.cy/(float)img_sz.cy;
 float k = KeRTL::MIN(kw,kh);
 ret.cx = k*(float)img_sz.cx; // Преобразовали к масштабу окна
 ret.cy = k*(float)img_sz.cy;
 return ret;
}


 DWORD      __fastcall TKadrPainter::get_bkgnd_mode()
 {
  return Fkadr_data ? (Fkadr_data->kadr.kadr_flags&MDB_KADR_FLAG_IMAGE_BKGND_MASK): 0;
 }

bool __fastcall TKadrPainter::get_grid_visible()
{
  return this->show_grid ? true:false;
}

void __fastcall TKadrPainter::set_grid_visible(bool v)
{
  if(v != grid_visible)
     {
      show_grid = v ? TRUE:FALSE;
      bkgnd.set_grid(v ? grid_step : 0 );
      //do_setup_bkgnd();
      Repaint();
     } 

}

void __fastcall TKadrPainter::set_grid_step(DWORD v)
{
 if(Fgrid_step!= v || !show_grid)
 {
   show_grid = v ? TRUE:FALSE;
   Fgrid_step = v;
   if(v)
   {
     bkgnd.set_grid(v ? grid_step : 0 );
     Repaint();
   }
 }
}

void __fastcall TKadrPainter::set_hide_bkgnd(BOOL v)
{
  if(Fhide_bkgnd != v)
  {
   Fhide_bkgnd = v;
   Repaint();
  }
}




 void         __fastcall TKadrPainter::release_bkgnd()
 {
   bkgnd.set_bitmap(NULL);
   this->Fbkgnd_image.Delete(1,Fbkgnd_image.Length());
 }


LONG     __fastcall TKadrPainter::get_kadr_width()
{
 return kadr_data ? kadr_data->kadr.kadr_size.cx : ClientWidth;
}

void     __fastcall TKadrPainter::set_kadr_width(LONG  w)
{
 if(Width!=w)
     Width = w;
 if(kadr_data)
 {
   if(kadr_data->kadr.kadr_size.cx != w)
      kadr_data->kadr.kadr_size.cx  = w;
 }
}

LONG     __fastcall TKadrPainter::get_kadr_height()
{
 return kadr_data ? kadr_data->kadr.kadr_size.cy : ClientHeight;
}

void     __fastcall TKadrPainter::set_kadr_height(LONG h)
{
 if(Height!=h)
     Height = h;
 if(kadr_data)
 {
   if(kadr_data->kadr.kadr_size.cy != h)
      kadr_data->kadr.kadr_size.cy  = h;
 }
}

long __fastcall  NumColors(unsigned short bitCount)
{
  if (bitCount == 1 || bitCount == 4 || bitCount == 8)
    return 1 << bitCount;
  if (bitCount == 16 || bitCount == 24 || bitCount == 32)
    return 0;
  return -1;
}

unsigned short NumBits(long colors)
{
  if (colors <= 2)
    return 1;
  if (colors <= 16)
    return 4;
  if (colors <= 256)
    return 8;
  if (colors <= 65536L)
    return 16;
  return 24;
}

int WINAPI InfoFromBitmapHandle(::TBitmapInfo * InfoStruct,HANDLE hBitmap)
{
 InfoStruct->Info = (LPBITMAPINFO)::GlobalLock(hBitmap);
  if(InfoStruct->Info == 0 )
     return NO_HANDLE;
  InfoStruct->hBitmap = hBitmap;
  if (InfoStruct->Info->bmiHeader.biSize != sizeof(BITMAPINFOHEADER))
  {
    return INVALID_BITMAP_HEADER;
  }
  if (InfoStruct->Info->bmiHeader.biCompression == BI_BITFIELDS)
  {
    InfoStruct->Mask = (unsigned long *)InfoStruct->Info->bmiColors;
    InfoStruct->Colors = (RGBQUAD *)((char *)InfoStruct->Info->bmiColors + 3 * sizeof(DWORD));
  }
  else {
    InfoStruct->Mask = 0;
    InfoStruct->Colors = (RGBQUAD *)InfoStruct->Info->bmiColors;
  }

  InfoStruct->NumClrs = (InfoStruct->Info->bmiHeader.biClrUsed ) ?
             ((InfoStruct->Info->bmiHeader.biClrImportant ) ? InfoStruct->Info->bmiHeader.biClrImportant : InfoStruct->Info->bmiHeader.biClrUsed)
              : NumColors(InfoStruct->Info->bmiHeader.biBitCount);

  InfoStruct->Bits = (unsigned char *)InfoStruct->Colors + (int)InfoStruct->NumClrs * sizeof(RGBQUAD);

  if (!InfoStruct->NumClrs)
    InfoStruct->Colors = 0;

  InfoStruct->Mode = DIB_RGB_COLORS;

  return 0;
}

 HBITMAP WINAPI     create_bitmap(DWORD width,DWORD height)
 {
    HDC ScreenDC = CreateDC("DISPLAY",0,0,0);
    HBITMAP bitmap = CreateCompatibleBitmap(ScreenDC,width,height);
    DeleteDC(ScreenDC);
    return bitmap;
 }



void __fastcall TKadrPainter::FormResize(TObject *Sender)
{
  bool need_repaint = false;
  if(kadr_width!=Width || kadr_height!=Height)
    {
     need_repaint = true;
     kadr_width  =Width;
     kadr_height =Height;
    }

   bkgnd.set_size(Width,Height);
  if(need_repaint)
    {
     changes |= MDB_KADR_FIELD_SIZE;
     Repaint();
    }
}
//---------------------------------------------------------------------------

void __fastcall TKadrPainter::FormDestroy(TObject *Sender)
{
  if(mem_dc)
    DeleteDC(mem_dc);
    this->release_bkgnd();
}
//---------------------------------------------------------------------------


void __fastcall TKadrPainter::prepare_paint_param(TLMOBJ_PAINT_PARAM &pp)
{
  ZeroMemory (&pp,sizeof(pp));
  pp.dw_size    = sizeof(pp);
  pp.font           = Canvas->Font->Handle;
  pp.paint_dc       = Canvas->Handle;
  pp.erase          = flash_counter&1 ? TRUE:FALSE;
  //pp.bkgnd_bitmap   = this->bkgnd_bmp;
  pp.bkgnd_bitmap   = bkgnd();


}

void __fastcall TKadrPainter::paint_entry(kadr_entryes_table::iterator ptr,TLMOBJ_PAINT_PARAM &pp)
{
 if(kadr_data && kadr_data->entryes.size())
 {
  if(get_record(ptr->rec_id,pp.record))
  {
    pp.position       = ptr->position;
    pp.size           = ptr->size;
    pp.bkgnd_color    = ptr->options.bkgnd_color;
    pp.color          = ptr->options.color;
    pp.paint_selected = kadr_data->is_selected(ptr);
    pp.precision      = MAKELONG(ptr->options.prec,ptr->options.digits);
    pp.paint_variant  = ptr->options.paint_as;
    pp.paint_options  = ptr->options.paint_flags;
    pp.changed        = 0;
    pp.normal_value   = pp.record.value;// ptr->options.normal_value;
    WideString w_str  = get_name(pp.record.rec_id,false);
    pp.short_name     = w_str.c_bstr();
    if(diag_off)       pp.record.diag    = 0;
    if(!ptr->size.cx || !ptr->size.cy)
      {
       painter_dll->call(PMCMD_CALC_SIZE,(LPARAM)&pp,0);
       ptr->size.cx=pp.size.cx;
       ptr->size.cy=pp.size.cy;
      }
    painter_dll->call(PMCMD_PAINT,(LPARAM)&pp,0);
    if(ptr->size.cx<pp.size.cx)
       ptr->size.cx=pp.size.cx;
    if(ptr->size.cy<pp.size.cy)
       ptr->size.cy=pp.size.cy;
  }
 }
}

void      __fastcall TKadrPainter::repaint_entry(kadr_entryes_table::iterator ptr)
{
    TLMOBJ_PAINT_PARAM pp;    
    prepare_paint_param(pp);
    this->paint_entry(ptr,pp,TRUE);
    this->paint_entry(ptr,pp,FALSE);

}



void __fastcall TKadrPainter::update_intersect(kadr_entryes_table::iterator ptr,bool before)
{
    TLMOBJ_PAINT_PARAM pp;    
    prepare_paint_param(pp);
    RECT r1,r2,i_r;
    set_rect(r1,ptr->position,ptr->size);
    kadr_entryes_table::iterator beg = before ? kadr_data->entryes.begin() : ptr,end = before ? ptr : kadr_data->entryes.end();
    while(beg<end)
      {
       if(beg!=ptr)
          {
           set_rect(r2,beg->position,beg->size);
           IntersectRect(&i_r,&r1,&r2);
           if(i_r.right || i_r.bottom)
             paint_entry(beg,pp);
          }
       ++beg;
      }

}

void __fastcall TKadrPainter::paint_entry(kadr_entryes_table::iterator ptr,TLMOBJ_PAINT_PARAM &pp,BOOL erase)
{
 if(kadr_data && kadr_data->entryes.size())
 {
  if(get_record(ptr->rec_id,pp.record))
  {
    pp.erase          = erase;
    pp.position       = ptr->position;
    pp.size           = ptr->size;
    pp.bkgnd_color    = ptr->options.bkgnd_color;
    pp.color          = ptr->options.color;
    pp.paint_selected = kadr_data->is_selected(ptr) ;
    pp.precision      = ptr->options.prec;
    pp.paint_variant  = ptr->options.paint_as;
    pp.paint_options  = ptr->options.paint_flags;
    pp.changed        = 0;
    pp.normal_value   = ptr->options.normal_value;
    WideString w_str  = get_name(pp.record.rec_id,false);
    pp.short_name     = w_str.c_bstr();
    if(!ptr->size.cx || !ptr->size.cy)
      {
       painter_dll->call(PMCMD_CALC_SIZE,(LPARAM)&pp,0);
       ptr->size.cx=pp.size.cx;
       ptr->size.cy=pp.size.cy;
      }
    painter_dll->call(PMCMD_PAINT,(LPARAM)&pp,0);
    update_intersect(ptr,erase);
  }
 }
}




void __fastcall TKadrPainter::paint_entry(DWORD idx,BOOL erase)
{
 if(kadr_data && idx<kadr_data->entryes.size())
 {
   TLMOBJ_PAINT_PARAM pp;
   prepare_paint_param(pp);
   kadr_entryes_table::iterator ptr = kadr_data->entryes.begin()+idx;
   paint_entry(ptr,pp,erase);
 }

}

 bool   __fastcall TKadrPainter::update_size(DWORD idx)
 {
 bool ret = false;
 if(kadr_data && idx<kadr_data->entryes.size())
 {
   TLMOBJ_PAINT_PARAM pp;
   prepare_paint_param(pp);
   kadr_entryes_table::iterator ptr = kadr_data->entryes.begin()+idx;
    pp.erase          = false;
    pp.position       = ptr->position;
    pp.size.cx        = pp.size.cy = 0;
    pp.precision      = ptr->options.prec;
    pp.paint_variant  = ptr->options.paint_as;
    pp.paint_options  = ptr->options.paint_flags;
    pp.changed        = 0;
    pp.normal_value   = ptr->options.normal_value;
    WideString w_str  = get_name(pp.record.rec_id,false);
    pp.short_name     = w_str.c_bstr();
    painter_dll->call(PMCMD_PAINT,(LPARAM)&pp,true);
    if(ptr->size.cx < pp.size.cx)
      ptr->size.cx = pp.size.cx, ret = true;
    if(ptr->size.cy < pp.size.cy)
       ptr->size.cy = pp.size.cy,ret = true;


  }
  return ret; 
 }


void __fastcall TKadrPainter::FormPaint(TObject *Sender)
{
 RECT r;
 SetRect(&r,0,0,Width,Height);
 bkgnd.paint(Canvas->Handle,r);
 if(kadr_data)
 {
  TLMOBJ_PAINT_PARAM pp;
  prepare_paint_param(pp);
   kadr_entryes_table::iterator ptr = kadr_data->entryes.begin(),end = kadr_data->entryes.end();
   while(ptr<end)
    paint_entry(ptr++,pp);
 }
}
//---------------------------------------------------------------------------
void __fastcall TKadrPainter::WndProc(Messages::TMessage &Message)
{
  switch(Message.Msg)
  {
   case WM_ERASEBKGND:
    {
      if(bkgnd())
         Message.Result = TRUE;
         else
         TForm::WndProc(Message);
    }
    break;
//    case WM_GETDLGCODE:
//         Message.Result = DLGC_WANTALLKEYS;
//    break;
   default : TForm::WndProc(Message); break;
  }
}

void   __fastcall TKadrPainter::on_kadr_change(DWORD mask)
{
  if(mask & (MDB_KADR_FIELD_FLAGS | MDB_KADR_FIELD_SIZE |MDB_KADR_FIELD_IMAGE))
    {
      bkgnd.set_mode(tlmpaint::TBmpBkgnd::bkgnd_mode_t(kadr_data->kadr.kadr_flags));
      bkgnd.set_size(Width,Height);
    }
   Repaint();
}

void   __fastcall TKadrPainter::on_record_change(DWORD mask,DWORD rec_id)
{


 kadr_entryes_table::iterator lo,hi;
 if(kadr_data->range_entry(rec_id,lo,hi))
 {
   TLMOBJ_PAINT_PARAM pp;
   prepare_paint_param(pp);
   while(lo<hi)
    this->paint_entry(lo++,pp);
 }
 
}

kadr_entryes_table::iterator   __fastcall TKadrPainter::find_entry(int x,int y)
{
 kadr_entryes_table::iterator ret = NULL;
 if(this->kadr_data)
  {
   kadr_entryes_table::reverse_iterator ptr = kadr_data->entryes.rbegin(),end =kadr_data->entryes.rend();
   RECT  r;
   POINT p;
   p.x = x;p.y = y;
   while(ptr<end && !&*ret)
   {
    set_rect(r,ptr->position,ptr->size);
     if(PtInRect(r,p))
       ret = &*ptr;
    ++ptr;
   }
  }
 return  ret;
}



void   __fastcall TKadrPainter::resize_entry(mdb_kadr_entry & entry,SIZE & delta)
{

  TLMOBJ_PAINT_PARAM pp;
  prepare_paint_param(pp);
  paint_entry(&entry,pp,TRUE);
  entry.size.cx+=delta.cx;
  entry.size.cy+=delta.cy;
  paint_entry(&entry,pp,FALSE);


}

void   __fastcall TKadrPainter::move_entry  (mdb_kadr_entry & entry,SIZE & delta)
{
  TLMOBJ_PAINT_PARAM pp;
  prepare_paint_param(pp);
  paint_entry(&entry,pp,TRUE);
  entry.position.x+=delta.cx;
  entry.position.y+=delta.cy;
  paint_entry(&entry,pp,FALSE);
  changes  |= MDB_KADR_FIELD_ENTRYES;
}

void   __fastcall TKadrPainter::resize_sel_entryes(SIZE & delta)
{
  std::vector<DWORD>::iterator ptr = kadr_data->selected.begin(),end =kadr_data->selected.end();
  while(ptr<end)
  {
   if(*ptr<kadr_data->entryes.size())
   resize_entry(kadr_data->entryes.at(*ptr),delta);
   ++ptr;
  }
}

void   __fastcall TKadrPainter::move_sel_entryes  (SIZE & delta)
{
  std::vector<DWORD>::iterator ptr = kadr_data->selected.begin(),end =kadr_data->selected.end();
  while(ptr<end)
  {
   if(*ptr<kadr_data->entryes.size())
    move_entry(kadr_data->entryes.at(*ptr),delta);
   ++ptr;
  }
}





void __fastcall TKadrPainter::FormMouseMove(TObject *Sender,
      TShiftState Shift, int X, int Y)
{
  //Перемещение мыши
  kadr_entryes_table::iterator  entry = mouse_entry[X][Y];
  if(!captured && Fover_entry!= &*entry)
  {
   Fover_entry = &*entry;
   if(on_over_entry)
      on_over_entry(Fover_entry);
  }

  if(lmb_down )
  {
    captured =true;
    if(mouse_sel_mode)
    {

     int old_rop = ::SetROP2(Canvas->Handle,R2_NOTXORPEN);
     Canvas->MoveTo(spot.x,spot.y);
     Canvas->LineTo(spot1.x,spot.y);
     Canvas->LineTo(spot1.x,spot1.y);
     Canvas->LineTo(spot.x,spot1.y);
     Canvas->LineTo(spot.x,spot.y);
     spot1.x = X;
     spot1.y = Y;

     Canvas->MoveTo(spot.x,spot.y);
     Canvas->LineTo(spot1.x,spot.y);
     Canvas->LineTo(spot1.x,spot1.y);
     Canvas->LineTo(spot.x,spot1.y);
     Canvas->LineTo(spot.x,spot.y);

      SetROP2(Canvas->Handle,old_rop);
    }
    else
    {
    if(kadr_data->sel_count() &&  over_entry )
    {
      mdb_kadr_entry * entry = kadr_data->get_selected(0);
      if(entry)
      {
      SIZE delta;
      delta.cx = X-spot.x  ;spot.x = X;
      delta.cy = Y-spot.y  ;spot.y = Y;
      if(delta.cx || delta.cy)
      {
      if(!captured)
          captured = true;

      if(Shift.Contains(ssShift))
         resize_sel_entryes(delta);
         else
         move_sel_entryes(delta);
         changes  |= MDB_KADR_FIELD_ENTRYES;
         if(on_over_entry)
            on_over_entry(entry);

      }
     }
    }
    else
    {
     //Переход в режим выделения мышью
     if(!this->select_on_down)
     {
      mouse_sel_mode = TRUE;
      spot1 = spot;
     }
    }
   }
  }
}


//---------------------------------------------------------------------------

void __fastcall TKadrPainter::FormMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
  if(Button == mbLeft)
     {
      lmb_down = TRUE;
      spot.x = X;
      spot.y = Y;
      if(over_entry)
      {
      if(!kadr_data->is_selected(over_entry))
          {
           select_on_down = over_entry ? true:false;
           if(!select_on_down)
             kadr_data->multi_select_mode_ = true;
           DWORD offset = kadr_data->get_offset(over_entry);
           select_entry(offset,Shift.Contains(ssCtrl));
           if(kadr_data->multi_select_mode_)
           {
            kadr_data->multi_select_mode_ = false;
            Repaint();
           }
          }
      }
     }

  if(Button == mbRight)
     rmb_down = TRUE;
}
//---------------------------------------------------------------------------



void __fastcall TKadrPainter::FormMouseUp(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
  if(Button == mbLeft)
     {
      lmb_down = FALSE;

      if(!captured && !select_on_down)
      {
       DWORD offset = kadr_data->get_offset(over_entry);
       select_entry(offset,Shift.Contains(ssCtrl));
      }
      else
      select_on_down = false;

    if(mouse_sel_mode)
       {
        mouse_sel_mode = FALSE;
        RECT r;
        r.left   = KeRTL::MIN(spot.x,spot1.x);
        r.top    = KeRTL::MIN(spot.y,spot1.y);
        r.right  = KeRTL::MAX(spot.x,spot1.x);
        r.bottom = KeRTL::MAX(spot.y,spot1.y);
        select_in_rect(r,Shift.Contains(ssCtrl));
        Repaint();
       }

     }

  if(Button == mbRight)
     {
      rmb_down = FALSE;
     }
    if(on_over_entry)
        on_over_entry(over_entry);
   if(captured)
     captured = false;

  mouse_sel_mode = FALSE;   
}
//---------------------------------------------------------------------------
void   __fastcall TKadrPainter::select_entry(DWORD offs,bool multi)
{

   if(offs<this->kadr_data->entryes.size())
   {
   mdb_kadr_entry * entry = &kadr_data->entryes.at(offs);
   if(!kadr_data->is_selected(offs))
    {
      if(!multi)
        {
         //kadr_data->clear_select();
        }
      //kadr_data->select(offs);
    }
    //else
    //kadr_data->deselect(&kadr_data->entryes.at(offs));
    on_select_entry(entry,multi);
   }
    else
    {
     //Снятие очиски
     if(!multi)
     {
      if(kadr_data->sel_count())
         //kadr_data->clear_select();
        on_select_entry(NULL,true);
     }
    }
   Repaint();
}

void   __fastcall TKadrPainter::select_in_rect(RECT & r,bool add)
{
 if(!add)
    select_entry(-1,false);
 kadr_entryes_table::iterator beg = kadr_data->entryes.begin(),end = kadr_data->entryes.end();
 kadr_data->multi_select_mode_ = true;
 while(beg<end)
 {
  POINT pt1,pt2;
  pt1 <<  beg->position;
  pt2 = pt1;
  pt2.x += beg->size.cx;
  pt2.y += beg->size.cy;
  if(PtInRect(&r,pt1) || PtInRect(&r,pt2) )
   {
        on_select_entry(&*beg,true);
        //kadr_data->select(beg);
   }
  ++beg;
 }
 kadr_data->multi_select_mode_ = false;


}


void   __fastcall TKadrPainter::set_sel_text_color(DWORD color)
{
  entry_index::iterator ptr = kadr_data->selected.begin(),end =kadr_data->selected.end();
  while(ptr<end)
  {
   mdb_kadr_entry & entry = kadr_data->entryes.at(*ptr);
   entry.options.color = color;
   this->repaint_entry(&entry);
   ++ptr;
  }
}

int   __fastcall TKadrPainter::set_sel_paint_var (DWORD paint_var,bool discrete )
{
  int ret = 0;
  entry_index::iterator ptr = kadr_data->selected.begin(),end =kadr_data->selected.end();
  while(ptr<end)
  {
   mdb_kadr_entry & entry = kadr_data->entryes.at(*ptr);
   mdb_short_record sr;
   get_record(entry.rec_id,sr);
   if( discrete == bool(sr.state & MDBR_STATE_DISCRETE))
   {
       if(entry.options.paint_as!=(BYTE)paint_var)
       {
        ++ret;
        paint_entry(*ptr,true);
        entry.options.paint_as = paint_var;
        SIZE save_sz;
        save_sz<<entry.size;
        entry.size.cx = entry.size.cy = 0;
        paint_entry(*ptr,false);
        if(!entry.size.cx || !entry.size.cy)
        {
          entry.size<< save_sz;
          entry.size<< save_sz;
          paint_entry(*ptr,false);
        }

       }
   }
   ++ptr;
  }
 return ret;
}

void   __fastcall TKadrPainter::set_sel_prec_digits (int prec,int digits)
{
  entry_index::iterator ptr = kadr_data->selected.begin(),end =kadr_data->selected.end();
  while(ptr<end)
  {
   mdb_kadr_entry & entry = kadr_data->entryes.at(*ptr);
    entry.options.prec = prec;
    entry.options.digits = digits;
    repaint_entry(&entry);
   ++ptr;
  }
}


void   __fastcall TKadrPainter::set_sel_bk_color  (DWORD color)
{
  entry_index::iterator ptr = kadr_data->selected.begin(),end =kadr_data->selected.end();
  while(ptr<end)
  {
   mdb_kadr_entry & entry = kadr_data->entryes.at(*ptr);
   entry.options.bkgnd_color = color;
   repaint_entry(&entry);
   ++ptr;
  }
}

void   __fastcall TKadrPainter::arrange_sel_left()
{
  if(kadr_data->sel_count()>1)
  {
  std::vector<DWORD>::iterator ptr = kadr_data->selected.begin(),end =kadr_data->selected.end();
  mdb_kadr_entry * entry = &kadr_data->entryes.at(*ptr);
  DWORD l_pos = entry->position.x;
  ++ptr;
  while(ptr<end)
  {
   entry = &kadr_data->entryes.at(*ptr);
   if(entry->position.x != (LONG)l_pos)
      {
       entry->position.x = l_pos;
       changes|=MDB_KADR_FIELD_ENTRYES;
      } 
      //repaint_entry(&entry);
   ++ptr;
  }

   Repaint();
  }
}

void   __fastcall TKadrPainter::do_arrange_grid(mdb_kadr_entry * entry)
{
  entry->position.x += entry->position.x%grid_step;
  entry->position.y += entry->position.y%grid_step;
}

struct entry_position_less:public std::binary_function<const DWORD&,const DWORD&,bool>
{
  single_kadr * kadr_data;
  bool          horizontal;
  entry_position_less(single_kadr * _kd,bool _horizontal):kadr_data(_kd),horizontal(_horizontal){}
  entry_position_less(const entry_position_less & src){*this = src;}
  entry_position_less & operator = (const entry_position_less & src){kadr_data = src.kadr_data;horizontal = src.horizontal;return * this; }
  bool operator ()(const DWORD& dw1,const DWORD& dw2)
  {
    mdb_kadr_entry & e1 = kadr_data->entryes.at(dw1);
    mdb_kadr_entry & e2 = kadr_data->entryes.at(dw2);
    if(&e1 && &e2)
    return horizontal ? (e1.position.x<e2.position.x) ? true: false : (e1.position.y<e2.position.y) ? true: false;
    else
    return false;
  }
};

void   __fastcall TKadrPainter::arrange_sel_grid(bool horizontal)
{
  if(kadr_data->sel_count()>1)
  {
   std::vector<DWORD>::iterator ptr = kadr_data->selected.begin(),end =kadr_data->selected.end();
   std::vector<DWORD> arrange_list(ptr,end);
   std::sort(arrange_list.begin(),arrange_list.end(),entry_position_less(kadr_data,horizontal));
   ptr = arrange_list.begin();
   end = arrange_list.end();
   mdb_kadr_entry * prev = NULL;
   while(ptr<end)
   {
    mdb_kadr_entry & e = kadr_data->entryes.at(*ptr);
    if(prev)
      {
       if(horizontal)
         {
          e.position.x = prev->position.x+prev->size.cx+this->grid_step;
          e.position.y = prev->position.y;
         }
         else
         {
          e.position.y = prev->position.y+prev->size.cy+this->grid_step;
          e.position.x = prev->position.x;
         }
      }
    do_arrange_grid(&e);
    prev = &e;
    ++ptr;

    changes|=MDB_KADR_FIELD_ENTRYES;
   }

   Repaint();
  }

}

void   __fastcall TKadrPainter::arrange_sel_top ()
{
  if(kadr_data->sel_count()>1)
  {
  std::vector<DWORD>::iterator ptr = kadr_data->selected.begin(),end =kadr_data->selected.end();
  mdb_kadr_entry * entry = &kadr_data->entryes.at(*ptr);
  DWORD t_pos = entry->position.y;
  ++ptr;
  while(ptr<end)
  {
   entry = &kadr_data->entryes.at(*ptr);
   if(entry->position.y !=(LONG) t_pos)
    {
     entry->position.y= t_pos;
     changes|=MDB_KADR_FIELD_ENTRYES;
    }
   //this->repaint_entry(&entry);
   ++ptr;
  }
   Repaint();
  }
}

void   __fastcall TKadrPainter::arrange_sel_size()
{
  if(kadr_data->sel_count()>1)
  {
  std::vector<DWORD>::iterator ptr = kadr_data->selected.begin(),end =kadr_data->selected.end();
  mdb_kadr_entry * entry = &kadr_data->entryes.at(*ptr);
  SIZE sz;
  sz.cx =  entry->size.cx;
  sz.cy =  entry->size.cy;
  ++ptr;
  while(ptr<end)
  {
   entry = &kadr_data->entryes.at(*ptr);
   if(entry->size.cx != sz.cx || entry->size.cy != sz.cy )
    {
     entry->size<< sz;
     changes|=MDB_KADR_FIELD_ENTRYES;
    } 
   //this->repaint_entry(entry);
   ++ptr;
  }
   Repaint();
  }
}

void   __fastcall TKadrPainter::reset_size()
{
  if(kadr_data->sel_count()>0)
  {
  std::vector<DWORD>::iterator ptr = kadr_data->selected.begin(),end =kadr_data->selected.end();
  while(ptr<end)
  {
   mdb_kadr_entry * entry = &kadr_data->entryes.at(*ptr);
   //if(!entry->options.paint_as)
   {
    entry->size.cx = entry->size.cy = 0;
    paint_entry(*ptr);
   }
   ++ptr;
  }
  changes|=MDB_KADR_FIELD_ENTRYES;
  Repaint();
  }
}

void   __fastcall TKadrPainter::move_outsiding()
{
 //Перемещение объектов выжедших за границу экрана
 kadr_entryes_table::iterator beg = kadr_data->entryes.begin(),
                              end = kadr_data->entryes.end();
 int w = Width;
 int h = Height;
 while(beg<end)
 {
  if(beg->position.x<1)
     beg->position.x=1;
  if(beg->position.y<1)
     beg->position.y=1;
  if((beg->position.x+beg->size.cx) >= w)
      beg->position.x = w - beg->size.cx-1;
  if((beg->position.y+beg->size.cy) >=h )
      beg->position.y = h - beg->size.cy-1;
  ++beg;
 }
 Repaint();
}

void   __fastcall TKadrPainter::select_all()
{
 int count = kadr_data->entryes.size();
 this->kadr_data->clear_selections();
 for(int i = 0;i<count;i++)
 {
  kadr_data->select(i);
 }
 Repaint();
}




void __fastcall TKadrPainter::FormKeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
  TSize sz;
//  bool alt_pressed  = Shift.Contains(ssAlt);
//  bool ctrl_pressed = Shift.Contains(ssCtrl);
  switch(Key)
  {
   case VK_LEFT:
        sz.cx = -1;sz.cy = 0;
        move_sel_entryes(sz);
        break;
   case VK_RIGHT:
        sz.cx = 1;sz.cy = 0;
        move_sel_entryes(sz);
        break;
   case VK_UP:
        sz.cy = -1;sz.cx = 0;
        move_sel_entryes(sz);
        break;
   case VK_DOWN:
        sz.cy = 1;sz.cx = 0;
        move_sel_entryes(sz);
        break;
  }
}
//---------------------------------------------------------------------------
DWORD __fastcall TKadrPainter::get_selected_recid()
{
 if(kadr_data->sel_count())
 {
  mdb_kadr_entry * ke=
  kadr_data->get_selected(0);
  if(ke)
    return ke->rec_id;
 }
 return -1;
}



void __fastcall TKadrPainter::FormKeyPress(TObject *Sender, System::WideChar &Key)

{
  TSize sz;
  sz.cx = sz.cy = 0;
  switch(Key)
  {
   case L'L':
   case L'l':
   case L'Д':
   case L'д':
        Key = 0;
        move_sel_entryes(sz);
        sz.cx = -1;
        move_sel_entryes(sz);

        break;
   case L'\'':
   case L'\"':
   case L'Э':
   case L'э':
        Key = 0;
        move_sel_entryes(sz);
        sz.cx = 1;
        move_sel_entryes(sz);
        break;
   case L'P':
   case L'p':
   case L'З':
   case L'з':
        Key = 0;
        move_sel_entryes(sz);
        sz.cy = -1;
        move_sel_entryes(sz);
        break;
   case L'>':
   case L'.':
   case L'Ю':
   case L'ю':
        Key = 0;
        move_sel_entryes(sz);
        sz.cy = 1;
        move_sel_entryes(sz);
        break;
  }
if(!Key)
{
         changes  |= MDB_KADR_FIELD_ENTRYES;
         if(this->over_entry && on_over_entry)
            on_over_entry(over_entry);

}
}
//---------------------------------------------------------------------------
bool   __fastcall TKadrPainter::enum_paint_variant(bool discrete,int idx,UnicodeString & s)
{
  char text[1024];
  if(painter_dll->call(discrete ? PMCMD_ENUM_DISCRETE_VARIANTS:PMCMD_ENUM_ANALOG_VARIANTS,(LPARAM)text,idx))
     {
       text[0] = 0;
     }
   s = text;return !s.IsEmpty();
}

void __fastcall TKadrPainter::set_diag_off(bool val)
{
  if(Fdiag_off != val)
   {
     Fdiag_off = val;
     Repaint();
   }
}
