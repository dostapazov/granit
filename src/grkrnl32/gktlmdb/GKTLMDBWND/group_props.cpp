//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "group_props.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TBaseProps *BaseProps;
//---------------------------------------------------------------------------
__fastcall TBaseProps::TBaseProps(TComponent* Owner)
        : TFrame(Owner)
{
}
//---------------------------------------------------------------------------
