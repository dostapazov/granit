#pragma hdrstop
#include "gkawpmodule.hpp"
#include "awp_main_form.h"


DWORD  __fastcall TGkAwpModule::send_login_request (UnicodeString user_name,UnicodeString passw)
{
  char buf[8192];
  LPMPROTO_HEADER mph      = (LPMPROTO_HEADER)buf;
  memset(mph,0,sizeof(*mph));
  mph->addr_to        = this->get_modem_source();
  mph->fa             = FA_SECURITY;
  mph->internal       = MP_INTERNAL_DEFAULT;

  local_ssid = KeRTL::GetTime();
  mph->data_size  = gksecur_login_format(mph->data,sizeof(buf)-sizeof(*mph),local_ssid,user_name.c_str(),passw.c_str());

  *((LPWORD)(((LPBYTE)mph->data)+mph->data_size)) = otd_calc_checksum(mph->data,mph->data_size,OTD_DEF_POLINOM);
  mph->data_size+=sizeof(WORD);
  return send_data(mph);
}

//-----------------------------------------------------------------------------
LRESULT __fastcall TGkAwpModule::handle_security  (MODEM_ADDR & from,  LPOTDM_PROTO_HEADER omph, DWORD data_size)
{
     LRESULT ret = GKH_RET_SUCCESS;
     OTDM_PROTO_STATE  ps(omph);
     if(ps.is_respond )
     {
      switch(ps.command)
      {
       case GKSECUR_PROTO_CMD_LOGIN:
       {
          remote_ssid = ps.is_error ? 0 : *((LPSESSION_ID)ps.p_any);
          if(this->main_form)
            {
             main_form->set_login_session_id(remote_ssid);
            }
       }
       break;
      }
     }
    return ret;
}

//-----------------------------------------------------------------------------
