//---------------------------------------------------------------------------

#pragma hdrstop
#include <windows.h>
#define _TLMOBJ_PAINTER_INTERNAL
//#define TLMOBJ_PAINTER_IMAGE_NAMES
//#include <boost/string.hpp>

#ifndef BOOST_THREAD_USE_LIB
#define BOOST_THREAD_USE_LIB
#endif


#include <tlm_painter.h>
#include <kertl.hpp>
#include <kfname.hpp>
#include <fstream>
#include <iostream>
#include <stdarg.h>
#include <pictdlgs.h>
#include <freeimage.hpp>

#pragma argsused

//using namespace boost;
//using namespace boost::algorithm;

static Tpaint_variants   * d_paint_vars = NULL;//new Tpaint_variants;
static Tpaint_variants   * a_paint_vars = NULL;//new Tpaint_variants;
extern READY_COLORS        ready_colors;

DWORD  __fastcall d_paint_common      (LPTLMOBJ_PAINT_PARAM paint_param,BOOL get_extent,void * pp);
DWORD  __fastcall d_paint_lamp        (LPTLMOBJ_PAINT_PARAM paint_param,BOOL get_extent,void * pp);
DWORD  __fastcall d_paint_switch      (LPTLMOBJ_PAINT_PARAM paint_param,BOOL get_extent,void * pp);
DWORD  __fastcall d_paint_sname       (LPTLMOBJ_PAINT_PARAM paint_param,BOOL get_extent,void * pp);
DWORD  __fastcall a_paint_text        (LPTLMOBJ_PAINT_PARAM paint_param,BOOL get_exten ,void * pp);
DWORD  __fastcall a_paint_hbar        (LPTLMOBJ_PAINT_PARAM paint_param,BOOL get_extent,void * pp);
DWORD  __fastcall a_paint_vbar        (LPTLMOBJ_PAINT_PARAM paint_param,BOOL get_extent,void * pp);
DWORD  __fastcall a_paint_digits      (LPTLMOBJ_PAINT_PARAM paint_param,BOOL get_extent,void * pp);
DWORD  __fastcall a_paint_pie         (LPTLMOBJ_PAINT_PARAM paint_param,BOOL calc_size,void  * pp);
DWORD  __fastcall paint_as_ready      (LPTLMOBJ_PAINT_PARAM paint_param,BOOL calc_size,void  * pp);
//Tpaint_variant *
//       __fastcall create_paint_variant(std::string & var_name,std::string & var_text);
void   __fastcall delete_paint_variant(Tpaint_variant * pv);
DWORD  __fastcall do_paint_variant    (LPTLMOBJ_PAINT_PARAM paint_param,Tpaint_variant * pv);
HBITMAP __fastcall LoadImage(const char * img_name,int * _cx = NULL,int * _cy = NULL);
void init_ready_colors();
void __fastcall  init_discrete();


extern "C" void tss_cleanup_implemented(void)
{
 //stub only
}


//inline void  show_message(char * text,...)
//{
//   va_list  args;
//   va_start(args,text);
//   char message[4096];
//   vsnprintf(message,KERTL_ARRAY_COUNT(message),text,args);
//   MessageBoxA(NULL,message,"Debug",MB_OK);
//   va_end(args);
//}
//
//inline void  show_message(wchar_t * text,...)
//{
//   va_list  args;
//   va_start(args,text);
//   wchar_t message[4096];
//   vsnwprintf(message,KERTL_ARRAY_COUNT(message),text,args);
//   MessageBoxW(NULL,message,L"Debug",MB_OK);
//   va_end(args);
//}



tlm_paint_proc dpaint_table[4] =
                               {
                                d_paint_lamp,
                                d_paint_switch,
                                d_paint_switch,
                                d_paint_sname,
                                };

tlm_paint_proc apaint_table[5] =
                               {
                                a_paint_text,
                                a_paint_hbar,
                                a_paint_vbar,
                                a_paint_pie,
                                a_paint_digits
                                };

DWORD  __fastcall do_paint_variant    (LPTLMOBJ_PAINT_PARAM paint_param,Tpaint_variant * pv)
{
 if(paint_param && paint_param->dw_size == sizeof(*paint_param) && pv && pv->paint_proc)
    return pv->paint_proc(paint_param,FALSE,pv);
 return -1;
}

DWORD __fastcall do_paint(Tpaint_variants &vars,LPTLMOBJ_PAINT_PARAM pp,BOOL calc_size)
{
 if(vars.size())
 {
  int pvar = pp->paint_variant<vars.size() ? pp->paint_variant :  0;
  Tpaint_variant * pv = vars.at(pvar);
  if(pv->paint_proc)
    {
     pv->paint_proc(pp,calc_size,pv);
     return 0;
    }
 }
 return -1;
}

DWORD WINAPI tlmobj_paint(LPTLMOBJ_PAINT_PARAM paint_param)
{
 DWORD ret = 0;
 if(paint_param && !IsBadReadPtr(paint_param,sizeof(*paint_param)) && paint_param->dw_size == sizeof(*paint_param) && paint_param->paint_dc)
 {

  HGDIOBJ old_font = NULL;
  if(paint_param->font)
     old_font = SelectObject(paint_param->paint_dc,paint_param->font);
  if(paint_param->paint_variant == RDY_PAINT_AS_READY)
    {  paint_as_ready(paint_param,FALSE,NULL);
       return 0;
    }

  if(paint_param->record.state & MDBR_STATE_DISCRETE)
  {
     if(!d_paint_vars->size()) init_discrete();
     ret = do_paint(*d_paint_vars,paint_param,FALSE);

  }
  else
  {
   ret = do_paint(*a_paint_vars,paint_param,FALSE);
  }

  if(paint_param->paint_selected)
  {
   RECT r ;
   set_rect(r,paint_param->position,paint_param->size);
   HPEN pen = CreatePen(PS_SOLID,2,0);
       pen = (HPEN)SelectObject(paint_param->paint_dc,pen);
   int old_rop = SetROP2(paint_param->paint_dc,R2_NOTXORPEN);
//   Rectangle(paint_param->paint_dc,r.left,r.top,r.right,r.bottom);
//   POINT old_pt;
//   MoveToEx(paint_param->paint_dc,r.left,r.top,&old_pt);
   POINT sel_points[5];
   sel_points[0].x = r.right;
   sel_points[0].y = r.top;
   sel_points[1].x = r.right;
   sel_points[1].y = r.bottom;
   sel_points[2].x = r.left;
   sel_points[2].y = r.bottom;
   sel_points[3].x = r.left;
   sel_points[3].y = r.top;
   sel_points[4].x = r.right;
   sel_points[4].y = r.top;
   Polyline(paint_param->paint_dc,sel_points,5);
//   LineTo  (paint_param->paint_dc,r.right,r.top);
//   LineTo  (paint_param->paint_dc,r.right,r.bottom);
//   LineTo  (paint_param->paint_dc,r.left,r.bottom);
//   LineTo  (paint_param->paint_dc,r.left,r.top);
//   MoveToEx(paint_param->paint_dc,old_pt.x,old_pt.y,&old_pt);

   SetROP2(paint_param->paint_dc,old_rop);
   pen = (HPEN)SelectObject(paint_param->paint_dc,pen);
   DeleteObject(pen);
  }
//  if(old_font)
//     SelectObject(paint_param->paint_dc,old_font);
 }
 else
 --ret;
 return ret;
}

DWORD WINAPI tlmobj_get_extent(LPTLMOBJ_PAINT_PARAM paint_param)
{
 DWORD ret = 0;
 if(paint_param && !IsBadReadPtr(paint_param,sizeof(*paint_param)) && paint_param->dw_size == sizeof(*paint_param))
 {
  if(paint_param->paint_variant == RDY_PAINT_AS_READY)
    {
       paint_as_ready(paint_param,TRUE,NULL);
       return 0;
    }

  if(paint_param->record.state & MDBR_STATE_DISCRETE)
  {
     ret = do_paint(*d_paint_vars,paint_param,TRUE);
  }
  else
  {
    ret = do_paint(*a_paint_vars,paint_param,TRUE);
  }
 }
 else
 --ret;
 return ret;

}
#pragma warn -8057
DWORD __fastcall handle_paint(DWORD cmd,LPTLMOBJ_PAINT_PARAM pp,DWORD p2)
{
  DWORD ret = -1;
  __try{
  switch(cmd)
  {
   case PMCMD_PAINT     : ret = tlmobj_paint(pp);break;
   case PMCMD_CALC_SIZE : ret = tlmobj_get_extent(pp);break;

  }
   }
   __except(EXCEPTION_CONTINUE_SEARCH)
   {
   }
 return ret;
}


DWORD __fastcall handle_paint_1(DWORD cmd,LPTLMOBJ_PAINT_PARAM1 pp,DWORD p2)
{
 DWORD ret = -1;

 switch(cmd)
 {
   case PMCMD_PAINT     : ret = 0;break;
   case PMCMD_CALC_SIZE : ret = 0;break;
 }
 return ret;
}
#pragma warn .8057

HDC        MemoryDC      = 0;
HDC        temp_dc       = 0;
HBITMAP    bmp_sw_mask   = NULL;
HBITMAP    bmp_sw        = NULL;
HBITMAP    bmp_lamp_mask = NULL;
HBITMAP    bmp_lamp      = NULL;
HFONT      flags_font    = NULL;
HBRUSH     ready_text_bkbrush = NULL;
HBRUSH     ready_bkbrush      = NULL;

void __fastcall release_variants(Tpaint_variants & vars)
{
  Tpaint_variants::iterator ptr = vars.begin(),end = vars.end();
  while(ptr<end)
     {
      (*ptr)->release();
      delete *ptr;
      ++ptr;
     }
  vars.clear();
  vars.resize(0);
}


LRESULT __fastcall release_module()
{
   if(MemoryDC)
      {
       DeleteDC(MemoryDC);
       MemoryDC = NULL;
      }
   if(temp_dc)
    {
      DeleteDC( temp_dc );
      temp_dc = NULL;
    }
   if(bmp_lamp_mask)
   {
      DeleteObject(bmp_lamp_mask);
      bmp_lamp_mask = NULL;
   }

   if(bmp_lamp)
      {
       DeleteObject(bmp_lamp);
       bmp_lamp = NULL;
      }
   if(bmp_sw)
    {
     DeleteObject(bmp_sw);
     bmp_sw = NULL;
    }
   if(bmp_sw_mask)
   {
     DeleteObject(bmp_sw_mask);
     bmp_sw_mask = NULL;
   }

   if(flags_font)
     {
      DeleteObject(flags_font);
      flags_font = NULL;
     }

   if(ready_bkbrush)
   {
      DeleteObject(ready_bkbrush);
      ready_bkbrush = NULL;
   }

   if(ready_text_bkbrush)
   {
      DeleteObject(ready_text_bkbrush);
      ready_text_bkbrush = NULL;
   }


   if(d_paint_vars)
   {
    release_variants(*d_paint_vars);
    delete d_paint_vars;
    d_paint_vars = NULL;
   }
   if(a_paint_vars)
   {
    release_variants(*a_paint_vars);
    delete a_paint_vars;
    a_paint_vars = NULL;
   }
   return  GKH_RET_SUCCESS;
}

int __fastcall set_paint_variant_mask(Tpaint_variant * pv,char * file_name)
{
  if(pv->mask)
    {
     DeleteObject(pv->mask);
     pv->mask = NULL;
    }

  HBITMAP bmp = NULL;
  #ifdef TLMOBJ_PAINTER_IMAGE_NAMES
   pv->mask_file = file_name;
  #endif

  if(file_name && *file_name)
     bmp = LoadImage(file_name);
  if(bmp)
  {

   pv->mask = bmp;
   pv->paint_proc = d_paint_common;
   return 0;
  }
  return -1;
}

int __fastcall set_paint_variant_image(Tpaint_variant * pv,char * file_name)
{
  int cx = 0,cy = 0;
  if(pv->image)
     {
      DeleteObject(pv->image);
      pv->image = NULL;
     }


#ifdef TLMOBJ_PAINTER_IMAGE_NAMES
   pv->image_file = file_name;
#endif
  HBITMAP bmp = NULL;
  if(file_name && *file_name)
     bmp = LoadImage(file_name,&cx,&cy);
  if(bmp)
  {
   pv->image = bmp;
   pv->sz.cx = cx;
   pv->elements_count = 1;
   pv->sz.cy = cy;
   pv->paint_proc = d_paint_common;
   return 0;
  }
  return -1;
}

LRESULT __fastcall set_paint_variant_width(Tpaint_variant *pv,DWORD width)
{
  DWORD w            = pv->sz.cx*pv->elements_count;
  pv->sz.cx          = width;
  pv->elements_count = w/width;
  pv->paint_proc     = d_paint_common;
  return 0;
}


LRESULT __fastcall set_paint_variant_multipos(Tpaint_variant *pv,DWORD mp)
{
  pv->multi_position = mp;
  pv->paint_proc     = d_paint_common;
  return 0;
}



HBITMAP LoadBitmapFromResource(DWORD res_id)
{
 HBITMAP bmp = NULL;
 HBITMAP res_bmp = LoadBitmap(ModuleInstance,MAKEINTRESOURCE(res_id));
 if(res_bmp)
 {
    BITMAP bmp_info;
    GetObject(res_bmp,sizeof(bmp_info),&bmp_info);
    HDC ScreenDC = CreateDC("DISPLAY",0,0,0);
    bmp = CreateCompatibleBitmap(ScreenDC,bmp_info.bmWidth,bmp_info.bmHeight);
    HDC src_dc = CreateCompatibleDC(ScreenDC);
    HDC dst_dc = CreateCompatibleDC(ScreenDC);
    res_bmp = (HBITMAP) SelectObject(src_dc,res_bmp);
        bmp = (HBITMAP) SelectObject(dst_dc,bmp);

    BitBlt(dst_dc,0,0,bmp_info.bmWidth,bmp_info.bmHeight,src_dc,0,0,SRCCOPY);

    res_bmp = (HBITMAP) SelectObject(src_dc,res_bmp);
        bmp = (HBITMAP) SelectObject(dst_dc,bmp);
    DeleteDC(dst_dc);
    DeleteDC(src_dc);
    DeleteDC(ScreenDC);
    DeleteObject(res_bmp);
 }
 return  bmp;
}


void __fastcall  init_analogs()
{
   Tpaint_variant * p;
   BITMAP bmp_info;

   p = new Tpaint_variant(0,L"Текстом",0,0);
   p->paint_proc =a_paint_text;
   a_paint_vars->push_back(p);

   p = new Tpaint_variant(0,L"H-Bar",0,0);
   p->paint_proc =a_paint_hbar;
   a_paint_vars->push_back(p);

   p = new Tpaint_variant(0,L"V-Bar",0,0);
   p->paint_proc =a_paint_vbar;
   a_paint_vars->push_back(p);

   p = new Tpaint_variant(0,L"LED",0,0);
   p->paint_proc = a_paint_digits;
   p->mask       = LoadBitmapFromResource(IDBMP_LED_DIGITS_MASK);
   p->image      = LoadBitmapFromResource(IDBMP_LED_DIGITS);
   GetObject(p->image,sizeof(bmp_info),&bmp_info);
   p->elements_count = 13;
   p->sz.cx =  bmp_info.bmWidth/p->elements_count;
   p->sz.cy =  bmp_info.bmHeight;
   a_paint_vars->push_back(p);

   p = new Tpaint_variant(0,L"PIE",0,0);
   p->paint_proc =a_paint_pie;
   a_paint_vars->push_back(p);


}


void __fastcall  init_discrete()
{
  Tpaint_variant * p;
  BITMAP bmp_info;

  p = new Tpaint_variant(0,L"Коротким именем",0,0);
  p->paint_proc = d_paint_sname;
  d_paint_vars->push_back(p);


  p = new Tpaint_variant(0,L"Лампочкой",16,16);
  p->paint_proc = d_paint_common;
//  p->image      = LoadBitmap(ModuleInstance,MAKEINTRESOURCE(IDBMP_DISCRETE_LAMP));
//  p->mask       = LoadBitmap(ModuleInstance,MAKEINTRESOURCE(IDBMP_DISCRETE_LAMP_MASK));
  p->image      = LoadBitmapFromResource(IDBMP_DISCRETE_LAMP);
  p->mask       = LoadBitmapFromResource(IDBMP_DISCRETE_LAMP_MASK);

  GetObject(p->image,sizeof(bmp_info),&bmp_info);
  p->sz.cx = p->sz.cy =  bmp_info.bmHeight;
  p->elements_count   =  bmp_info.bmWidth/p->sz.cx;
  p->multi_position   = 1;
  d_paint_vars->push_back(p);

  p = new Tpaint_variant(0,L"Квадратом",16,16);
  p->paint_proc = d_paint_common;
  p->image      = LoadBitmapFromResource(IDBMP_DISCRETE_SQUARE);
  p->mask       = LoadBitmapFromResource(IDBMP_DISCRETE_SQUARE_MASK);
  GetObject(p->image,sizeof(bmp_info),&bmp_info);
  p->sz.cx = p->sz.cy =  bmp_info.bmHeight;
  p->elements_count   =  bmp_info.bmWidth/p->sz.cx;
  p->multi_position = 1;
  d_paint_vars->push_back(p);


  p = new Tpaint_variant(0,L"SW Горизонтальный",16,16);
  p->paint_proc = d_paint_common;
  p->image      = LoadBitmapFromResource(IDBMP_HSWITCH);
  p->mask       = LoadBitmapFromResource(IDBMP_HSWITCH_MASK);
  GetObject(p->image,sizeof(bmp_info),&bmp_info);
  p->elements_count = 18;
  p->sz.cx =  bmp_info.bmWidth/p->elements_count;
  p->sz.cy =  bmp_info.bmHeight;
  p->multi_position = 2;
  d_paint_vars->push_back(p);

  p = new Tpaint_variant(0,L"SW Вертикальный",16,16);
  p->paint_proc = d_paint_common;
  p->image      = LoadBitmapFromResource(IDBMP_VSWITCH);
  p->mask       = LoadBitmapFromResource(IDBMP_VSWITCH_MASK);
  GetObject(p->image,sizeof(bmp_info),&bmp_info);
  p->elements_count = 18;
  p->sz.cx =  bmp_info.bmWidth/p->elements_count;
  p->sz.cy =  bmp_info.bmHeight;
  p->multi_position = 2;
  d_paint_vars->push_back(p);
}

void __fastcall  init_default()
{
    init_discrete();
    init_analogs ();
}

HBITMAP __fastcall LoadImage(const char * img_name,int * _cx,int * _cy)
{
  HBITMAP bmp = NULL;
  bmp = freeimage::TFreeImage::load_image(img_name);
  if(bmp && (_cx || _cy))
     {
      BITMAP bm;
      bzero(&bm,sizeof(bm));
      GetObject(bmp,sizeof(bm),&bm);
      if(_cx) *_cx = bm.bmWidth;
      if(_cy) *_cy = bm.bmHeight;
     }
//  LPVOID hDib = LoadImageFromFile((char*)img_name);
//  if(hDib)
//  {
//     LPBITMAPINFO bmi = (LPBITMAPINFO)::GlobalLock(hDib);
//     if(bmi)
//     {
//     int cx = bmi->bmiHeader.biWidth;
//     int cy = bmi->bmiHeader.biHeight;
//     ::GlobalUnlock(hDib);
//     if(_cx) * _cx = cx;
//     if(_cy) * _cy = cy;
//     bmp = DibToDdb(hDib,cx,cy,true);
//     }
//     else
//     FreeImage(hDib);
//  }
  return bmp;
}


//Tpaint_variant * __fastcall create_paint_variant(std::string & var_name,std::string & var_text)
//{
//       Tpaint_variant * pv = NULL;
//       std::vector<std::string> params;
//       boost::algorithm::split(params,var_text,boost::algorithm::is_any_of(", "));
//       if(params.size() >0 )
//       {
//        pv = new Tpaint_variant(var_name,0,0);
//        pv->paint_proc = d_paint_common;
//        BITMAP bi;
//        ZeroMemory(&bi,sizeof(bi));
//
//        for(DWORD i = 0;i<params.size();i++)
//        {
//         std::string &s = params.at(i);
//         switch(i)
//         {
//          case 0: //Изображение
//          if(!s.empty())
//          {
//          #ifdef TLMOBJ_PAINTER_IMAGE_NAMES
//          pv->image_file = s;
//          #endif
//          pv->image = (HBITMAP) LoadImage((char*)s.c_str());
//          if(pv->image)
//            {
//             GetObject(pv->image,sizeof(bi),&bi);
//             pv->sz.cx = bi.bmWidth;
//             pv->sz.cy = bi.bmHeight;
//             pv->elements_count = 1;
//
//            }
//          }
//          break;
//          case 1:
//          if(!s.empty())
//           {
//            #ifdef TLMOBJ_PAINTER_IMAGE_NAMES
//            pv->mask_file = s;
//            #endif
//            pv->mask = (HBITMAP) LoadImage((char*)s.c_str());
//           }
//          break;
//          case 2: if(s.length())
//          {
//           pv->sz.cy = atoi(s.c_str());
//           pv->elements_count = pv->sz.cx/pv->sz.cy;pv->sz.cx=pv->sz.cy;
//          }
//          break;
//          case 3: if(s.length())
//          {
//           pv->sz.cy = atoi(s.c_str());
//          } break;
//          case 4:
//          pv->switched = atoi(s.c_str());
//          break;
//         }
//        }
//       }
//
//   return pv;
//}

void __fastcall delete_paint_variant(Tpaint_variant * pv)
{
  if(pv)
     {
      pv->release();
      delete pv;
     }

}

//void __fastcall init_from_file(wchar_t* fname,std::ostream & log)
//{
// std::ifstream conf(fname);
// show_message(L"init from file %s",fname.c_str());
// if(conf.is_open())
// {
//  Tpaint_variant * pv;
//
//  while(!conf.eof())
//  {
//      /*
//       Разбирается строка вида
//       Лампочкой=image_bmp,mask_bmp,16,32,0
//       Переключателем=image_bmp,,16,1
//      */
//
//      char text[1024];
//      conf.getline((char*)text,KERTL_ARRAY_COUNT(text));
//      std::vector<std::string> strings,params;
//      std::string conf_line(text);
//      boost::algorithm::trim( conf_line );
//     if(*conf_line.c_str()!='#' && conf_line.length())
//     {
//      boost::algorithm::split(strings,conf_line,boost::algorithm::is_any_of("="));
//      if(strings.size()>1)
//      {
//       pv = create_paint_variant(strings.at(0),strings.at(1));
//        if(pv && pv->image && pv->sz.cx && pv->sz.cy && pv->elements_count)
//           {
//            d_paint_vars->push_back(pv);
//           }else
//           {
//            log<<"Error init variant for \""<<pv->name<<std::endl;
//            pv->release(); delete pv;
//           }
//
//      }
//     }
//  }
//  if(d_paint_vars->size())
//  {
//     pv = new Tpaint_variant("Коротким именем",0,0);
//     pv->paint_proc = d_paint_sname;
//     d_paint_vars->insert(d_paint_vars->begin(),pv);
//  }
//  init_analogs();
// }
 //else
 //show_message(L"Config file not open!");
//}


bool  __fastcall init_module()
{
 init_ready_colors();
 if(!d_paint_vars)
    d_paint_vars = new Tpaint_variants;
 if(!a_paint_vars)
     a_paint_vars = new Tpaint_variants;
if(!d_paint_vars->size() && !a_paint_vars->size())
 {

//   wchar_t dll_name[MAX_PATH<<2];
//   wchar_t log_name[MAX_PATH];
//   wchar_t conf_name[MAX_PATH];
//   GetModuleFileNameW(ModuleInstance,dll_name,KERTL_ARRAY_COUNT(dll_name));
//   KeRTL::TKFName<wchar_t> fname;
//   fname.set_fullname(dll_name);
//   snwprintf(log_name,KERTL_ARRAY_COUNT(log_name),L"%s.log",fname.get_name());
//   snwprintf(conf_name,KERTL_ARRAY_COUNT(conf_name),L"%s.conf",fname.get_name());
//   ofstream log(log_name,std::ios::trunc);
//   gregorian::date dt = date_time::day_clock<gregorian::date>::local_day() ;
//
//   log<<gregorian::to_simple_string(dt)<<" Init tlmobj_std_painter"<<std::endl;
//
//   if(KeRTL::is_file_exist(fname()))
//   {
//     init_from_file(conf_name,log);
//     if(!d_paint_vars->size())
//        {
//         log<<"No discrete paint variants. Use defaults init"<<std::endl;
//         init_discrete();
//        }
//   }
//   else
   {
    //show_message(L"No config file %s",fname());
    init_default();
   }

 }
 return true;
}

LRESULT __fastcall enum_variants(Tpaint_variants & vars,char * name,int idx)
{
 if(idx>=(int)vars.size())
    return GKH_RET_ERROR;
    Tpaint_variant * pv = vars.at(idx);
    strcpy(name,pv->name.c_str());
 return  GKH_RET_SUCCESS;
}

LRESULT __fastcall enum_discrete_variants(char * name,int idx)
{
  if(!d_paint_vars->size())
      init_discrete();
  return enum_variants(*d_paint_vars,name,idx);
}

LRESULT __fastcall enum_analog_variants(char * name,int idx)
{
  return enum_variants(*a_paint_vars,name,idx);
}

LRESULT __fastcall enum_discrete_variants_ex ()
{
  LRESULT ret = GKH_RET_ERROR;
  return ret;
}

DWORD __fastcall d_paint_common (LPTLMOBJ_PAINT_PARAM paint_param,BOOL get_extent,void * pp)
{
  if(!d_paint_vars->size())
      init_discrete();
  Tpaint_variant &pv = *(Tpaint_variant*)pp;
  if(!pv.image)
     return d_paint_sname(paint_param,get_extent,pp);
    return d_paint_lamp(paint_param,get_extent,pp);
}

LRESULT WINAPI module_main(DWORD cmd,LPARAM p1,LPARAM p2)
{
  LRESULT ret = -1;
  if(cmd==GKME_CMD_UNLOAD_MODULE || init_module())
  {
  switch(cmd)
  {
     case GKME_CMD_UNLOAD_MODULE       : release_module();  ret = TRUE;break;
     case PMCMD_ENUM_DISCRETE_VARIANTS : ret = enum_discrete_variants  ((char*)p1,(int)p2);break;
     case PMCMD_ENUM_ANALOG_VARIANTS   : ret = enum_analog_variants((char*)p1,(int)p2);break;
     //case PMCMD_CREATE_PAINT_VARIANT   : ret = (LRESULT) create_paint_variant(*&std::string((char *)p1),*&std::string((char*)p2));break;
     case PMCMD_DELETE_PAINT_VARIANT   : delete_paint_variant((Tpaint_variant*)p1);ret = 0;break;
     case PMCMD_DO_PAINT_VARIANT       : ret = do_paint_variant((LPTLMOBJ_PAINT_PARAM)p1,(Tpaint_variant*)p2);break;
     case PMCMD_CREATE_PAINT_VARIANT_EMPTY :
     {
      Tpaint_variant * pv = new Tpaint_variant(0);
      pv->paint_proc = d_paint_common;
      ret = (LRESULT) pv;
     }break;

     case PMCMD_PAINT_VAR_CTRL_SET_IMAGE    : ret = set_paint_variant_image   ((Tpaint_variant*)p1,(char *)p2);break;
     case PMCMD_PAINT_VAR_CTRL_SET_MASK     : ret = set_paint_variant_mask    ((Tpaint_variant*)p1,(char *)p2);break;
     case PMCMD_PAINT_VAR_CTRL_SET_WIDTH    : ret = set_paint_variant_width   ((Tpaint_variant*)p1,(DWORD)p2);break;
     case PMCMD_PAINT_VAR_CTRL_SET_MULTIPOS : ret = set_paint_variant_multipos((Tpaint_variant*)p1,(DWORD)p2);break;
     case PMCMD_PAINT_VAR_CTRL_SET_SWITCHED :
     { Tpaint_variant* pv = (Tpaint_variant*)p1 ;
       pv->switched = (DWORD)p2;
       pv->paint_proc = d_paint_common;
       ret = GKH_RET_SUCCESS;
     } break;
     case PMCMD_PAINT_VAR_CTRL_SET_HEIGHT   :
     {
        Tpaint_variant* pv = (Tpaint_variant*)p1;
        pv->paint_proc = d_paint_common;
        pv->sz.cy = (DWORD)p2;
     }break;
     case PMCMD_PAINT_VAR_ADD               :
     {
      Tpaint_variant* pv = (Tpaint_variant*)p1;
      pv->name = (char *)p2;
      pv->paint_proc = d_paint_common;
      ret = d_paint_vars->size();
            d_paint_vars->push_back(pv);

     }
     break;
     case PMCMD_PAINT_VAR_CLEAR:
      ret = GKH_RET_SUCCESS;
      release_variants(*d_paint_vars);
     break;
     case PMCMD_PAINT_VAR_GET:
     if(DWORD(p1)<d_paint_vars->size())
       ret = (LRESULT)d_paint_vars->at(p1);
     else
     ret = 0;
     break;
     case PMCMD_PAINT_SET_READY_COLORS:
     {
          LPREADY_COLORS rc = (LPREADY_COLORS) p1;
          if(rc && rc->dw_size == sizeof(*rc))
             {
              memcpy(&ready_colors,rc,sizeof(*rc));
              ret = 0;
             }
             else
              ret = -1;

     }
     break;
     case PMCMD_PAINT_GET_READY_COLORS:
     {
          LPREADY_COLORS rc = (LPREADY_COLORS) p1;
          if(rc && rc->dw_size == sizeof(*rc))
            {
             memcpy(rc,&ready_colors,sizeof(*rc));
             ret = 0;
             }
             else
             ret = -1;
     }
     break;
     default:
     {
     LPDWORD  dw_size = (LPDWORD)p1;
      if(dw_size && !IsBadReadPtr(dw_size,sizeof(*dw_size)))
      {
        switch(*dw_size)
        {
         case sizeof(TLMOBJ_PAINT_PARAM):
              ret = handle_paint(cmd,(LPTLMOBJ_PAINT_PARAM)p1,p2);
              break;
         case sizeof(TLMOBJ_PAINT_PARAM1):
              ret = handle_paint_1(cmd,(LPTLMOBJ_PAINT_PARAM1)p1,p2);
              break;
        }
       }
      }
   }
  }
 return ret;
}



HBITMAP WINAPI create_bitmap(DWORD width,DWORD height)
 {
    HDC ScreenDC = CreateDC("DISPLAY",0,0,0);
    HBITMAP bitmap = CreateCompatibleBitmap(ScreenDC,width,height);
    DeleteDC(ScreenDC);
    return bitmap;
 }



 HFONT __fastcall create_flags_font(HDC dc)
 {

   if(!flags_font)
   {

    flags_font         =  (HFONT)GetCurrentObject(dc,OBJ_FONT);
    LOGFONT lf;
    GetObject(flags_font,sizeof(lf),&lf);
    lf.lfHeight = -11;
    lf.lfUnderline = false;
    lf.lfStrikeOut = false;
    lf.lfItalic    = false;
    //flags_font = CreateFontIndirect(&lf);
    flags_font = (HFONT)GetStockObject(DEFAULT_GUI_FONT);

   }
   return flags_font;
 }

 READY_COLORS ready_colors = {0};

 void init_ready_colors()
 {
    if(!ready_colors.dw_size)
    {
     ready_colors.dw_size         = sizeof(ready_colors);
     ready_colors.bkgnd           = 0x00C0C0C0;
     ready_colors.bkgnd_stable    = 0x00FFFFFF;
     ready_colors.tutr            = 0x00F21616;
     ready_colors.state           = 0x0015ADB0;
     ready_colors.state_danger    = 0x000080FF;
     ready_colors.state_alarm     = 0x00342AF5;
     ready_colors.name_bad        = 0x00808080;
     ready_colors.name_good       = 0x0000FF00;
     ready_colors.border          = 0;
     ready_colors.border_selected = 0x00FF0000;

    }
 }

 DWORD __fastcall paint_as_ready (LPTLMOBJ_PAINT_PARAM paint_param,BOOL calc_size,void * /*pv*/)
 {                        //01234567
   HFONT font =  create_flags_font(paint_param->paint_dc);
   wchar_t *state_chars = L"ИАПНМСУX";   //История Авария Несоответствие Много Возврат Управление ОшибкаТУ\ТР
   wchar_t * const_str = L"W";
   if(calc_size || !paint_param->size.cx ||  !paint_param->size.cy)
   {

    SIZE text_size;
    if(!paint_param->short_name)
        paint_param->short_name = const_str;
       GetTextExtentPoint32W(paint_param->paint_dc,paint_param->short_name,wcslen(paint_param->short_name),&text_size);
    font = (HFONT) SelectObject(paint_param->paint_dc,font);
    SIZE state_size;
    GetTextExtentPoint32W(paint_param->paint_dc,const_str,1,&state_size);
    state_size.cx*=wcslen(state_chars);
    //state_size.cx+=16;
    paint_param->size.cx = std::max(state_size.cx,text_size.cx);
    paint_param->size.cy = std::max(paint_param->size.cy,state_size.cy+text_size.cy+8);
    font = (HFONT) SelectObject(paint_param->paint_dc,font);
   }

   if(!calc_size)
   {
   RECT r;
   set_rect(r,paint_param->position,paint_param->size);
   DWORD  bk_color       = (paint_param->record.dw_value&RDY_FLAG_STABLE) ? ready_colors.bkgnd_stable : ready_colors.bkgnd;
   DWORD  ready_flags = paint_param->record.dw_value;
   DWORD pen_color;
   if((ready_flags&RDY_FLAG_SELECTED))
      pen_color = ready_colors.border_selected;
      else
      pen_color = ready_colors.border;

   HBRUSH bk_brush  = CreateSolidBrush(bk_color);
   HPEN   bord_pen  = CreatePen(PS_SOLID,2,pen_color);

   bord_pen = (HPEN  )SelectObject(paint_param->paint_dc,bord_pen);
   bk_brush = (HBRUSH)SelectObject(paint_param->paint_dc,bk_brush);

   RoundRect(paint_param->paint_dc,r.left,r.top,r.right,r.bottom,10,12);

   bk_brush = (HBRUSH)SelectObject(paint_param->paint_dc,bk_brush);
   bord_pen = (HPEN)SelectObject(paint_param->paint_dc,bord_pen);

   DeleteObject(bord_pen);
   DeleteObject(bk_brush);


  if(paint_param->erase!=TLMOBJ_PAINT_FLAG_ERASE)
  {
   set_rect(r,paint_param->position,paint_param->size);
   grow_rect(r,-2,-2);
   SIZE text_size;
   DWORD old_color    = SetTextColor(paint_param->paint_dc,paint_param->record.diag ? ready_colors.name_bad : ready_colors.name_good);

   int old_mode    = SetBkMode(paint_param->paint_dc,TRANSPARENT);
   GetTextExtentPoint32W(paint_param->paint_dc,paint_param->short_name,lstrlenW(paint_param->short_name),&text_size);
   if(!(paint_param->erase == TLMOBJ_PAINT_FLAG_ERASE_FLASH && (paint_param->record.dw_value&RDY_FLAG_FLASH_NAME)))
   {
    //Рисуем имя
//    if(paint_param->record.dw_value&RDY_FLAG_NAME_BLACK_BKGND)
//    {
//     if(!ready_text_bkbrush ) ready_text_bkbrush = CreateSolidBrush(0);
//     RECT text_rect;
//     set_rect(text_rect,r.left+4,r.top+3,r.right-r.left-6,text_size.cy+2);
//     FillRect(paint_param->paint_dc,&text_rect,ready_text_bkbrush);
//    }
//    else
//     paint_param->record.dw_value = paint_param->record.dw_value;
    ExtTextOutW(paint_param->paint_dc,r.left+(paint_param->size.cx - text_size.cx)/2 ,r.top+2,0,&r,paint_param->short_name,lstrlenW(paint_param->short_name),NULL);
   }
   else
   old_mode = old_mode;



   /*Теперь рисуем флаги*/
   POINT state_pos;
   state_pos.x  = r.left+2;
   state_pos.y  = r.top+2+text_size.cy+4;

   font = (HFONT)SelectObject(paint_param->paint_dc,font);

   GetTextExtentPoint32W(paint_param->paint_dc,const_str,1,&text_size);
   /*Отрисовать  как готовность*/

   //Наличие архивов
   if(ready_flags&(RDY_FLAG_ALARM_ARCHIVE|RDY_FLAG_ARCHIVE))
      {
       SetTextColor(paint_param->paint_dc,(ready_flags&RDY_FLAG_ALARM_ARCHIVE) ? ready_colors.state_alarm : ready_colors.state);
       ExtTextOutW (paint_param->paint_dc,state_pos.x,state_pos.y,0,&r,state_chars+0,1,NULL);
      }
    state_pos.x += text_size.cx;
    //Аварийный процесс

    if(ready_flags&RDY_FLAG_ALARM)
    {
     SetTextColor(paint_param->paint_dc,ready_colors.state_alarm);
     ExtTextOutW (paint_param->paint_dc,state_pos.x,state_pos.y,0,&r,state_chars+1,1,NULL);
    }
    state_pos.x += text_size.cx;

    //Предупредительное
    if(ready_flags&RDY_FLAG_DANGER)
    {
     SetTextColor(paint_param->paint_dc,ready_colors.state);
     ExtTextOutW (paint_param->paint_dc,state_pos.x,state_pos.y,0,&r,state_chars+2,1,NULL);
    }
    state_pos.x += text_size.cx;

    //В несоответвии
    if( (ready_flags&RDY_FLAG_NOTNORMAL) && !(paint_param->erase == TLMOBJ_PAINT_FLAG_ERASE_FLASH && (paint_param->record.dw_value&RDY_FLAG_NOTNORMAL_FLASH)))
    {
     SetTextColor(paint_param->paint_dc,(ready_flags&RDY_FLAG_DANGER_NOTNORMAL) ? ready_colors.state_alarm : ready_colors.state);
     ExtTextOutW (paint_param->paint_dc,state_pos.x,state_pos.y,0,&r,state_chars+3,1,NULL);
    }
    state_pos.x += text_size.cx;

    if((ready_flags&RDY_FLAG_MANY) && !(paint_param->erase == TLMOBJ_PAINT_FLAG_ERASE_FLASH && (paint_param->record.dw_value&RDY_FLAG_MANY_FLASH)))
    {
     SetTextColor(paint_param->paint_dc,(ready_flags&RDY_FLAG_DANGER_MANY) ? ready_colors.state_alarm :ready_colors.state);
     ExtTextOutW (paint_param->paint_dc,state_pos.x,state_pos.y,0,&r,state_chars+4,1,NULL);
    }
    state_pos.x += text_size.cx;

    if((ready_flags&RDY_FLAG_BACKNORMAL) && !(paint_param->erase == TLMOBJ_PAINT_FLAG_ERASE_FLASH && (paint_param->record.dw_value&RDY_FLAG_BACKNORMAL_FLASH)))
    {
     SetTextColor(paint_param->paint_dc,(ready_flags&RDY_FLAG_DANGER_BACKNORMAL) ? ready_colors.state_alarm : ready_colors.state);
     ExtTextOutW (paint_param->paint_dc,state_pos.x,state_pos.y,0,&r,state_chars+5,1,NULL);
    }
    state_pos.x += text_size.cx;

    if(ready_flags&(RDY_FLAG_TUTR_PREPARE|RDY_FLAG_TUTR_ACTIVE))
    {
      SetTextColor(paint_param->paint_dc,ready_colors.tutr);
      if(!(paint_param->erase == TLMOBJ_PAINT_FLAG_ERASE_FLASH && (paint_param->record.dw_value&RDY_FLAG_TUTR_ACTIVE)) )
         ExtTextOutW (paint_param->paint_dc,state_pos.x,state_pos.y,0,&r,state_chars+6,1,NULL);
    }
    else
    {
     if(ready_flags&(RDY_FLAG_TUTR_ERROR))
     {
      SetTextColor(paint_param->paint_dc,ready_colors.state_alarm);
      ExtTextOutW (paint_param->paint_dc,state_pos.x,state_pos.y,0,&r,state_chars+7,1,NULL);
     }
    }
    state_pos.x += text_size.cx;

   SetTextColor(paint_param->paint_dc,old_color);
   SetBkMode(paint_param->paint_dc,old_mode);
   font = (HFONT)SelectObject(paint_param->paint_dc,font);
  }
  }
  return 0;
 }




