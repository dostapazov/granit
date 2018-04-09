//---------------------------------------------------------------------------
#pragma hdrstop
#include <vcl.h>
#include "entry_props.h"
#include <tlm_painter.hpp>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TEntryProps *EntryProps;
//---------------------------------------------------------------------------
__fastcall TEntryProps::TEntryProps(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
__fastcall TEntryProps::TEntryProps(TComponent* Owner,int pctx,DWORD _notify_command)
	: TForm(Owner)
        , notify_command(_notify_command)
        , Fdiscrete(false)
        , Fpaint_context(pctx)
        , Fin_setup     (0)
{
  bzero(&Fentry,sizeof(Fentry));

}
 void __fastcall TEntryProps::set_discrete(bool val)
 {
   if(Fdiscrete != val)
     {
      Fdiscrete  = val;
      get_paint_list();
      Height = val ? Constraints->MinHeight : Constraints->MaxHeight;
      Label2->Caption = val ? L"Цвет OFF":L"Цвет фона";
      Label3->Caption = val ? L"Цвет  ON":L"Цвет текста";

      Panel1->Visible = val;
      Label5->Visible = val;

      Panel2->Visible = !val;
      Panel3->Visible = !val;
      Label6->Visible = !val;
      Label7->Visible = !val;
     }
 }

 void __fastcall TEntryProps::set_entry   (mdb_kadr_entry val)
 {
   if(Fentry != val)
   {
     ++Fin_setup;
     Fentry = val;
     if(!paint_as->Items->Count) get_paint_list();
     paint_as->ItemIndex   = entry.options.paint_as;
     bk_color->Color       = (TColor)entry.options.bkgnd_color;
     fg_color->Color       = (TColor)entry.options.color;
     short_name->ItemIndex = (entry.options.paint_flags&PAINT_FLAGS_SHORT_NAMES);
     line_width->Position  = entry.options.line_width;
     part_of_background->Checked = entry.options.part_of_background;
     if(!discrete)
       {
         digits->Position    = entry.options.digits;
         precision->Position = entry.options.prec;
       }
     --Fin_setup;
   }
 }

 void __fastcall TEntryProps::update_paint_variants()
 {
   int save_idx = paint_as->ItemIndex;
   get_paint_list();
   paint_as->ItemIndex = save_idx;
 }

 void __fastcall TEntryProps::get_paint_list()
 {
   paint_as->Items->Clear();
   tlmpaint::painter_dll * pdll = tlmpaint::painter_dll::get_instance();
   if(pdll->is_loaded())
   {
       wchar_t name[512];
       int     idx = 0;
       bool    res ;
       do{
           res = discrete ? pdll->enum_discretes_ex(Fpaint_context,idx,name,KERTL_ARRAY_COUNT(name)):
                            pdll->enum_analogs_ex  (Fpaint_context,idx,name,KERTL_ARRAY_COUNT(name));
           if(res)
             {
              ++idx;
              paint_as->Items->Add(name);
             }
         }while(res);
       }

 }


void __fastcall TEntryProps::SelBkColorClick(TObject *Sender)
{
  TPanel * panel       = (Sender == SelBkColor) ?  bk_color : fg_color;
  LPDWORD  entry_color = (Sender == SelBkColor) ?  &entry.options.bkgnd_color : &entry.options.color;
  ColorDialog1->Color = panel->Color;
  if(ColorDialog1->Execute(Handle))
   {
    panel->Color =  ColorDialog1->Color;
    *entry_color =  ColorDialog1->Color;
    notify_changes();
   }

}
//---------------------------------------------------------------------------

void __fastcall TEntryProps::notify_changes()
{
   if(notify_command && !Fin_setup)
   {
    TWinControl * wc = dynamic_cast<TWinControl*>(Owner);
    if(wc)
       SendMessage(wc->Handle,notify_command,(WPARAM)this,(LPARAM)&entry);
   }
}

void __fastcall TEntryProps::paint_asChange(TObject *Sender)
{
     entry.options.paint_as = paint_as->ItemIndex;
     notify_changes();
}
//---------------------------------------------------------------------------
void __fastcall TEntryProps::short_nameClick(TObject *Sender)
{
     entry.options.paint_flags &=~PAINT_FLAGS_SHORT_NAMES;
     entry.options.paint_flags |= (short_name->ItemIndex&PAINT_FLAGS_SHORT_NAMES);
     notify_changes();
}
//---------------------------------------------------------------------------
void __fastcall TEntryProps::digitsChangingEx(TObject *Sender, bool &AllowChange,
          int NewValue, TUpDownDirection Direction)
{
  TUpDown * ctrl = dynamic_cast<TUpDown *>(Sender);
  if(ctrl)
  {
    if(Sender == digits )
      entry.options.digits = NewValue;
      else
      entry.options.prec   = NewValue;
      notify_changes();
  }
}
//---------------------------------------------------------------------------
void __fastcall TEntryProps::FormHide(TObject *Sender)
{
   entry.rec_id = entry.number = -1;
}
//---------------------------------------------------------------------------

void __fastcall TEntryProps::line_widthChangingEx(TObject *Sender, bool &AllowChange,
          int NewValue, TUpDownDirection Direction)
{
  entry.options.line_width = NewValue;
  notify_changes();
}
//---------------------------------------------------------------------------

void __fastcall TEntryProps::part_of_backgroundClick(TObject *Sender)
{
      entry.options.part_of_background = part_of_background->Checked;
      notify_changes();
}
//---------------------------------------------------------------------------

