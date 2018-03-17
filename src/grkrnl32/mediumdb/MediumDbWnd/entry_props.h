//---------------------------------------------------------------------------

#ifndef entry_propsH
#define entry_propsH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.Buttons.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <otd_proto_medium.h>
#include <Vcl.Dialogs.hpp>

//---------------------------------------------------------------------------
class TEntryProps : public TForm
{
__published:	// IDE-managed Components
	TGridPanel *GridPanel1;
	TLabel *Label1;
	TLabel *Label2;
	TLabel *Label3;
	TLabel *Label4;
	TRadioGroup *short_name;
	TComboBox *paint_as;
	TPanel *bk_color;
	TSpeedButton *SelBkColor;
	TPanel *fg_color;
	TSpeedButton *SelFgColor;
	TColorDialog *ColorDialog1;
	TLabel *Label5;
	TPanel *Panel1;
	TEdit *Edit1;
	TUpDown *line_width;
	TLabel *Label6;
	TPanel *Panel2;
	TEdit *Edit2;
	TUpDown *precision;
	TLabel *Label7;
	TPanel *Panel3;
	TEdit *Edit3;
	TUpDown *digits;
	TCheckBox *part_of_background;
	void __fastcall SelBkColorClick(TObject *Sender);
	void __fastcall paint_asChange(TObject *Sender);
	void __fastcall short_nameClick(TObject *Sender);
	void __fastcall digitsChangingEx(TObject *Sender, bool &AllowChange, int NewValue,
          TUpDownDirection Direction);
	void __fastcall FormHide(TObject *Sender);
	void __fastcall line_widthChangingEx(TObject *Sender, bool &AllowChange, int NewValue,
          TUpDownDirection Direction);
	void __fastcall part_of_backgroundClick(TObject *Sender);
private:	// User declarations
       bool              Fdiscrete;
       mdb_kadr_entry    Fentry;
       int               Fpaint_context;
       DWORD             notify_command;
       int               Fin_setup;
      __fastcall TEntryProps(TComponent* Owner);
   void __fastcall get_paint_list();
public:		// User declarations
virtual __fastcall TEntryProps(TComponent* Owner,int pctx,DWORD _notify_command );
          void __fastcall set_discrete(bool discrete);
          void __fastcall set_entry   (mdb_kadr_entry val);
        __property bool              discrete = {read = Fdiscrete , write = set_discrete};
        __property mdb_kadr_entry    entry    = {read = Fentry    , write = set_entry   };
          void __fastcall notify_changes();
          void __fastcall update_paint_variants();

};
//---------------------------------------------------------------------------
extern PACKAGE TEntryProps *EntryProps;
//---------------------------------------------------------------------------
#endif
