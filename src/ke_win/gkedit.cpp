/***************************************************************
 *
 * Модуль    : kertls.lib
 * Описание  : Edit control
 * Автор     :Остапенко Д.В.
 * Версия    :1
 * Создан    :10.01.2002 12:01:54
 *
 ****************************************************************/


#include <gkwctrls.hpp>

#ifndef _NO_NAMESPACE
namespace KeWin{
#endif

//winuser.h



  bool    __fastcall TEdit::DoCreate(HWND parent,TRect & r,int id,DWORD style,DWORD ex_style)
  {

   style |=(WS_CHILD|WS_VISIBLE);
   if(!(style&(ES_RIGHT|ES_CENTER)))
        style|=ES_LEFT;

   Create(parent,0,r,id,0,
          style,
          ex_style,L"EDIT");
   UD = 0;
   return hWnd ? true:false;
  }

  bool    __fastcall TEdit::DoCreate(HWND parent,int id)
  {
    TWinControl::DoCreate(parent,id);
    return hWnd ? true:false;
  }

  LRESULT     __fastcall TEdit::MessageHandler(MSG &msg)
  {
      return TWinControl::MessageHandler(msg);
  }

  DWORD  __fastcall TEdit::GetLineLength(DWORD line_number)
  {
   if(line_number< this->GetLinesCount())
   {
    DWORD  offset = GetLineOffset(line_number);
    return SendMessage(EM_LINELENGTH,offset,0);
   }
   return 0;
  }

  DWORD  __fastcall TEdit::GetLineText(DWORD line_number, char * text,int text_sz)
  {
    if(GetLineLength(line_number)<DWORD(text_sz))
    {
     DWORD len = SendMessage(EM_GETLINE,line_number,(LPARAM)text);
     text[len]=0;
    }
    return 0;
  }




#ifndef _NO_NAMESPACE
}//namespace KeWin{
#endif



 
