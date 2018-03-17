//---------------------------------------------------------------------------
#pragma hdrstop

#include <vcl.h>
#include "gkopc_client_line_settings.h"
#include <ke_secur.hpp>


//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "gkmod_frame"
#pragma resource "*.dfm"
TOpcLineSetingsFrame *OpcLineSetingsFrame;
//---------------------------------------------------------------------------
__fastcall TOpcLineSetingsFrame::TOpcLineSetingsFrame(TComponent* Owner)
	: TGKModuleFrame(Owner)
{
 Fserver_type = opc::opc_server_type_t::opc_server_da20;
 server_type_list->ItemIndex = (int)Fserver_type;
 config_editor = NULL;
 PageControl1->ActivePageIndex = 0;

}
//---------------------------------------------------------------------------

UnicodeString __fastcall TOpcLineSetingsFrame::get_line_text(TGKModuleInterface & mod_iface,DWORD line_num)
{
  UnicodeString str;
  BYTE buf[1024];
  bzero(buf,sizeof(buf));
  LPMODEM_LINE_PROPS mlp = (LPMODEM_LINE_PROPS)buf;
  mlp->dw_size      = sizeof(buf);
  mlp->line_text_sz = mlp->dw_size-sizeof(*mlp);

  if(mod_iface.call(MODEMCM_GET_LINE_PROPS,line_num,(LPARAM)mlp) == GKH_RET_SUCCESS && mlp->line_text_len)
   {
    str = mlp->line_text;
   }
   else
   str.printf(L"Линия № - %d",line_num);
   return str;
}


bool     __fastcall  TOpcLineSetingsFrame::apply_changes ()
{

 if(current_object)
 {
  DWORD line_number = (DWORD )current_tree_node->Data;
  GKOPC_LINE_CONFIG line_conf;
  memcpy(&line_conf,&line_config,sizeof(line_config));
  get_settings(&line_conf,true);
  DWORD cmd = (line_number == DWORD(-1)) ? CMD_OPC_ADD_LINE : CMD_OPC_UPD_LINE;
  if(mod_iface.call(cmd,(LPARAM)&line_conf,line_number))
     return false;
     else
     {
       memcpy(&line_config,&line_conf,sizeof(line_config));
       current_tree_node->Text = get_line_text(mod_iface, line_config.line_num);
       current_tree_node->Data = (LPVOID)line_config.line_num;
       ch_mask &= TuLuaScript->Tag | TrLuaScript->Tag;
       apply_scripts();
     }
 }
 return true;
}

bool     __fastcall  TOpcLineSetingsFrame::undo_changes  ()
{
 if(current_tree_node)
 {
  DWORD line_number = (DWORD )current_tree_node->Data;
  if(line_number == (DWORD)-1)
     delete_current();
    else
     setup_controls(&this->line_config);
  ch_mask = 0;
 }
 return true;
}

bool     __fastcall  TOpcLineSetingsFrame::delete_current()
{
 if(current_tree_node)
 {
 DWORD line_number = (DWORD )current_tree_node->Data;
 TTreeNode * node  = current_tree_node;
 ch_mask = 0;
 current_object = NULL;

  if(line_number != (DWORD)-1)
     {
      if(mod_iface.call(CMD_OPC_DEL_LINE,line_number,0))  return false;
     }
  node->Delete();
 }
 return true;
}

void     __fastcall  TOpcLineSetingsFrame::current_object_change (TObject * value)
{

  ++in_setup;
  TGKModuleFrame::current_object_change(value);
  if(current_tree_node)
  {
    //DWORD line_number = (DWORD )current_tree_node->Data;
    clear_controls();
    if(get_settings(&line_config,false))
       {
        setup_controls(&line_config);
       }
  }
  --in_setup;
  Visible = current_object ? true : false;
}

void     __fastcall  TOpcLineSetingsFrame::clear_controls()
{
 UnicodeString str;
 cp_number->Text      = str;
 cp_name->Text        = str;
 server_prog_id->Text = str;
 server_host->Text    = str;
 config_file->Text    = str;
 rate->Text           = 500;
 bandwidth->Text      = 0.0;
 cbLineOptReplaceTime->Checked = false;
 TuLuaScript->Text = str;
 TrLuaScript->Text = str;
}

void     __fastcall  TOpcLineSetingsFrame::setup_controls(LPGKOPC_LINE_CONFIG lc)
{
  if(lc->line_num  == (DWORD)-1 )
     cp_number->Text = UnicodeString();
  else
     cp_number->Text = lc->line_num  ;
  cp_name->Text = lc->cp_name;

  servers.clear();


  opc::TOpcServerList server_list;
  HRESULT res = server_list.create_instance(lc->server_host);
  if(res == S_OK)
  {
      res = server_list.enum_servers(Fserver_type,servers,NULL);
      wchar_t * prog_id   = NULL;
      wchar_t * type_name = NULL;
      res = server_list->GetClassDetails(lc->server_guid,&prog_id,&type_name);
      if(res == S_OK)
      {
         wcsncpy(lc->server_prog_id,prog_id,KERTL_ARRAY_COUNT(lc->server_prog_id));
         if(prog_id  )  CoTaskMemFree(prog_id);
         if(type_name)  CoTaskMemFree(type_name);
      }
  }

  server_prog_id->Text = lc->server_prog_id;
  server_host->Text    = lc->server_host;

  server_type_list->ItemIndex = lc->server_type;
  rate->Text        = lc->rate;
  config_file->Text = lc->config_file;
  bandwidth->Text   = lc->bandwidth;
  cbLineOptReplaceTime->Checked = (lc->options & GKOPC_LINE_OPT_REPLACE_TIME);
  setup_scripts();
}




void     __fastcall  TOpcLineSetingsFrame::current_object_changed(TObject * ,bool &enable_set)
{
 enable_set = true;
 if(current_object)
 {
  DWORD line_number = (DWORD )current_tree_node->Data;
  if(line_number == (DWORD) - 1 || ch_mask) enable_set =  false ;
 }
}



void __fastcall TOpcLineSetingsFrame::server_prog_idDropDown(TObject *Sender)
{
    server_prog_id->Items->Clear();
    servers.clear();
    HRESULT res = 0;
//    HANDLE hToken    = NULL;
//    if(LogonUser(L"SDTU",L".",L"sdtu",LOGON32_LOGON_BATCH,LOGON32_PROVIDER_DEFAULT,&hToken))
//       {
//        if(ImpersonateLoggedOnUser(hToken))
//         {
//          res = CoImpersonateClient ();
//          if(res)
//          {
//           _com_error err(res);
//           MessageBoxW(Handle,err.ErrorMessage(),_T("OPC Error"),MB_OK);
//          }
//
//          res = RpcImpersonateClient(NULL);
//          if(res)
//          {
//           _com_error err(res);
//           MessageBoxW(Handle,err.ErrorMessage(),_T("OPC Error"),MB_OK);
//          }
//         }
//       }

   res = CoInitializeSecurity(NULL,-1,NULL,NULL,RPC_C_AUTHN_LEVEL_CONNECT,RPC_C_IMP_LEVEL_IDENTIFY,NULL,EOAC_NONE,NULL);
   if(res) res = 0;
   if(res == S_OK)
   {
       opc::TOpcServerList server_list;
       res = server_list.create_instance(server_host->Text.c_str());
       if(res == S_OK)
       {
       int count = 0;
       res = server_list.enum_servers(Fserver_type,servers,&count);
       opc::TOpcServerList::server_entryes_t::iterator ptr = servers.begin();
       opc::TOpcServerList::server_entryes_t::iterator end = servers.end();
           while(ptr<end)
           {
            server_prog_id->Items->Add(ptr->prog_id.c_str());
            ++ptr;
           }
       }
   }
//   RevertToSelf();
//   RpcRevertToSelf();
//   CoRevertToSelf();
//   CloseHandle(hToken);
   if(res)
   {
     _com_error err(res);
     MessageBoxW(Handle,err.ErrorMessage(),_T("OPC Error"),MB_OK);
   }
}
//---------------------------------------------------------------------------

void __fastcall TOpcLineSetingsFrame::server_type_listChange(TObject *Sender)
{
  Fserver_type = opc::opc_server_type_t(server_type_list->ItemIndex);
}
//---------------------------------------------------------------------------

void __fastcall TOpcLineSetingsFrame::server_prog_idChange(TObject *Sender)
{
  //Смена сервера
  cp_numberChange(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TOpcLineSetingsFrame::config_editor_destroy(TObject *Sender)
{
     if(config_editor->opc_config_name != config_file->Text)
        config_file->Text = config_editor->opc_config_name;
     config_editor = NULL;
}

void __fastcall TOpcLineSetingsFrame::Button1Click(TObject *Sender)
{
  if(!config_editor)
     {
      config_editor = new TOpcConfigEditor(this);
      config_editor->OnDestroy = config_editor_destroy;
      TForm * form = dynamic_cast<TForm *>(Owner);
      if(form)
      {
       config_editor->Icon->Assign(form->Icon);
      }
     }
  config_editor->Visible = true;
  config_editor->Caption         = current_tree_node->Text + " " + config_file->Text;
  config_editor->opc_config_name = config_file->Text;
  //config_editor->opc_server_host = server_host->Text;
  //config_editor->opc_server_name = server_prog_id->Text;
  GUID _guid;
  UnicodeString prog_id;

  if(server_prog_id->ItemIndex < (int)servers.size() && server_prog_id->ItemIndex >=0 )
  {
    opc::TOpcServerList::server_entryes_t::value_type & server_rec = servers.at(server_prog_id->ItemIndex);
    _guid   = server_rec.guid;
    prog_id = server_rec.prog_id.c_str();
  }
  else
   {
    _guid    = this->line_config.server_guid;
    prog_id  = this->line_config.server_prog_id;
   }

  config_editor->opc_open_server(_guid,this->server_host->Text,prog_id);

  Sleep(100);
  config_editor->SetFocus();

}
//---------------------------------------------------------------------------


void __fastcall TOpcLineSetingsFrame::cp_numberChange(TObject *Sender)
{
 if(!in_setup)
 {
  TComponent * comp = dynamic_cast<TComponent*>(Sender);
  if(comp)
     ch_mask |= comp->Tag;
 }
}
//---------------------------------------------------------------------------

bool     __fastcall TOpcLineSetingsFrame::get_settings   (LPGKOPC_LINE_CONFIG line_conf,bool from_controls)
{
   if(from_controls)
   {
     line_conf->line_num     = _wtoi(cp_number->Text.c_str());
     line_conf->server_type  = server_type_list->ItemIndex;
     line_conf->rate         = _wtoi(rate->Text.c_str());
     line_conf->bandwidth    = _wtof(bandwidth->Text.c_str());
     wcsncpy(line_conf->cp_name,cp_name->Text.c_str(),KERTL_ARRAY_COUNT(line_conf->cp_name));
     wcsncpy(line_conf->config_file,config_file->Text.c_str(),KERTL_ARRAY_COUNT(line_conf->config_file));
     wcsncpy(line_conf->server_prog_id,server_prog_id->Text.c_str(),KERTL_ARRAY_COUNT(line_conf->server_prog_id));
     wcsncpy(line_conf->server_host   ,server_host->Text.c_str(),KERTL_ARRAY_COUNT(line_conf->server_host));
     if(server_prog_id->ItemIndex < (int)servers.size() && server_prog_id->ItemIndex>=0 )
     {
        line_conf->server_guid =  servers.at(server_prog_id->ItemIndex).guid;
        wcsncpy(line_conf->server_prog_id,servers.at(server_prog_id->ItemIndex).prog_id.c_str(),KERTL_ARRAY_COUNT(line_conf->server_prog_id));
     }
     //CLSIDFromProgIDEx(server_prog_id->Text.c_str(),&line_conf->server_guid);
     line_conf->options = 0;
     if(cbLineOptReplaceTime->Checked) line_conf->options |= GKOPC_LINE_OPT_REPLACE_TIME;
     return true;
   }
   else
   {
      line_conf->line_num = (DWORD )current_tree_node->Data;
      return mod_iface.call(CMD_OPC_GET_LINE,(LPARAM)line_conf,line_conf->line_num) == GKH_RET_SUCCESS ? true : false;
   }
}

void __fastcall TOpcLineSetingsFrame::bSelConfigFileClick(TObject *Sender)
{
  if(OpenDialog1->Execute(Handle))
  {
    config_file->Text = OpenDialog1->FileName;
  }
}
//---------------------------------------------------------------------------

LPSCRIPT_TEXT __fastcall alloc_script_text(LPSCRIPT_TEXT st,size_t sz )
{
  if(!sz && st)
      {
       delete [] (char*)st;
       st = NULL;
      }

  if(sz)
  {
   if(st)
     {
      if(st->text_capacity < sz)
        {
         alloc_script_text(st,0);
         st = alloc_script_text(NULL,sz);
        }
     }
     else
     {
      DWORD need_sz = sz+sizeof(*st);
      st = (LPSCRIPT_TEXT) new char [need_sz];
      st->dw_size       = need_sz;
      st->text_capacity = sz;
      st->text_len      = 0;
      st->text[0]       = 0;
     }

  }
  return st;
}

void     __fastcall  TOpcLineSetingsFrame::setup_scripts ()
{

   DWORD line_num = (DWORD )current_tree_node->Data;
   if(line_num != (DWORD)-1)
     {
      LPSCRIPT_TEXT st = alloc_script_text(NULL,8192);
      LRESULT ret = GKH_RET_SUCCESS;
      do{
          if(st->text_len < 0)
             st = alloc_script_text(st,-st->text_len + 256);
          ret = mod_iface.call(CMD_OPC_LINE_GET_TU_SCRIPT,line_num,(LPARAM)st);
        }while(ret == GKH_RET_SUCCESS && st->text_len<0);
        TuLuaScript->Text = UnicodeString(st->text,st->text_len);

      do{
          if(st->text_len < 0)
             st = alloc_script_text(st,-st->text_len + 256);
          ret = mod_iface.call(CMD_OPC_LINE_GET_TR_SCRIPT,line_num,(LPARAM)st);
        }while(ret == GKH_RET_SUCCESS && st->text_len<0);
        TrLuaScript->Text = UnicodeString(st->text,st->text_len);
      alloc_script_text(st,0);
     }

}

bool     __fastcall  TOpcLineSetingsFrame::apply_scripts ()
{
  bool ret = true;
  DWORD line_num   = (DWORD )current_tree_node->Data;
  LPSCRIPT_TEXT st = NULL;
  if(ch_mask & TuLuaScript->Tag)
     {
       st = alloc_script_text(st,std::max(4096,TuLuaScript->Text.Length()));
       st->text_len = TuLuaScript->Text.Length();
       KeRTL::Unicode2Ansi(st->text,st->text_len,TuLuaScript->Text.c_str());
       if(mod_iface.call(CMD_OPC_LINE_SET_TU_SCRIPT,line_num,(LPARAM)st))
          ret = false;
          else
          ch_mask&= ~TuLuaScript->Tag;
     }

  if(ch_mask & TrLuaScript->Tag)
     {
       st = alloc_script_text(st,std::max(4096,TrLuaScript->Text.Length()));
       st->text_len = TrLuaScript->Text.Length();
       KeRTL::Unicode2Ansi(st->text,st->text_len,TrLuaScript->Text.c_str());
       if(mod_iface.call(CMD_OPC_LINE_SET_TR_SCRIPT,line_num,(LPARAM)st))
          ret = false;
       else
       ch_mask&= ~TrLuaScript->Tag;
     }
   alloc_script_text(st,0);
   return ret;
}

void     __fastcall  TOpcLineSetingsFrame::trace_monitor(wchar_t *what,void * data,DWORD data_sz)
{
  if(what && *what)
     TraceMon->Lines->Add(what);
  TraceMon->Lines->Add(UnicodeString((wchar_t*)data,data_sz/sizeof(wchar_t)));
}





void __fastcall TOpcLineSetingsFrame::N1Click(TObject *Sender)
{
   TraceMon->Lines->Clear();
}
//---------------------------------------------------------------------------

void __fastcall TOpcLineSetingsFrame::N2Click(TObject *Sender)
{
  TraceMon->SelectAll();
}
//---------------------------------------------------------------------------

void __fastcall TOpcLineSetingsFrame::bRestartLineClick(TObject *Sender)
{
 if(current_tree_node)
 {
  DWORD line_number = (DWORD )current_tree_node->Data;
  mod_iface.call(CMD_OPC_RESTART_LINE,line_number,0);
 }

}
//---------------------------------------------------------------------------

