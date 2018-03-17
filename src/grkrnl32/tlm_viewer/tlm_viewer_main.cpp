//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "tlm_viewer_main.h"
#include "LayoutEdit.h"
#include <kestrm.hpp>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TTlmViewer *TlmViewer;
//---------------------------------------------------------------------------
__fastcall TTlmViewer::TTlmViewer(TComponent* Owner)
        : TForm(Owner)
{
 //Application->OnException = OnAppException;
 ZeroMemory(&appbar,sizeof(appbar));
 appbar.cbSize = sizeof(appbar);
 appbar.hWnd   = Handle;
 appbar.uCallbackMessage = WM_APPBAR_CALLBACK;
 appbar.uEdge            = ABE_TOP;
 appbar.lParam           = (LPARAM)this;
 SHAppBarMessage(ABM_NEW,&appbar);

}
//---------------------------------------------------------------------------

void __fastcall TTlmViewer::OnAppException(TObject * sender,Exception * excp)
{
 OutputDebugString(excp->Message.c_str());
}

void __fastcall TTlmViewer::CreateParams(TCreateParams &Params)
{
  TForm::CreateParams(Params);
  Params.Style   = WS_POPUP;
  Params.ExStyle = WS_EX_CLIENTEDGE|WS_EX_TOOLWINDOW |WS_EX_TOPMOST;
}

void __fastcall TTlmViewer::WndProc(Messages::TMessage &Message)
{
        //Обработка app-bar сообщений
       if(Message.Msg == WM_APPBAR_CALLBACK)
       {
        switch(Message.WParam)
        {
         case ABN_FULLSCREENAPP:
         break;
         case ABN_POSCHANGED:         break;         case ABN_STATECHANGE:         break;
         case ABN_WINDOWARRANGE:
         break;

        }
      }
      else
      {
       if( Message.Msg == WM_SYSCOMMAND &&
         ((Message.WParam&0xFFF0 ) == SC_MINIMIZE || (Message.WParam&0xFFF0 ) == SC_ICON ))
         {
          Message.Result = -1;
          return;
         }
       if(Message.Msg == WM_SHOWWINDOW && !Message.WParam)
       {
        return ;
       }
       TForm::WndProc(Message);
      } 
}


void __fastcall TTlmViewer::FormShow(TObject *Sender)
{
  Width  = Screen->Width;
  appbar.rc.right  = Screen->Width;
  appbar.rc.bottom = Height;
  appbar.uEdge     = ABE_TOP;

  SHAppBarMessage(ABM_QUERYPOS,&appbar);
  Top   = appbar.rc.top;
  Left  = 0;
  Width  = appbar.rc.right-appbar.rc.left;
  appbar.rc.bottom=appbar.rc.top+Height;

  if(!IsDebuggerPresent())
  {
  SHAppBarMessage(ABM_SETPOS,&appbar);
   SetWindowPos(appbar.hWnd, NULL, appbar.rc.left, appbar.rc.top,
     appbar.rc.right-appbar.rc.left,appbar.rc.bottom-appbar.rc.top,
     SWP_NOZORDER | SWP_NOACTIVATE);
  }   
  //MoveWindow(Handle,appbar.rc.left,appbar.rc.top,appbar.rc.right-appbar.rc.left,appbar.rc.bottom-appbar.rc.top,true);
  IPClient->Button1->OnClick = Reconnect;

}
//---------------------------------------------------------------------------

void __fastcall TTlmViewer::N2Click(TObject *Sender)
{
 Application->Terminate();
}
//---------------------------------------------------------------------------



void __fastcall TTlmViewer::DoConnectClick(TObject *Sender)
{
   term_event = CreateEventW(NULL,true,false,tlm_client::get_term_event_name().c_str());
   ResetEvent(term_event);
   //client.start_work(ServerAddr->Text.c_str(),atoi(ServerPort->Text.c_str()));
   client.start_work(IPClient->ServerAddr->Text.c_str(),atoi(IPClient->ServerPort->Text.c_str()));
   IPClient->client = &client;
   IPClient->Timer1->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TTlmViewer::DoDisconnectClick(TObject *Sender)
{
    SetEvent(term_event);
    client.TerminateAndWait(true,2500);
    CloseHandle(term_event);
    term_event = 0;
   IPClient->Timer1->Enabled = false;
   IPClient->client = NULL;

}
//---------------------------------------------------------------------------
void __fastcall TTlmViewer::Reconnect(TObject * sender)
{
 DoDisconnectClick(sender);
 clear_ready_bar();
 DoConnectClick(sender);
}

void __fastcall TTlmViewer::clear_ready_bar()
{
  for(int i = 0;i<this->ControlCount;)
  {
   TControl * ctrl = this->Controls[i];
   if(ctrl->ClassNameIs("TPaintBox"))
   {
    TPaintBox * pb = dynamic_cast<TPaintBox*>(ctrl);
    if(pb)
    {
    TPainterForm * pf = get_painter_frame(pb);
    if(pf)
      delete pf;
    delete pb;
    }
   }
   else
   ++i;
  }
}



void __fastcall TTlmViewer::N7Click(TObject *Sender)
{

  KadrsPopupMenu->Items->Clear();
  DWORD i = 0;
  mdb_kadr kadr;
  while(client.get_kadr(i,kadr,true))
  {
   ++i;
   TMenuItem * mi = new TMenuItem(this);;
   AnsiString str;
   str = client.get_text(kadr.name_id).c_str();
   if(!str.Length())
    str = client.get_text(kadr.sname_id).c_str();
   if(!str.Length())
    str.printf("Кадр № %04d ",kadr.kadr_id);
   mi->Caption = str;
   mi->Tag     = kadr.kadr_id;
   mi->OnClick = OpenKadrMenuItemClick;
   KadrsPopupMenu->Items->Add(mi);
  }
  if(i)
  {
   Types::TRect r;
   TMenuItem * mi = new TMenuItem(this);
   mi->Caption = '-';
   KadrsPopupMenu->Items->Add(mi);
   mi = new TMenuItem(this);
   mi->Caption = "Изменить расположение";
   mi->OnClick = ChangeLayout;
   KadrsPopupMenu->Items->Add(mi);

   mi = new TMenuItem(this);
   mi->Caption = "Сохранить расположение";
   mi->OnClick = SaveLayout;
   KadrsPopupMenu->Items->Add(mi);

   GetMenuItemRect(Handle,MainMenu1->Handle,N7->MenuIndex,&r);
   KadrsPopupMenu->Popup(r.left,r.top+r.Height());
  }
}
//---------------------------------------------------------------------------

TPaintBox    * __fastcall TTlmViewer::find_kadr_frame(DWORD kadr_id)
{
 for(int i = 0;i<this->ComponentCount;i++)
 {
  TComponent * comp = Components[i];
  if(comp->ClassNameIs("TPaintBox"))
  {
   TPaintBox    * pb = dynamic_cast<TPaintBox*>(comp);
   TPainterForm * pf = get_painter_frame(pb);
   if(pf && pf->get_kadr()->kadr_id == kadr_id)
      return pb;
  }
 }
 return NULL;
}

TPainterForm * __fastcall TTlmViewer::get_painter_frame(TPaintBox * pb)
{
  if(pb)
    return reinterpret_cast<TPainterForm *>(pb->Tag);
   return NULL;
}

void   __fastcall TTlmViewer::OpenKadrMenuItemClick(TObject * sender)
{
  /*Открытие кадра*/
  //Проверить не открыт ли кадр
  TMenuItem * mi = dynamic_cast<TMenuItem*>(sender);
  if(mi)
  {
   DWORD kadr_id = mi->Tag;

   TPaintBox    * pb = find_kadr_frame(kadr_id);
   if(!pb)
   {

   TPainterForm * pf = new TPainterForm(this,&client);
   pb = new TPaintBox(this);
   int rw = get_ready_width();
   mdb_kadr kdr;
   client.get_kadr(kadr_id,kdr,false);
   string kdr_name  = client.get_text(kdr.sname_id);

   pf->set_ready_painter(pb);
   pb->OnPaint = pf->PaintReady;

   pf->Top  = Height;
   pf->Left = (Screen->Width-Width)/2;
   pf->get_painter()->set_getter_record(get_record,&client);
   pf->get_painter()->set_getter_text(get_text,&client);
   client.order_kadr(mi->Tag,pf->Handle); // Подписка на кадр
   pf->Visible   = true;
   pb->Tag       = (int)pf;
   pb->Width     = KeRTL::MAX(80,(kdr_name.length() ? Canvas->TextWidth(kdr_name.c_str())+20:80)) ;
   pb->Left      = rw;
   //pb->Align     = alLeft;
   pb->OnClick   = ReopenKadr;
   pb->OnMouseUp = OnReadyMouseUp;
   InsertControl(pb);
   }
   else
   {
    this->ReopenKadr(pb);
   }
  }

}

void   __fastcall TTlmViewer::ReopenKadr(TObject * sender)
{
 /*Переоткрытие кадра*/
 TPaintBox * pb = dynamic_cast<TPaintBox*>(sender);
 if(pb)
 {

  TPainterForm * pf = (TPainterForm *) pb->Tag;
  if(pf)
  {

   if(IsIconic(pf->Handle))
      SendMessage(pf->Handle,WM_SYSCOMMAND,SC_RESTORE,0);
   pf->Visible = true;
   pf->SetFocus();

  }
 }
}



wstring __stdcall TTlmViewer::get_text  (LPVOID form_ptr,TTlmPainter * painter,DWORD txt_id)
{
 wstring ret;
 tlm_client * client = reinterpret_cast<tlm_client *>(form_ptr);
 if(client)
 {
  ret = client->get_wtext(txt_id);
 }
 return ret;
}

DWORD  __stdcall TTlmViewer::get_record(LPVOID form_ptr,TTlmPainter * painter ,mdb_record_ex * rec)
{
 DWORD  ret = 0;
 tlm_client * client = reinterpret_cast<tlm_client *>(form_ptr);
 if(client)
 {
  ret =(DWORD) client->get_record(rec->rec_id,*rec,false);

 }
 return ret;

}


void __fastcall TTlmViewer::ConnPropsClick(TObject *Sender)
{
  IPClient->Visible = true;
  IPClient->SetFocus();
}
//---------------------------------------------------------------------------



void __fastcall TTlmViewer::FormClose(TObject *Sender,
      TCloseAction &Action)
{
  SHAppBarMessage(ABM_REMOVE,&appbar);
}
//---------------------------------------------------------------------------


int  __fastcall TTlmViewer::get_ready_width()
{
  int ret = 0;
  for(int i = 0;i<this->ComponentCount;i++)
   {
    TComponent * comp = Components[i];
    if(comp->ClassNameIs("TPaintBox"))
    {
     TPaintBox    * pb = dynamic_cast<TPaintBox*>(comp);
     if(pb->Parent == this)
        ret+=pb->Width;
    }
   }
   return ret;
}

void __fastcall TTlmViewer::FormDestroy(TObject *Sender)
{
  SHAppBarMessage(ABM_REMOVE,&appbar);
  //client.TerminateAndWait(true);
   /*for(int i = 0;i<this->ComponentCount;i++)
   {
    TComponent * comp = Components[i];
    if(comp->ClassNameIs("TPaintBox"))
    {
     TPaintBox    * pb = dynamic_cast<TPaintBox*>(comp);
     delete pb;

    }
   }*/

}
//---------------------------------------------------------------------------

void __fastcall TTlmViewer::FormActivate(TObject *Sender)
{
  SHAppBarMessage(ABM_ACTIVATE,&appbar);

}
//---------------------------------------------------------------------------

void __fastcall TTlmViewer::FlashTimerTimer(TObject *Sender)
{
  //Отрисовка мигания
 ++TTlmPainter::flash_counter;
 for(int i = 0;i<this->ComponentCount;i++)
 {
  TComponent * comp = Components[i];
  if(comp->ClassNameIs("TPaintBox"))
  {
   TPaintBox    * pb = dynamic_cast<TPaintBox*>(comp);
   TPainterForm * pf = get_painter_frame(pb);
   if(pf )
   {
    if(pf->Visible && !IsIconic(pf->Handle))
       pf->flash_repaint();
   }

  }
 }


}
//---------------------------------------------------------------------------

int  __fastcall TTlmViewer::find_active_ready()
{
 for(int i = 0;i<this->ControlCount;i++)
 {
  TControl * comp = Controls[i];
  if(comp->ClassNameIs("TPaintBox"))
  {
   TPaintBox    * pb = dynamic_cast<TPaintBox*>(comp);
   TPainterForm * pf = get_painter_frame(pb);
   if(pf && pf->Focused())
      return i;
  }
 }
 return -1;
}

void           __fastcall TTlmViewer::OnReadyMouseUp(TObject * sender,TMouseButton mbtns,Classes::TShiftState shift,int X, int Y)
{
  if(mbtns == mbLeft)
     ReopenKadr(sender);
}

void __fastcall TTlmViewer::SaveLayout  (TObject * sender)
{
 //Сохранение  расположения
 AnsiString cn("TPaintBox");
 string str = client.get_data_dir();
 str += "layout.cfg";
 KeRTL::TFileStream fstrm( str.c_str());
 fstrm.Rewind();
 DWORD rd_cnt = 0;
 fstrm<<rd_cnt;
 for(int i = ControlCount;i;)
 {
  TControl * ctrl = Controls[--i];
  if(ctrl->ClassNameIs(cn))
  {
    TPaintBox    * pb    = dynamic_cast<TPaintBox*>(ctrl);
    TPainterForm * pf    = get_painter_frame(pb);
    fstrm<<pf->get_kadr()->kadr_id ;
    ++rd_cnt ;
  }
 }
 fstrm.Seek(0,KeRTL::TStream::smFromBegin);
 fstrm<<rd_cnt;
 fstrm.Flush();
 
}

void __fastcall TTlmViewer::ChangeLayout(TObject * sender)
{
 //Изменение  расположения
 TLaoutEdit * le = NULL;
 AnsiString cn("TPaintBox");
 for(int i = 0;i<this->ControlCount;i++)
 {
  TControl * ctrl = Controls[i];
  if(ctrl->ClassNameIs(cn))
   {
    if(!le)
       le = new TLaoutEdit(this);
    TListItem * item = le->ListView1->Items->Insert(0);
    TPaintBox    * pb    = dynamic_cast<TPaintBox*>(ctrl);
    TPainterForm * pf = get_painter_frame(pb);
    item->Caption = pf->get_kadr()->kadr_id;
    item->SubItems->Add(pf->get_kadr_name());
    item->Data = (LPVOID) pb;
    item->Checked = true;
   }
 }

 if(le  )
 {
  if(le->ShowModal()== mrOk )
  {
   //Чичас удаляем контролы
  for(int i = 0;i<this->ControlCount;)
  {
  TControl * ctrl = Controls[i];
  if(ctrl->ClassNameIs(cn))
   {
    RemoveControl(ctrl);
   }
   else
   ++i;
  }
   for(int i = le->ListView1->Items->Count;i;)
   {
    TListItem    * item = le->ListView1->Items->Item[--i];
    TPaintBox    * pb = reinterpret_cast<TPaintBox*>(item->Data);
    TPainterForm * pf = get_painter_frame(pb);
    if(item->Checked)
    {
     pb->Left  = get_ready_width();
     InsertControl (pb);
     //pb->Align = alLeft;
    } 
    else
    {
     //Отказ от подписки
     client.unorder_kadr(pf->get_kadr()->kadr_id,pf->Handle);
     delete pf;
     delete pb;
    }

   }
  }
  delete le;
 }
}


void __fastcall TTlmViewer::N5Click(TObject *Sender)
{
 ShellAbout(Handle,"ОУИК Центр АРМ","Компания промышленной автоматизации.\n\rСанкт-Петербург 2006 Остапенко Д.В. ",Application->Icon->Handle);
}
//---------------------------------------------------------------------------


