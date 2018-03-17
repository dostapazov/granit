//---------------------------------------------------------------------------

#ifndef mdb_tree_frameH
#define mdb_tree_frameH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include "gkmod_frame.h"
#include <Vcl.ComCtrls.hpp>
#include <mediumdb.h>
//---------------------------------------------------------------------------
class TMdbTreeFrame : public TGKModuleFrame
{
__published:	// IDE-managed Components
	TTreeView *MdbTree;
	void __fastcall MdbTreeExpanding(TObject *Sender, TTreeNode *Node, bool &AllowExpansion);

private:	// User declarations
protected:
TTreeNode        * db_root;
TTreeNode        * db_root_combine;
bool               Fcreate_mode;
virtual  void     __fastcall after_set_gkhandle();
         void     __fastcall setup_mdb_tree    ();
         void     __fastcall expand_node(TTreeNode * parent,sotd_param_addr & pa);
         void     __fastcall expand_combine();


public:		// User declarations
	      __fastcall TMdbTreeFrame(TComponent* Owner);
              __property bool create_mode  = { read=Fcreate_mode, write=set_create_mode };
virtual  void __fastcall set_create_mode(bool value);
  TTreeNode * __fastcall add_node(TTreeNode * parent,mdb_record & rec);
sotd_param_addr
              __fastcall get_node_addr  (TTreeNode * node);
         bool __fastcall get_node_record(TTreeNode *node, mdb_record & rec);
         bool __fastcall get_record     (DWORD id, mdb_record & rec);
         bool __fastcall get_record     (sotd_param_addr & pa, mdb_record & rec);
         void __fastcall set_node_text  (TTreeNode * node, mdb_record & rec);

};
//---------------------------------------------------------------------------
extern PACKAGE TMdbTreeFrame *MdbTreeFrame;
//---------------------------------------------------------------------------
#endif
