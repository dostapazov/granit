//---------------------------------------------------------------------------
#pragma hdrstop

#include <vcl.h>
#include "paint_props.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TPaintProps *PaintProps;
//---------------------------------------------------------------------------
__fastcall TPaintProps::TPaintProps(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TPaintProps::FormDeactivate(TObject *Sender)
{
   Visible = false;
}
//---------------------------------------------------------------------------
