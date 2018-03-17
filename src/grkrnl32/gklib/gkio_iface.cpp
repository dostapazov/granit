 #include <gkio_base.h>
 #include <algorithm>

 bool     __fastcall TGKIoInterface::load(GKHANDLE owner,wchar_t * dll_name,wchar_t *name)
 {
  TGKModuleLoader mldr;
  if(mldr.load_module(dll_name) && mldr.get_module_type() & MT_IO)
   {
    _handle = mldr.create_module(name);
    if(is_valid())
      {
       init_module_dll();
       module_init(owner,NULL,NULL,NULL);
       return true;
      }
   }
   return false;
 }

 bool     __fastcall TGKIoInterface::load(GKHANDLE owner,char    * dll_name,char   * name)
 {
  TGKModuleLoader mldr;
  if(mldr.load_module(dll_name) && mldr.get_module_type() & MT_IO)
   {
    wchar_t _name[MAX_PATH];
    if(name && *name) KeRTL::Ansi2Unicode(_name,name);
    _handle = mldr.create_module((name && *name) ? _name:NULL);
    if(is_valid())
      {
       init_module_dll();
       module_init(owner,NULL,NULL,NULL);
       return true;
      }
   }
   return false;
 }

 bool     __fastcall TGKIoInterface::get_device_text(wchar_t * text,DWORD bsz)
  {
   wchar_t buf[MAX_PATH];
   LPMIO_DEV_TEXT dt = (LPMIO_DEV_TEXT)buf;
   dt->dw_size = sizeof(buf);
   dt->text_sz = (sizeof(buf)-sizeof(dt->dw_size)-sizeof(dt->text_sz))/sizeof(wchar_t);
   dt->dev_text[0] = 0;
   bool res =  call(CMDIO_GET_DEVICE_TEXT,(LPARAM)dt,0) == GKH_RET_SUCCESS ? true:false;
   if(res)
     wcsncpy(text,dt->dev_text,std::min(bsz,dt->text_sz+1));

   return res;
  }


