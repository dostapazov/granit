//---------------------------------------------------------------------------
#pragma hdrstop
#include <vcl.h>
#include "gkopcitem_view_frame.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "gkmod_frame"
#pragma resource "*.dfm"
TGKOpcItemsViewFrame *GKOpcItemsViewFrame;
//---------------------------------------------------------------------------
__fastcall TGKOpcItemsViewFrame::TGKOpcItemsViewFrame(TComponent* Owner)
	: TGKModuleFrame(Owner)
{
}
//---------------------------------------------------------------------------
void     __fastcall  TGKOpcItemsViewFrame::current_object_change (TObject * value)
{
  TGKModuleFrame::current_object_change(value);
  setup_list_view();
}

void     __fastcall  TGKOpcItemsViewFrame::current_object_changed(TObject * ,bool &enable_set)
{
   ListView1->Items->Clear();
}


wchar_t * __fastcall variant_type_name(DWORD vtype)
{
 wchar_t *vt_names_0_38[] =

    {
        /*VT_EMPTY= 0,*/        L"empty",
        /*VT_NULL	= 1,*/	L"null",
        /*VT_I2	= 2,*/	L"short",
        /*VT_I4	= 3,*/	L"long",
        /*VT_R4	= 4,*/	L"float",
        /*VT_R8	= 5,*/	L"double",
        /*VT_CY	= 6,*/	L"vt_cy",
        /*VT_DATE	= 7,*/	L"date",
        /*VT_BSTR	= 8,*/	L"bstr",
        /*VT_DISPATCH	= 9,*/	L"dispatch",
        /*VT_ERROR	= 10,*/	L"error",
        /*VT_BOOL	= 11,*/	L"bool",
        /*VT_VARIANT	= 12,*/	L"variant",
        /*VT_UNKNOWN	= 13,*/	L"unknown",
        /*VT_DECIMAL	= 14,*/	L"decimal",
        /*VT_I1	= 16,*/	L"char",
        /*VT_UI1	= 17,*/	L"byte",
        /*VT_UI2	= 18,*/	L"word",
        /*VT_UI4	= 19,*/	L"dword",
        /*VT_I8	= 20,*/	L"__int64",
        /*VT_UI8	= 21,*/	L"byte",
        /*VT_INT	= 22,*/	L"int",
        /*VT_UINT	= 23,*/	L"uint",
        /*VT_VOID	= 24,*/	L"void",
        /*VT_HRESULT	= 25,*/	L"hresult",
        /*VT_PTR	= 26,*/	L"ptr",
        /*VT_SAFEARRAY	= 27,*/	L"safearray",
        /*VT_CARRAY	= 28,*/	L"carray",
        /*VT_USERDEFINED	= 29,*/	L"userdef",
        /*VT_LPSTR	= 30,*/	L"lpstr",
        /*VT_LPWSTR	= 31,*/	L"lpwstr",
        /*VT_RECORD	= 36,*/	L"record",
        /*VT_INT_PTR	= 37,*/	L"int *",
        /*VT_UINT_PTR	= 38,*/	L"uint *",
    };

 wchar_t *vt_names_64_73[] =
 {
        /*VT_FILETIME	= 64,*/	L"filetime",
        /*VT_BLOB	= 65,*/	L"blob",
        /*VT_STREAM	= 66,*/	L"stream",
        /*VT_STORAGE	= 67,*/	L"storage",
        /*VT_STREAMED_OBJECT	= 68,*/	L"streamed object",
        /*VT_STORED_OBJECT	= 69,*/	L"stored object",
        /*VT_BLOB_OBJECT	= 70,*/	L"blob object",
        /*VT_CF	= 71,*/	L"cf",
        /*VT_CLSID	= 72,*/	L"clsid",
        /*VT_VERSIONED_STREAM	= 73,*/	L"ver stream",
 };
 if(vtype<39)
     return vt_names_0_38[vtype];
 if(vtype>63 && vtype < 74)
     return vt_names_64_73[vtype-64];
  return L"?";

//        /*VT_BSTR_BLOB	= 0xfff,*/	L"bstr blob",
//        /*VT_VECTOR	= 0x1000,*/	L"vector",
//        /*VT_ARRAY	= 0x2000,*/	L"array",
//        /*VT_BYREF	= 0x4000,*/	L"byref",
//        /*VT_RESERVED	= 0x8000,*/	L"reserved",
//        /*VT_ILLEGAL	= 0xffff,*/	L"illegal",
//        /*VT_ILLEGALMASKED	= 0xfff,*/	L"ill masked",
//        /*VT_TYPEMASK	= 0xfff*/	L"typemask",


}

void     __fastcall  TGKOpcItemsViewFrame::setup_item(TListItem * item,gkopc_item * opc_item)
{
 if(item )
 {
  item->SubItems->Clear();
  if(opc_item)
  {
      item->Data = opc_item;
      item->Caption = opc_item->id.c_str();
      UnicodeString str;

      str = opc_item->item_state.wQuality;
      item->SubItems->Add(str);

      str = OleVariant(opc_item->item_state.vDataValue);
      item->SubItems->Add(str);
      str = OleVariant(opc_item->data_value);
      item->SubItems->Add(str);

      SYSTEMTIME st;
      FileTimeToSystemTime(&opc_item->item_state.ftTimeStamp,&st);
      str.printf(_T("%u.%02u.%02u %02u:%02u:%02u.%03u"),(DWORD) st.wYear,(DWORD) st.wMonth,(DWORD) st.wDay,(DWORD) st.wHour,(DWORD) st.wMinute,(DWORD) st.wSecond,(DWORD) st.wMilliseconds);
      item->SubItems->Add(str);

      str.printf(_T("%X"),opc_item->reg_result.hServer);
      item->SubItems->Add(str);

      str = opc_item->item_state.hClient;
      item->SubItems->Add(str);

      str = variant_type_name(opc_item->reg_result.vtCanonicalDataType);
      item->SubItems->Add(str);
  }
 }
}

void     __fastcall  TGKOpcItemsViewFrame::add_list_item (gkopc_item * opc_item)
{
  TListItem * item = ListView1->Items->Add();
  this->setup_item(item,opc_item);
}

void __fastcall TGKOpcItemsViewFrame::refresh_opc_item(WORD idx)
{

  if(idx< (WORD) ListView1->Items->Count)
  {
    ListView1->Items->BeginUpdate();
    TListItem  * item     = ListView1->Items->Item[idx];
    gkopc_item * opc_item = NULL;
    WORD line_num = (WORD )current_tree_node->Data;
    if(GKH_RET_SUCCESS == mod_iface.call(CMD_OPC_LINE_GET_ITEM,MAKELONG(line_num,idx),(LPARAM)&opc_item))
    {
      ListView1Deletion(ListView1,item);
      this->setup_item(item,opc_item);
    }
    ListView1->Items->EndUpdate();
  }
}


void     __fastcall  TGKOpcItemsViewFrame::setup_list_view ()
{
  ListView1->Items->BeginUpdate();
  ListView1->Items->Clear();
  if(current_tree_node && current_tree_node->Data != (LPVOID)-1)
  {
   gkopc_item * item_ptr = NULL;
   WORD line_num = (WORD )current_tree_node->Data;
   WORD idx     = 0;
   while(GKH_RET_SUCCESS == mod_iface.call(CMD_OPC_LINE_GET_ITEM,MAKELONG(line_num,idx),(LPARAM)&item_ptr))
   {
     add_list_item(item_ptr);
     idx++;
   }
  }
  ListView1->Items->EndUpdate();
}

void __fastcall TGKOpcItemsViewFrame::ListView1Deletion(TObject *Sender, TListItem *Item)

{
  if(current_tree_node && Item && Item->Data )
  {
    DWORD line_num = (DWORD )current_tree_node->Data;
    mod_iface.call(CMD_OPC_LINE_RELEASE_ITEM,line_num,(LPARAM)Item->Data);
  }
}
//---------------------------------------------------------------------------


