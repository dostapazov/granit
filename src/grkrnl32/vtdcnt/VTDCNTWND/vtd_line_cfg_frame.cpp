//---------------------------------------------------------------------------

#pragma hdrstop
#include <vcl.h>

#include "vtd_line_cfg_frame.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "gkmod_frame"
#pragma resource "*.dfm"
TVtdLineConfig *VtdLineConfig;
//---------------------------------------------------------------------------
__fastcall TVtdLineConfig::TVtdLineConfig(TComponent* Owner)
        : TGKModuleFrame(Owner)
{
}
//---------------------------------------------------------------------------

void   __fastcall TVtdLineConfig::set_device_number(DWORD num)
{
 if(this->device_param.number!= num)
   {
     ZeroMemory(&device_param,sizeof(device_param));
     device_param.dw_size = sizeof(device_param);
     device_param.number  = num;
   }
   get_device_param(&device_param);
   setup_controls(&device_param);
}

DWORD  __fastcall TVtdLineConfig::get_device_number()
{
 return device_param.number;
}

void    __fastcall TVtdLineConfig::after_set_gkhandle()
{
}

void    __fastcall TVtdLineConfig::get_device_param(LPVTDDEV_PARAM device_param)
{
  if(device_param)
  {
   device_param->dw_size = sizeof(*device_param);
   mod_iface.call(VTDCMD_GET_DEVICE,(LPARAM)device_param,device_param->number);
  }
}

void __fastcall reset_colors(TWinControl * win_ctrl)
{
 for(int i = 0;i<win_ctrl->ControlCount;i++)
    {
     TControl * control = win_ctrl->Controls[i];
      if(control->ClassNameIs("TEdit"))
        dynamic_cast<TEdit*>(control)->Font->Color = clDefault;
        else
        {
         if(control->ClassNameIs("TComboBox"))
            dynamic_cast<TComboBox*>(control)->Font->Color = clDefault;
            else
            {
             TWinControl * wc = dynamic_cast<TWinControl*>(control);
             if(wc && wc->ControlCount)
               reset_colors(wc);
            }
       }
    }

}

void    __fastcall TVtdLineConfig::setup_controls(LPVTDDEV_PARAM dp)
{

 if(dp->number<(DWORD)256)
 {
   DeviceNumber->Position = dp->number;
   DevType->ItemIndex     = dp->type;
   DevName->Text          = dp->name;
 }
 else
 {
   DeviceNumber->Position = 255;//dp->number;
   DevType->ItemIndex     = -1;
   DevName->Text          = "";
 }
 ch_mask = 0;
}


void    __fastcall TVtdLineConfig::set_ch_mask(DWORD mask)
{
 if(Fch_mask!= mask)
  {
   Fch_mask = mask;
   if(on_changed && Visible)
      on_changed(this);
   if(!mask)
     reset_colors(this);   
  }
}

bool  __fastcall TVtdLineConfig::apply_changes()
{
 volatile DWORD ret = 0;
 bool create  = this->device_param.number>255 ? true:false;
 DWORD old_numer = device_param.number;
 device_param.number = DeviceNumber->Position;
 device_param.type   = DevType->ItemIndex;
 WideString w_str(DevName->Text);
 if(w_str.Length())
   wcsncpy(device_param.name,w_str.c_bstr(),KERTL_ARRAY_COUNT(device_param.name));

 ret  = mod_iface.call(create ? VTDCMD_CREATE_DEVICE:VTDCMD_UPDATE_DEVICE,(LPARAM)&device_param,old_numer);
 if(ret == GKH_RET_SUCCESS)
    ch_mask = 0;
    else
    {
     if(create)
     device_param.number = (DWORD)-2;
     ret = GetLastError();
    }

 return ret ? false:true;
}

bool  __fastcall TVtdLineConfig::undo_changes()
{
 DWORD ret = 0;
 setup_controls(&device_param);
 return ret;
}

void __fastcall TVtdLineConfig::Edit1Change(TObject *Sender)
{
  TColor color = clDefault;
  if(this->device_param.number!=(DWORD) DeviceNumber->Position)
  {
   color = clBlue;
   ch_mask|=CCFL_DEV_NUM;
  }
  else
   ch_mask&=~CCFL_DEV_NUM;
   Edit1->Font->Color = color;

}
//---------------------------------------------------------------------------

void __fastcall TVtdLineConfig::DevTypeChange(TObject *Sender)
{
  TColor color = clDefault;
  if(device_param.type!=(DWORD)DevType->ItemIndex)
  {
  color = clBlue;
   ch_mask|=CCFL_DEV_TYPE;
  }
  else
   ch_mask&=~CCFL_DEV_TYPE;
   DevType->Font->Color = color;

}
//---------------------------------------------------------------------------

void __fastcall TVtdLineConfig::DevNameChange(TObject *Sender)
{
  TColor color = clDefault;
  if(UnicodeString(device_param.name).Compare(DevName->Text))
  {
   color = clBlue;
   ch_mask|=CCFL_DEV_NAME;
  }
  else
   ch_mask&=~CCFL_DEV_NAME;
    DevName->Font->Color = color;


}
//---------------------------------------------------------------------------


