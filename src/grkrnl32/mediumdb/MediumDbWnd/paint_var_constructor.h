//---------------------------------------------------------------------------

#ifndef paint_var_constructorH
#define paint_var_constructorH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include "tlm_painter_form.h"
#include <Vcl.ComCtrls.hpp>
#include <System.ImageList.hpp>
#include <Vcl.Buttons.hpp>
#include <Vcl.ImgList.hpp>
#include <tlm_painter.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.Dialogs.hpp>
#include <Vcl.Menus.hpp>
#include <Vcl.ToolWin.hpp>
#include <Vcl.ExtDlgs.hpp>


struct  paint_variant_param
{
 UnicodeString      name_id;
 int                cx;
 int                cy;
 bool               switched;
 UnicodeString      image;
 QWORD              image_time;
 UnicodeString      mask_id;
 QWORD              mask_time;
 int                multi_position;
};

typedef bool (__closure __fastcall * enum_variant_func_t)(int paint_context,int idx,UnicodeString & name);
typedef bool (__closure __fastcall * get_variant_data_t )(int paint_context,int idx,UnicodeString & image_name,UnicodeString & mask_name,int & cx,int & cy );

//---------------------------------------------------------------------------
class TPaintVariantConstruct : public TTlmPainterForm
{
__published:	// IDE-managed Components
	TImageList *ImageList1;
	TGroupBox *GroupBox1;
	TLabel *Label2;
	TEdit *elements_count;
	TLabel *Label3;
	TEdit *element_width;
	TLabel *Label4;
	TStaticText *image_size;
	TGroupBox *GroupBox2;
	TStaticText *image_name;
	TImage *pv_image;
	TGroupBox *GroupBox3;
	TImage *pv_mask;
	TStaticText *mask_name;
	TLabel *Label5;
	TComboBox *multi_pos;
	TGroupBox *GroupBox4;
	TGridPanel *GridPanel1;
	TLabel *Label6;
	TPanel *Panel2;
	TTrackBar *rec_value;
	TLabel *Label7;
	TPanel *Panel3;
	TCheckBox *st_work_out;
	TCheckBox *st_not_normal;
	TCheckBox *st_tutr;
	TCheckBox *st_diag;
	TLabel *Label8;
	TRadioGroup *ch_count;
	TLabel *Label9;
	TPanel *bk_color;
	TSpeedButton *SelBkColor;
	TLabel *Label10;
	TPanel *fg_color;
	TSpeedButton *SelFgColor;
	TCheckBox *cb_rec_value;
	TColorDialog *ColorDialog1;
	TPanel *Panel1;
	TLabel *Label1;
	TSpeedButton *bApply;
	TSpeedButton *bDelete;
	TComboBox *PaintVariantList;
	TOpenPictureDialog *OpenPictureDialog1;
	TSpeedButton *bAdd;
	TSpeedButton *bSave;
	void __fastcall PaintVariantListChange(TObject *Sender);
	void __fastcall PaintVariantListSelect(TObject *Sender);
	void __fastcall rec_valueChange(TObject *Sender);
	void __fastcall st_work_outClick(TObject *Sender);
	void __fastcall SelBkColorClick(TObject *Sender);
	void __fastcall cb_rec_valueClick(TObject *Sender);
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall pv_imageClick(TObject *Sender);
	void __fastcall elements_countChange(TObject *Sender);
	void __fastcall element_widthChange(TObject *Sender);
	void __fastcall multi_posChange(TObject *Sender);
	void __fastcall bDeleteClick(TObject *Sender);
	void __fastcall bApplyClick(TObject *Sender);
	void __fastcall bAddClick(TObject *Sender);
	void __fastcall bSaveClick(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
private:	// User declarations
       int  Fglobal_changes;
       int  Fchanges;
       int  Fin_setup;
       UnicodeString rec_paint_name ;
       tlmpaint::painter_dll  * paint_dll;
       LPVOID                   paint_variant;
       enum_variant_func_t      Fenum_variant;
       get_variant_data_t       Fget_variant_data;
       void __fastcall setup_list();
       bool __fastcall is_new_variant    ();
	__fastcall TPaintVariantConstruct(TComponent* Owner);

      virtual void __fastcall paint_entryes         (bool erase,bool erase_flash);
      virtual void __fastcall paint_selected_entryes(bool erase,bool erase_flash);
              void __fastcall setup_paint_entry     ();
              void __fastcall setup_controls        (LPVOID pvar);

public:		// User declarations
virtual __fastcall TPaintVariantConstruct(TComponent* Owner,int paint_context);
   bool __fastcall do_enum_variant   (int idx,UnicodeString & var_name);
   bool __fastcall do_get_variant    (int idx,UnicodeString & img_name,UnicodeString & mask_name,int & cx,int & cy);
LPVOID  __fastcall get_paint_variant (const MDB_KADR_ENTRY & entry) const{return paint_variant;}
   void __fastcall set_multiposition (int val);
   int  __fastcall get_multiposition (void   );
   void __fastcall set_element_width (int val);
   int  __fastcall get_element_width (void   );
   int  __fastcall get_element_height(void   );
   void __fastcall set_elements_count(int val);
   int  __fastcall get_elements_count(void   );

   void __fastcall set_paint_variant (LPVOID pvar);

   void __fastcall set_image         (UnicodeString file_name,bool mask);
   void __fastcall set_changes       (int ch);
   void __fastcall set_global_changes(int ch);

        __property int changes         = {read = Fchanges       , write = set_changes       };
        __property int global_changes  = {read = Fglobal_changes, write = set_global_changes};
   virtual       bool      __fastcall get_record   (DWORD rec_id,MDB_RECORD & rec,bool * rec_visible) const;
   virtual const wchar_t * __fastcall get_name     (MDB_RECORD & rec,bool short_name) const;
   //const wchar_t * __fastcall get_name(MDB_RECORD & rec) const;
};
//---------------------------------------------------------------------------
extern PACKAGE TPaintVariantConstruct *PaintVariantConstruct;
//---------------------------------------------------------------------------
#endif
