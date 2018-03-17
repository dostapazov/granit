//---------------------------------------------------------------------------


#ifndef gkmod_frameH
#define gkmod_frameH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <gkmodules.hpp>
//---------------------------------------------------------------------------
class TGKModuleFrame : public TFrame
{
__published:	// IDE-managed Components
private:
        void __fastcall Setmod_handle(GKHANDLE value);
        GKHANDLE __fastcall Getmod_handle();	// User declarations
protected:
        int        Fin_setup;
  DWORD            Fch_mask;
TNotifyEvent       Fon_changed;
TGKModuleInterface mod_iface;
TObject          * Fcurrent_object;
virtual  void     __fastcall  after_set_gkhandle() {};
virtual  bool     __fastcall  on_enum_handle(GKHANDLE parent,GKHANDLE h,LPVOID data){return true;};
         void     __fastcall  enum_modules(GKHANDLE h,LPVOID data);
virtual  void     __fastcall  current_object_change (TObject * value){ if(Fcurrent_object!=value) Fcurrent_object = value; }
  TTreeNode *     __fastcall  get_current_tree_node(){return dynamic_cast<TTreeNode*>(current_object);}
  TListItem *     __fastcall  get_current_list_item(){return dynamic_cast<TListItem*>(current_object);}
virtual  void     __fastcall  set_ch_mask(DWORD value);

public:		// User declarations
        __fastcall TGKModuleFrame(TComponent* Owner);
virtual  bool     __fastcall  apply_changes (){return true;}
virtual  bool     __fastcall  undo_changes  (){return true;}
virtual  bool     __fastcall  delete_current(){return true;}
virtual  void     __fastcall  current_object_changed(TObject * ,bool &enable_set){enable_set = true; }
         __property GKHANDLE mod_handle             = { read=Getmod_handle, write=Setmod_handle };
         __property DWORD ch_mask                   = {read = Fch_mask, write = set_ch_mask};
         __property TNotifyEvent on_changed         = {read = Fon_changed,write = Fon_changed};
         __property int in_setup                    = {read = Fin_setup, write = Fin_setup};
         __property TObject   * current_object      = { read = Fcurrent_object,write = current_object_change};
         __property TTreeNode * current_tree_node   = { read = get_current_tree_node};
         __property TListItem * current_list_item   = { read = get_current_list_item};
  static int      __fastcall  atoi(wchar_t *s){return _wtoi(s);}
  static double   __fastcall  atof(wchar_t *s){return _wtof(s);}

};
//---------------------------------------------------------------------------
extern PACKAGE TGKModuleFrame *GKModuleFrame;
//---------------------------------------------------------------------------

#endif
