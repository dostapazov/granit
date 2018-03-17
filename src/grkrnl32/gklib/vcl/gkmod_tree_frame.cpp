//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "gkmod_tree_frame.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "gkmod_frame"
#pragma resource "*.dfm"
TGKModuleTreeFrame *GKModuleTreeFrame;
//---------------------------------------------------------------------------
__fastcall TGKModuleTreeFrame::TGKModuleTreeFrame(TComponent* Owner)
        : TGKModuleFrame(Owner)
{
}
//---------------------------------------------------------------------------

void     __fastcall  TGKModuleTreeFrame::after_set_gkhandle()
{
 ModulesTree->Items->Clear();
 on_enum_handle(0,mod_handle,0);
 enum_modules(this->mod_handle,0);
}

bool     __fastcall  TGKModuleTreeFrame::on_enum_handle(GKHANDLE parent,GKHANDLE h,LPVOID data)
{
 wchar_t text[MAX_PATH];
 MODULE_INFO mi;
 mi.dw_size = sizeof(mi);
 mi.mi_mask = MI_FL_MODULE_NAME;
 mi.module_name   = text;
 mi.module_name_cbsz = sizeof(text)/sizeof(wchar_t);
 if(call_gkhandle(h,MCMD_GETMODULE_INFO,LPARAM(&mi),0))
    lstrcpyW(text,L"unnamed module");
 TTreeNode * node = ModulesTree->Items->AddChild((TTreeNode*)data,text);
 node->Data = h;
 int mc = (int) call_gkhandle(h,MCMD_GETMODULES_COUNT,0,0);
 if(mc>0)
    enum_modules(h,node);
 return true;
}

