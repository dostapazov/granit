//---------------------------------------------------------------------------
#pragma hdrstop
#include <vcl.h>
#include "gkexplorer_form.h"
#include "gkexplorer_rc.h"
#include <stdio.h>
#include "activation_form.h"
#include <kfname.hpp>
#include <tlicense.hpp>
#include <ke_secur.hpp>


//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "module_info_frame"
#pragma link "about_common_frame"
#pragma resource "*.dfm"
TGKExplorerFrm *GKExplorerFrm;

int __stdcall modules_comparer(LONG n1,LONG n2,LONG)
{
 TGKModuleInterface mi1((GKHANDLE)((TTreeNode *)n1)->Data,true); 
 TGKModuleInterface mi2((GKHANDLE)((TTreeNode *)n2)->Data,true); 
 return mi1.compare(mi2);
}

//---------------------------------------------------------------------------
__fastcall TGKExplorerFrm::TGKExplorerFrm(TComponent* Owner,HWND own_wnd,GKHANDLE h)
        : TGKModuleForm(Owner,own_wnd,h)
{
 added_module_node = NULL;
 mod_def     = NULL;
 StatusBar1->Panels->Items[0]->Width =  ModulesTree->Width;
}
//---------------------------------------------------------------------------

void     __fastcall TGKExplorerFrm::setup_form_sizes()
{
   float w = Monitor->WorkareaRect.Width ();
   float h = Monitor->WorkareaRect.Height();
   if(w>640)
      Width  = w*0.7;
   if(h>460)
      Height = h*0.7;
   ModulesTree->Width = Width/4;
}

void __fastcall TGKExplorerFrm::SetCommandStatus(AnsiString value)
{
        if(StatusBar1->Panels->Items[1]->Text != value) {
                StatusBar1->Panels->Items[1]->Text = value;
        }
}

AnsiString __fastcall TGKExplorerFrm::GetCommandStatus()
{
        return StatusBar1->Panels->Items[1]->Text;
}

GKHANDLE __fastcall TGKExplorerFrm::GetGKHandle(TTreeNode * node)
{
 if(node)
   {
    gkmod_wnd_iface* mi = (gkmod_wnd_iface*)node->Data;
    return mi->handle;
   }
 return (GKHANDLE)NULL;
}

void __fastcall TGKExplorerFrm::after_set_gkhandle()
{
 TGKModuleForm::after_set_gkhandle();
 AnsiString c ;
 c = load_rcstring(ModuleInstance,IDS_CAPTION);

 wchar_t buff [MAX_PATH];
 mod_iface.get_module_name(buff,sizeof(buff)/sizeof(wchar_t));
 c+=" \'";
 c+=AnsiString(buff);
 c+='\'';
 Caption = c;
 ModulesTree->Items->BeginUpdate();
 enum_modules(add_handle_node(0,mod_iface()),mod_iface())->Expand(false);
 ModulesTree->Items->EndUpdate();

}


TTreeNode * __fastcall TGKExplorerFrm::add_handle_node(TTreeNode * parent,GKHANDLE h)
{
 DWORD img_idx = 0;
 wchar_t buff [MAX_PATH];
 gkmod_wnd_iface * mwi =  new gkmod_wnd_iface(h);
 MODULE_INFO mi;
 mi.dw_size = sizeof(mi);
 mi.mi_mask = MI_FL_MODULE_NAME|MI_FL_ICON;
 mi.module_name_cbsz = sizeof(buff)/sizeof(wchar_t);
 mi.module_name      = buff;
 if(mwi->call(MCMD_GETMODULE_INFO,LPARAM(&mi),0) || !buff[0])
      {
       get_gkhandle_nameW(h,buff,sizeof(buff)/sizeof(wchar_t));
       if(!buff[0])
          safe_strcpy(buff,L"unnamed module");
      } 
 if(mi.icon)
 {
  TIcon * icon = new TIcon;
  icon->Handle = mi.icon;
  img_idx = ModuleIcons->AddIcon(icon);
  delete icon;
 } 
 added_module_node = ModulesTree->Items->AddChild(parent,buff);
 added_module_node->ImageIndex    = img_idx;
 added_module_node->StateIndex    = img_idx;
 added_module_node->SelectedIndex = img_idx;
 added_module_node->Data = mwi;
 set_notify(mwi->handle,MNF_COMMON);
 return added_module_node;
}

TTreeNode *   __fastcall TGKExplorerFrm::enum_modules(TTreeNode * parent,GKHANDLE h)
{

  int sub_mod_count = (int)call_gkhandle(h,MCMD_GETMODULES_COUNT,0,0);

  for(int i = 0;i<sub_mod_count;i++)
  {
    GKHANDLE    mod   = (GKHANDLE)call_gkhandle(h,MCMD_ENUM_MODULES,(LPARAM)i,0);
    TTreeNode * child = add_handle_node(parent,mod);
    enum_modules(child,mod);
    child->CustomSort(modules_comparer,0,false);
  }
  return parent;
}
#pragma warn -8057
void __fastcall TGKExplorerFrm::MiCloseClick(TObject *Sender)
{
 ModalResult = mrCancel;
}
//---------------------------------------------------------------------------
#pragma warn .8057


void __fastcall TGKExplorerFrm::update_commands(GKHANDLE h)
{

 MiAddModule->Enabled = int(call_gkhandle(h,MCMD_GETMODULES_COUNT,0,0))>=0 ? true:false;
 TbAddModule->Enabled = MiAddModule->Enabled;
 GKHANDLE owner =(GKHANDLE)call_gkhandle(h,MCMD_GETOWNER_HANDLE,0,0);
 MiDeleteModule->Enabled = owner ? true:false;
 TbDeleteModule->Enabled = MiDeleteModule->Enabled;
 bool running    = call_gkhandle(h,MCMD_IS_MODULE_RUNNING,0,0) ? true:false;
 bool own_running = owner ? true: call_gkhandle(owner,MCMD_IS_MODULE_RUNNING,0,0) ? true:false;
 MiStartModule->Enabled = (!running && own_running);
 TbStartModule->Enabled = (!running && own_running);
 MiStopModule->Enabled  = running;
 TbStopModule->Enabled  = running;
}

void __fastcall TGKExplorerFrm::update_status_bar(GKHANDLE h)
{
 MODULE_INFO mi;
 mi.dw_size = sizeof(mi);
 mi.mi_mask = MI_FL_MODULE_STATE;
 call_gkhandle(h,MCMD_GETMODULE_INFO,LPARAM(&mi),0);
 on_module_state(h,mi.module_state);

}

void    __fastcall TGKExplorerFrm::on_module_state(GKHANDLE mod,DWORD state)
{
 if(is_same_gkhandle(mod,GetGKHandle(ModulesTree->Selected)))
 {
 AnsiString str;
 int str_id;
 bool running = ((state&MODULE_STATE_RUNNING_MASK)==MODULE_STATE_RUNNING) ? true:false;
 bool stopped = ((state&MODULE_STATE_RUNNING_MASK)==0) ? true:false;

 if(state & MODULE_STATE_START_PENDING)
  str_id  = IDS_MODSTART_PENDING;// "Запуск модуля...";
 if(state & MODULE_STATE_STOP_PENDING)
  str_id  = IDS_MODSTOP_PENDING;// "Останов модуля...";
 if(running)
     str_id  = IDS_RUNNING;//"Работает";
 if(stopped)
    str_id  = IDS_STOPPED;//  "Остановлен";
    //update_commands(mod);
    this->MiStartModule->Enabled = stopped;
    this->TbStartModule->Enabled = stopped;
    this->MiStopModule->Enabled  = running;
    this->TbStopModule->Enabled  = running;

    TGKModuleInterface iface(mod,true);
    MODULE_INFO mi;
    mi.dw_size = sizeof(mi);
    mi.mi_mask = MI_FL_MODULE_TIMES;
    iface.get_module_info(&mi);
    ModuleInfoFrame1->set_work_time(*(__int64*)&mi.start_time,*(__int64*)&mi.stop_time,running);
    ModuleInfoFrame1->Timer1->Enabled = running;
    Application->ProcessMessages();
    ModuleStateText = load_rcstring(ModuleInstance,str_id);
    //ModulesTreeChange(this,ModulesTree->Selected);
 }
}
#pragma warn -8057
void    __fastcall TGKExplorerFrm::on_module_config_change(GKHANDLE mod,LPMODULE_CONFIG_DATA mcd)
{
}


void __fastcall TGKExplorerFrm::MiAddModuleClick(TObject *Sender)
{
  LRESULT result = GKH_RET_CANCEL;
  DWORD error = 0;
  begin_gk_command(UnicodeString(load_rcstring(ModuleInstance,IDS_CMD_APPEND)).c_str());
  GKHANDLE owner_mod = mod_iface();
  if(ModulesTree->Selected && OpenDialog1->Execute())
  {
    added_module_node = NULL;
    WideString w_str  = OpenDialog1->FileName;
    owner_mod         = GetGKHandle(ModulesTree->Selected);
    result            = call_gkhandle(owner_mod,MCMD_APPEND_MODULE,LPARAM(w_str.c_bstr()),TRUE);
    GKHANDLE new_mod  = (GKHANDLE) result;
    if(new_mod && LRESULT(new_mod)!= GKH_RET_ERROR)
    {
      if(!added_module_node) add_handle_node(ModulesTree->Selected,new_mod);
          ModulesTree->Selected->CustomSort(modules_comparer,0,false);
          added_module_node->Selected = true;
      MiSetupModuleClick(this);
    }
    else
    error = GetLastError();
  }
  end_gk_command(owner_mod,result,error);
}
//---------------------------------------------------------------------------


void __fastcall TGKExplorerFrm::ModulesTreeChange(TObject *Sender,
      TTreeNode *Node)
{
  CommandStatus = "";
  update_commands  (GetGKHandle(Node));
  update_status_bar(GetGKHandle(Node));
  ModuleInfoFrame1->setup_frame(GetGKHandle(Node));
  bool not_licensed = !ModuleInfoFrame1->licensed;
  miLicenseSeparator->Visible  = not_licensed;
  miRegisterModule->Visible    = not_licensed;
  miGenPrelicenseData->Visible = not_licensed;
  miGenTrialLicense->Visible = false;

}


//---------------------------------------------------------------------------

void __fastcall TGKExplorerFrm::ModulesTreeChanging(TObject *Sender,
      TTreeNode *Node, bool &AllowChange)
{
 reset_notify(GetGKHandle(Node),~MNF_COMMON);
}

//---------------------------------------------------------------------------

void __fastcall TGKExplorerFrm::ModulesTreeDeletion(TObject *Sender,
      TTreeNode *Node)
{
  this->reset_notify(GetGKHandle(Node),-1);
  delete (gkmod_wnd_iface*)Node->Data;
}

//---------------------------------------------------------------------------

void __fastcall TGKExplorerFrm::MiStartModuleClick(TObject *Sender)
{
 if(ModulesTree->Selected)
    {

     begin_gk_command(UnicodeString(load_rcstring(ModuleInstance,IDS_CMD_STARTING)).c_str());//"Запуск модуля"
     GKHANDLE mod = GetGKHandle(ModulesTree->Selected);
     LRESULT ret = call_gkhandle(mod,MCMD_START,MODULE_START_REQUEST,0);
     DWORD error = GetLastError();
     end_gk_command(mod,ret,error);
     if(ret && error  != MERR_OWNER_NOTRUNNING)
       MiSetupModuleClick(this);
    }

}
//---------------------------------------------------------------------------

void __fastcall TGKExplorerFrm::MiStopModuleClick(TObject *Sender)
{
 if(ModulesTree->Selected)
   {
     begin_gk_command(UnicodeString(load_rcstring(ModuleInstance,IDS_CMD_STOPPING)).c_str()); //"Останов модуля");
     GKHANDLE mod = GetGKHandle(ModulesTree->Selected);
     LRESULT ret =call_gkhandle(mod,MCMD_STOP,MODULE_STOP_REQUEST,0);
     end_gk_command(mod,ret,GetLastError());
   }
}

//---------------------------------------------------------------------------

void    __fastcall TGKExplorerFrm::show_module_wnd(GKHANDLE handle,DWORD wnd_type,LPVOID param)
{
   TTreeNode * node = find_module_node(handle,ModulesTree->Items->GetFirstNode());
   if(node)
   {
    gkmod_wnd_iface * mi = (gkmod_wnd_iface *)node->Data;
    if(mi && mi->load_window_module())
       mi->create_window(Handle,wnd_type,FALSE,NULL,TRUE);
   }
}

void __fastcall TGKExplorerFrm::MiSetupModuleClick(TObject *Sender)
{
 //Вызов окна настройки
 if(ModulesTree->Selected)
 {
  Screen->Cursor = crAppStart;
  gkmod_wnd_iface * mi = (gkmod_wnd_iface *)ModulesTree->Selected->Data;
  if(mi && mi->load_window_module())
    /*LPVOID wnd =*/ mi->create_window(Handle,MODULE_WND_TYPE_CONFIG,FALSE,NULL,TRUE);
  Screen->Cursor = crDefault;
 }
}
//---------------------------------------------------------------------------

void __fastcall TGKExplorerFrm::MiViewModuleClick(TObject *Sender)
{
 if(ModulesTree->Selected)
 {
  Screen->Cursor = crAppStart;
  gkmod_wnd_iface * mi = (gkmod_wnd_iface *)ModulesTree->Selected->Data;
  if(mi && mi->load_window_module())
    {
     //LPVOID wnd =
     mi->create_window(Handle,MODULE_WND_TYPE_VIEWER,FALSE,NULL,TRUE);
    }
  Screen->Cursor = crDefault;
 }
}

#pragma warn .8057

//---------------------------------------------------------------------------

LRESULT __fastcall TGKExplorerFrm::on_gkhandle_notify(GKHANDLE from,LPNOTIFY_CODE nc,DWORD mask)
{
  return TGKModuleForm::on_gkhandle_notify(from,nc,mask);
}

TTreeNode*  __fastcall TGKExplorerFrm::find_module_node(GKHANDLE handle ,TTreeNode * root)
{
  TTreeNode * ret = NULL;
  if(is_same_gkhandle(handle,GetGKHandle(root)))
     ret = root;
  for(int i = 0;!ret && i<root->Count  ;i++)
  {
   ret = find_module_node(handle,root->Item[i]);
  }
 return ret;
}

#pragma warn -8057

void    __fastcall TGKExplorerFrm::on_added_module(GKHANDLE owner, GKHANDLE mod_handle,DWORD type)
{
 TGKModuleInterface     i(mod_handle,true);
 TTreeNode * owner_node = find_module_node(owner,ModulesTree->Items->GetFirstNode());
 if(owner_node)
   {
    TTreeNode * node =   find_module_node(mod_handle,owner_node);
    if(!node)
     /*node =*/add_handle_node(owner_node,mod_handle);
    owner_node->CustomSort(modules_comparer,0,false);
   }

}

void    __fastcall TGKExplorerFrm::on_removed_module(GKHANDLE owner, GKHANDLE mod_handle,DWORD type)
{
    TTreeNode * node =   find_module_node(mod_handle,ModulesTree->Items->GetFirstNode()/*owner_node*/);
    if(node)
       node->Delete();
}

#pragma warn .8057

void    __fastcall TGKExplorerFrm::on_renamed_module(GKHANDLE mod_handle)
{
  TTreeNode *node=find_module_node(mod_handle,ModulesTree->Items->GetFirstNode());
  if(node )
     {
      if(node->Selected)
        ModuleInfoFrame1->setup_frame(mod_handle);
     } 
}

#pragma warn -8057

void __fastcall TGKExplorerFrm::MiDeleteModuleClick(TObject *Sender)
{
 LRESULT ret  ;

 if(ModulesTree->Selected && ModulesTree->Selected->Data)
 {
 UnicodeString str ;str = load_rcstring(ModuleInstance,IDS_CMD_REMOVING);
 begin_gk_command(str.c_str());
 gkmod_wnd_iface * wmi = (gkmod_wnd_iface*)ModulesTree->Selected->Data;
 GKHANDLE mod = wmi->handle;
 GKHANDLE owner =(GKHANDLE)(mod ? call_gkhandle(mod,MCMD_GETOWNER_HANDLE,0,0):NULL);
 if(owner && MessageBox(Handle,UnicodeString(load_rcstring(ModuleInstance,IDS_QUERY_REMOVING)).c_str(),str.c_str(),MB_YESNO) == ID_YES)
 {

     get_gkhandle_usage(mod);
     TGKModuleInterface mi(mod,true);
     if(mi.is_running())
        ret = call_gkhandle(mod,MCMD_STOP,MODULE_STOP_REQUEST,0);
        else
        ret = GKH_RET_SUCCESS;
     if(ret == GKH_RET_SUCCESS)
     {
      ret = call_gkhandle(owner,MCMD_REMOVE_MODULE,LPARAM(mod),0);
      end_gk_command(owner,ret,GetLastError());
     }
     else
     end_gk_command(mod,ret,GetLastError());

 }
 }
}
//---------------------------------------------------------------------------
void __fastcall TGKExplorerFrm::FormClose(TObject *Sender,
      TCloseAction &Action)
{

  if(Action == caHide)
    ModulesTree->Items->Clear();
}
//---------------------------------------------------------------------------

void __fastcall TGKExplorerFrm::FormHide(TObject *Sender)
{
  mod_iface.lock();
  TTreeNode * sel = this->ModulesTree->Selected;
  if(sel)
    {
     reset_notify(GetGKHandle(sel),-1);
    }
  mod_iface.unlock();
}

#pragma warn .8057

//---------------------------------------------------------------------------
TTreeNode * __fastcall find_node_by_gkhandle(TTreeNode * root,GKHANDLE h)
{
  TTreeNode * ret  = NULL;
  while(!ret && root)
  {
  TTreeNode * node = root->getFirstChild();
  do{
     if(node)
       {
        gkmod_wnd_iface* wmi = (gkmod_wnd_iface*)node->Data;
        if(!is_same_gkhandle(wmi->handle,h))
           node = root->GetNextChild(node);
           else
           {ret = node;break;}
           if(node && node->Count )
             ret = find_node_by_gkhandle(node,h);
       }
    }while(node && !ret);
    root = root->getNextSibling();
  }
  return ret;
}


DWORD   __fastcall TGKExplorerFrm::OnModuleWindowMessage(UINT msg,LPVOID wnd_handle,GKHANDLE h)
{
 if(msg == WGKM_WND_HIDE || msg == WGKM_WND_CLOSE)
   {
    TTreeNode * node = this->ModulesTree->Selected;
    gkmod_wnd_iface* wmi = (gkmod_wnd_iface*)node->Data;
    //TODO Переделать поиск по всем модулям
    if(!is_same_gkhandle(wmi->handle,h))
    {
     node = find_node_by_gkhandle(ModulesTree->Items->GetFirstNode(),h);
    }
    if(node)
    {
     wmi = (gkmod_wnd_iface*)node->Data;
     if(wmi)
     {
      call_gkhandle(wmi->handle,MCMD_FLUSH_CONFIG,0,0);
      wmi->destroy_window(wnd_handle,false);
     }


    } 
   }
 return 0;
}




void __fastcall TGKExplorerFrm::ModuleInfoFrame1Timer1Timer(
      TObject *Sender)
{
 ModuleInfoFrame1->Timer1Timer(Sender);
}
//---------------------------------------------------------------------------




void __fastcall TGKExplorerFrm::SetModuleStateText(AnsiString value)
{
        if(StatusBar1->Panels->Items[0]->Text != value) {
                StatusBar1->Panels->Items[0]->Text = value;
        }
}
AnsiString __fastcall TGKExplorerFrm::GetModuleStateText()
{
        return StatusBar1->Panels->Items[0]->Text;
}

#pragma warn -8057

void __fastcall TGKExplorerFrm::Splitter1Moved(TObject *Sender)
{
  StatusBar1->Panels->Items[0]->Width =  ModulesTree->Width;
}
//---------------------------------------------------------------------------
#pragma warn .8057

void    __fastcall TGKExplorerFrm::begin_gk_command(TCHAR * text)
{
 this->CommandStatus = text;
}

void    __fastcall TGKExplorerFrm::end_gk_command(GKHANDLE mod,LRESULT ret,DWORD error)
{
 wchar_t err_text[1024];
 TGKModuleInterface i(mod,true);
 *err_text = 0;
 AnsiString str;

 switch(ret)
 {
  case GKH_RET_ERROR    :
  i.get_error_text(error,err_text,sizeof(err_text)/sizeof(wchar_t));
  str = load_rcstring(ModuleInstance,IDS_ENDCMD_ERROR );// "Ошибка:";
  str +=err_text;
  break;
  case GKH_RET_EXCEPTION:
  {
   except_record  er;
   i.get_except_record(&er);/*"командa вызвалa исключение %X"*/
   str.sprintf(AnsiString(load_rcstring(ModuleInstance,IDS_ENDCMD_EXCEPT)).c_str(),er.except_rec.ExceptionCode);
  }
  break;
  case GKH_RET_CANCEL :
  str = load_rcstring(ModuleInstance,IDS_ENDCMD_CANCEL);//"команда отмененa";
  break;
  default:
  str = load_rcstring(ModuleInstance,IDS_ENDCMD_SUCCESS);//"выполненo успешно";
  break;
 }
 CommandStatus= CommandStatus+'-'+str;
}

#pragma warn -8057

void __fastcall TGKExplorerFrm::MiAboutClick(TObject *Sender)
{
 PageControl1->ActivePageIndex = 1;
}

//---------------------------------------------------------------------------

void __fastcall TGKExplorerFrm::N6Click(TObject *Sender)
{
 /*Определение модуля по адресу*/
  if(this->mod_def==NULL)
    {
     mod_def = new TModuleDefine(this);
     mod_def->Show();
    }
    else
    mod_def->SetFocus();

}
//---------------------------------------------------------------------------


void __fastcall TGKExplorerFrm::ModulesTreeDblClick(TObject *Sender)
{
 if(GetAsyncKeyState(VK_LSHIFT))
  MiViewModuleClick(this);
   else
  MiSetupModuleClick(this);
}
//---------------------------------------------------------------------------





void __fastcall TGKExplorerFrm::N1Click(TObject *Sender)
{
  TRect r;
  GetMenuItemRect(Handle,MainMenu1->Handle,N1->MenuIndex,&r);
  PopupMenu1->Popup(r.left,r.top+r.Height());
}
//---------------------------------------------------------------------------

void __fastcall TGKExplorerFrm::ModulesTreeKeyPress(TObject *Sender,
      char &Key)
{
   if(Key == 0x0D)
    {
      MiSetupModuleClick(this);
    }
}
//---------------------------------------------------------------------------
#pragma warn .8057

int     __fastcall TGKExplorerFrm::ShowModal()
{
  int ret = TGKModuleForm::ShowModal();
  return ret;
}






#pragma warn -8057
void __fastcall TGKExplorerFrm::miGenTrialLicenseClick(TObject *Sender)
{
  //Генерация Триал-лицензии
}
//---------------------------------------------------------------------------

void __fastcall TGKExplorerFrm::miRegisterModuleClick(TObject *Sender)
{
 //Регистрация модуля
 begin_gk_command(L"Активация лицензии");
 GKHANDLE mod = GetGKHandle(ModulesTree->Selected);
 LRESULT  ret = GKH_RET_ERROR;
 TActivationForm * af = new TActivationForm(this);
 if(af->ShowModal() == mrOk)
 {

  ret = call_gkhandle(mod,MCMD_LICENSE_ACTIVATE,(LPARAM)af->ActivationCode->Text.c_str(),af->ActivationCode->Text.Length());
  ModulesTreeChange(this,ModulesTree->Selected);
 }
 else
 ::SetLastError(ERROR_CANCELLED);
 end_gk_command(mod,ret,GetLastError());
 delete af;
}
//---------------------------------------------------------------------------

void __fastcall TGKExplorerFrm::miGenPrelicenseDataClick(TObject *Sender)
{
  //Генерация данных для формирования лицензии
  UnicodeString q_text;
  if(InputQuery(L"Создание запроса на лицензию",L"Наименование запроса",q_text))
  {
    wchar_t dll_name[1024];
    TGKModuleInterface mod(GetGKHandle(ModulesTree->Selected),true);
    mod.get_dll_name(dll_name,KERTL_ARRAY_COUNT(dll_name));
    TKFName<wchar_t> fname;
    fname.set_fullname(dll_name);
    fname.set_ext(L".lic");
    TLicense lic;
    lic.generate_timed_license(dll_name,0,q_text.c_str());
    lic.close();
    UnicodeString text;
    text.printf(L"Отправьте файл %s\n\rПо адресу dostapazov@gmail.com\n\rдля формирования ключа активации",fname());
    MessageBoxW(Handle,text.c_str(),L"Запрос лицензии",MB_ICONINFORMATION);

  }
}
//---------------------------------------------------------------------------
#pragma warn .8057

void    __fastcall TGKExplorerFrm::BeforeDestruction(void)
{
  ModulesTree->Items->Clear();
  TGKModuleForm::BeforeDestruction();
}

void __fastcall TGKExplorerFrm::miSaveRegistrySettingsClick(TObject *Sender)
{
  if(SaveDialog1->Execute(Handle))
  {
    this->begin_gk_command(L"Сохранение наcтроек модуля");
    TGKModuleInterface mod(GetGKHandle(ModulesTree->Selected),true);
    LRESULT ret = mod.call(MCMD_SAVE_REGISTRY,(LPARAM)SaveDialog1->FileName.c_str(),0);
    end_gk_command(mod(),ret,GetLastError());
  }
}
//---------------------------------------------------------------------------

