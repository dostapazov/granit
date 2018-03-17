//---------------------------------------------------------------------------

#include <stdio.h>

#include "gktlmdbfrm.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "gkmodule_form"
#pragma resource "*.dfm"
TGKTlmdbForm *GKTlmdbForm = NULL;
//---------------------------------------------------------------------------
__fastcall TGKTlmdbForm::TGKTlmdbForm(TComponent* Owner,HWND own_wnd,GKHANDLE h)
        : TGKModuleForm(Owner,own_wnd,h)
{

}
//---------------------------------------------------------------------------



void  __fastcall TGKTlmdbForm::beginDBcommand(DWORD cmd ,DWORD addr)
{
  /*Перед выполнением команды обновляем строку состояния*/
  char cmd_text[MAX_PATH<<1];
  int type = sotd_addr(addr).addrtype();
  int str_id = IDS_CMD_UNDEF;
  switch(cmd)
  {
   /*case TLMDBCM_CREATE:
   str_id = IDS_CMD_CREATE;
   type = (++type > OTD_ADDR_TYPE_SB) ? 0:type;
   break;
   case TLMDBCM_DELETE:
   str_id = IDS_CMD_DELETE;
   break;
   case TLMDBCM_MODIFY:
   str_id = IDS_CMD_RECREATE;
   break;*/
  }
  int len = LoadString(ModuleInstance,str_id,cmd_text,sizeof(cmd_text));
  cmd_text[len++] = ' ';
  LoadString(ModuleInstance,type+IDS_PU_TEXT,cmd_text+len,sizeof(cmd_text)-len);
  StatusBar1->Panels->Items[0]->Text = cmd_text;
  StatusBar1->Panels->Items[1]->Text = "";
}

void  __fastcall TGKTlmdbForm::endDBcommand  (DWORD ret_code)
{
 AnsiString text;
 wchar_t err_text[MAX_PATH];
 *err_text = 0;
 switch(ret_code)
 {
  case GKH_RET_SUCCESS:
       StatusBar1->Panels->Items[0]->Text = text;
  break;
  case GKH_RET_EXCEPTION:
       LoadStringW(ModuleInstance,IDS_CMD_EXCEPT,err_text,sizeof(err_text)/sizeof(wchar_t));
  break;
  case GKH_RET_CANCEL:
       LoadStringW(ModuleInstance,IDS_CMD_CANCEL,err_text,sizeof(err_text)/sizeof(wchar_t));
  break;
  default: mod_iface.get_error_text(GetLastError(),err_text,sizeof(err_text)/sizeof(wchar_t));
  break;
 }
 text = err_text;
 StatusBar1->Panels->Items[1]->Text = text;

}

void __fastcall TGKTlmdbForm::after_set_gkhandle()
{
 TGKModuleForm::after_set_gkhandle();
 wchar_t mod_name[MAX_PATH];
 mod_iface.get_module_name(mod_name,sizeof(mod_name)/sizeof(wchar_t));
 Caption = mod_name;
 on_module_state(mod_iface(),mod_iface.get_module_state());
 set_notify(mod_iface(),MNF_COMMON);
}


void                  __fastcall TGKTlmdbForm::on_module_state(GKHANDLE mod,DWORD state)
{
  bool running = ((state & MODULE_STATE_RUNNING_MASK )== MODULE_STATE_RUNNING) ? true:false;
  bool stopped  = (state & MODULE_STATE_RUNNING_MASK ) ? false:true;
  StartBtn->Enabled = stopped;
  StopBtn->Enabled  = running;
  if(running)
     StatusBar1->SimpleText = "Работатет";
  if(stopped)
     StatusBar1->SimpleText =  "Остановлен";
}

void                  __fastcall TGKTlmdbForm::on_module_config_change(GKHANDLE mod,LPMODULE_CONFIG_DATA mcd)
{
}


LRESULT __fastcall TGKTlmdbForm::on_gkhandle_notify(GKHANDLE from,LPNOTIFY_CODE nc,DWORD mask)
{
 return TGKModuleForm::on_gkhandle_notify(from,nc,mask);
}



LRESULT __fastcall TGKTlmdbForm::gkhandle_processing(DWORD cmd,LPARAM p1,LPARAM p2)
{
  LRESULT ret = GKH_RET_SUCCESS;
  ret = TGKModuleForm::gkhandle_processing(cmd,p1,p2);
  return ret;
}

void __fastcall TGKTlmdbForm::FormClose(TObject *Sender,
      TCloseAction &Action)
{
  if(Action == caHide && !owner_window)
     Action =  caFree;
}
//---------------------------------------------------------------------------


void __fastcall TGKTlmdbForm::MiDbLowClick(TObject *Sender)
{
  TDBLowForm *  dbfrm;
    dbfrm = new  TDBLowForm(this,mod_iface());
    dbfrm->Visible = true;
}
//---------------------------------------------------------------------------

void __fastcall TGKTlmdbForm::N6Click(TObject *Sender)
{
 Close();       
}
//---------------------------------------------------------------------------


void __fastcall TGKTlmdbForm::StartBtnClick(TObject *Sender)
{
 mod_iface.start(MODULE_START_REQUEST,NULL);
}
//---------------------------------------------------------------------------

void __fastcall TGKTlmdbForm::StopBtnClick(TObject *Sender)
{
 mod_iface.stop(MODULE_STOP_REQUEST);
}
//---------------------------------------------------------------------------



