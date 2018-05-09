//---------------------------------------------------------------------------

#pragma hdrstop
#include <vcl.h>
#include "iec60870_frm.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "gkmodule_form"
#pragma link "gkmod_frame"
#pragma link "iec6070_mod_settings"
#pragma link "iec60870_line_param"
#pragma link "about_common_frame"
#pragma link "iec60870_data"
#pragma link "mon_frame"
#pragma link "mon_frame"
#pragma resource "*.dfm"
TIec60870Frm *Iec60870Frm;
//---------------------------------------------------------------------------
__fastcall TIec60870Frm::TIec60870Frm(TComponent* Owner)
	: TGKModuleForm(Owner)
{
}
//---------------------------------------------------------------------------

__fastcall TIec60870Frm::TIec60870Frm(TComponent* Owner,HWND _parent,GKHANDLE _handle)
	: TGKModuleForm(Owner,_parent,_handle)
{
 root_node = NULL;
 in_setup  = 0;
}

void        __fastcall TIec60870Frm::setup_form_sizes()
{
 TGKModuleForm::setup_form_sizes();
 ModemTree->Width = Width/4;
}


void __fastcall TIec60870Frm::WndProc(Messages::TMessage &msg)
{
  switch(msg.Msg)
  {
    case WMUSER_SETUP_LINES_TREE : setup_lines()              ;  break;
    default                      : TGKModuleForm::WndProc(msg);  break;
  }
}


void     __fastcall TIec60870Frm::after_set_gkhandle()
{
   UnicodeString node_text = L"IEC60870 модем";
   ModemTree->Items->Clear();
   root_node = ModemTree->Items->Add(NULL,node_text);
   root_node->Selected = true;
   PostMessage(Handle,WMUSER_SETUP_LINES_TREE,0,0);
   in_setup = 0;
   TGKModuleForm::after_set_gkhandle();
   Application->ProcessMessages();
   on_module_state(mod_iface(),mod_iface.get_module_state());
   set_notify(mod_iface(),MNF_COMMON|MNF_MODEM);
   DataFrame->mod_handle = mod_handle;
   DataFrame->Align = alClient;
   PageControl1->ActivePage = ParamsSheet;
}




TTreeNode*     __fastcall TIec60870Frm::add_iec60870_line(const LPMODEM_LINE_PROPS mlp)
{
     UnicodeString line_name;
     line_name.printf(L"КП №-%d",(DWORD)mlp->addr.line);
     TTreeNode * node = ModemTree->Items->AddChild(root_node,line_name);
     node->Data = (LPVOID)mlp->addr.line;
     IEC60870LineParam1->update_line(node,mlp);
     return node;
}

void     __fastcall TIec60870Frm::setup_lines()
{
 BYTE buf[1024];
 bzero(buf,sizeof(buf));
 LPMODEM_LINE_PROPS mlp = (LPMODEM_LINE_PROPS)buf;
 mlp->dw_size = sizeof(buf);
 mlp->line_text_sz = sizeof(buf)-sizeof(*mlp);
 DWORD result = GKH_RET_SUCCESS;
 ModemTree->Items->BeginUpdate();
 for(int i = 0; i<OTD_ADDR_MAXVALUE && GKH_RET_SUCCESS == result;i++)
 {
   result = mod_iface.call(MODEMCM_ENUM_LINES,i,(LPARAM)mlp);
   if(GKH_RET_SUCCESS == result )
   {
    add_iec60870_line(mlp);
   }
 }
 root_node->Expand(false);
 ModemTree->Items->EndUpdate();
}


void     __fastcall TIec60870Frm::done_call_gkhandle(LRESULT ret,const wchar_t * str)
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

void     __fastcall TIec60870Frm::begin_call_gkhandle(const wchar_t * cmd_name)
{
  StatusBar1->Panels->Items[0]->Text =  cmd_name;
  StatusBar1->Panels->Items[1]->Text =  AnsiString();
}

TTreeNode * __fastcall TIec60870Frm::find_line_node(DWORD line_num)
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


void     __fastcall TIec60870Frm::on_modem_number    (const LPMODEM_CHANGE_NUMBER mns)
{
 if(root_node->Selected)
    {
     IEC60870ModSettings1->modem_number->Text = mns->new_number;
     IEC60870ModSettings1->ch_mask &= ~CCFL_MODEM_NUMBER;
    }
}

void     __fastcall TIec60870Frm::on_line_state      (const LPMODEM_LINE_STATE mls)
{
   TTreeNode * node = find_line_node((DWORD)mls->addr.line);
   if(node)
   {
    if(mls->line_state & MODEM_LINE_STATE_EXIST)
    {
    if(mls->state_changes)
       TIEC60870LineParam::set_node_image_index(node,mls->line_state);
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
          add_iec60870_line(mlp);
     }
   }
}

LRESULT  __fastcall TIec60870Frm::on_gkhandle_notify (GKHANDLE from,LPNOTIFY_CODE nc,DWORD mask)
{
 if(mask == MNF_MODEM)
 {
  switch(nc->notify_code)
  {
   case MNCODE_MODEM_NUMBER: on_modem_number((LPMODEM_CHANGE_NUMBER)nc->notify_data);break;
   case MNCODE_LINE_STATE  : on_line_state  ((LPMODEM_LINE_STATE)nc->notify_data);break;
  }
  return GKH_RET_SUCCESS;
 }
 if(mask == MNF_LINE_TX)
 {
   MonFrame->do_monotir(nc->notify_code,(LPBYTE)nc->notify_data,NOTIFY_DATA_SIZE(nc),true);
   return GKH_RET_SUCCESS;
 }

 if(mask == MNF_LINE_RX)
 {
   MonFrame->do_monotir(nc->notify_code,(LPBYTE)nc->notify_data,NOTIFY_DATA_SIZE(nc),false);
   return GKH_RET_SUCCESS;
 }

 if(mask == MNF_LINE_RECORD_CHANGED)
 {
   DataFrame->notify_record(nc->notify_code,(LPBYTE)nc->notify_data,NOTIFY_DATA_SIZE(nc));
   return GKH_RET_SUCCESS;
 }


 return TGKModuleForm::on_gkhandle_notify(from,nc,mask);
}


void     __fastcall TIec60870Frm::on_module_state(GKHANDLE mod,DWORD state)
{
  bool running       = ((state & MODULE_STATE_RUNNING_MASK )== MODULE_STATE_RUNNING) ? true:false;
  bool stopped       = (state & MODULE_STATE_RUNNING_MASK ) ? false:true;
  bool stop_pending  = ((state & MODULE_STATE_RUNNING_MASK ) == MODULE_STATE_STOP_PENDING ) ? true : false;
  bool start_pending = ((state & MODULE_STATE_RUNNING_MASK ) == MODULE_STATE_START_PENDING ) ? true : false;
  bStop->Enabled      = running;
  bStart->Enabled     = stopped;
  int idx = LINE_IMAGE_CONNECTING;
  UnicodeString  sb_text;
  if(running)
    {
     sb_text = L"В работе";
     idx = LINE_IMAGE_CONNECT;
    }

  if(stopped)
  {
     sb_text = L"Остановлен";
     idx = LINE_IMAGE_DISCONNECT;
  }

  if(stop_pending)
  {
     sb_text = L"Останавливается";
     idx = LINE_IMAGE_CONNECTING;
  }
  if(start_pending)
  {
     sb_text = L"Стартует";
     idx = LINE_IMAGE_CONNECTING;
  }

  StatusBar1->Panels->Items[0]->Text = sb_text;
  root_node->ImageIndex    = idx;
  root_node->StateIndex    = idx;
  root_node->SelectedIndex = idx;

}

void     __fastcall TIec60870Frm::on_module_config_change(GKHANDLE mod,LPMODULE_CONFIG_DATA mcd)
{
}

bool     __fastcall TIec60870Frm::get_module_settings()
{
 return true;
}



void __fastcall TIec60870Frm::bStartClick(TObject *Sender)
{
 done_call_gkhandle(mod_iface.start(MODULE_START_REQUEST,0),L"Запуск модуля");
}
//---------------------------------------------------------------------------
void __fastcall TIec60870Frm::bStopClick(TObject *Sender)
{
  done_call_gkhandle(mod_iface.stop(MODULE_STOP_REQUEST),L"Останов модуля");
}
//---------------------------------------------------------------------------

void     __fastcall TIec60870Frm::set_current_frame(TGKModuleFrame * value)
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

void __fastcall TIec60870Frm::ModemTreeChange(TObject *Sender, TTreeNode *Node)
{
   if(current_frame)
      current_frame->current_object = NULL;
   if(Node)
   {
   if(Node == root_node)
        {
         current_frame = IEC60870ModSettings1;
         PageControl1->ActivePage = ParamsSheet;
        }
      else
       current_frame = IEC60870LineParam1;
       if(current_frame)
       {
        current_frame->on_changed     = on_frame_changed;
        current_frame->mod_handle     = mod_handle;
        current_frame->ch_mask        = 0;
       }
   }

   current_frame->current_object   = Node;
   DataFrame->current_object = Node == root_node ? NULL:Node;
   MonFrame->current_object  = Node == root_node ? NULL:Node;
   MonFrame->obj_addr_size   = IEC60870LineParam1->obj_addr_size->ItemIndex+1;
   ViewSheet->TabVisible     = Node == root_node ? false : true;
   MonSheet->TabVisible      = ViewSheet->TabVisible;
   PageControl1Change(this);
   tbDelete->Enabled = current_frame && Node !=root_node ? true:false;

}
//---------------------------------------------------------------------------

void __fastcall TIec60870Frm::ModemTreeChanging(TObject *Sender, TTreeNode *Node,
          bool &AllowChange)
{
   if(current_frame)
   {
      current_frame->current_object_changed(Node,AllowChange);
   }
}
//---------------------------------------------------------------------------

void     __fastcall TIec60870Frm::on_frame_changed (TObject * sender)
{
  TGKModuleFrame * mf = dynamic_cast<TGKModuleFrame*>(sender);
  if(mf)
  {
   tbApply ->Enabled  = mf->ch_mask ? true:false;
   tbUndo  ->Enabled  = tbApply->Enabled;
   tbDelete->Enabled  = mf != this->IEC60870ModSettings1 ? true : false;
   tbNew->Enabled     = true;
  }
  else
  {
    tbDelete->Enabled = false;
    tbApply ->Enabled = false;
    tbUndo  ->Enabled = false;
    tbNew   ->Enabled = false;
  }
}


void __fastcall TIec60870Frm::PageControl1Change(TObject *Sender)
{
   DWORD set_mask   =  0;
   DWORD reset_mask =  0;

   if(PageControl1->ActivePage == this->MonSheet)
      set_mask |= MNF_LINE_TX|MNF_LINE_RX;
   else
    reset_mask |= MNF_LINE_TX|MNF_LINE_RX;

   if(PageControl1->ActivePage == this->ViewSheet)
     set_mask|= MNF_LINE_RECORD_CHANGED;
   else
    reset_mask |= MNF_LINE_RECORD_CHANGED;

    if(set_mask)
         set_notify(mod_iface(),set_mask);
    if(reset_mask)
         reset_notify(mod_iface(),reset_mask);


}
//---------------------------------------------------------------------------

void __fastcall TIec60870Frm::tbDeleteClick(TObject *Sender)
{

  if(current_frame &&
     IDYES == MessageBoxW(Handle,UnicodeString(UnicodeString(L"Удалить элемент ")+current_frame->current_tree_node->Text+ UnicodeString('?')).c_str(),L"Запрос на удаление",MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2))
     {
      current_frame->delete_current();
     }
}
//---------------------------------------------------------------------------

void __fastcall TIec60870Frm::tbApplyClick(TObject *Sender)
{
  if(current_frame)
    {
     begin_call_gkhandle(L"Применение изменений");
     bool success = current_frame->apply_changes() ;
     if(success)
     {
      current_frame->ch_mask = 0;
      if(current_frame == this->IEC60870LineParam1)
        MonFrame->obj_addr_size = IEC60870LineParam1->obj_addr_size->ItemIndex+1;
     }
     done_call_gkhandle(success ? 0 : GKH_RET_ERROR,NULL);
    }
}
//---------------------------------------------------------------------------

void __fastcall TIec60870Frm::tbUndoClick(TObject *Sender)
{
  if(current_frame && current_frame->undo_changes())
     current_frame->ch_mask = 0;


}
//---------------------------------------------------------------------------

void __fastcall TIec60870Frm::tbNewClick(TObject *Sender)
{
  PageControl1->ActivePage = ParamsSheet;
  TTreeNode * node = ModemTree->Items->AddChild(root_node,L"Новое КП...");
  node->Data = (LPVOID)-1;
  node->Selected = true;

}
//---------------------------------------------------------------------------

void __fastcall TIec60870Frm::ModemTreeCompare(TObject *Sender, TTreeNode *Node1,
          TTreeNode *Node2, int Data, int &Compare)
{
     Compare = (int)((DWORD)Node1->Data - (DWORD)Node2->Data);
}
//---------------------------------------------------------------------------

void __fastcall TIec60870Frm::ModemTreeDeletion(TObject *Sender, TTreeNode *Node)

{
 if(ModemTree->Selected == Node)
   {
     TTreeNode * _node = Node->getNextSibling();
     if(!_node) _node = Node->getPrevSibling();
     if(!_node) _node = Node->Parent;
     if( _node)
         _node->Selected = true;
   }
}
//---------------------------------------------------------------------------

void __fastcall TIec60870Frm::tbRefreshClick(TObject *Sender)
{
  TTreeNode * node  = this->ModemTree->Selected;
  DWORD line_number = -1;
  if(node && node!=root_node)
     line_number = (DWORD)node->Data;

  mod_iface.call(MODEMCM_REFRESH_LINE,line_number,-1);
}
//---------------------------------------------------------------------------

void __fastcall TIec60870Frm::ToolButton2Click(TObject *Sender)
{
  TIec60870Frm * frm = new TIec60870Frm(this);
  frm->mod_handle = this->mod_handle;
  frm->Visible = true;
}
//---------------------------------------------------------------------------

