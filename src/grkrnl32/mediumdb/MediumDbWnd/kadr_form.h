//---------------------------------------------------------------------------

#ifndef kadr_formH
#define kadr_formH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <mediumdb.h>
#include <kadrs_table.hpp>
#include "pictdlgs.h"
#include "gkmodule_form.h"
#include <ComCtrls.hpp>
#include <ImgList.hpp>
#include <ExtCtrls.hpp>
#include <ToolWin.hpp>
#include <tlm_painter.h>
#include <System.ImageList.hpp>
#ifdef __CODEGEARC__
#include <System.ImageList.hpp>
#endif


//---------------------------------------------------------------------------
struct kadr_entry
{
 mdb_kadr_entry    descript;
 MDB_SHORT_RECORD  value;
 DWORD             switch_count;
 WideString        sname;
 WideString        lname;
 kadr_entry(){descript.kadr_id = -1;};
 kadr_entry   (const kadr_entry & ke){*this = ke;}
 kadr_entry & operator =(const MDB_KADR_ENTRY &ke)   {memcpy(&descript,&ke,sizeof(descript));return * this;}
 kadr_entry & operator =(const MDB_SHORT_RECORD &rec){memcpy(&value,&rec,sizeof(value)); return * this;}
 kadr_entry & operator =(const kadr_entry &ke)
 {memcpy(&this->descript,&ke.descript,sizeof(this->descript));
 memcpy(&this->value,&ke.value,sizeof(this->value));
 sname = ke.sname;
 lname = ke.lname;
 return * this;}
};

bool operator < (const kadr_entry & ke1,const kadr_entry & ke2)
{
 if(ke1.descript.rec_id == ke2.descript.rec_id)
   return ke1.descript.number < ke2.descript.number ? true:false;
    return ke1.descript.rec_id < ke2.descript.rec_id ? true:false;
}

class TKadrForm;
typedef void __fastcall ( __closure * entry_notify_event)(TKadrForm * Sender,DWORD mask, mdb_kadr_entry * e);
typedef void __fastcall ( __closure * record_operations_event )(TKadrForm * Sender,DWORD record_id,DWORD op_code);


class TKadrForm : public TForm
{
__published:	// IDE-managed Components
        TStatusBar *StatusBar1;
        TTimer *Timer1;
        TToolBar *ToolBar1;
        TToolButton *ToolButton1;
        TToolButton *ReceiptBtn;
        TToolButton *ToolButton3;
        TToolButton *ApplyBtn;
        TToolButton *UndoBtn;
        TToolButton *DeleteBtn;
        TToolButton *ToolButton4;
        TToolButton *ToolButton5;
        TToolButton *ToolButton6;
        TImageList *ImageList1;
        TPanel *Panel1;
        TComboBox *EntryDrawAs;
        TCheckBox *CheckBox1;
        void __fastcall FormPaint(TObject *Sender);
        void __fastcall FormResize(TObject *Sender);
        void __fastcall FormMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
        void __fastcall FormMouseUp(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
        void __fastcall FormMouseMove(TObject *Sender, TShiftState Shift,
          int X, int Y);
        void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
        void __fastcall Timer1Timer(TObject *Sender);
        void __fastcall ReceiptBtnClick(TObject *Sender);
        void __fastcall DeleteBtnClick(TObject *Sender);
        void __fastcall ApplyBtnClick(TObject *Sender);
        void __fastcall UndoBtnClick(TObject *Sender);
        void __fastcall ToolButton4Click(TObject *Sender);
        void __fastcall ToolButton5Click(TObject *Sender);
        void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
        void __fastcall EntryDrawAsChange(TObject *Sender);
public:
typedef std::vector<kadr_entry> entry_table;

protected:	// User declarations

 TGKModuleLoader    painter_dll;
 HDC                MemoryDC;
 HBITMAP            dmask_bmp;
 HBITMAP            doff_bmp;
 HBITMAP            don_bmp;
 HBITMAP            bkgnd_bitmap;
 HANDLE             bkgnd_dib;

 AnsiString         FBkgndImage;
 mdb_kadr           Fkadr;
 BOOL               mouse_captured;
 BOOL               Feditable;
 DWORD              Fchanges;
 DWORD              flash_counter;
 DWORD              in_setup;
 entry_notify_event Fon_modify_kadr;
 entry_notify_event Fon_select_entry;
 entry_notify_event Fon_delete_entry;
 entry_notify_event Fon_apply;
 entry_notify_event Fon_undo;

 record_operations_event Frecord_operations;


 void        __fastcall set_changes(DWORD ch,mdb_kadr_entry * entry);
 DWORD       __fastcall Getentry_count();
 void        __fastcall setup_bkgnd();

 entry_table::iterator   selected_entry,over_entry;
 POINT                   spot;
 BOOL                    lb_down;
 BOOL                    rb_down;
 entry_table             Fentry_items;



 void         __fastcall LoadBitmap();
 void         __fastcall SetBkgndImage(AnsiString str);
 mdb_kadr *   __fastcall get_kadr(){return &Fkadr;}
 void         __fastcall paint_discrete_lamp(kadr_entry &entry,bool flash);
 void         __fastcall paint_discrete_swhor (kadr_entry &entry,bool flash);
 void         __fastcall paint_discrete_swvert(kadr_entry &entry,bool flash);
 void         __fastcall paint_discrete_sname (kadr_entry &entry,bool flash);
 void         __fastcall paint_discrete(kadr_entry & entry,bool flash = false);
 void         __fastcall paint_analog     (kadr_entry & entry,bool flash = false);
 void         __fastcall paint_analog_text(kadr_entry & entry,bool flash = false);
 void         __fastcall paint_analog_vertbar(kadr_entry &,bool flash);
 void         __fastcall paint_analog_horbar(kadr_entry & entry,bool flash); 
 void         __fastcall paint_entry(kadr_entry & entry,bool flash = false);
 void         __fastcall paint_entry_frame(kadr_entry & entry);
 void         __fastcall select_entry(kadr_entry * entry,bool notify = true);
 void __fastcall WndProc(Messages::TMessage &Message);
 entry_table::iterator __fastcall find_entry(int x,int y);

 void __fastcall move_entry(kadr_entry & entry,int dx,int dy);
 void __fastcall grow_entry(kadr_entry & entry,int dx,int dy);
 void __fastcall show_entry_info(kadr_entry * entry);
 void __fastcall update_status_bar(); 

 


public:		// User declarations
 __fastcall TKadrForm(TComponent* Owner);
 __property AnsiString     BkgndImage = {read = FBkgndImage,write = SetBkgndImage};
 __property mdb_kadr *     kadr       = {read = get_kadr};
 __property BOOL           editable  = {read = Feditable,write = set_editable};
 __property DWORD          changes    = {read = Fchanges,write = set_changes};
 __property entry_notify_event  on_modify_kadr  = {read = Fon_modify_kadr,write = Fon_modify_kadr};
 __property entry_notify_event  on_delete_entry  = {read = Fon_delete_entry,write = Fon_delete_entry};
 __property entry_notify_event  on_select_entry  = {read = Fon_select_entry,write = Fon_select_entry};
 __property entry_notify_event  on_apply         = {read = Fon_apply       ,write = Fon_apply       };
 __property entry_notify_event  on_undo          = {read = Fon_undo        ,write = Fon_undo        };
 __property record_operations_event record_operations = {read = Frecord_operations,write = Frecord_operations};

 __property DWORD entry_count  = { read=Getentry_count };
 __property mdb_kadr_entry     entry_item[DWORD idx] = {read = get_entry};
 __property LONG kadr_width   = {read = Fkadr.kadr_size.cx,write = set_kadr_width };
 __property LONG kadr_height  = {read = Fkadr.kadr_size.cy,write = set_kadr_height};

 mdb_kadr_entry  __fastcall get_entry(DWORD idx){return Fentry_items.at(idx).descript;}
 void        __fastcall set_changes(DWORD ch);
 void        __fastcall set_editable(BOOL value);

 void __fastcall set_kadr_width (LONG kw);
 void __fastcall set_kadr_height(LONG kh); 
 void __fastcall set_kadr (mdb_kadr & kadr);
 void __fastcall add_entry(mdb_kadr_entry & ke,MDB_SHORT_RECORD &sr,AnsiString sname,AnsiString name);
 void __fastcall update_value(MDB_SHORT_RECORD &sr,DWORD mask);
 void __fastcall update_entry(mdb_kadr_entry * entry);
 void __fastcall select_entry(mdb_kadr_entry * entry);
 void __fastcall set_entry_name(DWORD rec_id,AnsiString name,bool sname);
 void __fastcall clear();
 void __fastcall clear_entryes();

};
//---------------------------------------------------------------------------
extern PACKAGE TKadrForm *KadrForm;
//---------------------------------------------------------------------------


inline DWORD __fastcall TKadrForm::Getentry_count()
{
        return Fentry_items.size();
}




#endif
