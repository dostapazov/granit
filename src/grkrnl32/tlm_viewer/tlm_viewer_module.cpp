#include "tlm_viewer_module.hpp"
 

THeap heap(0xF000);

void * operator new (size_t sz)
{ return heap.Alloc(sz,0);}

void  operator delete (void * ptr)
{ heap.Free(ptr,0);}

void * operator new [] (size_t sz)
{ return heap.Alloc(sz,0);}

void  operator delete [] (void * ptr)
{ heap.Free(ptr,0);}


LRESULT WINAPI module_main(DWORD cmd,LPARAM p1,LPARAM p2)
{
  LRESULT ret = 0;
       switch(cmd)
       {
        case GKME_CMD_GET_MODULETYPE:
			ret = MT_TLM_VIEWER ;break;
        case GKME_CMD_CREATE_HANDLE :
        {
            tlm_viewer_mod * mod =new tlm_viewer_mod;
            ret  = (LRESULT)(*(TGKHandleBased*)(mod))();
            if(!ret)
             delete mod;
        }
        break;
       }
       return ret;
}

 tlm_viewer_mod::tlm_viewer_mod()
 {
  lock_param    = GKHB_AUTO_LOCK_OFF;
  alloc_gkhandle(L"TLM_VIWER_MODULE");
  this->main_form = NULL;
 }

 DWORD   __fastcall tlm_viewer_mod::get_config_data(DWORD mask,LPVOID buf,DWORD bsz)
 {
  DWORD ret = 0;
  return ret;
 }

 bool    __fastcall tlm_viewer_mod::set_config_data(DWORD mask,LPVOID buf,DWORD bsz)
 {
  bool ret = false;
  return ret;
 }

 bool    __fastcall tlm_viewer_mod::compare_config_data(DWORD mask,LPVOID buf,DWORD bsz,LPDWORD ch_mask,BOOL * restart)
 {
  bool ret = false;
  return ret;
 }

 void    __fastcall tlm_viewer_mod::release (LPVOID p)
 {
   if(main_form)
     delete main_form;
   main_form = 0;  
   TGKModule::release(p);
 }


 DWORD   __fastcall tlm_viewer_mod::start(DWORD reason,LPARAM p2)
 {
  if(!main_form)
      main_form = new TTlmViewerMainForm (Application);
  if(main_form)
     {
      Application->Title = "Просмотр кадров";
      Application->Icon  = main_form->Icon;
      modemmer.open((wchar_t*)MODEMMER_HANDLE_NAME_W);
      modemmer.set_notify(_handle,MNF_MODEM,TRUE);
      modemmer.call(MDMCM_REGISTER_HANDLER,LPARAM((*this)()),FA_OTD_MEDIUM);
      main_form->Show();
      send_command(true,true,OTDMPROTO_CMD_ENUM_KADRS,0,NULL,NULL);
      return TGKModule::start(reason,p2);
     }
    else
    return GKH_RET_ERROR;

 }

 DWORD   __fastcall tlm_viewer_mod::stop (DWORD reason)
 {
  if(main_form)
     {
      main_form->Hide();
      if(reason == MODULE_STOP_PROGRAM_SHUTDOWN)
      {
       delete main_form;
       main_form = NULL;
      }
      modemmer.set_notify(_handle,MNF_MODEM,FALSE);
      modemmer.call(MDMCM_UNREGISTER_HANDLER,LPARAM((*this)()),FA_OTD_MEDIUM);
      modemmer.close();
     }
  return TGKModule::stop(reason);
 }

 LRESULT __fastcall tlm_viewer_mod::processing    (DWORD cmd,LPARAM p1,LPARAM p2)
 {
  LRESULT ret = GKH_RET_ERROR;
  if(MCMD_TYPE(cmd) == MT_MODEMMER)
  {
   switch(cmd)
   {
    case MDMCM_FA_HANDLE: ret = handle_data((LPMPROTO_HEADER)p1,modemmer());break;
   }
  }
  else
  ret = TGKModule::processing(cmd,p1,p2);
  return ret;

 }

 LRESULT __fastcall tlm_viewer_mod::handle_data(LPMPROTO_HEADER mph,GKHANDLE h)
 {
  LRESULT ret = GKH_RET_SUCCESS;
  AnsiString str;
  str.printf("recv kadr %d",mph->pkt_num);
  main_form->Label1->Caption = str;
  return  ret;
 }


DWORD       __fastcall tlm_viewer_mod::send_command(bool begin,bool finish,DWORD cmd,DWORD err,LPVOID data,DWORD data_size )
{
 DWORD ret = 0;
 BYTE   buffer[16384];
 LPMPROTO_HEADER     mph;
 LPOTDM_PROTO_HEADER omph;
 DWORD  need_size = sizeof(MPROTO_HEADER)-sizeof(mph->data[0])+sizeof(*omph)-sizeof(omph->data[0])+data_size;
 if(need_size<sizeof(buffer))
   mph = (LPMPROTO_HEADER) buffer;
  else
   mph = (LPMPROTO_HEADER) new BYTE[need_size];
 if(mph)
 {
 ZeroMemory(mph,need_size);
 mph->addr_to.addr  = -1;
 mph->addr_to.addr  = 0;
 mph->addr_to.modem = 201;

 mph->data_size = need_size+(sizeof(mph->data)-sizeof(*mph));
 mph->fa = FA_OTD_MEDIUM;
 omph =(LPOTDM_PROTO_HEADER) mph->data;
 omph->command    = cmd&OTDM_COMMAND_MASK;
 if(begin)
   omph->command  |= OTDM_BEGINDATA;
 if(finish)
  omph->command  |= OTDM_ENDDATA;
  if(err)
  omph->command  |= OTDM_ERROR;
  omph->error     = err;
  omph->data_size = data_size;
  memcpy(omph->data,data,data_size);
  modemmer.call(MDMCM_SEND,(LPARAM)mph,0);

  if( buffer!=(LPBYTE)mph)
      delete  [](LPBYTE)mph;
  }
  return ret;
}





