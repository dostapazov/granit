//---------------------------------------------------------------------------


#ifndef module_info_frameH
#define module_info_frameH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <gkmodules.h>
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>
//---------------------------------------------------------------------------
class TModuleInfoFrame : public TFrame
{
__published:	// IDE-managed Components
        TTimer *Timer1;
        TPanel *Panel1;
        TListView *ListView1;
        TLabel *Label2;
        TStaticText *StaticText2;
        TImage *ModIcon;
        TTrackBar *TrackBar1;
        TLabel *Label1;
        TLabel *ProcessPriorityClass;
        void __fastcall Timer1Timer(TObject *Sender);
        void __fastcall FrameResize(TObject *Sender);
        void __fastcall TrackBar1Change(TObject *Sender);
private:	// User declarations
__int64              work_time;
GKHANDLE             _handle;
bool                 Flicensed;
public:		// User declarations
        __fastcall TModuleInfoFrame(TComponent* Owner);
void    __fastcall setup_frame(GKHANDLE h);
void    __fastcall set_work_time(__int64  start,__int64 stop,bool running);
        __property bool licensed = {read = Flicensed};
};
//---------------------------------------------------------------------------
extern PACKAGE TModuleInfoFrame *ModuleInfoFrame;
//---------------------------------------------------------------------------
#endif
