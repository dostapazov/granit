//---------------------------------------------------------------------------


#ifndef sc_base_frameH
#define sc_base_frameH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <gkmodules.hpp>
#include <granit_modem.h>
//---------------------------------------------------------------------------
class TSCBaseFrame;

typedef void __fastcall ( __closure * scframe_event)(TSCBaseFrame  * sender);

class TSCBaseFrame : public TFrame
{
__published:	// IDE-managed Components
protected:
        otd_addr      addr;
        DWORD         Fchanges_mask;
        scframe_event Fon_change;
        bool          Fcreate_mode;
        void __fastcall Setmod_handle(GKHANDLE value);
        GKHANDLE __fastcall Getmod_handle();
        void __fastcall Setchanges_mask(DWORD value);	// User declarations
TGKModuleInterface  mod_iface;
virtual  void  __fastcall after_set_gkhandle() {};

public:		// User declarations
        __fastcall TSCBaseFrame(TComponent* Owner);
        __property GKHANDLE mod_handle  = { read=Getmod_handle, write=Setmod_handle };
        __property DWORD changes_mask  = { read=Fchanges_mask, write=Setchanges_mask };
        __property scframe_event on_change  = { read=Fon_change, write=Fon_change };

         DWORD          __fastcall get_addr(){return addr.addr;}

         void  __fastcall begin_call_gkhandle(TCHAR * cmd_text);
         void  __fastcall done_call_gkhandle (DWORD ret,TCHAR * text = 0);
virtual  bool  __fastcall apply_changes() {return true;};
virtual  bool  __fastcall undo_changes () {return true;};
virtual  void  __fastcall set_active   (DWORD _addr);
virtual  void  __fastcall set_inactive(){Visible = false; }
virtual  void  __fastcall update_changes(LPGRSC_ACR acr)     {}
virtual  void  __fastcall update_data_changes(LPGRSC_ACR acr){}
virtual AnsiString __fastcall get_text();
        __property bool create_mode  = { read=Fcreate_mode, write=Fcreate_mode };
};
//---------------------------------------------------------------------------
extern PACKAGE TSCBaseFrame *SCBaseFrame;
//---------------------------------------------------------------------------


inline UnicodeString __fastcall load_rcstring(HINSTANCE h,DWORD rcid)
{
  wchar_t * text;
  int len = LoadStringW(h,rcid,(wchar_t * )&text,0);
  return UnicodeString(text,len);
}


#endif
