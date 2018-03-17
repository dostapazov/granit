//---------------------------------------------------------------------------

#ifndef tlm_viewer_main_formH
#define tlm_viewer_main_formH

#include "gktlm_viewer_form.h"
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.Menus.hpp>
#include <Vcl.StdCtrls.hpp>

//---------------------------------------------------------------------------

#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include "tlm_viewer_param.h"
#include "tlm_kadr_viewer.h"
#include <KeRTLio.hpp>
#include "about_form.h"
#include <Vcl.AppEvnts.hpp>
#include <System.ImageList.hpp>
#include <Vcl.ImgList.hpp>
#include <Vcl.Buttons.hpp>
#include "login_dlg.h"
#include <gksecur_proto.h>
#include "tlm_viewer_param.h"




#define  HOTKEY_ID_CLEAR_ALL   1
#define  HOTKEY_ID_RECEIPT_ALL 2

#define TLMMF_WRITE_CONFIG (WM_USER+2000)
#define TLMMF_REMOVE_ORDER (WM_USER+2001)
#define TLMMF_DB_CHANGE    (WM_USER+2002)

//---------------------------------------------------------------------------

//typedef std::map<DWORD,TTlmKadrViewer*> Tkadrs_opened;



class TTlmViewerMainForm : public TTlmViewerForm
{
__published:	// IDE-managed Components
	TTimer *Timer1;
	TPopupMenu *PopupMenu1;
	TMenuItem *miMoveKadrLeft;
	TMenuItem *miMoveKadrRight;
	TMenuItem *N3;
	TMenuItem *miUnorderKadr;
	TMenuItem *miResetHistory;
	TTrayIcon *TrayIcon1;
	TPopupMenu *TrayPopup;
	TMenuItem *N4;
	TImageList *TrayImage;
	TPanel *Panel1;
	TSpeedButton *bMainMenu;
	TSpeedButton *SpeedButton2;
	TPopupMenu *MainPopup;
	TMenuItem *N1;
	TMenuItem *N2;
	TMenuItem *miAbout;
	TMenuItem *miOpenedKadrs;
	TMenuItem *miCloseAllKadrs;
	TMenuItem *miMoveKadrUp;
	TMenuItem *miMoveKadrDown;
	TMenuItem *miLogin;
	void __fastcall FormCloseQuery   (TObject *Sender, bool &CanClose);
	void __fastcall miAboutClick     (TObject *Sender);
	void __fastcall miParametersClick(TObject *Sender);
	void __fastcall Timer1Timer(TObject *Sender);
	void __fastcall miResetHistoryClick(TObject *Sender);
	void __fastcall miUnorderKadrClick(TObject *Sender);
	void __fastcall miMoveKadrLeftClick(TObject *Sender);
	void __fastcall miMoveKadrRightClick(TObject *Sender);
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall N4Click(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall SpeedButton2Click(TObject *Sender);
	void __fastcall bMainMenuClick(TObject *Sender);
	void __fastcall MainPopupPopup(TObject *Sender);
	void __fastcall miCloseAllKadrsClick(TObject *Sender);
	void __fastcall miMoveKadrUpClick(TObject *Sender);
	void __fastcall miMoveKadrDownClick(TObject *Sender);
	void __fastcall miLoginClick(TObject *Sender);


protected:
        //TTlmViewerParams * params;
        TLoginDlg        * login;
        SESSION_ID         local_ssid;
        SESSION_ID         remote_ssid;
        TAboutForm       * about;
        KeWin::TSize       max_kadr_size;


        void __fastcall params_close(TObject *Sender, TCloseAction &Action);
        void __fastcall about_close (TObject *Sender, TCloseAction &Action);
virtual DWORD __fastcall handle_get_kadr (Tkadrs & klist,Tnames * _names,LPOTDM_PROTO_HEADER omph,bool end);
virtual void  __fastcall handle_mproto         (LPMPROTO_HEADER mph);
virtual void  __fastcall handle_medproto_error (LPOTDM_PROTO_HEADER omph);

        void  __fastcall handle_paint_variants(LPVOID data,DWORD data_size,bool beg_resp,bool end_resp);
        void  __fastcall handle_paint_image   (LPVOID data,DWORD data_size,bool beg_resp,bool end_resp);
        bool  __fastcall kadr_order_add   (DWORD kadr_id);
        bool  __fastcall kadr_order_remove(DWORD kadr_id);
        void  __fastcall kadrs_order      (bool order);
        void  __fastcall get_kadr_extent  (Tmdb_kadr_ex * kadr,KeWin::TSize & sz,bool update_max_size = false);
        void  __fastcall update_max_kadr_size();
        int   __fastcall calc_kadr_columns(int READY_SPACER);
        int   __fastcall calc_ready_height(int READY_SPACER);
        void  __fastcall update_ready_layout();


virtual void __fastcall  WndProc          (Winapi::Messages::TMessage & msg);
virtual bool __fastcall  get_record       (TObject * sender, DWORD id, bool offset, mdb_record_ex & rec);
virtual bool __fastcall  get_text         (DWORD id,UnicodeString & name);
virtual void __fastcall  set_kadr_entryes (DWORD kadr_id);
virtual DWORD __fastcall on_select_entry (TTlmPainter * p,DWORD entry  );
virtual BOOL  __fastcall on_mouse_buttons(TTlmPainter * p,Tmouse_data * md);
virtual void  __fastcall update_kadr     (DWORD kadr_id,DWORD mask);
virtual void  __fastcall repaint_changed_records();

        void  __fastcall do_setup_painter_dll();
        void  __fastcall do_setup_painter_dll(mdb_paint_variant & paint_var,bool repaint_kadrs);
        void  __fastcall repaint_all_kadrs() ;


        void  __fastcall open_kadr       (Tmdb_kadr_ex * kadr,bool show = true,bool focused = true);
        void  __fastcall handle_modem_line_state(DWORD ma,DWORD state);


        bool  __fastcall     scan_ready  (Tmdb_kadr_ex * kadr);
        void  __fastcall     rescan_ready(DWORD kadr_id);
        void  __fastcall     reset_double_buffered();
        void  __fastcall     set_ready_color(TColor color);
        void  __fastcall     swap_kadr_position(Tmdb_kadr_ex * k1,Tmdb_kadr_ex * k2);

enum how_kadr_find_t {left,right,up,down};
Tmdb_kadr_ex* __fastcall find_next_kadr( Tmdb_kadr_ex * k,how_kadr_find_t how);

        void  __fastcall show_kadrs(bool show);
        void  __fastcall mi_opened_click(TObject *Sender);
        void __fastcall  update_kadrs_bkgnd();
        void __fastcall  close_all_kadrs   (bool force);
        bool __fastcall  hot_key_register  (int   id,WORD hot_key);
 UnicodeString __fastcall get_layout_file  (UnicodeString layout_name);
        void __fastcall  save_layout       (TObject * sender);
        void __fastcall  open_layout       (TObject * sender);
        void __fastcall  enum_layouts      (TObject * sender);
        void __fastcall  receipt_all       ();
        void __fastcall  clear_all         ();
        void __fastcall  handle_database_change ();
        void __fastcall  control_sound_thread(bool start);
        void __fastcall  on_login_click(TObject * sender);
        void __fastcall  handle_security (LPMPROTO_HEADER mph);
        void __fastcall  handle_logout();
virtual bool __fastcall create_painter    ();
   __fastcall TTlmViewerMainForm(TComponent* Owner);
public:		// User declarations
friend TGKTlmViewer;
virtual   __fastcall TTlmViewerMainForm(TComponent* Owner,TGKTlmViewer * viewer_module);

//        bool __fastcall  write_config     (DWORD mask, HKEY reg_key, wchar_t * config_folder);
//        bool __fastcall  read_config      (DWORD mask, HKEY reg_key, wchar_t * config_folder);
        void  __fastcall start();
        void  __fastcall stop();
        void  __fastcall reconnect();
        void  __fastcall do_save_layout(UnicodeString layout_name);
        void  __fastcall do_open_layout(UnicodeString layout_name);
        void  __fastcall renumber_ordered_kadrs();
virtual void  __fastcall handle_recv(LPMPROTO_HEADER mph);
        void  __fastcall move_to_monitor(int mon_num);
        void  __fastcall set_ready_font (LPLOGFONT  lf);



  private:
  // Нить воспроизведения звуков

  class  TSoundThread : public Classes::TThread
  {
  TTlmViewerMainForm * form;
  void __fastcall Execute();
  void __fastcall do_play_sound(UnicodeString snd);

  TSoundThread(){};
  public:
  TSoundThread(TTlmViewerMainForm * _form):Classes::TThread(),form (_form) {}
  };
  TSoundThread * sound_thread;
};
//---------------------------------------------------------------------------
extern PACKAGE TTlmViewerMainForm *TlmViewerMainForm;
//---------------------------------------------------------------------------
#endif
