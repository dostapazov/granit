/***************************************************************
 *
 * Модуль    : kertls.lib
 * Описание  : Кдассы WinControls
 * Автор     :Остапенко Д.В.
 * Версия    :1 
 * Создан    :10.01.2002 12:01:06
 *
 ****************************************************************/

#ifndef GRKE_WINCTRLS_INC
#define GRKE_WINCTRLS_INC


#ifndef GRKE_WINDOW_INC
#include <gkwind.hpp>
#endif

// winuser.h

#ifndef _NO_NAMESPACE
namespace KeWin
{
#endif

 KEWIN_CLASS TWinControl:public TWindow
 {
  protected:
          COLORREF  TextColor;
          const char * __fastcall GetClassName(){return "WndControl";};
          DWORD        __fastcall GetStyle(bool Ex = false){return Ex ? 0:WS_CHILD|WS_VISIBLE;};
  LRESULT __fastcall MessageHandler(MSG & msg);
  bool    __fastcall OnWMCreate(LPCREATESTRUCT );
  HBRUSH  __fastcall OnCtlColor(HWND,HDC,UINT);
  virtual DWORD __fastcall GetDlgCode(){return DefaultProcessing();}
  public:
  TWinControl(HINSTANCE inst= GetModuleHandle(0)):TWindow(inst),TextColor(0){}
  virtual void   __fastcall Create
    ( HWND Parent,const char * Caption,RECT & r,int Id = -1,
      HMENU Menu = 0,DWORD Style=0,DWORD ExStyle=0,const char * ClassName=0);
      bool __fastcall DoCreate(HWND Parent,int Id);
      void __fastcall EnableSelfDraw(bool);
      virtual DWORD   GetOwnerDrawStyle(){return 0;};
      COLORREF __fastcall GetTextColor();
      COLORREF __fastcall SetTextColor(COLORREF);
 };


 KEWIN_CLASS TButton:public TWinControl
 {
  public:
  TButton(HINSTANCE inst= GetModuleHandle(0)):TWinControl(inst){};
  bool __fastcall DoCreate(HWND Parent,char * Caption,TRect & r,int Id = -1);
  DWORD   GetOwnerDrawStyle(){return BS_OWNERDRAW;};
 };


 KEWIN_CLASS TStatic:public TWinControl
 {
   public:
   TStatic(HINSTANCE inst= GetModuleHandle(0)):TWinControl(inst){};
   bool __fastcall DoCreate(HWND Parent,char * Caption,TRect & r,int Id = -1);
   DWORD   GetOwnerDrawStyle(){return SS_OWNERDRAW;};
 };


 /*Мultiline Edit control*/
 KEWIN_CLASS TEdit:public TWinControl
 {
  LRESULT     __fastcall MessageHandler(MSG &msg);
  public:
  TEdit(HINSTANCE inst= GetModuleHandle(0)):TWinControl(inst){};
  bool   __fastcall DoCreate(HWND parent,TRect & r,int id=-1,DWORD style = 0,DWORD ex_style = 0);
  bool   __fastcall DoCreate(HWND parent,int id);
  DWORD  __fastcall GetText(char * text,int text_sz);
  bool   __fastcall SetText(char * text);
  bool   __fastcall IsMultiline  ();
  DWORD  __fastcall GetLinesCount();
  DWORD  __fastcall GetLineLength(DWORD line_number);
  DWORD  __fastcall GetLineText  (DWORD line_number, char * text,int text_sz);
  DWORD  __fastcall GetLineOffset(DWORD line);
 };



/* inlines */
      inline COLORREF __fastcall TWinControl::GetTextColor()
      {
       return TextColor;
      }


  inline bool __fastcall TWinControl::DoCreate(HWND Parent,int Id)
  {
    CreateFromHWND(::GetDlgItem((::HWND)Parent,Id));
    return hWnd ? true:false;
  }

  inline void __fastcall TWinControl::EnableSelfDraw(bool e)
  {
   if(hWnd)
    {
     if(e)
     SetWindowLong(GWL_STYLE,GetWindowLong(GWL_STYLE)|GetOwnerDrawStyle());
     else
     SetWindowLong(GWL_STYLE,GetWindowLong(GWL_STYLE)&~GetOwnerDrawStyle());
    }
  }


inline  DWORD   __fastcall TEdit::GetText(char * text,int text_sz)
{   return SendMessage(WM_GETTEXT,text_sz,LPARAM(text));  }

inline  bool   __fastcall TEdit::SetText(char * text)
{  return SendMessage(WM_SETTEXT,0,LPARAM(text)) ? true:false; }

inline bool   __fastcall TEdit::IsMultiline  ()
{ return (GetWindowLong(GWL_STYLE)&ES_MULTILINE) ? true:false;}

inline  DWORD  __fastcall TEdit::GetLinesCount()
{ return SendMessage(EM_GETLINECOUNT,0,0);}

inline  DWORD __fastcall TEdit::GetLineOffset(DWORD line)
{  return SendMessage(EM_LINEINDEX,line,0);}


#ifndef _NO_NAMESPACE
//namespace KeWin
}
#endif

#endif // #define GRKE_WINCTRLS_INC





 