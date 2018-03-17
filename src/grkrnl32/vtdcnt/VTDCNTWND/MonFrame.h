//---------------------------------------------------------------------------


#ifndef MonFrameH
#define MonFrameH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <text_viewer.hpp>
#include <ExtCtrls.hpp>
#include "..\vtdcnt.h"
#include <Buttons.hpp>

//---------------------------------------------------------------------------

typedef KeWin::TTextViewer<char> TModMon;
class TMonitorFrame : public TFrame
{
__published:	// IDE-managed Components
        TGroupBox *MonitorBox;
        TPanel *Panel1;
        TSpeedButton *SpeedButton1;
        TCheckBox *ShowDetail;
        TCheckBox *ShowTXRX;
        void __fastcall FrameResize(TObject *Sender);
        void __fastcall SpeedButton1Click(TObject *Sender);
private:	// User declarations
TModMon         mon;
void __fastcall BeforeDestruction(void);
void __fastcall create_mon_instance();
void __fastcall monitor_txrx(LPBYTE data,DWORD sz,bool tx);

void __fastcall monitor_kadr_std( LPVOID  ,bool tx);
void __fastcall mon_date_time_std(LPVOID  );
void __fastcall mon_base_config_std(LPVOID);
void __fastcall mon_param_std(lpvtd_resp_kadr rk);
void __fastcall mon_curr_param(lpvtd_resp_kadr rk);


public:		// User declarations
        __fastcall TMonitorFrame(TComponent* Owner);
void    __fastcall monitor_vtd_events(DWORD code,LPVOID data, DWORD sz );

};
//---------------------------------------------------------------------------
extern PACKAGE TMonitorFrame *MonitorFrame;
//---------------------------------------------------------------------------
#endif
