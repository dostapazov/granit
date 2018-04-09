/***************************************************************
 *
 * Модуль    : kertls.lib
 * Описание  : Текстовый просмотрщик
 * Автор     : Остапенко Д.В.
 * Версия    :1
 * Создан    :11.01.2002 10:41:59
 *
 ****************************************************************/


#ifndef TEXT_VIEWER_HPP
#define TEXT_VIEWER_HPP
 #ifndef  GRKE_WINCTRLS_INC
 #include <gkwctrls.hpp>
 #endif
 #include <vector>


#ifndef _NO_NAMESPACE
namespace KeWin{
#endif

 using namespace std;
 inline int __fastcall line_len(const char * line)
 {return lstrlenA(line);}

 inline int __fastcall line_len(const wchar_t * line)
 {return lstrlenW(line);}

 inline char * __fastcall line_copy( char * dst,const char* src)
 {return lstrcpyA(dst,src);}
 inline wchar_t* __fastcall line_copy( wchar_t * dst,const wchar_t* src)
 {return lstrcpyW(dst,src);}

 inline int    __fastcall line_copy_lim( char * dst,const char* src,int lim)
 {
  if(lim)
  {
  lim--;
  return lstrcpynA(dst,src,lim-1) ? lstrlenA(dst):0;
  }
  return 0;
 }
 inline int    __fastcall line_copy_lim( wchar_t * dst,const wchar_t* src,int lim)
 {
  if(lim)
  {
  lim--;
  return lstrcpynW(dst,src,lim-1) ? lstrlenW(dst):0;
  }
  return 0;
 }



 #define TTV_TXTVIEWER_FIRST (WM_USER+1000)

 #define TTV_GETLINES_LIMIT  (TTV_TXTVIEWER_FIRST)
 #define TTV_IS_UNICODE      (TTV_TXTVIEWER_FIRST+1)
 #define TTV_ADD_UNICODE     (TTV_TXTVIEWER_FIRST+2)
 #define TTV_ADD_ANSI        (TTV_TXTVIEWER_FIRST+3)
 #define TTV_INSERT_UNICODE  (TTV_TXTVIEWER_FIRST+4)
 #define TTV_INSERT_ANSI     (TTV_TXTVIEWER_FIRST+5)
 #define TTV_REPLACE_UNICODE (TTV_TXTVIEWER_FIRST+6)
 #define TTV_REPLACE_ANSI    (TTV_TXTVIEWER_FIRST+7)
 #define TTV_SETLINES_LIMIT  (TTV_TXTVIEWER_FIRST+8)
 #define TTV_REMOVE_LINE     (TTV_TXTVIEWER_FIRST+9)
 #define TTV_GETLINES_COUNT  (TTV_TXTVIEWER_FIRST+10)
 #define TTV_CLEAR           (TTV_TXTVIEWER_FIRST+11)
 #define TTV_GET_LINE_TEXT   (TTV_TXTVIEWER_FIRST+12)
 #define TTV_GET_LINE_COLOR  (TTV_TXTVIEWER_FIRST+13)
 #define TTV_SET_LINE_COLOR  (TTV_TXTVIEWER_FIRST+14)

    template<typename C>
    struct get_text_param
    {
     DWORD line;
     C *   buff_ptr;
     DWORD buff_size;
     get_text_param():line (0),buff_ptr (NULL),buff_size(0){}
     get_text_param(DWORD ln,C* buff,DWORD bsz):line(ln),buff_ptr (buff),buff_size(bsz){}
    };

    template<typename C>
    struct color_line
    {
     DWORD color;
     C*    text;
     color_line(){text = NULL;}
     color_line(const C * _text,DWORD _color = -1);
     color_line(const color_line & cl) {text = NULL;*this = cl;}
     ~color_line(){free();}
     C* operator()(){return text;}
     color_line & operator = (const color_line & cl)
     {color = cl.color; *this = cl.text;return *this;}
     color_line & operator = (const C * _text);

     int   __fastcall length(){return text ? line_len(text):0;}
     void  __fastcall free(){if(text) delete[] text; text = 0;}
    };

template<typename C>
color_line<C>::color_line(const C * _text,DWORD _color )
{
text  =   NULL;
color = _color;
*this = _text;
};

template<typename C>
color_line<C> & color_line<C>::operator = (const C* _text)
{
free();
text  = (_text && *_text) ? new C[line_len(_text)+1] : NULL; if(text) line_copy(text,_text);
return *this;
}



 template<typename C>
 KEWIN_CLASS TTextViewer:public TWindow
 {
    protected:
    typedef color_line<C>    text_line;
    vector<text_line*>       lines; //Список строк
    DWORD           lines_limit;   //Ограничение на количество строк
    int             line_height;   //Высота строки в пикселах
    int             line_space;    //Пространство между строк в пикселах
    int             x_margin;      //Отступ
    int             offset_lines ; //кол-во строк cверху
    int             wrap_lines   ; //Разрешение переноса строки
    bool            vscroll_bar_visible;//ОТображается или нет полоса прогрутки
    SCROLLINFO      vsb;
    int             max_line_width;     //Максимальная ширина строки в символах
    int             max_line_len;
    int             offset_symbols;
    bool            hscroll_bar_visible;//ОТображается или нет полоса прогрутки
    SCROLLINFO      hsb;
    bool            lock_draw;

    void         __fastcall GetWndClass(WNDCLASSEXW & wc);
    const wchar_t * __fastcall GetClassName(){return L"KeWin_TTextViewer";};
    void         __fastcall OnSize(int h,int x,int y);
    void         __fastcall OnWMSetFont(HFONT Font,bool Redraw);
    void         __fastcall OnWMKeyDown(DWORD wParam,DWORD lParam);
    DWORD        __fastcall GetDlgCode(){return DLGC_WANTARROWS	;}
    int          __fastcall Paint(HDC dc,RECT & r,bool);
    int          __fastcall visible_lines_count();//Количество видимых линий
    void         __fastcall free_lines();
    void         __fastcall OnWMDestroy();
    bool         __fastcall OnSizing(int side ,TRect & old_rect,TRect & new_rect);
    void         __fastcall OnWMGetMinmaxInfo(LPMINMAXINFO mmi);


    void __fastcall do_add_line    (const C * line,DWORD color);
    void __fastcall do_insert_line (int pos, const C* line);
    void __fastcall do_replace_line(int pos,const  C* line);
    void __fastcall do_set_lines_limit(int ll){lines_limit = ll;}
    void __fastcall do_remove_line(int idx);
    DWORD __fastcall do_get_line_text(get_text_param<C> * param);
    COLORREF __fastcall do_get_line_color(DWORD line_num);
    void  __fastcall do_set_line_color(DWORD line_num,COLORREF color);
    void __fastcall update_vscroll();
    void __fastcall update_hscroll();
    void __fastcall hscroll_handle(HWND sb,DWORD code,DWORD position);
    void __fastcall vscroll_handle(HWND sb,DWORD code,DWORD position);
    void __fastcall do_clear();
    int  __fastcall calc_line_width(C* str);
    int  __fastcall mouse_wheel(WORD keys,short delta, short xPos,short yPos);
    LRESULT  __fastcall MessageHandler(MSG & msg);

    public:
    TTextViewer();
    ~TTextViewer(){free_lines();Destroy();};
    void         __fastcall set_lock_draw(bool lock);
    bool __fastcall DoCreate(HWND parent,TRect & r,int id = -1,DWORD lines_limit = 512,const wchar_t * class_name = 0);
    bool __fastcall DoCreate(HWND parent,int id);


    bool __fastcall is_unicode  (){return sizeof(C) == sizeof(wchar_t);}

    void  __fastcall add_line    (const char * line,DWORD color = -1);
    void  __fastcall insert_line (int pos, const char* line)    {SendMessage(TTV_INSERT_ANSI,WPARAM(pos),LPARAM(line));}
    void  __fastcall replace_line(int pos, const  char* line)   {SendMessage(TTV_REPLACE_ANSI,WPARAM(pos),LPARAM(line));}
    void  __fastcall add_line    (const wchar_t * line,DWORD color = -1) {SendMessage(TTV_ADD_UNICODE,color,LPARAM(line));Repaint(false,NULL);}
    void  __fastcall insert_line (int pos, const wchar_t* line) {SendMessage(TTV_INSERT_UNICODE,WPARAM(pos),LPARAM(line));}
    void  __fastcall replace_line(int pos, const  wchar_t* line){SendMessage(TTV_REPLACE_UNICODE,WPARAM(pos),LPARAM(line));}
    void  __fastcall set_lines_limit(int ll){SendMessage(TTV_SETLINES_LIMIT,WPARAM(ll),0);};
    DWORD __fastcall get_lines_count(){return (DWORD)SendMessage(TTV_GETLINES_COUNT,0,0);};
    void  __fastcall clear(){SendMessage(TTV_CLEAR,0,0);}
    DWORD __fastcall get_max_line_len(){return max_line_len;}
    DWORD __fastcall get_max_line_width(){return max_line_width;}
    DWORD __fastcall lines_count(){return lines.size();}
    DWORD __fastcall get_line_text(int line_num,C * buffer,DWORD buffer_len)
    {get_text_param<C> gtp(line_num,buffer,buffer_len); return(DWORD)SendMessage(TTV_GET_LINE_TEXT,0,(LPARAM)&gtp);}
    COLORREF __fastcall get_line_color(DWORD line_num)
    {return (COLORREF)SendMessage(TTV_GET_LINE_COLOR,line_num,0);};
    void     __fastcall set_line_color(DWORD line_num,COLORREF color,BOOL repaint = FALSE)
    {
     SendMessage(TTV_SET_LINE_COLOR,line_num,color);
     if(repaint) Repaint();
    }
 };


/*implementation */

template<typename C>
TTextViewer<C>::TTextViewer():
TWindow()
{
lines_limit = 256;
vscroll_bar_visible = false;
hscroll_bar_visible = false;
max_line_width      = 0;
max_line_len        = 0;
x_margin            = 0;
ZeroMemory(&vsb,sizeof(vsb));
vsb.cbSize = sizeof   (vsb);
ZeroMemory(&hsb,sizeof(hsb));
hsb.cbSize = sizeof   (hsb);
};

template<typename C>
void     __fastcall TTextViewer<C>::GetWndClass(WNDCLASSEXW & wc)
{
 TWindow::GetWndClass(wc);
 wc.style |= CS_OWNDC;
 wc.style &= ~CS_PARENTDC;
}



template<typename C>
bool __fastcall TTextViewer<C>::DoCreate(HWND parent,TRect & r,int id,DWORD lines_limit,const wchar_t * class_name )
{
 this->lines_limit =lines_limit;
 this->line_space  = 2;
 DWORD style = WS_VISIBLE;
       if(parent)
       style |= (WS_CHILD|WS_TABSTOP);
 Create(parent,0,r,id,0,style,0,class_name);
 SetBkColor(0xFFFFFF);
 ::EnableWindow(hWnd,TRUE);
 return hWnd ? true:false;
}

template<typename C>
bool __fastcall TTextViewer<C>::DoCreate(HWND parent,int id)
{

}


template<typename C>
int          __fastcall TTextViewer<C>::Paint(HDC dc,RECT & r,bool erase)
{
 // Отрисовка строк
 if(Font)
    Font = (HFONT)SelectObject(dc,Font);
 std::vector<text_line*>::iterator i ,e;
  i = lines.begin();
  e = lines.end();
  //advance(i,offset_lines);
  i+=offset_lines;
 int fh = line_height+line_space;
 TRect cr = GetClientRect();
 int y_pos = line_space;
 COLORREF save_color = GetTextColor(dc);
 __try{
 while(i<e && *i && y_pos<cr.Height())
 {

  if(!erase)
  {
   //Стереть строку
   FillRect(dc,&TRect(0,y_pos,cr.Width(),fh),this->bkBrush);
  }

  text_line * line = *i;
  int len ;
  if(!IsBadReadPtr(line,sizeof(*line)))
     len = line->length()-int(hsb.nPos);
     else
     len = 0;
  if(len>0)
  {
    #ifdef __BORLANDC__
    #pragma warn -8008
    #pragma warn -8066
    #endif
    if(save_color != line->color && line->color != (DWORD)-1)
      SetTextColor(dc,line->color);
    if(sizeof(C) == sizeof(char))
     TextOutA(dc,x_margin,y_pos,(char*)line->text+hsb.nPos,len);
     else
     TextOutW(dc,x_margin,y_pos,(wchar_t*)line->text+hsb.nPos,len);
    if(save_color != line->color && line->color != (DWORD)-1)
      SetTextColor(dc,save_color);

    #ifdef __BORLANDC__
    #pragma warn .8008
    #pragma warn .8066
    #endif
  }
   y_pos+=fh;
   i++;
 }
 if(y_pos<cr.Height())
 {
  cr.Move(0,y_pos);
  cr.Grow(0,-y_pos);
  FillRect(dc,&cr,bkBrush);
 }
 }
 __except(EXCEPTION_CONTINUE_SEARCH){DebugBreak();};
 if(Font)
    Font = (HFONT)SelectObject(dc,Font);

 return 0;
}


template<typename C>
inline int          __fastcall TTextViewer<C>::visible_lines_count()//Количество видимых линий
{
 int full_line_height = line_height+line_space;
 if(full_line_height)
 return max(0,int(GetClientRect().Height()/(full_line_height)));
 return 0;
}

template<typename C>
void         __fastcall TTextViewer<C>::OnWMSetFont(HFONT Font,bool Redraw)
{
    LOGFONT lf;
    GetObject(Font,sizeof(lf),&lf);
    this->line_height = lf.lfHeight;
    if(line_height<0)
    line_height = -MulDiv(line_height,GetDeviceCaps(ClientDC, LOGPIXELSY), 72);
    SelectObject(ClientDC,Font);
    TWindow::OnWMSetFont(Font,Redraw);
}

template<typename C>
int  __fastcall TTextViewer<C>::calc_line_width(C* str)
{
 int ret = 0;
 if(str)
 {
  SIZE sz;
  int len;

    #ifdef __BORLANDC__
    #pragma warn -8008
    #pragma warn -8066
    #endif
    if(sizeof(C) == sizeof(char))
     {
      len = lstrlenA((char*)str);
      GetTextExtentPoint32A((::HDC)ClientDC,(char*)str,len,&sz);
     }
     else
     {
      len = lstrlenW((wchar_t*)str);
      GetTextExtentPoint32W((::HDC)ClientDC,(wchar_t*)str,len,&sz);
     }
     max_line_len = max(len,max_line_len);
    #ifdef __BORLANDC__
    #pragma warn .8008
    #pragma warn .8066
    #endif
    ret = sz.cx;
 }
 return ret;
}

template<typename C>
void         __fastcall TTextViewer<C>::set_lock_draw(bool lock)
{
  lock_draw = lock;

  if(!lock_draw)
  {
   update_hscroll();
   update_vscroll();
   LockWindowUpdate(NULL);
   RedrawWindow((::HWND)hWnd,0,0,RDW_INVALIDATE);
  }
  else
  {
   LockWindowUpdate(hWnd);
  }

}

template<typename C>
 void  __fastcall TTextViewer<C>::add_line    (const char * line,DWORD color  )
 {
  if(hWnd )
   {
    SendMessage(TTV_ADD_ANSI,color,LPARAM(line));
    Repaint(false,NULL);

   }
   else
   do_add_line ((C*)line,color);


 }

template<typename C>
void __fastcall TTextViewer<C>::do_add_line(const C * _line,DWORD color )
{
 text_line *tl = new text_line(_line,color);
 max_line_width = max(calc_line_width((*tl)()),max_line_width);
 lines.push_back(tl);
 if(lines.size()> DWORD(lines_limit))
  {
   delete *lines.begin();
   lines.erase(lines.begin());
  }
  offset_lines = max(0,int(lines.size())-this->visible_lines_count());
  if(!lock_draw)
     {
      update_hscroll();
      update_vscroll();
      RedrawWindow((::HWND)hWnd,0,0,RDW_INVALIDATE);
     }
}

    /*Вставка строки*/
    template<typename C>
    void __fastcall TTextViewer<C>::do_insert_line (int pos, const C* line)
    {
     if(pos<int(lines.size()))
     {
      if(this->lines_limit>0 && lines_limit+1 < lines.size())
      {
       delete *lines.begin();
       lines.erase(lines.begin());
      }

      vector<text_line*>::iterator i = lines.begin();
      advance(i,pos);
      text_line *tl = new text_line(line);
      max_line_width = max(calc_line_width((*tl)()),max_line_width);
       lines.insert(i,tl);
       update_hscroll();
       update_vscroll();
       RedrawWindow(hWnd,0,0,RDW_INVALIDATE);
     }
     else
     {
      lines.resize(pos+1,0);
      insert_line(pos,line);
     }
    }


    /*Замещение строки*/
    template<typename C>
    void __fastcall TTextViewer<C>::do_replace_line(int pos,const C* line )
    {
     if(pos>int(lines.size()))
       insert_line(pos,line);
       else
       {
         vector<text_line*>::iterator i = lines.begin();
         advance(i,pos);
         if(line && *line)
            max_line_width = max(calc_line_width((C*)line),max_line_width);
          *(*i) = line;
          update_hscroll();
          RedrawWindow(hWnd,0,0,RDW_INVALIDATE);

       }
    }

template<typename C>
void         __fastcall TTextViewer<C>::free_lines()
{
  vector<text_line*>::iterator i = lines.begin(),e = lines.end();

  while(i<e)
  {

   if(*i)
	  {
       delete (*i);
      }
    i++;
  }
  lines.erase(lines.begin(),e);
  lines.resize(0);
}

template<typename C>
void         __fastcall TTextViewer<C>::do_remove_line(int idx)
{
 if(idx<int(lines.size()))
 {
   vector<text_line*>::iterator  i = lines.begin();
   advance(i,idx);
   if(*i)
     delete (*i);
    lines.erase(i);
 }
}

template<typename C>
void         __fastcall TTextViewer<C>::do_clear()
{
 free_lines();
 offset_lines   = 0;
 max_line_width = 0;
 max_line_len   = 0;
 update_hscroll();
 update_vscroll();
 RedrawWindow(hWnd,0,0,RDW_INVALIDATE);
}


template<typename C>
DWORD __fastcall TTextViewer<C>::do_get_line_text(get_text_param<C> * param)
{
 DWORD ret = 0;
 if(param && param->line < get_lines_count() && param->buff_ptr && param->buff_size)
 {
   text_line & tl = *lines.at(param->line);
   ret = line_copy_lim(param->buff_ptr,tl.text,param->buff_size);

 }
 return ret;
}

template<typename C>
COLORREF __fastcall TTextViewer<C>::do_get_line_color(DWORD line_num)
{
 COLORREF ret = -1;
 if(line_num<lines.size())
 {
  text_line & tl = *lines.at(line_num);
  ret = tl.color;
 }
 return ret;
}

template<typename C>
void  __fastcall TTextViewer<C>::do_set_line_color(DWORD line_num,COLORREF color)
{
 if(line_num<lines.size())
 {
  text_line & tl = *lines.at(line_num);
  tl.color = color;
 }
}


template<typename C>
void         __fastcall TTextViewer<C>::OnWMDestroy()
{
 free_lines();
 TWindow::OnWMDestroy();
}

template <typename C>
int  __fastcall TTextViewer<C>::mouse_wheel(WORD keys,short delta, short xPos,short yPos)
{
 int cnt = abs(delta)/WHEEL_DELTA;
 DWORD scode = delta>0 ?  ((keys&MK_SHIFT) ? SB_PAGEUP:SB_LINEUP):((keys&MK_SHIFT)?SB_PAGEDOWN:SB_LINEDOWN);
 while(cnt--)
  vscroll_handle(NULL,scode,0);
 return 0;
}


template <typename C>
void __fastcall TTextViewer<C>::OnWMKeyDown(DWORD wParam,DWORD lParam)
{

    WPARAM wScrollNotify(-1);
    switch (wParam) {
        case VK_UP:
            wScrollNotify = SB_LINEUP;
            break;
        case VK_PRIOR:
            wScrollNotify = SB_PAGEUP;
            break;
        case VK_NEXT:
            wScrollNotify = SB_PAGEDOWN;
            break;
        case VK_DOWN:
            wScrollNotify = SB_LINEDOWN;
            break;
        case VK_HOME:
            wScrollNotify = SB_TOP;
            break;
        case VK_END:
            wScrollNotify = SB_BOTTOM;
            break;
    }
    if (wScrollNotify!=(DWORD)-1 )
        SendMessage(WM_VSCROLL,MAKELONG(wScrollNotify, 0), 0L);
}


#ifdef __BORLANDC__
#pragma warn -8008
#pragma warn -8066
#endif

template<typename C>
LRESULT  __fastcall TTextViewer<C>::MessageHandler(MSG& msg)
{
 int ret = 0;
 switch(msg.message)
 {
  case WM_MOUSEWHEEL:
  if(msg.hwnd == hWnd)
  return mouse_wheel(LOWORD(msg.wParam),HIWORD(msg.wParam),LOWORD(msg.lParam),HIWORD(msg.lParam));
  break;
  case WM_GETDLGCODE      : return GetDlgCode();
  case WM_LBUTTONDOWN     : SetFocus();break;
  case WM_KEYDOWN         :   OnWMKeyDown(msg.wParam,msg.lParam);break;
  case TTV_GETLINES_COUNT : ret = int(lines.size());break;
  case TTV_GETLINES_LIMIT : ret = lines_limit;
  break;
  case TTV_IS_UNICODE     : ret = int( is_unicode());
  break;
  case TTV_ADD_UNICODE    : if(is_unicode())
                              do_add_line((C*)msg.lParam,(DWORD)msg.wParam);
                              else
                              {
                                char _text[1024];
                                char * text;
                                int len = safe_strlen((wchar_t*)msg.lParam);
                                if(len<(int)KERTL_ARRAY_COUNT(_text))
                                text = _text;
                                else
                                text  = new char[ len +1 ];
                                if(text)
                                {
                                 Unicode2Ansi(text,len,(wchar_t*)msg.lParam);
                                 do_add_line ((C*)text,(DWORD)msg.wParam);
                                 if(text != _text)
                                    delete [] text;
                                }

                              }
  break;
  case TTV_ADD_ANSI       : if(!is_unicode())
                              do_add_line((C*)msg.lParam,(DWORD)msg.wParam);
                             else
                              {
                                wchar_t _text[1024];
                                wchar_t * text;
                                int len = safe_strlen((char*)msg.lParam);
                                if(len<(int)KERTL_ARRAY_COUNT(_text))
                                text = _text;
                                else
                                text  = new wchar_t[ len +1 ];
                                if(text)
                                {
                                 Ansi2Unicode(text,len,(char*)msg.lParam);
                                 do_add_line ((C*)text,(DWORD)msg.wParam);
                                 if(text != _text)
                                    delete [] text;

                                }
                              }

  break;
  case TTV_INSERT_UNICODE : if(is_unicode())
                              do_insert_line(int(msg.wParam),(C*)msg.lParam);
  break;
  case TTV_INSERT_ANSI    : if(!is_unicode())
                              do_insert_line(int(msg.wParam),(C*)msg.lParam);
  break;
  case TTV_REPLACE_UNICODE: if(is_unicode())
                              this->do_replace_line(int(msg.wParam),(C*)msg.lParam);
  break;
  case TTV_REPLACE_ANSI   : if(!is_unicode())
                              this->do_replace_line(int(msg.wParam),(C*)msg.lParam);
  break;
  case TTV_SETLINES_LIMIT  :do_set_lines_limit(int(msg.wParam));
  break;
  case WM_VSCROLL          :
                           vscroll_handle(HWND(msg.lParam),LOWORD(msg.wParam),HIWORD(msg.wParam));
  break;
  case WM_HSCROLL          :
                           hscroll_handle(HWND(msg.lParam),LOWORD(msg.wParam),HIWORD(msg.wParam));
  break;
  case TTV_CLEAR           : do_clear();break;
  case TTV_GET_LINE_TEXT   : ret = (LRESULT)do_get_line_text((get_text_param<C> *)msg.lParam);break;
  case TTV_GET_LINE_COLOR  : ret = (LRESULT)do_get_line_color((DWORD)msg.wParam);break;
  case TTV_SET_LINE_COLOR  : do_set_line_color((DWORD)msg.wParam,(COLORREF)msg.lParam);break;
  default : ret =TWindow::MessageHandler(msg);
 }
 return ret;
}

#ifdef __BORLANDC__
#pragma warn .8008
#pragma warn .8066
#endif


template <typename C>
void __fastcall TTextViewer<C>::vscroll_handle(HWND sb,DWORD code,DWORD position)
{

 switch (code)
 {
  case SB_LINEDOWN      : offset_lines++;offset_lines = min(offset_lines,int(lines.size()-visible_lines_count()));break;
  case SB_LINEUP        : offset_lines--;offset_lines = max(offset_lines,0);break;
  case SB_PAGEUP        : offset_lines-=visible_lines_count();offset_lines = max(offset_lines,0);break;
  case SB_PAGEDOWN      : offset_lines+=visible_lines_count();offset_lines = min(offset_lines,vsb.nMax);break;
  case SB_THUMBTRACK    :
  case SB_THUMBPOSITION : offset_lines = position ;
  //case SB_ENDSCROLL   :
  break;
 }
   offset_lines = max(offset_lines,0);
   vsb.fMask =SIF_POS;
   vsb.nPos  = offset_lines;
   SetScrollInfo(hWnd,SB_VERT,&vsb,TRUE);
   EnableScrollBar(hWnd,SB_VERT,ESB_ENABLE_BOTH);
   RedrawWindow(hWnd,0,0,RDW_INVALIDATE);
   SetFocus();
}

template <typename C>
void __fastcall TTextViewer<C>::hscroll_handle(HWND sb,DWORD code,DWORD position)
{

 switch (code)
 {
  case SB_LINELEFT      : hsb.nPos = max(hsb.nPos-1,0);
  break;
  case SB_LINERIGHT     : hsb.nPos = min(hsb.nPos+1,max_line_width);
  break;
  case SB_THUMBTRACK    :
  break;
  case SB_THUMBPOSITION : hsb.nPos = position ;
  break;
 }
  {
   hsb.fMask =SIF_POS;
   SetScrollInfo(hWnd,SB_HORZ,&hsb,TRUE);
   EnableScrollBar(hWnd,SB_HORZ,ESB_ENABLE_BOTH);
   RedrawWindow(hWnd,0,0,RDW_INVALIDATE);
  }
   SetFocus();
}


template<typename C>
void __fastcall TTextViewer<C>::update_hscroll()
{
 bool need_show = max_line_width > GetClientRect().Width();
 if(hscroll_bar_visible != need_show  )
  {
   ShowScrollBar((::HWND)hWnd,SB_HORZ,need_show);
   vscroll_bar_visible = need_show;
  }
  if(vscroll_bar_visible)
   {
     hsb.fMask  = SIF_RANGE|SIF_PAGE|SIF_POS;
     hsb.nMax   = max_line_len;
     SetScrollInfo((::HWND)hWnd,SB_HORZ,&hsb,TRUE);
   }
}

template<typename C>
void __fastcall TTextViewer<C>::update_vscroll()
{

   bool need_show = lines.size()>DWORD(visible_lines_count());
   if(need_show!= vscroll_bar_visible)
   {
    ShowScrollBar((::HWND)hWnd,SB_VERT,need_show);
    vscroll_bar_visible = need_show;
   }
   if(vscroll_bar_visible)
   {
     int vlc    = visible_lines_count();
     vsb.fMask  = SIF_RANGE|SIF_PAGE|SIF_POS;
     vsb.nMax   = lines.size()-visible_lines_count();

     vsb.nPage  = vlc && vsb.nMax ? lines.size()/visible_lines_count():0;
     vsb.nPos   = offset_lines;
     SetScrollInfo((::HWND)hWnd,SB_VERT,&vsb,TRUE);
   }
}

template<typename C>
void         __fastcall TTextViewer<C>::OnSize(int h,int x,int y)
{
 update_vscroll();
 //hscroll_bar_visible = false;
 update_hscroll();
 TWindow::OnSize(h,x,y);
}

template<typename C>
bool         __fastcall TTextViewer<C>::OnSizing(int side ,TRect & old_rect,TRect & new_rect)
{
 return   TWindow::OnSizing(side,old_rect,new_rect);
}

template<typename C>
void         __fastcall TTextViewer<C>::OnWMGetMinmaxInfo(LPMINMAXINFO mmi)
{
 TWindow::OnWMGetMinmaxInfo(mmi);
}


#ifndef _NO_NAMESPACE
}//namespace KeWin{
#endif



#endif


