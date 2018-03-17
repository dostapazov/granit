//---------------------------------------------------------------------------
#pragma hdrstop
#include <vcl.h>
#include "trend_forms.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "tchart_frame"
#pragma resource "*.dfm"
TTrendForm *TrendForm;
//---------------------------------------------------------------------------
__fastcall TTrendForm::TTrendForm(TComponent* Owner)
	: TForm(Owner)
{
 this->set_time_long(60);
}
//---------------------------------------------------------------------------
void __fastcall TTrendForm::TChartFrame1Chart1DragOver(TObject *Sender, TObject *Source,
          int X, int Y, TDragState State, bool &Accept)
{
    Accept = true;
}
//---------------------------------------------------------------------------
void __fastcall TTrendForm::add_selected(TTlmKadrViewer * kadr_viewer)
{
  if(kadr_viewer)
  {
       trecords_index ri;
       kadr_viewer->get_selected_recid(ri);
       if(ri.size())
       {
       trecords_index::iterator ptr = ri.begin();
       trecords_index::iterator end = ri.end();
           while(ptr<end)
           {
             chart_frame->get_series(*ptr);
             ++ptr;
           }
        chart_frame->add_shear();
        Timer1->Enabled = true;
       }

  }
}

//---------------------------------------------------------------------------

void __fastcall TTrendForm::TChartFrame1Chart1DragDrop(TObject *Sender, TObject *Source,
          int X, int Y)
{
  UnicodeString str;
  if(Source)
  {
   TRecDrag * rd = dynamic_cast<TRecDrag *>(Source);
   if(rd)
     {
       this->add_selected(rd->kadr_viewer);
     }
  }
}
//---------------------------------------------------------------------------
void __fastcall TTrendForm::Timer1Timer(TObject *Sender)
{
  //Добавление среза
  chart_frame->add_shear();
  chart_frame->remove_points(points_limit);

}
//---------------------------------------------------------------------------
void __fastcall TTrendForm::FormClose(TObject *Sender, TCloseAction &Action)
{
  Action = caFree;
}
//---------------------------------------------------------------------------
void __fastcall TTrendForm::tbHideLegendClick(TObject *Sender)
{
  //tbHideLegend->Down = !tbHideLegend->Down;
  chart_frame->legen_visible = !tbHideLegend->Down;
  //tbHideLegend->Down = chart_frame->legen_visible;
}
//---------------------------------------------------------------------------
void __fastcall TTrendForm::tbAxisAutomaticClick(TObject *Sender)
{
  chart_frame->axis_automatic = tbAxisAutomatic->Down;
}
//---------------------------------------------------------------------------
void __fastcall TTrendForm::ToolButton1Click(TObject *Sender)
{
  Timer1->Interval = std::max(10,_wtoi(ShearFreq->Text.c_str()));
  ShearFreq->Text  = Timer1->Interval;
  set_time_long(std::max(1,_wtoi(TimeLong->Text.c_str())));

}
//---------------------------------------------------------------------------

 void __fastcall TTrendForm::set_time_long(int sec)
 {
   TimeLong->Text = sec;
   points_limit = (sec*1000)/Timer1->Interval;
   chart_frame->remove_points(points_limit);

 }


void __fastcall TTrendForm::LineWidthChangingEx(TObject *Sender, bool &AllowChange,
          int NewValue, TUpDownDirection Direction)
{
  chart_frame->set_pen_width(NewValue);
}
//---------------------------------------------------------------------------

void __fastcall TTrendForm::SpeedButton1Click(TObject *Sender)
{
 chart_frame->set_stairs(SpeedButton1->Down);
}
//---------------------------------------------------------------------------

