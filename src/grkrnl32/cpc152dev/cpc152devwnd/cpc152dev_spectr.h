//---------------------------------------------------------------------------

#ifndef cpc152dev_spectrH
#define cpc152dev_spectrH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include "gkmodule_form.h"
#include <Vcl.ComCtrls.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.ToolWin.hpp>
#include <VCLTee.Chart.hpp>
#include <VCLTee.Series.hpp>
#include <VclTee.TeeGDIPlus.hpp>
#include <VCLTee.TeEngine.hpp>
#include <VCLTee.TeeProcs.hpp>
#include <Vcl.Buttons.hpp>
#include <complex>
#include <fftw3.h>
#include "cpc152dev.h"
#include "cpc152proto.h"
#include <map>
#include "dspl.h"

typedef std::complex<double> complex_t;
#define WM_USER_DRAW_SPECTR    (WM_USER+2000)
#define WM_USER_HANDLE_NOTIFY  (WM_USER+2001)

struct channel_data
{
  TLineSeries * series;
  int    scan_count;
  int    scan_size;
  double offs;
  double RMS,tempRMS;
  complex_t sum;
  double   c_arg;
  double   c_abs;
  double   last_value;
  double *values;
  double *prev_values;
  int resample_size;
  int rs_sz_n;
  double * rs_values;
  channel_data():series(0),values(0),scan_count(0),scan_size(0),rs_values(0){};
  channel_data(TLineSeries * s):series(s),values(0),scan_count(0),scan_size(0),rs_values(0){}
  channel_data(const channel_data & src):series(src.series),values(0),scan_count(0),scan_size(0),rs_values(0){}
  channel_data & operator = (const channel_data & src) {series = src.series; scan_count = (0);scan_size =(0); return *this;}
  ~channel_data(){alloc_buffer(0);}
  void __fastcall  alloc_buffer(int sz)
  {
   if(rs_values) delete [] rs_values;
   if(values)
   {
    delete [] values;
    delete [] prev_values;
   }
   scan_size = sz;
   scan_count= 0;
   if(scan_size)
   {
    prev_values = new double[scan_size];
    values      = new double[scan_size];
   }
  };

  bool __fastcall put(double val);
  void __fastcall update_series(int resample = 0);

};

typedef std::map<sotd_param_addr,channel_data> series_map_t;




//---------------------------------------------------------------------------
class TCpc152SignalSpectr : public TGKModuleForm
{
__published:	// IDE-managed Components
	TStatusBar *StatusBar1;
	TPanel *Panel1;
	TSplitter *Splitter1;
	TChart *SignalChart;
	TChart *SpectrChart;
	TTeeGDIPlus *TeeGDIPlus1;
	TPanel *Panel2;
	TPanel *Panel3;
	TLabel *Label1;
	TSpeedButton *SpeedButton1;
	TSpeedButton *SpeedButton2;
	TLabel *Label2;
	TEdit *DevNum;
	TEdit *GroupNum;
	TLabel *Label3;
	TLabel *Label4;
	TEdit *scan_count;
	TLabel *Label5;
	TEdit *DftFrame;
	TSpeedButton *SpeedButton3;
	TCheckBox *Pause;
	TComboBox *ParamNum;
	TUpDown *DftExp;
	TBarSeries *SpectrSeries;
	TSplitter *Splitter2;
	TChart *PhaseChart;
	TFastLineSeries *PhaseSeries;
	TSpeedButton *SpeedButton4;
	TLabel *Label6;
	TEdit *Const;
	TComboBox *DftWndType;
	TCheckBox *AutoDFT;
	TLabel *TmLen;
	TLabel *Label7;
	TEdit *FrqBeg;
	TEdit *FrqEnd;
	TEdit *SampleFreq;
	TLabel *RMS;
	TLabel *DPhase;
	TEdit *Resampl;
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall SpeedButton1Click(TObject *Sender);
	void __fastcall SpeedButton2Click(TObject *Sender);
	void __fastcall SpeedButton3Click(TObject *Sender);
	void __fastcall DftExpChangingEx(TObject *Sender, bool &AllowChange, int NewValue,
          TUpDownDirection Direction);
	void __fastcall SpeedButton4Click(TObject *Sender);
	void __fastcall scan_countChange(TObject *Sender);
private:	// User declarations
protected:
                int Fcp_number;
                series_map_t series_map;
         fftw_plan   dft_plan;
         double    * dft_input;
         complex_t * dft_result;
         double    * dft_window;

void     __fastcall after_set_gkhandle();
void     __fastcall on_module_state(GKHANDLE mod,DWORD state);
LRESULT  __fastcall on_gkhandle_notify (GKHANDLE from,LPNOTIFY_CODE nc,DWORD mask);
void     __fastcall on_module_config_change(GKHANDLE mod,LPMODULE_CONFIG_DATA mcd);
void     __fastcall handle_notify          (lpcpc152scan_data sd);
void     __fastcall alloc_dft_frame        (int dft_frame_size);
void     __fastcall do_dft(channel_data & cd);
static   double Gausse(double n, double frameSize,double Q);
static   double Hamming(double n, double frameSize);
static   double Hann(double n, double frameSize);
static   double BlackmannHarris(double n, double frameSize);
void __fastcall prepare_window(int sz,int dft_sz);
static HINSTANCE dspl_lib;
static HANDLE    dspl_obj;
virtual void __fastcall WndProc(TMessage & msg);
        void __fastcall do_draw_spectr();
int __fastcall get_resample(){return _wtoi(this->Resampl->Text.c_str());}

public:		// User declarations
	__fastcall TCpc152SignalSpectr(TComponent* Owner);
        __property int cp_number = {read = Fcp_number, write = Fcp_number};
        int __fastcall get_scan_count()   {return scan_count->Text.ToInt();}
        int __fastcall get_dev_num   ()   {return DevNum->Text.ToInt()  ;}
        int __fastcall get_grp_num   ()   {return GroupNum->Text.ToInt();}
        int __fastcall get_param_num ()   {return ParamNum->Text.ToInt();}
        int __fastcall get_dft_frame ()   {return DftFrame->Text.ToInt();}
};
//---------------------------------------------------------------------------
extern PACKAGE TCpc152SignalSpectr *Cpc152SignalSpectr;
//---------------------------------------------------------------------------
#endif
