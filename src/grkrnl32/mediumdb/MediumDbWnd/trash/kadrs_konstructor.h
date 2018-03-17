//---------------------------------------------------------------------------

#ifndef kadrs_konstructorH
#define kadrs_konstructorH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "gkmodule_form.h"
#include <kadrs_table.hpp>
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>
#include <ToolWin.hpp>
#include "mediumdb_frm.h"
#include <Graphics.hpp>
#include <Dialogs.hpp>
#include <ExtDlgs.hpp>
#include <ImgList.hpp>
#include <Buttons.hpp>
#include "kadr_form.h"
#include <System.ImageList.hpp>
#ifdef __CODEGEARC__
#include <System.ImageList.hpp>
#endif
#include <map>
typedef map<DWORD,TKadrForm*> kadrs_map;


//---------------------------------------------------------------------------
class TMdbKadrConstuctor : public TGKModuleForm
{
__published:	// IDE-managed Components
        TToolBar *ToolBar1;
        TOpenPictureDialog *OpenPictureDialog1;
        TStatusBar *StatusBar1;
        TTreeView *KadrsTree;
        TSplitter *Splitter1;
        TPageControl *PageControl1;
        TTabSheet *KadrParam;
        TPanel *Panel1;
        TLabel *Label1;
        TEdit *KadrName;
        TLabel *Label2;
        TListView *KadrEntryList;
        TEdit *KadrImage;
        TGroupBox *DrawParam;
        TSpeedButton *SpeedButton1;
        TImageList *ImageList2;
        TToolButton *CreateKadr;
        TToolButton *ToolButton4;
        TToolButton *ApplyBtn;
        TToolButton *UndoBtn;
        TToolButton *DeleteKadr;
        TLabel *Label5;
        TEdit *EntryNumber;
        TColorDialog *ColorDialog1;
        TStaticText *EntryBkgndColor;
        TStaticText *EntryColor;
        TSpeedButton *DeleteEntry;
        TSpeedButton *DownEntry;
        TLabel *Label6;
        TEdit *SKadrName;
        TCheckBox *ImageCentr;
        TCheckBox *ImageStretch;
        TGroupBox *DiscreteGroup;
        TComboBox *DiscreteDrawAs;
        TRadioGroup *NormalValue;
        TGroupBox *AnalogGroup;
        TLabel *Label4;
        TEdit *EntryPrecision;
        TCheckBox *CheckBox1;
        TSpeedButton *UpEntry;
        TComboBox *AnalogDrawAs;
        void __fastcall FormDestroy(TObject *Sender);
        void __fastcall ClearImageClick(TObject *Sender);
        void __fastcall CreateKadrClick(TObject *Sender);
        void __fastcall KadrsTreeChanging(TObject *Sender, TTreeNode *Node,
          bool &AllowChange);
        void __fastcall ApplyBtnClick(TObject *Sender);
        void __fastcall UndoBtnClick(TObject *Sender);
        void __fastcall KadrNameChange(TObject *Sender);
        void __fastcall KadrImageChange(TObject *Sender);
        void __fastcall KadrsTreeChange(TObject *Sender, TTreeNode *Node);
        void __fastcall DeleteKadrClick(TObject *Sender);
        void __fastcall KadrEntryListSelectItem(TObject *Sender,
          TListItem *Item, bool Selected);
        void __fastcall EntryBkgndColorClick(TObject *Sender);
        void __fastcall EntryColorClick(TObject *Sender);
        void __fastcall PageControl1Changing(TObject *Sender,
          bool &AllowChange);
        void __fastcall KadrEntryListCompare(TObject *Sender,
          TListItem *Item1, TListItem *Item2, int Data, int &Compare);
        void __fastcall EntryDrawAsClick(TObject *Sender);
        void __fastcall ToolButton2Click(TObject *Sender);
        void __fastcall Panel1Resize(TObject *Sender);
        void __fastcall DownEntryClick(TObject *Sender);
        void __fastcall DeleteEntryClick(TObject *Sender);
        void __fastcall KadrsTreeDblClick(TObject *Sender);
        void __fastcall SpeedButton1Click(TObject *Sender);
        void __fastcall EntryPrecisionChange(TObject *Sender);
        void __fastcall KadrEntryListDragOver(TObject *Sender,
          TObject *Source, int X, int Y, TDragState State, bool &Accept);
        void __fastcall KadrEntryListDragDrop(TObject *Sender,
          TObject *Source, int X, int Y);
        void __fastcall SKadrNameChange(TObject *Sender);
        void __fastcall EntryNumberChange(TObject *Sender);
        void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
        void __fastcall NormalValueClick(TObject *Sender);
        void __fastcall ImageCentrClick(TObject *Sender);
        void __fastcall UpEntryClick(TObject *Sender);
        void __fastcall AnalogDrawAsChange(TObject *Sender);
private:	// User declarations
kadr_entryes_table  entry_table;
TTreeNode * root;
mdb_kadr    kadr;
DWORD Fapply_mask;
bool Fcreate_mode;
bool Fin_setup;

kadrs_map   kmap;  


void     __fastcall on_module_config_change(GKHANDLE mod,LPMODULE_CONFIG_DATA mcd);
void     __fastcall on_module_state(GKHANDLE mod,DWORD state) ;
void     __fastcall after_set_gkhandle();
LRESULT  __fastcall on_gkhandle_notify (GKHANDLE from,LPNOTIFY_CODE nc,DWORD mask);
void     __fastcall on_record_changed(DWORD rec_id,DWORD mask);
void     __fastcall on_record_removed(DWORD rec_id);

void     __fastcall on_kadr_created(DWORD kadr_id);
void     __fastcall on_kadr_content(LPMDB_KADR_CHANGED kc);
void     __fastcall on_kadr_removed(DWORD kadr_id);

void     __fastcall add_list_item(mdb_kadr_entry &entry);
bool     __fastcall get_kadr(DWORD kadr_id,MDB_KADR & kdr);
void     __fastcall paint_as_discrete(mdb_kadr_entry & entry,mdb_record & rec);
void     __fastcall paint_as_analog(mdb_kadr_entry & entry,mdb_record & rec);
void     __fastcall open_kadr_form();
void     __fastcall setup_kadr_form(TKadrForm * form);

mdb_kadr_entry *  __fastcall find_entry(DWORD id,DWORD number = -1);
mdb_kadr_entry *  __fastcall find_entry(TListItem * item);
AnsiString        __fastcall get_record_name(DWORD id,bool short_name);
AnsiString        __fastcall get_text       (DWORD str_id);
DWORD             __fastcall set_text       (DWORD str_id,AnsiString str);
bool              __fastcall get_record     (DWORD id,mdb_record & rec);


void   __fastcall get_entryes(TKadrForm * form);
void   __fastcall set_entryes();
void   __fastcall setup_kadrs_tree();
void   __fastcall setup_kadr(DWORD kadr_id,TKadrForm * form);
void   __fastcall do_create_kadr();
void   __fastcall do_apply_kadr();

        void __fastcall Setapply_mask(DWORD value);
        void __fastcall Setcreate_mode(bool value);
TTreeNode * __fastcall find_kadr_node(DWORD kid);        
TListItem * __fastcall find_list_item(mdb_kadr_entry * entry);
void   __fastcall on_kadr_modify(TKadrForm * form,DWORD ch_mask,mdb_kadr_entry * entry);
void   __fastcall on_delete_entry(TKadrForm * form,DWORD ch_mask,mdb_kadr_entry * entry);
void   __fastcall on_select_entry(TKadrForm * form,DWORD ch_mask,mdb_kadr_entry * entry);
void   __fastcall on_apply(TKadrForm * form,DWORD ch_mask,mdb_kadr_entry * entry);
void   __fastcall on_undo (TKadrForm * form,DWORD ch_mask,mdb_kadr_entry * entry);
void   __fastcall on_kadr_close (TKadrForm * form);
void   __fastcall on_kadr_destroy(TObject * Sender);
void   __fastcall renumber_entryes();

 void __fastcall record_operations(TKadrForm * Sender,DWORD record_id,DWORD op_code);


public:		// User declarations
        __fastcall TMdbKadrConstuctor(TComponent* Owner,GKHANDLE module);
        __property DWORD apply_mask  = { read=Fapply_mask, write=Setapply_mask };
        __property bool create_mode  = { read=Fcreate_mode, write=Setcreate_mode };
        __property bool in_setup  = { read=Fin_setup, write=Fin_setup };
};
//---------------------------------------------------------------------------
extern PACKAGE TMdbKadrConstuctor *MdbKadrConstuctor;
//---------------------------------------------------------------------------






#endif
