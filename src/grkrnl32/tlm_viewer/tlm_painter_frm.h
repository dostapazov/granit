//---------------------------------------------------------------------------

#ifndef tlm_painter_frmH
#define tlm_painter_frmH
//---------------------------------------------------------------------------
#include "tlm_net_client.hpp"
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <tlmpainter.hpp>
#include "pictdlgs.h"
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <ToolWin.hpp>
#include <ImgList.hpp>
#include <Buttons.hpp>
#include "tlm_net_client.hpp"
#include <System.ImageList.hpp>



#define MSG_UPDATE_STATUS_BAR  (WM_APP+200)

//---------------------------------------------------------------------------
class TPainterForm : public TForm
{
__published:	// IDE-managed Components
        TStatusBar *StatusBar1;
        TCoolBar *CoolBar1;
        TPanel *Panel1;
        TImageList *ImageList1;
        TSpeedButton *SpeedButton1;
        TSpeedButton *SendTUOn;
        TSpeedButton *SendTUOff;
        TSpeedButton *SendTUCancel;
        void __fastcall Panel1Resize(TObject *Sender);
        void __fastcall FormDestroy(TObject *Sender);
        void __fastcall FormHide(TObject *Sender);
        void __fastcall FormShow(TObject *Sender);
        void __fastcall FormActivate(TObject *Sender);
        void __fastcall FormDeactivate(TObject *Sender);
        void __fastcall SpeedButton1Click(TObject *Sender);
        void __fastcall SendTUOnClick(TObject *Sender);
        void __fastcall SendTUOffClick(TObject *Sender);
        void __fastcall SendTUCancelClick(TObject *Sender);
private:	// User declarations
 mdb_kadr        kadr;
 TTlmPainter     Fpainter;
 DWORD           selected_num;
 TPaintBox    *  ready_painter;
 tlm_client   *  client;
 void __fastcall do_send_tu(DWORD op);
public:		// User declarations
        __fastcall TPainterForm(TComponent* Owner,tlm_client * cl);
void    __fastcall open_kadr(mdb_kadr & kadr);
TTlmPainter * __fastcall get_painter(){return &Fpainter;}
        __property TTlmPainter * painter = {read = get_painter};
 void   __fastcall  update_size(int cx,int cy);
 void   __fastcall  ShowEntryInfo(DWORD ent_num);
 void   __fastcall  set_ready_painter(TPaintBox * pb);
 TPaintBox *  __fastcall get_ready_painter(){return ready_painter ;}
 mdb_kadr  *  __fastcall get_kadr(){return & kadr;}
 void         __fastcall PaintReady(TObject * sender);
 void __fastcall flash_repaint();

 AnsiString __fastcall get_kadr_name();


protected:
        void __fastcall OnCmdSetBkgnd(TMessage & msg);
        void __fastcall OnCmdSetCaption(TMessage & msg);
        void __fastcall OnCmdUpdateKadr(TMessage & msg);
        void __fastcall OnCmdSetEntryes(TMessage & msg);
        void __fastcall OnCmdRepaintKadr(TMessage & msg);
        static DWORD WINAPI on_select_entry(LPVOID,TTlmPainter *,DWORD);
        static DWORD WINAPI on_mouse_over_entry(LPVOID,TTlmPainter *,DWORD);
        void   __fastcall CreateParams(TCreateParams & cp);
        void   __fastcall update_status_bar();
        void   __fastcall update_commands();
        void   __fastcall OnMsgUpdateSB(TMessage & msg);
        void   __fastcall WndProc(TMessage & msg);

public:
      void __fastcall set_tu_enabled(bool val);
      __property bool tu_enabled = {write = set_tu_enabled};



};
//---------------------------------------------------------------------------
extern PACKAGE TPainterForm *PainterForm;
//---------------------------------------------------------------------------
#endif
