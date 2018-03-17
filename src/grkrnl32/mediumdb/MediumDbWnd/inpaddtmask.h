//----------------------------------------------------------------------------
#ifndef inpaddtmaskH
#define inpaddtmaskH
//----------------------------------------------------------------------------
#include <SysUtils.hpp>
#include <Windows.hpp>
#include <Messages.hpp>
#include <Classes.hpp>
#include <Graphics.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <ExtCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
//----------------------------------------------------------------------------
class TInputAddrMask : public TForm
{
__published:
        TEdit *Edit1;
        TLabel *Label1;
        TBitBtn *BitBtn1;
        TBitBtn *BitBtn2;
private:
public:
	virtual __fastcall TInputAddrMask(TComponent *Owner);
	virtual __fastcall TInputAddrMask(TComponent *Owner,UnicodeString capt,UnicodeString prompt);
};
//----------------------------------------------------------------------------
extern TInputAddrMask *InputAddrMask;
//----------------------------------------------------------------------------
#endif
