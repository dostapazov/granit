//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "tlm_painter_frm.h"
#include <stdio.h>
#include <mmsystem.h>


//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TPainterForm *PainterForm;
//---------------------------------------------------------------------------
__fastcall TPainterForm::TPainterForm(TComponent* Owner,tlm_client * cl)
        : TForm(Owner),
        client(cl),
          Fpainter(NULL)
{
  if(!TTlmPainter::painter_dll.is_loaded())
      TTlmPainter::painter_dll.load_module("tlmobj_std_painter.dll");

}
//---------------------------------------------------------------------------



void   __fastcall TPainterForm::CreateParams(TCreateParams & cp)
{
 TForm::CreateParams(cp);
 cp.Style|=WS_CLIPCHILDREN;
}


void    __fastcall TPainterForm::open_kadr(mdb_kadr & kadr)
{
  painter->enable_edit(false);
  painter->set_kadr(&kadr);
  if(Fpainter() == NULL)
  {
   //Создание экземпляра окна
   KeWin::TRect r(0,0,kadr.kadr_size.cx,kadr.kadr_size.cy);
   painter->Create(Panel1->Handle,"",r,-1,NULL,WS_VISIBLE|WS_CHILD|WS_TABSTOP,WS_EX_CONTROLPARENT);
   //painter->SetFlags(FLWND_DOUBLE_BUFFERED,true);
   painter->SetFont(Panel1->Font->Handle,false);
   painter->set_on_select(on_select_entry,this);
   painter->set_on_mouse_over(on_mouse_over_entry,this);

  }
  selected_num = -1;
  update_size(kadr.kadr_size.cx,kadr.kadr_size.cy);
}

void __fastcall TPainterForm::ShowEntryInfo(DWORD ent_num)
{

   AnsiString str ;
   if(ent_num!=(DWORD)-1 )
    {

     mdb_kadr_entry ke;
     mdb_record_ex  rec;
     wstring        name;
     painter->get_kadr_entry(ent_num,ke);
     painter->get_record(ke.rec_id,rec);
     name = painter->get_wtext(rec.name);
     if(!name.length())
     name = painter->get_wtext(rec.sname);
     if(name.length())
        str=name.c_str();
        else
        {
         str.printf("sn_id = %d n_id = %d",rec.sname,rec.name);
        }
     SYSTEMTIME st;
     FileTimeToSystemTime((LPFILETIME)&rec.time,&st);
     str.cat_printf(" { ID=%d %0.2f %02d-%02d-%04d  %02d:%02d:%02d.%03d",rec.rec_id,rec.value,(int)st.wDay,(int)st.wMonth,(int)st.wYear,(int)st.wHour,(int)st.wMinute,(int)st.wSecond,(int)st.wMilliseconds);

     str.cat_printf("D = %04X S = %04X }",rec.diag,rec.state);

    }
    StatusBar1->Panels->Items[0]->Text = str;
}

DWORD WINAPI TPainterForm::on_mouse_over_entry(LPVOID data,TTlmPainter * p_from,DWORD entry_num)
{
 TPainterForm * form = reinterpret_cast<TPainterForm *>(data);
 if(form)
 {
  if(entry_num!=(DWORD)-1)
  {
   AnsiString hint;
   hint.printf("Номер элемента %d",entry_num);
   form->Hint = hint;
   Types::TPoint p;
   GetCursorPos(&p);
   Application->ShowHint = true;
   Application->ActivateHint(p);

  }
  else
  {
   form->ShowHint = false;
   Application->ShowHint = false;

  }

  form->ShowEntryInfo((entry_num != (DWORD)-1)?entry_num:form->selected_num);
 }
 return TRUE;
}


void   __fastcall TPainterForm::update_commands()
{
    if(painter->get_selected_count() == 1)
    {
     mdb_record_ex     rec;
     rec.rec_id = painter->get_selected_recid(0);
     painter->get_record(rec.rec_id,rec);
     if(rec.state&MDBR_STATE_TUFLAG) 
     {
      if(rec.state&MDBR_STATE_TU)
       {
        tu_enabled = false;
        SendTUCancel->Enabled = true;
       }
       else
       {
        tu_enabled = true;
        SendTUCancel->Enabled = false;
       } 
     }
     else
     tu_enabled = false;
    }
    else
     tu_enabled = false;
}

DWORD WINAPI TPainterForm::on_select_entry(LPVOID data,TTlmPainter  * p_from,DWORD ent_num)
{
  TPainterForm * form = reinterpret_cast<TPainterForm *>(data);
  if(form)
  {
    form->selected_num = ent_num;
    form->ShowEntryInfo(ent_num);
    form->update_commands();
  }
  return TRUE;
}

void __fastcall TPainterForm::set_tu_enabled(bool val)
{
 SendTUOff->Enabled = val;
 SendTUOn->Enabled = val;
 SendTUCancel->Enabled = val;
}


void __fastcall TPainterForm::OnCmdSetBkgnd(TMessage & msg)
{
        //TODO: Add your source code here
        Fpainter.set_bkgnd(string((char*)msg.LParam));

}


void __fastcall TPainterForm::OnCmdSetCaption(TMessage & msg)
{
        //TODO: Add your source code here

        char * capt_text = (char*) msg.LParam;
        if(capt_text)
        Caption = capt_text;

}

void __fastcall TPainterForm::OnCmdUpdateKadr(TMessage & msg)
{
        //TODO: Add your source code here

        if(msg.LParam)
        {
         kadr = *(mdb_kadr*)msg.LParam;
         open_kadr(kadr);
        }
        Visible = true;


}
void __fastcall TPainterForm::Panel1Resize(TObject *Sender)
{
  ;
}
//---------------------------------------------------------------------------


void __fastcall TPainterForm::OnCmdSetEntryes(TMessage & msg)
{

        //TODO: Add your source code here
        painter->clear_entryes();
        entryes_list::iterator beg = (entryes_list::iterator)msg.LParam;
        entryes_list::iterator end = beg;
        advance(end,msg.WParam);
        while(msg.WParam && beg<end)
        {
         painter->add_entry(*beg);
         ++beg;
        }
        /*
        AnsiString str;
        str.printf("Элементов в кадре %d размеры кадра %d,%d",msg.WParam,this->kadr.kadr_size.cx,kadr.kadr_size.cy);
        StatusBar1->Panels->Items[1]->Text = str;
        */


}

void __fastcall TPainterForm::OnCmdRepaintKadr(TMessage & msg)
{
        //TODO: Add your source code here
        DWORD ch_mask = msg.WParam;
        mdb_kadr * _kadr = (mdb_kadr *)msg.LParam;
        if(_kadr)
        {
         kadr = *_kadr;
         PostMessage(Handle,msg.Msg,msg.WParam,NULL);
        }
        else
        {
        if(ch_mask)
           {
            painter->set_kadr(&kadr,ch_mask);
           }

        if(ch_mask&MDB_KADR_FIELD_SIZE)
          update_size(kadr.kadr_size.cx,kadr.kadr_size.cy);

        if(ch_mask & MDB_KADR_FIELD_NAME)
        {
         AnsiString str = painter->get_wtext(kadr.name_id).c_str();
         if(!str.Length())
           str =  painter->get_wtext(kadr.sname_id).c_str();
         Caption =str;

        }
        if(this->ready_painter)
           {
            ready_painter->Refresh();
           }
         update_status_bar();
         ShowEntryInfo(selected_num);
         update_commands();
         painter->Repaint(false);
         if(kadr.discrete_changes)
          {
            ::PlaySound("TSSW.WAV",NULL,SND_FILENAME|SND_ASYNC);
          }
        }
}



 void   __fastcall TPainterForm::update_status_bar()
 {
  AnsiString str;
  /*
  str.printf("D = %04X S = %04X DCH = %04X CH = %04X",(DWORD)kadr.kadr_diag,
                                                       (DWORD)kadr.kadr_state,
                                                       (DWORD)kadr.discrete_changes,
                                                       (DWORD)kadr.kadr_changes);
  StatusBar1->Panels->Items[0]->Text = str;
  str.printf("Элементов в кадре %d размеры кадра %d,%d",painter->get_entry_count(),(DWORD)kadr.kadr_size.cx,(DWORD)kadr.kadr_size.cy);
  StatusBar1->Panels->Items[1]->Text = str;
  */
 }

 void   __fastcall  TPainterForm::update_size(int cx,int cy)
 {

          AutoScroll = false;
          Panel1->Left = 0;
          Panel1->Top  = CoolBar1->Height;

          Panel1->Width  = cx;
          Panel1->Height = cy;

          int newWidth  =  cx  + (GetSystemMetrics(SM_CXBORDER)<<1);
          int newHeight =  cy  +  GetSystemMetrics (SM_CYCAPTION)+
                          (GetSystemMetrics(SM_CYBORDER)<<2)+
                    StatusBar1->Height+
                    CoolBar1->Height;
          if(newWidth>Screen->Width)
             {
              newWidth=Screen->Width;
              AutoScroll = true;
             }
          if(newHeight>Screen->Height)
          {
             newHeight=Screen->Height;
              AutoScroll = true;
          }
          Width =  newWidth;
          Height = newHeight;
          painter->SetSize(cx,cy);
 }

 void   __fastcall  TPainterForm::set_ready_painter(TPaintBox * pb)
 {

  ready_painter =pb;
  ready_painter->Tag = (int)this;
 }

AnsiString __fastcall TPainterForm::get_kadr_name()
{

  AnsiString name = Fpainter.get_wtext(this->kadr.sname_id).c_str();
  if(!name.Length())
   name.printf("Кадр ID %d",kadr.kadr_id);
  return name;

}


void   __fastcall TPainterForm::PaintReady(TObject * sender)
{
 /*Отрисовка готовности*/
 TPaintBox * pb = ready_painter;

   RECT r = pb->ClientRect;

   bool is_focused = Screen->ActiveForm == this ? true:false;
   DWORD edge_flags;
   if(is_focused)
      edge_flags = EDGE_SUNKEN;
      else
      edge_flags =EDGE_RAISED;

   if(!DrawEdge(pb->Canvas->Handle,&r,edge_flags,BF_RECT) )
        GetLastError();

   mdb_kadr * kadr = get_kadr();

    if((Fpainter.flash_counter&1) || kadr->discrete_changes == 0 )
    {
    // Мигание готовностью по изменениям
    AnsiString name;
    name   = get_kadr_name();
    if(kadr->kadr_diag)
    pb->Canvas->Font->Color = clBlack;
    else
    pb->Canvas->Font->Color = clGreen;

    int x = (pb->Width-pb->Canvas->TextWidth(name))/2;
    pb->Canvas->TextOut(x,2,name);
    }
    pb->Canvas->Font->Color = clDefault;
    /*
    ОТрисовка ТУ
    */
    int top =  2+pb->Canvas->TextHeight("T");
    pb->Canvas->Font->Color = clBlue;
    if(kadr->kadr_state&MDBR_STATE_TU)
    {
     if((Fpainter.flash_counter&1))
     {
     //Один из объектов находится в состоянии ТУ
      ImageList1->Draw(pb->Canvas,2,top,0,true);
      //pb->Canvas->TextOut(2,top,"Ту");
     }
    }


    if(kadr->kadr_state&MDBR_STATE_TUERROR)
    {
     //Один из объектов находится в состоянии ошибка ТУ
     //pb->Canvas->Font->Color = clRed;
     //pb->Canvas->TextOut(2,top,"ТуОш");
     ImageList1->Draw(pb->Canvas,18,top,1,true);
    }






}



void __fastcall TPainterForm::WndProc(TMessage & msg)
{
 switch(msg.Msg)
 {
  case   TC_CMD_SET_BKGND     :  OnCmdSetBkgnd(msg)   ;break;
  case   TC_CMD_SET_CAPTION   :  OnCmdSetCaption(msg) ;break;
  case   TC_CMD_UPDATE_KADR   :  OnCmdUpdateKadr(msg) ;break;
  case   TC_CMD_SET_ENTRYES   :  OnCmdSetEntryes(msg) ;break;
  case   TC_CMD_REPAINT_KADR  :  OnCmdRepaintKadr(msg);break;
  //case MSG_UPDATE_STATUS_BAR:  OnMsgUpdateSB(msg)   ;break;
  case   WM_SYSCOMMAND:
         if(msg.WParam == SC_MINIMIZE) Fpainter.release_bkgnd_bitmaps();
           else
           if(msg.WParam == SC_RESTORE) Fpainter.restore_bkgnd_bitmaps();

  default : TForm::WndProc(msg);break;
 }
}

void __fastcall TPainterForm::flash_repaint()
{
  Fpainter.flash_repaint();
  ready_painter->Repaint();
}

void __fastcall TPainterForm::FormDestroy(TObject *Sender)
{
 Fpainter.Destroy();
}
//---------------------------------------------------------------------------


void __fastcall TPainterForm::FormHide(TObject *Sender)
{
    Fpainter.release_bkgnd_bitmaps();
}
//---------------------------------------------------------------------------

void __fastcall TPainterForm::FormShow(TObject *Sender)
{
  this->Fpainter.restore_bkgnd_bitmaps();
}
//---------------------------------------------------------------------------


void __fastcall TPainterForm::FormActivate(TObject *Sender)
{
  ready_painter->Refresh();
}
//---------------------------------------------------------------------------

void __fastcall TPainterForm::FormDeactivate(TObject *Sender)
{
  this->ready_painter->Refresh();
}
//---------------------------------------------------------------------------

void __fastcall TPainterForm::SpeedButton1Click(TObject *Sender)
{
  //kadr.discrete_changes = 0;
  //kadr.kadr_changes     = 0;
  int sc  = painter->get_selected_count();
  if(sc)
  {
   for(int i = 0;i<sc;)
   {
    client->receipt(painter->get_selected_recid(i),sc == (i+1) ? true:false);
    i++;
   } 
  }
  else
  client->receipt_kadr(this->kadr.kadr_id);
  ready_painter->Repaint();
  update_status_bar();
}
//---------------------------------------------------------------------------
void __fastcall TPainterForm::do_send_tu(DWORD op)
{
  //Отправка команды по выбранному 
  client->send_tu(painter->get_selected_recid(0),op);
}
//---------------------------------------------------------------------------

void __fastcall TPainterForm::SendTUOnClick(TObject *Sender)
{
  do_send_tu(OTD_TUOP_ON);
}
//---------------------------------------------------------------------------

void __fastcall TPainterForm::SendTUOffClick(TObject *Sender)
{
  do_send_tu(OTD_TUOP_OFF);
}
//---------------------------------------------------------------------------

void __fastcall TPainterForm::SendTUCancelClick(TObject *Sender)
{
 do_send_tu(OTD_TUTROP_CANCEL);
}
//---------------------------------------------------------------------------

