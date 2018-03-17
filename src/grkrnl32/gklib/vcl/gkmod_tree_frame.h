//---------------------------------------------------------------------------

#ifndef gkmod_tree_frameH
#define gkmod_tree_frameH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "gkmod_frame.h"
#include <ComCtrls.hpp>
//---------------------------------------------------------------------------
class TGKModuleTreeFrame : public TGKModuleFrame
{
__published:	// IDE-managed Components
        TTreeView *ModulesTree;
private:	// User declarations
void     __fastcall  after_set_gkhandle() ;
bool     __fastcall  on_enum_handle(GKHANDLE parent,GKHANDLE h,LPVOID data) ;
public:		// User declarations
        __fastcall TGKModuleTreeFrame(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TGKModuleTreeFrame *GKModuleTreeFrame;
//---------------------------------------------------------------------------
#endif
