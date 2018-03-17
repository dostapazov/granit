#pragma hdrstop
#include <iec60870proto.h>

 lpiec60870_proto_header_t WINAPI iec60870_proto_init_frame(LPBYTE ptr,DWORD sz,LPBYTE data,DWORD len)
 {
   lpiec60870_proto_header_t proto_ptr = NULL;

   if(sz>=(len+sizeof(*proto_ptr)))
   {
      proto_ptr = (lpiec60870_proto_header_t)ptr;
      proto_ptr->start_marker = IEC60870_PROTO_START_MARKER;
      proto_ptr->lenght  = sizeof(DWORD)+len;
      proto_ptr->dw_ctrl = 0;
      if(len && data)
        {
         ptr+= sizeof(proto_ptr->start_marker)+sizeof(proto_ptr->lenght)+sizeof(DWORD);
         memcpy(ptr,data,len);
        }
   }
   return proto_ptr;
 }

 lpiec60870_proto_header_t WINAPI iec60870_proto_startdt (LPBYTE ptr,DWORD sz,bool confirm)
 {
   lpiec60870_proto_header_t phdr = iec60870_proto_init_frame(ptr,sz,NULL,0);
   if(phdr)
   {
    phdr->acpiU.format   = acpi_type_U;
    phdr->acpiU.startdt  = confirm ? at_con : at_act;
   }
   return phdr;
 }

 lpiec60870_proto_header_t WINAPI iec60870_proto_stopdt (LPBYTE ptr,DWORD sz,bool confirm)
 {
   lpiec60870_proto_header_t phdr = iec60870_proto_init_frame(ptr,sz,NULL,0);
   if(phdr)
   {
    phdr->acpiU.format   = acpi_type_U;
    phdr->acpiU.stopdt   = confirm ? at_con : at_act;
   }
   return phdr;
 }


 lpiec60870_proto_header_t WINAPI iec60870_proto_testfr  (LPBYTE ptr,DWORD sz,bool confirm)
 {
   lpiec60870_proto_header_t phdr = iec60870_proto_init_frame(ptr,sz,NULL,0);
   if(phdr)
   {
    phdr->acpiU.format   = acpi_type_U;
    phdr->acpiU.testfr   = confirm ? at_con : at_act;
   }
   return phdr;
 }

 lpiec60870_asdu_header    WINAPI iec60870_proto_get_asdu_header (lpiec60870_proto_header_t phdr)
 {
   lpiec60870_asdu_header asdu_hdr = NULL;
   if(iec60870_proto_is_valid(phdr) && phdr->lenght >= (sizeof(phdr->dw_ctrl)+sizeof(*asdu_hdr)))
   {
    LPBYTE ptr = (LPBYTE)&phdr->dw_ctrl;
    ptr+=sizeof(phdr->dw_ctrl);
    asdu_hdr = (lpiec60870_asdu_header)ptr;
   }
   return asdu_hdr;
 }

 int  WINAPI   iec60870_proto_frame_add_data  (lpiec60870_proto_header_t phdr,LPBYTE data,DWORD len)
 {
    if(phdr && (phdr->lenght+len)<=IEC60870_MAX_FRAME_LENGHT && data && len )
    {
      LPBYTE dest_ptr = (LPBYTE)phdr;
             dest_ptr+= sizeof(phdr->start_marker)+sizeof(phdr->lenght)+phdr->lenght;
      memcpy(dest_ptr,data,len);
      phdr->lenght+=len;
      return phdr->lenght;
    }
  return 0;
 }

 DWORD WINAPI                     iec60870_proto_get_obj_addr    (LPBYTE ptr,DWORD addr_sz)
 {
   DWORD ret = 0;
   if(addr_sz>sizeof(ret)) addr_sz = sizeof(ret);
   memcpy(&ret,ptr,addr_sz);
   return ret;
 }

 void WINAPI                     iec60870_proto_set_obj_addr    (DWORD addr,DWORD addr_sz,LPBYTE ptr_to)
 {
   if(addr_sz>sizeof(addr)) addr_sz = sizeof(addr);
   memcpy(ptr_to,&addr,addr_sz);
 }

 lpiec60870_proto_header_t WINAPI iec60870_proto_receipt(LPBYTE buf,DWORD sz,WORD kadr_number)
 {
   lpiec60870_proto_header_t phdr = iec60870_proto_init_frame(buf,sz,NULL,0);
   if(phdr)
   {
	phdr->acpiS.format   = acpi_type_S;
	phdr->acpiS.rx_number.set_number(kadr_number);
   }
   return phdr;
 }

 lpiec60870_proto_header_t WINAPI iec60870_proto_interrogate(LPBYTE buf,DWORD sz,DWORD req_addr,DWORD common_addr,DWORD addr_size)
 {
	  iec60870_asdu_header ah;
	  ah.asdu_type   = C_IC_NA_1;
	  ah.obj_count   = 1;
	  ah.seq         = 0;
	  ah.cause       = tc_activation;
	  ah.pn          = 0;
	  ah.test        = 0;
	  ah.common_addr   =  common_addr;
	  ah.origin_addr   =  req_addr;
	  lpiec60870_proto_header_t hptr = iec60870_proto_init_frame(buf,sz,(LPBYTE)&ah,sizeof(ah));

          BYTE    data[8];
          DWORD   dsz = addr_size;
          iec60870_proto_set_obj_addr(0,dsz,data);
          data[dsz++] = qoi_common;

	  iec60870_proto_frame_add_data(hptr,data,dsz );
	  hptr->acpiI.rx_number.value = hptr->acpiI.tx_number.value = 0;
	  return hptr;
 }

  lpiec60870_proto_header_t WINAPI iec60870_proto_control(LPBYTE buf,DWORD sz,DWORD orig_addr,DWORD common_addr,DWORD command,DWORD ctrl_addr,DWORD addr_sz,iec60870_rctrl rctrl)
  {
	  iec60870_asdu_header ah;
	  ah.asdu_type   = command;
	  ah.obj_count   = 1;
	  ah.seq         = 0;
	  ah.cause       = tc_activation;
	  ah.pn          = 0;
	  ah.test        = 0;
	  ah.common_addr =  common_addr;
	  ah.origin_addr =  orig_addr;
	  lpiec60870_proto_header_t hptr = iec60870_proto_init_frame(buf,sz,(LPBYTE)&ah,sizeof(ah));
	  iec60870_proto_frame_add_data(hptr,(LPBYTE)&ctrl_addr,addr_sz);
	  iec60870_proto_frame_add_data(hptr,(LPBYTE)&rctrl,sizeof(rctrl));
	  hptr->acpiI.rx_number.value = hptr->acpiI.tx_number.value = 0;
	  return hptr;

  }






