#ifndef _TLM_PAINTER_INC
#define _TLM_PAINTER_INC


#define IDBMP_DISCRETE_LAMP        1000
#define IDBMP_DISCRETE_LAMP_MASK   1001

#define IDBMP_DISCRETE_SQUARE      1002
#define IDBMP_DISCRETE_SQUARE_MASK 1003

#define IDBMP_VSWITCH              1004
#define IDBMP_VSWITCH_MASK         1005

#define IDBMP_HSWITCH              1006
#define IDBMP_HSWITCH_MASK         1007


#define IDBMP_DIGIT_0              1100
#define IDBMP_DIGIT_1              1101
#define IDBMP_DIGIT_2              1102
#define IDBMP_DIGIT_3              1103
#define IDBMP_DIGIT_4              1104
#define IDBMP_DIGIT_5              1105
#define IDBMP_DIGIT_6              1106
#define IDBMP_DIGIT_7              1107
#define IDBMP_DIGIT_8              1108
#define IDBMP_DIGIT_9              1109
#define IDBMP_DIGIT_COMMA_0        1200
#define IDBMP_DIGIT_COMMA_1        1201
#define IDBMP_DIGIT_COMMA_2        1202
#define IDBMP_DIGIT_COMMA_3        1203
#define IDBMP_DIGIT_COMMA_4        1204
#define IDBMP_DIGIT_COMMA_5        1205
#define IDBMP_DIGIT_COMMA_6        1206
#define IDBMP_DIGIT_COMMA_7        1207
#define IDBMP_DIGIT_COMMA_8        1208
#define IDBMP_DIGIT_COMMA_9        1209

#define IDBMP_LED_DIGITS_MASK      1500
#define IDBMP_LED_DIGITS           1501

#define IDFNT_LED_FONT             1600

#ifndef RC_INVOKED

#include <mediumdb.h>

#define  PMCMD_PAINT                         100
#define  PMCMD_CALC_SIZE                     101

//Клонирование варианта
#define  PMCMD_COPY_PAINT_VARIANT            104
//Создание варианта отрисовки
//p1 = char * var_name, p2 = char * var_text
#define  PMCMD_CREATE_PAINT_VARIANT          105


//Удаление варианта отрисовки
//p1 = (LPARAM)(Tpaint_variant *)
#define  PMCMD_DELETE_PAINT_VARIANT          106

//Отрисовка варианта
//p1 =(LPTLMOBJ_PAINT_PARAM), p2 = (Tpaint_variant*)
#define  PMCMD_DO_PAINT_VARIANT              107

#define  PMCMD_CREATE_PAINT_VARIANT_EMPTY    108

#define  PMCMD_PAINT_VAR_CTRL_SET_IMAGE      110
#define  PMCMD_PAINT_VAR_CTRL_SET_MASK       111
#define  PMCMD_PAINT_VAR_CTRL_SET_SWITCHED   112
#define  PMCMD_PAINT_VAR_CTRL_SET_WIDTH      113
#define  PMCMD_PAINT_VAR_CTRL_SET_HEIGHT     114

//Добавить
#define  PMCMD_PAINT_VAR_ADD                 115
#define  PMCMD_PAINT_VAR_GET                 116

#define  PMCMD_PAINT_SET_READY_COLORS        117
#define  PMCMD_PAINT_GET_READY_COLORS        118

//Удвление из списка дискретных
//p1 - paint_context , p2 - idx
#define  PMCMD_PAINT_VAR_REMOVE              119

#define PMCMD_PAINT_VAR_CLEAR                120

//Перечисление вариантов отрисовки готовностей
#define PMCMD_PAINT_VAR_CTRL_SET_MULTIPOS    122

//#define PMCMD_PAINT_CTRL_SET_QUESTION_SIGN   123

#define  PMCMD_PAINT_VAR_CTRL_GET_WIDTH      130
#define  PMCMD_PAINT_VAR_CTRL_GET_HEIGHT     131
#define  PMCMD_PAINT_VAR_CTRL_GET_ELEMENTS   132
#define  PMCMD_PAINT_VAR_CTRL_GET_MULTIPOS   133


#define  PMCMD_PAINT_VAR_CTRL_SET_IMAGE_W    210
#define  PMCMD_PAINT_VAR_CTRL_SET_MASK_W     211

#define  PMCMD_PAINT_VAR_CTRL_GET_IMAGE      212
#define  PMCMD_PAINT_VAR_CTRL_GET_MASK       213
#define  PMCMD_PAINT_VAR_CTRL_GET_IMAGE_NAME 214
#define  PMCMD_PAINT_VAR_CTRL_GET_MASK_NAME  215

//Создание контекста отрисовки   возврат  id контекста
#define  PMCMD_PAINT_CONTEXT_CREATE          250
//Удаление  контекста отрисовки
#define  PMCMD_PAINT_CONTEXT_DELETE          251

//Копирование контекстов рисования
#define  PMCMD_PAINT_CONTEXT_COPY_ALL        256
#define  PMCMD_PAINT_CONTEXT_COPY_DISCRETE   257
#define  PMCMD_PAINT_CONTEXT_COPY_ANALOGS    258
#define  PMCMD_PAINT_CONTEXT_COPY_READY      259

#define  PAINT_CONTEXT_DISCRETE_MASK         1
#define  PAINT_CONTEXT_ANALOG_MASK           2
#define  PAINT_CONTEXT_READY_MASK            4


//Перечисление дисретных вариантов
#define  PMCMD_ENUM_DISCRETE_VARIANTS        300
//Перечисление дисретных вариантов расширенная версия учетом контекста отрисовки
#define  PMCMD_ENUM_DISCRETE_VARIANTS_EX     301

//return GKH_RET_ERROR  end of list
#define  PMCMD_ENUM_ANALOG_VARIANTS          302
//Перечисление аналоговых вариантов расширная с учетом контекста отрисовки
//return GKH_RET_ERROR  end of list
#define  PMCMD_ENUM_ANALOG_VARIANTS_EX       303
//Перечисление Готовнотей
#define PMCMD_ENUM_READY_VARIANTS            304
//Перечисление Готовнотей вариантов расширная с учетом контекста отрисовки
#define PMCMD_ENUM_READY_VARIANTS_EX         305

#pragma pack(push,1)

struct ENUM_EX_VARSIANTS_DATA_A
{
  DWORD cbsz;
  DWORD paint_context;
  DWORD idx;
  DWORD text_sz;
  char  __dummy;
  char  *text;
};
typedef ENUM_EX_VARSIANTS_DATA_A* LPENUM_EX_VARSIANTS_DATA_A;

struct ENUM_EX_VARSIANTS_DATA_W
{
  DWORD cbsz;
  DWORD paint_context;
  DWORD idx;
  DWORD text_sz;
  wchar_t __dummy;
  wchar_t  *text;
};

typedef ENUM_EX_VARSIANTS_DATA_W* LPENUM_EX_VARSIANTS_DATA_W;

#pragma pack(pop)


/*Получение варианта отрисовки  p1 = index*/




#define PAINT_DISCRETE_AS_SHORTNAME 0
#define PAINT_DISCRETE_AS_LAMP      1
#define PAINT_DISCRETE_AS_HSWITCH   2
#define PAINT_DISCRETE_AS_VSWITCH   3

#define PAINT_ANALOG_AS_TEXT        0
#define PAINT_ANALOG_AS_HBAR        1
#define PAINT_ANALOG_AS_VBAR        2
#define PAINT_ANALOG_AS_PIE         3

#define PAINT_OPT_FLASHED                 0x80000000
#define PAINT_OPT_SELECTED                0x40000000

#define PAINT_OPT_ADD_RIGHT_SHORTNAME     0x00000001
#define PAINT_OPT_ADD_LEFT_SHORTNAME      0x00000002
#define PAINT_OPT_ADD_TOP_SHORTNAME       0x00000004
#define PAINT_OPT_ADD_BOTTOM_SHORTNAME    0x00000008
#define PAINT_OPT_ADD_SHORTNAME_MASK      0x0000000F



inline void         __fastcall set_rect(RECT & r,const LONG x,const LONG y,const LONG w,const LONG h)
{
  r.left = r.right  = x;
  r.top  = r.bottom = y;
  r.right  +=w;
  r.bottom +=h;
}

inline void         __fastcall set_rect(RECT & r,const MDB_GPOINT & pt,const MDB_GSIZE & sz)
 {
  set_rect(r,pt.x,pt.y,sz.cx,sz.cy);
 }

inline void         __fastcall set_rect(RECT & r,const POINT & pt,const SIZE & sz)
 {
  set_rect(r,pt.x,pt.y,sz.cx,sz.cy);
 }

inline void         __fastcall set_rect(RECT & r,const POINT & lt,const POINT & br)
 {
  int x1 = lt.x;
  int x2 = br.x;
  int y1 = lt.y;
  int y2 = br.y;
  if(x2<x1) std::swap(x1,x2);
  if(y2<y1) std::swap(y2,y1);
  set_rect(r,x1,y1,x2-x1,y2-y1);
 }



 inline LONG __fastcall rect_width(const RECT & r)
 {
    return r.right - r.left;
 }

 inline LONG  __fastcall rect_height(const RECT & r)
 {
    return r.bottom - r.top;
 }



#pragma pack(push,4)

typedef struct TLMOBJ_PAINT_PARAM
{
  DWORD                 dw_size;
  HDC                   paint_dc;
  MDB_GPOINT            position;
  MDB_GSIZE             size;
  DWORD                 bkgnd_color;   //Цвет фона
  DWORD                 color;         //Цвет текста
  HFONT                 font;          //Шрифт если не 0
  BOOL                  paint_selected;/*Признак отрисовки выбранного*/
  int                   erase;
  HBITMAP               bkgnd_bitmap;  /*Фоновое изображение*/

  MDB_RECORD            record;

  DWORD                 precision;     // Знаков после запятой
  DWORD                 paint_variant; // Вариант отрисовки
  DWORD                 paint_options; // Опции отрисовки

  DWORD                 changed;      /* Скока раз ТС переключалась*/
  DWORD                 normal_value; /* Нормальное положение для ТС*/
  const wchar_t*        short_name;

}TLMOBJ_PAINT_PARAM,*LPTLMOBJ_PAINT_PARAM;

typedef struct _TLMOBJ_PAINT_PARAM1
{
 DWORD          dw_size;
 DWORD          paint_ctx;    //Контекст отрисовки
 HDC            paint_dc;     //DC где рисовать
 HBITMAP        bkgnd_bmp;    //Фоновое изображение
 HFONT          font;         //Шрифт для отрисовки текста
 int            erase;        //Очистить стереть
 BOOL           selected;     //Рисовать как выбранный
 DWORD          flash_counter;//Cчетчик мигания
 MDB_KADR_ENTRY entry;        //Описатель в кадре
 MDB_RECORD     record;       //Описатель записи
 wchar_t        question_sign;
 const wchar_t* short_name; //Короткое имя
 const wchar_t* full_name;  //Длинное имя
}TLMOBJ_PAINT_PARAM1,*LPTLMOBJ_PAINT_PARAM1;

#define TLMOBJ_PAINT_FLAG_ERASE        1
#define TLMOBJ_PAINT_FLAG_ERASE_FLASH  2


#pragma pack(pop)

typedef DWORD (__fastcall * tlm_paint_proc)(LPTLMOBJ_PAINT_PARAM paint_param,BOOL,void * paint_var);

#ifdef __cplusplus
extern "C"{
#endif
HBITMAP WINAPI create_bitmap(DWORD width,DWORD height);
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus


inline void __fastcall move_rect(RECT & r,int dx,int dy)
{
 r.left+=dx,r.top+=dy;
 r.right+=dx,r.bottom+=dy;
}

inline void __fastcall grow_rect(RECT & r,int dx,int dy)
{
 r.right+=dx,r.bottom+=dy;
}



inline void __fastcall union_rect(RECT &r1,RECT & r2,RECT &result)
{
 result.left   = KeRTL::MIN(r1.left,r2.left);
 result.top    = KeRTL::MIN(r1.top ,r2.top );
 result.right  = KeRTL::MAX(r1.right,r2.right);
 result.bottom = KeRTL::MAX(r1.bottom,r2.bottom);
}

inline void __fastcall intersect_rect(RECT &r1,RECT & r2,RECT &result)
{
 result.left   = KeRTL::MAX(r1.left,r2.left);
 result.top    = KeRTL::MAX(r1.top ,r2.top );
 result.right  = KeRTL::MIN(r1.right,r2.right);
 result.bottom = KeRTL::MIN(r1.bottom,r2.bottom);
}

#endif

//#ifdef _TLMOBJ_PAINTER_INTERNAL

#include <string>
#include <vector>
#include <memory>

  #define RESOURCE_BITMAP  L"//RESOURCE_BITMAP:"

  bool __fastcall init_module();


  class Tpaint_variant;

  class obj_painter_t
  {
    protected:
    obj_painter_t(){};
    virtual void __fastcall paint     (const LPTLMOBJ_PAINT_PARAM1  pparam, const Tpaint_variant * pvar) = 0;
    virtual void __fastcall calc_size (const LPTLMOBJ_PAINT_PARAM1  pparam, const Tpaint_variant * pvar,LPSIZE sz) {};
    static  void __fastcall get_rect(RECT & r,const LPTLMOBJ_PAINT_PARAM1  pparam);

    static  void __fastcall erase     (const LPTLMOBJ_PAINT_PARAM1  pparam, const Tpaint_variant * pvar);
    static  void __fastcall erase     (HDC dc,HBITMAP bmp,RECT & r ,const Tpaint_variant * pvar);
    static  void __fastcall name_size (const LPTLMOBJ_PAINT_PARAM1  pparam, SIZE & cx);
    static  void __fastcall name_paint(const LPTLMOBJ_PAINT_PARAM1  pparam, const COLORREF color, const RECT * r );
    static  void __fastcall name_paint(const HDC dc,const RECT & r,const COLORREF color,const wchar_t * name);
    static  void __fastcall name_paint_add     (const LPTLMOBJ_PAINT_PARAM1  pparam);
    static  void __fastcall name_add_get_rect  (const LPTLMOBJ_PAINT_PARAM1  pparam,RECT & dst_rect);

    static  void __fastcall paint_question_sign(LPTLMOBJ_PAINT_PARAM1 paint_param,LPRECT r = NULL);
    static  int  __fastcall get_temp_bmp_bits (const HDC dc, const Tpaint_variant * pv);
    static  bool __fastcall set_temp_bmp_bits (const HDC dc, const Tpaint_variant * pv);
    static  bool __fastcall init_temp_bmp     (const Tpaint_variant * pvar,int cx,int cy);
    static  void __fastcall frame_entry       (const LPTLMOBJ_PAINT_PARAM1  pparam,const Tpaint_variant * pvar);
    static  void __fastcall paint_line        (const LPTLMOBJ_PAINT_PARAM1  pparam,COLORREF color);
    public:
    virtual ~obj_painter_t()  {release();};
    virtual void            __fastcall release(){}
    virtual obj_painter_t * __fastcall clone   () = NULL;

    void operator ()(const LPTLMOBJ_PAINT_PARAM1  pparam,const Tpaint_variant * pvar);
    LRESULT  __fastcall do_calc_size (const LPTLMOBJ_PAINT_PARAM1  pparam, const Tpaint_variant * pvar,LPSIZE sz);
       int   __fastcall get_value_text(LPTLMOBJ_PAINT_PARAM1 paint_param, bool fill_zero,wchar_t * result,DWORD result_sz,int * point_pos = NULL);

    static void    __fastcall rect_upper (const RECT & src,const SIZE & sz, LPRECT result);
    static void    __fastcall rect_bottom(const RECT & src,const SIZE & sz, LPRECT result);
    static void    __fastcall rect_right (const RECT & src,const SIZE & sz, LPRECT result);
    static void    __fastcall rect_left  (const RECT & src,const SIZE & sz, LPRECT result);
    static HBITMAP __fastcall create_bitmap(DWORD width,DWORD height);
    static HBITMAP __fastcall clone_bitmap (HBITMAP src);
    static void    __fastcall centering_rects(RECT & dest, RECT & src);
    static HBITMAP __fastcall load_image(const char    * file_name);
    static HBITMAP __fastcall load_image(const wchar_t * file_name);
    static HBITMAP __fastcall load_image(HINSTANCE inst,DWORD res_id);
    static void    __fastcall get_bitmap_size(HBITMAP bmp,SIZE & sz);
    static COLORREF danger_color;
    static COLORREF alarm_color;

  };

class discrete_painter_t: public obj_painter_t
{
 protected:
 virtual void __fastcall paint     (const LPTLMOBJ_PAINT_PARAM1  pparam, const Tpaint_variant * pvar) ;
 virtual void __fastcall calc_size (const LPTLMOBJ_PAINT_PARAM1  pparam, const Tpaint_variant * pvar,LPSIZE sz);
         int  __fastcall get_index (const  LPMDB_RECORD   record );
         void            colored_temp_bmp(const HDC dc, const Tpaint_variant * pvar,COLORREF on_white,COLORREF on_black);

 public:
 discrete_painter_t(){};
 virtual obj_painter_t * __fastcall clone   (){return new discrete_painter_t;}
};

class analog_text_painter_t : public obj_painter_t
{
  protected:
  bool      simple_text;
  virtual void  __fastcall paint         (const LPTLMOBJ_PAINT_PARAM1  pparam,const Tpaint_variant * pvar) ;
  virtual void  __fastcall calc_size     (const LPTLMOBJ_PAINT_PARAM1  pparam, const Tpaint_variant * pvar,LPSIZE sz);
  public:
  analog_text_painter_t(bool _simple_text = false):simple_text(_simple_text){};
  virtual obj_painter_t * __fastcall clone   (){return new analog_text_painter_t(simple_text);}
};

class analog_bar_painter_t : public analog_text_painter_t
{
  protected:
  bool vertical;
  virtual void __fastcall paint(const LPTLMOBJ_PAINT_PARAM1  pparam,const Tpaint_variant * pvar);
  static  void __fastcall split_rect    (bool vertical,int split_line,RECT & r1,RECT & r2);
  static  void __fastcall calc_bar_param(bool vertical,RECT & r,MDB_RECORD & rec
                                         ,int & pixels,int & val_pos
                                         ,int & min_danger_pos,int & max_danger_pos
                                         ,int & min_alarm_pos,int & max_alarm_pos
                                         ,int & zero_pos);
          void  __fastcall get_rect(RECT & r,const LPTLMOBJ_PAINT_PARAM1  pparam);
  virtual void __fastcall calc_size  (const LPTLMOBJ_PAINT_PARAM1  pparam, const Tpaint_variant * pvar,LPSIZE sz);

  public:
  analog_bar_painter_t(bool _vertical,bool _paint_text)
  :analog_text_painter_t(_paint_text)
   ,vertical(_vertical)
   {};
  virtual obj_painter_t * __fastcall clone   () {return new analog_bar_painter_t(vertical,simple_text);}
};

class analog_led_painter_t : public analog_text_painter_t
{
  protected:
          HBITMAP   image_bitmap;
          HBITMAP   mask_bitmap ;
          HDC       image_dst_dc;
          HDC       mask_dst_dc;
          int       image_width;
          void __fastcall create_temp_bitmaps (int width,int height);
          void __fastcall release_temp_bitmaps();
          HDC  __fastcall get_image_dc    (HDC dc){if(!image_dst_dc) image_dst_dc = CreateCompatibleDC(dc); return image_dst_dc;}
          HDC  __fastcall get_mask_dc     (HDC dc){if(!mask_dst_dc ) mask_dst_dc  = CreateCompatibleDC(dc); return mask_dst_dc ;}

  virtual void __fastcall paint           (const LPTLMOBJ_PAINT_PARAM1  pparam,const Tpaint_variant * pvar);
          void __fastcall get_rect        (RECT & r,const LPTLMOBJ_PAINT_PARAM1  pparam);
          void __fastcall get_colors      (const LPTLMOBJ_PAINT_PARAM1  pparam,DWORD & color,DWORD & bk_color);
          int  __fastcall get_digit_idx   (wchar_t ch     ,int & width, int digit_width);
          int  __fastcall get_digits_width(wchar_t * vtext,int len    , int digit_width);
  virtual void __fastcall calc_size     (const LPTLMOBJ_PAINT_PARAM1  pparam, const Tpaint_variant * pvar,LPSIZE sz);



  public:
  analog_led_painter_t():image_bitmap(NULL),mask_bitmap(NULL),image_dst_dc(NULL),mask_dst_dc(NULL),image_width(0){};
  virtual ~analog_led_painter_t(){};
  virtual void  __fastcall release();
  virtual obj_painter_t * __fastcall clone   () {return new analog_led_painter_t();}
};


#define DEF_FRAME_COLOR RGB(255,255,255)

struct   Tpaint_variant
{
 DWORD        paint_context;
 SIZE         sz;
 int          elements_count;
 int          switched;
 int          multi_position; //Ограичение для многопозиционных дискретных значений
 HBITMAP      mask;
 HBITMAP      image;

 HBITMAP      temp_bmp;
 HDC          temp_dc;
 HDC          mem_dc;
 LPBYTE       temp_bmp_rgb;
 BITMAPINFO   temp_bmp_info;
 DWORD        reserv_get_di_bits[8];

 HPEN         frame_pen;
 DWORD        frame_color;
 DWORD        frame_pen_width;

 std::wstring name;
 std::wstring image_file;
 std::wstring mask_file;
 tlm_paint_proc   paint_proc;
 obj_painter_t  * obj_painter;

 void     __fastcall release()
 {
  if(mem_dc      )  {DeleteDC    (mem_dc   );  mem_dc       = NULL;}
  if(temp_dc     )  {DeleteDC    (temp_dc  );  temp_dc      = NULL;}
  if(mask        )  {DeleteObject(  mask   );  mask         = NULL;}
  if(image       )  {DeleteObject( image   );  image        = NULL;}
  if(temp_bmp    )  {DeleteObject(temp_bmp );  temp_bmp     = NULL;}
  if(frame_pen   )  {DeleteObject(frame_pen);  frame_pen    = NULL;}
  if(temp_bmp_rgb)  {delete [] temp_bmp_rgb;   temp_bmp_rgb = NULL;}
  if(obj_painter )  {obj_painter->release() ;  delete obj_painter; obj_painter  = NULL;}

 }

 HDC  __fastcall get_temp_dc(HDC dc) const  {  if( !temp_dc && dc) ((Tpaint_variant*)this)->temp_dc = CreateCompatibleDC(dc); return temp_dc;}
 HDC  __fastcall get_mem_dc (HDC dc) const  {  if( !mem_dc  && dc) ((Tpaint_variant*)this)->mem_dc  = CreateCompatibleDC(dc); return  mem_dc;}

 void __fastcall release_temp_bmp( ) const
  {
    if(temp_bmp) DeleteObject(temp_bmp);
     ((Tpaint_variant*)this)->temp_bmp = NULL;
    if(temp_bmp_rgb) delete [] temp_bmp_rgb;
     ((Tpaint_variant*)this)->temp_bmp_rgb = NULL;
     bzero(&((Tpaint_variant*)this)->temp_bmp_info,sizeof(temp_bmp_info));
  }

  HPEN  __fastcall get_frame_pen() const
  {
   if(!frame_pen)
   {
    LOGBRUSH lb;
    lb.lbStyle = BS_SOLID;
    lb.lbColor = frame_color;
    lb.lbHatch = HS_HORIZONTAL;
    ((Tpaint_variant*)this)->frame_pen = ExtCreatePen(PS_GEOMETRIC|PS_DASH|PS_ENDCAP_ROUND,frame_pen_width,&lb,0,NULL);;
    //((Tpaint_variant*)this)->frame_pen = CreatePen(PS_DASH,frame_pen_width,frame_color);
   }
   return frame_pen;
  }

 Tpaint_variant(DWORD _paint_context)
               :paint_context(_paint_context)
               ,elements_count(0)
               ,mask    ( NULL )
               ,image   ( NULL )
               ,temp_bmp( NULL )
               ,temp_dc ( NULL )
               ,mem_dc  ( NULL )
               ,switched(0)
               ,multi_position(1)
               ,paint_proc( NULL)
               ,obj_painter(NULL)
               ,temp_bmp_rgb(NULL)
               ,frame_pen   (NULL)
               ,frame_pen_width(2)

 {
  sz.cx = 0;
  sz.cy = 0;
  frame_color = DEF_FRAME_COLOR;
  bzero(&temp_bmp_info,sizeof(temp_bmp_info));
 }

 Tpaint_variant(DWORD _paint_context,std::wstring  str,int _width,int _height)
               :paint_context(_paint_context)
               ,name(str)
               ,elements_count(0)
               ,mask(NULL)
               ,image(NULL)
               ,temp_bmp( NULL )
               ,temp_dc ( NULL )
               ,mem_dc (NULL)
               ,switched(0)
               ,multi_position(1)
               ,paint_proc(NULL)
               ,obj_painter(NULL)
               ,temp_bmp_rgb(NULL)
               ,frame_pen_width(2)
 {
  sz.cx = _width;
  sz.cy = _height;
  bzero(&temp_bmp_info,sizeof(temp_bmp_info));
  frame_color = DEF_FRAME_COLOR;
 }

 Tpaint_variant (DWORD paint_context,const Tpaint_variant &src);

 ~Tpaint_variant()
 {
  release();
 }
};

inline Tpaint_variant::Tpaint_variant (DWORD _paint_context,const Tpaint_variant &src)
               :paint_context(_paint_context)
               ,mask    ( NULL )
               ,image   ( NULL )
               ,temp_bmp( NULL )
               ,temp_dc ( NULL )
               ,mem_dc  ( NULL )
               ,switched(0)
               ,temp_bmp_rgb(NULL)
               ,frame_pen   (NULL)

{
  name = src.name;
  elements_count = src.elements_count;
  frame_color    = src.frame_color;
  frame_pen_width = src.frame_pen_width;

  mask  = obj_painter_t::clone_bitmap(src.mask);
  image = obj_painter_t::clone_bitmap(src.image);

  multi_position  = src.multi_position;
  paint_proc      = src.paint_proc;
  if(src.obj_painter)
  obj_painter      = src.obj_painter->clone();
  image_file       = src.image_file;
  mask_file  = src.mask_file ;
  sz     = src.sz;
}

typedef std::vector<Tpaint_variant*>  Tpaint_variants;
//#endif

#endif // _RC_INVOKED
#endif

