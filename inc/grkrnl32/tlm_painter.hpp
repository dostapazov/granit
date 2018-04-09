#ifndef _TLM_PAINTER_HPP_INC_
#define _TLM_PAINTER_HPP_INC_

#include     <tlm_painter.h>
#include     <record_table.hpp>
#include     <kadrs_table.hpp>
#include     <text_table.hpp>
#include     <gkmodules.hpp>
#include     <freeimage.hpp>
#ifdef __WIN32__
#pragma link ("freeimage32.lib")
#endif

namespace tlmpaint
{
      inline HBITMAP __fastcall load_image(const wchar_t * image_name)
      {
       return freeimage::TFreeImage::load_image(image_name);
      }

      class  TDC
      {
       public:
         TDC():dc(NULL){};
         TDC (HDC src     )   {dc = src;}
         TDC (const TDC & src){*this = src;}
        ~TDC ()               {if(dc) DeleteDC(dc);}
       HDC operator ()()const      {return dc;}
       TDC & operator = (const HDC   src_dc) { if(dc) DeleteDC(dc); if(src_dc) dc = CreateCompatibleDC(src_dc);else dc = NULL; return * this;}
       TDC & operator = (const TDC & src   ) { return (*this) = src();  }
       private:
        HDC   dc;
      };

      template <typename Hgdiobj>
      class  TGdiObject
      {
       public:
       TGdiObject():gdi_object(NULL)            {}
       TGdiObject(Hgdiobj _obj):gdi_object(_obj){}
       ~TGdiObject(){if(gdi_object )DeleteObject(gdi_object);}
       TGdiObject & operator = (Hgdiobj _obj){if(gdi_object )DeleteObject(gdi_object); gdi_object = _obj; return *this;}
       Hgdiobj operator()(){return gdi_object;}
       private:
       Hgdiobj   gdi_object;
      };

      struct TGdiSetter
      {
       HGDIOBJ operator()(HDC dc,HGDIOBJ gdi_obj){return (HGDIOBJ)SelectObject(dc,gdi_obj);}
      };

      struct TRop2Setter
      {
       int operator ()(HDC dc,int rop){return SetROP2(dc,rop);}
      };

      struct TTextColorSetter
      {
       COLORREF operator ()(HDC dc,COLORREF color) { return SetTextColor(dc,color); }
      };

      struct TTextBkModeSetter
      {
       int operator ()(HDC dc,int mode) { return SetBkMode(dc,mode); }
      };


      template <typename Setter,typename Object>
      class TSetter
      {
       public:
         TSetter(HDC _dc,const Object _obj):dc_(_dc)
         {
          object_ = setter_(dc_,_obj);
         }
        ~TSetter(){setter_(dc_,object_);}
       HDC operator ()(){return dc_;}
       Object   operator()(Object _obj){return setter_(dc_,_obj);}
       private:
       HDC      dc_;
       Setter   setter_;
       Object   object_;
      };

      typedef TSetter<TGdiSetter,HGDIOBJ>        TGdiSwap;
      typedef TSetter<TRop2Setter,int>           TRopSwap;
      typedef TSetter<TTextColorSetter,COLORREF> TTextColorSwap;
      typedef TSetter<TTextBkModeSetter,int>     TTextBkModeSwap;

      class TBmpBkgnd
      {
       public:
       enum bkgnd_mode_t {
                          bkgnd_default = MDB_KADR_FLAG_IMAGE_BKGND_DEFAULT,
                          bkgnd_center  = MDB_KADR_FLAG_IMAGE_BKGND_CENTER,
                          bkgnd_stretch = MDB_KADR_FLAG_IMAGE_BKGND_STRETCH,
                          bkgnd_stretch_proportianal = MDB_KADR_FLAG_IMAGE_BKGND_STRETCH_PROPORTIONAL
                         };
       TBmpBkgnd();
       ~TBmpBkgnd(){release();}
       void    __fastcall set_grid  ( int _grid_step);
       int     __fastcall get_grid  (){return grid;}
       void    __fastcall set_grayed(bool _grayed   );
       bool    __fastcall is_grayed (){return grayed;}
       void    __fastcall set_size  ( int _width, int _height);
       void    __fastcall set_mode  ( bkgnd_mode_t mode    );
       void    __fastcall set_bitmap(HBITMAP bmp,bool del_src = true);
       void    __fastcall paint     ( const HDC paint_dc, const RECT & r);
       HBITMAP operator()(){return bkgnd_bmp;}
       static
       HBITMAP __fastcall create_bitmap(int width,int height);
       template <typename VarType>
       void    __fastcall get_size  ( VarType & _width, VarType & _height)
       {
         _width = width; _height = height;
       }

       SIZE    __fastcall get_src_bitmap_size  ();

       protected:
       void      __fastcall setup_bitmap ();
       void      __fastcall create_bitmap();
       void      __fastcall fill_bitmap  ();
       void      __fastcall release_bkgnd_bmp();
       void      __fastcall release      ();
       void      __fastcall calc_stretch_proportional_size(const SIZE src_size,SIZE & result);
       void      __stdcall  setup_grid_and_gray();
       void      __fastcall do_setup_grid(LPRGBQUAD b_arr, int bmp_width, int bmp_height);
       void      __fastcall do_gray_bkgnd(LPRGBQUAD b_arr,DWORD count);


       private:
        HDC              dc;
        bool             grayed;
        int              grid;
        bkgnd_mode_t     bkgnd_mode;
        int              width;
        int              height;
        HBITMAP          src_bmp;
        HBITMAP          bkgnd_bmp;
      };

      class painter_dll
      {
        public:
        static painter_dll *  __fastcall  get_instance ();
        static        void    __fastcall  release_instance ();
                      bool    __fastcall  load_library(wstring & _lib_name);
                      bool    __fastcall  unload_library();
                      LRESULT __fastcall  call(DWORD cmd,LPARAM p1,LPARAM p2);
                      bool    __fastcall  enum_discretes(int idx,wchar_t * name,int cb_sz);
                      bool    __fastcall  enum_analogs  (int idx,wchar_t * name,int cb_sz);
                      bool    __fastcall  enum_ready    (int idx,wchar_t * name,int cb_sz);

                      bool    __fastcall  enum_discretes_ex(int ctx,int idx,char * name,int cb_sz);
                      bool    __fastcall  enum_analogs_ex  (int ctx,int idx,char * name,int cb_sz);
                      bool    __fastcall  enum_ready_ex    (int ctx,int idx,char * name,int cb_sz);
                      bool    __fastcall  enum_discretes_ex(int ctx,int idx,wchar_t * name,int cb_sz);
                      bool    __fastcall  enum_analogs_ex  (int ctx,int idx,wchar_t * name,int cb_sz);
                      bool    __fastcall  enum_ready_ex    (int ctx,int idx,wchar_t * name,int cb_sz);

                      DWORD   __fastcall  paint_context_create();
                      DWORD   __fastcall  paint_context_delete(DWORD cxt);
                      DWORD   __fastcall  paint_context_copy           (DWORD dst_cxt,DWORD src_ctx);
                      DWORD   __fastcall  paint_context_copy_discrete  (DWORD dst_cxt,DWORD src_ctx);
                      DWORD   __fastcall  paint_context_copy_analog    (DWORD dst_cxt,DWORD src_ctx);
                      DWORD   __fastcall  paint_context_copy_ready     (DWORD dst_cxt,DWORD src_ctx);

                      LPVOID  __fastcall  paint_var_get           (int pctx,int idx);
                      LPVOID  __fastcall  paint_var_create        (DWORD  pctx);
                      LRESULT __fastcall  paint_var_delete        (LPVOID pv  );
                      LPVOID  __fastcall  paint_var_get_copy      (LPVOID  pv);
                      HBITMAP __fastcall  paint_var_get_image     (LPVOID pv,wchar_t * name);
                      HBITMAP __fastcall  paint_var_get_mask      (LPVOID pv,wchar_t * name);
                      int     __fastcall  paint_var_get_image_name(LPVOID pv,wchar_t * name);
                      int     __fastcall  paint_var_get_mask_name (LPVOID pv,wchar_t * name);
                      LRESULT __fastcall  paint_var_set_image     (LPVOID pv,const char    * image_name);
                      LRESULT __fastcall  paint_var_set_image     (LPVOID pv,const wchar_t * image_name);
                      LRESULT __fastcall  paint_var_set_image     (LPVOID pv,HBITMAP bmp);
                      LRESULT __fastcall  paint_var_set_mask      (LPVOID pv,const  char   * image_name);
                      LRESULT __fastcall  paint_var_set_mask      (LPVOID pv,const wchar_t * image_name);
                      LRESULT __fastcall  paint_var_set_mask      (LPVOID pv,HBITMAP bmp);
                      LRESULT __fastcall  paint_var_set_width     (LPVOID pv,int width);
                      LRESULT __fastcall  paint_var_get_width     (LPVOID pv);
                      LRESULT __fastcall  paint_var_get_height    (LPVOID pv);
                      LRESULT __fastcall  paint_var_get_elements  (LPVOID pv);
                      LRESULT __fastcall  paint_var_get_multipos  (LPVOID pv);

                      LRESULT __fastcall  paint_var_set_height  (LPVOID pv,int height);
                      LRESULT __fastcall  paint_var_set_switched(LPVOID pv,bool switched);
                      int     __fastcall  paint_var_add         (LPVOID pv,const wchar_t * name);
                      int     __fastcall  paint_var_remove      (DWORD pctx,DWORD idx);
                      LRESULT __fastcall  paint                 (LPVOID pp,LPVOID pvar = NULL);
                      LRESULT __fastcall  calc_size             (LPVOID pp,LPSIZE sz);
                      LRESULT __fastcall  paint_var_set_multipos(LPVOID pv,int mpos);
            static    bool    __fastcall  is_loaded()           {return instance && dll.is_loaded();}
        private:
        painter_dll(){};
        std::wstring lib_name;
        static TGKModuleLoader  dll;
        static painter_dll * instance;
      };


//      inline implementations

 inline TBmpBkgnd::TBmpBkgnd():
                  dc        ( NULL ),
                  grayed    (false ),
                  grid      (  0   ),
                  bkgnd_mode(bkgnd_default),
                  width     (  0   ),
                  height    (  0   ),
                  src_bmp   ( NULL ),
                  bkgnd_bmp ( NULL )
        {}



// inline implementation

inline   bool    __fastcall  painter_dll::enum_discretes(int idx,wchar_t * name,int /*cb_sz*/)
   {
     return  call(PMCMD_ENUM_DISCRETE_VARIANTS,(LPARAM)name,idx) == GKH_RET_SUCCESS ? true : false;
   }

inline   bool    __fastcall  painter_dll::enum_analogs  (int idx,wchar_t * name,int cb_sz)
   {
     return  call(PMCMD_ENUM_ANALOG_VARIANTS,(LPARAM)name,idx) == GKH_RET_SUCCESS ? true : false;
   }

inline   bool    __fastcall  painter_dll::enum_ready    (int idx,wchar_t * name,int cb_sz)
   {
     return  call(PMCMD_ENUM_READY_VARIANTS,(LPARAM)name,idx) == GKH_RET_SUCCESS ? true : false;
   }


inline  bool    __fastcall  painter_dll::enum_discretes_ex(int paint_ctx,int idx,char * name,int cb_sz)
{
   ENUM_EX_VARSIANTS_DATA_A data;
   data.cbsz = sizeof(data);
   data.paint_context = paint_ctx;
   data.idx  = idx;
   data.text_sz = cb_sz;
   data.text    = name;
   return call(PMCMD_ENUM_DISCRETE_VARIANTS_EX,(LPARAM)&data,0) == GKH_RET_SUCCESS ? true : false;
}

inline  bool    __fastcall  painter_dll::enum_discretes_ex(int paint_ctx,int idx,wchar_t * name,int cb_sz)
{
   ENUM_EX_VARSIANTS_DATA_W data;
   data.cbsz = sizeof(data);
   data.paint_context = paint_ctx;
   data.idx  = idx;
   data.text_sz = cb_sz;
   data.text    = name;
   return call(PMCMD_ENUM_DISCRETE_VARIANTS_EX,(LPARAM)&data,0)== GKH_RET_SUCCESS ? true : false;
}

inline  bool    __fastcall  painter_dll::enum_analogs_ex  (int paint_ctx,int idx,char * name,int cb_sz)
{
   ENUM_EX_VARSIANTS_DATA_A data;
   data.cbsz = sizeof(data);
   data.paint_context = paint_ctx;
   data.idx  = idx;
   data.text_sz = cb_sz;
   data.text    = name;
   return call(PMCMD_ENUM_ANALOG_VARIANTS_EX,(LPARAM)&data,0)== GKH_RET_SUCCESS ? true : false;
}

inline  bool    __fastcall  painter_dll::enum_analogs_ex  (int paint_ctx,int idx,wchar_t * name,int cb_sz)
{
   ENUM_EX_VARSIANTS_DATA_W data;
   data.cbsz = sizeof(data);
   data.paint_context = paint_ctx;
   data.idx  = idx;
   data.text_sz = cb_sz;
   data.text    = name;
   return call(PMCMD_ENUM_ANALOG_VARIANTS_EX,(LPARAM)&data,0)== GKH_RET_SUCCESS ? true : false;
}


inline  bool    __fastcall  painter_dll::enum_ready_ex    (int paint_ctx,int idx,char * name,int cb_sz)
{
   ENUM_EX_VARSIANTS_DATA_A data;
   data.cbsz = sizeof(data);
   data.paint_context = paint_ctx;
   data.idx  = idx;
   data.text_sz = cb_sz;
   data.text    = name;
   return call(PMCMD_ENUM_READY_VARIANTS_EX,(LPARAM)&data,0)== GKH_RET_SUCCESS ? true : false;
}

inline  bool    __fastcall  painter_dll::enum_ready_ex    (int paint_ctx,int idx,wchar_t * name,int cb_sz)
{
   ENUM_EX_VARSIANTS_DATA_W data;
   data.cbsz = sizeof(data);
   data.paint_context = paint_ctx;
   data.idx  = idx;
   data.text_sz = cb_sz;
   data.text    = name;
   return call(PMCMD_ENUM_READY_VARIANTS_EX,(LPARAM)&data,0)== GKH_RET_SUCCESS ? true : false;
}


inline  DWORD   __fastcall  painter_dll::paint_context_create()
{
   return call(PMCMD_PAINT_CONTEXT_CREATE,0,0);
}

inline  DWORD   __fastcall  painter_dll::paint_context_delete(DWORD ctx)
{
   return call(PMCMD_PAINT_CONTEXT_DELETE,ctx,0);
}

inline DWORD   __fastcall  painter_dll::paint_context_copy   (DWORD dst_ctx,DWORD src_ctx)
{
  return call(PMCMD_PAINT_CONTEXT_COPY_ALL,dst_ctx,src_ctx);
}

inline DWORD   __fastcall  painter_dll::paint_context_copy_discrete  (DWORD dst_ctx,DWORD src_ctx)
{
  return call(PMCMD_PAINT_CONTEXT_COPY_DISCRETE,dst_ctx,src_ctx);
}

inline DWORD   __fastcall  painter_dll::paint_context_copy_analog    (DWORD dst_ctx,DWORD src_ctx)
{
  return call(PMCMD_PAINT_CONTEXT_COPY_ANALOGS,dst_ctx,src_ctx);
}

inline DWORD   __fastcall  painter_dll::paint_context_copy_ready     (DWORD dst_ctx,DWORD src_ctx)
{
  return call(PMCMD_PAINT_CONTEXT_COPY_READY,dst_ctx,src_ctx);
}



inline   LPVOID  __fastcall  painter_dll::paint_var_get (int pctx,int idx)
   {
     return (LPVOID)call(PMCMD_PAINT_VAR_GET,pctx,idx);
   }

inline   LPVOID  __fastcall  painter_dll::paint_var_create(DWORD pctx )
   {
      return (LPVOID)call(PMCMD_CREATE_PAINT_VARIANT_EMPTY,pctx,0);
   }

inline LRESULT  __fastcall  painter_dll::paint_var_delete  (LPVOID  pv)
{
  return call(PMCMD_DELETE_PAINT_VARIANT,(LPARAM)pv,0);
}

inline LPVOID   __fastcall  painter_dll::paint_var_get_copy  (LPVOID  pv)
{
  return (LPVOID)call(PMCMD_COPY_PAINT_VARIANT,(LPARAM)pv,0);
}


inline   LRESULT __fastcall  painter_dll::paint_var_set_image   (LPVOID pv,const wchar_t * image_name)
   {
     return call(PMCMD_PAINT_VAR_CTRL_SET_IMAGE_W,(LPARAM)pv,(LPARAM)image_name);
   }

   inline   LRESULT __fastcall  painter_dll::paint_var_set_image   (LPVOID pv,const char  * image_name)
   {
     return call(PMCMD_PAINT_VAR_CTRL_SET_IMAGE,(LPARAM)pv,(LPARAM)image_name);
   }

inline   LRESULT __fastcall  painter_dll::paint_var_set_image   (LPVOID pv,HBITMAP bmp)
   {
     return call(PMCMD_PAINT_VAR_CTRL_SET_IMAGE,(LPARAM)pv,(LPARAM)bmp);
   }

inline  HBITMAP __fastcall  painter_dll::paint_var_get_image   (LPVOID pv,wchar_t * name)
{
  return (HBITMAP)call(PMCMD_PAINT_VAR_CTRL_GET_IMAGE,(LPARAM)pv,(LPARAM)name);
}

inline  int     __fastcall  painter_dll::paint_var_get_image_name(PVOID pv,wchar_t * name)
{
  return call(PMCMD_PAINT_VAR_CTRL_GET_IMAGE_NAME,(LPARAM)pv,(LPARAM)name);
}


inline  HBITMAP __fastcall  painter_dll::paint_var_get_mask      (LPVOID pv,wchar_t * name)
{
  return (HBITMAP)call(PMCMD_PAINT_VAR_CTRL_GET_MASK,(LPARAM)pv,(LPARAM)name);
}

inline  int     __fastcall  painter_dll::paint_var_get_mask_name(LPVOID pv,wchar_t * name)
{
  return call(PMCMD_PAINT_VAR_CTRL_GET_MASK_NAME,(LPARAM)pv,(LPARAM)name);
}


inline   LRESULT __fastcall  painter_dll::paint_var_set_mask    (LPVOID pv,const wchar_t * image_name)
   {
     return call(PMCMD_PAINT_VAR_CTRL_SET_MASK_W,(LPARAM)pv,(LPARAM)image_name);
   }

inline   LRESULT __fastcall  painter_dll::paint_var_set_mask    (LPVOID pv,const  char   * image_name)
   {
     return call(PMCMD_PAINT_VAR_CTRL_SET_MASK,(LPARAM)pv,(LPARAM)image_name);
   }


inline   LRESULT __fastcall  painter_dll::paint_var_set_mask    (LPVOID pv,HBITMAP bmp)
   {
     return call(PMCMD_PAINT_VAR_CTRL_SET_MASK,(LPARAM)pv,(LPARAM)bmp);
   }

inline   LRESULT __fastcall  painter_dll::paint_var_set_width   (LPVOID pv,int width)
   {
     return call(PMCMD_PAINT_VAR_CTRL_SET_WIDTH,(LPARAM)pv,width);
   }

inline   LRESULT __fastcall  painter_dll::paint_var_set_height  (LPVOID pv,int height)
   {
     return call(PMCMD_PAINT_VAR_CTRL_SET_HEIGHT,(LPARAM)pv,height);
   }

inline   LRESULT __fastcall  painter_dll::paint_var_set_switched(LPVOID pv,bool switched)
   {
     return call(PMCMD_PAINT_VAR_CTRL_SET_SWITCHED,(LPARAM)pv,switched);
   }

inline  int     __fastcall  painter_dll::paint_var_add         (LPVOID pv,const wchar_t * name)
  {
    return call(PMCMD_PAINT_VAR_ADD,(LPARAM)pv,(LPARAM)name);
  }

inline  int     __fastcall  painter_dll::paint_var_remove     (DWORD pctx,DWORD idx)
  {
    return call(PMCMD_PAINT_VAR_REMOVE,pctx,idx);
  }

inline  LRESULT __fastcall  painter_dll::paint_var_set_multipos(LPVOID pv,int mpos)
  {
     return call(PMCMD_PAINT_VAR_CTRL_SET_MULTIPOS,(LPARAM)pv,mpos);
  }

inline  LRESULT __fastcall  painter_dll::paint_var_get_width   (LPVOID pv)
{
     return call(PMCMD_PAINT_VAR_CTRL_GET_WIDTH,(LPARAM)pv,0);
}

inline  LRESULT __fastcall  painter_dll::paint_var_get_height   (LPVOID pv)
{
     return call(PMCMD_PAINT_VAR_CTRL_GET_HEIGHT,(LPARAM)pv,0);
}

inline  LRESULT __fastcall  painter_dll::paint_var_get_elements(LPVOID pv)
{
     return call(PMCMD_PAINT_VAR_CTRL_GET_ELEMENTS,(LPARAM)pv,0);
}

inline  LRESULT __fastcall  painter_dll::paint_var_get_multipos(LPVOID pv)
{
     return call(PMCMD_PAINT_VAR_CTRL_GET_MULTIPOS,(LPARAM)pv,0);
}


inline  LRESULT __fastcall  painter_dll::paint  (LPVOID pp,LPVOID pvar)
{
   return call(pvar ? PMCMD_DO_PAINT_VARIANT : PMCMD_PAINT,(LPARAM)pp,(LPARAM)pvar);
}


inline  LRESULT __fastcall  painter_dll::calc_size             (LPVOID pp,LPSIZE sz)
{
    return call(PMCMD_CALC_SIZE,(LPARAM)pp,(LPARAM)sz);
}


 template <typename vector_t>
 struct index_access
 {
   const typename vector_t::value_type & operator()(const vector_t & v,  const typename vector_t::size_type & idx)
   {
    return v.at(idx);
   }
 };

 template <typename vector_t,typename action_t>
 class index_action_t
 {
   vector_t & data;
   action_t & action;
   public:
   index_action_t(vector_t & _v, action_t & _act):data(_v),action(_act){}
   index_action_t(const index_action_t & src ):data ( src.data),action ( src.action){}
   void operator ()(const typename vector_t::size_type & idx)
   {
    action(data.at(idx));
   }
 };

 template <typename getter_t>
 class paint_action_t
 {
  tlmpaint::painter_dll * paint_dll;
  PVOID               paint_variant;
  TLMOBJ_PAINT_PARAM1 pp;
  bool                flash_erase;
  const getter_t   * _getter;

  paint_action_t(){};
  public:
  paint_action_t(DWORD paint_ctx,const HDC dc,HFONT font
                 ,const HBITMAP bkgnd,bool erase,bool _flash_erase
                 ,getter_t * getter_
                 )
                 :flash_erase(_flash_erase)
                 ,_getter( getter_ )
  {
   bzero(&pp,sizeof(pp));
   pp.dw_size   = sizeof(pp);
   pp.paint_ctx = paint_ctx;
   pp.paint_dc  = dc;
   pp.font      = font;
   pp.bkgnd_bmp = bkgnd;
   paint_dll  =  tlmpaint::painter_dll::get_instance();
   pp.erase   = erase;
  }

  paint_action_t (const paint_action_t & _src){*this = _src;}
  paint_action_t & operator = (const paint_action_t & _src) {pp = _src.pp;_getter = _src._getter;flash_erase = _src.flash_erase; return *this;}
  void operator ()(const mdb_kadr_entry & entry);
  SIZE calc_size (const mdb_kadr_entry & entry);
 };

   template <typename getter_t>
   void paint_action_t<getter_t>::operator ()(const mdb_kadr_entry & entry)
   {
      if(_getter)
      {
       pp.entry = entry;
       bool     rec_visible = true;
       _getter->get_record(pp.entry.rec_id,pp.record,&rec_visible);
       if(rec_visible)
       {
          bool is_selected = entry.is_selected();
          bool is_flashed  =  is_record_flashing(&pp.record) ;
          if(entry.options.part_of_background) is_flashed = false;

          pp.erase &= ~TLMOBJ_PAINT_FLAG_ERASE_FLASH;
          if(flash_erase   )
          {
            if(is_flashed)
               pp.erase |= TLMOBJ_PAINT_FLAG_ERASE_FLASH;
            if(is_selected)
               pp.entry.state &= ~MDB_KADR_ENTRY_STATE_SELECTED;
          }
           pp.short_name = _getter->get_name(pp.record,true);
           paint_dll->paint(&pp,_getter->get_paint_variant(entry));
       }
      }
   }

   template <typename getter_t>
   SIZE paint_action_t<getter_t>::calc_size (const mdb_kadr_entry & entry)
   {
     SIZE sz;
      if(_getter)
      {
       pp.entry = entry;
       _getter->get_record(pp.entry.rec_id,pp.record,NULL);
       pp.short_name = _getter->get_name(pp.record,true);
       paint_dll->calc_size(&pp,&sz);
      }
      else
      sz.cx = sz.cy = 0;
     return sz;
   }




class move_action_t
{
 private:
 int dx,dy;
 move_action_t(){}
 public:
 move_action_t(int _dx,int _dy):dx(_dx),dy(_dy){}
 move_action_t(const move_action_t & src){*this = src;}
 move_action_t & operator = (const move_action_t & src){dx = src.dx;dy = src.dy; return *this;}
 void operator ()(mdb_kadr_entry & entry)
 {
   entry.position.x +=dx;
   entry.position.y +=dy;
 }
};

class size_action_t
{
 private:
 int dx,dy;
 size_action_t(){}
 public:
 size_action_t(int _dx,int _dy):dx(_dx),dy(_dy){}
 size_action_t(const size_action_t & src){*this = src;}
 size_action_t & operator = (const size_action_t & src){dx = src.dx;dy = src.dy; return *this;}
 void operator ()(mdb_kadr_entry & entry)
 {
    entry.size.cx +=dx;
    entry.size.cy +=dy;
 }
};

} //end of namecpace

#endif