//---------------------------------------------------------------------------
#pragma hdrstop
#include <vcl.h>

#include "MonFrame.h"
#include <stdio.h>

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TMonitorFrame *MonitorFrame;
//---------------------------------------------------------------------------
__fastcall TMonitorFrame::TMonitorFrame(TComponent* Owner)
        : TFrame(Owner)
{

}
//---------------------------------------------------------------------------

void __fastcall TMonitorFrame::BeforeDestruction(void)
{
 TFrame::BeforeDestruction();
 mon.Destroy();
}

void __fastcall TMonitorFrame::create_mon_instance()
{
  if(NULL == mon())
  {
  KeWin::TRect r(0,0,MonitorBox->Width,MonitorBox->Height);
  r.Grow(-10,-12);
  r.Move(4,8);
  try{
      char txtv_name [MAX_PATH];
      safe_strcpy(txtv_name,"VTDMonitor");
      int name_len = lstrlen(txtv_name);
      int create_limit = 128;
      int create_cntr = 0;
      while(!mon.DoCreate(MonitorBox->Handle,r,-1,256,txtv_name) && create_cntr<create_limit)
      {
        wsprintf(txtv_name+name_len,"%d",create_cntr++);
      }
     }
    catch(...)
     { }
   }
      if(mon())
      {
        HFONT font = NULL;
        LOGFONT log_font = {0};
        log_font.lfHeight   = -12;
        log_font.lfCharSet  = RUSSIAN_CHARSET;
        strcpy(log_font.lfFaceName ,"Courier New");
        if(log_font.lfFaceName[0])
         font = CreateFontIndirect(&log_font);
        if(!font)
        {
        font = CreateFont(-12,0,0,0,FW_NORMAL ,0,0,0,
                                Font->Charset,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH,
                                "Courier New");
         GetObject(font,sizeof(log_font),&log_font);
        }
        font = mon.SetFont(font,false);

        //mon.SetBkColor(mon_bkgnd_color);
        mon.set_lines_limit(8192);
        DeleteObject(font);
     }
  }

  
void    __fastcall TMonitorFrame::monitor_vtd_events(DWORD code,LPVOID data, DWORD sz )
{
  switch(code)
  {
   case VTDNC_TX         :monitor_txrx((LPBYTE)data,sz,true );break;
   case VTDNC_RX         :monitor_txrx((LPBYTE)data,sz,false);break;

   case VTDNC_TXKADR_STD :monitor_kadr_std(data,true);
                          break;
   case VTDNC_RXKADR_STD :monitor_kadr_std(data,false);
                          break;
   case VTDNC_RXKADR_VG  :break;
   case VTDNC_RXKADR_Y   :break;
  }
}

void __fastcall TMonitorFrame::monitor_kadr_std( LPVOID data,bool tx)
{
 if(this->ShowDetail->Checked)
 {
 lpvtd_resp_kadr  rk  = (lpvtd_resp_kadr )data;
 lpvtd_query_kadr qk =  (lpvtd_query_kadr)data;
 
 char text [1024];
 switch(rk->command)
 {
  case VTD_QUERY_DATE_TIME  :
                             if(tx)
                             {
                              sprintf(text,"Счётчик № %03d ВЫЗОВ текущей даты ,времени заводского номера вычислителя",qk->dev_number);
                              mon.add_line(text);
                             }
                             else
                             mon_date_time_std((lpvtd_num_date_time)rk->data);
                             break;
  case VTD_QUERY_BASE_CONFIG:
                             if(tx)
                             {
                              sprintf(text,"Счётчик № %03d ВЫЗОВ основных параметров конфигурации",qk->dev_number);
                              mon.add_line(text);
                             }
                             else
                             mon_base_config_std((lpvtd_base_config)rk->data);break;
  case VTD_QUERY_PARAM      :
                             if(tx)
                             {
                              sprintf(text,"Счётчик № %03d ВЫЗОВ параметров %s № %d c %d по %d",
                              qk->dev_number,qk->Npipe&VTD_USER_PARAM?"потребителя":" трубы",qk->Npipe&(~VTD_USER_PARAM),qk->Npar,qk->CNTpar);
                              mon.add_line(text);
                             }
                             else
                             mon_param_std(rk);break;
  case VTD_QUERY_CURRENT_PARAM :
                             if(tx)
                             {
                              sprintf(text,"Счётчик № %03d ВЫЗОВ текущих параметров %s № %d",
                              qk->dev_number,qk->Npipe&VTD_USER_PARAM?"потребителя":" трубы",qk->Npipe&(~VTD_USER_PARAM));
                              mon.add_line(text);
                             }
                             else
                             mon_curr_param(rk);break;

 }
 }
}

void __fastcall print_vtd_dt(char * text,lpvtd_date vdt,lpvtd_time vtm,char * pref = 0)
{
 int len = 0;
 if(pref)
    {
     len += lstrlen(pref);
     strcpy(text,pref);
    }
 if(vdt)
 len+=sprintf(text+len,"%02d-%02d-%02d%s",vdt->day,vdt->mon,vdt->year,vtm ?" ":"" );
 if(vtm)
  sprintf(text+len,"%02d:%02d:%02d",vtm->hour,vtm->min,vtm->sec );
}


void __fastcall TMonitorFrame::mon_date_time_std(LPVOID data)
{
 lpvtd_num_date_time vdt = (lpvtd_num_date_time)data;
 char text[MAX_PATH];
 mon.add_line(
"Обработка  заводского номера вычислителя, текущих даты и времени вычислителя,\
даты и времени пуска узлов вычислителя на счет:");
         sprintf(text,"заводской номер - %d%d%d%d%d%d%d%d",
                                                (DWORD)vdt->c_code.dig1
                                               ,(DWORD)vdt->c_code.dig2
                                               ,(DWORD)vdt->c_code.dig3
                                               ,(DWORD)vdt->c_code.dig4
                                               ,(DWORD)vdt->c_code.dig5
                                               ,(DWORD)vdt->c_code.dig6
                                               ,(DWORD)vdt->c_code.dig7
                                               ,(DWORD)vdt->c_code.dig8
                                               );

   mon.add_line(text);
   int len = sprintf(text,"Текущее дата/время ");
   print_vtd_dt(text+len,&vdt->curr_date,&vdt->curr_time);
   mon.add_line(text);
   for(int i = 0;i<10;i++)
   {
   

   len = sprintf(text,"Запуск потребителя № %d на счёт :", i);
   print_vtd_dt(text+len,&vdt->start_dt[i].date,&vdt->start_dt[i].time);
   mon.add_line(text);

   }

}

char vtd_std_convert_char(char ch)
{
 switch(ch)
 {
  case 0x0A: ch = '-'; break;
  case 0x0B: ch = 'E'; break;
  case 0x0C: ch = 'M'; break;
  case 0x0D:ch = 'W'; break;
  case 0x0E: ch = 'P'; break;
  case 0x0F: ch = ' '; break;
  default: ch+='0';
 }
 return ch;
}

int __fastcall vtd_std_convert_disp(char * out,vtd_digits & dig)
{
 *out = vtd_std_convert_char(dig.dig1);
 out++;
 *out = vtd_std_convert_char(dig.dig2);
 out++;
 *out = vtd_std_convert_char(dig.dig3);
 out++;
 *out = vtd_std_convert_char(dig.dig4);
 out++;
 *out = vtd_std_convert_char(dig.dig5);
 out++;
 *out = vtd_std_convert_char(dig.dig6);
 out++;
 *out = vtd_std_convert_char(dig.dig7);
 out++;
 *out = vtd_std_convert_char(dig.dig8);
 out++;
 *out =0;
 return 8;
}

void __fastcall TMonitorFrame::mon_base_config_std(LPVOID data)
{
  lpvtd_base_config bc = (lpvtd_base_config )data;
  char text[1024];
  int len;
  mon.add_line("Основные параметры конфигурации труб и потребителей");

  for(int i = 0;i<10;i++)
  {

    len = sprintf(text,"№ %02d ",i);
    len+= vtd_std_convert_disp(text+len,bc->ki[i]);
          sprintf(text+len," Pj01 = %4.2f Pj00 { %s }",bc->Pj01[i].Pj01,bc->Pj00[i].Pj00,bc->Pj00[i].Pj00);
    mon.add_line(text);
  }
}

void __fastcall TMonitorFrame::mon_param_std(lpvtd_resp_kadr rk)
{
 char text[MAX_PATH];
 int len;
 DWORD   p_count = rk->data_len/sizeof(float);
 float * param = (float*)  rk->data;
 float * param_end = param+p_count;
 sprintf(text,"Параметры . количество %d",p_count);
 mon.add_line(text);
 while(param<param_end)
 {
  sprintf(text,"%.2f",*param);
  ++param;
  mon.add_line(text);
 }


}

void __fastcall TMonitorFrame::mon_curr_param(lpvtd_resp_kadr rk)
{
  char text[1024];
  if(rk->data_len == 244)
  {
   //Текущие параметры труб
   lpvtd_pipe_curr_param  cp = (lpvtd_pipe_curr_param)rk->data;
   sprintf(text,"Текущие параметры труб время %02d:%02d:%02d",(DWORD)cp->time.hour,(DWORD)cp->time.min,(DWORD)cp->time.sec);
   mon.add_line(text);
   sprintf(text,"| %3c | %16s | %16s | %16s | %16s | %16s | %16s |",'№',"P","T пр","Т обр","Расход","Масса","Мощность");
   mon.add_line(text);
   for(int i = 0;i<10;i++)
   {
    sprintf(text,"| %03d | %16.2f | %16.2f | %16.2f | %16.2f | %16.2f | %16.2f |",i,cp->pressure[i],cp->temp[i],cp->temp_back[i],cp->Gc[i],cp->Vc[i],cp->Nk[i]);
    mon.add_line(text);

   }
  }
  else
  {
   //текущие параметры потребителей
   lpvtd_user_curr_param  cp = (lpvtd_user_curr_param)rk->data;
   sprintf(text,"Текущие параметры потребителя");
   mon.add_line(text);
   sprintf(text,"| %3c | %16s | %16s | %16s | %16s |",'№',"Энергия","P утечек","М утечек","По льготе");
   mon.add_line(text);
   for(int i = 0;i<10;i++)
   {
    sprintf(text,"| %03d | %16.2f | %16.2f | %16.2f | %16.2f |",i,cp->W[i],cp->Qc[i],cp->Vc[i],cp->Wl[i]);
    mon.add_line(text);
   }
  }
}




void __fastcall TMonitorFrame::FrameResize(TObject *Sender)
{
 if(mon())
 {
   mon.SetSize(MonitorBox->Width-8,MonitorBox->Height-12);
 }
 else
 this->create_mon_instance();
}
//---------------------------------------------------------------------------

void __fastcall TMonitorFrame::monitor_txrx(LPBYTE data,DWORD sz,bool tx)
{
 if(sz && ShowTXRX->Checked)
 {
 char text[2048];
 SYSTEMTIME st;
 GetLocalTime(&st);
 sprintf(text,"%02d:%02d:%02d,%03d %s длина %03d байт",(DWORD)st.wHour,(DWORD)st.wMinute,(DWORD)st.wSecond,(DWORD)st.wMilliseconds,tx ? "Передача":"Приём",sz);
 mon.add_line(text);
 int len = 0;
   for (DWORD i = 0;i<sz;++i)
   {
    len+=sprintf(text+len,"%02X ",(DWORD)*data++);
    if(len>32*3)
      mon.add_line(text),len = 0;

   }
  mon.add_line(text);
 }

}

void __fastcall TMonitorFrame::SpeedButton1Click(TObject *Sender)
{
  mon.clear();      
}
//---------------------------------------------------------------------------

