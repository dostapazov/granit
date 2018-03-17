//---------------------------------------------------------------------------
#pragma hdrstop
#include <vcl.h>
#include "iec60870_line_param.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "gkmod_frame"
#pragma resource "*.dfm"
TIEC60870LineParam *IEC60870LineParam;
//---------------------------------------------------------------------------
__fastcall TIEC60870LineParam::TIEC60870LineParam(TComponent* Owner)
	: TGKModuleFrame(Owner)
{

}
//---------------------------------------------------------------------------

DWORD    __fastcall TIEC60870LineParam::get_line_number()
{
  return this->current_tree_node ? (DWORD)current_tree_node->Data : (DWORD)-1;
}

bool     __fastcall TIEC60870LineParam::is_create_mode ()
{
  return this->current_tree_node && line_number == DWORD(-1) ? true : false;
}


void     __fastcall TIEC60870LineParam::set_node_image_index(TTreeNode * node,DWORD line_state)
{
 if(node)
 {
   DWORD idx = LINE_IMAGE_DISCONNECT;
   if(line_state&MODEM_LINE_STATE_CONNECT)
      idx = LINE_IMAGE_CONNECT;
      else
      {
       if(line_state&MODEM_LINE_STATE_CONNECTING)
         idx = LINE_IMAGE_CONNECTING;
      }
  node->ImageIndex     = idx;
  node->SelectedIndex  = idx;
  node->StateIndex     = idx;
 }
}

void     __fastcall  TIEC60870LineParam::update_line(TTreeNode * line_node,LPMODEM_LINE_PROPS mlp)
{
  if(mlp->line_text_len)
      line_node->Text =   UnicodeString (mlp->line_text,mlp->line_text_len);
   set_node_image_index(line_node,mlp->line_state);
}

void     __fastcall  TIEC60870LineParam::update_line(TTreeNode * line_node)
{
       BYTE buf[1024];
       bzero(buf,sizeof(buf));
       LPMODEM_LINE_PROPS mlp = (LPMODEM_LINE_PROPS)buf;
       mlp->dw_size = sizeof(buf);
       mlp->line_text_sz = sizeof(buf)-sizeof(*mlp);
       if(GKH_RET_SUCCESS == mod_iface.call(MODEMCM_GET_LINE_PROPS,(LPARAM)line_node->Data,(LPARAM)mlp))
         update_line(line_node,mlp);
}

bool     __fastcall  TIEC60870LineParam::apply_changes ()
{
 IEC60870_LINE_CONFIG lc;
 get_settings(&lc,true);
 if(GKH_RET_SUCCESS == mod_iface.call(this->is_create_mode() ? CMD_IEC60870_ADD_LINE : CMD_IEC60870_UPD_LINE,(LPARAM)&lc,line_number))
   {
    line_config = lc;
    if(is_create_mode())
    {
     current_tree_node->Data = (LPVOID)lc.cp_number;
     update_line(current_tree_node);
    }
    update_line(current_tree_node);
   }
 return true;
}

bool     __fastcall  TIEC60870LineParam::undo_changes  ()
{
 if(is_create_mode())
    delete_current();
    else
    {
     setup_controls(&this->line_config);
     ch_mask = 0;
    }
 return true;
}

bool     __fastcall  TIEC60870LineParam::delete_current()
{
 if(!is_create_mode())
   {
    if(GKH_RET_SUCCESS != mod_iface.call(CMD_IEC60870_DEL_LINE,line_number,0))
        return false;
   }
   {
    current_tree_node->Data = (LPVOID)-2;
    current_tree_node->Delete();
   }
 return true;
}

bool     __fastcall  TIEC60870LineParam::get_settings  (LPIEC60870_LINE_CONFIG mcfg,bool from_controls)
{
  if(from_controls)
  {
    mcfg->cp_number = _wtoi(this->CpNumber->Text.c_str());
    safe_strcpyn(mcfg->cp_name,cp_name->Text.c_str(),KERTL_ARRAY_COUNT(mcfg->cp_name));
    safe_strcpyn(mcfg->host_addr,this->host_addr->Text.c_str(),KERTL_ARRAY_COUNT(mcfg->host_addr));
    mcfg->host_port = _wtoi(host_port->Text.c_str());
    mcfg->obj_addr_size  = obj_addr_size->ItemIndex+1;
    mcfg->WT[WT_TX_TIMER]   = _wtoi(transmit_delay->Text.c_str());
    mcfg->WT[WT_RX_TIMER]   = _wtoi(rx_timeout->Text.c_str  ());
    mcfg->W_count           = _wtoi(W_count->Text.c_str     ());
    mcfg->K_count           = _wtoi(K_count->Text.c_str     ());
    mcfg->dev_addr          = _wtoi(dev_addr->Text.c_str    ());
    mcfg->common_addr       = _wtoi(common_addr->Text.c_str ());
  }
  else
  {
    if(mod_iface.call(CMD_IEC60870_GET_LINE,(LPARAM)mcfg,line_number))
      return false;
  }
 return true;
}

void     __fastcall  TIEC60870LineParam::setup_controls(const LPIEC60870_LINE_CONFIG mcfg)
{
   ++in_setup;
   if(mcfg->cp_number == (DWORD)-1)
     {
       CpNumber->Text  = UnicodeString();
       cp_name->Text   = UnicodeString();
       host_addr->Text = UnicodeString();
       host_port->Text = IEC60870_DEFAULT_TCP_PORT;
       transmit_delay->Text = 100;
       rx_timeout->Text     = 200;
       W_count->Text        = 30;
       K_count->Text        = 10;
       obj_addr_size->ItemIndex = 3;
       dev_addr->Text       = UnicodeString();
       common_addr->Text    = UnicodeString();

     }
     else
     {
       CpNumber->Text = mcfg->cp_number;
       cp_name->Text   = mcfg->cp_name;
       host_addr->Text = mcfg->host_addr;
       host_port->Text = mcfg->host_port;
       transmit_delay->Text  = mcfg->WT[WT_TX_TIMER];
       rx_timeout->Text      = mcfg->WT[WT_RX_TIMER];
       W_count->Text         = mcfg->W_count;
       K_count->Text         = mcfg->K_count;
       obj_addr_size->ItemIndex = mcfg->obj_addr_size-1;
       dev_addr->Text       = mcfg->dev_addr;
       common_addr->Text    = mcfg->common_addr;
     }
   --in_setup;
}

void     __fastcall  TIEC60870LineParam::current_object_change (TObject * value)
{
  TGKModuleFrame::current_object_change(value);
  if(line_number!=(DWORD)-1)
    {
     line_config.cp_number = line_number;
     if(!get_settings(&line_config,false))
        {
          _wcspcpy(line_config.cp_name,L"Ошибка чтения параметров");
        }
    }
    else
    {
     line_config.cp_number  = -1;
     line_config.host_port  = IEC60870_DEFAULT_TCP_PORT;
     line_config.WT[WT_TX_TIMER] = 100;
     line_config.WT[WT_RX_TIMER] = 200;
     line_config.W_count         = 30;
     line_config.K_count         = 30;
     line_config.obj_addr_size   = 3;
     line_config.dev_addr        = 0;
     line_config.common_addr     = 0;

    }
  setup_controls(&line_config);
}

void __fastcall TIEC60870LineParam::CpNumberChange(TObject *Sender)
{
  if(!in_setup)
  {
    TComponent * comp = dynamic_cast<TComponent *>(Sender);
    if(comp) ch_mask |= comp->Tag;
  }
}
//---------------------------------------------------------------------------


















