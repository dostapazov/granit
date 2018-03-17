//---------------------------------------------------------------------------

#ifndef vtdsetupH
#define vtdsetupH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "gkmodule_form.h"
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <ToolWin.hpp>
#include  <modemmer.h>
#include "gkmod_frame.h"
#include "mod_config_frame.h"
#include "vtd_line_cfg_frame.h"
#include <ImgList.hpp>
#include "..\vtdcnt.h"
#include "MonFrame.h"
#include "vtd_group_cfg_frame.h"
#include "view_frame.h"
#include <Menus.hpp>
#include <System.ImageList.hpp>

//---------------------------------------------------------------------------
class TVTDSetupFrm : public TGKModuleForm
{
__published:	// IDE-managed Components
        TToolBar *ToolBar1;
        TStatusBar *StatusBar1;
        TTreeView *TreeView1;
        TSplitter *Splitter1;
        TPageControl *PageControl1;
        TTabSheet *TabSheet1;
        TTabSheet *TabSheet2;
        TTabSheet *TabSheet3;
        TVtdModConfig *VtdModConfig1;
        TVtdLineConfig *VtdLineConfig1;
        TToolButton *StartBtn;
        TToolButton *StopBtn;
        TToolButton *ToolButton3;
        TToolButton *NewBtn;
        TToolButton *DelBtn;
        TToolButton *ToolButton1;
        TToolButton *ApplyBtn;
        TToolButton *UndoBtn;
        TImageList *ImageList1;
        TMonitorFrame *MonitorFrame1;
        TVtdGroupConfig *VtdGroupConfig1;
        TToolButton *ToolButton2;
        TToolButton *RefreshBtn;
        TTabSheet *TabSheet4;
        TMemo *Memo1;
        TLabel *Label30;
        TViewFrame *ViewFrame1;
        TToolButton *ToolButton4;
        TPopupMenu *PopupMenu1;
        TMenuItem *N1;
        TMenuItem *N2;
        TLabel *Label2;
        void __fastcall TreeView1Change(TObject *Sender, TTreeNode *Node);
        void __fastcall ApplyBtnClick(TObject *Sender);
        void __fastcall UndoBtnClick(TObject *Sender);
        void __fastcall NewBtnClick(TObject *Sender);
        void __fastcall TreeView1Changing(TObject *Sender, TTreeNode *Node,
          bool &AllowChange);
        void __fastcall DelBtnClick(TObject *Sender);
        void __fastcall StartBtnClick(TObject *Sender);
        void __fastcall StopBtnClick(TObject *Sender);
        void __fastcall Splitter1Moved(TObject *Sender);
        void __fastcall FormHide(TObject *Sender);
        void __fastcall FormShow(TObject *Sender);
        void __fastcall RefreshBtnClick(TObject *Sender);
        void __fastcall PageControl1Changing(TObject *Sender,
          bool &AllowChange);
        void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
        void __fastcall N1Click(TObject *Sender);
        void __fastcall N2Click(TObject *Sender);
private:	// User declarations
TTreeNode * module_node;
AnsiString  __fastcall get_line_text(DWORD num);
TTreeNode * __fastcall find_line(DWORD num);
TTreeNode * __fastcall add_line_node(DWORD num);
TTreeNode * __fastcall add_line_node(LPVTDDEV_PARAM dp);
TTreeNode * __fastcall find_group(TTreeNode * line,DWORD grp_num);
TTreeNode * __fastcall find_group(DWORD line_num,DWORD grp_num);
TTreeNode * __fastcall add_group(TTreeNode * line,LPVTDDEV_GROUP grp);
protected:
void     __fastcall on_changed(TObject * Sender);
DWORD    __fastcall get_ch_mask();
void     __fastcall on_module_config_change(GKHANDLE mod,LPMODULE_CONFIG_DATA mcd);
void     __fastcall on_module_state(GKHANDLE mod,DWORD state) ;
LRESULT  __fastcall on_gkhandle_notify (GKHANDLE from,LPNOTIFY_CODE nc,DWORD mask);
void     __fastcall after_set_gkhandle();
void     __fastcall setup_mod_tree();
void     __fastcall show_error(AnsiString cmd, DWORD err);
void     __fastcall update_node_text(TTreeNode * node);
void     __fastcall on_device_change(DWORD code,DWORD dev,DWORD grp);
void     __fastcall on_modem_change(DWORD _code,LPVOID  data,DWORD sz);
void     __fastcall on_modem_line_state(LPMODEM_LINE_STATE ls);
void     __fastcall on_modem_change_number(LPMODEM_CHANGE_NUMBER mcn);
bool     __fastcall is_create_mode();


public:		// User declarations
        __fastcall TVTDSetupFrm(TComponent* Owner,HWND owner_hwnd,GKHANDLE handle = 0);
        __property bool CreateMode = {read = is_create_mode};
void __fastcall before_show(LPMODULE_WINDOW mw);
};
//---------------------------------------------------------------------------
extern PACKAGE TVTDSetupFrm *VTDSetupFrm;
//---------------------------------------------------------------------------

inline DWORD __fastcall get_dev_grp_number(TTreeNode * node,DWORD & dn,DWORD& gn)
{
 DWORD dw_data = 0;
 if(node)
 {
  dw_data = (DWORD)node->Data;
  dn = LOWORD(dw_data);
  gn = HIWORD(dw_data);
 }
 return dw_data;
}

inline void __fastcall set_dev_grp_number(TTreeNode * node,DWORD dn,DWORD gn = -1)
{
 node->Data = (LPVOID)MAKELONG(dn,gn);
}

#endif
