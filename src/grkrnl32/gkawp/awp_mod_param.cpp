//---------------------------------------------------------------------------

#pragma hdrstop
#include "awp_mod_param.h"
#include <browse_folder.hpp>
#include "ready_painter.h"


//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TAwpModuleParams *AwpModuleParams;
//---------------------------------------------------------------------------
__fastcall TAwpModuleParams::TAwpModuleParams(TComponent* Owner)
	: TForm(Owner),module(NULL)
{
}

__fastcall TAwpModuleParams::TAwpModuleParams(TComponent* Owner,TGkAwpModule      * _mod)
	: TForm(Owner),module(_mod)
{
   Fenum_layouts = NULL;
   Fopen_layout  = NULL;
   Fsave_layout  = NULL;
   Fon_request_kadr_list = NULL;
   Fcolors_changed       = 0;
   Ffont_changed         = false;
   Forders_changed       = false;
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
   PageControl1->ActivePageIndex = 0;
}
//---------------------------------------------------------------------------

void __fastcall TAwpModuleParams::WndProc(Winapi::Messages::TMessage &Message)
{
  switch(Message.Msg)
  {
   case WNDCM_HANDLE_ENUM_KADRS:
        Message.Result     = 0;
        handle_enum_kadrs  ((LPMODEM_ADDR)Message.WParam,(LPOTDM_PROTO_STATE)Message.LParam );
   break;

   default: TForm::WndProc(Message);
   break;
  }
}

void __fastcall TAwpModuleParams::handle_enum_kadrs(LPMODEM_ADDR ma, LPOTDM_PROTO_STATE ps)
{
 if(!ps->is_error)
 {
  modem_addr addr = this->get_modem_addr();
  if(((addr.sta != ma->sta && addr.sta == BYTE(-1)) || (addr.modem != ma->modem && addr.modem == BYTE(-1)) || (addr.line != ma->line  && addr.line ==  BYTE(-1) ))
      || (addr.addr == 0 && ma->sta ==0 && ma->modem == 0)
    )
      {
       addr = *ma;
       set_modem_addr(addr);
      }
  if(addr.addr == ma->addr )
     kadr_list_add(ma,ps->p_kadr,ps->data_size);
  if(ps->end_data)
     Forders_changed = false;
 }
}

void   __fastcall TAwpModuleParams::kadr_list_add   (LPMODEM_ADDR ma,LPOTDM_PROTO_KADR pkadr,DWORD data_size)
{
  UnicodeString sname,lname;
  TGkAwpModule::get_kadr_names(pkadr,sname,lname);
  TListItem * item = this->KadrList->Items->Add();
  LPBYTE ptr = new BYTE[data_size];
  memcpy(ptr,pkadr,data_size);
  item->Caption    = pkadr->kadr.kadr_id;
  item->SubItems->Add(sname);
  item->SubItems->Add(lname );
  if(this->module) item->Checked = module->is_kadr_ordered(ma,pkadr->kadr.kadr_id);
  item->Data       = (LPVOID) ptr;

}

LPOTDM_PROTO_KADR __fastcall TAwpModuleParams::get_kadr      (int idx,bool &ordered)
{
  if(idx<this->KadrList->Items->Count)
     {
       TListItem * item = KadrList->Items->Item[idx];
       ordered = item->Checked;
       return (LPOTDM_PROTO_KADR)item->Data;
     }
   return NULL;
}



void __fastcall TAwpModuleParams::bSelViewerClick(TObject *Sender)
{
  SelViewerDlg->FileName = ExportViewer->Text;
  if(SelViewerDlg->Execute(Handle))
    {
      ExportViewer->Text = SelViewerDlg->FileName;
    }
}
//---------------------------------------------------------------------------


void __fastcall TAwpModuleParams::bSelDataFolderClick(TObject *Sender)
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

void __fastcall TAwpModuleParams::bSelExporFolderClick(TObject *Sender)
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

void __fastcall TAwpModuleParams::SourceClick(TObject *Sender)
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


void __fastcall TAwpModuleParams::bOkClick(TObject *Sender)
{
  ModalResult = mrOk;
  Close();
}
//---------------------------------------------------------------------------

void __fastcall TAwpModuleParams::bCancelClick(TObject *Sender)
{
  ModalResult = mrCancel;
  Close();

}
//---------------------------------------------------------------------------




void __fastcall TAwpModuleParams::LayoutsChange(TObject *Sender)
{
   bReadLayout->Enabled = !Layouts->Text.IsEmpty() ? true : false;
}
//---------------------------------------------------------------------------

bool __fastcall TAwpModuleParams::select_sound(UnicodeString & result)
{
    if(SoundOpenDlg->Execute(Handle))
    {
       result = SoundOpenDlg->FileName;
       return true;
    }
  return false;
}



void __fastcall TAwpModuleParams::bSelDisparitySndClick(TObject *Sender)
{
  UnicodeString str;
  if(select_sound(str))
     {
      if(Sender == bSelDisparitySnd)
         snd_disparity->Text = str;
      if(Sender == bSelBackSnd)
         snd_back->Text = str;
      if(Sender == bSelAlarmSnd)
         snd_alarm->Text = str;
      if(Sender == bSelDangerSnd)
         snd_danger->Text = str;
     }
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


void __fastcall TAwpModuleParams::bSelAllClick(TObject *Sender)
{
  for_each_list_items(KadrList,set_action());
}
//---------------------------------------------------------------------------

void __fastcall TAwpModuleParams::bUnselAllClick(TObject *Sender)
{
  for_each_list_items(KadrList,reset_action());
}
//---------------------------------------------------------------------------

void __fastcall TAwpModuleParams::bSelInvertClick(TObject *Sender)
{
  for_each_list_items(KadrList,inv_action());
}
//---------------------------------------------------------------------------



void __fastcall TAwpModuleParams::bApplyClick(TObject *Sender)
{
   ModalResult = mrOk;
   if(OnClose)
   {
      TCloseAction ca = caNone;
      OnClose(this,ca);
   }
   ModalResult = mrNone;
}
//---------------------------------------------------------------------------

void __fastcall TAwpModuleParams::ReadyColorsEditDrawButton(TObject *Sender, int Index,
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
 void   __fastcall TAwpModuleParams::set_ready_color(int idx, TColor color)
 {
   switch(idx)
   {
     case 0 : Fready_colors.bkgnd           = color ;
     break;
     case 1 : Fready_colors.bkgnd_stable    = color ;
     break;
     case 2 : Fready_colors.name_bad        = color ;
     break;
     case 3 : Fready_colors.name_good       = color ;
     break;
     case 4 : Fready_colors.tutr            = color ;
     break;
     case 5 : Fready_colors.state_off       = color ;
     break;
     case 6 : Fready_colors.state_on        = color ;
     break;
     case 7 : Fready_colors.state_danger    = color ;
     break;
     case 8 : Fready_colors.state_alarm     = color ;
     break;
     case 9 : Fready_colors.border          = color ;
     break;
     case 10: Fready_colors.border_selected = color ;
     break;
   }

 }
//---------------------------------------------------------------------------

 TColor __fastcall TAwpModuleParams::get_ready_color(int idx)
 {
   TColor color = clWhite;
   switch(idx)
   {
     case 0 : color = (TColor)Fready_colors.bkgnd;
     break;
     case 1 : color = (TColor)Fready_colors.bkgnd_stable;
     break;
     case 2 : color = (TColor)Fready_colors.name_bad;
     break;
     case 3 : color = (TColor)Fready_colors.name_good;
     break;
     case 4 : color = (TColor)Fready_colors.tutr;
     break;
     case 5 : color = (TColor)Fready_colors.state_off;
     break;
     case 6 : color = (TColor)Fready_colors.state_on;
     break;
     case 7 : color = (TColor)Fready_colors.state_danger;
     break;
     case 8 : color = (TColor)Fready_colors.state_alarm;
     break;
     case 9 : color = (TColor)Fready_colors.border;
     break;
     case 10: color = (TColor)Fready_colors.border_selected;
     break;
   }
   return  color;
 }


void __fastcall TAwpModuleParams::ReadyColorsEditButtonClicked(TObject *Sender, int Index)

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

 void   __fastcall TAwpModuleParams::set_modem_addr (modem_addr ma)
 {
   switch(ma.addr)
   {
    case (DWORD) 0 : Source->ItemIndex = 0; break;
    case (DWORD)-1 : Source->ItemIndex = 1; break;
    default:
    {
     UnicodeString str;
     str.printf(L"%u.%u",(DWORD)ma.modem,(DWORD)ma.line);
     Source->ItemIndex = -1;
     Source->Text = str;
    }
    break;
   }
 }

 modem_addr  __fastcall TAwpModuleParams::get_modem_addr ()
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
    ret.reserv1 = -1;
   }
   break;
  }
  return ret;
 }

  void        __fastcall TAwpModuleParams::set_parameters (TAWParameters & params)
  {
   DataFolder->Text     = params.data_folder;
   ExportFolder->Text   = params.comtrade_export_folder;
   ExportViewer->Text   = params.comtrade_viewer_app;
   set_modem_addr(params.modem_source);

   set_ready_colors(params.ready_colors);

   HFONT font;
   font = CreateFontIndirectW(&params.ready_font);
   if(font)  ReadyFont->Font->Handle =  font ;
   font = CreateFontIndirectW(&params.ready_state_font);
   if(font)  StateFont->Font->Handle =  font ;

   MonitorSelect->ItemIndex    = params.ready_monitor;
   ReadySpacer->Position       = params.ready_space;
   ready_eq_size->Checked      = params.ready_equal_size ? true : false;

   Layouts->Text               = params.layout_name;
   LayoutsChange(this);

   HotKeyClearAll->HotKey      = params.hot_key_clear_all;
   HotKeyReceiptAll->HotKey    = params.hot_key_receipt_all;
   OrderPeriod->Text           = params.recall_period;
   StableReadyTimer->Text      = params.ready_stable_timer;

   snd_disparity->Text         = params.sound_disparity;
   snd_back->Text              = params.sound_back;
   snd_alarm->Text             = params.sound_alarm;
   snd_danger->Text            = params.sound_danger;
   FlashPeriod->Text           = params.flash_period;
   bRefreshKadrClick(this);
  }

 void        __fastcall TAwpModuleParams::get_parameters (TAWParameters & params)
 {
   params.modem_source           = get_modem_addr          ();
   params.data_folder            = DataFolder->Text.Trim   ();
   params.comtrade_export_folder = ExportFolder->Text.Trim ();
   params.comtrade_viewer_app    = ExportViewer->Text.Trim ();
   params.hot_key_receipt_all    = HotKeyReceiptAll->HotKey;
   params.hot_key_clear_all      = HotKeyClearAll->HotKey;
   params.recall_period          = _wtoi(OrderPeriod->Text.c_str());
   params.layout_name            = Layouts->Text.Trim();
   params.sound_alarm            = snd_alarm->Text.Trim();
   params.sound_danger           = snd_danger->Text.Trim();
   params.sound_disparity        = snd_disparity->Text.Trim();
   params.sound_back             = snd_back->Text.Trim();
   params.ready_monitor          = MonitorSelect->ItemIndex;
   params.ready_space            = ReadySpacer->Position;
   params.ready_equal_size       = ready_eq_size->Checked;
   params.ready_stable_timer     = _wtoi(StableReadyTimer->Text.c_str()) ;
   params.ready_colors           = get_ready_colors();
   GetObjectW(ReadyFont->Font->Handle,sizeof(params.ready_font),&params.ready_font);
   GetObjectW(StateFont->Font->Handle,sizeof(params.ready_font),&params.ready_state_font);
   params.flash_period           = std::max(250,abs(_wtoi(FlashPeriod->Text.c_str())));
 }



void __fastcall TAwpModuleParams::bRefreshKadrClick(TObject *Sender)
{
  if(module  )
  {
   KadrList->Clear() ;
   Application->ProcessMessages();
   if(!module->is_running()) module->open_modemmer();
   modem_addr addr_to   = get_modem_addr();
   module->send_command(addr_to,FA_OTD_MEDIUM,true,true,OTDMPROTO_CMD_ENUM_KADRS,0,NULL,0);
   Forders_changed = false;
  }
}
//---------------------------------------------------------------------------


void __fastcall TAwpModuleParams::FormDestroy(TObject *Sender)
{
  if(module && !module->is_running() )
     module->close_modemmer();

}
//---------------------------------------------------------------------------
UnicodeString __fastcall TAwpModuleParams::get_ready_text(DWORD /*str_id*/)
{
  return UnicodeString(L"Готовность");
}

void __fastcall TAwpModuleParams::ReadyTestPaint(TObject *Sender)
{
   sk.ready_state    = -1;
   sk.ready_state &=~(rds_rc_active/*|rds_state_flashing/*|rds_name_flashing*/);
   sk.kadr.kadr_diag = 0;
   sk.kadr.kadr_id   = 1;

   ready_painter_t rp(ReadyFont->Font->Handle,StateFont->Font->Handle,&Fready_colors);
   rp.on_get_kadr  = get_single_kadr;
   rp.on_get_text  = get_ready_text;
   mdb_kadr_entry entry(1,1,1);
   entry.position.x = entry.position.y = 2;
   entry.size = rp.ready_calc_size(ReadyTest->Canvas->Handle,entry);
   entry.set_selected(true);
   rp.ready_paint(ReadyTest->Canvas->Handle,entry,false,Timer1->Tag&1,NULL);
}
//---------------------------------------------------------------------------

void __fastcall TAwpModuleParams::SpeedButton3Click(TObject *Sender)
{
  if(ReadyFont->Execute(Handle))
    {
      this->Ffont_changed = true;
    }
}
//---------------------------------------------------------------------------

void __fastcall TAwpModuleParams::SpeedButton4Click(TObject *Sender)
{
  if(StateFont->Execute(Handle))
    {
      Ffont_changed = true;
    }
}
//---------------------------------------------------------------------------

void __fastcall TAwpModuleParams::KadrListDeletion(TObject *Sender, TListItem *Item)

{
   if(Item->Data) delete [] (LPBYTE)Item->Data;
}
//---------------------------------------------------------------------------

void __fastcall TAwpModuleParams::KadrListChange(TObject *Sender, TListItem *Item,
          TItemChange Change)
{
 LPOTDM_PROTO_KADR pkadr = (LPOTDM_PROTO_KADR)Item->Data;
 if(pkadr)
 {
  modem_addr ma = get_modem_addr();
  if(module && Item->Checked != module->is_kadr_ordered(&ma,pkadr->kadr.kadr_id))
    this->Forders_changed = true;
 }
}
//---------------------------------------------------------------------------

void __fastcall TAwpModuleParams::PageControl1Change(TObject *Sender)
{
  Timer1->Enabled = PageControl1->ActivePage == ReadySheet ? true:false;
}
//---------------------------------------------------------------------------


void __fastcall TAwpModuleParams::Timer1Timer(TObject *Sender)
{
       Timer1->Tag++;
       this->ReadyTest->Repaint();
}
//---------------------------------------------------------------------------



void __fastcall TAwpModuleParams::bSaveCurrentLayoutClick(TObject *Sender)
{
  if(!Layouts->Text.IsEmpty() && save_layout)
  {
     save_layout(Layouts->Text);
     SetFocus();
  }
}
//---------------------------------------------------------------------------

void __fastcall TAwpModuleParams::bReadLayoutClick(TObject *Sender)
{
  if(!Layouts->Text.IsEmpty() && open_layout)
     {
      open_layout(Layouts->Text);
      SetFocus();
     }
}
//---------------------------------------------------------------------------

void __fastcall TAwpModuleParams::LayoutsDropDown(TObject *Sender)
{
  if(this->enum_layouts)
     enum_layouts(Layouts->Items);
}
//---------------------------------------------------------------------------

void __fastcall TAwpModuleParams::ReadyFontApply(TObject *Sender, HWND Wnd)
{
  Ffont_changed = true;
  ReadyTest->Repaint();
}
//---------------------------------------------------------------------------

