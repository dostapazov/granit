//---------------------------------------------------------------------------

#ifndef history_dataH
#define history_dataH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.ToolWin.hpp>
#include "gktlm_viewer_form.h"

#include <otdm_proto.h>
#include <Vcl.ExtCtrls.hpp>
#include <System.Actions.hpp>
#include <System.ImageList.hpp>
#include <Vcl.ActnList.hpp>
#include <Vcl.ImgList.hpp>
#include <Vcl.Menus.hpp>
//---------------------------------------------------------------------------
#include "comtrade.h"
#include <kertlio.hpp>
#include <storage.hpp>
#include <VCLTee.Chart.hpp>
#include <VclTee.TeeGDIPlus.hpp>
#include <VCLTee.TeEngine.hpp>
#include <VCLTee.TeeProcs.hpp>
#include <VCLTee.Series.hpp>
#include <VCLTee.TeeSpline.hpp>
#include <Vcl.Dialogs.hpp>
#include <map>


struct hist_stat_t
{
 DWORD   rec_id;
 double  rec_min,rec_max,rec_avg,rec_integral;
 QWORD   time_lenght;
 QWORD   prev_time;
 QWORD   curr_time;
 double  curr_value;
 int     count;
 int     changes;
 hist_stat_t(DWORD _rec_id = -1){rec_id = _rec_id;rec_min = 9999999.9;rec_max = -rec_min; rec_avg = rec_integral = curr_value = .0; time_lenght = prev_time = curr_time = 0;count = 0;changes = 0;}
 hist_stat_t(const hist_stat_t & hs){*this = hs;}
 hist_stat_t & operator = (const hist_stat_t & hs)
 {
  memcpy(this,&hs,sizeof(*this));
  return *this;
 }
 bool operator < (const hist_stat_t & hs) {return rec_id < hs.rec_id ? true : false; }
 void __fastcall set_value(DWORD rec_id,double value,QWORD tm);
};



typedef std::map<DWORD,hist_stat_t> hist_stats_map_t;



#define HDCM_HANDLE_ARCHIVE  WM_USER+1000
typedef std::map<DWORD,TChartSeries* > Tseries;


typedef void __fastcall (__closure * Trequest_history_proc)(UnicodeString,LONGLONG ,LONGLONG ,DWORD);

class THistoryForm : public TForm
{
__published:	// IDE-managed Components
	TToolBar *ToolBar1;
	TStatusBar *StatusBar1;
	TPageControl *PageControl1;
	TTimer *Timer1;
	TImageList *ImageList1;
	TToolButton *tbTrends;
	TActionList *ActionList1;
	TAction *actAddSeries;
	TAction *actExportComtrade;
	TToolButton *tbRequestAlarmPeriod;
	TTabSheet *tanHistory;
	TListView *HistView;
	TTabSheet *tabGraphics;
	TChart *Chart1;
	TToolButton *tbAddChart;
	TToolButton *tbRemoveChart;
	TToolButton *ToolButton3;
	TAction *actRemoveSeries;
	TTabSheet *tabStatistic;
	TListView *StatisticView;
	TToolButton *ToolButton1;
	TAction *actExportHTML;
	TSaveDialog *SaveDialog1;
	TPanel *Panel1;
	void __fastcall TabSheet1Resize(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall Timer1Timer(TObject *Sender);
	void __fastcall tbRequestAlarmPeriodClick(TObject *Sender);
	void __fastcall actExportComtradeExecute(TObject *Sender);
	void __fastcall HistViewData(TObject *Sender, TListItem *Item);
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall HistViewSelectItem(TObject *Sender, TListItem *Item, bool Selected);
	void __fastcall actAddSeriesExecute(TObject *Sender);
	void __fastcall actRemoveSeriesExecute(TObject *Sender);
	void __fastcall PageControl1Change(TObject *Sender);
	void __fastcall HistViewCustomDrawItem(TCustomListView *Sender, TListItem *Item,
          TCustomDrawState State, bool &DefaultDraw);
	void __fastcall HistViewCustomDrawSubItem(TCustomListView *Sender, TListItem *Item,
          int SubItem, TCustomDrawState State, bool &DefaultDraw);
	void __fastcall actExportHTMLExecute(TObject *Sender);
	void __fastcall Chart1DragOver(TObject *Sender, TObject *Source, int X, int Y, TDragState State,
          bool &Accept);

private:	// User declarations
protected:
    UnicodeString          temp_file_name ;
    KeRTL::TFileStream    *temp_file_stream;
    KeRTL::TStorage       *temp_data_storage;
    Trequest_history_proc  Frequest_proc;
    mdb_record_ex          record;
    Tget_record_text       Fon_get_text  ;
    Tgetter_record         Fon_get_record;
    int                    timer_count;
    __int64                start_time;
    __int64                stop_time;
    UnicodeString          kadr_text;
    UnicodeString          period_text;
    DWORD                  Falarm;
    Tseries                series;
    OTDM_ARCHIVE_REC       rec_data;
    DWORD                  rec_index;
    TGKTlmViewer           * viewer_module;
    virtual void __fastcall WndProc(Winapi::Messages::TMessage &Message);
            void __fastcall handle_archive(LPOTDM_ARCHIVE_RECORDS_RESPOND resp, BOOL end_data);
            void __fastcall end_recv_data  ();
            bool __fastcall storage_create ();
            void __fastcall storage_delete ();
            bool __fastcall storage_created(){return temp_data_storage && temp_data_storage->is_good();}
            void __fastcall get_value_text(DWORD rec_id,double val   ,UnicodeString & text,bool is_null);
            void __fastcall get_state_text      ( DWORD state,UnicodeString & text,bool is_null);
            void __fastcall get_state_tu_text   ( DWORD state,UnicodeString & text,bool is_null);
            void __fastcall get_state_view_text ( DWORD state,UnicodeString & text,bool is_null);
            void __fastcall get_diag_text  ( DWORD diag,UnicodeString & text,bool is_null);
            void __fastcall make_staticstic();
            void __fastcall export_HTML_history(KeRTL::TFileStream & fs);
            void __fastcall export_HTML_stats  (KeRTL::TFileStream & fs);


        __fastcall THistoryForm(TComponent* Owner);
public:		// User declarations
	__fastcall THistoryForm(TComponent* Owner,TGKTlmViewer* vm,UnicodeString _kadr_text,UnicodeString _period_text);
        __property Tget_record_text      on_get_text   = {read = Fon_get_text  , write = Fon_get_text  };
        __property Tgetter_record        on_get_record = {read = Fon_get_record, write = Fon_get_record};
        __property Trequest_history_proc request_proc  = {read = Frequest_proc , write = Frequest_proc };
        __property DWORD                 alarm         = {read = Falarm        , write = Falarm        };

        int  __fastcall do_resample_channel      (Tcomtrade_channel * ch,int resample);
        DWORD __fastcall comtrade_prepare         (Tcomtrade& cmt,int resample = 0);
        void  __fastcall comtrade_write           (DWORD rec_no,Tcomtrade& cmt,string & file_name);
        void  __fastcall comtrade_write_resample  (DWORD rec_no,Tcomtrade& cmt,string & file_name,int rs);
static  void  __fastcall get_time_text  ( __int64 tmval,UnicodeString & text,bool is_null,char time_sep = ':',bool msec = true);
        TColor __fastcall get_color_for_list_view(DWORD diag,DWORD state);
        AnsiString __fastcall get_color_for_html (DWORD diag,DWORD state);


};
//---------------------------------------------------------------------------
extern PACKAGE THistoryForm *HistoryForm;
//---------------------------------------------------------------------------


#endif
