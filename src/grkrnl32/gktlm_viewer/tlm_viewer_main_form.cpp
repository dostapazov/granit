//---------------------------------------------------------------------------

#pragma hdrstop
#include <vcl.h>

#include <tregstry.hpp>
#include <KeStrm.hpp>
#include <kfname.hpp>
#include <Math.hpp>
#include <algorithm>
#include <vector_index.hpp>
#include "tlm_viewer_main_form.h"
#include "history_data.h"


//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "gktlm_viewer_form"
#pragma link "gktlm_viewer_form"
#pragma link "gktlm_viewer_form"
#pragma resource "*.dfm"

using namespace stdlib_utils;


TTlmViewerMainForm *TlmViewerMainForm = NULL;
//---------------------------------------------------------------------------
__fastcall TTlmViewerMainForm::TTlmViewerMainForm(TComponent* Owner)
	: TTlmViewerForm(Owner)
{
 about         = NULL;
 login         = NULL;
 Constraints->MaxHeight = Height;
 max_kadr_size.cx = 0;
 max_kadr_size.cy = 0;
}


 __fastcall TTlmViewerMainForm::TTlmViewerMainForm(TComponent* Owner,TGKTlmViewer * _viewer_module)
 	: TTlmViewerForm(Owner,_viewer_module)
{


 about         = NULL;
 login         = NULL;
 Constraints->MaxHeight = Height;
 max_kadr_size.cx = 0;
 max_kadr_size.cy = 0;
  if(!TTlmPainter::painter_dll.is_loaded())
      TTlmPainter::painter_dll.load_module("tlmobj_std_painter.dll");
      //TTlmPainter::painter_dll.load_module("tlmobj_painter.dll");

}

//---------------------------------------------------------------------------
void __fastcall TTlmViewerMainForm::FormCloseQuery(TObject *Sender, bool &CanClose)
{
  //CanClose = false;
}
//---------------------------------------------------------------------------

void __fastcall TTlmViewerMainForm::miAboutClick(TObject *Sender)
{
 //ShellAboutW(Handle,L"Просмотор кадров БД среднего уровня",L"Остапенко Д.В.\n\r Норильк-Азов 1993-2016",this->Icon->Handle);
 if(!about)
   {
     about = new TAboutForm(this);
     about->Visible = true;
     about->OnClose = about_close;
   }
   about->SetFocus();
}
//---------------------------------------------------------------------------
void __fastcall TTlmViewerMainForm::about_close (TObject *Sender, TCloseAction &Action)
{
   Action = caFree;
   about  = NULL;
}


void  __fastcall TTlmViewerMainForm::start()
{
 if(!painter_created)  create_painter();
  WindowState = wsMaximized;
  update_ready_layout();
  Visible     = true;
  kadrs_order(true);
  show_kadrs (true);

  Repaint();
  SetFocus();
  Timer1->Enabled = true;
  UnicodeString aol = viewer_module->get_auto_open_layout();
  if(!aol.IsEmpty())
     do_open_layout(aol);
  TlmViewerMainForm = this;
  control_sound_thread(true);
}

void  __fastcall TTlmViewerMainForm::show_kadrs(bool show)
{

  Tkadrs & kadrs_ordered = viewer_module->get_kadrs_ordered();
  Tkadrs::iterator kptr = kadrs_ordered.begin();
  Tkadrs::iterator kend = kadrs_ordered.end();
  while(kptr<kend)
  {
   Tmdb_kadr_ex * kadr = *kptr;
   if(kadr->tv)
      kadr->tv->Visible = show;
   ++kptr;
  }
}

void  __fastcall TTlmViewerMainForm::stop()
{
  control_sound_thread(false);

  hot_key_register(HOTKEY_ID_CLEAR_ALL  ,0);
  hot_key_register(HOTKEY_ID_RECEIPT_ALL,0);
  viewer_module->reset_diag ();

  Application->OnMessage = NULL;
  kadrs_order(false);
  Visible         = false;
  Timer1->Enabled = false;
  show_kadrs      (false);

}


void   __fastcall TTlmViewerMainForm::handle_security   (LPMPROTO_HEADER mph)
{
     LPOTDM_PROTO_HEADER omph = (LPOTDM_PROTO_HEADER)mph->data;

     switch(omph->command&OTDM_COMMAND_MASK)
     {
      case GKSECUR_PROTO_CMD_LOGIN:
          if(omph->command&OTDM_RESPOND)
          {
           UnicodeString str;
           if(!omph->error && gksecur_login_extract(omph,&remote_ssid,NULL,0,NULL,0))
              {
                remote_ssid = remote_ssid;
                str.printf(L"Ключ сессии %ld",remote_ssid);
               }
              else
              {
               remote_ssid = 0;
               str = L"Имя пользователя и пароль неизвестны";

              }
             if(login)
                {
                 login->StatusBar1->Panels->Items[0]->Text = str;
                 login->Visible = !remote_ssid;
                }
          }
      break;
      default : break;
     }
}



void  __fastcall TTlmViewerMainForm::handle_recv(LPMPROTO_HEADER mph)
{
  if(mph->fa == FA_SECURITY)
 {
   handle_security(mph);
   return;
 }
 TTlmViewerForm::handle_recv(mph);
}

void   __fastcall TTlmViewerMainForm::handle_mproto         (LPMPROTO_HEADER mph)
{
 LPOTDM_PROTO_HEADER omph = (LPOTDM_PROTO_HEADER)mph->data;
 if(omph->command & OTDM_RESPOND )
 {
   bool beg_resp = (omph->command&OTDM_BEGINDATA) ? true:false;
   bool end_resp = (omph->command&OTDM_ENDDATA) ? true:false;
   DWORD cmd = omph->command&OTDM_COMMAND_MASK;
   switch(cmd)
   {
    case    OTDMPROTO_CMD_GET_PAINT_VARIANTS: if(!omph->error) handle_paint_variants(omph->data,omph->data_size,beg_resp,end_resp);
    break;
    case    OTDMPROTO_CMD_PAINT_IMAGE        : if(!omph->error) handle_paint_image(omph->data,omph->data_size,beg_resp,end_resp);
    break;
    case   OTDMPROTO_CMD_NOTIFY_DBCHANGE:   PostMessage(Handle,TLMMF_DB_CHANGE,0,0);
    break;
    default : TTlmViewerForm::handle_mproto(mph);
   }
 }
}

void   __fastcall TTlmViewerMainForm::handle_medproto_error (LPOTDM_PROTO_HEADER omph)
{
  DWORD cmd = omph->command&OTDM_COMMAND_MASK;
  //LPOTDM_PROTO_KADR p_kadr = (LPOTDM_PROTO_KADR )omph->data;
  switch(cmd)
  {
   case OTDMPROTO_CMD_GETKADR:
   case OTDMPROTO_CMD_ENUM_KADRS:
   {
    Tkadrs::iterator ptr;
    DWORD kadr_id = omph->data[0];
    TGKTlmViewer::locker l(viewer_module->get_mutex());

    if(binary_search  (viewer_module->get_kadrs_avail(),&Tmdb_kadr_ex(kadr_id),&ptr))
       viewer_module->get_kadrs_avail().erase(ptr);
    if(binary_search(viewer_module->get_kadrs_ordered(),&Tmdb_kadr_ex(kadr_id),&ptr))
        ::PostMessage(Handle,TLMMF_REMOVE_ORDER,kadr_id,0);
    }
    break;
    default:
    TTlmViewerForm::handle_medproto_error (omph);
    break;
   }

}

DWORD  __fastcall TTlmViewerMainForm::handle_get_kadr      (Tkadrs & klist,Tnames * _names,LPOTDM_PROTO_HEADER omph,bool end)
{
  DWORD ch = TTlmViewerForm::handle_get_kadr(klist,_names,omph,end);
  if(&klist == &viewer_module->get_kadrs_ordered())
  {
   LPOTDM_PROTO_KADR p_kadr = (LPOTDM_PROTO_KADR )omph->data;
   if(ch&MDB_KADR_FIELD_IMAGE)
      {
       check_kadr_image(p_kadr->kadr.kadr_id);
       ch&=~MDB_KADR_FIELD_IMAGE;
      }
   PostMessage(Handle,TLMKV_WM_UPDATE_KADR,ch,p_kadr->kadr.kadr_id);
   return ch;
  }

  if(&klist == &viewer_module->get_kadrs_avail() /*&& params*/)
  {
//   LPOTDM_PROTO_KADR kadr = (LPOTDM_PROTO_KADR )omph->data;
//   Tkadrs & kadrs_ordered = viewer_module->get_kadrs_ordered();
//   bool ordered = binary_search(kadrs_ordered,&Tmdb_kadr_ex(kadr->kadr.kadr_id));
//   if(!ordered || (DWORD)params->KadrList->Items->Count < kadrs_ordered.size())
//      SendMessage(params->Handle,WNDCM_ADD_KADR,WPARAM(ordered),(LPARAM)kadr);
  }
 return ch;
}





void __fastcall TTlmViewerMainForm::miParametersClick(TObject *Sender)
{
     viewer_module->show_parameters() ;
}
//---------------------------------------------------------------------------
void __fastcall TTlmViewerMainForm::params_close(TObject *Sender, TCloseAction &Action)
{
//  if(params->ModalResult == mrOk)
//  {
//	bool params_changed = false;
//        UnicodeString data_dir = viewer_module->get_data_dir();
//	if(data_dir.CompareIC(params->DataFolder->Text))
//	 {
//		viewer_module->set_data_dir(params->DataFolder->Text);
//		params_changed = true;
//                ForceDirs(viewer_module->get_data_dir().c_str());
//	 }
//
//	if(THistoryForm::ExportFolder.CompareIC(params->ExportFolder->Text))
//	 {
//	   THistoryForm::ExportFolder = params->ExportFolder->Text;
//		params_changed = true;
//	 }
//	if(THistoryForm::ExportViewer.CompareIC(params->ExportViewer->Text))
//	 {
//	   THistoryForm::ExportViewer = params->ExportViewer->Text;
//           params_changed = true;
//	 }
//
////      if( params->cbNotGrayBkgnd->Checked != TTlmPainter::not_gray_bkgnd)
////        {
////         TTlmPainter::not_gray_bkgnd = params->cbNotGrayBkgnd->Checked ;
////         params_changed = true;
////         update_kadrs_bkgnd();
////        }
//
//        if(viewer_module->get_enable_double_buffered()  != params->cbDoubleBuffered->Checked)
//        {
//         viewer_module->set_enable_double_buffered (  params->cbDoubleBuffered->Checked );
//         params_changed = true;
//         reset_double_buffered();
//        }
//
//        if(THistoryForm::history_time_before != params->HistoryTimeBefore->Text.ToInt())
//        {
//         THistoryForm::history_time_before = params->HistoryTimeBefore->Text.ToInt();
//         params_changed = true;
//        }
//        if(THistoryForm::history_time_after != params->HistoryTimeAfter->Text.ToInt())
//        {
//         THistoryForm::history_time_after = params->HistoryTimeAfter->Text.ToInt();
//         params_changed = true;
//        }
//
//
//        bool rdy_changed = false;
//	for(int i = 0;i<params->KadrList->Items->Count;i++)
//	{
//	  TListItem   * item = params->KadrList->Items->Item[i];
//
//	  if(item->Checked)
//            rdy_changed |= kadr_order_add   ((DWORD)item->Data);
//            else
//            rdy_changed |= kadr_order_remove((DWORD)item->Data);
//        }
//
//        if(params->font_changed)
//          {
//           rdy_changed = true;
//           PainterPanel->Font = params->ReadyFont->Font;
//           PainterPanel->Font->Charset =  RUSSIAN_CHARSET;
//           painter->SetFont(PainterPanel->Font->Handle,true);
//          }
//
//         if(params->MonitorSelect->ItemIndex    != viewer_module->get_main_form_monitor())
//          {
//            viewer_module->set_main_form_monitor(params->MonitorSelect->ItemIndex);
//            rdy_changed = true;
//          }
//
//          if(viewer_module->get_ready_spacer() != params->ReadySpacer->Position)
//          {
//            viewer_module->set_ready_spacer(params->ReadySpacer->Position);
//            rdy_changed = true;
//          }
//
//          if(rdy_changed) { update_ready_layout(); params_changed = true;}
//
//          if(params->colors_changed)
//          {
//           READY_COLORS & rdy_colors = viewer_module->get_ready_colors();
//           rdy_colors  = params->get_ready_colors();
//           painter->painter_dll(PMCMD_PAINT_SET_READY_COLORS,(LPARAM)&rdy_colors,0);
//           painter->Repaint(true);
//           params_changed = true;
//       }
//
//
//	if(params->Source->ItemIndex  != viewer_module->get_external_source())
//	  {
//           kadrs_order(false);
//           viewer_module->set_external_source(params->Source->ItemIndex);
//
//	   params_changed = true;
//           viewer_module->reset_diag();
//           kadrs_order(true);
//          }
//
//          if(params->Layouts->Text != viewer_module->get_auto_open_layout())
//          {
//            viewer_module->set_auto_open_layout(params->Layouts->Text);
//            params_changed = true;
//          }
//
//          if(params->HotKeyClearAll->HotKey      != viewer_module->get_hot_key_clear_all())
//          {
//             viewer_module->set_hot_key_clear_all(params->HotKeyClearAll->HotKey);
//             params_changed    = true;
//             hot_key_register(HOTKEY_ID_CLEAR_ALL,params->HotKeyClearAll->HotKey);
//          }
//
//         if(params->HotKeyReceiptAll->HotKey    != viewer_module->get_hot_key_receipt_all())
//         {
//             viewer_module->set_hot_key_receipt_all ( params->HotKeyReceiptAll->HotKey);
//             params_changed      = true;
//             hot_key_register(HOTKEY_ID_CLEAR_ALL,params->HotKeyReceiptAll->HotKey);
//         }
//
//         if(_wtoi(params->OrderPeriod->Text.c_str())!= viewer_module->get_order_period())
//         {
//            params_changed = true;
//            viewer_module->set_order_period(_wtoi(params->OrderPeriod->Text.c_str()));
//         }
//         if(_wtoi(params->StableReadyTimer->Text.c_str())!= viewer_module->get_stable_ready_timer())
//         {
//            params_changed = true;
//            viewer_module->set_stable_ready_timer(_wtoi(params->StableReadyTimer->Text.c_str()));
//         }
//
//         if( params->snd_disparity->Text.Compare(viewer_module->get_snd_disparity()))
//         {
//           viewer_module->get_snd_disparity() = params->snd_disparity->Text;
//           params_changed = true;
//         }
//
//         if(   params->snd_back->Text.Compare(viewer_module->get_snd_back()))
//         {
//           params_changed = true;
//           viewer_module->get_snd_back() = params->snd_back->Text;
//         }
//
//
//
//    if(params_changed)
//	   {
//             viewer_module->config_change(CCFL_PARAMS);
//	   }
//
//  PostMessage(Handle,TLMKV_WM_SET_ENTRYES,0,0);
//  }

  if(Action!=caNone)
  {
     Action = caFree;
  }
}



void __fastcall TTlmViewerMainForm::kadrs_order(bool order)
{
  TGKTlmViewer::locker l(viewer_module->get_mutex());
  if(order)
     request_paint_variants();
  Tkadrs & kadrs_ordered = viewer_module->get_kadrs_ordered() ;
  Tkadrs::iterator ptr = kadrs_ordered.begin();
  Tkadrs::iterator end = kadrs_ordered.end();
  while(ptr<end)
  {
    kadr_order(viewer_module->get_modem_source(),(*ptr)->kadr_id,order);
    ++ptr;
  }
}



void  __fastcall TTlmViewerMainForm::get_kadr_extent (Tmdb_kadr_ex * kadr, KeWin::TSize & sz,bool update_max_size)
{

      kadr->kadr_entry.size.cx = 0;
      kadr->kadr_entry.size.cy = 0;
      painter->calc_min_entry_size(kadr->kadr_entry,sz);
      UnicodeString capt = get_kadr_text(*kadr);
      capt+=L'W';
      KeWin::TSize capt_sz;
      GetTextExtentPoint32W(painter->GetClientDC(),capt.c_str(),capt.Length(),&capt_sz);
      sz.cx = std::max(capt_sz.cx,sz.cx);
      //sz.cy = std::max(capt_sz.cy,sz.cy);
      if(update_max_size)
         {
          max_kadr_size.cx = std::max(max_kadr_size.cx,sz.cx);
          max_kadr_size.cy = std::max(max_kadr_size.cy,sz.cy);
         }
}

void  __fastcall TTlmViewerMainForm::update_max_kadr_size()
{
  max_kadr_size.cx = max_kadr_size.cy = 0;
  TGKTlmViewer::locker l(viewer_module->get_mutex());
  Tkadrs & kadrs_ordered = viewer_module->get_kadrs_ordered() ;
  Tkadrs::iterator kbeg = kadrs_ordered.begin(),kend = kadrs_ordered.end();
  while(kbeg<kend)
  {
    Tmdb_kadr_ex * kadr = *kbeg;
    KeWin::TSize sz;
    get_kadr_extent(kadr,sz,true);
    ++kbeg;
  }

}


bool  __fastcall TTlmViewerMainForm::kadr_order_add (DWORD kadr_id)
{
  Tkadrs::iterator a_ptr,o_ptr;
  Tmdb_kadr_ex kdr(kadr_id);
  Tmdb_kadr_ex * kadr = NULL;
  TGKTlmViewer::locker l(viewer_module->get_mutex());
  Tkadrs & kadrs_ordered = viewer_module->get_kadrs_ordered() ;
  Tkadrs & kadrs_avail   = viewer_module->get_kadrs_avail  ();
  if(!binary_search(kadrs_ordered,&kdr,&o_ptr) && binary_search(kadrs_avail,&kdr,&a_ptr) )
    {
      kadr = *a_ptr;
      kadrs_avail.erase(a_ptr);
      o_ptr = kadrs_ordered.insert(o_ptr,kadr);

      mdb_kadr_entry & kadr_entry =kadr->get_kadr_entry();
      kadr_entry.kadr_id = 0;
      kadr_entry.rec_id = kadr_id;
      kadr_entry.number = -1;
      kadr_entry.options.bkgnd_color = painter->GetBkColor();
      kadr_entry.options.color       = viewer_module->get_ready_color();
      kadr_entry.options.paint_as    = RDY_PAINT_AS_READY;

      painter->add_entry(kadr_entry);
      kadr_order(viewer_module->get_modem_source(),kadr_id,true);
      viewer_module->config_change(CCFL_KADRS);
      return true;
    }
  return false;
}

 int   __fastcall TTlmViewerMainForm::calc_kadr_columns(int READY_SPACER)
 {

    return Width/(max_kadr_size.cx + READY_SPACER);
 }

 int   __fastcall TTlmViewerMainForm::calc_ready_height(int READY_SPACER)
 {
   int columns = calc_kadr_columns(READY_SPACER);
   Tkadrs & kadrs_ordered = viewer_module->get_kadrs_ordered() ;
   int rows    = kadrs_ordered.size()/columns;
   if(!rows) ++rows;
   int height = rows*max_kadr_size.cy + (rows)*(READY_SPACER);
   return height;
 }


bool   __fastcall TTlmViewerMainForm::kadr_order_remove(DWORD kadr_id)
{
  kadr_order(viewer_module->get_modem_source(),kadr_id,false);
  Tkadrs::iterator kptr;
  TGKTlmViewer::locker l(viewer_module->get_mutex());
  Tkadrs & kadrs_ordered = viewer_module->get_kadrs_ordered() ;
  if(binary_search(kadrs_ordered,&Tmdb_kadr_ex(kadr_id),&kptr))
  {
    Tmdb_kadr_ex * kadr = *kptr;
    mdb_kadr_entry & e = kadr->get_kadr_entry();
    painter->remove_entry(e);
    kadrs_ordered.erase(kptr);
    delete kadr;
    viewer_module->config_change(CCFL_KADRS);
    PostMessage(Handle,TLMKV_WM_SET_ENTRYES,kadr_id,0);
    return true;
  }
  return false;
  //Удалить entry
}


void __fastcall  TTlmViewerMainForm::WndProc            (Winapi::Messages::TMessage & msg)
{
 switch(msg.Msg)
 {
 case WM_NCHITTEST:
      TTlmViewerForm::WndProc(msg);
      if(msg.Result == HTCAPTION) msg.Result = HTCLIENT;
	break;
  case 	 TLMKV_WM_RESCAN_READY:
         rescan_ready(msg.LParam);
    break;
  case TLMKV_WM_MLINE_STATE:
       handle_modem_line_state((DWORD)msg.WParam,(DWORD)msg.LParam);
    break;
  case TLMKV_SETUP_PAINTER_DLL: do_setup_painter_dll();
       break;
  case WM_HOTKEY:
       switch(msg.WParam)
       {
           case HOTKEY_ID_CLEAR_ALL   :clear_all   ();break;
           case HOTKEY_ID_RECEIPT_ALL :receipt_all ();break;
       }
  break;
  case TLMMF_WRITE_CONFIG:
    viewer_module->write_config(msg.WParam,(HKEY)msg.LParam,NULL);
  break;
  case TLMMF_REMOVE_ORDER:
       kadr_order_remove(msg.WParam);
  break;
  case TLMMF_DB_CHANGE   : handle_database_change ();break;

  default : TTlmViewerForm::WndProc(msg);
	break;
 }
}

bool __fastcall  TTlmViewerMainForm::get_text         (DWORD id,UnicodeString & name)
{
  if(!TTlmViewerForm::get_text(id,name))
       name.printf(L"Kadr ID-%u",id);
  return true;
}

bool __fastcall TTlmViewerMainForm::get_record       (TObject * sender, DWORD id, bool offset, mdb_record_ex & rec)
{
  //Здесь для отрисовки готовности мы record заполняем из описателя кадра
  Tkadrs::iterator ptr;
  TGKTlmViewer::locker l(viewer_module->get_mutex());
  Tkadrs & kadrs_ordered = viewer_module->get_kadrs_ordered() ;
  if(binary_search(kadrs_ordered,&Tmdb_kadr_ex(id),&ptr))
  {
    Tmdb_kadr_ex * kadr = *ptr;
    rec.rec_id   = kadr->kadr_id;
    rec.diag     = kadr->kadr_diag;
    //rec.value    = kadr->kadr_diag ? 0:1;
    rec.sname    = kadr->sname_id;
    rec.name     = kadr->name_id;
    rec.dw_value = kadr->ready_flags;
    if(kadr->ready_flags&RDY_FLAG_MASK_FLASHED)
       rec.state |= MDBR_STATE_FLASH;
       else
       rec.state &= ~MDBR_STATE_FLASH;

    return true;
  }
  return false;
}





  void __fastcall  TTlmViewerMainForm::set_kadr_entryes (DWORD kadr_id)
  {
   TGKTlmViewer::locker l(viewer_module->get_mutex());
   Tkadrs & kadrs_ordered = viewer_module->get_kadrs_ordered() ;
    painter->clear_entryes();
    Tkadrs::iterator ptr = kadrs_ordered.begin();
    Tkadrs::iterator end = kadrs_ordered.end();
    while(ptr<end)
    {
     painter->add_entry((*ptr)->get_kadr_entry());
     ++ptr;
    }
    update_ready_layout();
  }

  DWORD __fastcall TTlmViewerMainForm::on_select_entry (TTlmPainter * p,DWORD entry  )
  {
    Tkadrs::iterator kptr;
    mdb_kadr_entry ke;
    painter->get_kadr_entry(entry,ke);
    TGKTlmViewer::locker l(viewer_module->get_mutex());
    Tkadrs & kadrs_ordered = viewer_module->get_kadrs_ordered() ;
    if(binary_search(kadrs_ordered,&Tmdb_kadr_ex(ke.kadr_id),&kptr))
    {
      Tmdb_kadr_ex * kadr = *kptr;
      open_kadr(kadr,true,true);
    }
    return TRUE;
  }

void __fastcall TTlmViewerMainForm::open_kadr(Tmdb_kadr_ex * kadr,bool show,bool focused)
{
      TTlmKadrViewer * tv = dynamic_cast<TTlmKadrViewer *>(kadr->get_viewer_form());
      if(!tv)
        {
          tv = new TTlmKadrViewer(this,viewer_module);
          kadr->set_viewer_form(tv);
          tv->open_kadr(kadr);
        }
      if(show && !tv->Visible)
      {
        // Не выходить за границы
        tv->Visible = true;
        Application->ProcessMessages();
        Vcl::Forms::TMonitor * mon = Screen->MonitorFromWindow((unsigned int)Handle);
        if(mon && mon->MonitorNum == viewer_module->get_main_form_monitor())
        {
          System::Types::TRect r = mon->WorkareaRect;
          r.top = Height;
          tv->Top = std::max(tv->Top,(int)r.top);
        }
      }

      if(focused)
         tv->SetFocus();
}

BOOL  __fastcall TTlmViewerMainForm::on_mouse_buttons(TTlmPainter * p,Tmouse_data * md)
{

  if(!md->down)
  {
    DWORD ent_num = painter->find_entry(md->pt);
    if(ent_num !=DWORD (-1))
    {
     DWORD kadr_id = painter->get_entry_rec_id(ent_num);
     Tkadrs::iterator kptr;
     TGKTlmViewer::locker l(viewer_module->get_mutex());
     Tkadrs & kadrs_ordered = viewer_module->get_kadrs_ordered() ;
     if(binary_search(kadrs_ordered,&Tmdb_kadr_ex(kadr_id),&kptr))
     {
      Tmdb_kadr_ex * kadr = *kptr;
      if(md->mbuttons&MOUSE_BUTTON_LEFT)
      {
       open_kadr(kadr);
      }

      if(md->mbuttons&MOUSE_BUTTON_RIGHT)
        {
          System::Types::TPoint pt;
          pt.X = md->pt.x;
          pt.Y = md->pt.y;
          pt = PainterPanel->ClientToScreen(pt);
          PopupMenu1->Tag  = kadr_id;
          PopupMenu1->Popup(pt.X,pt.Y);
        }
     }
   }
  }
  return TRUE;
}

void   __fastcall TTlmViewerMainForm::update_kadr           (DWORD kadr_id,DWORD mask)
{
  Tkadrs::iterator kptr;
  Tkadrs & kadrs_ordered = viewer_module->get_kadrs_ordered() ;
  if(mask && binary_search(kadrs_ordered,&Tmdb_kadr_ex(kadr_id),&kptr))
  {
    Tmdb_kadr_ex * kadr = *kptr;
    kadr->timer = 0;
    if(mask & (MDB_KADR_FIELD_NAME|MDB_KADR_FIELD_SNAME))
       update_ready_layout();

     if(!scan_ready(*kptr) || (mask&MDB_KADR_FIELD_STATE))
       {
        painter->update_entry((*kptr)->get_kadr_entry());
        painter->repaint_record(kadr_id);
       }
  }
}

void  __fastcall TTlmViewerMainForm::reconnect()
{
  //Отключилась линия источник
  handle_logout();
  viewer_module->reset_diag();
  kadrs_order(true);
}

void __fastcall TTlmViewerMainForm::handle_logout()
{
  remote_ssid = local_ssid = 0;
  if(this->login)  delete login;
  login = NULL;
}




void __fastcall TTlmViewerMainForm::Timer1Timer(TObject *Sender)
{
  ++TTlmPainter::flash_counter;
  TGKTlmViewer::locker l(viewer_module->get_mutex());
  Tkadrs & kadrs_ordered = viewer_module->get_kadrs_ordered() ;
  Tkadrs::iterator kptr = kadrs_ordered.begin();
  Tkadrs::iterator end  = kadrs_ordered.end();
  bool flashed = false;
  while(kptr<end)
  {
   Tmdb_kadr_ex * kadr = *kptr;
   kadr->timer       += Timer1->Interval;
   kadr->ready_timer += Timer1->Interval;

   if(kadr->timer>(DWORD)viewer_module->get_order_period()*1000)
     {
      kadr->timer = 0;
      if(viewer_module->get_order_period())
         kadr_order(viewer_module->get_modem_source(),kadr->kadr_id,true);
     }

   if(viewer_module->get_stable_ready_timer()>0)
   {
   if(kadr->ready_timer > (DWORD)viewer_module->get_stable_ready_timer()*60*1000)
      {
       set_kadr_ready_flags(kadr,RDY_FLAG_STABLE,0);
       kadr->ready_timer -= Timer1->Interval; //Исключить переполнение
      }
      else
      set_kadr_ready_flags(kadr,0,RDY_FLAG_STABLE);
   }

   if((kadr->ready_flags&RDY_FLAG_MASK_FLASHED))
      {
       flashed = true;
       if(kadr->tv) kadr->tv->painter->flash_repaint();
      }
   ++kptr;
  }

 //if(flashed)
     {
      painter->flash_repaint();
     }
//     else
//     this->repaint_all_kadrs();
}
//---------------------------------------------------------------------------

bool  __fastcall     TTlmViewerMainForm::scan_ready  (Tmdb_kadr_ex * kadr)
{
  if(kadr)
  {
   TGKTlmViewer::locker l(viewer_module->get_mutex());
   Trecords & records = viewer_module->get_records() ;
   Tentryes_list::iterator ptr = kadr->entryes.begin();
   Tentryes_list::iterator end = kadr->entryes.end();
   DWORD new_state        = kadr->ready_flags&(RDY_FLAG_SELECTED|RDY_FLAG_ALARM_ARCHIVE|RDY_FLAG_ARCHIVE|RDY_FLAG_FLASH_NAME|RDY_FLAG_STABLE);

   WORD  new_diag         = 0;
   DWORD ch               = 0;
   DWORD total_changes    = 0;
   while(ptr<end)
   {
	Trecords::iterator rptr;
	if(binary_search(records,mdb_record_ex(ptr->rec_id),&rptr))
	{
          new_diag |= rptr->diag;

	  bool is_flashed  = rptr->is_flashed();
	  bool is_alarm    = rptr->is_alarm_state();
	  bool is_danger   = rptr->is_danger_state();

          if(rptr->state & MDBR_STATE_TUTR_PREPARE)
              new_state |= RDY_FLAG_TUTR_PREPARE;
          if(rptr->state & MDBR_STATE_TUTR_ACTIVE)
              new_state |= RDY_FLAG_TUTR_ACTIVE;
          if(rptr->state & MDBR_STATE_TUTR_ERROR)
              new_state |= RDY_FLAG_TUTR_ERROR;

	 if(rptr->is_discrete() )
	 {
	  if(!rptr->is_static())
	  {
	   ch = rptr->get_changes();
           total_changes += ch;
	   if(!rptr->is_normal_state())
		  {
                   new_state|= (RDY_FLAG_NOTNORMAL | ((rptr->options&MDBR_OPT_ALARM) ? RDY_FLAG_DANGER_NOTNORMAL : 0));
                   if(is_flashed)
                      new_state |= RDY_FLAG_NOTNORMAL_FLASH;
                  }
		  else
		  {
		   if(ch)
                   {
		      new_state|=(RDY_FLAG_BACKNORMAL| ((rptr->options&MDBR_OPT_ALARM)  ? RDY_FLAG_DANGER_BACKNORMAL : 0));
                   }
                  if(is_flashed)
                     new_state |= RDY_FLAG_BACKNORMAL_FLASH;
		  }

		if(ch>2)
                {
		   new_state|=(RDY_FLAG_MANY|((rptr->options&MDBR_OPT_ALARM)  ? RDY_FLAG_DANGER_MANY : 0));
                   if(is_flashed)
                      new_state |= RDY_FLAG_MANY_FLASH;
                }
	  }
	 }
	 else
	 {
	  if(is_alarm)
	     new_state |= RDY_FLAG_ALARM;
	    else
	    {
	    if(is_danger)
              new_state |= RDY_FLAG_DANGER;
	    }
	 }
	}
	++ptr;
   }
     FLASHWINFO fli;
     ZeroMemory(&fli,sizeof(fli));
     fli.cbSize    = sizeof(fli);
     fli.hwnd      = Handle;
     if(!total_changes)
       {
        new_state &= ~RDY_FLAG_FLASH_NAME;
        fli.dwFlags = FLASHW_STOP;
       }
       else
       {
        fli.uCount    = 3;
        fli.dwTimeout = Timer1->Interval;
        fli.dwFlags   = FLASHW_ALL;
       }

//      if(Visible)
//         FlashWindowEx(&fli);

   if(kadr->kadr_diag != new_diag )
      {
       kadr->kadr_diag   = new_diag;
       kadr->ready_timer = 0;
       kadr->ready_flags &=~RDY_FLAG_STABLE;
      }
   DWORD ls_changes = kadr->ready_flags ^ new_state;
   if(ls_changes)
	 {
          ls_changes&=new_state;
           if(ls_changes& (RDY_FLAG_ALARM|RDY_FLAG_DANGER ) )
             TrayIcon1->IconIndex = 2;
             else
             {
              if(ch)
                TrayIcon1->IconIndex = 1;
                else
                TrayIcon1->IconIndex = 0;
             }
          if(kadr->ready_flags ^ new_state)
          {
           kadr->ready_flags = 0;
	   set_kadr_ready_flags(kadr,new_state,0);
          }
	  return true;
	 }
  }
 return false;
}

//bool  __fastcall     TTlmViewerMainForm::scan_ready  (Tmdb_kadr_ex * kadr)
//{
//  if(kadr)
//  {
//   locker l(mut);
//   Tentryes_list::iterator ptr = kadr->entryes.begin();
//   Tentryes_list::iterator end = kadr->entryes.end();
//   DWORD new_state        = kadr->ready_flags&(RDY_FLAG_SELECTED|RDY_FLAG_ALARM_ARCHIVE|RDY_FLAG_ARCHIVE|RDY_FLAG_FLASH_NAME);
//
//   WORD  new_diag         = 0;
//   DWORD ch               = 0;
//   while(ptr<end)
//   {
//	Trecords::iterator rptr;
//	if(binary_search(records,mdb_record_ex(ptr->rec_id),&rptr))
//	{
//          new_diag |= rptr->diag;
//	  if(rptr->is_flashed())
//		 new_state |= RDY_FLAG_MASK_FLASHED;
//	  bool is_alarm  = rptr->is_alarm_state();
//	  bool is_danger = rptr->is_danger_state();
//          if(rptr->state & MDBR_STATE_TU_ACTIVE)
//              new_state |= RDY_FLAG_TUTR_ACTIVE;
//          if(rptr->state & MDBR_STATE_TUERROR)
//              new_state |= RDY_FLAG_TUTR_ERROR;
//
//	 if(rptr->is_discrete() )
//	 {
//	  if(!rptr->is_static())
//	  {
//	   ch = rptr->get_changes();
//
//	   if(!rptr->is_normal_state())
//		  new_state|= (RDY_FLAG_NOTNORMAL | ((rptr->options&MDBR_OPT_ALARM) ? RDY_FLAG_DANGER_NOTNORMAL : 0));
//		  else
//		  {
//		   if(ch)
//		   new_state|=(RDY_FLAG_BACKNORMAL| ((rptr->options&MDBR_OPT_ALARM)  ? RDY_FLAG_DANGER_BACKNORMAL : 0));
//		  }
//		if(ch>2)
//		   new_state|=(RDY_FLAG_MANY|((rptr->options&MDBR_OPT_ALARM)  ? RDY_FLAG_DANGER_MANY : 0));
//	  }
//	 }
//	 else
//	 {
//	  if(is_alarm)
//		new_state |= RDY_FLAG_ALARM;
//		else
//		{
//		if(is_danger)
//		   new_state |= RDY_FLAG_DANGER;
//		}
//	 }
//	}
//	++ptr;
//   }
//   if(kadr->kadr_diag != new_diag )
//      kadr->kadr_diag = new_diag;
//   DWORD ls_changes = kadr->ready_flags ^ new_state;
//   if(ls_changes)
//	 {
//          ls_changes&=new_state;
//           if(ls_changes& (RDY_FLAG_ALARM|RDY_FLAG_DANGER ) )
//             TrayIcon1->IconIndex = 2;
//             else
//             {
//              if(ch)
//                TrayIcon1->IconIndex = 1;
//                else
//                TrayIcon1->IconIndex = 0;
//             }
//	  kadr->ready_flags = new_state;
//	  set_kadr_ready_flags(kadr,new_state,true);
//	  return true;
//	 }
//  }
// return false;
//}


void  __fastcall     TTlmViewerMainForm::rescan_ready(DWORD kadr_id)
{
    TGKTlmViewer::locker l(viewer_module->get_mutex());
    Tkadrs & kadrs_ordered = viewer_module->get_kadrs_ordered() ;
    Tkadrs::iterator kbeg = kadrs_ordered.begin(),kend = kadrs_ordered.end(),kptr;
    bool changes = false;
    if(binary_search(kadrs_ordered,&Tmdb_kadr_ex(kadr_id),&kptr))
    {
          changes = scan_ready(*kptr);
	  if(changes )
	   {
		while(kbeg<kend)
		{
		 if(kbeg!=kptr)
		    scan_ready(*kbeg);
		 ++kbeg;

        }
       }
    }
}

struct dbl_buffer_setter
{
 bool dbl_buffered;
 dbl_buffer_setter(bool v):dbl_buffered(v){}
 dbl_buffer_setter(const dbl_buffer_setter & s){*this = s;}
 const dbl_buffer_setter & operator = (const dbl_buffer_setter & s){dbl_buffered = s.dbl_buffered; return *this;}
 void operator ()(Tmdb_kadr_ex * kdr)
 {
  if(kdr && kdr->tv && kdr->tv->painter_created)
     kdr->tv->painter->SetFlags(FLWND_DOUBLE_BUFFERED,dbl_buffered);
 }
};

void __fastcall   TTlmViewerMainForm::reset_double_buffered()
{
  TGKTlmViewer::locker l(viewer_module->get_mutex());
  Tkadrs & kadrs_ordered = viewer_module->get_kadrs_ordered() ;
  painter->SetFlags(FLWND_DOUBLE_BUFFERED,viewer_module->get_enable_double_buffered());
  std::for_each(kadrs_ordered.begin(),kadrs_ordered.end(),dbl_buffer_setter(viewer_module->get_enable_double_buffered()));
}

struct bkgnd_updater
{
 bkgnd_updater(){}
 void operator ()(Tmdb_kadr_ex * kdr)
 {
  if(kdr && kdr->tv && kdr->tv->painter_created)
     {
      kdr->tv->painter->do_setup_bkgnd() ;
      kdr->tv->painter->Repaint(true);
     }
 }
};

void __fastcall   TTlmViewerMainForm::update_kadrs_bkgnd()
{
  TGKTlmViewer::locker l(viewer_module->get_mutex());
  Tkadrs & kadrs_ordered = viewer_module->get_kadrs_ordered() ;
  bkgnd_updater bk_upd;
  std::for_each(kadrs_ordered.begin(),kadrs_ordered.end(),bk_upd);
}

struct kadr_closer
{
 kadr_closer(){}
 void operator ()(Tmdb_kadr_ex * kdr)
 {
  if(kdr && kdr->tv )
   PostMessage(kdr->tv->Handle,WM_CLOSE,0,0);
 }
};

struct kadr_closer_forced
{
 kadr_closer_forced(){}
 void operator ()(Tmdb_kadr_ex * kdr)
 {
  if(kdr && kdr->tv )
     {
      delete kdr->tv;
      kdr->tv = NULL;
     }
 }
};


void __fastcall  TTlmViewerMainForm::close_all_kadrs   (bool force)
{
  {
  TGKTlmViewer::locker l(viewer_module->get_mutex());
  Tkadrs & kadrs_ordered = viewer_module->get_kadrs_ordered() ;
  if(force)
  std::for_each(kadrs_ordered.begin(),kadrs_ordered.end(),kadr_closer_forced());
  else
  std::for_each(kadrs_ordered.begin(),kadrs_ordered.end(),kadr_closer());
  }
  Application->ProcessMessages();
}



void  __fastcall TTlmViewerMainForm::handle_modem_line_state(DWORD _ma,DWORD state)
{
  modem_addr ma(_ma);
  if(!(state&MODEM_LINE_STATE_CONNECT) )
  {
   if(ma.addr == viewer_module->get_modem_source().addr )
         reconnect();
  }
  else
  {
    modem_addr ma = viewer_module->get_modem_source() ;
    if(!ma.addr || ma.addr == (DWORD)-1  )
       kadrs_order(true);
  }
}

void  __fastcall     TTlmViewerMainForm::set_ready_color(TColor color)
{
  if(viewer_module->get_ready_color() !=color)
  {
   viewer_module->get_ready_color() = color ;
   TGKTlmViewer::locker l(viewer_module->get_mutex());
   Tkadrs & kadrs_ordered = viewer_module->get_kadrs_ordered();
   Tkadrs::iterator ptr  = kadrs_ordered.begin(),end = kadrs_ordered.end();
   while(ptr<end)
   {
     mdb_kadr_entry & e = (*ptr)->get_kadr_entry();
     e.options.color = viewer_module->get_ready_color();
     painter->update_entry(e);
     ++ptr;
   }
  }
}

void __fastcall TTlmViewerMainForm::miResetHistoryClick(TObject *Sender)
{
  //Сброс флагов историй
  DWORD kadr_id =  (DWORD)PopupMenu1->Tag;
  Tkadrs::iterator kptr;
   TGKTlmViewer::locker l(viewer_module->get_mutex());
   Tkadrs & kadrs_ordered = viewer_module->get_kadrs_ordered();
  if(binary_search(kadrs_ordered,&Tmdb_kadr_ex(kadr_id),&kptr))
  {
   (*kptr)->ready_flags&=~(RDY_FLAG_ALARM_ARCHIVE|RDY_FLAG_ARCHIVE);
   update_kadr(kadr_id,MDB_KADR_FIELD_STATE);
  }
}
//---------------------------------------------------------------------------

void __fastcall TTlmViewerMainForm::miUnorderKadrClick(TObject *Sender)
{
  DWORD kadr_id =  (DWORD)PopupMenu1->Tag;
  kadr_order_remove(kadr_id);
  viewer_module->config_change(CCFL_KADRS);
}
//---------------------------------------------------------------------------

void __fastcall TTlmViewerMainForm::swap_kadr_position(Tmdb_kadr_ex * k1,Tmdb_kadr_ex * k2)
{
  if(k1 && k2)
  {
   mdb_kadr_entry * e1 = &k1->get_kadr_entry();
   mdb_kadr_entry * e2 = &k2->get_kadr_entry();
   std::swap(e1->number,e2->number);
   update_ready_layout();
   PostMessage(Handle,TLMKV_WM_SET_ENTRYES,0,0);
   viewer_module->config_change(CCFL_KADRS);
  }
}

void __fastcall TTlmViewerMainForm::miMoveKadrLeftClick(TObject *Sender)
{
  DWORD kadr_id =  (DWORD)PopupMenu1->Tag;
  Tkadrs::iterator kptr;
   TGKTlmViewer::locker l(viewer_module->get_mutex());
   Tkadrs & kadrs_ordered = viewer_module->get_kadrs_ordered();
  if(binary_search(kadrs_ordered,&Tmdb_kadr_ex(kadr_id),&kptr))
  {
    swap_kadr_position(*kptr,find_next_kadr(*kptr,left));
  }
}
//---------------------------------------------------------------------------

void __fastcall TTlmViewerMainForm::miMoveKadrRightClick(TObject *Sender)
{
  DWORD kadr_id =  (DWORD)PopupMenu1->Tag;
  Tkadrs::iterator kptr;
   TGKTlmViewer::locker l(viewer_module->get_mutex());
   Tkadrs & kadrs_ordered = viewer_module->get_kadrs_ordered();
  if(binary_search(kadrs_ordered,&Tmdb_kadr_ex(kadr_id),&kptr))
  {
    swap_kadr_position(*kptr,find_next_kadr(*kptr,right));
  }

}
//---------------------------------------------------------------------------

void  __fastcall TTlmViewerMainForm::repaint_changed_records()
{
  TGKTlmViewer::locker l(viewer_module->get_mutex());
  Tkadrs & kadrs_ordered = viewer_module->get_kadrs_ordered();
  if(viewer_module->get_changed_records().size())
  {
   Tkadrs::iterator beg = kadrs_ordered.begin();
   Tkadrs::iterator end = kadrs_ordered.end  ();
   while(beg<end)
   {
    TTlmViewerForm * tv = (*beg)->tv;
    if(tv)
       {
        update_kadr((*beg)->kadr_id,MDB_KADR_FIELD_VALUES);
        tv->repaint_changed_records();
       }
    scan_ready(*beg);
    ++beg;
   }

   viewer_module->get_changed_records().clear();
  }


}


void __fastcall TTlmViewerMainForm::FormDestroy(TObject *Sender)
{
  if(kadr) delete kadr;
   TTlmPainter::painter_dll.unload_module();
}
//---------------------------------------------------------------------------
void  __fastcall TTlmViewerMainForm::do_setup_painter_dll(mdb_paint_variant & paint_var,bool repaint_kadrs)
{
   UnicodeString file_name;
   UnicodeString folder = viewer_module->get_data_dir();
   folder+=IMAGE_FOLDER;

  DWORD pv = TTlmPainter::painter_dll(PMCMD_PAINT_VAR_GET,paint_var.idx,0);
  bool add = true;
  if(!pv)
      pv = TTlmPainter::painter_dll(PMCMD_CREATE_PAINT_VARIANT_EMPTY ,0,0);
      else
      add = false;

  if(paint_var.image_id!=(DWORD)-1)
  {
   UnicodeString text;
   TTlmViewerForm::get_text(paint_var.image_id,text);
   file_name = folder;
   file_name+=text;
   AnsiString a_file_name = file_name;
   TTlmPainter::painter_dll(PMCMD_PAINT_VAR_CTRL_SET_IMAGE,pv,(LPARAM)a_file_name.c_str());
  }
  else
  TTlmPainter::painter_dll(PMCMD_PAINT_VAR_CTRL_SET_IMAGE,pv,(LPARAM)"");


  if(paint_var.mask_id!=(DWORD)-1)
  {
   UnicodeString text;
   TTlmViewerForm::get_text(paint_var.mask_id,text);
   file_name = folder;
   file_name+=text;
   AnsiString a_file_name = file_name;
   TTlmPainter::painter_dll(PMCMD_PAINT_VAR_CTRL_SET_MASK,pv,(LPARAM)a_file_name.c_str());
  }
  else
  TTlmPainter::painter_dll(PMCMD_PAINT_VAR_CTRL_SET_MASK,pv,(LPARAM)"");

  TTlmPainter::painter_dll(PMCMD_PAINT_VAR_CTRL_SET_WIDTH,pv,paint_var.size.cx);
  TTlmPainter::painter_dll(PMCMD_PAINT_VAR_CTRL_SET_HEIGHT,pv,paint_var.size.cy);
  TTlmPainter::painter_dll(PMCMD_PAINT_VAR_CTRL_SET_SWITCHED,pv,paint_var.switched);
  TTlmPainter::painter_dll(PMCMD_PAINT_VAR_CTRL_SET_MULTIPOS,pv,paint_var.multi_position);
  if(add)
    TTlmPainter::painter_dll(PMCMD_PAINT_VAR_ADD,pv,(LPARAM)"");
  if(repaint_kadrs)
     repaint_all_kadrs();
}

struct kadrs_repainter
{
 kadrs_repainter(){}
 void operator ()(Tmdb_kadr_ex * kdr)
 {
  if(kdr && kdr->tv && kdr->tv->Visible)
     kdr->tv->Repaint();
 }

};

void  __fastcall TTlmViewerMainForm::repaint_all_kadrs()
{
  TGKTlmViewer::locker l(viewer_module->get_mutex());
  Tkadrs & kadrs_ordered = viewer_module->get_kadrs_ordered();
  std::for_each(kadrs_ordered.begin(),kadrs_ordered.end(),kadrs_repainter());
}

void  __fastcall TTlmViewerMainForm::do_setup_painter_dll()
{

 TGKTlmViewer::locker l(viewer_module->get_mutex());
 Tpaint_variants & paint_vars = viewer_module->get_paint_vars();
 Tpaint_variants::iterator beg = paint_vars.begin(),end = paint_vars.end();
 while(beg<end)
 {
  do_setup_painter_dll(*beg,false);
  ++beg;
 }
 repaint_all_kadrs();
}

void  __fastcall TTlmViewerMainForm::handle_paint_variants(LPVOID data,DWORD data_size,bool beg_resp,bool end_resp)
{
  LPOTDM_PAINT_VARIANTS pv = (LPOTDM_PAINT_VARIANTS)data;
  LPMDB_PAINT_VARIANT   pvar_beg = pv->pvars;
  LPMDB_PAINT_VARIANT   pvar_end = pvar_beg + pv->count;
  Tpaint_variants & paint_vars = viewer_module->get_paint_vars();
  if(beg_resp)
     paint_vars.clear();
  while(pvar_beg<pvar_end)
  {
   mdb_paint_variant paint_var(pvar_beg);
   paint_vars.push_back  (paint_var);
   ++pvar_beg;
  }
  if(end_resp)
    {
     PostMessage(Handle,TLMKV_SETUP_PAINTER_DLL,0,0);
    }
}

void  __fastcall TTlmViewerMainForm::handle_paint_image   (LPVOID data,DWORD data_size,bool beg_resp,bool end_resp)
{
   if(beg_resp)
   {
      LPOTDM_PROTO_IMAGE_BEGIN ibeg = (LPOTDM_PROTO_IMAGE_BEGIN)data;
      if(!kadr ) kadr = new Tmdb_kadr_ex;
      file_begin(kadr->file,ibeg);
   }
   else
   {
     file_chunk(kadr->file,(LPOTDM_PROTO_IMAGE_CHUNK)data,end_resp);
   }
}


void __fastcall TTlmViewerMainForm::N4Click(TObject *Sender)
{
   this->Visible = true;
   WindowState = wsMaximized;
   SetFocus();
   BringWindowToTop(Handle);
}
//---------------------------------------------------------------------------



void __fastcall TTlmViewerMainForm::FormClose(TObject *Sender, TCloseAction &Action)
{
 Action = caHide;
}
//---------------------------------------------------------------------------

void __fastcall TTlmViewerMainForm::FormShow(TObject *Sender)
{
  TrayIcon1->Visible = false;
  show_kadrs(true);
  move_to_monitor(viewer_module->get_main_form_monitor());
  painter->Repaint(true);
  Repaint();
  Refresh();
}
//---------------------------------------------------------------------------

void __fastcall TTlmViewerMainForm::SpeedButton2Click(TObject *Sender)
{
  show_kadrs(false);
  Visible = false;
  TrayIcon1->Visible = true;
}
//---------------------------------------------------------------------------

void __fastcall TTlmViewerMainForm::bMainMenuClick(TObject *Sender)
{
  System::Types::TPoint pt = bMainMenu->ClientOrigin;
  pt.y += bMainMenu->Height;
  MainPopup->Popup(pt.x,pt.y);
}
//---------------------------------------------------------------------------


void __fastcall TTlmViewerMainForm::mi_opened_click(TObject *Sender)
{
 TMenuItem * item = dynamic_cast<TMenuItem*>(Sender);
 if(item && item->Tag)
 {
  TForm * f = (TForm*)(void*)item->Tag;
  f->SetFocus();

 }
}

void __fastcall TTlmViewerMainForm::MainPopupPopup(TObject *Sender)
{
  //miOpenedKadrs->Clear();
  while(miOpenedKadrs->Count)
  {
     TMenuItem * mi = miOpenedKadrs->Items[miOpenedKadrs->Count-1];
     if(mi!= this->miCloseAllKadrs)
        delete mi;
        else
        break;
  }
  TGKTlmViewer::locker l(viewer_module->get_mutex());
  Tkadrs & kadrs_ordered = viewer_module->get_kadrs_ordered();
  Tkadrs::iterator kptr = kadrs_ordered.begin();
  Tkadrs::iterator kend = kadrs_ordered.end();
  while(kptr<kend)
  {
   Tmdb_kadr_ex * kadr = *kptr;
   if(kadr->tv)
      {
       TMenuItem * item = new TMenuItem(miOpenedKadrs);
       item->Caption = kadr->tv->Caption;
       item->Tag     = (int)kadr->tv;
       item->OnClick = mi_opened_click;
       miOpenedKadrs->Add(item);
      }
   ++kptr;
  }
  miOpenedKadrs->Visible = miOpenedKadrs->Count;

}
//---------------------------------------------------------------------------

void __fastcall TTlmViewerMainForm::miCloseAllKadrsClick(TObject *Sender)
{
  close_all_kadrs(false);
}
//---------------------------------------------------------------------------

   const char layout_sw[] = "LAYOUT";
   #pragma pack(push,1)

   struct     layout_entry
   {
     DWORD kadr_id;
     int   x_pos;
     int   y_pos;
     bool  not_gray;
     bool  fixed;
     layout_entry():kadr_id(-1),x_pos(0),y_pos(0),not_gray(false),fixed(false){}
   };

   #pragma pack(pop)

   void  __fastcall TTlmViewerMainForm::do_save_layout(UnicodeString layout_name)
   {
//      UnicodeString  text;
//      text.sprintf(L"SaveLayout kadrs_ordered count = %d ",kadrs_ordered.size());
//      ShowMessage(text);

      std::vector<layout_entry> lev;
      {
       TGKTlmViewer::locker l(viewer_module->get_mutex());
       Tkadrs & kadrs_ordered = viewer_module->get_kadrs_ordered();
          Tkadrs::iterator kbeg = kadrs_ordered.begin();
          Tkadrs::iterator kend = kadrs_ordered.end  ();
          while(kbeg<kend)
          {
           Tmdb_kadr_ex * kdr = (*kbeg);
           if( kdr->tv && kdr->tv->Visible)
           {
               layout_entry le;
               le.kadr_id  = kdr->kadr_id;
               le.x_pos    = kdr->tv->Left;
               le.y_pos    = kdr->tv->Top;
               le.fixed    = kdr->tv->fixed;
               le.not_gray = kdr->tv->not_gray_bkgnd;
               lev.push_back(le);
           }
           ++kbeg;
          }
      }

//      text.sprintf(L"layout entryes count = %d ",lev.size());
//      ShowMessage(text);


      layout_name = get_layout_file(layout_name);
      KeRTL::TFileStream  fs (layout_name.c_str());
      KeRTL::TStorage storage(&fs);
      if(storage.create(0,layout_sw,strlen(layout_sw),64))
      {
         storage.clear() ;
         for(unsigned i = 0;i<lev.size();i++)
         {
           layout_entry  le = lev.at(i);
           storage.add_record(&le,sizeof(le));
         }
      }

//      text.sprintf(L"end of save layout %s",layout_name.c_str());
//      ShowMessage(text);


   }

   UnicodeString __fastcall TTlmViewerMainForm::get_layout_file(UnicodeString layout_name)
   {
    UnicodeString layout_file = viewer_module->get_data_dir();
    layout_file += layout_name;
    layout_file += L".layout";
    return layout_file;
   }

   void  __fastcall TTlmViewerMainForm::do_open_layout(UnicodeString layout_name)
   {

      layout_name = get_layout_file(layout_name);
      KeRTL::TFileStream  fs (layout_name.c_str());
      KeRTL::TStorage storage(&fs);
      DWORD ver;
      if(storage.open(layout_sw,strlen(layout_sw),&ver))
      {
        TGKTlmViewer::locker l(viewer_module->get_mutex());
        Tkadrs & kadrs_ordered = viewer_module->get_kadrs_ordered();
        close_all_kadrs(true);
        DWORD count = storage.get_record_count();
        for(DWORD i = 0;i<count;i++)
        {
          layout_entry le;
          storage.get_record(i,&le,sizeof(le));
          Tkadrs::iterator kptr;
          if(binary_search(kadrs_ordered,&Tmdb_kadr_ex(le.kadr_id),&kptr))
          {
            open_kadr(*kptr,false,false);
            TTlmViewerForm * frm = (*kptr)->tv;
            frm->Visible  = true;
            frm->Left           = le.x_pos;
            frm->Top            = le.y_pos;
            frm->not_gray_bkgnd = le.not_gray;
            frm->fixed          = le.fixed;
          }
        }
      }
   }

        void __fastcall  TTlmViewerMainForm::save_layout    (TObject * sender)
        {
//          if(params && !params->Layouts->Text.IsEmpty())
//          {
//             UnicodeString str = params->Layouts->Text;
//             do_save_layout(str);
//          }
        }

        void __fastcall  TTlmViewerMainForm::open_layout    (TObject * sender)
        {
//          if(params && !params->Layouts->Text.IsEmpty())
//          {
//               do_open_layout(params->Layouts->Text);
//               params->SetFocus();
//          }
        }

       void __fastcall  TTlmViewerMainForm::enum_layouts   (TObject * sender)
       {
//        if(params)
//        {
//             params->Layouts->Clear();
//             UnicodeString layouts_wildcard = get_layout_file(L"*");
//             TSearchRec sr;
//             if(!FindFirst(layouts_wildcard,faAnyFile,sr))
//             {
//               do{
//                   wchar_t layout_name[MAX_PATH];
//                   split_path(sr.Name.c_str(),NULL,NULL,layout_name,NULL);
//                   params->Layouts->Items->Add(layout_name);
//                 }while(!FindNext(sr));
//               FindClose(sr);
//             }
//        }
       }

 void __fastcall hot_key_vcl2winapi(WORD hot_key,DWORD & flags,DWORD & vk)
 {
   vk = LOBYTE(hot_key);
   WORD mod = HIBYTE(hot_key);
   flags = 0;

   if(mod&(0x20<<hkShift))
      flags|= MOD_SHIFT;

   if(mod&(0x20<<hkCtrl))
      flags|= MOD_CONTROL;

   if(mod&(0x20<<hkAlt))
      flags|= MOD_ALT;

 }

 bool __fastcall  TTlmViewerMainForm::hot_key_register  (int id,WORD hot_key)
 {
   UnregisterHotKey(Handle,id);
   if(hot_key)
   {
      DWORD flags,vk;
      hot_key_vcl2winapi(hot_key,flags,vk);
      return RegisterHotKey(Handle,id,flags,vk);
   }
  return true;
 }


 void __fastcall  TTlmViewerMainForm::receipt_all       ()
 {
   TGKTlmViewer::locker l(viewer_module->get_mutex());
   Trecords & records = viewer_module->get_records();
   Trecords::iterator beg = records.begin();
   Trecords::iterator end = records.end  ();
   while(beg<end)
   {
     beg->receipt();
     viewer_module->get_changed_records()[beg->rec_id,MDBR_FIELD_VALUE|MDBR_FIELD_STATE];
     ++beg;
   }
  repaint_changed_records();
 }

 void __fastcall  TTlmViewerMainForm::clear_all         ()
 {
   TGKTlmViewer::locker l(viewer_module->get_mutex());
   Trecords & records = viewer_module->get_records();
   Trecords::iterator beg = records.begin();
   Trecords::iterator end = records.end  ();
   while(beg<end)
   {
     beg->receipt();
     viewer_module->get_changed_records()[beg->rec_id,MDBR_FIELD_VALUE|MDBR_FIELD_STATE];
     ++beg;
   }
  repaint_changed_records();
 }

 void __fastcall  TTlmViewerMainForm::handle_database_change ()
 {
   TGKTlmViewer::locker l(viewer_module->get_mutex());


   close_all_kadrs (true);
   viewer_module->get_kadrs_avail().clear();
   viewer_module->get_records().clear   () ;
   viewer_module->get_names().clear     ();
   viewer_module->get_paint_vars().clear();
   painter->painter_dll(PMCMD_PAINT_VAR_CLEAR,0,0);
   Sleep(500);
   kadrs_order     (true);

   viewer_module->config_change(CCFL_KADRS|CCFL_NAMES|CCFL_RECORDS);
 }

  void __fastcall TTlmViewerMainForm::control_sound_thread(bool start)
  {
    if(start)
      {
       if(!sound_thread)
        sound_thread = new TSoundThread(this);
      }
      else
      {
        if(sound_thread)
          {
            sound_thread->Terminate();
            sound_thread->WaitFor();
            delete sound_thread;
            sound_thread = NULL;
          }
      }
  }

  void __fastcall TTlmViewerMainForm::TSoundThread::do_play_sound(UnicodeString snd)
  {
    if(!snd.IsEmpty())
       PlaySoundW(snd.c_str(),NULL,SND_FILENAME|SND_SYNC);
  }

  void __fastcall TTlmViewerMainForm::TSoundThread::Execute()
  {
    while(!this->CheckTerminated())
    {
      Sleep(100);
      if(form->get_viewer_module()->get_discrete_disparity_count())
        do_play_sound(form->get_viewer_module()->get_snd_disparity());
      if(form->get_viewer_module()->get_discrete_back_count())
        do_play_sound(form->get_viewer_module()->get_snd_back());


    }
    ReturnValue = 0;
  }


  int __fastcall build_kadrs_index(Tkadr_indexes & kindexes,Tkadrs & kadrs)
  {
     kindexes.clear();
     kindexes.reserve(kadrs.size());
     for(Tkadr_indexes::value_type n = 0; n < kadrs.size();n++ )
            kindexes.push_back(n);
     std::sort(kindexes.begin(),kindexes.end(),Tmdb_kadr_ex_less(&Tmdb_kadr_ex(),&kadrs,true));
     return kindexes.size();
  }

  void  __fastcall TTlmViewerMainForm::move_to_monitor(int mon_num)
  {
    if(mon_num<Screen->MonitorCount)
    {
     Forms::TMonitor * mon = Screen->Monitors[mon_num];
     Left  = mon->WorkareaRect.Left;
     Top   = mon->WorkareaRect.Top;
     Width = mon->WorkareaRect.Width();
     Application->ProcessMessages();
     update_ready_layout();
    }
  }

  void __fastcall TTlmViewerMainForm::update_ready_layout()
  {
     //Размещение элементов по строке готовности
     TGKTlmViewer::locker l(viewer_module->get_mutex());
     Tkadrs & kadrs_ordered = viewer_module->get_kadrs_ordered();
     Tkadr_indexes kindexes;
     int READY_SPACER = viewer_module->get_ready_spacer();
     int indent = 2;

     if(build_kadrs_index(kindexes,kadrs_ordered))
     {
       Constraints->MaxHeight = 0;
       Tkadr_indexes::iterator idx_ptr = kindexes.begin();
       Tkadr_indexes::iterator idx_end = kindexes.end  ();
       int x_pos = indent;
       int y_pos = indent;
       this->max_kadr_size.cx = max_kadr_size.cy = 0;
       KeWin::TSize ksize(0,0);
       int pp_cw = PainterPanel->ClientWidth;
       while(idx_ptr < idx_end)
       {
         Tmdb_kadr_ex * kadr = kadrs_ordered.at(*idx_ptr);
         get_kadr_extent(kadr,ksize,true);
         if((x_pos+ksize.cx+READY_SPACER) > pp_cw)
         {
            x_pos = indent;
            y_pos +=(READY_SPACER+ksize.cy);
         }
         kadr->kadr_entry.position.x = x_pos;
         kadr->kadr_entry.position.y = y_pos;
         x_pos+= ksize.cx+READY_SPACER;
         kadr->kadr_entry.size<<ksize;
         ++idx_ptr;
       }

        y_pos+=ksize.cy ;
        y_pos+=6;//std::max(6,READY_SPACER);
        int pph = PainterPanel->Height;
        pph = PainterPanel->Height;
        pph = PainterPanel->Top;
        pph*=2;

        ClientHeight = y_pos+ pph ;
        PainterPanel->Height = y_pos;
        Constraints->MaxHeight = ClientHeight;
        pph = PainterPanel->Height;
        renumber_ordered_kadrs();
        pph = PainterPanel->Height;
        pph = PainterPanel->Height;
     }
     else
     {
       //Нет кадров установить мимнимальный размер
        KeWin::TSize ksize;
        get_kadr_extent(&Tmdb_kadr_ex(0),ksize,false);
        int y_pos = ksize.cy + READY_SPACER;
        PainterPanel->Height = y_pos;
        Constraints->MaxHeight = 0;
        ClientHeight = y_pos;
        Constraints->MaxHeight = y_pos;
     }
    PainterPanelResize(this);
    painter->Repaint(true);
  }

Tmdb_kadr_ex* __fastcall TTlmViewerMainForm::find_next_kadr( Tmdb_kadr_ex * k,how_kadr_find_t how)
{
  int ready_spacer = viewer_module->get_ready_spacer();
  Tmdb_kadr_ex *ret = NULL;
  POINT pt ;
  pt.x = k->kadr_entry.position.x;
  pt.y = k->kadr_entry.position.y;
  int READY_SPACER = viewer_module->get_ready_spacer() ;
  switch(how)
  {
   case left:
        pt.y += k->kadr_entry.size.cy/2;
        pt.x -= (READY_SPACER<<1);
   break;
   case right:
        pt.y += k->kadr_entry.size.cy/2;
        pt.x += k->kadr_entry.size.cx+(READY_SPACER<<1);
   break;
   case up:
        pt.x += k->kadr_entry.size.cx/2;
        pt.y -= (READY_SPACER<<1);
   break;
   case down:
        pt.x += k->kadr_entry.size.cx/2;
        pt.y += k->kadr_entry.size.cy+(READY_SPACER<<1);
   break;
  }
  DWORD kadr_id = painter->find_entry(pt);
  if(kadr_id !=(DWORD)-1)
  {
   kadr_id = painter->get_entry_rec_id(kadr_id);
   Tmdb_kadr_ex kdr(kadr_id);
   //Tmdb_kadr_ex * kadr = NULL;
   Tkadrs::iterator ptr;
   TGKTlmViewer::locker l(viewer_module->get_mutex());
   Tkadrs & kadrs_ordered = viewer_module->get_kadrs_ordered();
   if(binary_search(kadrs_ordered,&kdr,&ptr) )
   {
     ret = *ptr;
   }
  }
   return ret;
}







void __fastcall TTlmViewerMainForm::miMoveKadrUpClick(TObject *Sender)
{
  DWORD kadr_id =  (DWORD)PopupMenu1->Tag;
  Tkadrs::iterator kptr;
     TGKTlmViewer::locker l(viewer_module->get_mutex());
     Tkadrs & kadrs_ordered = viewer_module->get_kadrs_ordered();
  if(binary_search(kadrs_ordered,&Tmdb_kadr_ex(kadr_id),&kptr))
  {
    swap_kadr_position(*kptr,find_next_kadr(*kptr,up));
  }

}
//---------------------------------------------------------------------------

void __fastcall TTlmViewerMainForm::miMoveKadrDownClick(TObject *Sender)
{
  DWORD kadr_id =  (DWORD)PopupMenu1->Tag;
  Tkadrs::iterator kptr;
     TGKTlmViewer::locker l(viewer_module->get_mutex());
     Tkadrs & kadrs_ordered = viewer_module->get_kadrs_ordered();
  if(binary_search(kadrs_ordered,&Tmdb_kadr_ex(kadr_id),&kptr))
  {
    swap_kadr_position(*kptr,find_next_kadr(*kptr,down));
  }

}
//---------------------------------------------------------------------------

struct kadr_pos_less
{
  int compare(const Tkadrs::value_type & k1,const Tkadrs::value_type& k2)
  {
    mdb_kadr_entry & ke1 = k1->get_kadr_entry();
    mdb_kadr_entry & ke2 = k2->get_kadr_entry();
    if(ke1.position.y == ke2.position.y)
       {
       return ke1.position.x - ke2.position.x;
       }
       return ke1.position.y - ke2.position.y;
  }
  bool operator()(const Tkadrs::value_type & k1,const Tkadrs::value_type& k2)
  {
    return compare(k1,k2)<0 ? true : false;
  }
};
void  __fastcall TTlmViewerMainForm::renumber_ordered_kadrs()
{
  TGKTlmViewer::locker l(viewer_module->get_mutex());
  Tkadrs & kadrs_ordered = viewer_module->get_kadrs_ordered();
  Tkadrs::iterator ptr =  kadrs_ordered.begin();
  std::vector<Tkadrs::size_type> index;
  vector_index::index_build(kadrs_ordered,kadr_pos_less(),index);
  std::vector<Tkadrs::size_type>::iterator beg = index.begin();
  std::vector<Tkadrs::size_type>::iterator end = index.end  ();
  DWORD number = 0;
  while(beg<end)
  {
    mdb_kadr_entry & ke = kadrs_ordered.at(*beg)->get_kadr_entry();
    ke.number = number++;
    ++beg;
  }


}


void __fastcall TTlmViewerMainForm::miLoginClick(TObject *Sender)
{
  if(!this->login)
  {
    login = new TLoginDlg(this);
    login->bLogin->OnClick = on_login_click;
  }
  login->Visible = true;
  login->SetFocus();
}
//---------------------------------------------------------------------------

 void __fastcall  TTlmViewerMainForm::on_login_click(TObject * sender)
 {

  char buf[8192];
  LPMPROTO_HEADER mph      = (LPMPROTO_HEADER)buf;
  memset(mph,0,sizeof(*mph));
  mph->fa             = FA_SECURITY;
  mph->internal       = MP_INTERNAL_DEFAULT;

  local_ssid = KeRTL::GetTime();
  mph->data_size  = gksecur_login_format(mph->data,sizeof(buf)-sizeof(*mph),local_ssid,login->user_name->Text.c_str(),login->user_passw->Text.c_str());
  wchar_t _usr[128],_psw[64];
  *((LPWORD)(((LPBYTE)mph->data)+mph->data_size)) = otd_calc_checksum(mph->data,mph->data_size,OTD_DEF_POLINOM);
  mph->data_size+=sizeof(WORD);
  mph->addr_to = viewer_module->get_modem_source();
  viewer_module->send_data(mph);
 }

 bool __fastcall TTlmViewerMainForm::create_painter    ()
 {
     if(TTlmViewerForm::create_painter())
     {
       mdb_kadr_ex * k = new mdb_kadr_ex(0);
       painter->set_kadr(k,0);
       return true;
     }
   return false;
 }

void  __fastcall TTlmViewerMainForm::set_ready_font (LPLOGFONT  lf)
{
  if(lf)
  {
    HFONT font =  CreateFontIndirect(lf);
    if(font)
      {
        PainterPanel->Font->Handle = font;
        font = painter->SetFont(font,true);
        if(font) DeleteObject(font);
        update_ready_layout();
      }
  }
}




