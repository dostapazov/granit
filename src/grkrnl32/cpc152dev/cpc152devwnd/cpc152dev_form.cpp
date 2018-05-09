//---------------------------------------------------------------------------
#pragma hdrstop
#include <vcl.h>
#include "cpc152dev_form.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "gkmodule_form"
#pragma link "cpc152_mod_frame"
#pragma link "gkmod_frame"
#pragma link "cpc152_dev_frame"
#pragma link "about_common_frame"
#pragma resource "*.dfm"
TCpc152DevFrm *Cpc152DevFrm;
//---------------------------------------------------------------------------
__fastcall TCpc152DevFrm::TCpc152DevFrm(TComponent* Owner)
	: TGKModuleForm(Owner)
{
}
//---------------------------------------------------------------------------
__fastcall TCpc152DevFrm::TCpc152DevFrm(TComponent* Owner,HWND parent,GKHANDLE h)
	: TGKModuleForm(Owner,parent,h)
{
}
//---------------------------------------------------------------------------

void        __fastcall TCpc152DevFrm::setup_form_sizes()
{
 TGKModuleForm::setup_form_sizes();
 Cpc152Tree->Width = Width/4;
}


void     __fastcall TCpc152DevFrm::begin_call_gkhandle(wchar_t * cmd_name)
{
 StatusBar1->Panels->Items[0]->Text =  cmd_name;
 StatusBar1->Panels->Items[1]->Text =  AnsiString();
}

void     __fastcall TCpc152DevFrm::done_call_gkhandle(LRESULT ret,wchar_t * str)
{
 DWORD last_error = ret ? GetLastError():0;
 if(str)
 {
  StatusBar1->Panels->Items[0]->Text = str;
 }
 switch(ret)
 {

  case GKH_RET_SUCCESS:
  case GKH_RET_ERROR  :
                      {
                      wchar_t  err_text[MAX_PATH];
                      mod_iface.get_error_text(last_error,err_text,sizeof(err_text)/sizeof(wchar_t));
                      StatusBar1->Panels->Items[1]->Text = AnsiString(err_text);
                      }
                      break;


 }
}

void     __fastcall TCpc152DevFrm::on_module_config_change(GKHANDLE mod,LPMODULE_CONFIG_DATA mcd)
{

}

void     __fastcall TCpc152DevFrm::after_set_gkhandle()
{
   Cpc152Tree->Items->Clear();
   UnicodeString node_text = L"CPC152 модем";
   root_node = Cpc152Tree->Items->Add(NULL,node_text);
   in_setup = 0;
   TGKModuleForm::after_set_gkhandle();
   Cpc152mod_frame1->mod_handle = mod_handle;
   cpc152dev_frame1->mod_handle = mod_handle;
   Application->ProcessMessages();
   on_module_state(mod_iface(),mod_iface.get_module_state());
   set_notify(mod_iface(),MNF_COMMON|MNF_MODEM|MNF_NOTIFY_CPC152_ALARM_START_ADD|MNF_NOTIFY_CPC152_ALARM_START_DEL);
   PostMessage(Handle,WMUSER_SETUP_LINES_TREE,0,0);
   PageControl1->ActivePageIndex = 0;
}

void __fastcall TCpc152DevFrm::WndProc(Messages::TMessage &msg)
{
   switch(msg.Msg)
   {
     case WMUSER_SETUP_LINES_TREE:
     this->setup_lines_tree();msg.Result = 0;
     break;
     default:
     TGKModuleForm::WndProc(msg);
     break;
   }
}

TTreeNode  *     __fastcall TCpc152DevFrm::find_line_node (DWORD line_num)
{
    TTreeNode * ret = NULL;
    for(int i = 0;i<root_node->Count && !ret;i++)
    {
      TTreeNode * node = root_node->Item[i];
      if(line_num == (DWORD)node->Data)
         ret = node;
    }
    return ret;
}


void     __fastcall TCpc152DevFrm::set_node_image_index (TTreeNode * node,DWORD line_state)
{
 if(node)
 {
   DWORD idx = LINE_IMAGE_DISCONNECT;
   if(line_state&MODEM_LINE_STATE_CONNECT)
      idx = LINE_IMAGE_CONNECT;
      else
      {
       if(line_state&MODEM_LINE_STATE_CONNECTING)
         idx = LINE_IMAGE_CONNECTING;
      }
  node->ImageIndex     = idx;
  node->SelectedIndex  = idx;
  node->StateIndex     = idx;
 }
}

void     __fastcall TCpc152DevFrm::on_line_state        (const LPMODEM_LINE_STATE mls)
{
   TTreeNode * node = find_line_node((DWORD)mls->addr.line);
   if(node)
   {
    if(mls->line_state & MODEM_LINE_STATE_EXIST)
    {
    if(mls->state_changes)
       set_node_image_index(node,mls->line_state);
    }
    else
     node->Delete();
   }
   else
   {
     if(mls->line_state & MODEM_LINE_STATE_EXIST)
     {

       BYTE buf[1024];
       bzero(buf,sizeof(buf));
       LPMODEM_LINE_PROPS mlp = (LPMODEM_LINE_PROPS)buf;
       mlp->dw_size = sizeof(buf);
       mlp->line_text_sz = sizeof(buf)-sizeof(*mlp);
       if(GKH_RET_SUCCESS==mod_iface.call(MODEMCM_GET_LINE_PROPS,mls->addr.line,(LPARAM)mlp))
          {
           this->add_line_node(mlp->addr.line,mlp->line_state);
           //add_iec60870_line(mlp);
          }
     }
   }
}


LRESULT __fastcall TCpc152DevFrm::on_gkhandle_notify (GKHANDLE from,LPNOTIFY_CODE nc,DWORD mask)
{
   LRESULT ret = GKH_RET_SUCCESS;
   switch(mask)
   {
     case     MNF_NOTIFY_CPC152_ALARM_START_ADD:
     case     MNF_NOTIFY_CPC152_ALARM_START_DEL:

     if(current_frame == this->cpc152dev_frame1 && (DWORD)cpc152dev_frame1->get_cp_number() == nc->notify_code)
     {

        cpc152dev_frame1->on_alarm_start_notify((LPCPC152_ALARM_START)nc->notify_data
                                                 ,mask == MNF_NOTIFY_CPC152_ALARM_START_ADD ? true : false
                                               );
     }
     break;
     case MNF_MODEM:
     {
      switch(nc->notify_code)
      {
       //case MNCODE_MODEM_NUMBER: on_modem_number((LPMODEM_CHANGE_NUMBER)nc->notify_data);break;
       case MNCODE_LINE_STATE  : on_line_state  ((LPMODEM_LINE_STATE)nc->notify_data);break;
      }
     }
     break;
     default:
       ret =  TGKModuleForm::on_gkhandle_notify(from,nc,mask);
     break;
   }
  return ret;
}



void     __fastcall TCpc152DevFrm::on_module_state(GKHANDLE mod,DWORD state)
{
  bool running = ((state & MODULE_STATE_RUNNING_MASK )== MODULE_STATE_RUNNING) ? true:false;
  bool stopped  = (state & MODULE_STATE_RUNNING_MASK ) ? false:true;
  tbStop->Enabled      = running;
  tbStart->Enabled     = stopped;
  if(running)
     StatusBar1->Panels->Items[0]->Text = "Активен";
  if(stopped)
     StatusBar1->Panels->Items[0]->Text = "Остановлен";

}

void __fastcall TCpc152DevFrm::tbStartClick(TObject *Sender)
{
  done_call_gkhandle(mod_iface.start(MODULE_START_REQUEST,0),L"Запуск модуля");
}
//---------------------------------------------------------------------------
void __fastcall TCpc152DevFrm::tbStopClick(TObject *Sender)
{
  done_call_gkhandle(mod_iface.stop(MODULE_STOP_REQUEST),L"Останов модуля");
}
//---------------------------------------------------------------------------

void     __fastcall TCpc152DevFrm::set_current_frame(TGKModuleFrame * value)
{
  if(Fcurrent_frame)
     Fcurrent_frame->Visible = false;
  Fcurrent_frame = value;
  if(Fcurrent_frame)
  {
     Fcurrent_frame->Align   = alClient;
     Fcurrent_frame->Visible = true;
  }

}

void __fastcall TCpc152DevFrm::Cpc152TreeChanging(TObject *Sender, TTreeNode *Node,
          bool &AllowChange)
{
   if(current_frame)
      current_frame->current_object_changed(Node,AllowChange);
}
//---------------------------------------------------------------------------
void __fastcall TCpc152DevFrm::Cpc152TreeChange(TObject *Sender, TTreeNode *Node)

{
   if(current_frame)
      current_frame->current_object = NULL;
   if(Node)
   {
   if(Node == root_node)
       current_frame = Cpc152mod_frame1;
      else
       current_frame = cpc152dev_frame1;
       current_frame->on_changed     = on_frame_changed;
       current_frame->mod_handle     = mod_handle;
       current_frame->current_object = Node;
       current_frame->ch_mask        = 0;

   }else
   current_frame = 0;
   PageControl1Change(this);
   tbDelete->Enabled = current_frame && Node !=root_node ? true:false;
}
//---------------------------------------------------------------------------
void __fastcall TCpc152DevFrm::tbApplyClick(TObject *Sender)
{
 if(current_frame)
 {
   begin_call_gkhandle(L"Применение изменений");
   bool success = current_frame->apply_changes() ;
   if(success)
	   current_frame->ch_mask = 0;
   done_call_gkhandle(success ? 0 : GKH_RET_ERROR,NULL);
 }
}
//---------------------------------------------------------------------------

void     __fastcall TCpc152DevFrm::on_frame_changed (TObject * sender)
{
  TGKModuleFrame * mf = dynamic_cast<TGKModuleFrame*>(sender);
  if(mf)
  {
   tbApply->Enabled  = mf->ch_mask ? true:false;
   tbUndo->Enabled   = tbApply->Enabled;
  }
}

void __fastcall TCpc152DevFrm::tbNewClick(TObject *Sender)
{
 // Create new cpc152 controller-connection
 TTreeNode * node = this->Cpc152Tree->Items->AddChild(root_node,L"Новое подключение..");
 node->Data      = (LPVOID )-1;
 node->Selected  = true;
}
//---------------------------------------------------------------------------

void __fastcall TCpc152DevFrm::tbDeleteClick(TObject *Sender)
{
  TTreeNode * node = Cpc152Tree->Selected;
  if(node && node != root_node &&
     IDYES == MessageBox(Handle,UnicodeString(UnicodeString(L"Удалить ") +node->Text + UnicodeString(L" ?" )).c_str(),L"Запрос на удаление",MB_YESNO|MB_DEFBUTTON2)
     )
  {
   current_frame->delete_current();
  }
}
//---------------------------------------------------------------------------


void __fastcall TCpc152DevFrm::Cpc152TreeCollapsing(TObject *Sender, TTreeNode *Node,
          bool &AllowCollapse)
{
  AllowCollapse = Node!= root_node ? true : false;
}
//---------------------------------------------------------------------------

TTreeNode * __fastcall TCpc152DevFrm::add_line_node(DWORD line_num,DWORD line_state)
{
 CPC152_LINE_CONFIG lc;
 bzero(&lc,sizeof(lc));
 lc.dw_size  = sizeof(lc);
 lc.mask = -1;
 lc.cp_number = line_num;
 Tcpc152dev_frame::read_line_config(&mod_iface,&lc);
 UnicodeString ltext = Tcpc152dev_frame::get_line_text(&lc);
 TTreeNode  * node = Cpc152Tree->Items->AddChild(root_node,ltext);
 node->Data = (LPVOID)lc.cp_number;
 this->set_node_image_index(node,line_state);
 return node;
}

void __fastcall TCpc152DevFrm::setup_lines_tree()
{
 MODEM_LINE_PROPS mlp;
 bzero(&mlp,sizeof(mlp));
 mlp.dw_size = sizeof(mlp);

 DWORD result = GKH_RET_SUCCESS;
 for(int i = 0; i<255 && GKH_RET_SUCCESS == result;i++)
 {
   result = mod_iface.call(MODEMCM_ENUM_LINES,i,(LPARAM)&mlp);
   if(GKH_RET_SUCCESS == result )
   {
     add_line_node(mlp.addr.line,mlp.line_state);
   }
 }
 root_node->Expand(false);
}



/*
  int generate_sin(fftw_complex * out,double freq,double discr,double ampl,int pcount,bool positive)
  {
    //Генерация синусоиды частотой freq
    //Дискретностью discr
    double PI2     = M_PI * 2.0;
    double period  = discr/freq;
    double delta_angle   = PI2/period;
    double angle = 0;
    int count = period*pcount;
    count*=2;
    for(int i = 0;i<count;i++)
    {
      double val = sin(angle);
      if(positive)
         val += 1.0 ;
      *out[0] = val*ampl;
      ++out;
      angle+=delta_angle;
      if(angle> PI2)
         angle-=PI2;
    }
    return count;
  }
 */


void __fastcall TCpc152DevFrm::on_spectr_destroy(TObject * sender)
{
}

void __fastcall TCpc152DevFrm::bShowOscClick(TObject *Sender)
{
  if(current_frame == cpc152dev_frame1)
  {
   TCpc152SignalSpectr * spectr  = new TCpc152SignalSpectr(this);
   spectr->Caption    = cpc152dev_frame1->current_tree_node->Text;
   spectr->cp_number  = cpc152dev_frame1->get_cp_number();
   spectr->SampleFreq->Text = this->cpc152dev_frame1->SampleFreq->Text;
   spectr->scan_count->Text = this->cpc152dev_frame1->SamplesCount->Text;
   spectr->DftExp->Position = this->cpc152dev_frame1->UpDown1->Position;
   if(AnalogsList->Items->Count && AnalogsList->Selected)
   {
     TListItem * item = AnalogsList->Selected;
     spectr->DevNum->Text = item->Caption;
     spectr->GroupNum->Text = item->SubItems->Strings[0];

   }
   spectr->mod_handle = mod_handle;
   spectr->Visible = true;
   spectr->SetFocus();
  }
}
//---------------------------------------------------------------------------




void __fastcall TCpc152DevFrm::tbUndoClick(TObject *Sender)
{
 if(current_frame)
 {
   begin_call_gkhandle(L"Отмена изменений");
   bool success = current_frame->undo_changes() ;
   if(success)
	   current_frame->ch_mask = 0;
   done_call_gkhandle(success ? 0 : GKH_RET_ERROR,NULL);
 }
}
//---------------------------------------------------------------------------

void __fastcall TCpc152DevFrm::PageControl1Change(TObject *Sender)
{
   if(PageControl1->ActivePage == ViewTabSheet)
   {
      setup_view_tab();
   }
   else
   ViewTimer->Enabled = false;

}
//---------------------------------------------------------------------------
void  __fastcall TCpc152DevFrm::setup_view_tab()
{
  if(this->current_frame == cpc152dev_frame1)
  {
      Panel2->Visible = true;
      AnalogsList->Clear();
      CPC152_ANALOG_STATS sts;
      bzero(&sts,sizeof(sts));
      sts.dw_size = sizeof(sts);
      sts.enum_idx =  0;
      while(GKH_RET_SUCCESS == mod_iface.call(CMD_CPC152DEV_ENUM_ANALOGS,cpc152dev_frame1->get_cp_number(),LPARAM(&sts)))
      {
         TListItem * item =  AnalogsList->Items->Add();
         item->Caption    = sts.dev_num;
         item->SubItems->Add(sts.grp_num);
         item->SubItems->Add(sts.param_count);
         item->SubItems->Add(sts.avg_scan_time);
         item->SubItems->Add(sts.avg_scan_param_time);
         item->SubItems->Add(sts.overtimes);
         ++sts.enum_idx;
      }
     Panel2->Visible = true;
     update_connect_state();
  }
   else
   {
     Panel2->Visible = false;
   }
     ViewTimer->Enabled = Panel2->Visible;

}

void __fastcall TCpc152DevFrm::update_connect_state()
{
  MODEM_LINE_STATS mls;
  bzero(&mls,sizeof(mls));
  mls.dw_size = sizeof(mls);
  UnicodeString conn_str,tm_str;

  if(current_frame == cpc152dev_frame1 && GKH_RET_SUCCESS == mod_iface.call(MODEMCM_GET_LINE_STATS,cpc152dev_frame1->get_cp_number(),(LPARAM)&mls))
  {
    if(mls.line_state&MODEM_LINE_STATE_CONNECT)
       {
         conn_str = L"Соединено";
         QWORD line_connect_time = NS100_MSEC(GetTime(false)-mls.begin_connect_time.QuadPart);
         char  buf[32] ;
         secunds2HMS(line_connect_time/1000,buf,sizeof(buf));
         tm_str = buf;
       }
       else
       {
         if(mls.line_state&MODEM_LINE_STATE_CONNECTING)
             conn_str = L"Подключение...";
             else
             conn_str = L"Нет связи";
       }

  }
  else
  conn_str = L"Неизвестно";
  ConnectState->Caption = conn_str;
  ConnectTime ->Caption = tm_str;

}


void __fastcall TCpc152DevFrm::ViewTimerTimer(TObject *Sender)
{
   //Обновление статистики по аналоговым группам
    CPC152_ANALOG_STATS sts;
   bzero(&sts,sizeof(sts));
   sts.dw_size = sizeof(sts);
   for(int i = 0;i<AnalogsList->Items->Count;i++)
   {
     TListItem * item =   AnalogsList->Items->Item[i];
     sts.dev_num = item->Caption.ToInt();
     sts.grp_num = item->SubItems->Strings[0].ToInt();
     if(GKH_RET_SUCCESS == mod_iface.call(CMD_CPC152DEV_GET_ANALOG_STATS,cpc152dev_frame1->get_cp_number(),LPARAM(&sts)))
     {
         item->SubItems->Strings[2] = (sts.avg_scan_time);
         item->SubItems->Strings[3] = (sts.avg_scan_param_time);
         item->SubItems->Strings[4] = (sts.overtimes);

     }
   }
   update_connect_state();
}
//---------------------------------------------------------------------------

