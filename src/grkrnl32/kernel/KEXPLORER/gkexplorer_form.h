//---------------------------------------------------------------------------

#ifndef gkexplorer_formH
#define gkexplorer_formH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "gkmodule_form.h"
#include <ComCtrls.hpp>
#include <ImgList.hpp>
#include <Menus.hpp>
#include <ToolWin.hpp>
#include <Dialogs.hpp>
#include <ExtCtrls.hpp>
#include "module_info_frame.h"
#include "moddet_frm.h"
#include <ActnList.hpp>
#include <System.ImageList.hpp>
#include "about_common_frame.h"


//---------------------------------------------------------------------------
class TGKExplorerFrm : public TGKModuleForm
{
__published:	// IDE-managed Components
        TStatusBar *StatusBar1;
        TMainMenu *MainMenu1;
        TMenuItem *N1;
        TMenuItem *N7;
        TMenuItem *MiClose;
        TImageList *ImageList1;
        TToolBar *ToolBar1;
        TOpenDialog *OpenDialog1;
        TTreeView *ModulesTree;
        TSplitter *Splitter1;
        TToolButton *TbClose;
        TToolButton *ToolButton2;
        TToolButton *TbAddModule;
        TToolButton *TbDeleteModule;
        TToolButton *ToolButton5;
        TToolButton *TbStartModule;
        TToolButton *TbStopModule;
        TMenuItem *N3;
        TMenuItem *MiHelp;
        TMenuItem *MiAbout;
        TToolButton *TbSetupModule;
        TToolButton *TbViewModule;
        TToolButton *ToolButton4;
        TMenuItem *N6;
        TPopupMenu *PopupMenu1;
        TMenuItem *MiAddModule;
        TMenuItem *MiDeleteModule;
        TMenuItem *MiReplace;
        TMenuItem *N11;
        TMenuItem *MiStartModule;
        TMenuItem *MiStopModule;
        TMenuItem *N12;
        TMenuItem *MiSetupModule;
        TMenuItem *MiViewModule;
        TMenuItem *N8;
        TImageList *ModuleIcons;
        TPageControl *PageControl1;
        TTabSheet *TabSheet1;
        TTabSheet *TabSheet2;
        TModuleInfoFrame *ModuleInfoFrame1;
        TLabel *Label1;
	TMenuItem *miLicenseSeparator;
	TMenuItem *miGenTrialLicense;
	TMenuItem *miRegisterModule;
	TMenuItem *miGenPrelicenseData;
	TAboutCommonFrame *AboutCommonFrame1;
	TMenuItem *N2;
	TMenuItem *miSaveRegistrySettings;
	TSaveDialog *SaveDialog1;
        void __fastcall MiCloseClick(TObject *Sender);
        void __fastcall MiAddModuleClick(TObject *Sender);
        void __fastcall ModulesTreeChange(TObject *Sender,
          TTreeNode *Node);
        void __fastcall MiStartModuleClick(TObject *Sender);
        void __fastcall MiStopModuleClick(TObject *Sender);
        void __fastcall MiSetupModuleClick(TObject *Sender);
        void __fastcall MiViewModuleClick(TObject *Sender);
        void __fastcall ModulesTreeChanging(TObject *Sender,
          TTreeNode *Node, bool &AllowChange);
        void __fastcall ModulesTreeDeletion(TObject *Sender,
          TTreeNode *Node);
        void __fastcall MiDeleteModuleClick(TObject *Sender);
        void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
        void __fastcall FormHide(TObject *Sender);
        void __fastcall ModuleInfoFrame1Timer1Timer(TObject *Sender);
        void __fastcall Splitter1Moved(TObject *Sender);
        void __fastcall MiAboutClick(TObject *Sender);
        void __fastcall N6Click(TObject *Sender);
        void __fastcall ModulesTreeDblClick(TObject *Sender);
        void __fastcall N1Click(TObject *Sender);
        void __fastcall ModulesTreeKeyPress(TObject *Sender, char &Key);
	void __fastcall miGenTrialLicenseClick(TObject *Sender);
	void __fastcall miRegisterModuleClick(TObject *Sender);
	void __fastcall miGenPrelicenseDataClick(TObject *Sender);
	void __fastcall miSaveRegistrySettingsClick(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
private:	// User declarations

TTreeNode  * added_module_node;
LRESULT    __fastcall on_gkhandle_notify(GKHANDLE from,LPNOTIFY_CODE nc,DWORD mask);
        void __fastcall SetModuleStateText(AnsiString value);
        AnsiString __fastcall GetModuleStateText();
void    __fastcall begin_gk_command(TCHAR * text);
void    __fastcall end_gk_command(GKHANDLE mod,LRESULT ret,DWORD error);
        void __fastcall SetCommandStatus(AnsiString value);
        AnsiString __fastcall GetCommandStatus();

protected:
void __fastcall after_set_gkhandle();
void __fastcall update_commands(GKHANDLE h);
void __fastcall update_status_bar(GKHANDLE h);
TTreeNode*  __fastcall enum_modules(TTreeNode * parent,GKHANDLE h);
TTreeNode*  __fastcall add_handle_node(TTreeNode * parent,GKHANDLE h);
TTreeNode*  __fastcall find_module_node(GKHANDLE,TTreeNode * root);
static GKHANDLE __fastcall GetGKHandle(TTreeNode * node);
DWORD   __fastcall OnModuleWindowMessage(UINT msg,LPVOID wnd_handle,GKHANDLE h);
void    __fastcall show_module_wnd(GKHANDLE handle,DWORD wnd_type,LPVOID param);
void    __fastcall on_module_state(GKHANDLE mod,DWORD state) ;
void    __fastcall on_module_config_change(GKHANDLE mod,LPMODULE_CONFIG_DATA mcd);
void    __fastcall on_added_module  (GKHANDLE owner,GKHANDLE mod,DWORD mod_type);
void    __fastcall on_removed_module(GKHANDLE owner,GKHANDLE mod,DWORD mod_type);
void    __fastcall on_renamed_module(GKHANDLE mod_handle);
        __property AnsiString CommandStatus  = { read=GetCommandStatus, write=SetCommandStatus };
void    __fastcall BeforeDestruction(void);
virtual void     __fastcall setup_form_sizes();

public:		// User declarations
        __fastcall TGKExplorerFrm(TComponent* Owner,HWND own_wnd,GKHANDLE h);
        __property AnsiString ModuleStateText  = { read=GetModuleStateText, write=SetModuleStateText };
int     __fastcall ShowModal();
TModuleDefine * mod_def;

};
//---------------------------------------------------------------------------
extern PACKAGE TGKExplorerFrm *GKExplorerFrm;
//---------------------------------------------------------------------------


#endif


