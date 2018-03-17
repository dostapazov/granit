//---------------------------------------------------------------------------
#pragma hdrstop
#include <vcl.h>
#include "gkopc_client_frm.h"
#include <opcda_i.c>
#include <opcenum_i.c>
//#include <opccomn_i.c>


//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "gkmodule_form"
#pragma link "gkmod_frame"
#pragma link "gkopc_client_mod_settings_frame"
#pragma link "gkopc_client_line_settings"
#pragma link "gkopcitem_view_frame"
#pragma link "about_common_frame"
#pragma resource "*.dfm"
TGKOpcClientFrm *GKOpcClientFrm;
//---------------------------------------------------------------------------
__fastcall TGKOpcClientFrm::TGKOpcClientFrm(TComponent* Owner)
	: TGKModuleForm(Owner)
{
}
//---------------------------------------------------------------------------

__fastcall TGKOpcClientFrm::TGKOpcClientFrm(TComponent* Owner, HWND parent, GKHANDLE mod_handle )
	: TGKModuleForm(Owner,parent,mod_handle)
{
}

void __fastcall TGKOpcClientFrm::WndProc(Messages::TMessage & msg)
{
   if(msg.Msg == WMUSER_SETUP_LINES_TREE)
     {
      msg.Result = 0;
      setup_lines();
     }
     else
     TGKModuleForm::WndProc(msg);

}

void __fastcall TGKOpcClientFrm::add_opc_line(LPMODEM_LINE_PROPS mlp )
{
  UnicodeString str = TOpcLineSetingsFrame::get_line_text(mod_iface,mlp->addr.line);
  TTreeNode * node = LinesTree->Items->AddChild(root_node,str);
  node->Data = (LPVOID)mlp->addr.line;
}

void __fastcall TGKOpcClientFrm::setup_lines()
{
   LinesTree->Items->BeginUpdate();
   root_node->DeleteChildren();
     BYTE buf[1024];
     bzero(buf,sizeof(buf));
     LPMODEM_LINE_PROPS mlp = (LPMODEM_LINE_PROPS)buf;
     mlp->dw_size = sizeof(buf);
     LRESULT result = GKH_RET_SUCCESS;
     for(int i = 0; i<OTD_ADDR_MAXVALUE && GKH_RET_SUCCESS == result;i++)
     {
       result = mod_iface.call(MODEMCM_ENUM_LINES,i,(LPARAM)mlp);
       if(GKH_RET_SUCCESS == result )
          add_opc_line(mlp);
     }
   root_node->Expand(false);
   LinesTree->Items->EndUpdate();
}



void     __fastcall TGKOpcClientFrm::on_module_state(GKHANDLE mod,DWORD state)
{
  bool running       = ((state & MODULE_STATE_RUNNING_MASK )== MODULE_STATE_RUNNING) ? true:false;
  bool stopped       = (state & MODULE_STATE_RUNNING_MASK ) ? false:true;
  bool stop_pending  = ((state & MODULE_STATE_RUNNING_MASK ) == MODULE_STATE_STOP_PENDING ) ? true : false;
  bool start_pending = ((state & MODULE_STATE_RUNNING_MASK ) == MODULE_STATE_START_PENDING ) ? true : false;
  bStop->Enabled     = running;
  bRefresh->Enabled  = running;
  bStart->Enabled    = stopped;
  UnicodeString  sb_text;
  if(running)
     sb_text = L"В работе";
  if(stopped)
     sb_text = L"Остановлен";
  if(stop_pending)
     sb_text = L"Останавливается";
  if(start_pending)
     sb_text = L"Стартует";
  StatusBar1->Panels->Items[0]->Text = sb_text;
}

void     __fastcall TGKOpcClientFrm::on_module_config_change(GKHANDLE mod,LPMODULE_CONFIG_DATA mcd)
{
}


void __fastcall TGKOpcClientFrm::bStartClick(TObject *Sender)
{
  mod_iface.start(MODULE_START_REQUEST,0);
}
//---------------------------------------------------------------------------

void __fastcall TGKOpcClientFrm::bStopClick(TObject *Sender)
{
  mod_iface.stop(MODULE_STOP_REQUEST);
}
//---------------------------------------------------------------------------

#pragma warn -8004
void     __fastcall TGKOpcClientFrm::after_set_gkhandle()
{
  //CoInitializeEx(NULL,COINITBASE_MULTITHREADED);
  CoInitializeEx(NULL,COINIT_APARTMENTTHREADED);
  HRESULT res =  CoInitializeSecurity(NULL,-1,NULL,NULL,RPC_C_AUTHN_LEVEL_CONNECT,RPC_C_IMP_LEVEL_IMPERSONATE,NULL,EOAC_NONE,NULL);

  TGKModuleForm::after_set_gkhandle();
  in_setup  = 0;
  LinesTree->Items->BeginUpdate();
  LinesTree->Items->Clear    ();
  LinesTree->Items->EndUpdate();

  Fcurrent_frame = NULL;
  root_node = LinesTree->Items->Add(NULL,"Модуль OPC Клиент");
  root_node->Selected = true;
  PostMessage(Handle,WMUSER_SETUP_LINES_TREE ,0,0);
  on_module_state(mod_iface(),mod_iface.get_module_state());
  set_notify(mod_iface(),MNF_COMMON|MNF_MODEM);
  DataFrame->mod_handle = mod_handle;
  DataFrame->Align = alClient;
}
#pragma warn .8004


void __fastcall TGKOpcClientFrm::FormCreate(TObject *Sender)
{
//
}
//---------------------------------------------------------------------------


void     __fastcall TGKOpcClientFrm::done_call_gkhandle(LRESULT ret,const wchar_t * str)
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

void     __fastcall TGKOpcClientFrm::begin_call_gkhandle(const wchar_t * cmd_name)
{
  StatusBar1->Panels->Items[0]->Text =  cmd_name;
  StatusBar1->Panels->Items[1]->Text =  AnsiString();
}




void __fastcall TGKOpcClientFrm::LinesTreeChanging(TObject *Sender, TTreeNode *Node,
          bool &AllowChange)
{
   if(current_frame)
   {
      current_frame->current_object_changed(Node,AllowChange);
   }
}
//---------------------------------------------------------------------------

void     __fastcall TGKOpcClientFrm::set_current_frame(TGKModuleFrame * value)
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


void __fastcall TGKOpcClientFrm::LinesTreeChange(TObject *Sender, TTreeNode *Node)

{
   if(current_frame)
      current_frame->current_object = NULL;
   if(Node)
   {
   if(Node == root_node)
       {
        current_frame = OpcModSettings1;
        PageControl1->ActivePage = ConfigSheet;

       }
      else
       {
        current_frame = this->OpcLineSetingsFrame1;

       }

       if(current_frame)
       {
        current_frame->on_changed     = on_frame_changed;
        current_frame->mod_handle     = mod_handle;
        current_frame->ch_mask        = 0;
        if(current_frame == OpcLineSetingsFrame1)
          set_notify(mod_handle,MNF_NOTIFY_SCRIPT_TRACE|MNF_NOTIFY_SCRIPT_ERROR);
       }
       else
         set_notify(mod_handle,MNF_NOTIFY_SCRIPT_TRACE|MNF_NOTIFY_SCRIPT_ERROR);


   }

   current_frame->current_object   = Node;
   DataFrame->current_object = Node == root_node ? NULL:Node;
   //MonFrame->current_object = Node == root_node ? NULL:Node;
   //PageControl1Change(this);
   tbDelete->Enabled = current_frame && Node !=root_node ? true:false;

}
//---------------------------------------------------------------------------
void     __fastcall TGKOpcClientFrm::on_frame_changed (TObject * sender)
{
  TGKModuleFrame * mf = dynamic_cast<TGKModuleFrame*>(sender);
  if(mf)
  {
   tbApply ->Enabled  = mf->ch_mask ? true:false;
   tbUndo  ->Enabled  = tbApply->Enabled;
   tbDelete->Enabled  = mf != this->OpcModSettings1 ? true : false;
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

void __fastcall TGKOpcClientFrm::tbNewClick(TObject *Sender)
{
  TTreeNode * node = LinesTree->Items->AddChild(root_node,L"Новый OPC клиент");
  node->Data     = (LPVOID)-1;
  node->Selected = true;
  tbUndo->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TGKOpcClientFrm::tbApplyClick(TObject *Sender)
{
  if(current_frame)
     {
      begin_call_gkhandle(L"Применение изменений");
      done_call_gkhandle( current_frame->apply_changes() ? GKH_RET_SUCCESS : GKH_RET_ERROR,NULL);
     }
}
//---------------------------------------------------------------------------

void __fastcall TGKOpcClientFrm::tbDeleteClick(TObject *Sender)
{
  if(current_frame)
     {
      begin_call_gkhandle(L"Удаление элемента");
      done_call_gkhandle( current_frame->delete_current() ? GKH_RET_SUCCESS : GKH_RET_ERROR,NULL);
     }
}
//---------------------------------------------------------------------------

void __fastcall TGKOpcClientFrm::tbUndoClick(TObject *Sender)
{
  if(current_frame)
     {
      begin_call_gkhandle(L"Применение изменений");
      done_call_gkhandle( current_frame->undo_changes() ? GKH_RET_SUCCESS : GKH_RET_ERROR,NULL);
     }
}
//---------------------------------------------------------------------------


void __fastcall TGKOpcClientFrm::LinesTreeDeletion(TObject *Sender, TTreeNode *Node)

{
  TTreeNode * sel_node = NULL;
  if(Node)
  {

  }

  if(!sel_node ) sel_node = root_node;
  sel_node->Selected = true;
}
//---------------------------------------------------------------------------


void __fastcall TGKOpcClientFrm::PageControl1Change(TObject *Sender)
{
  if(PageControl1->ActivePage == ViewOpcItemsSheet)
     {
      DataFrame->mod_handle = mod_handle;
      DataFrame->current_object = LinesTree->Selected;
      set_notify(mod_iface(),MNF_LINE_ITEM_CHANGED);
     }
     else
     reset_notify(mod_iface(),MNF_LINE_ITEM_CHANGED);

     if(PageControl1->ActivePage == ConfigSheet)
     {
     current_frame->current_object = LinesTree->Selected;
     DataFrame->current_object = NULL;
     }

}
//---------------------------------------------------------------------------

LRESULT  __fastcall TGKOpcClientFrm::on_gkhandle_notify (GKHANDLE from,LPNOTIFY_CODE nc,DWORD mask)
{
  if(mask == MNF_LINE_ITEM_CHANGED)
  {
   if(PageControl1->ActivePage == ViewOpcItemsSheet && DataFrame->current_tree_node && LOWORD(nc->notify_code) == (WORD)DataFrame->current_tree_node->Data)
      {
        DataFrame->refresh_opc_item(HIWORD(nc->notify_code));
      }
   return GKH_RET_SUCCESS;
  }

  if(mask == MNF_NOTIFY_SCRIPT_TRACE || mask == MNF_NOTIFY_SCRIPT_ERROR)
    {
     if(current_frame == OpcLineSetingsFrame1 && current_frame->current_tree_node && nc->notify_code == (DWORD) current_frame->current_tree_node->Data)
        {
         OpcLineSetingsFrame1->trace_monitor(mask == MNF_NOTIFY_SCRIPT_ERROR ? L"SCRIPT SYNTAX ERROR":L"",(wchar_t*)nc->notify_data,NOTIFY_DATA_SIZE(nc));
        }
        return GKH_RET_SUCCESS;
    }
  return TGKModuleForm::on_gkhandle_notify (from,nc,mask);
}

void __fastcall TGKOpcClientFrm::FormClose(TObject *Sender, TCloseAction &Action)

{
  //Action = caFree;
}
//---------------------------------------------------------------------------

void __fastcall TGKOpcClientFrm::FormDestroy(TObject *Sender)
{
   CoUninitialize();
}
//---------------------------------------------------------------------------

void __fastcall TGKOpcClientFrm::bRefreshClick(TObject *Sender)
{
 //  Команда обновление
 DWORD line_number = -1;
 if(current_frame == OpcLineSetingsFrame1)
    line_number = (DWORD)LinesTree->Selected->Data;
 mod_iface.call(MODEMCM_REFRESH_LINE,line_number,-1);
}
//---------------------------------------------------------------------------

