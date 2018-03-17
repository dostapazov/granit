//---------------------------------------------------------------------------

#pragma hdrstop
#include <vcl.h>

#include "kadr_form.h"
#include <stdio.h>
#include <math.h>
#include <dibs.h>
#include <mediumdb.rh>
#include <algorithm>
#include <kadr_painter.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "gkmodule_form"
#pragma resource "*.dfm"
TKadrForm *KadrForm;
//---------------------------------------------------------------------------
__fastcall TKadrForm::TKadrForm(TComponent* Owner)
        :TForm(Owner)
{
 
 selected_entry   = NULL;
 lb_down = rb_down = FALSE;
 mouse_captured = FALSE;
 Feditable = FALSE;
 Fon_modify_kadr = 0;
 Fon_select_entry = 0;
 Fon_delete_entry = 0;
 Fon_apply = 0;
 Fon_undo  = 0;
 Frecord_operations = 0;
 in_setup = 0;
 
 MemoryDC = CreateCompatibleDC(Canvas->Handle);
 HANDLE hDib;
 dmask_bmp = ::LoadBitmap(ModuleInstance,MAKEINTRESOURCE(BMP_DISCRETE_MASK)); //DibToDdb(hDib,0,0,TRUE);
 doff_bmp  = ::LoadBitmap(ModuleInstance,MAKEINTRESOURCE(BMP_DISCRETE_OFF));//DibToDdb(hDib,0,0,TRUE);;
 don_bmp   = ::LoadBitmap(ModuleInstance,MAKEINTRESOURCE(BMP_DISCRETE_ON));//DibToDdb(hDib,0,0,TRUE);;
 bkgnd_dib = NULL;
 bkgnd_bitmap = 0;
 painter_dll.load_module("tlmobj_std_painter.dll");
}
//---------------------------------------------------------------------------

//D:\USER\GRANIT\SRC\GrKrnl32\MediumDB\MediumDbWnd\discrete_image_mask.bmp
//D:\USER\GRANIT\SRC\GrKrnl32\MediumDB\MediumDbWnd\discrete_image_off.bmp
//D:\USER\GRANIT\SRC\GrKrnl32\MediumDB\MediumDbWnd\discrete_image_on.bmp

 void        __fastcall TKadrForm::set_editable(BOOL value)
 {
  if(Feditable!=value)
  {
  Feditable = value;
  ApplyBtn->Visible = value;
  UndoBtn->Visible  = value;
  DeleteBtn->Visible  = value;
  if(value)
     {
      StatusBar1->Panels->Insert(0);
      TStatusPanel * panel = StatusBar1->Panels->Items[0];
      panel->Width = 300;
     }
    else
    StatusBar1->Panels->Delete(0);
  }
 }



 void           __fastcall TKadrForm::set_changes(DWORD ch,mdb_kadr_entry * entry)
 {
   if(!in_setup)
   {
   if(Fchanges != ch)
   {
    Fchanges = ch;
   }
    if(this->on_modify_kadr &&ch)
      on_modify_kadr(this,Fchanges,entry);
    ApplyBtn->Enabled = ch ? true:false;
    UndoBtn->Enabled  = ApplyBtn->Enabled;
   }
 }

 void        __fastcall TKadrForm::set_changes(DWORD ch)
 {
   if(Fchanges != ch)
   {
    Fchanges = ch;
   }
    if(this->on_modify_kadr && ch)
      on_modify_kadr(this,Fchanges,NULL);
   ApplyBtn->Enabled = ch ? true:false;
   UndoBtn->Enabled  = ApplyBtn->Enabled;
 }


 void __fastcall TKadrForm::clear()
 {
  Fkadr = mdb_kadr();
  if(bkgnd_dib)
    FreeImage(bkgnd_dib),bkgnd_dib = NULL;
  clear_entryes();
  changes = 0;
  Caption = "";
  Repaint();
 }

 void __fastcall TKadrForm::clear_entryes()
 {
  Fentry_items.erase(Fentry_items.begin(),Fentry_items.end());
  selected_entry = over_entry = NULL;
  Repaint();
 }

 void __fastcall TKadrForm::set_kadr(mdb_kadr & _kadr)
 {
   if(!_kadr.kadr_size.cx) _kadr.kadr_size.cx = Width;
   if(!_kadr.kadr_size.cy) _kadr.kadr_size.cy = Height;
   Fkadr = _kadr;
   OnResize = NULL;
   set_kadr_width(Fkadr.kadr_size.cx);
   set_kadr_height(Fkadr.kadr_size.cy);
   setup_bkgnd();
   OnResize     = FormResize;
 }

 void __fastcall TKadrForm::set_kadr_width (LONG kw)
 {
  ClientWidth =  kw;
 }

 void __fastcall TKadrForm::set_kadr_height(LONG kh)
 {
  ClientHeight = kh + StatusBar1->Height+ ToolBar1->Height;
 }


 void         __fastcall TKadrForm::SetBkgndImage(AnsiString str)
 {
  FBkgndImage = str;
  LoadBitmap();
  Repaint();
 }

 void       __fastcall TKadrForm::LoadBitmap()
 {
   //Загрузка из фала

   if(bkgnd_dib)
     FreeImage(bkgnd_dib),bkgnd_dib = NULL;

   bkgnd_dib = LoadImageFromFile(FBkgndImage.c_str());
   if(bkgnd_dib)
    setup_bkgnd();
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

 void __fastcall TKadrForm::setup_bkgnd()
 {
   if(bkgnd_bitmap)
     DeleteObject(bkgnd_bitmap);
    bkgnd_bitmap = create_bitmap(kadr_width,kadr_height);
    HANDLE old_dcbmp = SelectObject(MemoryDC,bkgnd_bitmap);
    DeleteObject(old_dcbmp);
    HBRUSH br = CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
    RECT r;
    ::set_rect(r,0,0,kadr_width,kadr_height);
    FillRect(MemoryDC,&r,br);
    DeleteObject(br);
    if(bkgnd_dib)
    {
      SetStretchBltMode(MemoryDC,COLORONCOLOR);
     ::TBitmapInfo bi(bkgnd_dib);
     if(this->Fkadr.kadr_flags & MDB_KADR_FLAG_IMAGE_STRETCH)
       {
          StretchDIBits(
                  MemoryDC,0,0,kadr_width,kadr_height,
                  0,0,bi.Info->bmiHeader.biWidth,
                  bi.Info->bmiHeader.biHeight,
                  bi.Bits,bi.Info,bi.Mode,SRCCOPY
                  );
       }
       else
       {
          DWORD pos_x = 0,pos_y = 0;
          if(Fkadr.kadr_flags&MDB_KADR_FLAG_IMAGE_CENTER)
          {
           pos_x = (kadr_width - bi.Info->bmiHeader.biWidth)/2;
           pos_y = (kadr_height - bi.Info->bmiHeader.biHeight)/2;
          } 
          StretchDIBits(
                  MemoryDC,pos_x,pos_y,bi.Info->bmiHeader.biWidth,bi.Info->bmiHeader.biHeight,
                  0,0,bi.Info->bmiHeader.biWidth,
                  bi.Info->bmiHeader.biHeight,
                  bi.Bits,bi.Info,bi.Mode,SRCCOPY
                  );

       }

    }

 }

 void         __fastcall TKadrForm::paint_discrete_lamp(kadr_entry &entry,bool flash)
 {
  //Отрисовка лампочки

  TLMOBJ_PAINT_PARAM pp;
  ZeroMemory(&pp,sizeof(pp));
  pp.dw_size  = sizeof(pp);
  pp.paint_dc = Canvas->Handle;
  pp.position = entry.descript.position;
  pp.size     = entry.descript.size;
  pp.record   = entry.value;
  pp.normal_value = entry.descript.options.normal_value;
  pp.precision    = entry.descript.options.prec;
  pp.bkgnd_color = entry.descript.options.bkgnd_color;
  pp.color = entry.descript.options.color;
  pp.font  = NULL;
  pp.paint_variant = entry.descript.options.paint_as;
  pp.paint_options = 0;
  pp.bkgnd_bitmap  = this->bkgnd_bitmap;
  pp.short_name    = entry.sname.c_bstr();
  painter_dll(PMCMD_PAINT,(LPARAM)&pp,0);
  if(!entry.descript.size.cy < pp.size.cy)
      entry.descript.size.cy = pp.size.cy;
  if(!entry.descript.size.cx<pp.size.cx)
      entry.descript.size.cx = pp.size.cx;

  return ;
 }
 void         __fastcall TKadrForm::paint_discrete_swhor (kadr_entry &entry,bool flash)
 {
  paint_discrete_lamp(entry,flash);
  return ;
 }

 void         __fastcall TKadrForm::paint_discrete_swvert(kadr_entry &entry,bool flash)
 {
  paint_discrete_lamp(entry,flash);
  return ;
 }


 void         __fastcall TKadrForm::paint_discrete_sname (kadr_entry &entry,bool flash)
 {
   if(entry.sname.Length())
   {
   SIZE sz;
   GetTextExtentPointW(Canvas->Handle,entry.sname.c_bstr(),entry.sname.Length(),&sz);
   if(entry.descript.size.cx<sz.cx)
      entry.descript.size.cx=sz.cx;
   if(entry.descript.size.cy<sz.cy)
      entry.descript.size.cy=sz.cy;
    HANDLE old_dcbmp;
    old_dcbmp = SelectObject(MemoryDC,bkgnd_bitmap);
    BitBlt(Canvas->Handle,entry.descript.position.x,entry.descript.position.y,entry.descript.size.cx,entry.descript.size.cy,MemoryDC,entry.descript.position.x,entry.descript.position.y-ToolBar1->Height,SRCCOPY);
    SelectObject(MemoryDC,old_dcbmp);

    if(!flash)
       {
        TColor color   =  ((DWORD)entry.value.value) ?  (TColor)entry.descript.options.color:clBlack;
        if(((DWORD)entry.value.value) && color == clBlack)
           color = clYellow;

        if(entry.value.state&(MDBR_STATE_DANGER|MDBR_STATE_ALARM))
           color = clRed;
        Canvas->Font->Color = color;
        RECT r ;
        set_rect(r,entry.descript.position,entry.descript.size);
        int old_mode = SetBkMode(Canvas->Handle,TRANSPARENT);
        ExtTextOutW(Canvas->Handle,r.left,r.top,0,&r,entry.sname.c_bstr(),entry.sname.Length(),NULL);
        SetBkMode(Canvas->Handle,old_mode);
       }

   }
   else
   paint_discrete_lamp(entry,flash);
 }

 void         __fastcall TKadrForm::paint_discrete(kadr_entry & entry,bool flash )
 {
  switch(entry.descript.options.paint_as)
  {
   case TS_PAINT_AS_LAMP       : paint_discrete_lamp  (entry,flash);break;
   case TS_PAINT_AS_SWITCH_HOR : paint_discrete_swhor (entry,flash);break;
   case TS_PAINT_AS_SWITCH_VERT: paint_discrete_swvert(entry,flash);break;
   case TS_PAINT_AS_SHORT_NAME : paint_discrete_sname (entry,flash);break;

  }

 }

 void         __fastcall TKadrForm::paint_analog(kadr_entry & entry,bool flash   )
 {
   switch(entry.descript.options.paint_as)
   {
    case   1:paint_analog_vertbar(entry,flash);break;
    case   2:paint_analog_horbar (entry,flash);break;
    default :paint_analog_text   (entry,flash);break;
   }
 }

 void         __fastcall TKadrForm::paint_analog_vertbar(kadr_entry & entry,bool flash)
 {

  if(entry.descript.size.cx == 0)
     entry.descript.size.cx = 16;
  if(entry.descript.size.cy == 0)
     entry.descript.size.cy = 16;
  RECT r;
  set_rect(r,entry.descript.position,entry.descript.size);
  HBRUSH br = CreateSolidBrush(entry.descript.options.bkgnd_color);
  FillRect(Canvas->Handle,&r,br);
  DeleteObject(br);
  float v_min     = entry.value.min_value,v_max = entry.value.max_value;
  float distance  = (float)fabs(v_max-v_min);
  float zero_dist = fabs(.0-v_min);
  float k = ((float)entry.descript.size.cy)/distance;
  int   curr_cy = (DWORD)(k*(float)fabs((entry.value.value - entry.value.min_value)));
  curr_cy = entry.descript.size.cy-curr_cy;
  grow_rect(r,0,-curr_cy);
  move_rect(r,0,curr_cy);
  br = CreateSolidBrush(entry.descript.options.color);
  FillRect(Canvas->Handle,&r,br);
  DeleteObject(br);
  if((entry.value.max_value*entry.value.min_value)<0.0)
  {
   //Отрисовка 0
   curr_cy = r.bottom-(DWORD)(k*fabs(zero_dist));
   int old_rop = SetROP2(Canvas->Handle,R2_NOTXORPEN);
   MoveToEx(Canvas->Handle,r.left,curr_cy,NULL);
   LineTo(Canvas->Handle,r.right,curr_cy);
   SetROP2(Canvas->Handle,old_rop);
  }
 }

 void         __fastcall TKadrForm::paint_analog_horbar(kadr_entry & entry,bool flash)
 {

  if(entry.descript.size.cx == 0)
     entry.descript.size.cx = 16;
  if(entry.descript.size.cy == 0)
     entry.descript.size.cy = 16;
  RECT r;
  set_rect(r,entry.descript.position,entry.descript.size);
  HBRUSH br = CreateSolidBrush(entry.descript.options.bkgnd_color);
  FillRect(Canvas->Handle,&r,br);
  DeleteObject(br);
  float v_min    = entry.value.min_value,v_max = entry.value.max_value;
  float zero_dist = fabs(.0-v_min);
  float distance = (float)fabs(v_max-v_min);
  float k = ((float)entry.descript.size.cx)/distance;
  int   curr_cx = (DWORD)(k*(float)fabs((entry.value.value - entry.value.min_value)));
  curr_cx = entry.descript.size.cx-curr_cx;
  grow_rect(r,-curr_cx,0);
  br = CreateSolidBrush(entry.descript.options.color);
  FillRect(Canvas->Handle,&r,br);
  DeleteObject(br);
  if((entry.value.max_value*entry.value.min_value)<0.0)
  {
   //Отрисовка 0
   curr_cx = (DWORD)(k*zero_dist);
   curr_cx+=r.left;
   int old_rop = SetROP2(Canvas->Handle,R2_NOTXORPEN);
   MoveToEx(Canvas->Handle,curr_cx,r.top,NULL);
   LineTo(Canvas->Handle,curr_cx,r.bottom);
   SetROP2(Canvas->Handle,old_rop);
  }
 }

 
 void         __fastcall TKadrForm::paint_analog_text(kadr_entry & entry,bool flash )
 {
  RECT r;
  set_rect(r,entry.descript.position,entry.descript.size);
  Canvas->Brush->Color = (TColor)entry.descript.options.bkgnd_color;
  Canvas->FillRect(r);
  if(!flash)
  {

  TColor text_color;
  text_color = entry.descript.options.color ? (TColor)entry.descript.options.color : clAqua;
  if(entry.value.diag )
     text_color = clGray;
    else
    {
     if(entry.value.state&MDBR_STATE_ALARM)
       text_color = clRed;
       else
     if(entry.value.state&MDBR_STATE_DANGER)
       text_color = clYellow;

    }

  char format[32];
  DWORD width = entry.descript.options.digits;
  if(!width)
      {
       if((DWORD)entry.value.value)
         width = log10(fabs((double)entry.value.value));
       width = std::max((DWORD)2,width);
      }

  sprintf(format,"%%% d.%df",width,entry.descript.options.prec);
  char str[MAX_PATH];
  int len = sprintf(str,format,entry.value.value);
  SIZE sz;
  Canvas->Font->Color = text_color;
  GetTextExtentPoint(Canvas->Handle,str,len,&sz);
  int old_mode = SetBkMode(Canvas->Handle,TRANSPARENT);
  DrawText(Canvas->Handle,str,len,&r,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
  if(entry.descript.size.cx < sz.cx)
     entry.descript.size << sz;
  if(entry.value.diag )
    {
     Canvas->Font->Color = clRed;
     ExtTextOut(Canvas->Handle,r.left+entry.descript.size.cx/2,r.top,ETO_OPAQUE,NULL,"?",1,NULL);
    }
     SetBkMode(Canvas->Handle,old_mode);
  }
 }

 void         __fastcall TKadrForm::paint_entry(kadr_entry & entry,bool flash )
 {
  if(entry.value.state&MDBR_STATE_DISCRETE)
    paint_discrete(entry,flash);
    else
    paint_analog(entry,flash);
 }

void         __fastcall TKadrForm::paint_entry_frame(kadr_entry & entry)
{
     int old_rop = SetROP2(Canvas->Handle,R2_NOTXORPEN);
     RECT r;
     set_rect(r,entry.descript.position,entry.descript.size);
     move_rect(r,-3,-3);
     grow_rect(r,6,6);
     Canvas->MoveTo(r.left,r.top);
     Canvas->LineTo(r.right,r.top);
     Canvas->LineTo(r.right,r.bottom);
     Canvas->LineTo(r.left,r.bottom);
     Canvas->LineTo(r.left,r.top);
     SetROP2(Canvas->Handle,old_rop);
}


 void __fastcall TKadrForm::show_entry_info(kadr_entry * entry)
 {
  AnsiString str;
  TStatusPanel * panel = StatusBar1->Panels->Items[0];
  if(editable)
  {
  if(entry)
    {
     str.printf("id-%d №-%d X=%03d,Y=%03d Ширина=%d, высота=%d ",
                entry->value.rec_id,entry->descript.number,
                entry->descript.position.x,entry->descript.position.y,
                entry->descript.size.cx,entry->descript.size.cy);
     panel->Text = str;
    }
    else
    panel->Text = "";
    panel = StatusBar1->Panels->Items[1];
  }

  if(entry)
    {
     if(entry->lname.Length())
       panel->Text = entry->lname;
      else
      {
      if(entry->sname.Length())
       panel->Text = entry->sname;
       else
       {
        str.printf("Id %d",entry->value.rec_id);
        panel->Text = str;
       }
      }
    }
    else
    panel->Text = "";

 }

 void __fastcall TKadrForm::update_status_bar()
 {
  show_entry_info(&*selected_entry);
 }

 void __fastcall TKadrForm::select_entry(mdb_kadr_entry * entry)
 {

 entry_table::iterator beg = Fentry_items.begin(),end = Fentry_items.end();
 while(beg<end)
 {
  if(beg->descript.rec_id == entry->rec_id && beg->descript.number == entry->number)
   {
    this->select_entry(&*beg,false);
   }
  ++beg;
 }
 }

void         __fastcall TKadrForm::select_entry(kadr_entry * entry,bool notify)
{
   in_setup = true;
   //TStatusPanel * panel =  StatusBar1->Panels->Items[StatusBar1->Panels->Count-1];
   if(selected_entry != entry)
   {
    RECT r;
    if(&*selected_entry )
     {
      set_rect(r,selected_entry->descript.position,selected_entry->descript.size);
      move_rect(r,-4,-4);
      grow_rect(r,8,8);
      InvalidateRect(Handle,&r,TRUE);
     }
     selected_entry = entry;
     if(entry)
     {
      if(flash_counter&1)
         paint_entry_frame(*entry);
      paint_entry(*entry,flash_counter&1);
      EntryDrawAs->ItemIndex = entry->descript.options.paint_as;
     }
     if(notify && on_select_entry)
        on_select_entry(this,MDB_KADR_FIELD_ENTRYES,&*selected_entry ? &selected_entry->descript:NULL);
     DeleteBtn->Enabled = &*selected_entry ? true:false;
     update_status_bar();
   }
   in_setup = false;
}




void __fastcall TKadrForm::FormPaint(TObject *Sender)
{
 
 if(bkgnd_bitmap)
   {
    HANDLE old_dcbmp = SelectObject(MemoryDC,bkgnd_bitmap);
    BitBlt(Canvas->Handle,0,ToolBar1->Height,kadr_width,kadr_height,MemoryDC,0,0,SRCCOPY);
    SelectObject(MemoryDC,old_dcbmp);
   }
 entry_table::iterator beg = Fentry_items.begin(),end = Fentry_items.end();
 while(beg<end)
 {
  paint_entry(*beg);
  ++beg;
 }
}
//---------------------------------------------------------------------------

void __fastcall TKadrForm::WndProc(Messages::TMessage &Message)
{
  if(Message.Msg == WM_ERASEBKGND)
    {
      if(bkgnd_bitmap)Message.Result = TRUE;
      else
      TForm::WndProc(Message);
    }
    else
    TForm::WndProc(Message);
}

void __fastcall TKadrForm::add_entry(mdb_kadr_entry & ke,MDB_SHORT_RECORD &sr,AnsiString sname,AnsiString name)
{
  kadr_entry entry;
  if(ke.position.x<ToolBar1->Height)
     ke.position.x = ToolBar1->Height+1;
  if(ke.position.y<2)
     ke.position.y = 2;
  entry = ke;
  entry = sr;
  entry.sname = sname;
  entry.lname = name;
  entry_table::iterator ptr = Fentry_items.insert(std::lower_bound(Fentry_items.begin(),Fentry_items.end(),entry),entry);
  this->paint_entry(*ptr);
  //select_entry (ptr,false);
  Repaint();
}

void __fastcall TKadrForm::update_value(MDB_SHORT_RECORD &sr,DWORD mask)
{
 bool need_repaint = Visible;
 entry_table::iterator beg = Fentry_items.begin(),end = Fentry_items.end();
 while(beg<end)
 {
  if(beg->value.rec_id == sr.rec_id)
   {

    if((mask & MDBR_FIELD_VALUE) && (beg->value.state&MDBR_STATE_DISCRETE))
    {
      if(beg->switch_count || DWORD(beg->value.value)!=DWORD(sr.value))
      {
           ++beg->switch_count;
      }
    }
    *beg = sr;
    
    if(need_repaint)
       paint_entry(*beg);
   }
  ++beg;
 }
}

 void __fastcall TKadrForm::update_entry(mdb_kadr_entry * entry)
 {
 entry_table::iterator beg = Fentry_items.begin(),end = Fentry_items.end();
 while(beg<end)
 {
  if(beg->descript.rec_id == entry->rec_id && beg->descript.number == entry->number)
   {
    beg->descript = *entry;
    paint_entry(*beg,false);
    beg = end;
   }
  ++beg;
 }
 }



 void __fastcall TKadrForm::set_entry_name(DWORD rec_id,AnsiString name,bool sname)
 {

  entry_table::iterator beg = Fentry_items.begin(),end = Fentry_items.end();
  while(beg<end)
  {
   if(beg->value.rec_id == rec_id)
   {
    if(sname)
      beg->sname = name;
      else
      {
      beg->lname = name;
      if(selected_entry == beg)
         StatusBar1->Panels->Items[0]->Text = name;
      }    
   }
  ++beg;
 }

 }



void __fastcall TKadrForm::FormResize(TObject *Sender)
{
  bool ch = false;
  DWORD kw = ClientWidth;
  DWORD kh = ClientHeight-StatusBar1->Height - ToolBar1->Height;
  if(Fkadr.kadr_size.cx != (long)kw)
    Fkadr.kadr_size.cx  = kw,ch = true;
  if(Fkadr.kadr_size.cy != (long)kh)
    Fkadr.kadr_size.cy   = kh,ch = true;
  if(ch)
  {
   setup_bkgnd();
   Repaint();
   changes|=MDB_KADR_FIELD_SIZE;
  }
}
//---------------------------------------------------------------------------

TKadrForm::entry_table::iterator __fastcall TKadrForm::find_entry(int x,int y)
{
 entry_table::iterator ret = NULL;
 entry_table::iterator beg = Fentry_items.begin(),end = Fentry_items.end();
 POINT pt; pt.x = x;pt.y = y;
 while(beg<end && !&*ret)
 {
  RECT r;
  set_rect(r,beg->descript.position,beg->descript.size);
  if(PtInRect(r,pt))
     {
      ret = beg;
     }
     else
  ++beg;
 }
 return  ret;
}

void __fastcall TKadrForm::FormMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
 if(Button == mbLeft)
   {
    lb_down = true;
    spot.x = X;
    spot.y = Y;

   } 
 if(Button == mbRight)
   rb_down = true;
 entry_table::iterator entry = find_entry(X,Y);
 select_entry(&*entry);
}
//---------------------------------------------------------------------------



void __fastcall TKadrForm::FormMouseUp(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
 if(Button == mbLeft)
   {
    lb_down = FALSE;
    if(mouse_captured)
      {
       ReleaseCapture();
       mouse_captured = false;
      } 
   } 
 if(Button == mbRight)
   rb_down = FALSE;

}
//---------------------------------------------------------------------------

void __fastcall TKadrForm::FormMouseMove(TObject *Sender,
      TShiftState Shift, int X, int Y)
{
 if(&*selected_entry && lb_down && editable)
 {
 if(!mouse_captured)
  {
   SetCapture(Handle);
   mouse_captured = true;
  }
  POINT delta ;
  int dx = X-spot.x;
  int dy = Y-spot.y;
  spot.x  = X;
  spot.y  = Y;

  if(delta.x || delta.y)
  {
  if(Shift.Contains(ssShift))
    grow_entry(*selected_entry,dx,dy);
  else
   move_entry(*selected_entry,dx,dy);
  }
 }
 else
 {
  entry_table::iterator entry = find_entry(X,Y);
  if(over_entry!=entry)
  {
    over_entry = entry;
    show_entry_info(&*over_entry ? &*over_entry : &*selected_entry);
  }
 }

}
//---------------------------------------------------------------------------

 void __fastcall TKadrForm::move_entry(kadr_entry & entry,int dx,int dy)
 {
   RECT inv_rect;
   POINT new_position;
   SIZE  new_size;
   new_position << entry.descript.position;
   new_size <<entry.descript.size;
   new_position.x+=dx;
   new_position.y+=dy;
   RECT new_rect,old_rect;
   ::set_rect(new_rect,new_position,new_size);
   set_rect(old_rect,entry.descript.position,entry.descript.size);
   union_rect(old_rect,new_rect,inv_rect);
   entry.descript.position << new_position;
   move_rect(inv_rect,-4,-4);
   grow_rect(inv_rect,8,8);
   InvalidateRect(Handle,&inv_rect,false);
   paint_entry(*selected_entry,false);
   paint_entry_frame(*selected_entry);
   set_changes(MDB_KADR_FIELD_ENTRYES,&entry.descript);
   update_status_bar();

 }

 void __fastcall TKadrForm::grow_entry(kadr_entry & entry,int dx,int dy)
 {
    RECT inv_rect;
    set_rect(inv_rect,entry.descript.position, entry.descript.size);
    entry.descript.size.cx+=dx;
    entry.descript.size.cy+=dy;
    move_rect(inv_rect,-4,-4);
    grow_rect(inv_rect,8,8);
    InvalidateRect(Handle,&inv_rect,false);
    paint_entry(*selected_entry,false);
    paint_entry_frame(*selected_entry);
    set_changes(MDB_KADR_FIELD_ENTRYES,&entry.descript);
    update_status_bar();
 }



void __fastcall TKadrForm::FormClose(TObject *Sender, TCloseAction &Action)
{
painter_dll.unload_module();
 Action = caFree;
 if(MemoryDC)
   DeleteDC(MemoryDC);

if(dmask_bmp) FreeImage(dmask_bmp);
if(doff_bmp ) FreeImage(doff_bmp );
if(don_bmp  ) FreeImage(don_bmp  );

}
//---------------------------------------------------------------------------

void __fastcall TKadrForm::Timer1Timer(TObject *Sender)
{
 if(&*selected_entry && !mouse_captured)
 {
  paint_entry_frame(*selected_entry);
 }
 entry_table::iterator beg = Fentry_items.begin(),end =Fentry_items.end();
 while(beg<end)
 {
  if(beg->switch_count && (beg->value.state&MDBR_STATE_DISCRETE))
    paint_entry(*beg,(flash_counter&1));
   ++beg;
 }
 ++flash_counter;
}
//---------------------------------------------------------------------------

void __fastcall TKadrForm::ReceiptBtnClick(TObject *Sender)
{
 /*Квитирование*/
 entry_table::iterator beg = Fentry_items.begin(),end =Fentry_items.end();
 while(beg<end)
 {
  if(beg->value.state&MDBR_STATE_DISCRETE)
     {
      beg->descript.options.normal_value  = (BYTE)beg->value.value;
      beg->switch_count = 0;
      paint_entry(*beg,false);
     }
   ++beg;
 }

}
//---------------------------------------------------------------------------




void __fastcall TKadrForm::DeleteBtnClick(TObject *Sender)
{
  if(&*selected_entry)
   {
    if(this->on_delete_entry)
      on_delete_entry(this,MDB_KADR_FIELD_ENTRYES,&selected_entry->descript);
    Fentry_items.erase(selected_entry);
    changes |= MDB_KADR_FIELD_ENTRYES;
    if(selected_entry<this->Fentry_items.end() && on_select_entry)
       {
        kadr_entry * e = &*selected_entry;
        selected_entry = 0;
        over_entry     = 0;
        select_entry(e,true);
       }
       else
       select_entry(0,true);
    Repaint();
   }
}
//---------------------------------------------------------------------------

void __fastcall TKadrForm::ApplyBtnClick(TObject *Sender)
{
  if(on_apply)
     on_apply(this,0,0);      
}
//---------------------------------------------------------------------------

void __fastcall TKadrForm::UndoBtnClick(TObject *Sender)
{
 if(on_undo)
    on_undo(this,0,0);       
}
//---------------------------------------------------------------------------

void __fastcall TKadrForm::ToolButton4Click(TObject *Sender)
{
 if(record_operations && &*selected_entry)
    record_operations(this,selected_entry->descript.rec_id,MDBR_OPERATION_TUON_MORE);
}
//---------------------------------------------------------------------------

void __fastcall TKadrForm::ToolButton5Click(TObject *Sender)
{
 if(record_operations && &*selected_entry)
    record_operations(this,selected_entry->descript.rec_id,MDBR_OPERATION_TUOFF_LESS);
}
//---------------------------------------------------------------------------


void __fastcall TKadrForm::FormKeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
 int dx = 0,dy = 0;
 switch(Key)
 {
  case VK_LEFT : dx = -1;
   break ;
  case VK_RIGHT: dx = 1;
  break;
  case VK_DOWN:  dy = 1;
  break;
  case VK_UP:    dy = -1;
  break;
 }

 if(dx || dy)
 {
 if(&*selected_entry)
 {
  if(Shift.Contains(ssShift))
    grow_entry(*selected_entry,dx,dy);
    else
    {
    if(Shift.Contains(ssCtrl))
      move_entry(*selected_entry,dx,dy);
    }

 }
 else
 select_entry( &*Fentry_items.begin());
 Key = 0;
 }
 

}
//---------------------------------------------------------------------------

void __fastcall TKadrForm::EntryDrawAsChange(TObject *Sender)
{
  if(&*selected_entry)
  {
   selected_entry->descript.options.paint_as = EntryDrawAs->ItemIndex;
   paint_entry(*selected_entry,flash_counter&1);
   set_changes(MDB_KADR_FIELD_ENTRYES,&selected_entry->descript);
   
   
  }
}
//---------------------------------------------------------------------------

