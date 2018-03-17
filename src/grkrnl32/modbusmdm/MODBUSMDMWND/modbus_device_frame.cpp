//---------------------------------------------------------------------------

#pragma hdrstop
#include <vcl.h>

#include "modbus_device_frame.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "gkmod_frame"
#pragma resource "*.dfm"
TModbusDevFrame *ModbusDevFrame;
//---------------------------------------------------------------------------
__fastcall TModbusDevFrame::TModbusDevFrame(TComponent* Owner)
        : TGKModuleFrame(Owner)
{
  in_setup    = 0;
  ch_mask     = 0;
  Fon_changed = NULL;
  this->Ftree_node = NULL;
}
//---------------------------------------------------------------------------
void   __fastcall TModbusDevFrame::set_tree_node(TTreeNode * node)
{
 if(this->Ftree_node != node)
 {
    Ftree_node = node;
    setup_controls();
    ch_mask = 0;
 }
}

MODBUS_POINT &  __fastcall TModbusDevFrame::get_point()
{
 return *(LPMODBUS_POINT)tree_node->Data;
}

void   __fastcall TModbusDevFrame::set_point_no(DWORD _point_no)
{
 if(point_no!=_point_no)
 {
  get_point().point_no = _point_no;
  read_point();
 }
}

void   __fastcall TModbusDevFrame::read_point()
{
  mod_iface.call(MBCM_GET_POINT,(LPARAM)&get_point(),0);
  setup_controls();
}


 void     __fastcall  TModbusDevFrame::set_ch_mask(DWORD val)
 {
   if(Fch_mask != val)
    {
     Fch_mask = val;
     if(on_changed) on_changed(this);
    }
 }

int __fastcall TModbusDevFrame::ComSpeed2idx(int speed)
{
 int i = 0;
 switch(speed)
 {
  case  CBR_1200    : i = 0 ;break;
  case  CBR_2400    : i = 1 ;break;
  case  CBR_4800    : i = 2 ;break;
  case  CBR_9600    : i = 3 ;break;
  case  CBR_19200   : i = 4 ;break;
  case  CBR_38400   : i = 5 ;break;
  case  CBR_57600   : i = 6 ;break;
  case  CBR_115200  : i = 7 ;break;
 }
 return i;
}

int __fastcall TModbusDevFrame::idx2ComSpeed(int idx)
{
 int i = 0;
 switch(idx)
 {
  case  0 : i = CBR_1200    ;break;
  case  1 : i = CBR_2400    ;break;
  case  2 : i = CBR_4800    ;break;
  case  3 : i = CBR_9600    ;break;
  case  4 : i = CBR_19200   ;break;
  case  5 : i = CBR_38400   ;break;
  case  6 : i = CBR_57600   ;break;
  case  7 : i = CBR_115200  ;break;
 }
 return i;
}



void   __fastcall TModbusDevFrame::setup_controls()
{
 begin_setup();
 DevNumber->Enabled = is_new_point();
 if(!DevNumber->Enabled)
 {
  DevNumber->Text          = modbus_point.point_no;
  DevName->Text            = modbus_point.point_name;
  ComPortNumber->Position  = modbus_point.comport_settings.port_num;
  ComPortSpeed->ItemIndex  = ComSpeed2idx(modbus_point.comport_settings.port_speed);
  StopBits->ItemIndex      = modbus_point.comport_settings.port_stopbits;
  ParityControl->ItemIndex = modbus_point.comport_settings.port_oddeven;
  host_addr->Text          = modbus_point.host_params.host_addr;
  host_port->Text          = modbus_point.host_params.host_port;
  media_type->ItemIndex    = modbus_point.media_type;
  ComPortDelay->Text       = modbus_point.comport_settings.port_delay;
  ComPortDelayTCP->Text    = modbus_point.comport_settings.port_delay;
 }
 else
 {
  AnsiString text;
  DevNumber->Text     = text;
  DevName->Text       = text;

  ComPortNumber->Position  = 1;
  ComPortSpeed->ItemIndex  = ComSpeed2idx(modbus_point.comport_settings.port_speed);
  StopBits->ItemIndex      = 1;
  ParityControl->ItemIndex = 0;
  host_addr->Text          = text;
  host_port->Text          = text;
  media_type->ItemIndex    = MODBUS_MEDIA_TYPE_ETHERNET;
  ComPortDelay->Text       = 100;
  ComPortDelayTCP->Text    = 100;
 }
 media_typeClick(this);
 end_setup();
}

bool   __fastcall  TModbusDevFrame::delete_current()
{

 bool ret = is_new_point() ? true : mod_iface.call(MBCM_REMOVE_POINT,point_no,0) == GKH_RET_SUCCESS ? true:false;
 if(ret)
   this->Ftree_node->Delete();
 return ret;
}



bool   __fastcall  TModbusDevFrame::apply_changes()
{
 bool ret = true;
  //Создание нового / установка новых параметров
  bool was_new = is_new_point();
  modbus_point.point_no = atoi(DevNumber->Text.c_str());
  wcsncpy(modbus_point.point_name,WideString(DevName->Text).c_bstr(),KERTL_ARRAY_COUNT(modbus_point.point_name));

  modbus_point.media_type = media_type->ItemIndex;
  safe_strcpy(modbus_point.host_params.host_addr,host_addr->Text.c_str());
  modbus_point.host_params.host_port = atoi(this->host_port->Text.c_str());

  modbus_point.comport_settings.port_num      = ComPortNumber->Position;
  modbus_point.comport_settings.port_speed    = idx2ComSpeed(ComPortSpeed->ItemIndex);
  modbus_point.comport_settings.port_oddeven  = ParityControl->ItemIndex;
  modbus_point.comport_settings.port_stopbits = StopBits->ItemIndex;
  DWORD port_delay ;
  if(modbus_point.media_type == MODBUS_MEDIA_TYPE_COMPORT_VIA_ETHERNET)
     port_delay = _wtoi(this->ComPortDelayTCP->Text.c_str());
     else
     port_delay = _wtoi(this->ComPortDelay->Text.c_str());
     modbus_point.comport_settings.port_delay    = port_delay;



  if(mod_iface.call(was_new ? MBCM_CREATE_POINT : MBCM_UPDATE_POINT ,(LPARAM)&modbus_point,0))
     ret = false;
     else
     {
      Ftree_node->Text = DevName->Text;
      ch_mask    = 0;
     }
 return ret;
}

bool   __fastcall  TModbusDevFrame::undo_changes()
{
 if(Ftree_node)
 {
  if(is_new_point())
    delete_current();
    else
    setup_controls();
  ch_mask = 0;
 }

 return true;
}


void __fastcall TModbusDevFrame::DevNumberChange(TObject *Sender)
{
   if(!in_setup)
   {
     //Изменили номер или имя
     ch_mask |= CCFL_POINTS;
   }
}
//---------------------------------------------------------------------------

void __fastcall TModbusDevFrame::media_typeClick(TObject *Sender)
{
  if(!in_setup)
  {
   if(this->modbus_point.media_type != (DWORD)media_type->ItemIndex)
     ch_mask |=CCFL_MEDIATYPE;
     else
     ch_mask &=~CCFL_MEDIATYPE;
   }  
     pnl0->Visible = media_type->ItemIndex == 0 ? true:false;
     pnl1->Visible = !pnl0->Visible;
     if(pnl1->Visible)
     {
       Label5->Visible = media_type->ItemIndex == MODBUS_MEDIA_TYPE_COMPORT_VIA_ETHERNET ? true : false;
       ComPortDelayTCP->Visible = Label5->Visible;
     }
}
//---------------------------------------------------------------------------

void __fastcall TModbusDevFrame::host_addrChange(TObject *Sender)
{
   if(!in_setup)
   {
    if(   host_addr->Text.CompareIC(modbus_point.host_params.host_addr)
       || (DWORD)atoi(host_port->Text.c_str())    != modbus_point.host_params.host_port
       || (DWORD)atoi(ComPortDelayTCP->Text.c_str()) != modbus_point.comport_settings.port_delay
      )
       ch_mask |= CCFL_HOST_PARAMS;
       else
       ch_mask &=~CCFL_HOST_PARAMS;
   }
}
//---------------------------------------------------------------------------

