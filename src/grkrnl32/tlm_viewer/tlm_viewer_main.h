//---------------------------------------------------------------------------

#ifndef tlm_viewer_mainH
#define tlm_viewer_mainH
//---------------------------------------------------------------------------
#include "tlm_net_client.hpp"
#include "tlm_painter_frm.h"
#include "TIpClientForm.h"
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Menus.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>

#define WM_APPBAR_CALLBACK (WM_USER+100)

//---------------------------------------------------------------------------
class TTlmViewer : public TForm
{
__published:	// IDE-managed Components
        TMainMenu *MainMenu1;
        TMenuItem *N1;
        TMenuItem *N2;
        TMenuItem *N3;
        TMenuItem *ConnProps;
        TMenuItem *DoConnect;
        TMenuItem *DoDisconnect;
        TMenuItem *N7;
        TPopupMenu *KadrsPopupMenu;
        TTimer *FlashTimer;
        TPopupMenu *PopupMenu1;
        TMenuItem *ReadyMoveLeft;
        TMenuItem *ReadyMoveRight;
        TMenuItem *N6;
        TMenuItem *SaveLauout;
        TMenuItem *N4;
        TMenuItem *N5;
        void __fastcall FormShow(TObject *Sender);
        void __fastcall N2Click(TObject *Sender);
        void __fastcall DoConnectClick(TObject *Sender);
        void __fastcall DoDisconnectClick(TObject *Sender);
        void __fastcall N7Click(TObject *Sender);
        void __fastcall ConnPropsClick(TObject *Sender);
        void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
        void __fastcall FormDestroy(TObject *Sender);
        void __fastcall FormActivate(TObject *Sender);
        void __fastcall FlashTimerTimer(TObject *Sender);
        void __fastcall N5Click(TObject *Sender);

private:	// User declarations
      APPBARDATA   appbar;
      tlm_client   client;
      HANDLE       term_event;
      void   __fastcall OpenKadrMenuItemClick(TObject * sender);
      void   __fastcall ReopenKadr(TObject * sender);
      TPaintBox    * __fastcall find_kadr_frame(DWORD kadr_id);
      int            __fastcall find_active_ready();
      TPainterForm * __fastcall get_painter_frame(TPaintBox * pb);
      void           __fastcall OnReadyMouseUp(TObject * sender,TMouseButton mbtns,Classes::TShiftState shift,int X, int Y);
      void           __fastcall SendTUOn (TObject * sender);
      void           __fastcall SendTUOff(TObject * sender);

      static wstring __stdcall get_text  (LPVOID,TTlmPainter * painter,DWORD);
      static DWORD   __stdcall get_record(LPVOID,TTlmPainter * painter ,mdb_record_ex * rec);
public:		// User declarations
        __fastcall TTlmViewer(TComponent* Owner);
protected:
        void __fastcall WndProc(Messages::TMessage &Message);
        void __fastcall CreateParams(TCreateParams &Params);
        void __fastcall Reconnect(TObject * sender);
        void __fastcall clear_ready_bar();
        void __fastcall OnAppException(TObject * sender,Exception * exp);
        void __fastcall SaveLayout  (TObject * sender);
        void __fastcall ChangeLayout(TObject * sender);
        int  __fastcall get_ready_width();

};
//---------------------------------------------------------------------------
extern PACKAGE TTlmViewer *TlmViewer;
//---------------------------------------------------------------------------
#endif
