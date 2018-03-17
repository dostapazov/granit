//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "GKEvent_frm.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "gkmodule_form"
#pragma resource "*.dfm"
TGKEventFrm *GKEventFrm;
//---------------------------------------------------------------------------
__fastcall TGKEventFrm::TGKEventFrm(TComponent* Owner,HWND owner_hwnd,GKHANDLE handle)
        : TGKModuleForm(Owner,owner_hwnd,handle)
{
}
//---------------------------------------------------------------------------

void __fastcall TGKEventFrm::SetUp(TListItem * item, LPVOID func)
{
  LType->Caption=item->Caption;
  LDate->Caption=item->SubItems->Strings[0];
  LTime->Caption=item->SubItems->Strings[1];
  LSource->Caption=item->SubItems->Strings[2];
  MDesc->Clear();
  MData->Clear();
  for (int i=3; i<item->SubItems->Count; i++)
    MDesc->Lines->Add(item->SubItems->Strings[i]);
  if (item->Data)
  {
    LData->Enabled=true;
    MData->Enabled=true;
    if (func && !IsBadCodePtr((FARPROC)func))
    {
      const DWORD sz=32656;
      wchar_t wstr[sz]=L"";
      FillMemory(wstr,sz*sizeof(wchar_t),1);
      int len=((ReportDataToStrW)func)(item->Data,wstr,sz);
      if (len<0)
      {
        wchar_t *wstr1=new wchar_t[-len];
        ((ReportDataToStrW)func)(item->Data,wstr1,(DWORD)-len);
        if (wstr1[len-1])
          MData->Lines->Text=AnsiString((char *)wstr1);
        else
          MData->Lines->Text=AnsiString(wstr1);
        delete [] wstr1;
      }
      else if (NULL==len)
        MData->Lines->Text=ToStringByte(item->Data);
      else
      {
        if (wstr[len-1])
          MData->Lines->Text=AnsiString((char *)wstr);
        else
          MData->Lines->Text=AnsiString(wstr);
      }
    }
    else
      MData->Lines->Text=ToStringByte(item->Data);
  }
  else
  {
    LData->Enabled=false;
    MData->Enabled=false;
  }
}

void __fastcall TGKEventFrm::BtCloseClick(TObject *Sender)
{
  Close();
}
//---------------------------------------------------------------------------


void __fastcall TGKEventFrm::BtUpClick(TObject *Sender)
{
  if (TListView *lv=dynamic_cast<TListView *>((dynamic_cast<TPageControl *>(Owner))->ActivePage->Controls[0]))
    if (lv->ItemIndex>0)
      lv->ItemIndex=lv->ItemIndex-1;
}
//---------------------------------------------------------------------------

void __fastcall TGKEventFrm::BtDownClick(TObject *Sender)
{
  if (TListView *lv=dynamic_cast<TListView *>((dynamic_cast<TPageControl *>(Owner))->ActivePage->Controls[0]))
    if (lv->ItemIndex<(lv->Items->Count-1))
      lv->ItemIndex=lv->ItemIndex+1;
}
//---------------------------------------------------------------------------

AnsiString __fastcall TGKEventFrm::ToStringByte(LPVOID Data)
{
  AnsiString s="",s1,s2;
  if (!Data) return s;
  DWORD sz=*reinterpret_cast<DWORD *>(Data);
  BYTE *PData=reinterpret_cast<BYTE *>((BYTE *)Data+sizeof(sz));
  BYTE b;
  DWORD i=0;
  while (i<sz)
  {
    s1.sprintf("%04X:",i);
    s2="";
    for (DWORD j=0; j<8; j++)
    {
      if (i==sz) break;
      b=*(PData+i);
      s1.cat_sprintf(" %02X",b);
      s2+=(b<32)?'.':(char)b;
      i++;
    }
    while (s1.Length()<32) s1+=" ";
    s+=s1+s2+"\r\n";
  }
  return s;
}
