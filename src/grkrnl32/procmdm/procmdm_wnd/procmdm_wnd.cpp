//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "procmdm_wnd.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "gkmodule_form"
#pragma link "FldrBrws"
#pragma resource "*.dfm"
TProcessMdmWnd *ProcessMdmWnd;
//---------------------------------------------------------------------------
__fastcall TProcessMdmWnd::TProcessMdmWnd(TComponent* Owner,HWND parent,GKHANDLE mod)
        : TGKModuleForm(Owner,parent,mod)
{
 #ifdef COMERCIAL_BUILD
 Label30->Caption = "ООО \"Компания промышленной автоматизации\"";
 #endif

}
//---------------------------------------------------------------------------

void     __fastcall TProcessMdmWnd::on_module_state(GKHANDLE mod,DWORD state)
{
  bool running = ((state & MODULE_STATE_RUNNING_MASK )== MODULE_STATE_RUNNING) ? true:false;
  bool stopped  = (state & MODULE_STATE_RUNNING_MASK ) ? false:true;
  StartBtn->Enabled = stopped;
  StopBtn->Enabled  = running;
  if(running)
     StatusBar1->Panels->Items[0]->Text =  "Работает";
  if(stopped)
     StatusBar1->Panels->Items[0]->Text =  "Остановлен";
}

void     __fastcall TProcessMdmWnd::on_module_config_change(GKHANDLE mod,LPMODULE_CONFIG_DATA mcd)
{
 LPPMSETTINGS pms =(LPPMSETTINGS)mcd->data;
 if(mcd->mask&CCFL_MODEM_NUMBER)
   settings.modem_number = pms->modem_number;
 if(mcd->mask&CCFL_CMDSHOW)
   settings.cmd_show = pms->cmd_show;
 if(mcd->mask&CCFL_PROCNAME)
    safe_strcpy(settings.proc_name,pms->proc_name);
 if(mcd->mask&CCFL_WORKDIR)
    safe_strcpy(settings.work_dir,pms->work_dir);
 if(mcd->mask&CCFL_CMDLINE)
    safe_strcpy(settings.cmd_line,pms->cmd_line);
 if(mcd->mask&CCFL_AUTORESTART)
   settings.auto_restart = pms->auto_restart;

  setup_config_controls();

}

void     __fastcall TProcessMdmWnd::after_set_gkhandle()
{
  TGKModuleForm::after_set_gkhandle();
  if(get_settings())
     setup_config_controls();
  on_module_state(mod_iface(),mod_iface.get_module_state());
  set_notify(mod_iface(),MNF_COMMON);
}

void     __fastcall TProcessMdmWnd::setup_config_controls()
{
 ModemNumber->Position = settings.modem_number;
 ExeName->Text         = settings.proc_name;
 WorkDir->Text   = settings.work_dir;
 CmdLine->Text   = settings.cmd_line;
 AutoRestart->Checked = settings.auto_restart;
 CmdShow->Checked = settings.cmd_show == SW_SHOW ? true:false;

 chmask = 0;

}

bool  __fastcall TProcessMdmWnd::get_settings()
{
 LPMODULE_CONFIG_DATA mcd;
 BYTE buffer[sizeof(*mcd)+sizeof(PMSETTINGS)];
 mcd = (LPMODULE_CONFIG_DATA)buffer;
 mcd->dw_sz = sizeof(buffer);
 mcd->mask  = -1;
 LPPMSETTINGS pms =(LPPMSETTINGS)mcd->data;
 ZeroMemory(pms,sizeof(*pms));
 pms->dw_size = sizeof(*pms);
 if(mod_iface.get_config_data(mcd,mcd->dw_sz))
   {
    memcpy(&settings,pms,sizeof(settings));
    return true;
   }
  return false;
}


void __fastcall TProcessMdmWnd::SpeedButton2Click(TObject *Sender)
{
 FolderBrowser1->Execute();
}
//---------------------------------------------------------------------------

void __fastcall TProcessMdmWnd::SpeedButton1Click(TObject *Sender)
{
 if(OpenDialog1->Execute())
   {
    ExeName->Text = OpenDialog1->FileName;
   }
}
//---------------------------------------------------------------------------


void __fastcall TProcessMdmWnd::Setchmask(DWORD value)
{
        //TODO: Add your source code here
        if(Fchmask!=value)
        {
         Fchmask = value;
        }
         ApplyBtn->Enabled = value ? true:false;
         UndoBtn ->Enabled = value ? true:false;
}
void __fastcall TProcessMdmWnd::ExeNameChange(TObject *Sender)
{
  AnsiString str = settings.proc_name;
  if(str.AnsiCompareIC(ExeName->Text))
    {
     chmask|=CCFL_PROCNAME;
    }
    else
    chmask&=~CCFL_PROCNAME;
    ExeName->Font->Color = (chmask&CCFL_PROCNAME) ? clHotLight:clDefault;

}
//---------------------------------------------------------------------------

void __fastcall TProcessMdmWnd::WorkDirChange(TObject *Sender)
{
  AnsiString str = settings.work_dir;
  if(str.AnsiCompareIC(WorkDir->Text))
    {
     chmask|=CCFL_WORKDIR;
    }
    else
    chmask&=~CCFL_WORKDIR;
    WorkDir->Font->Color = (chmask&CCFL_WORKDIR) ? clHotLight:clDefault;

}
//---------------------------------------------------------------------------

void __fastcall TProcessMdmWnd::CmdLineChange(TObject *Sender)
{
  AnsiString str = settings.cmd_line;
  if(str.AnsiCompareIC(CmdLine->Text))
    {
     chmask|=CCFL_CMDLINE;
    }
    else
    chmask&=~CCFL_CMDLINE;
    CmdLine->Font->Color = (chmask&CCFL_CMDLINE) ? clHotLight:clDefault;

}
//---------------------------------------------------------------------------

void __fastcall TProcessMdmWnd::UndoBtnClick(TObject *Sender)
{
 if(get_settings())
    this->setup_config_controls();
}
//---------------------------------------------------------------------------


void __fastcall TProcessMdmWnd::ApplyBtnClick(TObject *Sender)
{
 WideString wstr;
 LPMODULE_CONFIG_DATA mcd;
 BYTE buffer[sizeof(*mcd)+sizeof(PMSETTINGS)];
 mcd = (LPMODULE_CONFIG_DATA)buffer;
 mcd->dw_sz = sizeof(buffer);
 mcd->mask  = chmask;
 LPPMSETTINGS pms =(LPPMSETTINGS)mcd->data;
 ZeroMemory(pms,sizeof(*pms));
 pms->dw_size = sizeof(*pms);
 memcpy(pms,&settings,sizeof(settings));
 if(chmask&CCFL_MODEM_NUMBER)
   pms->modem_number = ModemNumber->Position;
 //if(chmask&CCFL_CMDSHOW)
 //   pms->cmd_show = ?;
 if(chmask&CCFL_PROCNAME)
    {
     wstr = ExeName->Text;
     safe_strcpy(pms->proc_name,wstr.c_bstr());
    }
 if(chmask&CCFL_WORKDIR)
    {
     wstr = WorkDir->Text;
     safe_strcpy(pms->work_dir,wstr.c_bstr());
    }
 if(chmask&CCFL_CMDLINE)
    {
     wstr = CmdLine->Text;
     safe_strcpy(pms->cmd_line,wstr.c_bstr());
    }
 if(chmask& CCFL_CMDSHOW)
   pms->cmd_show = CmdShow->Checked ? SW_SHOW:SW_HIDE;
 if(chmask& CCFL_AUTORESTART)
   pms->auto_restart = (DWORD)AutoRestart->Checked ;

      
 mod_iface.set_config_data(mcd,sizeof(buffer));

}
//---------------------------------------------------------------------------


void __fastcall TProcessMdmWnd::ModemNumberChangingEx(TObject *Sender,
      bool &AllowChange, short NewValue, TUpDownDirection Direction)
{
  if(settings.modem_number!=(DWORD) NewValue)
    {
      chmask|=CCFL_MODEM_NUMBER;
    }
    else
     chmask&=~CCFL_MODEM_NUMBER;
    ModemNumberEdit->Font->Color = (chmask&CCFL_MODEM_NUMBER) ? clHotLight:clDefault;

}
//---------------------------------------------------------------------------

void     __fastcall TProcessMdmWnd::handle_call_result(LRESULT ret)
{
  DWORD err = GetLastError();
  wchar_t err_text[MAX_PATH];
  err_text[0] =0;
  mod_iface.get_error_text(err,err_text,sizeof(err_text)/sizeof(wchar_t));
  StatusBar1->Panels->Items[1]->Text = err_text;
}

void __fastcall TProcessMdmWnd::StopBtnClick(TObject *Sender)
{                                     
 handle_call_result(mod_iface.stop(MODULE_STOP_REQUEST));
}
//---------------------------------------------------------------------------

void __fastcall TProcessMdmWnd::StartBtnClick(TObject *Sender)
{
 handle_call_result(mod_iface.start(MODULE_START_REQUEST,0));
}
//---------------------------------------------------------------------------

void __fastcall TProcessMdmWnd::AutoRestartClick(TObject *Sender)
{
  if(settings.auto_restart!= (BOOL)AutoRestart->Checked)
     chmask|=CCFL_AUTORESTART;
     else
     chmask&=~CCFL_AUTORESTART;
   AutoRestart->Font->Color =(chmask&CCFL_AUTORESTART) ? clHotLight:clDefault;
}
//---------------------------------------------------------------------------

void __fastcall TProcessMdmWnd::CmdShowClick(TObject *Sender)
{
  int cmd_show = CmdShow->Checked ? SW_SHOW:SW_HIDE;
  if(settings.cmd_show != cmd_show)
     chmask|=CCFL_CMDSHOW;
     else
     chmask&=~CCFL_CMDSHOW;
   CmdShow->Font->Color =(chmask&CCFL_CMDSHOW) ? clHotLight:clDefault;

}
//---------------------------------------------------------------------------

