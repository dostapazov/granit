/*
 Заголовок определений для
 отрисовки различных объектов
*/


#ifndef _TLMDRAWING_OBJS_INC
#define _TLMDRAWING_OBJS_INC

#include <geometry.hpp>
#include <vector>
#include <Kestrm.hpp>

using namespace KeWin;
using namespace std;
using namespace KeRTL;

  //Базовый класс для рисуемых объектов
  class TDrawingBase
  {
    protected:
    KeWin::TRect area;
    virtual void  __fastcall do_paint(HDC dc, KeWin::TPoint & begin,bool flash_state) = 0;
    virtual void  __fastcall release (){};
    public:
    TDrawingBase();
    TDrawingBase(TRect & r);
    void __fastcall paint(HDC dc, bool flash_state){do_paint(dc,area.TopLeft(),flash_state);} ;
    virtual DWORD __fastcall get_type() = 0;
    virtual DWORD __fastcall stream_write(KeRTL::TStream *);
    virtual DWORD __fastcall stream_read (KeRTL::TStream *);
  };


  //Вспомогательный класс для создания объектов по типу
  class TDrawingCreater
  {
   public:
   TDrawingCreater(){};
   static TDrawingBase * __fastcall CreateObject(DWORD type);

  };

  #define TDB_TYPE_DRAWER 1

  class TDrawer:public TDrawingBase
  {
   protected:
   vector<TDrawingBase*> draw_list;
   public:
   TDrawer();
   DWORD __fastcall get_type() {return TDB_TYPE_DRAWER;}

  };

  class TShape:public TDrawingBase
  {
    public:
    enum  ShapesType {stLRDLine,stRDLLine,stTopLine,stBottomLene,stLeftLine,stRightLine,stRect,stRoundRect,stRound,stEllipse};
    protected:
    ShapesType Type
    DWORD      PenStyle;
    DWORD      PenWidth;
    COLORREF   Color;
    DWORD      FlashColor;
    HPEN       Pen;
    public:
    TShape();

    void  __fastcall do_paint(HDC dc, KeWin::TPoint & begin,bool flash_state) = 0;
    void  __fastcall release (){};
    DWORD __fastcall get_type() = 0;
    DWORD __fastcall stream_write(KeRTL::TStream *) = 0;
    DWORD __fastcall stream_read (KeRTL::TStream *) = 0;
  };


#endif

