//---------------------------------------------------------------------------

#ifndef kadrs_editorH
#define kadrs_editorH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "gkmodule_form.h"
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <ToolWin.hpp>
#include <kadrs_table.hpp>
#include <ImgList.hpp>
#include <Buttons.hpp>
#include <System.ImageList.hpp>
#include <Vcl.Dialogs.hpp>
#include <Vcl.ExtDlgs.hpp>
#ifdef __CODEGEARC__
#include <System.ImageList.hpp>
#endif
#include <tlm_painter.hpp>
#include <map>

typedef std::map<DWORD,TTreeNode *> kadr_map_t;


//---------------------------------------------------------------------------
class Tkadr_data;
class TPaintVariantConstruct;

class TKadrsEditor : public TGKModuleForm
{
__published:	// IDE-managed Components
        TToolBar *ToolBar1;
        TStatusBar *StatusBar1;
        TTreeView *KadrsTree;
        TSplitter *Splitter1;
        TImageList *ImageList1;
        TToolButton *CreateBtn;
        TToolButton *DeleteBtn;
        TToolButton *ApplyBtn;
        TToolButton *UndoBtn;
        TToolButton *ToolButton5;
        TPanel *Panel1;
        TLabel *Label1;
        TStaticText *KadrID;
        TLabel *Label2;
        TEdit *KadrName;
        TLabel *Label3;
        TEdit *KadrSName;
        TListView *EntryList;
        TToolButton *ToolButton1;
        TToolButton *OpenKadrBtn;
        TSpeedButton *EntryMoveUp;
        TSpeedButton *EntryMoveDown;
        TSpeedButton *EntryDelete;
	TToolButton *PaintVarEdit;
	TToolButton *ToolButton2;
        void __fastcall EntryListCompare(TObject *Sender, TListItem *Item1,
          TListItem *Item2, int Data, int &Compare);
        void __fastcall EntryListResize(TObject *Sender);
        void __fastcall EntryListDragOver(TObject *Sender, TObject *Source,
          int X, int Y, TDragState State, bool &Accept);
	void __fastcall KadrsTreeChange(TObject *Sender, TTreeNode *Node);
	void __fastcall KadrsTreeDeletion(TObject *Sender, TTreeNode *Node);
	void __fastcall EntryListSelectItem(TObject *Sender, TListItem *Item, bool Selected);
	void __fastcall OpenKadrBtnClick(TObject *Sender);
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall ApplyBtnClick(TObject *Sender);
	void __fastcall UndoBtnClick(TObject *Sender);
	void __fastcall EntryMoveUpClick(TObject *Sender);
	void __fastcall KadrNameChange(TObject *Sender);
	void __fastcall EntryDeleteClick(TObject *Sender);
	void __fastcall CreateBtnClick(TObject *Sender);
	void __fastcall DeleteBtnClick(TObject *Sender);
	void __fastcall PaintVarEditClick(TObject *Sender);
	void __fastcall EntryListDragDrop(TObject *Sender, TObject *Source, int X, int Y);
	void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
	void __fastcall FormKeyPress(TObject *Sender, System::WideChar &Key);




private:	// User declarations

protected:
bool                    create_mode ;
int                     Fin_setup;
TTreeNode             * kadrs_root;
tlmpaint::painter_dll * pdll;
int                     paint_context;
kadr_map_t              kadr_map;
TPaintVariantConstruct * pvc;


DWORD              __fastcall set_text(DWORD text_id,UnicodeString str);
void               __fastcall after_set_gkhandle();
void               __fastcall on_module_config_change(GKHANDLE mod,LPMODULE_CONFIG_DATA mcd);
void               __fastcall on_module_state(GKHANDLE mod,DWORD state);
TTreeNode *        __fastcall add_kadr_node(DWORD kid);
TTreeNode *        __fastcall add_kadr_node(mdb_kadr & kadr);
TTreeNode *        __fastcall find_kadr    (DWORD kadr_id);
void               __fastcall setup_kadrs_tree();

LRESULT            __fastcall on_gkhandle_notify (GKHANDLE from,LPNOTIFY_CODE nc,DWORD mask);
void               __fastcall on_record_notify(DWORD ncode,LPMDB_RECORD_CHANGED);
void               __fastcall on_kadr_notify(DWORD ncode,LPMDB_KADR_CHANGED);
void               __fastcall on_kadr_content(TTreeNode * node);
void               __fastcall on_kadr_change        (TTreeNode * node);
void               __fastcall on_kadr_change_content(TTreeNode * node);

void               __fastcall show_kadr_state (Tkadr_data * kd);
void               __fastcall get_recid_number(TListItem * item,DWORD &record_id,DWORD & num);

bool               __fastcall get_kadr_entryes(Tkadr_data * kd);
void               __fastcall setup_entry_list();

              void __fastcall store_paint_variants(int pcontext);
              void __fastcall set_paint_variant(UnicodeString name,LPMDB_PAINT_VARIANT pv,bool update);
              void __fastcall load_painter_dll ();
              void __fastcall setup_painter_dll();

   const wchar_t * __fastcall get_painter_library_name();

              bool __fastcall _apply_entryes       (Tkadr_data * kd);
              bool __fastcall _apply_kadr_changes  (Tkadr_data * kd);
              bool __fastcall _undo_kadr_changes   (Tkadr_data * kd);
              void __fastcall _set_kadr_changes    (Tkadr_data * kd,DWORD ch);
              void __fastcall _kadr_select_changed (Tkadr_data * kd,DWORD offset);
              void __fastcall _kadr_entryes_changed(Tkadr_data * kd);
              void __fastcall WndProc(TMessage & msg);
              void __fastcall move_entry_number(TListItem * item,bool down);
  mdb_kadr_entry * __fastcall find_entry(TListItem * item);

              void __fastcall pvc_close(TObject * sende,TCloseAction & ca);

              //Завершение операции Drag-Drop
              void __fastcall _add_entry         (Tkadr_data * kd ,mdb_record & rec,int X,int Y );
              void __fastcall drop_finish        (Tkadr_data * kd,int X,int Y);
              void __fastcall drop_from_tree_view(Tkadr_data * kd,TTreeView * tv,int X,int Y);
              void __fastcall drop_from_list_view(Tkadr_data * kd,TListView * lv,int X,int Y);
      Tkadr_data * __fastcall get_kadr_data_from_drop_sender (TObject * sender);
      void         __fastcall repaint_all_kadrs  ();
      void         __fastcall save_all_kadrs     ();


public:		// User declarations
        __fastcall TKadrsEditor(TComponent* Owner,GKHANDLE module_handle);
 void   __fastcall reload();
 void   __fastcall update_paint_variants(LPNOTIFY_CODE nc);
 __property  int         in_setup      = {read = Fin_setup, write = Fin_setup};
 __property  Tkadr_data* current_kadr  = {read = get_current_kadr};

 UnicodeString      __fastcall get_text(DWORD text_id) const;
 UnicodeString      __fastcall get_name(DWORD text_id,bool ) const;
 const wchar_t*     __fastcall get_name(MDB_RECORD & rec,bool short_name ) const;
 bool               __fastcall get_record(DWORD rec_id,MDB_RECORD& sr,bool * is_visible = NULL) const;


  Tkadr_data* __fastcall get_current_kadr();
  Tkadr_data* __fastcall get_kadr        (TTreeNode * Node);
         void __fastcall _add_entry      (Tkadr_data * kd ,mdb_kadr_entry & entry );

};
//---------------------------------------------------------------------------
extern PACKAGE TKadrsEditor *KadrsEditor;
//---------------------------------------------------------------------------

#endif
