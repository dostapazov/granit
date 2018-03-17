//----------------------------------------------------------------------------
#pragma hdrstop
#include <vcl.h>

#include "inpaddtmask.h"
//----------------------------------------------------------------------------
#pragma resource "*.dfm"
TInputAddrMask *InputAddrMask;
//----------------------------------------------------------------------------
__fastcall TInputAddrMask::TInputAddrMask(TComponent *Owner)
	: TForm(Owner)
{
}
//----------------------------------------------------------------------------

 __fastcall TInputAddrMask::TInputAddrMask(TComponent *Owner,UnicodeString capt,UnicodeString prompt)
	: TForm(Owner)
{
 this->Caption = capt;
 this->Label1->Caption = prompt;
}

