//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "gk_about.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TExplAbout *ExplAbout;
//---------------------------------------------------------------------------
__fastcall TExplAbout::TExplAbout(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TExplAbout::FormShow(TObject *Sender)
{
  Timer1->Interval = 10;
  Timer1->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TExplAbout::Timer1Timer(TObject *Sender)
{
  int val = AlphaBlendValue;
  int inc = 1;
  if(val<100)
     inc = 50;
  if(val<150)
     inc = 10;
  if(val<255)
    {
      val+=inc;
      AlphaBlendValue = val;
    }
    else
    {
     AlphaBlendValue = 255;
     AlphaBlend = false;
     Timer1->Enabled = false;
    }
}
//---------------------------------------------------------------------------

void __fastcall TExplAbout::AfterConstruction(void)
{
  TForm::AfterConstruction();
  HRGN region = CreateRoundRectRgn(0,0,Width,Height,30,30);
  SetWindowRgn(Handle,region,FALSE);


}

void     __fastcall TExplAbout::WndProc(Messages::TMessage &Message)
{
 if(Message.Msg == WM_NCHITTEST)
     Message.Result =  HTCAPTION;
    else
    TForm::WndProc(Message); 

}
void __fastcall TExplAbout::FormKeyPress(TObject *Sender, char &Key)
{
  if(Key == VK_ESCAPE)
  {
   ModalResult = mrCancel;
  }
}
//---------------------------------------------------------------------------
