//---------------------------------------------------------------------------

#ifndef tchart_frameH
#define tchart_frameH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.ToolWin.hpp>
#include <VCLTee.Chart.hpp>
#include <VclTee.TeeGDIPlus.hpp>
#include <VCLTee.TeEngine.hpp>
#include <VCLTee.TeeProcs.hpp>
#include <map>

class TGkAwpModule;
//---------------------------------------------------------------------------
class TTChartFrame : public TFrame
{
__published:	// IDE-managed Components
	TChart *Chart1;
private:	// User declarations
        TGkAwpModule * Fmodule;
public:		// User declarations
typedef std::map<DWORD,TChartSeries* > Tseries;
	        __fastcall TTChartFrame (TComponent* Owner);
  bool          __fastcall series_exist (DWORD rec_id);
  TChartSeries* __fastcall get_series   (DWORD rec_id);
  bool          __fastcall remove_series(DWORD rec_id);

  void          __fastcall add_value    (DWORD rec_id,double time,double value,bool bad);
  void          __fastcall add_shear    ();
  void          __fastcall adjust_points(TChartSeries * s,int limit,int interval);
  void          __fastcall adjust_points(int limit,int interval);

  void          __fastcall set_legend_visible(bool v);
  bool          __fastcall get_legend_visible();
  void          __fastcall set_axis_automatic(bool v);
  bool          __fastcall get_axis_automatic();
  void          __fastcall set_pen_width     (int w);
  void          __fastcall set_stairs        (bool enable);

  __property bool legen_visible    = {read = get_legend_visible,write = set_legend_visible};
  __property bool axis_automatic   = {read = get_axis_automatic,write = set_axis_automatic};
  __property TGkAwpModule * module = {read = Fmodule, write = Fmodule};

private:
Tseries rec_series;
double  min_value,max_value;
};
//---------------------------------------------------------------------------
extern PACKAGE TTChartFrame *TChartFrame;
//---------------------------------------------------------------------------

inline   bool          __fastcall TTChartFrame::series_exist(DWORD rec_id)
        {
          return this->rec_series.count(rec_id) ? true : false;
        }

#endif

