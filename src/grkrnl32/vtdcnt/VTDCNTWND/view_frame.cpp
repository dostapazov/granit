//---------------------------------------------------------------------------
#pragma hdrstop
#include <vcl.h>

#include "view_frame.h"
#include "vtdsetup.h"
#include <stdio.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "gkmod_frame"
#pragma resource "*.dfm"
TViewFrame *ViewFrame;
//---------------------------------------------------------------------------
__fastcall TViewFrame::TViewFrame(TComponent* Owner)
        : TGKModuleFrame(Owner)
{

}
//---------------------------------------------------------------------------

void   __fastcall TViewFrame::set_number(DWORD n)
{
 bool update = Fnumber == n ? true:false;
 Fnumber = n;
 DWORD dn = LOWORD(n);
 DWORD gn = HIWORD(n);
 this->ViewDevPanel->Visible = false;
 this->ViewGrpPanel->Visible = false;
 if(dn!=(WORD)-1)
 {
  if(gn!=(WORD)-1)
   setup_group(dn,gn,update);
   else
   setup_device(dn);
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


void   __fastcall TViewFrame::setup_device(DWORD dn)
{
 DevListView->Clear();
 vtd_num_date_time dttm_null = {0};
 vtd_num_date_time dttm = {0};
 vtd_base_config   bc =   {0};
 if(GKH_RET_SUCCESS == mod_iface.call(VTDCMD_GET_NUMDATE_TIME,dn,(LPARAM)&dttm) &&
    GKH_RET_SUCCESS == mod_iface.call(VTDCMD_GET_BASE_CONFIG,dn,(LPARAM)&bc) &&
    memcmp(&dttm_null,&dttm,sizeof(dttm))
    )
 {

 char str[1024];
 sprintf(str,"%X%X%X%X%X%X%X%X",(DWORD)dttm.c_code.dig1
              ,(DWORD)dttm.c_code.dig2
              ,(DWORD)dttm.c_code.dig3
              ,(DWORD)dttm.c_code.dig4
              ,(DWORD)dttm.c_code.dig5
              ,(DWORD)dttm.c_code.dig6
              ,(DWORD)dttm.c_code.dig7
              ,(DWORD)dttm.c_code.dig8);

  PlantDevNumber->Caption = str;
  print_vtd_dt(str,&dttm.curr_date,&dttm.curr_time);
  DevDtTm->Caption = str;
  print_vtd_dt(str,&dttm.prevrep_date,0);
  PrevRepDt->Caption = str;
  print_vtd_dt(str,&dttm.lastrep_date,0);
  LastRepDt->Caption = str;


 for(int i = 0;i<10;)
 {
  TListItem * item = DevListView->Items->Add();
  print_vtd_dt(str,&dttm.start_dt[i].date,&dttm.start_dt[i].time);
  item->SubItems->Add(str);
  sprintf(str,"%.2f",bc.Pj01[i].Pj01);
  item->SubItems->Add(str);
  vtd_std_convert_disp(str,bc.ki[i]);
  item->SubItems->Add(str);
  item->SubItems->Add((char*)bc.Pj00[i].Pj00);
  item->Caption = ++i;
 }
  ViewDevPanel->Visible = true;
 }

}

void   __fastcall TViewFrame::setup_group (DWORD dn,DWORD gn,bool update)
{
   Label2->Visible = false;
   ViewGrpPanel->Visible = true;
   if(!update)
   {
     ViewGrpList->Columns->Clear();
     TListColumn * col = ViewGrpList->Columns->Add();
     col->Caption = '№';

     if(gn & VTD_CURR_PARAM)
     {
      if(gn& VTD_USER_PARAM)
      {
        //Текущие параметры трубы пользователя
       col = ViewGrpList->Columns->Add();
       col->Caption = "Энергия";
       col = ViewGrpList->Columns->Add();
       col->Caption = "P утеч";
       col = ViewGrpList->Columns->Add();
       col->Caption = "M утеч";
       col = ViewGrpList->Columns->Add();
       col->Caption = "По льготе";
      }
      else
      {
        //Текущие параметры трубы
       col = ViewGrpList->Columns->Add();
       col->Caption = "P";
       col = ViewGrpList->Columns->Add();
       col->Caption = "T пр";
       col = ViewGrpList->Columns->Add();
       col->Caption = "T обр";
       col = ViewGrpList->Columns->Add();
       col->Caption = "Расход";
       col = ViewGrpList->Columns->Add();
       col->Caption = "Масса";
       col = ViewGrpList->Columns->Add();
       col->Caption = "Мощьность";

      }
     }
     else
     {
       col = ViewGrpList->Columns->Add();
       col->Caption = "Значение";
     }
   }

   BYTE buff[2048];
   LPVTD_GET_GROUP_DATA dg = (LPVTD_GET_GROUP_DATA) buff;
   ZeroMemory(dg,sizeof(*dg));
   dg->dw_size = sizeof(buff);
   dg->device  = dn;
   dg->group   = gn;
   dg->parts   = OTD_PROTO_PART_DIAG|OTD_PROTO_PART_DATA;
   if(mod_iface.call(VTDCMD_GET_GROUP_DATA,(LPARAM)dg,0)==GKH_RET_SUCCESS && dg->data_len)
   {
     sotd_proto sop(dg->data,dg->data_len);
     if(gn & VTD_CURR_PARAM)
     {
      if(gn& VTD_USER_PARAM)
        setup_curr_user_param(sop);
       else
        setup_curr_param(sop);
     }
     else
     setup_pipe_param(sop);
   }
}

char * fmt = "%.2f";

void __fastcall     TViewFrame::setup_curr_user_param(sotd_proto  & sop)
{
  ViewGrpList->Clear();
  DWORD pipe  = 0  ; //= sop.op.data->numbers.loN;
  DWORD param ;      //sop.op.data->numbers.hiN;
  lpvtd_user_curr_param cp=(lpvtd_user_curr_param)sop.op.data->data;
  AnsiString str ;
  while(pipe<10)
  {
   TListItem * item;
   item = ViewGrpList->Items->Add();
   item->Caption = pipe+1;
   str.printf(fmt,cp->W[pipe]);
   item->SubItems->Add(str);
   str.printf(fmt,cp->Qc[pipe]);
   item->SubItems->Add(str);
   str.printf(fmt,cp->Vc[pipe]);
   item->SubItems->Add(str);
   str.printf(fmt,cp->Wl[pipe]);
   item->SubItems->Add(str);
   ++pipe;
  }

}

void __fastcall     TViewFrame::setup_curr_param(sotd_proto& sop)
{
  ViewGrpList->Clear();
  DWORD pipe  = 0  ; //= sop.op.data->numbers.loN;
  DWORD param ; //sop.op.data->numbers.hiN;
  AnsiString str ;
  lpvtd_pipe_curr_param cp=(lpvtd_pipe_curr_param)sop.op.data->data;
  while(pipe<10)
  {
   TListItem * item;
   item = ViewGrpList->Items->Add();
   item->Caption = pipe+1;
   str.printf(fmt,cp->pressure[pipe]);
   item->SubItems->Add(str);
   str.printf(fmt,cp->temp[pipe]);
   item->SubItems->Add(str);
   str.printf(fmt,cp->temp_back[pipe]);
   item->SubItems->Add(str);
   str.printf(fmt,cp->Gc[pipe]);
   item->SubItems->Add(str);
   str.printf(fmt,cp->Vc[pipe]);
   item->SubItems->Add(str);
   str.printf(fmt,cp->Nk[pipe]);
   item->SubItems->Add(str);

   ++pipe;
  }
  float val ;
  otd_get_value(sop.op.data,0,&val,sizeof(val));
  DWORD tm = (DWORD)val;
  DWORD h,m,s;
  h = tm/10000;
  m = (tm-h*10000)/100;
  s = (tm-h*10000 - m*100);
  str.printf("Время %02d:%02d:%02d",h,m,s);
  Label2->Caption = str;
  Label2->Visible = true;
}

void __fastcall     TViewFrame::setup_pipe_param(sotd_proto& sop)
{
  ViewGrpList->Clear();
  DWORD n  = sop.op.data->numbers.loN;
  DWORD en = sop.op.data->numbers.hiN;
  AnsiString str ;
  while(n<=en)
  {
   float val = 0;
   TListItem * item;
   otd_get_value(sop.op.data,n,&val,sizeof(val));
   item = ViewGrpList->Items->Add();
   item->Caption = n;
   str.printf("%.2f",val);
   item->SubItems->Add(str);
   ++n;
  }
}



