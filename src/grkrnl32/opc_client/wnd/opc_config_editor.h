//---------------------------------------------------------------------------

#ifndef opc_config_editorH
#define opc_config_editorH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>

#include <opc_helper.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.ToolWin.hpp>
#include <System.ImageList.hpp>
#include <Vcl.ImgList.hpp>

#include <otd_addr.h>
#include <gkopc_client.h>
#include <Vcl.Menus.hpp>
#include <Vcl.Dialogs.hpp>
#include <Vcl.Buttons.hpp>

#include <map>

//---------------------------------------------------------------------------
struct gkopc_item_def_vcl : public gkopc_item
{
  TTreeNode * opc_node;
  TTreeNode * otd_node;
  gkopc_item_def_vcl():gkopc_item(),opc_node(NULL),otd_node(NULL){}
  gkopc_item_def_vcl(const UnicodeString _id,const UnicodeString _acp, const group_param_t & gp)
                    :gkopc_item(_id.c_str(),_acp.c_str(),gp)
                    ,opc_node(NULL),otd_node(NULL){}

  gkopc_item_def_vcl(const gkopc_item_def_vcl & src){*this = src;}
  gkopc_item_def_vcl & operator = (const gkopc_item_def_vcl & src)
  {
    *((gkopc_item *)this) = src;
    opc_node = src.opc_node;
    otd_node = src.otd_node;
    return *this;
  }
  ~gkopc_item_def_vcl()
  {
   if(opc_node)
      opc_node->Data = NULL;
      opc_node = NULL;
      otd_node = NULL;

  }
};

typedef vector_index::indexed_vector<gkopc_item_def_vcl,gkopc_item_comparer,gkopc_item_comparer_group_param> gkopc_items_vector;


class TOpcConfigEditor : public TForm
{
__published:	// IDE-managed Components
	TPanel *Panel1;
	TPanel *Panel2;
	TLabel *Label2;
	TStaticText *ServerStateText;
	TLabel *Label3;
	TStaticText *ServerStartTime;
	TTreeView *OpcServerItemsTree;
	TLabel *Label4;
	TStaticText *ServerCurrentTime;
	TLabel *Label1;
	TStaticText *ServerUpdateTime;
	TStatusBar *StatusBar1;
	TToolBar *ToolBar1;
	TToolButton *tbRefreshOpcItems;
	TTimer *Timer1;
	TTreeView *OtdTree;
	TImageList *ImageList1;
	TToolButton *ToolButton2;
	TToolButton *tbCreateGroup;
	TToolButton *tbRemove;
	TToolButton *ToolButton1;
	TToolButton *tbAddItem;
	TToolButton *tbSave;
	TPopupMenu *SaveMenu;
	TMenuItem *miSave;
	TMenuItem *miSaveAs;
	TSaveDialog *SaveDialog1;
	TToolButton *tbMoveUp;
	TToolButton *tbMoveDown;
	TToolButton *ToolButton5;
	TToolButton *tbRenumber;
	TSplitter *Splitter1;
	TSplitter *Splitter2;
	TPanel *Panel3;
	TListView *opc_item_props_view;
	TLabel *Label5;
	TStaticText *GroupCount;
	TPanel *Panel4;
	TSplitter *Splitter3;
	TPopupMenu *PopupMenu1;
	TMenuItem *miCopyTagName;
	TPanel *item_panel;
	TPageControl *PageControl1;
	TTabSheet *TabSheet1;
	TTabSheet *TabSheet2;
	TLabel *Label6;
	TEdit *tutr_object;
	TLabel *Label7;
	TEdit *tutr_tag;
	TMemo *CalcScript;
	TToolButton *ToolButton3;
	TToolButton *ApplyPersonal;
	TToolButton *UndoPersonal;
	TToolButton *ToolButton4;
	TToolButton *bExport;
	TToolButton *bImport;
	TSaveDialog *ExportDlg;
	TOpenDialog *ImportDlg;
	TTabSheet *TabSheet4;
	TMemo *Memo1;
	TMemo *TuTrScript;
	TLabel *Label8;
	TToolButton *tbFlat;
	TPanel *Panel5;
	TEdit *edFilter;
	TPanel *Panel6;
	TLabel *Label9;
	TEdit *FolderDiv;
	TLabel *Label10;
	TEdit *ItemDiv;

	void __fastcall tbRefreshOpcItemsClick(TObject *Sender);
	void __fastcall Timer1Timer(TObject *Sender);
	void __fastcall OpcServerItemsTreeChange(TObject *Sender, TTreeNode *Node);
	void __fastcall FormResize(TObject *Sender);
	void __fastcall OtdTreeChange(TObject *Sender, TTreeNode *Node);
	void __fastcall tbCreateGroupClick(TObject *Sender);
	void __fastcall tbRemoveClick(TObject *Sender);
	void __fastcall tbAddItemClick(TObject *Sender);
	void __fastcall OpcServerItemsTreeCustomDrawItem(TCustomTreeView *Sender, TTreeNode *Node,
          TCustomDrawState State, bool &DefaultDraw);
	void __fastcall OpcServerItemsTreeAddition(TObject *Sender, TTreeNode *Node);
	void __fastcall OpcServerItemsTreeDeletion(TObject *Sender, TTreeNode *Node);
	void __fastcall OtdTreeDeletion(TObject *Sender, TTreeNode *Node);
	void __fastcall miSaveClick(TObject *Sender);
	void __fastcall miSaveAsClick(TObject *Sender);
	void __fastcall Memo1DblClick(TObject *Sender);
	void __fastcall tbMoveUpClick(TObject *Sender);
	void __fastcall tbMoveDownClick(TObject *Sender);
	void __fastcall tbRenumberClick(TObject *Sender);
	void __fastcall opc_item_props_viewResize(TObject *Sender);
	void __fastcall tbSaveClick(TObject *Sender);
	void __fastcall OpcServerItemsTreeExpanding(TObject *Sender, TTreeNode *Node, bool &AllowExpansion);
	void __fastcall miCopyTagNameClick(TObject *Sender);
	void __fastcall ApplyPersonalClick(TObject *Sender);
	void __fastcall UndoPersonalClick(TObject *Sender);
	void __fastcall bExportClick(TObject *Sender);
	void __fastcall StatusBar1DrawPanel(TStatusBar *StatusBar, TStatusPanel *Panel,
          const TRect &Rect);
	void __fastcall bImportClick(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall OpcServerItemsTreeCollapsing(TObject *Sender, TTreeNode *Node, bool &AllowCollapse);



private:	// User declarations
         UnicodeString                    FOpcServerHost;
         UnicodeString                    FOpcServerName;
         GUID                             FOpcServerGuid;
         opc::TOpcServer                *  opc_server;
         opc::TOpcGroup                 *  opc_group;
         opc::TOpcBrowseServerAddrSpace *  opc_browser;
         TComInterface<IOPCItemProperties> iprops;
         bool is_tree_model;

         UnicodeString FConfigFile;
         TTreeNode *    discrete_node;
         TTreeNode *    analog1_node;
         TTreeNode *    analog2_node;

         gkopc_items_vector opc_items;
//       UnicodeString  divisor;
//		 UnicodeString  prefix;
//		 UnicodeString  folder_divisor;
//		 UnicodeString  item_divisor;

         int     progress_top;
         int     progress_current;

         float   __fastcall get_progress_percent();
         void    __fastcall set_opc_server_guid(GUID _guid);
         GUID    __fastcall get_opc_server_guid();
         void    __fastcall set_opc_server_host(UnicodeString str);
UnicodeString    __fastcall get_opc_server_host();
         void    __fastcall set_opc_server_name(UnicodeString str);
UnicodeString    __fastcall get_opc_server_name();
         void    __fastcall set_opc_config_name(UnicodeString str);
UnicodeString    __fastcall get_opc_config_name();
         void    __fastcall set_changes(bool ch );
         bool    __fastcall get_changes();
         void    __fastcall opc_close_server();
         void    __fastcall opc_open_server ();
         void    __fastcall opc_server_refresh_info();
         void    __fastcall set_opc_state_text(OPCSERVERSTATE state);
         HRESULT __fastcall add_to_opc_group(const wchar_t * item_id,OPCITEMRESULT * iresult);

		 void    __fastcall fill_opc_items();
         void    __fastcall fill_opc_items_tree(TTreeNode * parent);
         void    __fastcall fill_opc_items_flat();
		 void    __fastcall setup_opc_item_props_list(DWORD prop_count,LPDWORD prop_ids,LPWSTR * prop_descr,LPVARIANT  prop_values);
		 void    __fastcall setup_item_result        (OPCITEMRESULT * iresult);
    TTreeNode*   __fastcall create_group_node(TTreeNode * node,DWORD num = -1);
         bool    __fastcall check_enable_add_item   ();
         bool    __fastcall check_enable_delete_item();
         bool    __fastcall add_gkopc_item(UnicodeString id,UnicodeString access_path,group_param_t & gp,gkopc_items_vector::iterator & ptr);
         void    __fastcall add_gkopc_item (TTreeNode * src,TTreeNode * dst);
         bool    __fastcall identfy_divisor(TTreeNode *item_node);
         void    __fastcall do_export(UnicodeString file_name);
         void    __fastcall do_import(UnicodeString file_name);

gkopc_items_vector::iterator  __fastcall add_gkopc_item(UnicodeString id,UnicodeString access_path,group_param_t & gp,TTreeNode * opc_item,TTreeNode * otd_item);

       TTreeNode*__fastcall get_otd_root_node(BYTE fa);
        void     __fastcall build_otd_tree();
        void     __fastcall read_config();
        void     __fastcall write_config();

        void     __fastcall move_otd_node      (TTreeNode * node,bool down);
        void     __fastcall move_otd_group_node(TTreeNode * node,bool down);
        void     __fastcall renumber_otd_group (TTreeNode * node);
        void     __fastcall swap_otd_nodes     (gkopc_item_def_vcl & i1,gkopc_item_def_vcl & i2);
        void     __fastcall swap_otd_nodes     (TTreeNode * node1,TTreeNode * node2);
        void     __fastcall set_otd_node_group_param(TTreeNode * node,const group_param_t &gp);
        void     __fastcall set_item_personal_params(gkopc_item_def_vcl & item,bool from_controls);
		void     __fastcall set_item_personal_params(TTreeNode * node,bool from_controls);

		void     __fastcall show_opc_error (HRESULT res);

public:		// User declarations
	__fastcall TOpcConfigEditor(TComponent* Owner);
        __property GUID          opc_server_guid = {read = get_opc_server_guid, write = set_opc_server_guid };
        __property UnicodeString opc_server_host = {read = get_opc_server_host, write = set_opc_server_host };
        __property UnicodeString opc_server_name = {read = get_opc_server_name, write = set_opc_server_name };
        __property UnicodeString opc_config_name = {read = get_opc_config_name, write = set_opc_config_name };
        __property bool          changes         = {read = get_changes        , write = set_changes         };

         void    __fastcall opc_open_server (GUID _guid,UnicodeString host,UnicodeString ProgId);

         void    __fastcall set_current_group(TTreeNode * node,opc::TOpcBrowseServerAddrSpace *  opc_browser);

static  UnicodeString __fastcall get_time_str(__int64 timestamp,bool local = true);
static  void          __fastcall com_mem_free(LPVOID ptr){CoTaskMemFree(ptr);}
static  void          __fastcall com_mem_free(DWORD count, LPWSTR * ptr);
static  void          __fastcall com_mem_free(DWORD count, LPVARIANT  ptr);
static  TTreeNode *   __fastcall get_root_node(TTreeNode * node);
static  bool          __fastcall is_param     (TTreeNode * node);
static  bool          __fastcall is_group     (TTreeNode * node);
static  bool          __fastcall is_group_root(TTreeNode * node);
static UnicodeString  __fastcall get_param_text(DWORD pn);
static UnicodeString  __fastcall get_item_opc_id(TTreeNode * node,UnicodeString _divisor,UnicodeString _prefix);


};
//---------------------------------------------------------------------------

inline bool __fastcall TOpcConfigEditor::is_param(TTreeNode * node)
{
      group_param_t gp(node->Data);
      return (gp.param == (WORD)-1) ? false : true;
}

inline bool          __fastcall TOpcConfigEditor::is_group     (TTreeNode * node)
{
   group_param_t gp(node->Data);
   return (gp.param == (WORD)-1 && gp.sb != (BYTE)-1) ? true : false;
}

inline bool          __fastcall TOpcConfigEditor::is_group_root(TTreeNode * node)
{
   return node->Parent ? false : true;
}


extern PACKAGE TOpcConfigEditor *OpcConfigEditor;
//---------------------------------------------------------------------------
#endif
