
 #ifndef GEOMETRY_HPP_INC
  #define GEOMETRY_HPP_INC

 #ifndef _WINDOWS_
 #define STRICT
 #define WIN32_LEAN_AND_MEAN
  #include <windows.h>
 #undef WIN32_LEAN_AND_MEAN
 #endif

#ifndef _NO_NAMESPACE
#ifndef KrnlWin
#define KrnlWin KeWin
#endif
namespace KeWin
{
#endif

   struct TPoint :public tagPOINT
   {
     TPoint();
     TPoint(int x,int y );
     TPoint(const TPoint& p);
     TPoint(const POINT & p);
    TPoint & __fastcall operator = (const TPoint & p);
    TPoint & __fastcall operator +=(const TPoint & p);
    TPoint & __fastcall operator -=(const TPoint & p);
    void     __fastcall Move       (int dx,int dy);
   };

   int __fastcall operator == (const TPoint &p1,const TPoint & p2);
   int __fastcall operator != (const TPoint &p1,const TPoint & p2);

   struct TSize :public tagSIZE
   {
     TSize ();
     TSize (int x,int y );
     TSize (const TSize& p);
     TSize (const SIZE & s);
    TSize & __fastcall operator = (const TSize & p);
    TSize & __fastcall operator +=(const TSize & p);
    TSize & __fastcall operator -=(const TSize & p);
    void    __fastcall Grow(int dx,int dy);
   };

   int __fastcall operator  == (const TSize &p1,const TSize & p2);
   int __fastcall operator  != (const TSize &p1,const TSize & p2);


   struct TRect:public tagRECT
   {
     TRect();
     TRect(const  TRect &r);
     TRect(const  RECT  &r);
     TRect(int x,int y,int width,int height );
     TRect(const TPoint& Begin,const TSize & sz );
     TRect(const TPoint& TopLeft,const TPoint & BottomRight );

    TRect & __fastcall operator = (const TRect & r);
    void   __fastcall Move(int dx,int dy);
    void   __fastcall Grow(int cx,int cy);
    int    __fastcall Contain(const TPoint & p) const ;
    int    __fastcall Contain(const TRect  & r) const ;
    TPoint __fastcall TopLeft() const ;
    TPoint __fastcall TopRight() const ;
    TPoint __fastcall BottomLeft() const ;
    TPoint __fastcall BottomRight() const ;
    int    __fastcall Width();
    int    __fastcall Height();
    TSize  __fastcall Size();
   };

   int  operator == (const TRect &r1,const TRect & r2);
   int  operator != (const TRect &r1,const TRect & r2);


//--------------------------- Implement ----------------------------------------


    inline  TPoint::TPoint()
    {x = y = 0;}

    inline  TPoint::TPoint(int ax,int ay )
    { x = ax;y=ay;}

    inline  TPoint::TPoint(const TPoint& p)
    { x = p.x;y=p.y;}

    inline  TPoint::TPoint(const POINT& p)
    { x = p.x;y=p.y;}

    inline TPoint & __fastcall TPoint::operator = (const TPoint & p)
    { x = p.x;y=p.y; return * this;}

    inline TPoint & __fastcall TPoint::operator +=(const TPoint & p)
    { x += p.x;y+=p.y; return * this;}

    inline TPoint & __fastcall TPoint::operator -=(const TPoint & p)
    { x -= p.x;y-=p.y; return * this;}

    inline int __fastcall operator == (const TPoint &p1,const TPoint & p2)
    {return (p1.x == p2.x && p1.y == p2.y);}
    inline int __fastcall operator != (const TPoint &p1,const TPoint & p2)
    {return !(p1==p2);}
    inline void   __fastcall TPoint::Move(int dx,int dy)
    {x+=dx;y+=dy;}


    inline TSize::TSize()
    {cx=cy=0;}
    inline  TSize::TSize(int x,int y )
    {cx=x;cy=y;}
    inline  TSize::TSize(const TSize& s)
    { cx=s.cx;cy=s.cy;}
    inline  TSize::TSize(const SIZE& s)
    { cx=s.cx;cy=s.cy;}
    inline TSize & __fastcall TSize::operator = (const TSize & s)
    { cx=s.cx;cy=s.cy; return *this;}
    inline TSize & __fastcall TSize::operator +=(const TSize & s)
    { cx+=s.cx;cy+=s.cy;return *this;}
    inline TSize & __fastcall TSize::operator -=(const TSize & s)
    { cx-=s.cx;cy-=s.cy;return *this;}

    inline void    __fastcall TSize::Grow(int dx,int dy)
    { cx+=dx;cy+=dy;}

   inline int __fastcall operator == (const TSize &s1,const TSize & s2)
   { return (s1.cx == s2.cx && s1.cy == s2.cy); }
   inline int __fastcall operator != (const TSize &s1,const TSize & s2)
   {return !(s1==s2);}


   inline   TRect::TRect()
    {left = right = top = bottom = 0;}

    inline  TRect::TRect(int x,int y,int width,int height )
    {left = x; top = y;right = x+width;bottom = y+height;}

    inline  TRect::TRect(const TPoint& Begin,const TSize & sz )
    {left  = Begin.x;top = Begin.y;right =  left +sz.cx; bottom = top+sz.cy; }

    inline  TRect::TRect(const TPoint& TopLeft,const TPoint & BottomRight )
    { left = TopLeft.x; top = TopLeft.y; right = BottomRight.x;bottom = BottomRight.y;  }
    inline  TRect::TRect(const  TRect &r)
    {*this = r;}


    inline TRect &  __fastcall TRect::operator = (const TRect & r)
    {left = r.left;right = r.right;top=r.top;bottom = r.bottom;return *this;}
    inline TRect::TRect (const RECT & r)
    {left = r.left;right = r.right;top=r.top;bottom = r.bottom;}

    inline void     __fastcall TRect::Move(int dx,int dy)
    {left +=dx;top+=dy; right+=dx;bottom+=dy;}

    inline void     __fastcall TRect::Grow(int dx,int dy)
    { right+=dx;bottom+=dy; }

    inline int      __fastcall TRect::Contain(const TPoint & p) const 
    { return ( p.x>=left && p.x<= right && p.y>=top && p.y<=bottom );}

    inline int      __fastcall TRect::Contain(const TRect  & r) const
	{
	 #ifdef _WIN64
      return (Contain(r.TopLeft()) && Contain(r.TopRight()) && Contain(r.BottomLeft()) && Contain(r.BottomRight()));
	 #else
	  return (Contain(*&r.TopLeft()) && Contain(*&r.TopRight()) && Contain(*&r.BottomLeft()) && Contain(*&r.BottomRight()));
	 #endif
	}

    inline TPoint __fastcall TRect::TopLeft() const
    { return TPoint(left,top);}
    inline TPoint __fastcall TRect::TopRight() const
    {return TPoint(right,top);}
    inline TPoint __fastcall TRect::BottomLeft() const
    {return TPoint(left,bottom);}
    inline TPoint __fastcall TRect::BottomRight() const 
    {return TPoint(right,bottom);}

    inline int    __fastcall TRect::Width()
    {return right - left;}
    inline int    __fastcall TRect::Height()
    {return bottom - top;}

     inline TSize  __fastcall TRect::Size()
     {
      return TSize(Width(),Height());
     }


    inline int operator == (const TRect &r1,const TRect & r2)
    {return (r1.left == r2.left && r1.right == r2.right && r1.top == r2.top && r1.bottom == r2.bottom);}

    inline int operator != (const TRect &r1,const TRect & r2)
    {return !(r1==r2);}

#ifndef _NO_NAMESPACE
    }
#endif


 #endif

