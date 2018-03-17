//---------------------------------------------------------------------------

#pragma warn -8058
#include <vcl.h>
#pragma warn .8058
#pragma hdrstop

#include "sc_monitor.h"
#include <stdio.h>
#include "grscfrm_rh.h"
#include <clipbrd.hpp>
#include <TRegstry.hpp>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "gkmodule_form"
#pragma resource "*.dfm"
TSCMonitor *SCMonitor;
//---------------------------------------------------------------------------
#define FTXAFB_RECEIPT   0
#define FTXAFB_CALLDATA  1
#define FTXAFB_CALLTS    2
#define FTXAFB_CALLTIT   3
#define FTXAFB_CALLTII   4
#define FTXAFB_TUTR      5


char * radix_str[] =
{
"%03o",
"%03d",
"%02X"
};

__fastcall TSCMonitor::TSCMonitor(TComponent* Owner,GKHANDLE handle)
        : TGKModuleForm(Owner,NULL,handle)
{

  #ifdef _DEBUG
  Button1->Visible = true;
  #else
  Button1->Visible = false;
  #endif
  
  settings_change  = false;
  ZeroMemory(&log_font,sizeof(log_font));
  mon_success =  mon_pause  = false;
  fmon_radix = 2;
  filter_addr.addr = -1;
  filter_mode = SCMON_FALL;
  frx_enable = ftx_enable;
  frx_cp = frx_type = frx_afb = frx_group = frx_inf =
  ftx_cp = ftx_type = ftx_afb = 255;
  ftx_enable = frx_enable = true;
  ToolButton1Click(ToolButton4);
  ParamBox->Width = 0;
}

//---------------------------------------------------------------------------
void __fastcall TSCMonitor::MonClearBtnClick(TObject *Sender)
{
  if(monitor)
     monitor->clear();
}
//---------------------------------------------------------------------------

void __fastcall TSCMonitor::MonPauseBtnClick(TObject *Sender)
{
  mon_pause = !mon_pause;
  MonPauseBtn->AllowAllUp = !mon_pause;
  MonPauseBtn->Down = mon_pause;
}
//---------------------------------------------------------------------------

void __fastcall TSCMonitor::FilterBtnClick(TObject *Sender)
{
  ParamBox->Width = 140;
}
//---------------------------------------------------------------------------



void __fastcall TSCMonitor::ToolButton1Click(TObject *Sender)
{
  TComponent * comp = dynamic_cast<TComponent *>(Sender);
  if(comp)
  {
   fmon_radix = comp->Tag;
   change_filter(filter_addr.addr);
   RxCPNumber  =  frx_cp;
   RxGRPNumber =  frx_group;
   RxAFBNumber =  frx_afb;
   TxCPNumber  =  ftx_cp;
   TxAFBNumber =  ftx_afb;

  }
}
//---------------------------------------------------------------------------

void __fastcall TSCMonitor::miFALLClick(TObject *Sender)
{
 TComponent * comp = dynamic_cast<TComponent *>(Sender);
 filter_mode = comp->Tag;
 FlowFilterText->Visible = filter_mode == SCMON_FFLOW;
 ParamBox->Width = (filter_mode == SCMON_FFILTER) ? 140:0;

 if(monitor)
   monitor->SetSize(MonitorBox->Width-10,MonitorBox->Height-10);

}
//---------------------------------------------------------------------------

void __fastcall TSCMonitor::FormResize(TObject *Sender)
{
  if(monitor)
     {
      monitor->SetSize(MonitorBox->Width-10,MonitorBox->Height-10);
     }
}
//---------------------------------------------------------------------------

void __fastcall TSCMonitor::FormClose(TObject *Sender,
      TCloseAction &Action)
{
   if(settings_change)
      write_settings();
   if(Action == caHide)
      Action = caFree;
}
//---------------------------------------------------------------------------

void __fastcall TSCMonitor::BeforeDestruction(void)
{
  if(monitor)
     {
      monitor->Destroy();
      delete monitor;
     }
   if(settings_change)
      write_settings();
 TGKModuleForm::BeforeDestruction();
}

void     __fastcall TSCMonitor::on_module_state(GKHANDLE mod,DWORD state)
{
}

void     __fastcall TSCMonitor::on_module_config_change(GKHANDLE mod,LPMODULE_CONFIG_DATA mcd)
{
}

void     __fastcall TSCMonitor::after_set_gkhandle()
{

  mon_bkgnd_color     =  GetSysColor(COLOR_INFOBK);
  rx_text_color       =  RGB(0,255,0);
  tx_text_color       =  RGB(0,0,255);
  err_text_color      =  RGB(255,0,0);
  Inherit::after_set_gkhandle();
  read_settings();
}

void __fastcall TSCMonitor::read_settings()
{
  MODULE_INIT mi;
  wchar_t  reg_key_name[MAX_PATH];
  ZeroMemory(&mi,sizeof(mi));
  mi.dw_size = sizeof(mi);
  mi.reg_key_name_size = MAX_PATH;
  mi.reg_key_name  =reg_key_name;
  if(mod_iface.get_config_params(&mi))
  {
  TRegsReader rr(DupKey(mi.reg_key));
  if(rr.OpenKey(REG_KEY_SCMON))
  {
    mon_bkgnd_color     =  rr.ReadDword(REG_DW_MONBKGND,GetSysColor(COLOR_INFOBK));
    rx_text_color       =  rr.ReadDword(REG_DW_TXTEXTCOLOR ,RGB(0,255,0));
    tx_text_color       =  rr.ReadDword(REG_DW_RXTEXTCOLOR ,RGB(255,0,0));
    err_text_color      =  rr.ReadDword(REG_DW_ERRTEXTCOLOR,RGB(0,0,255));
    rr.ReadBytes(REG_VAL_MONFONT,&log_font,sizeof(log_font));
  }
  }
}

void __fastcall TSCMonitor::write_settings()
{
 if(settings_change)
 {
  MODULE_INIT mi;
  wchar_t  reg_key_name[MAX_PATH];
  ZeroMemory(&mi,sizeof(mi));
  mi.dw_size = sizeof(mi);
  mi.reg_key_name_size = MAX_PATH;
  mi.reg_key_name  =reg_key_name;
  if(mod_iface.get_config_params(&mi))
  {
  TRegsWriter rw(DupKey(mi.reg_key));
  if(rw.OpenKey(REG_KEY_SCMON,true))
  {
    mon_bkgnd_color     =  rw.WriteDword(REG_DW_MONBKGND    ,mon_bkgnd_color);
    rx_text_color       =  rw.WriteDword(REG_DW_TXTEXTCOLOR ,rx_text_color  );
    tx_text_color       =  rw.WriteDword(REG_DW_RXTEXTCOLOR ,tx_text_color  );
    err_text_color      =  rw.WriteDword(REG_DW_ERRTEXTCOLOR,err_text_color );
    rw.WriteBytes(REG_VAL_MONFONT,&log_font,sizeof(log_font));
  }
   settings_change = false;
  }
 } 

}


LRESULT  __fastcall TSCMonitor::on_gkhandle_notify (GKHANDLE from,LPNOTIFY_CODE nc,DWORD mask)
{
 if(mask  == MNF_SCTXRX)
 {
  DWORD len = nc->dw_size-sizeof(*nc)+sizeof(nc->notify_data[0]);
  if(nc->notify_code == MNCODE_SCTXRX_ERROR)
     monitor_error(nc->notify_data[0]);
  else
    monitor_data (nc->notify_code == MNCODE_SCTX ? true:false,(LPMIO_REQUEST)nc->notify_data,len);
  return GKH_RET_SUCCESS;
 }
 return Inherit::on_gkhandle_notify(from,nc,mask);
}

void     __fastcall TSCMonitor::separate_line(DWORD color)
{
   char buf[2048];
   int len = KeRTL::MIN((DWORD)sizeof(buf)-1,monitor->get_max_line_len());
   memset(buf,0x97,len);
   buf[len] = 0;
   monitor->add_line(buf,color);
}

void     __fastcall TSCMonitor::monitor_error(DWORD  err)
{
 if( filter_mode!=1 || NoErrorsPrint->Checked == false)
 {
   wchar_t err_text[MAX_PATH];
   *err_text = 0;
   mod_iface.get_error_text(err,err_text,sizeof(err_text)/sizeof(wchar_t));
   if(*err_text)
      {
       AnsiString s(err_text);
       s.Delete(s.Length()-1,2);
       monitor->add_line(s.c_str(),err_text_color);
      }
 }
}



void     __fastcall TSCMonitor::monitor_data(bool tx,LPMIO_REQUEST io_req,DWORD sz)
{

 if(!mon_pause)
 {
  DWORD color = tx ? tx_text_color:rx_text_color;
  TCHAR buf[1024];
  if(mon_success)
    this->separate_line(color);
  mon_success = false;

  if((filter_mode!=SCMON_FFILTER) || ((tx && ftx_enable) || (!tx&&frx_enable)))
  {
  DWORD error;
  gr_proto gp ;
  gp.dw_size = sizeof(gp);
  error = io_req->channel == 255 ? 0:abs((int)gr_proto_parser((LPBYTE)io_req->data,io_req->data_len,&gp));
  SYSTEMTIME st;
  GetLocalTime(&st);
  TCHAR rxtx_type[32];
  TCHAR parse_error[128];
  LoadString(ModuleInstance,STR_MON_RXTYPE+(int)tx,rxtx_type,sizeof(rxtx_type));
  LoadString(ModuleInstance,STR_MON_ERR0+error,parse_error,sizeof(parse_error));
  sprintf(buf,_T("%02d.%02d.%02d %02d:%02d:%02d:%03d  %s %3d : %s"),
          (DWORD)st.wDay,(DWORD)st.wMonth,(DWORD)st.wYear,
          (DWORD)st.wHour,(DWORD)st.wMinute,(DWORD)st.wSecond,(DWORD)st.wMilliseconds,
          rxtx_type,io_req->channel,parse_error);
  if(error)
     {
        if(filter_mode!=SCMON_FFILTER || NoErrorsPrint->Checked == false)
        {
         monitor->add_line(buf,err_text_color);
         monitor_bytes((LPBYTE)io_req->data,io_req->data_len,color);
         mon_success = true;
        }
     }
     else
     {
      if(io_req->channel == 255)
      {
       if(filter_mode==SCMON_FFILTER && this->ShowSysChannel->Checked)
       {
        monitor->add_line(buf,err_text_color);
        monitor_bytes((LPBYTE)io_req->data,io_req->data_len,color);
        if(DetailEnabled->Checked)
        {
         
        }
       }
      }
      else
      {
       if(check_filter(gp,tx))
         {
           monitor->add_line(buf,color);
           monitor_bytes((LPBYTE)io_req->data,io_req->data_len,color);
           if(DetailEnabled->Checked)
             monitor_details(gp,color);
           mon_success = true;

         }
      }
     }
  }
 }

}

void     __fastcall TSCMonitor::monitor_bytes(LPBYTE data,DWORD len,DWORD color)
{
 int  text_len = 0;
 char text[MAX_PATH<<2];
 LPBYTE end = data+len;
 while(data<end)
 {
  text_len+=sprintf(text+text_len,radix_str[fmon_radix],(DWORD)*data++);
  text[text_len++] = ' ';
 }

 if(text_len)
   {
    text[text_len] = 0;
    monitor->add_line(text,color);
   }
}

void __fastcall get_proto_numbers(gr_proto & gp,DWORD & floor,DWORD & cp,DWORD & mod)
{
 floor = (DWORD) gp.cp->floor;
 cp    = (DWORD) gp.cp->cp_number;
 mod   = (DWORD) gp.type->afb;
 if(!cp)
     {
      cp = GRPROTO_MK_CPNUM(floor,mod);
      mod = 0;
     }
}


bool     __fastcall TSCMonitor::check_filter(gr_proto & gp,bool tx)
{
  DWORD  floor,cp,mod ;
  get_proto_numbers(gp,floor,cp,mod);

 if(filter_mode == SCMON_FFILTER)
  {
   if(tx)
   {
    if(ftx_cp != 255 && ftx_cp  != (DWORD)cp)
       return false;
    if(ftx_type!=255 && ftx_type!= (DWORD)gp.type->ktype)
       return false;
    if(ftx_afb!=255 )
    {
       switch(ftx_afb)
       {
       case FTXAFB_CALLDATA   : if(gp.type->afb<GRKTAFB_CALLTS ||gp.type->afb>GRKTAFB_CALLTII) return false;break;
       case FTXAFB_CALLTS     : if(gp.type->afb!=GRKTAFB_CALLTS ) return false;break;
       case FTXAFB_CALLTIT    : if(gp.type->afb!=GRKTAFB_CALLTIT) return false;break;
       case FTXAFB_CALLTII    : if(gp.type->afb!=GRKTAFB_CALLTII) return false;break;
       case FTXAFB_TUTR       : if(gp.type->afb<GRKTAFB_TUON ||gp.type->afb>GRKTAFB_CMD2) return false;break;
       }
    }
   }
   else
   {
    if(frx_cp != 255 && frx_cp  != (DWORD)cp)
       return false;
    if(frx_type!=255 && frx_type!= (DWORD)gp.type->ktype)
       return false;
    if(frx_afb!=255  && frx_afb != (DWORD)mod)
       return false;
    if(gp.info)
    {
     if(frx_inf !=255 && frx_inf != (DWORD)gp.info->inf)
       return false;
     if(frx_group !=255 && frx_group != (DWORD)gp.info->grp)
       return false;
    }
   }
   return true;
  }

 if(filter_mode == SCMON_FFLOW)
  {
    /*Фильтр следования за выбором*/
    if((DWORD)filter_addr.cp != cp )
    {
      gr_proto_cp cpn;
      cpn.cp_number = filter_addr.cp;
      if(gp.cp->line || cpn.floor != gp.cp->floor)
          return false;
    }


    if(!gp.info )
      {
       if(gp.type->ktype == GRKT_CALL   )
       {
         switch(gp.type->afb)
         {
          case  GRKTAFB_CALLTS : if(filter_addr.fa == OTD_FA_DISCRETE      ) return true;break;
          case  GRKTAFB_CALLTIT: if(filter_addr.fa == OTD_FA_ANALOG        ) return true;break;
          case  GRKTAFB_CALLTII: if(filter_addr.fa == OTD_FA_ANALOG_INTEGR ) return true;break;
         }
       }
      }
      else
      {
         switch(filter_addr.fa & OTD_FA_ALL)
         {
          case    OTD_FA_DISCRETE      : if(gp.info->inf != GRINF_TS  ) return false; break;
          case    OTD_FA_ANALOG        : if(gp.info->inf != GRINF_TIT ) return false; break;
          case    OTD_FA_ANALOG_INTEGR : if(gp.info->inf != GRINF_TII ) return false; break;
          case    OTD_FA_ALL      : break;
          default : return false;
         }

        if(gp.info->grp != filter_addr.sb && filter_addr.sb<OTD_ADDR_MAXVALUE)
         return false;
      }
  }
 return true;
}




void     __fastcall TSCMonitor::monitor_details(gr_proto & gp,DWORD color)
{
  
  TCHAR detail_templ1[64];
  TCHAR  detail_templ2[64];
  TCHAR  kadr_type    [64];
  TCHAR  kadr_inf     [64];
  TCHAR  kadr_inf1    [64];
  TCHAR  call_afbs    [64];



  LoadString(ModuleInstance,STR_MON_DETAIL2_1+fmon_radix,detail_templ2,sizeof(detail_templ2));
  LoadString(ModuleInstance,STR_MON_KADRTYPE0+(DWORD)gp.type->ktype,kadr_type,sizeof(kadr_type));
  LoadString(ModuleInstance,STR_MON_CALLAFB00+(DWORD)gp.type->afb,call_afbs,sizeof(call_afbs));

  DWORD  floor,cp,mod ;
  get_proto_numbers(gp,floor,cp,mod);
  TCHAR  buf[1024];
  int len ;

 if(gp.type->ktype == GRKT_ERROR)
    color = err_text_color;
 if(gp.type->ktype == GRKT_CALL)
    {
     LoadString(ModuleInstance,STR_MON_DETAIL3_1+fmon_radix,detail_templ1,sizeof(detail_templ1));
     len =sprintf(buf,detail_templ1,kadr_type,call_afbs,floor,cp);
     if(gp.type->afb == GRKTAFB_TRCANCEL  || ( gp.type->afb >= GRKTAFB_TUON && gp.type->afb <= GRKTAFB_CMD2))
     {
       LoadString(ModuleInstance,STR_MON_TUTR_DETAIL,call_afbs,sizeof(call_afbs));
       lpgr_proto_data_tu  dtu = (lpgr_proto_data_tu)gp.data;
       len+=sprintf(buf+len,call_afbs,gr_proto_poscode2val(dtu->pc_group,sizeof(dtu->pc_group)),gr_proto_poscode2val(dtu->pc_object,sizeof(dtu->pc_object)));
     }
    }
    else
    {
     LoadString(ModuleInstance,STR_MON_DETAIL1_1+fmon_radix,detail_templ1,sizeof(detail_templ1));
     len = sprintf(buf,detail_templ1,kadr_type,floor,cp,mod);
    }

  if(gp.info)
  {
  *kadr_inf1 = 0;
  LoadString(ModuleInstance,STR_MON_KADRINF0 +(((DWORD)gp.info->inf)&0x07),kadr_inf,sizeof(kadr_inf));
  if((gp.info->inf&GRINF_ERRFL))
   LoadString(ModuleInstance,STR_MON_KADRINF8 ,kadr_inf1,sizeof(kadr_inf1));
  len+=sprintf(buf+len,detail_templ2,
               (DWORD)gp.info->grp,
               kadr_inf,
               kadr_inf1);
  }

  if(len)
     monitor->add_line(buf,color);
}

void    __fastcall TSCMonitor::change_filter(DWORD addr)
{
  filter_addr.addr = addr;
  sotd_addr sa(addr);

  TCHAR fa_text[32];
  TCHAR buf    [1024];
  TCHAR templ  [256];

  LoadString(ModuleInstance,(sa.fa==OTD_ADDR_MAXVALUE) ? (DWORD)STR_MON_FAALL : (STR_MON_FA0+(DWORD)(sa.fa&3)),fa_text,sizeof(fa_text));
  LoadString(ModuleInstance,STR_MON_FILTER_TEMPL,buf,sizeof(buf));
  sprintf(templ,buf,radix_str[fmon_radix],_T("%s"),radix_str[fmon_radix]);
  sprintf(buf,templ,(DWORD)sa.cp,fa_text,(DWORD)sa.sb);
  FlowFilterText->Text = buf;
}


void __fastcall TSCMonitor::RxEnableClick(TObject *Sender)
{
  frx_enable = RxEnable->Checked;
}
//---------------------------------------------------------------------------

void __fastcall TSCMonitor::TxEnableClick(TObject *Sender)
{
  ftx_enable = TxEnable->Checked;
}
//---------------------------------------------------------------------------

void __fastcall TSCMonitor::RxCPChange(TObject *Sender)
{
  DWORD _val = RxCPNumber;
  frx_cp = _val;
}
//---------------------------------------------------------------------------

void __fastcall TSCMonitor::RxTYPEChange(TObject *Sender)
{
  frx_type;
  switch(RxTYPE->ItemIndex)
  {
   case  0 :frx_type = GRKT_DATAIN ; break;
   case  1 :frx_type = GRKT_READY  ; break;
   case  2 :frx_type = GRKT_NODATA ; break;
   case  3 :frx_type = GRKT_ERROR  ; break;
   default :frx_type = 255 ; break;
  }
}
//---------------------------------------------------------------------------

void __fastcall TSCMonitor::RxGRPChange(TObject *Sender)
{
   frx_group = RxGRPNumber;
}
//---------------------------------------------------------------------------

void __fastcall TSCMonitor::RxInfChange(TObject *Sender)
{
  switch(RxInf->ItemIndex)
  {
   case 0 : frx_inf = GRINF_TS;break;
   case 1 : frx_inf = GRINF_TIT;break;
   case 2 : frx_inf = GRINF_TII;break;
   default: frx_inf = 255;break;
  }
}
//---------------------------------------------------------------------------

void __fastcall TSCMonitor::RxAFBChange(TObject *Sender)
{
   frx_afb = RxAFBNumber;

}
//---------------------------------------------------------------------------

void __fastcall TSCMonitor::TxCPChange(TObject *Sender)
{
   ftx_cp = TxCPNumber;

}
//---------------------------------------------------------------------------

void __fastcall TSCMonitor::TxAFBChange(TObject *Sender)
{
  ftx_afb = TxAFBNumber;
}
//---------------------------------------------------------------------------

void __fastcall TSCMonitor::TxTypeChange(TObject *Sender)
{
  switch(TxType->ItemIndex)
  {
   case  FTXAFB_RECEIPT    :  ftx_type =  GRKT_RECEIPT;ftx_afb = FTXAFB_RECEIPT ;break;
   case  FTXAFB_CALLDATA   :  ftx_type =  GRKT_CALL   ;ftx_afb = FTXAFB_CALLDATA;break;
   case  FTXAFB_CALLTS     :  ftx_type =  GRKT_CALL   ;ftx_afb = FTXAFB_CALLTS  ;break;
   case  FTXAFB_CALLTIT    :  ftx_type =  GRKT_CALL   ;ftx_afb = FTXAFB_CALLTIT ;break;
   case  FTXAFB_CALLTII    :  ftx_type =  GRKT_CALL   ;ftx_afb = FTXAFB_CALLTII ;break;
   case  FTXAFB_TUTR       :  ftx_type =  GRKT_CALL   ;ftx_afb = FTXAFB_TUTR    ;break;
   default :  ftx_type =  255;ftx_afb = 255;      ;break;

  }
}
//---------------------------------------------------------------------------


void     __fastcall TSCMonitor::get_lines(KeRTL::TStream & stream)
{
  TCHAR buffer[MAX_PATH];
  for(DWORD i = 0; monitor && i<monitor->get_lines_count();i++)
  {
   DWORD len = monitor->get_line_text(i,buffer,sizeof(buffer)/sizeof(wchar_t));
   if(len)
      stream.Write(buffer,len);
   stream.Write((void*)L"\r\n",2*sizeof(wchar_t));
   //Application->ProcessMessages();
  }
  stream<<0;
}

void __fastcall TSCMonitor::SaveToFileClick(TObject *Sender)
{
 //Сохранение в файл
 if(monitor && OpenDialog1->Execute())
 {
  KeRTL::TFileStream stream(OpenDialog1->FileName.c_str(),false);
  get_lines(stream);
  stream.Flush();
 }
}
//---------------------------------------------------------------------------





void __fastcall TSCMonitor::CopyToClipboardClick(TObject *Sender)
{
 if(monitor)
 {
  TClipboard * clp = Clipboard();
  if(clp)
  {
   /*clp->Open();
   KeRTL::TMemoryStream stream(0x10000,0x10000);
   get_lines(stream);
   clp->SetTextBuf(stream.GetMemory());
   HGLOBAL hMem = GlobalAlloc(GMEM_DDESHARE,sizeof(int));
   int * locale = (int*) GlobalLock(hMem);
   if(locale)
   {
    *locale = MAKELCID(MAKELANGID(LANG_RUSSIAN,SUBLANG_DEFAULT),0);
    GlobalUnlock(hMem);
    clp->SetAsHandle(CF_LOCALE,(int)hMem);
   }
   if(hMem)
     GlobalFree(hMem);
   clp->Close();
   */
  }
 }
}
//---------------------------------------------------------------------------
void    __fastcall TSCMonitor::add_string(char * str,DWORD color)
{
 if(monitor && !mon_pause)
  {
   monitor->add_line(str,color ? color:err_text_color);
  }
}


DWORD    __fastcall a_toi(const char    * str){return atoi(str);}
//inline  DWORD    __fastcall a_toi(wchar_t * str){return _wtoi(str);}
typedef DWORD (__fastcall * cvt_func)(const char * );

cvt_func cvt_ftable[3] =
{
  octtoi
 ,a_toi
 ,hextoi
};

void __fastcall TSCMonitor::SetRxCPNumber(DWORD value)
{
 //TODO: Add your source code here
 AnsiString str;
 str.printf(radix_str[fmon_radix],value);
 RxCP->Text = str;
}
DWORD __fastcall TSCMonitor::GetRxCPNumber()
{
  AnsiString s = RxCP->Text;
  DWORD ret = 255 ;
  if(s.Length() && (fmon_radix>=0 && fmon_radix<=2))
  {
     const char * str = s.c_str() ;
     ret = cvt_ftable[fmon_radix](str);
  }
  return ret;
}

void __fastcall TSCMonitor::SetRxGRPNumber(DWORD value)
{
        //TODO: Add your source code here
 AnsiString str;
 str.printf(radix_str[fmon_radix],value);
 RxGRP->Text = str;
}
DWORD __fastcall TSCMonitor::GetRxGRPNumber()
{
        //TODO: Add your source code here
  AnsiString s = RxGRP->Text;
  DWORD ret = s.Length() ? cvt_ftable[fmon_radix](s.c_str()):255;
  return ret;
}

void __fastcall TSCMonitor::SetRxAFBNumber(DWORD value)
{
        //TODO: Add your source code here
 AnsiString str;
 str.printf(radix_str[fmon_radix],value);
 RxAFB->Text = str;
}

DWORD __fastcall TSCMonitor::GetRxAFBNumber()
{
        //TODO: Add your source code here
  AnsiString s = RxAFB->Text;
  DWORD ret = s.Length() ? cvt_ftable[fmon_radix](s.c_str()):255;
  return ret;
}

void __fastcall TSCMonitor::SetTxCPNumber(DWORD value)
{
        //TODO: Add your source code here
 AnsiString str;
 str.printf(radix_str[fmon_radix],value);
 TxCP->Text = str;
}

DWORD __fastcall TSCMonitor::GetTxCPNumber()
{

  AnsiString s =TxCP->Text;
  DWORD ret = s.Length() ? cvt_ftable[fmon_radix](s.c_str()):255;
  return ret;
}


void __fastcall TSCMonitor::SetTxAFBNumber(DWORD value)
{
 AnsiString str;
 str.printf(radix_str[fmon_radix],value);
 TxAFB->Text = str;
}
DWORD __fastcall TSCMonitor::GetTxAFBNumber()
{
  AnsiString s = TxAFB->Text;
  DWORD ret = s.Length() ? cvt_ftable[fmon_radix](s.c_str()) : 255;
  return ret;
}
void __fastcall TSCMonitor::FormShow(TObject *Sender)
{
  if(!monitor)
     create_monitor_view();
  if(monitor)
     {
      monitor->ShowWindow(SW_NORMAL);
      set_notify(mod_iface(),MNF_SCTXRX);
     }
}
//---------------------------------------------------------------------------

void __fastcall TSCMonitor::create_monitor_view()
{
  if(monitor)
    delete monitor;
    
  monitor = new KeWin::TTextViewer<TCHAR>;
  KeWin::TRect r(0,0,MonitorBox->Width,MonitorBox->Height);
  r.Grow(-10,-10);
  r.Move(4,8);
  if(monitor)
  {
  try{
      char txtv_name [MAX_PATH];
      safe_strcpy(txtv_name,"SCMonitor");
      int name_len = strlen(txtv_name);
      int create_limit = 128;
      int create_cntr = 0;
      while(!monitor->DoCreate(MonitorBox->Handle,r,-1,256,txtv_name) && create_cntr<create_limit)
      {
        sprintf(txtv_name+name_len,"%d",create_cntr++);
      }
   }
   catch( Exception & ex)
   {
    Caption = Caption+_T(" VCL::")+ex.Message;
    delete monitor;
    monitor = NULL;
   }
   catch(...)
   {
    Caption = Caption+" Oopps Unknown Exception ...";
    delete monitor;
    monitor = NULL;
   }
   }
      if(monitor && (*monitor)())
      {
      HFONT font = NULL;
      if(log_font.lfFaceName[0])
       font = CreateFontIndirect(&log_font);
      if(!font)
      {
      font = CreateFont(-12,0,0,0,FW_NORMAL ,0,0,0,
                              Font->Charset,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH,
                              _T("Courier New"));
       GetObject(font,sizeof(log_font),&log_font);                       
      }                        
      font = monitor->SetFont(font,false);

      monitor->SetBkColor(mon_bkgnd_color);
      monitor->set_lines_limit(8192);
      DeleteObject(font);
      }

}

void __fastcall TSCMonitor::FormHide(TObject *Sender)
{
    reset_notify(mod_iface(),-1);
}
//---------------------------------------------------------------------------

void __fastcall TSCMonitor::Button1Click(TObject *Sender)
{
  BYTE buf[2048];
  LPMIO_REQUEST  io_req = (LPMIO_REQUEST)buf;
  ZeroMemory(io_req,sizeof(*io_req));
  io_req->dw_size = sizeof(*io_req);
  io_req->data_size = io_req->data_len = 21;
  LPBYTE data = (LPBYTE)io_req->data;
  /*data[0] = 0x00 ;
  data[1] = 0x82 ;
  data[2] = 0x20 ;
  data[3] = 0x00 ;
  data[4] = 0x53 ;
  data[5] = 0xDC ;
  monitor_data(false,io_req,5+sizeof(*io_req));
  */
  memcpy(data,"\x04\x41\xA0\xFA\xF1\xF7\xFF\xFF\xFF\xFF\xFF\x05\x0E\x08\x00\x00\x00\x00\x00\x6C\xDC",21);
  monitor_data(false,io_req,20+sizeof(*io_req));


}
//---------------------------------------------------------------------------

void __fastcall TSCMonitor::N3Click(TObject *Sender)
{
  ColorDialog1->Color = (TColor)mon_bkgnd_color;
  if(ColorDialog1->Execute())
   {
     COLORREF ncolor = ColorDialog1->Color;
     if(ncolor!=mon_bkgnd_color)
       {
        mon_bkgnd_color = ncolor;
        monitor->SetBkColor(mon_bkgnd_color);
        monitor->Repaint();
        settings_change = true; 
       }

   }
}
//---------------------------------------------------------------------------
void __fastcall TSCMonitor::replace_moncolor(COLORREF old_color,COLORREF new_color)
{
 DWORD l_count = monitor->get_lines_count();
 for(DWORD i = 0;i<l_count;i++)
  {
   if(monitor->get_line_color(i) == old_color)
      monitor->set_line_color(i,new_color,FALSE);
  }
  monitor->Repaint();
 
}




void __fastcall TSCMonitor::N4Click(TObject *Sender)
{
  ColorDialog1->Color = (TColor)tx_text_color;
  if(ColorDialog1->Execute())
   {
     COLORREF ncolor = ColorDialog1->Color;
     if(ncolor!=tx_text_color)
       {
        replace_moncolor(tx_text_color,ncolor);
        tx_text_color = ncolor;
        settings_change = true;
       }

   }
}
//---------------------------------------------------------------------------

void __fastcall TSCMonitor::N5Click(TObject *Sender)
{
  ColorDialog1->Color = (TColor)rx_text_color;
  if(ColorDialog1->Execute())
   {
     COLORREF ncolor = ColorDialog1->Color;
     if(ncolor!=rx_text_color)
       {
        replace_moncolor(rx_text_color,ncolor);
        rx_text_color = ncolor;
        settings_change = true;
       }

   }
}
//---------------------------------------------------------------------------

void __fastcall TSCMonitor::N6Click(TObject *Sender)
{
  ColorDialog1->Color = (TColor)err_text_color;
  if(ColorDialog1->Execute())
   {
     COLORREF ncolor = ColorDialog1->Color;
     if(ncolor!=err_text_color)
       {
        err_text_color = ncolor;
        replace_moncolor(err_text_color,ncolor);
        settings_change = true;
       }

   }

}
//---------------------------------------------------------------------------

void __fastcall TSCMonitor::FormDestroy(TObject *Sender)
{
   if(settings_change)
      write_settings();
}
//---------------------------------------------------------------------------




void __fastcall TSCMonitor::N7Click(TObject *Sender)
{
  HFONT   font =  monitor->GetFont();
  GetObject(font,sizeof(log_font),&log_font);
  FontDialog1->Font->Handle = font;
  if(this->FontDialog1->Execute())
  {
   font = FontDialog1->Font->Handle;
   GetObject(font,sizeof(log_font),&log_font);
   settings_change = true;
  }

   //или новый или восстановить старый
   font = CreateFontIndirect(&log_font);
   if(font)
     {
      font = monitor->SetFont(font,true);
      DeleteObject(font);
     }

}
//---------------------------------------------------------------------------

void __fastcall TSCMonitor::FontDialog1Apply(TObject *Sender, HWND Wnd)
{
   HFONT font = FontDialog1->Font->Handle;
   LOGFONT lf;
   GetObject(font,sizeof(lf),&lf);
   GetObject(font,sizeof(lf),&lf);
   font = CreateFontIndirect(&lf);
   if(font)
     {
      font = monitor->SetFont(font,true);
      DeleteObject(font);
     }
}
//---------------------------------------------------------------------------


