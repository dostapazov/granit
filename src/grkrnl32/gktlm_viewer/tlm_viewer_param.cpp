//---------------------------------------------------------------------------

#pragma hdrstop
#include <vcl.h>

#include <browse_folder.hpp>
#include "tlm_viewer_param.h"


//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TTlmViewerParams *TlmViewerParams;
//---------------------------------------------------------------------------
__fastcall TTlmViewerParams::TTlmViewerParams(TComponent* Owner)
	: TForm(Owner)
{
   Fon_request_kadr_list = NULL;
   Fcolors_changed       = 0;
   Ffont_changed         = false;
   MonitorSelect->Items->Clear();
   DISPLAY_DEVICEW dd;
   dd.cb  = sizeof(dd);
   for(int i = 0;i<Screen->MonitorCount;i++)
   {
     Forms::TMonitor * mon = Screen->Monitors[i];
     EnumDisplayDevicesW(NULL,i,&dd,0);
     UnicodeString str;
     str.printf(L"%s (%d x %d)",dd.DeviceString ,mon->Width,mon->Height);
     MonitorSelect->Items->Add(str);
   }
}
//---------------------------------------------------------------------------

void __fastcall TTlmViewerParams::WndProc(Winapi::Messages::TMessage &Message)
{
  switch(Message.Msg)
  {
   case WNDCM_ADD_KADR:
        Message.Result = 0;
        kadr_list_add((LPOTDM_PROTO_KADR)Message.LParam,(bool)Message.WParam);
   break;

   default: TForm::WndProc(Message);
   break;
  }
}

void __fastcall TTlmViewerParams::kadr_list_add(LPOTDM_PROTO_KADR pk,bool ordered)
{
  wchar_t * ptr    = pk->names;
  UnicodeString   ksname = ptr;
  ptr+=ksname.Length()+1;
  UnicodeString kname = ptr;
  kadr_list_add( pk->kadr.kadr_id,ksname,kname,ordered);
}

void __fastcall TTlmViewerParams::kadr_list_add(DWORD kadr_id,UnicodeString sname,UnicodeString name,bool ordered)
{
  TListItem * item = this->KadrList->Items->Add();
  item->Caption    = kadr_id;
  item->Data       = (LPVOID) kadr_id;
  item->SubItems->Add(sname);
  item->SubItems->Add(name );
  item->Checked = ordered;
}



void __fastcall TTlmViewerParams::bSelViewerClick(TObject *Sender)
{
  SelViewerDlg->FileName = ExportViewer->Text;
  if(SelViewerDlg->Execute(Handle))
    {
      ExportViewer->Text = SelViewerDlg->FileName;
    }
}
//---------------------------------------------------------------------------


void __fastcall TTlmViewerParams::bSelDataFolderClick(TObject *Sender)
{
 KeWin::TFolderBrowser fb(Handle);
 wchar_t buf[2048];
 wcsncpy(buf,DataFolder->Text.c_str(),KERTL_ARRAY_COUNT(buf));
 *buf = 0;
 if(fb.SelectFolder(buf,KERTL_ARRAY_COUNT(buf),(wchar_t*)L"Выбор папки данных",true,false))
 {
    wcscat(buf,L"\\");
    DataFolder->Text = buf;
 }
}
//---------------------------------------------------------------------------

void __fastcall TTlmViewerParams::bSelExporFolderClick(TObject *Sender)
{
 KeWin::TFolderBrowser fb(Handle);
 wchar_t buf[2048];
 wcsncpy(buf,ExportFolder->Text.c_str(),KERTL_ARRAY_COUNT(buf));
 *buf = 0;
 if(fb.SelectFolder(buf,KERTL_ARRAY_COUNT(buf),(wchar_t*)L"Выбор папки экспорта",true,false))
    {
     wcscat(buf,L"\\");
     ExportFolder->Text = buf;
    }

}
//---------------------------------------------------------------------------

void __fastcall TTlmViewerParams::SourceClick(TObject *Sender)
{
  //Отправить зарос на перечисление кадров
  KadrList->Clear();
  if(on_request_kadr_list)
     {
      modem_addr ma(Source->ItemIndex ? -1:0);
      on_request_kadr_list(ma);
     }
}
//---------------------------------------------------------------------------


void __fastcall TTlmViewerParams::bOkClick(TObject *Sender)
{
  ModalResult = mrOk;
  Close();
}
//---------------------------------------------------------------------------

void __fastcall TTlmViewerParams::bCancelClick(TObject *Sender)
{
  ModalResult = mrCancel;
  Close();

}
//---------------------------------------------------------------------------




void __fastcall TTlmViewerParams::LayoutsChange(TObject *Sender)
{
   bReadLayout->Enabled = !Layouts->Text.IsEmpty() ? true : false;
}
//---------------------------------------------------------------------------

bool __fastcall TTlmViewerParams::select_sound(UnicodeString & result)
{
    if(SoundOpenDlg->Execute(Handle))
    {
       result = SoundOpenDlg->FileName;
       return true;
    }
  return false;
}



void __fastcall TTlmViewerParams::bSelDisparitySndClick(TObject *Sender)
{
  UnicodeString str;
  if(select_sound(str))
     snd_disparity->Text = str;
}
//---------------------------------------------------------------------------

void __fastcall TTlmViewerParams::bSelBackSndClick(TObject *Sender)
{
  UnicodeString str;
  if(select_sound(str))
     snd_back->Text = str;
}
//---------------------------------------------------------------------------

template <typename action_t>
void __fastcall for_each_list_items(TListView * lv,action_t act)
{
  lv->Items->BeginUpdate();
  for(int i = 0;i<lv->Items->Count;i++)
     {
      TListItem * item = lv->Items->Item[i];
      act(item);
     }
  lv->Items->EndUpdate();
}

struct set_action
{
    void operator ()(TListItem * item){item->Checked = true;}
};

struct reset_action
{
    void operator ()(TListItem * item){item->Checked = false;}
};

struct inv_action
{
    void operator ()(TListItem * item){item->Checked = !item->Checked;}
};


void __fastcall TTlmViewerParams::SpeedButton1Click(TObject *Sender)
{
  for_each_list_items(KadrList,set_action());
}
//---------------------------------------------------------------------------

void __fastcall TTlmViewerParams::SpeedButton2Click(TObject *Sender)
{
  for_each_list_items(KadrList,reset_action());
}
//---------------------------------------------------------------------------

void __fastcall TTlmViewerParams::SpeedButton3Click(TObject *Sender)
{
  for_each_list_items(KadrList,inv_action());
}
//---------------------------------------------------------------------------



void __fastcall TTlmViewerParams::bApplyClick(TObject *Sender)
{
   ModalResult = mrOk;
   if(OnClose)
   {
      TCloseAction ca = caNone;
      OnClose(this,ca);
   }

}
//---------------------------------------------------------------------------

void __fastcall TTlmViewerParams::ReadyColorsEditDrawButton(TObject *Sender, int Index,
          TCanvas *Canvas,  Types::TRect &Rect, TButtonDrawState State)
{
  UnicodeString str = ReadyColorsEdit->Items->Items[Index]->Caption;
   System::Types::TSize sz = Canvas->TextExtent(str);
   Canvas->Rectangle(Rect);
   OffsetRect(Rect,1,1);
   InflateRect(Rect,-4,-4);
   Canvas->TextRect (Rect,str);
   System::Types::TRect color_box = Rect;
   color_box.Right-=8;
   color_box.Left = color_box.Right - 32;
   Canvas->Rectangle(color_box);
   TColor save_color = Canvas->Brush->Color;
   Canvas->Brush->Color = get_ready_color(Index);
   Canvas->FillRect(color_box);
   Canvas->Brush->Color = save_color;


}
//---------------------------------------------------------------------------
 void   __fastcall TTlmViewerParams::set_ready_color(int idx, TColor color)
 {
   switch(idx)
   {
     case 0: Fready_colors.bkgnd           = color ;
     break;
     case 1: Fready_colors.bkgnd_stable    = color ;
     break;
     case 2: Fready_colors.name_bad        = color ;
     break;
     case 3: Fready_colors.name_good       = color ;
     break;
     case 4: Fready_colors.tutr            = color ;
     break;
     case 5: Fready_colors.state           = color ;
     break;
     case 6: Fready_colors.state_danger    = color ;
     break;
     case 7: Fready_colors.state_alarm     = color ;
     break;
     case 8: Fready_colors.border          = color ;
     break;
     case 9: Fready_colors.border_selected = color ;
     break;
   }

 }
//---------------------------------------------------------------------------

 TColor __fastcall TTlmViewerParams::get_ready_color(int idx)
 {
   TColor color = clWhite;
   switch(idx)
   {
     case 0: color = (TColor)Fready_colors.bkgnd;
     break;
     case 1: color = (TColor)Fready_colors.bkgnd_stable;
     break;
     case 2: color = (TColor)Fready_colors.name_bad;
     break;
     case 3: color = (TColor)Fready_colors.name_good;
     break;
     case 4: color = (TColor)Fready_colors.tutr;
     break;
     case 5: color = (TColor)Fready_colors.state;
     break;
     case 6: color = (TColor)Fready_colors.state_danger;
     break;
     case 7: color = (TColor)Fready_colors.state_alarm;
     break;
     case 8: color = (TColor)Fready_colors.border;
     break;
     case 9: color = (TColor)Fready_colors.border_selected;
     break;
   }
   return  color;
 }


void __fastcall TTlmViewerParams::ReadyColorsEditButtonClicked(TObject *Sender, int Index)

{
   ColorDialog1->Color = get_ready_color(Index);
   if(ColorDialog1->Execute(Handle))
     {
       set_ready_color(Index,ColorDialog1->Color );
       ReadyColorsEdit->Refresh();
       ++Fcolors_changed;
     }
}
//---------------------------------------------------------------------------


void __fastcall TTlmViewerParams::ReadyFontDblClick(TObject *Sender)
{
  FontDialog1->Font = ReadyFont->Font;
  if(FontDialog1->Execute(Handle))
    {
      ReadyFont->Font = FontDialog1->Font;
      Ffont_changed = true;
    }
}
//---------------------------------------------------------------------------



 void   __fastcall TTlmViewerParams::set_modem_addr (modem_addr ma)
 {
   switch(ma.addr)
   {
    case (DWORD) 0 : Source->ItemIndex = 0; break;
    case (DWORD)-1 : Source->ItemIndex = 1; break;
    default:
    {
     UnicodeString str;
     str.printf(L"%u.%u",(DWORD)ma.modem,(DWORD)ma.line);
     Source->Text = str;
    }
    break;
   }
 }

 modem_addr  __fastcall TTlmViewerParams::get_modem_addr ()
 {
  modem_addr ret;
  switch(Source->ItemIndex)
  {
   case 0: ret.addr =  0;break;
   case 1: ret.addr = -1;break;
   default:
   {
    ret.addr = 0;
    str2addr(Source->Text.c_str(),&ret.modem,2,L'.');
   }
   break;
  }
  return ret;
 }

  void        __fastcall TTlmViewerParams::set_parameters (Tviewer_parameters & params)
  {
   //params->Icon->Assign(Icon);

   DataFolder->Text     = params.data_dir.c_str();
   ExportFolder->Text   = params.export_dir.c_str();
   ExportViewer->Text   = params.export_viewer.c_str();
   this->set_modem_addr(params.modem_source);


   cbDoubleBuffered->Checked = params.enable_double_buffered ;
   HistoryTimeBefore->Text = params.history_time_before;
   HistoryTimeAfter ->Text = params.history_time_after ;


   READY_COLORS & rdy_colors = params.ready_colors;
   set_ready_colors(rdy_colors);
   HFONT font = CreateFontIndirect(&params.ready_font);
   if(font)
      ReadyFont->Font->Handle =  font ;

   MonitorSelect->ItemIndex    = params.main_form_monitor;
   ReadySpacer->Position       = params.ready_spacer;


//   bSaveCurrentLayout->OnClick = save_layout;
//   bReadLayout->OnClick        = open_layout;

//   Layouts->OnDropDown         = enum_layouts;
   Layouts->Text               = params.auto_open_layout.c_str();
   LayoutsChange(this);

   HotKeyClearAll->HotKey      = params.hot_key_clear_all;
   HotKeyReceiptAll->HotKey    = params.hot_key_receipt_all;
   OrderPeriod->Text           = params.order_period;
   StableReadyTimer->Text      = params.stable_ready_timer;
   snd_disparity->Text         = params.snd_disparity.c_str();
   snd_back->Text              = params.snd_back.c_str();

//   KadrList->Items->BeginUpdate();
//
//   TGKTlmViewer::locker l(viewer_module->get_mutex());
//   Tkadrs & kadrs_ordered = viewer_module->get_kadrs_ordered();
//   Tkadrs::iterator ptr = kadrs_ordered.begin();
//   Tkadrs::iterator end = kadrs_ordered.end  ();
//   while(ptr<end)
//    {
//      UnicodeString sname;
//      UnicodeString name;
//      TTlmViewerForm::get_text((*ptr)->sname_id,sname);
//      TTlmViewerForm::get_text((*ptr)->name_id , name);
//      params->kadr_list_add((*ptr)->kadr_id,sname,name,true);
//      ++ptr;
//    }
//    params->KadrList->Items->EndUpdate();
//
//    params->Visible = true;
//
//    modem_addr ma(viewer_module->get_external_source() ? -1:0);
//    request_kadr_list(ma);
//
  }

 void        __fastcall TTlmViewerParams::get_parameters (Tviewer_parameters & params)
 {

}





