//---------------------------------------------------------------------------

#ifndef import_csvH
#define import_csvH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.Grids.hpp>
#include <Vcl.Buttons.hpp>
#include <Vcl.Dialogs.hpp>
#include <Vcl.ComCtrls.hpp>
#include <gkmodules.hpp>
#include <otd_addr.h>
#include <mediumdb.h>
//---------------------------------------------------------------------------
class TImportCSV : public TForm
{
__published:	// IDE-managed Components
	TBitBtn *bOk;
	TBitBtn *bCancel;
	TOpenDialog *OpenDialog1;
	TPageControl *PageControl1;
	TTabSheet *TabSheet1;
	TStringGrid *ImpData;
	TTabSheet *TabSheet2;
	TMemo *Memo1;
	TProgressBar *ProgressBar1;
	TTabSheet *TabSheet3;
	TMemo *Memo2;
	TSaveDialog *SaveDialog1;
	TRadioButton *rbAddRecords;
	TRadioButton *rbUpdateRecords;
	void __fastcall bOkClick(TObject *Sender);
private:	// User declarations
protected:
         otd_param_addr  addr;
         float min_val,max_val;
         int kvants;
         DWORD kadr_id;
         UnicodeString kadr_name;
         TTreeNode     * Ftree_node;

         TGKModuleInterface  mod_iface;
         void __fastcall do_import();
         void __fastcall import_line(int lno);
         bool  __fastcall get_otd_addr(otd_param_addr & addr,TStrings * s );
         void  __fastcall get_scale_values(BYTE fa,TStrings * s,float & min_value,float & max_value,DWORD & kvants,DWORD & options,DWORD & percent);
         DWORD __fastcall get_name_id(TStrings * s,bool long_name);

         void __fastcall set_tree_node   (TTreeNode * node);
         void __fastcall make_export_data(TTreeNode * node);
         void __fastcall rec2text        (mdb_record & rec,TStrings * s);
         bool __fastcall text2rec        (mdb_record & rec,UnicodeString & long_name,UnicodeString & short_name,TStrings * s);

public:		// User declarations
	__fastcall TImportCSV(TComponent* Owner);
        bool __fastcall select_file(HWND wnd);
        __property GKHANDLE gkmodule = {read = get_gkhandle, write = set_gkhandle};
        __property TTreeNode * tree_node = {read = Ftree_node, write = set_tree_node};
        GKHANDLE __fastcall get_gkhandle(){return mod_iface();}
           void  __fastcall set_gkhandle(GKHANDLE handle);
           DWORD __fastcall TImportCSV::set_text(DWORD str_id,WideString str);

};
//---------------------------------------------------------------------------
extern PACKAGE TImportCSV *ImportCSV;
//---------------------------------------------------------------------------
#endif
