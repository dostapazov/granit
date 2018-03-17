//Редактирование только вариантов отрисовки дискретных сигналов

#pragma hdrstop
#pragma argsused

/*extern "C" int _libmain(unsigned long reason)
{
	return 1;
}
*/

#include <tlm_painter.h>
#include <kertl.hpp>
#include <memory>

 struct tlmobj_paint_context
 {
    DWORD              paint_context;
    Tpaint_variants    discrete_paint_vars;
    Tpaint_variants    analog_paint_vars;
    Tpaint_variants    ready_paint_vars;
    tlmobj_paint_context (DWORD pc):paint_context(pc){}
    ~tlmobj_paint_context(){release();}
    void   __fastcall init_analogs ();
    void   __fastcall init_discrete();
    void   __fastcall init_ready   ();
    void   __fastcall release      (DWORD mask = -1);

    static LRESULT __fastcall paint_var_set_bitmap   (Tpaint_variant* pv,const char    *img_name,bool mask );
    static LRESULT __fastcall paint_var_set_bitmap   (Tpaint_variant* pv,const wchar_t *img_name,bool mask );
    static LRESULT __fastcall paint_var_set_bitmap   (Tpaint_variant* pv,HBITMAP bmp,bool mask );
    static LRESULT __fastcall paint_var_set_width    (Tpaint_variant* pv,DWORD width);
    static LRESULT __fastcall paint_var_set_height   (Tpaint_variant* pv,DWORD height);
    static LRESULT __fastcall paint_var_set_multipos (Tpaint_variant* pv,const int mp);

    static LRESULT __fastcall paint_var_get_width    (Tpaint_variant* pv);
    static LRESULT __fastcall paint_var_get_height   (Tpaint_variant* pv);
    static LRESULT __fastcall paint_var_get_multipos (Tpaint_variant* pv);
    static LRESULT __fastcall paint_var_get_elements (Tpaint_variant* pv);
    static HBITMAP __fastcall paint_var_get_bitmap   (Tpaint_variant* pv,wchar_t * image_name,bool mask   );
    static int     __fastcall paint_var_get_bitmap_name(Tpaint_variant* pv,wchar_t * image_name,bool mask   );

 };
 typedef std::vector<tlmobj_paint_context * > tlmobj_painter_contexts_t;

 enum paint_var_type_t {pv_discrete,pv_analog,pv_ready};

 class tlmobj_painter_t
 {
    private:
    tlmobj_painter_t ();
    ~tlmobj_painter_t();

    static tlmobj_painter_t * instance;

    tlmobj_painter_contexts_t  paint_contexts;


    LRESULT __fastcall paint_var_create      (DWORD pctx);
    LRESULT __fastcall paint_var_delete      (Tpaint_variant * pv);
    LPVOID  __fastcall paint_var_copy        (Tpaint_variant * pv);
    LRESULT __fastcall paint_var_add         (Tpaint_variant* pv,const wchar_t * name);
    LRESULT __fastcall paint_var_remove      (DWORD ctx,DWORD id);


    LRESULT __fastcall paint_vars_clear         (DWORD ctx);
    LRESULT __fastcall paint_var_get            (DWORD ctx,DWORD idx);
    LRESULT __fastcall paint_vars_enum_discrete (DWORD ctx,wchar_t * name,DWORD idx);
    LRESULT __fastcall paint_vars_enum_analog   (DWORD ctx,wchar_t * name,DWORD idx);
    LRESULT __fastcall paint_vars_enum_ready    (DWORD ctx,wchar_t * name,DWORD idx);

    LRESULT __fastcall paint_vars_enum_discrete_ex (DWORD param);
    LRESULT __fastcall paint_vars_enum_analog_ex   (DWORD param);
    LRESULT __fastcall paint_vars_enum_ready_ex    (DWORD param);


    LRESULT __fastcall do_paint_variant         (const LPVOID ptr,const Tpaint_variant * pv);
    LRESULT __fastcall do_paint_variant         (const LPTLMOBJ_PAINT_PARAM   pp, const Tpaint_variant *pv);
    LRESULT __fastcall do_paint_variant         (const LPTLMOBJ_PAINT_PARAM1  pp, const Tpaint_variant *pv);
    LRESULT __fastcall do_paint                 (LPVOID ptr);
    LRESULT __fastcall do_paint                 (const LPTLMOBJ_PAINT_PARAM   pp);
    LRESULT __fastcall do_paint                 (const LPTLMOBJ_PAINT_PARAM1  pp);

    LRESULT __fastcall do_calc_size             (LPVOID ptr,LPSIZE sz);
    LRESULT __fastcall do_calc_size             (const LPTLMOBJ_PAINT_PARAM   pp,LPSIZE sz);
    LRESULT __fastcall do_calc_size             (const LPTLMOBJ_PAINT_PARAM1  pp,LPSIZE sz);
    LRESULT __fastcall do_calc_size_variant     (const LPTLMOBJ_PAINT_PARAM   pp, const Tpaint_variant *pv,LPSIZE sz);
    LRESULT __fastcall do_calc_size_variant     (const LPTLMOBJ_PAINT_PARAM1  pp, const Tpaint_variant *pv,LPSIZE sz);


    void    __fastcall convert_paint_params     (const TLMOBJ_PAINT_PARAM  & pp,TLMOBJ_PAINT_PARAM1 & paint_param);

    tlmobj_paint_context* __fastcall get_paint_context   (DWORD ctx);

    DWORD                 __fastcall paint_context_create();
    DWORD                 __fastcall paint_context_delete(DWORD ctx);
    DWORD                 __fastcall paint_context_copy  (DWORD dst_ctx,DWORD src_ctx,DWORD mask);

    public:
    static tlmobj_painter_t * __fastcall get_instance    ();
    static               void __fastcall release_instance();
    LRESULT operator  ()(DWORD cmd, LPARAM p1,LPARAM p2);
 };





     tlmobj_painter_t * tlmobj_painter_t::instance = NULL;

     tlmobj_painter_t::tlmobj_painter_t ()
     {

     }

    tlmobj_painter_t::~tlmobj_painter_t()
    {

    }

    DWORD                 __fastcall tlmobj_painter_t::paint_context_delete(DWORD ctx)
    {
       if(ctx<paint_contexts.size())
         {
           tlmobj_painter_contexts_t::iterator ptr = paint_contexts.begin();
           std::advance(ptr,ctx);
           delete *ptr;
           *ptr = NULL;
           return GKH_RET_SUCCESS;
         }
      return GKH_RET_ERROR;
    }


    DWORD                 __fastcall tlmobj_painter_t::paint_context_create()
    {
        DWORD ret = 0;
        tlmobj_painter_contexts_t::iterator ptr = paint_contexts.begin();
        tlmobj_painter_contexts_t::iterator end = paint_contexts.end  ();
        while(ptr<end)
             {
              if(NULL == *ptr)
                 {
                  *ptr = new tlmobj_paint_context(ret);
                  return ret;
                 }
              ++ret;
              ++ptr;
             }
        ret = paint_contexts.size();
        paint_contexts.push_back(new tlmobj_paint_context(ret));
        return ret ;
    }

    tlmobj_paint_context* __fastcall tlmobj_painter_t::get_paint_context(DWORD ctx)
    {
       if(!paint_contexts.size())
           paint_context_create();
       if(ctx>= this->paint_contexts.size())
          ctx = 0;
       return paint_contexts.at(ctx);
    }

    void                  __fastcall copy_variants(DWORD dst_ctx, const Tpaint_variants & src,Tpaint_variants & dst)
    {
        dst.resize(src.size());
        Tpaint_variants::const_iterator src_beg = src.begin();
        Tpaint_variants::const_iterator src_end = src.end  ();
        Tpaint_variants::iterator       dst_ptr = dst.begin();
        while(src_beg<src_end)
        {
           Tpaint_variant * pv = *dst_ptr;
           if(pv) {delete pv;}
           pv = new Tpaint_variant(dst_ctx,**src_beg);
           *dst_ptr = pv;
           ++src_beg;
           ++dst_ptr;
        }

    }

    DWORD                 __fastcall tlmobj_painter_t::paint_context_copy  (DWORD dst_ctx,DWORD src_ctx,DWORD mask)
    {
        if(src_ctx<paint_contexts.size())
        {
           tlmobj_paint_context * src = paint_contexts.at(src_ctx);
           if(dst_ctx>paint_contexts.size())
              dst_ctx = paint_context_create();
           tlmobj_paint_context * dst = paint_contexts.at(dst_ctx);
           dst->release(mask);

           if(mask&PAINT_CONTEXT_DISCRETE_MASK)
            copy_variants(dst_ctx,src->discrete_paint_vars,dst->discrete_paint_vars);

           if(mask&PAINT_CONTEXT_ANALOG_MASK)
            copy_variants(dst_ctx,src->analog_paint_vars,dst->analog_paint_vars);

           if(mask&PAINT_CONTEXT_READY_MASK)
            copy_variants(dst_ctx,src->ready_paint_vars,dst->ready_paint_vars);

          return dst_ctx;
        }
      return -1;
    }




   tlmobj_painter_t * __fastcall tlmobj_painter_t::get_instance()
   {
   if(!instance) instance = new tlmobj_painter_t;
    return instance;
   }

 void __fastcall tlmobj_painter_t::release_instance()
 {
   if(instance)
   {
     delete instance;
     instance = NULL;
   }
 }

LRESULT __fastcall tlmobj_painter_t::paint_var_create(DWORD ctx)
{
  tlmobj_paint_context * pctx = this->get_paint_context(ctx);
  Tpaint_variant * pv = new Tpaint_variant(ctx);
  pv->obj_painter =  new discrete_painter_t ;
  return (LRESULT)pv;
}

LRESULT __fastcall tlmobj_painter_t::paint_var_delete(Tpaint_variant * pv)
{
  if( pv )
     {
       delete pv;
       return 0;
     }
  return -1;
}

LPVOID __fastcall tlmobj_painter_t::paint_var_copy(Tpaint_variant * pv)
{
  if( pv )
      return (LPVOID )new Tpaint_variant(pv->paint_context,*pv);
  return 0;
}


    LRESULT __fastcall tlmobj_paint_context::paint_var_set_bitmap(Tpaint_variant* pv,const  char * img_name,bool mask)
    {
     LRESULT ret = -1;
     if(pv)
     {
      ret = paint_var_set_bitmap(pv,(HBITMAP)img_name,mask);
      if(ret == GKH_RET_ERROR)
      {
        HBITMAP bmp = obj_painter_t::load_image  (img_name);
        ret = paint_var_set_bitmap(pv,bmp,mask);
        std::wstring * str = mask ? &pv->mask_file : &pv->image_file;
        if(ret == GKH_RET_SUCCESS)
        {
          str->resize (strlen(img_name)+1,0);
          KeRTL::Ansi2Unicode(const_cast<std::wstring::pointer>(str->data()),str->size(),img_name);
        }
        else
        str->clear();
      }

     }
     return ret;
    }

    LRESULT __fastcall tlmobj_paint_context::paint_var_set_bitmap(Tpaint_variant* pv,const wchar_t * img_name,bool mask)
    {
     LRESULT ret = -1;
     if(pv)
     {

      ret = paint_var_set_bitmap(pv,(HBITMAP)img_name,mask);
      if(ret == GKH_RET_ERROR)
      {
        HBITMAP bmp = obj_painter_t::load_image  (img_name);
        ret = paint_var_set_bitmap(pv,bmp,mask);
        std::wstring * str = mask ? &pv->mask_file : &pv->image_file;

        if(ret == GKH_RET_SUCCESS)
             (*str) = img_name;
             else
             str->clear();
      }
     }
     return ret;
    }

    LRESULT __fastcall tlmobj_paint_context::paint_var_set_bitmap(Tpaint_variant* pv,HBITMAP bmp,bool mask)
    {
     LRESULT ret = GKH_RET_ERROR;
     if(pv)
     {
      BITMAP  i_bmp;
      bzero(&i_bmp,sizeof(i_bmp));
      if(GetObject(bmp,sizeof(i_bmp),&i_bmp))
      {
        ret = GKH_RET_SUCCESS;
        if(mask)
        {
          if(pv->mask) DeleteObject(pv->mask);
             pv->mask = bmp;
        }
        else
        {
          if(pv->image) DeleteObject(pv->image);
          pv->image = bmp;
          pv->elements_count = 1;
          obj_painter_t::get_bitmap_size(bmp,pv->sz);
          pv->release_temp_bmp();
        }
      }
     }
     return ret;
    }


    LRESULT __fastcall tlmobj_paint_context::paint_var_set_width (Tpaint_variant* pv,DWORD width)
    {
     LRESULT ret = -1;
     if(pv && pv->sz.cx != (LONG)width)
     {
       obj_painter_t::get_bitmap_size(pv->image,pv->sz);
       int bmp_width = pv->sz.cx;
       if(width)
       {
           pv->elements_count = bmp_width/width;
           pv->sz.cx = width;
       }
          else
       {
          pv->elements_count = 1;
          pv->sz.cx = bmp_width;
       }
         ret = 0;
     }
     pv->release_temp_bmp();
     return ret;
    }

    LRESULT __fastcall tlmobj_paint_context::paint_var_set_height(Tpaint_variant* pv,DWORD height)
    {
     LRESULT ret = -1;
     if(pv && pv->sz.cy != (LONG)height)
     {
       ret = 0;
       pv->sz.cy = height;
       pv->release_temp_bmp();
     }
     return ret;
    }

    LRESULT __fastcall tlmobj_paint_context::paint_var_get_width    (Tpaint_variant* pv)
    {
     return pv ? pv->sz.cx : 0;
    }

    LRESULT __fastcall tlmobj_paint_context::paint_var_get_height   (Tpaint_variant* pv)
    {
     return pv ? pv->sz.cy : 0;

    }

    LRESULT __fastcall tlmobj_paint_context::paint_var_get_multipos (Tpaint_variant* pv)
    {
     return pv ? pv->multi_position : 0;
    }

    LRESULT __fastcall tlmobj_paint_context::paint_var_get_elements (Tpaint_variant* pv)
    {
        return pv ? pv->elements_count : 0;
    }

    int     __fastcall tlmobj_paint_context::paint_var_get_bitmap_name(Tpaint_variant* pv,wchar_t * image_name,bool mask   )
    {
     if(image_name)
       {
        std::wstring & str = mask ? pv->mask_file : pv->image_file;
        wcscpy(image_name,str.data());
        return wcslen(image_name);
       }
       return 0;
    }

    HBITMAP __fastcall tlmobj_paint_context::paint_var_get_bitmap   (Tpaint_variant* pv,wchar_t * img_name,bool mask   )
    {
     paint_var_get_bitmap_name(pv,img_name,mask);
     return   pv ?   obj_painter_t::clone_bitmap( mask ? pv->mask : pv->image) : NULL;
    }

    LRESULT __fastcall tlmobj_painter_t::paint_var_add(Tpaint_variant* pv, const wchar_t * name)
    {
     LRESULT ret = -1;
     if(pv)
     {
       tlmobj_paint_context *pctx = get_paint_context(pv->paint_context);
       pv->name = name;
       if(!pv->obj_painter)  pv->obj_painter = new discrete_painter_t;
       ret =  pctx->discrete_paint_vars.size();
       pctx->discrete_paint_vars.push_back(pv);
     }
     return ret;
    }

    LRESULT __fastcall tlmobj_painter_t::paint_var_remove(DWORD ctx,DWORD idx)
    {
      tlmobj_paint_context * pctx = this->get_paint_context(ctx);
      if(pctx)
      {
       if(idx<pctx->discrete_paint_vars.size())
          {
            Tpaint_variants::iterator ptr = pctx->discrete_paint_vars.begin();
            advance(ptr,idx);
            delete *ptr;
            pctx->discrete_paint_vars.erase(ptr);
            return 0;
          }

      }
      return -1;
    }

    LRESULT __fastcall tlmobj_painter_t::paint_vars_clear      (DWORD ctx)
    {
        LRESULT ret = 0;
        tlmobj_paint_context * pctx = this->get_paint_context(ctx);
        if(pctx) pctx->discrete_paint_vars.clear();
        return ret;
    }

    LRESULT __fastcall tlmobj_painter_t::paint_var_get         (DWORD ctx,DWORD idx)
    {
        tlmobj_paint_context * pctx = this->get_paint_context(ctx);
        if(pctx && (idx< pctx->discrete_paint_vars.size()) )
          return (LRESULT)pctx->discrete_paint_vars.at(idx);
       return NULL;
    }

   LRESULT __fastcall tlmobj_painter_t::paint_vars_enum_discrete       (DWORD ctx,wchar_t * name,DWORD idx)
    {
      tlmobj_paint_context * pctx = this->get_paint_context(ctx);
      if(pctx &&  (idx<pctx->discrete_paint_vars.size()) && name)
      {
       Tpaint_variant * var = pctx->discrete_paint_vars.at(idx);
       wcscpy(name,var->name.c_str());
       return GKH_RET_SUCCESS;
      }
      return GKH_RET_ERROR;
    }

   LRESULT __fastcall tlmobj_painter_t::paint_vars_enum_analog       (DWORD ctx,wchar_t * name,DWORD idx)
   {
      tlmobj_paint_context * pctx = this->get_paint_context(ctx);
      if(pctx &&  (idx<pctx->analog_paint_vars.size()) && name)
      {
       Tpaint_variant * var = pctx->analog_paint_vars.at(idx);
       wcscpy(name,var->name.c_str());
       return GKH_RET_SUCCESS;
      }
      return GKH_RET_ERROR;
   }

   LRESULT __fastcall tlmobj_painter_t::paint_vars_enum_ready       (DWORD ctx,wchar_t * name,DWORD idx)
   {
      tlmobj_paint_context * pctx = this->get_paint_context(ctx);
      if(pctx &&  (idx<pctx->ready_paint_vars.size()) && name)
      {
       Tpaint_variant * var = pctx->ready_paint_vars.at(idx);
       wcscpy(name,var->name.c_str());
       return GKH_RET_SUCCESS;
      }
      return GKH_RET_ERROR;
   }

    LRESULT __fastcall tlmobj_painter_t::paint_vars_enum_discrete_ex (DWORD param)
    {
      LRESULT ret = GKH_RET_ERROR;
      LPENUM_EX_VARSIANTS_DATA_A a_data = (LPENUM_EX_VARSIANTS_DATA_A)param;
      LPENUM_EX_VARSIANTS_DATA_W w_data = (LPENUM_EX_VARSIANTS_DATA_W)param;
      if(param && ( a_data->cbsz == sizeof(*a_data) || a_data->cbsz == sizeof(*w_data)) )
      {
         tlmobj_paint_context * pctx = get_paint_context(a_data->paint_context);
         if(!pctx->discrete_paint_vars.size())
             pctx->init_discrete();
         if(pctx && a_data->idx < pctx->discrete_paint_vars.size())
            {
             ret = GKH_RET_SUCCESS;
             Tpaint_variant * var = pctx->discrete_paint_vars.at(a_data->idx);
             if(a_data->cbsz == sizeof(*w_data))
                wcsncpy(w_data->text,var->name.c_str(),w_data->text_sz);
                else
                KeRTL::Unicode2Ansi(a_data->text,a_data->text_sz,var->name.c_str());
            }

      }
      return ret;
    }

    LRESULT __fastcall tlmobj_painter_t::paint_vars_enum_analog_ex   (DWORD param)
    {
      LRESULT ret = GKH_RET_ERROR;
      LPENUM_EX_VARSIANTS_DATA_A a_data = (LPENUM_EX_VARSIANTS_DATA_A)param;
      LPENUM_EX_VARSIANTS_DATA_W w_data = (LPENUM_EX_VARSIANTS_DATA_W)param;
      if(param && ( a_data->cbsz == sizeof(*a_data) || a_data->cbsz == sizeof(*w_data)) )
      {
         tlmobj_paint_context * pctx = get_paint_context(a_data->paint_context);
         if(pctx && (a_data->idx < pctx->analog_paint_vars.size()) )
            {
             ret = GKH_RET_SUCCESS;
             Tpaint_variant * var = pctx->analog_paint_vars.at(a_data->idx);
             if(a_data->cbsz == sizeof(*w_data))
                wcsncpy(w_data->text,var->name.c_str(),w_data->text_sz);
                else
                KeRTL::Unicode2Ansi(a_data->text,a_data->text_sz,var->name.c_str());
            }

      }
      return ret;
    }

    LRESULT __fastcall tlmobj_painter_t::paint_vars_enum_ready_ex    (DWORD param)
    {
      LRESULT ret = GKH_RET_ERROR;
      LPENUM_EX_VARSIANTS_DATA_A a_data = (LPENUM_EX_VARSIANTS_DATA_A)param;
      LPENUM_EX_VARSIANTS_DATA_W w_data = (LPENUM_EX_VARSIANTS_DATA_W)param;
      if(param && ( a_data->cbsz == sizeof(*a_data) || a_data->cbsz == sizeof(*w_data)) )
      {
         tlmobj_paint_context * pctx = get_paint_context(a_data->paint_context);
         if(pctx && (a_data->idx < pctx->ready_paint_vars.size()) )
            {
             ret = GKH_RET_SUCCESS;
             Tpaint_variant * var = pctx->ready_paint_vars.at(a_data->idx);
             if(a_data->cbsz == sizeof(*w_data))
                wcsncpy(w_data->text,var->name.c_str(),w_data->text_sz);
                else
                KeRTL::Unicode2Ansi(a_data->text,a_data->text_sz,var->name.c_str());
            }

      }
      return ret;
    }



   void  __fastcall tlmobj_painter_t::convert_paint_params   (const TLMOBJ_PAINT_PARAM & pp,TLMOBJ_PAINT_PARAM1 & paint_param)
   {
    bzero(&paint_param,sizeof(paint_param));
    paint_param.dw_size    = sizeof(paint_param);
    paint_param.entry.options.paint_as = pp.paint_variant;
    paint_param.paint_dc   = pp.paint_dc;
    paint_param.bkgnd_bmp  = pp.bkgnd_bitmap;
    paint_param.font       = pp.font;
    paint_param.erase      = pp.erase;
    paint_param.record     = pp.record;

    paint_param.entry.position = pp.position;
    paint_param.entry.size     = pp.size;
    paint_param.entry.options.bkgnd_color = pp.bkgnd_color;
    paint_param.entry.options.color       = pp.color;
    paint_param.entry.options.prec        = LOWORD(pp.precision);
    paint_param.entry.options.digits      = HIWORD(pp.precision);
    paint_param.short_name                = pp.short_name;
    if(pp.paint_selected)
       paint_param.entry.state |= MDB_KADR_ENTRY_STATE_SELECTED;
   }

   LRESULT __fastcall tlmobj_painter_t::do_paint_variant      (const LPTLMOBJ_PAINT_PARAM  pp, const Tpaint_variant * pv)
   {
    TLMOBJ_PAINT_PARAM1 paint_param;
    convert_paint_params(*pp,paint_param);
    return   do_paint_variant(&paint_param,pv);
   }

   LRESULT __fastcall tlmobj_painter_t::do_paint_variant      (const LPTLMOBJ_PAINT_PARAM1  pp, const Tpaint_variant * pv)
   {
     LRESULT ret = GKH_RET_ERROR;
     if(pv->obj_painter)
     {
       (*pv->obj_painter)(pp,pv);
       ret = GKH_RET_SUCCESS;
     }
     return ret;
   }

   LRESULT __fastcall tlmobj_painter_t::do_paint_variant      (const LPVOID ptr,const Tpaint_variant *pv)
   {
    LRESULT ret = GKH_RET_ERROR;
    if(ptr)
    {
       LPDWORD dw = (LPDWORD)ptr;
       switch(*dw)
       {
         case sizeof(TLMOBJ_PAINT_PARAM) : ret = do_paint_variant((LPTLMOBJ_PAINT_PARAM )ptr,pv);
         break;
         case sizeof(TLMOBJ_PAINT_PARAM1): ret = do_paint_variant((LPTLMOBJ_PAINT_PARAM1)ptr,pv);
         break;
       }
    }
    return ret;
   }

    LRESULT __fastcall tlmobj_painter_t::do_paint              (LPVOID ptr)
    {
      LRESULT ret = GKH_RET_ERROR;
    if(ptr)
    {
       LPDWORD dw = (LPDWORD)ptr;
       switch(*dw)
       {
         case sizeof(TLMOBJ_PAINT_PARAM) : ret = do_paint((LPTLMOBJ_PAINT_PARAM )ptr);
         break;
         case sizeof(TLMOBJ_PAINT_PARAM1): ret = do_paint((LPTLMOBJ_PAINT_PARAM1)ptr);
         break;
       }
    }
    return ret;
    }

    LRESULT __fastcall tlmobj_painter_t::do_paint              (LPTLMOBJ_PAINT_PARAM  pp )
    {
       TLMOBJ_PAINT_PARAM1 paint_param;
       convert_paint_params(*pp,paint_param);
       return do_paint(&paint_param);
    }

    LRESULT __fastcall tlmobj_painter_t::do_paint              (LPTLMOBJ_PAINT_PARAM1 pp )
    {
       LRESULT ret = GKH_RET_ERROR;
       tlmobj_paint_context * ctx  = get_paint_context(pp->paint_ctx);
       Tpaint_variants * vars ;
       if(is_record_discrete(&pp->record))
       {
         vars = &ctx->discrete_paint_vars;
         if(!vars->size())
             ctx->init_discrete();


       }
       else
       {
        vars = &ctx->analog_paint_vars;
            if(!vars->size())  ctx->init_analogs();
       }

       DWORD paint_as = std::min(UINT(pp->entry.options.paint_as),vars->size()-1);
       if(paint_as < vars->size())
          {
            Tpaint_variant * pv = vars->at(paint_as);
            ret = do_paint_variant(pp,pv);
          }

       return ret;
    }

    LRESULT __fastcall tlmobj_painter_t::do_calc_size          (LPVOID ptr,LPSIZE sz)
    {
     LRESULT ret = GKH_RET_ERROR;
        if(ptr)
        {
           LPDWORD dw = (LPDWORD)ptr;
           switch(*dw)
           {
             case sizeof(TLMOBJ_PAINT_PARAM) : ret = do_calc_size((LPTLMOBJ_PAINT_PARAM )ptr,sz);
             break;
             case sizeof(TLMOBJ_PAINT_PARAM1): ret = do_calc_size((LPTLMOBJ_PAINT_PARAM1)ptr,sz);
             break;
           }
        }
     return ret;
    }

    LRESULT __fastcall tlmobj_painter_t::do_calc_size          (const LPTLMOBJ_PAINT_PARAM   pp,LPSIZE sz)
    {
     LRESULT ret = GKH_RET_ERROR;
     TLMOBJ_PAINT_PARAM1 pparam;
     bzero(&pparam,sizeof(pparam));
     pparam.dw_size = sizeof(pparam);
     convert_paint_params(*pp,pparam);
     ret = do_calc_size(&pparam,sz);
     pp->size = pparam.entry.size;
     return ret;
    }

    LRESULT __fastcall tlmobj_painter_t::do_calc_size          (const LPTLMOBJ_PAINT_PARAM1  pp,LPSIZE sz)
    {
       LRESULT ret = GKH_RET_ERROR;
       tlmobj_paint_context * pctx = this->get_paint_context(pp->paint_ctx);
       Tpaint_variants * vars ;
       if(is_record_discrete(&pp->record))
        {
          vars = &pctx->discrete_paint_vars ;
          if(!vars->size()) pctx->init_discrete();
        }
       else
       {
         vars = &pctx->analog_paint_vars;
         if(!vars->size()) pctx->init_analogs();
       }
       DWORD paint_as = pp->entry.options.paint_as;
       SIZE _sz;
       if(paint_as < vars->size())
          {
            Tpaint_variant * pv = vars->at(paint_as);
            ret = do_calc_size_variant(pp,pv,&_sz);

          }
          else
          _sz.cx = _sz.cy = 24;
     if(sz) *sz = _sz;
     return ret;
    }


    LRESULT __fastcall tlmobj_painter_t::do_calc_size_variant  (const LPTLMOBJ_PAINT_PARAM   pp, const Tpaint_variant *pv,LPSIZE sz)
    {
     LRESULT ret = GKH_RET_ERROR;
     TLMOBJ_PAINT_PARAM1 pparam;
     bzero(&pparam,sizeof(pparam));
     pparam.dw_size = sizeof(pparam);
     convert_paint_params(*pp,pparam);
     SIZE _sz;
     ret = do_calc_size_variant(&pparam,pv,&_sz);
     pp->size<<_sz;
     if(sz) *sz = _sz;
     return ret;
    }

    LRESULT __fastcall tlmobj_painter_t::do_calc_size_variant  (const LPTLMOBJ_PAINT_PARAM1  pp, const Tpaint_variant *pv,LPSIZE sz)
    {
     LRESULT ret = GKH_RET_ERROR;
     if(pp && pp->dw_size == sizeof(*pp) && pv && pv->obj_painter)
          ret = pv->obj_painter->do_calc_size(pp,pv,sz);
     return ret;
    }



 LRESULT __fastcall tlmobj_paint_context::paint_var_set_multipos(Tpaint_variant* pv,const int mp)
 {
  if(pv)
  {
   pv->multi_position = mp;
   return GKH_RET_SUCCESS;
  }

  return GKH_RET_ERROR;
 }





LRESULT tlmobj_painter_t::operator ()(DWORD cmd, LPARAM p1,LPARAM p2)
{
  LRESULT ret = -1;
  switch(cmd)
  {
     case PMCMD_COPY_PAINT_VARIANT          : ret = (LRESULT)paint_var_copy  ((Tpaint_variant*)p1);break;
     case PMCMD_CREATE_PAINT_VARIANT_EMPTY  : ret = paint_var_create(p1);break;
     case PMCMD_DELETE_PAINT_VARIANT        : ret = paint_var_delete((Tpaint_variant*)p1);break;
     case PMCMD_PAINT_VAR_CTRL_SET_IMAGE    : ret = tlmobj_paint_context::paint_var_set_bitmap  ((Tpaint_variant*)p1,( char *  )p2,false);break;
     case PMCMD_PAINT_VAR_CTRL_SET_MASK     : ret = tlmobj_paint_context::paint_var_set_bitmap  ((Tpaint_variant*)p1,( char *  )p2,true);break;
     case PMCMD_PAINT_VAR_CTRL_SET_IMAGE_W  : ret = tlmobj_paint_context::paint_var_set_bitmap  ((Tpaint_variant*)p1,(wchar_t *)p2,false);break;
     case PMCMD_PAINT_VAR_CTRL_SET_MASK_W   : ret = tlmobj_paint_context::paint_var_set_bitmap  ((Tpaint_variant*)p1,(wchar_t *)p2,true);break;
     case PMCMD_PAINT_VAR_CTRL_SET_WIDTH    : ret = tlmobj_paint_context::paint_var_set_width   ((Tpaint_variant*)p1,(DWORD)p2);break;
     case PMCMD_PAINT_VAR_CTRL_GET_WIDTH    : ret = tlmobj_paint_context::paint_var_get_width   ((Tpaint_variant*)p1);break;
     case PMCMD_PAINT_VAR_CTRL_SET_HEIGHT   : ret = tlmobj_paint_context::paint_var_set_height  ((Tpaint_variant*)p1,(DWORD)p2);break;
     case PMCMD_PAINT_VAR_CTRL_GET_HEIGHT   : ret = tlmobj_paint_context::paint_var_get_height  ((Tpaint_variant*)p1);break;
     case PMCMD_PAINT_VAR_CTRL_SET_SWITCHED : ret = tlmobj_paint_context::paint_var_set_multipos((Tpaint_variant*)p1,2)       ; break;
     case PMCMD_PAINT_VAR_CTRL_SET_MULTIPOS : ret = tlmobj_paint_context::paint_var_set_multipos((Tpaint_variant*)p1,(int) p2);break;
     case PMCMD_PAINT_VAR_CTRL_GET_MULTIPOS : ret = tlmobj_paint_context::paint_var_get_multipos((Tpaint_variant*)p1);break;
     case PMCMD_PAINT_VAR_CTRL_GET_ELEMENTS : ret = tlmobj_paint_context::paint_var_get_elements((Tpaint_variant*)p1);break;
     case PMCMD_PAINT_VAR_ADD               : ret = paint_var_add((Tpaint_variant*)p1,(const wchar_t*)p2); break;
     case PMCMD_PAINT_VAR_REMOVE            : ret = paint_var_remove(p1,p2); break;
     case PMCMD_PAINT_VAR_CLEAR             : ret = paint_vars_clear(p1);                                  break;
     case PMCMD_PAINT_VAR_GET               : ret = paint_var_get   ((DWORD)p1,(DWORD)p2);                 break;
     case PMCMD_ENUM_DISCRETE_VARIANTS      : ret = paint_vars_enum_discrete (0,(wchar_t * )p1,(DWORD) p2);break;
     case PMCMD_ENUM_DISCRETE_VARIANTS_EX   : ret = paint_vars_enum_discrete_ex(p1)                       ;break;
     case PMCMD_ENUM_ANALOG_VARIANTS        : ret = paint_vars_enum_analog   (0,(wchar_t * )p1,(DWORD) p2);break;
     case PMCMD_ENUM_ANALOG_VARIANTS_EX     : ret = paint_vars_enum_analog_ex(p1)                         ;break;
     case PMCMD_ENUM_READY_VARIANTS         : ret = paint_vars_enum_ready    (0,(wchar_t * )p1,(DWORD) p2);break;
     case PMCMD_ENUM_READY_VARIANTS_EX      : ret = paint_vars_enum_ready_ex (p1)                         ;break;
     case PMCMD_DO_PAINT_VARIANT            : ret = do_paint_variant((LPVOID)p1,(Tpaint_variant*)p2)      ;break;
     case PMCMD_PAINT                       : ret = do_paint((LPVOID)p1)                                  ;break;
     case PMCMD_CALC_SIZE                   : ret = do_calc_size((LPVOID)p1,(LPSIZE)p2);                   break;
     case PMCMD_PAINT_CONTEXT_CREATE        : ret = this->paint_context_create()  ;                        break;
     case PMCMD_PAINT_CONTEXT_DELETE        : ret = this->paint_context_delete(p1);                        break;
     case PMCMD_PAINT_CONTEXT_COPY_ALL      : ret = this->paint_context_copy  (p1,p2,-1);                  break;
     case PMCMD_PAINT_CONTEXT_COPY_DISCRETE : ret = this->paint_context_copy  (p1,p2,PAINT_CONTEXT_DISCRETE_MASK);break;
     case PMCMD_PAINT_CONTEXT_COPY_ANALOGS  : ret = this->paint_context_copy  (p1,p2,PAINT_CONTEXT_ANALOG_MASK  );break;
     case PMCMD_PAINT_CONTEXT_COPY_READY    : ret = this->paint_context_copy  (p1,p2,PAINT_CONTEXT_READY_MASK   );break;


     case PMCMD_PAINT_VAR_CTRL_GET_IMAGE     : ret = (LRESULT)tlmobj_paint_context::paint_var_get_bitmap((Tpaint_variant*)p1,(wchar_t *) p2,false);break;
     case PMCMD_PAINT_VAR_CTRL_GET_MASK      : ret = (LRESULT)tlmobj_paint_context::paint_var_get_bitmap((Tpaint_variant*)p1,(wchar_t *) p2,true );break;
     case PMCMD_PAINT_VAR_CTRL_GET_IMAGE_NAME: ret = (LRESULT)tlmobj_paint_context::paint_var_get_bitmap_name((Tpaint_variant*)p1,(wchar_t *) p2,false);break;
     case PMCMD_PAINT_VAR_CTRL_GET_MASK_NAME : ret = (LRESULT)tlmobj_paint_context::paint_var_get_bitmap_name((Tpaint_variant*)p1,(wchar_t *) p2,true );break;
  }
  return ret;
}

LRESULT WINAPI module_main(DWORD cmd,LPARAM p1,LPARAM p2)
{
 LRESULT ret = 0;
 tlmobj_painter_t * tp = tlmobj_painter_t::get_instance();
 if(cmd==GKME_CMD_UNLOAD_MODULE)
    tlmobj_painter_t::release_instance();
    else
    ret = (*tp)(cmd,p1,p2);
 return  ret;
}

void   __fastcall paint_var_deleter(Tpaint_variant * pv)
{
 if(pv)
    delete pv;
}

void   __fastcall tlmobj_paint_context::release      (DWORD mask)
{
 if(mask&PAINT_CONTEXT_DISCRETE_MASK)
 {
  std::for_each(discrete_paint_vars.begin(),discrete_paint_vars.end(),paint_var_deleter);
  discrete_paint_vars.clear();
 }

 if(mask&PAINT_CONTEXT_ANALOG_MASK)
 {
  std::for_each(analog_paint_vars.begin(),analog_paint_vars.end(),paint_var_deleter);
  analog_paint_vars.clear();
 }

 if(mask&PAINT_CONTEXT_READY_MASK)
 {
  std::for_each(ready_paint_vars.begin(),ready_paint_vars.end(),paint_var_deleter);
  ready_paint_vars.clear();
 }
}


void   __fastcall tlmobj_paint_context::init_discrete()
{
  Tpaint_variant * pv ;
  pv  = new Tpaint_variant(paint_context);
  pv->obj_painter = new discrete_painter_t;
  pv->multi_position = 1;
  pv->name  = L"Коротким именем";
  discrete_paint_vars.push_back(pv);

  wchar_t   image_name[MAX_PATH];
  int len = snwprintf(image_name,KERTL_ARRAY_COUNT(image_name),L"%s",RESOURCE_BITMAP);

  pv  = new Tpaint_variant(paint_context);
  pv->obj_painter = new discrete_painter_t;
  snwprintf(image_name+len,KERTL_ARRAY_COUNT(image_name)-len,L"%d",IDBMP_DISCRETE_LAMP);
  paint_var_set_bitmap(pv,image_name,false);
  snwprintf(image_name+len,KERTL_ARRAY_COUNT(image_name)-len,L"%d",IDBMP_DISCRETE_LAMP_MASK);
  paint_var_set_bitmap(pv,image_name,true);
  int elements_count = pv->sz.cx / pv->sz.cy;
  paint_var_set_width  (pv,pv->sz.cx / elements_count);
  pv->multi_position = 1;
  pv->name  = L"Лампой";
  discrete_paint_vars.push_back(pv);

  pv  = new Tpaint_variant(paint_context);
  pv->obj_painter = new discrete_painter_t;
  snwprintf(image_name+len,KERTL_ARRAY_COUNT(image_name)-len,L"%d",IDBMP_DISCRETE_SQUARE);
  paint_var_set_bitmap(pv,image_name,false);
  snwprintf(image_name+len,KERTL_ARRAY_COUNT(image_name)-len,L"%d",IDBMP_DISCRETE_SQUARE_MASK);
  paint_var_set_bitmap(pv,image_name,true);
  elements_count = pv->sz.cx / pv->sz.cy;
  paint_var_set_width  (pv,pv->sz.cx / elements_count);
  pv->multi_position = 1;
  pv->name  = L"Квадратом";
  discrete_paint_vars.push_back(pv);

  pv  = new Tpaint_variant(paint_context);
  pv->obj_painter = new discrete_painter_t;
  paint_var_set_bitmap(pv,LoadBitmap(ModuleInstance,MAKEINTRESOURCE(IDBMP_HSWITCH)),false);
  paint_var_set_bitmap(pv,LoadBitmap(ModuleInstance,MAKEINTRESOURCE(IDBMP_HSWITCH_MASK)),true );
  paint_var_set_width  (pv,32);
  pv->multi_position = 2;
  pv->name  = L"Разъеденитель горизонтальный";
  discrete_paint_vars.push_back(pv);

  pv  = new Tpaint_variant(paint_context);
  pv->obj_painter = new discrete_painter_t;
  snwprintf(image_name+len,KERTL_ARRAY_COUNT(image_name)-len,L"%d",IDBMP_VSWITCH);
  paint_var_set_bitmap(pv,image_name,false);
  snwprintf(image_name+len,KERTL_ARRAY_COUNT(image_name)-len,L"%d",IDBMP_VSWITCH_MASK);
  paint_var_set_bitmap(pv,image_name,true);
  //pv->elements_count = pv->sz.cx / pv->sz.cy;
  paint_var_set_width  (pv,22);
  pv->multi_position = 2;
  pv->name  = L"Разъеденитель вертикальный";
  discrete_paint_vars.push_back(pv);


}

void   __fastcall tlmobj_paint_context::init_analogs()
{
  //obj_painter_t::set_question_sign(L'X');
  // paint_variant 0
  Tpaint_variant * pv ;
  pv  = new Tpaint_variant(paint_context);
  pv->frame_pen_width = 2;
  pv->obj_painter = new analog_text_painter_t(false);
  pv->name = L"текст";
  analog_paint_vars.push_back(pv);

  // paint_variant 1
  pv  = new Tpaint_variant(paint_context);
  pv->frame_pen_width = 2;
  pv->obj_painter = new analog_bar_painter_t(false,true);
  pv->name = L"bar Горизонтальный";
  analog_paint_vars.push_back(pv);

  // paint_variant 2
  pv  = new Tpaint_variant(paint_context);
  pv->frame_pen_width = 2;
  pv->obj_painter = new analog_bar_painter_t(true,true);
  pv->name = L"bar вертикальный";
  analog_paint_vars.push_back(pv);

//  pv  = new Tpaint_variant(paint_context);
//  pv->obj_painter = new analog_bar_painter_t(false,true);
//  pv->name = "ТИ abs-bar Горизонтальный";
//  analog_paint_vars.push_back(pv);
//
//  pv  = new Tpaint_variant(paint_context);
//  pv->obj_painter = new analog_bar_painter_t(true,true);
//  pv->name = "ТИ abs-bar вертикальный";
//  analog_paint_vars.push_back(pv);

  // paint_variant 3
  pv  = new Tpaint_variant(paint_context);
  pv->frame_pen_width = 2;
  paint_var_set_bitmap(pv,LoadBitmap(ModuleInstance,MAKEINTRESOURCE(IDBMP_LED_DIGITS     )),false);
  paint_var_set_bitmap(pv,LoadBitmap(ModuleInstance,MAKEINTRESOURCE(IDBMP_LED_DIGITS_MASK)),true );
  paint_var_set_width(pv,pv->sz.cx/13);
  pv->obj_painter = new analog_led_painter_t();
  pv->name = L"LED-Индикаторы";
  analog_paint_vars.push_back(pv);
}

void   __fastcall tlmobj_paint_context::init_ready  ()
{

}



