//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "tlm_painter_frm.h"
#include "tlmpainter.hpp"
#include "pictdlgs.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm2 *Form2;
//---------------------------------------------------------------------------
__fastcall TForm2::TForm2(TComponent* Owner)
        : TForm(Owner),painter(NULL)
{
 painter.painter_dll.load_module("tlmobj_std_painter.dll");
}
//---------------------------------------------------------------------------
void __fastcall TForm2::FormShow(TObject *Sender)
{
 RECT r = Panel1->ClientRect;
 mdb_kadr * kadr = new mdb_kadr(1,-1,-1);
 ClientWidth  = 450;
 ClientHeight = 400;
 kadr->kadr_size.cx = 450;
 kadr->kadr_size.cy = 400;
 kadr->kadr_flags   = MDB_KADR_FLAG_ IMAGE_STRETCH_PROPORTIONAL;

 mdb_kadr_entry e(1,1,1);
 e.options.prec = 2;
 e.rec_id   = 1;
 e.position.x = 10;
 e.position.y = 10;

 e.size  << *&KeWin::TSize(60,22);

 painter.add_entry(e);

 e.rec_id = 2;
 e.position.x = 10;
 e.position.y = 40;
 e.size       << *&KeWin::TSize(60,22);
 e.options.paint_as = 0;
 painter.add_entry(e);

 e.rec_id = 3;
 e.position.x = 10;
 e.position.y = 70;
 e.size       << *&KeWin::TSize(60,22);
 painter.add_entry(e);

 e.rec_id = 4;
 e.position.x = 10;
 e.position.y = 100;
 e.size       << *&KeWin::TSize(60,22);
 painter.add_entry(e);

 e.rec_id = 5;
 e.position.x = 10;
 e.position.y = 130;
 e.options.paint_as = TA_PAINT_AS_HBAR;
 e.size       << *&KeWin::TSize(60,22);
 painter.add_entry(e);

 e.options.paint_as = 0;



 e.rec_id = 30;
 e.position.x = 100;
 e.position.y = 10;
 e.size       << *&KeWin::TSize(60,22);
 painter.add_entry(e);

 e.rec_id = 40;
 e.position.x = 100;
 e.position.y = 40;
 e.size       << *& KeWin::TSize(60,22);
 e.options.color = clYellow;
 painter.add_entry(e);

 e.rec_id = 41;
 e.position.x = 100;
 e.position.y = 70;
 e.size       << *& KeWin::TSize(60,22);
 e.options.color = clYellow;
 painter.add_entry(e);

 e.rec_id = 42;
 e.position.x = 100;
 e.position.y = 100;
 e.size       << *& KeWin::TSize(60,22);
 e.options.color = clYellow;
 painter.add_entry(e);

 e.rec_id = 43;
 e.position.x = 100;
 e.position.y = 130;
 e.size       << *& KeWin::TSize(60,22);
 e.options.color = clYellow;
 painter.add_entry(e);

 e.rec_id = 44;
 e.position.x = 150;
 e.position.y = 180;
 e.size       << *& KeWin::TSize(60,22);
 e.options.color = clRed;
 painter.add_entry(e);

 painter.enable_edit(true);
 painter.Create(Panel1->Handle,"",r,-1,NULL,WS_CHILD|WS_TABSTOP,WS_EX_CONTROLPARENT);
 painter.SetFlags(FLWND_DOUBLE_BUFFERED,true);
 painter.set_kadr(kadr);
 wstring iname = L"D:\\USER\\IMAGE\\IMG\\Shed in Field.jpg";
 painter.set_bkgnd(iname);
 painter.set_getter_text(get_wtext,this);
 painter.set_getter_record(get_record,this);
 painter.set_on_select(on_select_entry,this);
 painter.ShowWindow(SW_SHOW);
 painter.SetFocus();


}

//---------------------------------------------------------------------------

string __stdcall TForm2::get_text  (LPVOID data,TTlmPainter*,DWORD id)
{
 char text[MAX_PATH];
 wsprintfA(text,"строка %d",id);
 return string(text);
}
wstring __stdcall TForm2::get_wtext  (LPVOID data,TTlmPainter*,DWORD id)
{
 wchar_t text[MAX_PATH];
 wsprintfW(text,L"строка %d",id);
 return wstring(text);
}

DWORD  __stdcall TForm2::get_record(LPVOID data,TTlmPainter*,mdb_record_ex * rec)
{
 if(rec)
 {
  rec->diag  = 0;
  switch(rec->rec_id)
  {
   case 1:
        rec->addr.addr.pu = 10;
        rec->addr.addr.cp = 20;
        rec->addr.addr.fa = 1;
        rec->addr.addr.sb = 1;
        rec->addr.param_number = 1;
        rec->value = random(20000);
        rec->value/=100.0;

        break;
   case 2:
   case 3:
   case 4:
   case 5:
        rec->addr.addr.pu = 10;
        rec->addr.addr.cp = 20;
        rec->addr.addr.fa = 1;
        rec->addr.addr.sb = 1;
        rec->addr.param_number = 2;
        rec->max_value  = 1000;
        rec->value = random(rec->max_value);
        break;
   case 30:
        rec->addr.addr.pu = 10;
        rec->addr.addr.cp = 20;
        rec->addr.addr.fa = 0;
        rec->addr.addr.sb = 1;
        rec->addr.param_number = 1;
        rec->value = random(100)&1;
        rec->state |= MDBR_STATE_DISCRETE;
        break;

   case 40:
   case 41:
   case 42:
   case 43:

        rec->addr.addr.pu = 10;
        rec->addr.addr.cp = 20;
        rec->addr.addr.fa = 0;
        rec->addr.addr.sb = 1;
        rec->addr.param_number = 2;
        rec->value = random(100)&1;
        rec->state |= MDBR_STATE_DISCRETE|MDBR_STATE_TUFLAG;
        break;

   default: break;
  }
  return rec->rec_id;
 }
 return -1;
}

DWORD  __stdcall TForm2::on_select_entry(LPVOID data,TTlmPainter* painter,DWORD offs)
{
 TForm2 * form = reinterpret_cast<TForm2*>(data);
 if(form)
 {
  form->ToolButton6->Enabled = form->painter.get_selected_count() >1 ? true:false;
  AnsiString str;
  if(painter->is_selected(offs))
  {
    mdb_kadr_entry e;
    painter->get_kadr_entry(offs,e);
    string entry_text =
    form->get_text(data,painter,offs);
    str.printf("x:%03d y:%03d w:%03d h:%03d  %s ",e.position.x,e.position.y,e.size.cx,e.size.cy,
    entry_text.c_str());
  }
  form->StatusBar1->Panels->Items[0]->Text = str;
 }
 return TRUE;
}

void __fastcall TForm2::N2Click(TObject *Sender)
{
 Application->Terminate();
}
//---------------------------------------------------------------------------

void __fastcall TForm2::FormDestroy(TObject *Sender)
{
  this->painter.Destroy();
}
//---------------------------------------------------------------------------

void __fastcall TForm2::Panel1Resize(TObject *Sender)
{
  //int cxf     = GetSystemMetrics(SM_CXFRAME);
  //int cyf     = GetSystemMetrics(SM_CYFRAME);
  //int cy_capt = GetSystemMetrics(SM_CYCAPTION);
  painter.SetSize(Panel1->Width/*-(cxf<<1)*/,Panel1->Height/*-(cyf<<1)*/);
}
//---------------------------------------------------------------------------




void __fastcall TForm2::SpeedButton1Click(TObject *Sender)
{
 painter.set_bkgnd_mode(MDB_KADR_FLAG_IMAGE_STRETCH_PROPORTIONAL);
}
//---------------------------------------------------------------------------

void __fastcall TForm2::SpeedButton2Click(TObject *Sender)
{
 painter.set_bkgnd_mode(MDB_KADR_FLAG_IMAGE_CENTER);
       
}
//---------------------------------------------------------------------------

void __fastcall TForm2::SpeedButton3Click(TObject *Sender)
{
 painter.set_bkgnd_mode(MDB_KADR_FLAG_IMAGE_STRETCH);
}
//---------------------------------------------------------------------------

void __fastcall TForm2::ToolButton1Click(TObject *Sender)
{
 char  f_name[MAX_PATH<<2];
 if(OpenPicture(Handle,f_name,sizeof(f_name),"¬ыбор фонового изображени€",NULL,NULL,TRUE)==0)
 {
  string str(f_name);
  painter.set_bkgnd(str);
 }
}
//---------------------------------------------------------------------------



void __fastcall TForm2::EnableGridClick(TObject *Sender)
{
 TMenuItem * mi = dynamic_cast<TMenuItem *>(Sender);
 if(mi || !mi->Tag)
   {
    if(mi && mi->Tag)
     painter.enable_show_grid(true);
     painter.set_grid_step(mi->Tag);
   }
   else
   painter.enable_show_grid(false);

}
//---------------------------------------------------------------------------



void __fastcall TForm2::ToolButton5Click(TObject *Sender)
{
  painter.enable_edit(!painter.is_edit_enable());

}
//---------------------------------------------------------------------------

void __fastcall TForm2::N11Click(TObject *Sender)
{
  TMenuItem * mi = dynamic_cast<TMenuItem *>(Sender);
  if(mi )
    painter.arrange_selected((how_arrange) mi->Tag);
}
//---------------------------------------------------------------------------

