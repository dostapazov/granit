//---------------------------------------------------------------------------

#ifndef awp_main_formH
#define awp_main_formH
//---------------------------------------------------------------------------
#include "gkawpmodule.hpp"
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include "tlm_painter_form.h"
#include <System.ImageList.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.ImgList.hpp>
#include <Vcl.Menus.hpp>
#include <Vcl.ToolWin.hpp>

#include "about_form.h"
#include "login_dlg.h"
#include <gksecur_proto.h>

class ready_painter_t;

#define WM_SHOW_REC_INFO      (WM_PAINTER_USER_FIRST+1)
#define WM_DO_LAYOUT_READY    (WM_PAINTER_USER_FIRST+2)
#define WM_DO_CLOSE_ALL_KADRS (WM_PAINTER_USER_FIRST+3)
#define WM_DO_SHOW_PARAMS     (WM_PAINTER_USER_FIRST+4)
#define WM_ON_SOURCE_CONNECT  (WM_PAINTER_USER_FIRST+5)


#define  HOTKEY_ID_CLEAR_ALL   1
#define  HOTKEY_ID_RECEIPT_ALL 2


//---------------------------------------------------------------------------
class TAwpMainForm : public TTlmPainterForm
{
__published:	// IDE-managed Components
	TToolButton *tbMainMenu;
	TToolButton *tbMinimize;
	TPopupMenu *MainPopup;
	TMenuItem *miParameters;
	TMenuItem *N2;
	TMenuItem *miAbout;
	TMenuItem *miLogin;
	TMenuItem *miOpenedKadrs;
	TMenuItem *miCloseAllKadrs;
	TBevel *Bevel1;
	TMenuItem *miDebug;
	TMenuItem *miDbhMakeallFlashing;
	TTrayIcon *wp_tray_icon;
	TImageList *IconsImage;
	TTimer *TrayTimer;
	void __fastcall tbMainMenuClick(TObject *Sender);
	void __fastcall miParametersClick(TObject *Sender);
	void __fastcall miAboutClick(TObject *Sender);
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
	void __fastcall miCloseAllKadrsClick(TObject *Sender);
	void __fastcall MainPopupPopup(TObject *Sender);
	void __fastcall miDbhMakeallFlashingClick(TObject *Sender);
	void __fastcall tbMinimizeClick(TObject *Sender);
	void __fastcall wp_tray_iconDblClick(TObject *Sender);
	void __fastcall TrayTimerTimer(TObject *Sender);
	void __fastcall miLoginClick(TObject *Sender);
public:
        enum class move_direction_t
       {
        md_left  = 1,
        md_right = 2,
        md_up    = 3,
        md_down  = 4
       };

private:	// User declarations
        TGkAwpModule    * module;
        TAboutForm      * about;
        int               Fready_space;
        bool              Fready_equal_size;
        TFont           * ready_font;
        TFont           * ready_state_font;
        READY_COLORS      ready_colors;
        ready_painter_t * ready_painter;
        TLoginDlg       * login_dlg;

virtual      __fastcall TAwpMainForm(TComponent* Owner);
             void __fastcall about_close(TObject *Sender,TCloseAction & ca);
             void __fastcall    release_ready_painter();
ready_painter_t * __fastcall get_ready_painter();
single_kadr     * __fastcall get_kadr         (DWORD kadr_id);
        void      __fastcall paint_entryes    (bool erase,bool flash);
        bool      __fastcall get_flash_erase  (){return false;}
        void      __fastcall update_ready_sizes   (LPMDB_GSIZE  max_size);
virtual bool      __fastcall get_record   (DWORD rec_id,MDB_RECORD & rec,bool * rec_visible) const;
  UnicodeString   __fastcall get_kadr_caption (){return Caption;}
virtual void      __fastcall select_changed  (DWORD offset,bool selected);
        void      __fastcall show_record_info  (const DWORD rec_id,bool _set = true);
virtual void      __fastcall WndProc(TMessage & msg);
        void      __fastcall mi_open_kadr_click(TObject *Sender);
        void      __fastcall move_current_ready(move_direction_t dir);
virtual void      __fastcall  do_move_selected(int dx,int dy);
virtual void      __fastcall  do_size_selected(int dx,int dy){return;}
virtual void      __fastcall  on_mouse_up     (TMouseButton button,TShiftState shift,int X, int Y);
        void      __fastcall  on_source_connect(bool connect);
        void      __fastcall  on_login_click   (TObject * sender);
public:		// User declarations
virtual __fastcall TAwpMainForm(TComponent* Owner,TGkAwpModule * mod);
void    __fastcall set_window_monitor(int mon_idx,bool no_layout);
        __property int   ready_space      = {read = Fready_space};
        __property bool  ready_equal_size = {read = Fready_equal_size};
        void __fastcall  set_ready_font        (const LOGFONTW & lf,bool  state,bool no_layout);
        void __fastcall  set_kadr_layout_params(int ready_space,bool ready_equal_size,bool no_layout);
        void __fastcall  set_ready_colors      (const READY_COLORS & rclrs);
        void __fastcall  layout_ready          ();
        void __fastcall  set_form_screen_position(TForm * frm);
        bool __fastcall  hot_key_register    (int id,WORD hot_key);
        void __fastcall  set_login_session_id(SESSION_ID ssid    );

};
//---------------------------------------------------------------------------
extern PACKAGE TAwpMainForm *AwpMainForm;
//---------------------------------------------------------------------------
#endif
