//---------------------------------------------------------------------------

#ifndef tlm_kadr_editorH
#define tlm_kadr_editorH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include "tlm_painter_form.h"
#include <System.ImageList.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.ImgList.hpp>
#include <Vcl.ToolWin.hpp>
#include <Vcl.Menus.hpp>
#include <Vcl.Dialogs.hpp>
#include <Vcl.ExtDlgs.hpp>
//---------------------------------------------------------------------------
#include "entry_props.h"
#include "record_props.h"


#define WM_APPLY_CHANGES          (WM_USER+1001)
#define WM_UNDO_CHANGES           (WM_USER+1002)
#define WM_KADR_CHANGED           (WM_USER+1003)
#define WM_ENTRY_SELECT_CHANGE    (WM_USER+1004)
#define WM_NOTIFY_ENTRY_CHANGED   (WM_USER+1005)
#define WM_NOTIFY_ENTRYES_CHANGED (WM_USER+1006)


class Tkadr_data;

class TTlmKadrEditor : public TTlmPainterForm
{
__published:	// IDE-managed Components
	TToolButton *tbApply;
	TToolButton *tbUndo;
	TToolButton *tbDelete;
	TToolButton *tbFullScreen;
	TToolButton *tbBkGnd;
	TToolButton *tbArrageLeft;
	TToolButton *tbArangeTop;
	TToolButton *tbProps;
	TToolButton *ToolButton1;
	TToolButton *ToolButton2;
	TToolButton *ToolButton3;
	TPopupMenu *bkPopup;
	TMenuItem *N1;
	TMenuItem *miNoGrid;
	TMenuItem *miGrid4x4;
	TMenuItem *miGrid8x8;
	TMenuItem *miGrid16x16;
	TMenuItem *miBkMode;
	TMenuItem *miBkgndNoChange;
	TMenuItem *miBkgndCenter;
	TMenuItem *miBkgndKadrSize;
	TMenuItem *miBkgndProportionalKadrSize;
	TMenuItem *miSelectBkgnd;
	TOpenPictureDialog *PictDlg;
	TToolButton *ToolButton4;
	TToolButton *tbDiag;
	TToolButton *tbSameSize;
	TMenuItem *miArrageLeft;
	TMenuItem *miArrangeTop;
	TMenuItem *miLayoutHorz;
	TMenuItem *miLayoutVert;
	TMenuItem *miDelete;
	TMenuItem *N13;
	TMenuItem *miEditRecord;
	TMenuItem *N3;
	TMenuItem *miSetOptimalSize;
	TMenuItem *N2;
	TMenuItem *N4;
	TMenuItem *N6;
	TMenuItem *miLayoutInvisible;
	TMenuItem *N9;
	TMenuItem *miSetSameSize;
	TMenuItem *miGrid2x2;
	TMenuItem *miSetKadrExtentAsBkgnd;
	TMenuItem *miMakeEntryCopy;
	void __fastcall tbFullScreenClick(TObject *Sender);
	void __fastcall miBkgndNoChangeClick(TObject *Sender);
	void __fastcall miSelectBkgndClick(TObject *Sender);
	void __fastcall FormResize(TObject *Sender);
	void __fastcall miGrid16x16Click(TObject *Sender);
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall tbApplyClick(TObject *Sender);
	void __fastcall tbUndoClick(TObject *Sender);
	void __fastcall tbDiagClick(TObject *Sender);
	void __fastcall paint_areaDblClick(TObject *Sender);
	void __fastcall tbPropsClick(TObject *Sender);
	void __fastcall FormActivate(TObject *Sender);
	void __fastcall FormDeactivate(TObject *Sender);
	void __fastcall tbArrageLeftClick(TObject *Sender);
	void __fastcall tbArangeTopClick(TObject *Sender);
	void __fastcall tbSameSizeClick(TObject *Sender);
	void __fastcall kadr_popupPopup(TObject *Sender);
	void __fastcall tbDeleteClick(TObject *Sender);
	void __fastcall miLayoutVertClick(TObject *Sender);
	void __fastcall miEditRecordClick(TObject *Sender);
	void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
	void __fastcall miSetOptimalSizeClick(TObject *Sender);
	void __fastcall miLayoutInvisibleClick(TObject *Sender);
	void __fastcall miSetKadrExtentAsBkgndClick(TObject *Sender);
	void __fastcall miMakeEntryCopyClick(TObject *Sender);
private:	// User declarations
        int Fin_setup;
        TEntryProps  * entry_props;
        TRecordProps * record_props;


        virtual void __fastcall  show_entry_info (mdb_kadr_entry *entry);
        virtual void __fastcall  set_current_over(DWORD val);
        virtual void __fastcall  do_move_selected(int dx,int dy);
        virtual void __fastcall  do_size_selected(int dx,int dy);
                void __fastcall  on_entry_props_close (TObject *Sender, TCloseAction &Action);
                void __fastcall  on_record_props_close(TObject *Sender, TCloseAction &Action);

        virtual void __fastcall  select_changed  (DWORD offset,bool selected);
                void __fastcall  show_props      (DWORD offset,bool selected);

                void __fastcall  entry_props_cling(int dx,int dy);
                void __fastcall  entry_props_chanhed( TEntryProps * e_props,LPMDB_KADR_ENTRY entry );

                void __fastcall  do_arrange      (bool top);
                void __fastcall  do_same_size_set();
virtual UnicodeString __fastcall get_kadr_caption();
virtual UnicodeString __fastcall get_image_file  ();
                 void __fastcall update_commands ();
        virtual void __fastcall WndProc(TMessage & msg);
virtual int  __fastcall  get_captured_delta(){return 4;}
public:		// User declarations

virtual	__fastcall TTlmKadrEditor(TComponent* Owner);
        bool __fastcall get_record(DWORD rec_id, MDB_RECORD & rec,bool * rec_visible) const;
        Tkadr_data * __fastcall get_kadr_data   ();
        void         __fastcall set_kadr_data   (Tkadr_data * val);
        DWORD        __fastcall get_kadr_changes();
        void         __fastcall set_kadr_changes(DWORD val);
virtual void         __fastcall set_kadr        (single_kadr * kadr);

        __property Tkadr_data*  kadr_data      = {read = get_kadr_data   , write = set_kadr_data    };
        __property DWORD        kadr_changes   = {read = get_kadr_changes, write = set_kadr_changes };
        void         __fastcall update_paint_variants();
};
//---------------------------------------------------------------------------
extern PACKAGE TTlmKadrEditor *TlmKadrEditor;
//---------------------------------------------------------------------------

inline void __fastcall  TTlmKadrEditor::do_move_selected(int dx,int dy)
{
    TTlmPainterForm::do_move_selected(dx,dy);
    kadr_changes |= MDB_KADR_FIELD_ENTRYES;
}

inline void __fastcall  TTlmKadrEditor::do_size_selected(int dx,int dy)
{
    TTlmPainterForm::do_size_selected(dx,dy);
    kadr_changes |= MDB_KADR_FIELD_ENTRYES;
}


class Tkadr_data:public single_kadr
{
 public:
          Tkadr_data ():single_kadr(){}
 virtual ~Tkadr_data(){if(user_data) delete get_kadr_editor();}
 UnicodeString  kadr_name;
 UnicodeString  kadr_sname;
 UnicodeString  image_name;
 TTreeNode     *owner_node;
 TTlmKadrEditor* __fastcall get_kadr_editor(){return reinterpret_cast<TTlmKadrEditor*>(user_data); }
 bool __fastcall is_new(){return kadr.kadr_id == (DWORD)-1 ? true : false; }
};
//---------------------------------------------------------------------------

inline Tkadr_data * __fastcall TTlmKadrEditor::get_kadr_data()
{
 return dynamic_cast<Tkadr_data *>(this->Fkadr);
}

inline void         __fastcall TTlmKadrEditor::set_kadr_data(Tkadr_data * val)
{
  if(kadr_data!=val)
     set_kadr(val);
}



#endif
