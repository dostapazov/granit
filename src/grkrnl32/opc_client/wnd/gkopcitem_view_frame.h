//---------------------------------------------------------------------------

#ifndef gkopcitem_view_frameH
#define gkopcitem_view_frameH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include "gkmod_frame.h"
#include <Vcl.ComCtrls.hpp>
#include <gkopc_client.h>

//---------------------------------------------------------------------------
class TGKOpcItemsViewFrame : public TGKModuleFrame
{
__published:	// IDE-managed Components
	TListView *ListView1;
	void __fastcall ListView1Deletion(TObject *Sender, TListItem *Item);
private:	// User declarations
void     __fastcall  current_object_change (TObject * value);
void     __fastcall  current_object_changed(TObject * ,bool &enable_set);
void     __fastcall  setup_list_view       ();
void     __fastcall  add_list_item         (gkopc_item * item_ptr);
void     __fastcall  setup_item(TListItem * item,gkopc_item * opc_item);


public:		// User declarations
	__fastcall TGKOpcItemsViewFrame(TComponent* Owner);
   void __fastcall refresh_opc_item(WORD idx);
};
//---------------------------------------------------------------------------
extern PACKAGE TGKOpcItemsViewFrame *GKOpcItemsViewFrame;
//---------------------------------------------------------------------------

wchar_t * __fastcall variant_type_name(DWORD vtype);

#endif
