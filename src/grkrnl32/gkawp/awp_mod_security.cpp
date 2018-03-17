#pragma hdrstop
#include "gkawpmodule.hpp"


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
     if(omph->command & OTDM_RESPOND )
     {
      ret = GKH_RET_SUCCESS;
     }

    return ret;
}

//-----------------------------------------------------------------------------
