//---------------------------------------------------------------------------

#pragma hdrstop
#include <vcl.h>
#include "tchart_frame.h"
#include <VCLTee.Series.hpp>
#include <VCLTee.TeeSpline.hpp>
#include <System.DateUtils.hpp>
#include <limits.h>
#include "gkawpmodule.hpp"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TTChartFrame *TChartFrame;
//---------------------------------------------------------------------------
__fastcall TTChartFrame::TTChartFrame(TComponent* Owner)
	: TFrame(Owner),Fmodule(NULL)
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
          mdb_record rec;
          TFastLineSeries * fls  = dynamic_cast<TFastLineSeries *>(ptr->second);
          if(fls && module->get_record(ptr->first,rec) && !rec.is_discrete())
          {
           fls->Stairs = enable;
          }
          ++ptr;
       }

 }


  bool           __fastcall TTChartFrame::remove_series  (DWORD rec_id)
  {
     if(rec_series.count(rec_id))
     {
      TChartSeries * ser = rec_series[rec_id];
      rec_series.erase(rec_id);
      Chart1->RemoveSeries(ser);
      delete ser;
     }
    return false;
  }

  TChartSeries * __fastcall TTChartFrame::get_series  (DWORD rec_id)
  {
      TChartSeries *& ret = rec_series[rec_id];
      if(!ret && module)
       {
          TFastLineSeries * fls = new TFastLineSeries(Chart1);
          fls->XValues->DateTime = true;
          ret = fls;
          mdb_record rec;
          if(module->get_record(rec_id,rec))
          {
            UnicodeString  str;
            module->get_string(rec.name,str);
            if(str.IsEmpty()) str.printf(L"record id %d",rec_id);
            fls->Title  = str;
            fls->Stairs = rec.is_discrete();
          }

          Chart1->AddSeries(fls);
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
         mdb_record rec;
         if(module->get_record(ptr->first,rec))
            {
             //TChartSeries *  s = ptr->second;
             double value = rec.value;

             if(rec.is_discrete())
             {
              double ampl = Chart1->LeftAxis->Maximum - Chart1->LeftAxis->Minimum;
              value *= (ampl*.2);
             }
             add_value(rec.rec_id,dtm.Val,value,rec.diag ? true : false);
            }
         ++ptr;
       }
   }

  void          __fastcall TTChartFrame::adjust_points(TChartSeries * s,int limit,int interval)
  {

       if(s->XValues->Count<limit)
       {
           TDateTime dtm = TDateTime::CurrentDateTime() ;
           dtm = IncMilliSecond(dtm,-limit*interval);
           while(s->XValues->Count<limit)
           {
             s->AddNullXY(dtm.Val,0);
             dtm = IncMilliSecond(dtm,interval);
           }
       }
       else
       while(s->XValues->Count>limit)
       {
          s->Delete(0);
       }
  }

  void          __fastcall TTChartFrame::adjust_points(int limit,int interval)
  {
       Tseries::iterator ptr = rec_series.begin();
       Tseries::iterator end = rec_series.end  ();
       while(ptr!=end)
       {
         adjust_points(ptr->second,limit,interval);
         ++ptr;
       }
  }



