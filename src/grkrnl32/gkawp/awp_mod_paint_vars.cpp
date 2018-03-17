#pragma hdrstop
#include "gkawpmodule.hpp"
#include <tlmpainter.hpp>

void    __fastcall TGkAwpModule::handle_paint_image    (OTDM_PROTO_STATE & ps)
{
  TFile & file = main_kadr.get_file();
  if(ps.beg_data)
  {
   UnicodeString  image_name ( ps.p_image_begin->image_name);
   //OutputDebugStringW(image_name.c_str());
   set_string(ps.p_image_begin->image_id,image_name);
   if(!wcsstr(image_name.c_str(),RESOURCE_BITMAP))
     {

      file_image_begin(file,true,ps.p_image_begin);
     }
     else
     file.Close();

  }
  else
    file.Write(ps.p_image_chunk->chunk_data,ps.p_image_chunk->chunk_size);
  if(ps.end_data)
     file.Close() ;
}
//------------------------------------------------------------------------------
void    __fastcall TGkAwpModule::handle_paint_variants (OTDM_PROTO_STATE & ps)
{

  LPMDB_PAINT_VARIANT   pvar_beg = ps.p_paint_vars->pvars;
  LPMDB_PAINT_VARIANT   pvar_end = pvar_beg + ps.p_paint_vars->count;

  if(ps.beg_data)  paint_vars.clear();
  while(pvar_beg<pvar_end)
  {
   mdb_paint_variant paint_var(pvar_beg);
   paint_vars.push_back       (paint_var);
   ++pvar_beg;
  }
  if(ps.end_data)
  {
    painter_dll_setup();
    locker l(mut);
    cc_flags|= CCFL_PAINTVARS;
  }
}
//------------------------------------------------------------------------------
void    __fastcall TGkAwpModule::painter_dll_setup    ()
{

   tlmpaint::painter_dll * pdll = tlmpaint::painter_dll::get_instance();
   int new_pctx = pdll->paint_context_create();
   paint_variants_t::iterator ptr = paint_vars.begin();
   paint_variants_t::iterator end = paint_vars.end  ();

   while(ptr<end)
   {
     mdb_paint_variant & mdb_pv = *ptr;
     LPVOID pv = pdll->paint_var_create(new_pctx);
     UnicodeString resource_prefix = RESOURCE_BITMAP;
     UnicodeString fname = _get_string(mdb_pv.image_id);
     if(!wcsstr(fname.c_str(),resource_prefix.c_str()))
       fname = make_module_file_name(fname,PAINTER_IMAGES_PATH);
     pdll->paint_var_set_image (pv, fname.c_str());
     fname = _get_string(mdb_pv.mask_id);
     if(!wcsstr(fname.c_str(),resource_prefix.c_str()))
        fname = make_module_file_name(fname,PAINTER_IMAGES_PATH);
     pdll->paint_var_set_mask  (pv,fname.c_str());
     pdll->paint_var_set_height(pv,mdb_pv.size.cy);
     pdll->paint_var_set_width(pv,mdb_pv.size.cx);
     pdll->paint_var_set_multipos(pv,ptr->multi_position);
     pdll->paint_var_add(pv,L"");
     ++ptr;
   }
   locker l(mut);
   pdll->paint_context_copy   (paint_context,new_pctx);
   pdll->paint_context_delete (new_pctx);
   kadrs_repaint();

}

 void    __fastcall TGkAwpModule::request_paint_variants ()
 {
   paint_vars_requested = true;
   bool req_beg = true,req_end = false;

   OTDM_PAINT_VARIANTS request;
   bzero(&request,sizeof(request));
   request.pvars->idx = -1;
   request.count = 1;
   send_command(FA_OTD_MEDIUM,req_beg,req_end,OTDMPROTO_CMD_GET_PAINT_VARIANTS|OTDM_ORDER,0,&request,sizeof(request));
 }



