//---------------------------------------------------------------------------

#pragma hdrstop
#include <vcl.h>
#include "tchart_frame.h"
#include <VCLTee.Series.hpp>
#include <VCLTee.TeeSpline.hpp>
#include <limits.h>
#include "gktlm_viewer_mod.hpp"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TTChartFrame *TChartFrame;
//---------------------------------------------------------------------------
__fastcall TTChartFrame::TTChartFrame(TComponent* Owner)
	: TFrame(Owner),Fviewer_module(NULL)
{
 min_value = LLONG_MAX;
 max_value = LLONG_MIN;
}
//---------------------------------------------------------------------------
  void          __fastcall TTChartFrame::set_axis_automatic(bool v)
  {
    Chart1->LeftAxis->Automatic = v;
    if(v)
    {
     min_value = LLONG_MAX;
     max_value = LLONG_MIN;
    }
  }

  bool          __fastcall TTChartFrame::get_axis_automatic()
  {
    return Chart1->LeftAxis->Automatic;
  }

  void          __fastcall TTChartFrame::set_legend_visible(bool v)
  {
    Chart1->Legend->Visible = v;
  }

  bool          __fastcall TTChartFrame::get_legend_visible()
  {
    return Chart1->Legend->Visible;
  }

  void          __fastcall TTChartFrame::set_pen_width     (int w)
  {
       Tseries::iterator ptr = rec_series.begin();
       Tseries::iterator end = rec_series.end  ();
       while(ptr!=end)
       {
          ptr->second->Pen->Width = w;
          ++ptr;
       }
  }

 void          __fastcall TTChartFrame::set_stairs        (bool enable)
 {
       Tseries::iterator ptr = rec_series.begin();
       Tseries::iterator end = rec_series.end  ();
       while(ptr!=end)
       {
          mdb_record_ex rec;
          TFastLineSeries * fls  = dynamic_cast<TFastLineSeries *>(ptr->second);
          if(fls && viewer_module->__get_record(ptr->first,false,rec) && !rec.is_discrete())
          {
           fls->Stairs = enable;
          }
          ++ptr;
       }

 }


  TChartSeries * __fastcall TTChartFrame::get_series  (DWORD rec_id)
  {
      TChartSeries *& ret = rec_series[rec_id];
      if(!ret && viewer_module)
       {
          TFastLineSeries * fls = new TFastLineSeries(Chart1);
          fls->XValues->DateTime = true;
          ret = fls;
          mdb_record_ex rec;
          if(viewer_module->__get_record(rec_id,false,rec))
          {
            UnicodeString  str;
            viewer_module->__get_text  (rec.name,str);
            if(str.IsEmpty()) str.printf(L"record id %d",rec_id);
            fls->Title  = str;
            fls->Stairs = rec.is_discrete();
          }

          this->Chart1->AddSeries(fls);
       }
     return ret;
  }

  void          __fastcall TTChartFrame::add_value   (DWORD rec_id,double time,double value,bool bad)
  {
       TChartSeries *  s = this->get_series(rec_id);

       if(bad)
       {
       s->AddXY(time,value,UnicodeString(),clGray);
       }
       else
       s->AddXY(time,value);

       min_value = std::min(min_value,value);
       max_value = std::max(max_value,value);
       double ampl = fabs(max_value - min_value);
       double add  = std::max(1.0,ampl*0.05);
       double _min_val = min_value-add;
       double _max_val = max_value+add;
       if(_min_val>Chart1->LeftAxis->Maximum)
       {
         Chart1->LeftAxis->Maximum = _max_val;
         Chart1->LeftAxis->Minimum = _min_val;
       }
       else
       {
        Chart1->LeftAxis->Minimum = _min_val;
        Chart1->LeftAxis->Maximum = _max_val;
       }
  }

   void          __fastcall TTChartFrame::add_shear   ()
   {
       TDateTime dtm = TDateTime::CurrentDateTime();
       Tseries::iterator ptr = rec_series.begin();
       Tseries::iterator end = rec_series.end  ();

       while(ptr!=end)
       {
         mdb_record_ex rec;
         if(viewer_module->__get_record(ptr->first,false,rec))
            {
             //TChartSeries *  s = ptr->second;
             double value = rec.value;
             if(rec.is_discrete()) value *=20.0;
                add_value(rec.rec_id,dtm.Val,value,rec.diag ? true : false);
            }
         ++ptr;
       }
   }

  void          __fastcall TTChartFrame::remove_points(TChartSeries * s,int limit)
  {
       while(s->YValues->Count>limit)
         {
          s->Delete(0);
         }
  }

  void          __fastcall TTChartFrame::remove_points(int limit)
  {
       Tseries::iterator ptr = rec_series.begin();
       Tseries::iterator end = rec_series.end  ();
       while(ptr!=end)
       {
         remove_points(ptr->second,limit);
         ++ptr;
       }

  }



