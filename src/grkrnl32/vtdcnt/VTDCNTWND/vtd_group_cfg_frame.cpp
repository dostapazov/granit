//---------------------------------------------------------------------------

#pragma hdrstop
#include <vcl.h>

#include "vtd_group_cfg_frame.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "gkmod_frame"
#pragma resource "*.dfm"
TVtdGroupConfig *VtdGroupConfig;
//---------------------------------------------------------------------------
__fastcall TVtdGroupConfig::TVtdGroupConfig(TComponent* Owner)
        : TGKModuleFrame(Owner)
{
 Fch_mask    = 0;
 on_changed  = 0;
}
//---------------------------------------------------------------------------

void   __fastcall TVtdGroupConfig::after_set_gkhandle()
{
}

void   __fastcall TVtdGroupConfig::set_ch_mask(DWORD cm)
{
 if(Fch_mask != cm)
  {
    Fch_mask = cm;
    if(on_changed)
       on_changed(this);
  }
}

DWORD  __fastcall TVtdGroupConfig::get_number()
{
 return (DWORD) MAKELONG(dev_number,grp_param.group_number);
}

void   __fastcall TVtdGroupConfig::set_number(DWORD num)
{
 dev_number = LOWORD(num);
 grp_param.group_number = HIWORD(num);
 get_param(&grp_param);
 setup_controls();
 ch_mask = 0;
 
}

AnsiString __fastcall TVtdGroupConfig::get_text(DWORD number)
{
  AnsiString str ;
  
  if(number &VTD_CURR_PARAM)
      str= "Текущие параметры";
      else
      str = "Параметры";
     str+= (number &VTD_USER_PARAM) ? " потребителя" : " трубы";
     str.cat_printf(" № %d",number&0x0F);
     return str;

}

bool  __fastcall TVtdGroupConfig::apply_changes()
{
 DWORD ret ;
 VTDDEV_GROUP grp;
 grp.dw_size = sizeof(grp);
 get_param_from_controls(&grp);
 if(this->grp_param.group_number == (WORD)-3)
   {
    // режим создания
    ret = mod_iface.call(VTDCMD_CREATE_GROUP,(LPARAM)&grp,this->dev_number);
   }
   else
   {
    ret = mod_iface.call(VTDCMD_UPDATE_GROUP,(LPARAM)&grp,MAKELONG(dev_number,grp_param.group_number));
   }
   if(ret == GKH_RET_SUCCESS)
     {
      CopyMemory(&grp_param,&grp,sizeof(grp));
      ch_mask = 0;
     }
     else
     ret = GetLastError(); 
 return ret ? false : true;
}

bool  __fastcall TVtdGroupConfig::undo_changes()
{

 setup_controls();
 ch_mask = 0;
 return true;
}

void __fastcall TVtdGroupConfig::setup_controls()
{
  DWORD kind_idx = 0;
  if(this->grp_param.group_number!=(WORD)-3)
  {
  if(grp_param.group_number & VTD_CURR_PARAM)
     ++kind_idx;
  if(grp_param.group_number & VTD_USER_PARAM)
     kind_idx+=2;
   ParamKind->ItemIndex = kind_idx;
   GrpNumber->Position  = grp_param.group_number&0x0F;
   QueryPeriod->Text    = grp_param.query_period;
   FirstParam->Position = grp_param.first;
   ParamCount->Position = grp_param.count;

  }
  else
  {
   //режим создания
   ParamKind->ItemIndex = -1;
   NumberEdit->Text = AnsiString();
   VTDPanel->Visible = false;
   QueryPeriod->Text = 300;
   FirstParam->Position = 1;
   ParamCount->Position = 63;
  }
  ParamKindChange(this);

}


bool   __fastcall TVtdGroupConfig::get_param(LPVTDDEV_GROUP gparam,DWORD num)
{
 if(gparam)
 {
   gparam->dw_size = sizeof(*gparam);
   if(num == DWORD (-1))
      num = number;
   return  mod_iface.call(VTDCMD_GET_GROUP,LPARAM(gparam),num ) == GKH_RET_SUCCESS ? true:false;
 }
 return false;  
}

void __fastcall TVtdGroupConfig::get_param_from_controls(LPVTDDEV_GROUP gparam)
{
 gparam->group_number =  GrpNumber->Position;
 gparam->first =  FirstParam->Position;
 gparam->count =  ParamCount->Position;
 
 switch(ParamKind->ItemIndex)
 {
  case 1: gparam->group_number |=VTD_CURR_PARAM;
          gparam->first = 0;
          gparam->count = 61;
          break;
  case 2: gparam->group_number |=VTD_USER_PARAM;break;
  case 3: gparam->group_number |=(VTD_USER_PARAM|VTD_CURR_PARAM);
          gparam->first = 0;
          gparam->count = 40;
          break;
 }
 gparam->query_period = atoi(QueryPeriod->Text.c_str());
 if(gparam->query_period<3)
     gparam->query_period = 3;
}


void __fastcall TVtdGroupConfig::update_grp_num()
{
 AnsiString str;
 if(NumberEdit->Text.Length() && ParamKind->ItemIndex>=0)
 {
   VTDDEV_GROUP grp;
   grp.dw_size = sizeof(grp);
   get_param_from_controls(&grp);
   str.printf("Dec : %03d   Hex : %02X",grp.group_number,grp.group_number); 
 }
 GroupNumber->Caption = str;
}

void __fastcall TVtdGroupConfig::ParamKindChange(TObject *Sender)
{
 VTDPanel->Visible = (ParamKind->ItemIndex&1) ? false:true;
 ch_mask|=CCFL_GRP_NUM;
 update_grp_num();
}
//---------------------------------------------------------------------------
void __fastcall TVtdGroupConfig::NumberEditChange(TObject *Sender)
{
  ch_mask|=CCFL_GRP_NUM;
  update_grp_num();      
}
//---------------------------------------------------------------------------

void __fastcall TVtdGroupConfig::QueryPeriodChange(TObject *Sender)
{
  ch_mask|=CCFL_GRP_QUERY_PERIOD;      
}
//---------------------------------------------------------------------------

void __fastcall TVtdGroupConfig::Edit3Change(TObject *Sender)
{
  ch_mask|=CCFL_GRP_PARAMS;      
}
//---------------------------------------------------------------------------

