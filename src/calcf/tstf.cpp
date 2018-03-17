//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "tstf.h"
#include <calcf.h>
#include <stdio.h>

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------
//((x0&0xFFFFFE)<<3)|(x0&1)
void __fastcall TForm1::ReplaceCommas(AnsiString & text)
{
// char * ptr = text.c_str();
// char * end_ptr = ptr+text.Length();
// while(ptr<end_ptr)
// {
//  if(*ptr == ',')
//     *ptr = '.';
//   ++ptr;
// }
}

void __fastcall TForm1::Button1Click(TObject *Sender)
{

  ClearVariables(Calc);
  Results->Lines->Clear();
  long double res;
  //long double vars[3] = {5,5,5};
  //res = CalcFormulaVars(Calc,"x1=mul(x0,x1,x2)",3,vars);

  if(CheckBox1->Checked)
  {
    AnsiString text = Expr->Lines->Text;
    //ReplaceCommas(text);
    res = CalcFormula(Calc,text.c_str());
    SetResult(Calc,res);
  }
  else
  {

    for(int i = 0;i<Expr->Lines->Count;i++)
    {
     AnsiString text = Expr->Lines->Strings[i];
     //ReplaceCommas(text);
     res = CalcFormula(Calc,text.c_str());
     SetResult(Calc,res);
    }
  }
}
//---------------------------------------------------------------------------



void __fastcall TForm1::Button2Click(TObject *Sender)
{

  Caption = "Wait";
  ClearVariables(Calc);
  DWORD old_priority = GetThreadPriority(GetCurrentThread());
  SetThreadPriority(GetCurrentThread(),THREAD_PRIORITY_TIME_CRITICAL);
  DWORD cTick = GetTickCount();
  for(int i = 0;i<10000;i++)
    {
     for(int j = 0;j<Expr->Lines->Count;j++)
        {
         AnsiString text = Expr->Lines->Strings[i];
         CalcFormula(Calc,text.c_str());
        }
    }


  cTick = GetTickCount()-cTick;
  SetThreadPriority(GetCurrentThread(),old_priority);
  Button1Click(this);
  char out[1024];
  Results->Lines->Add(AnsiString("10000 итераций за ") + AnsiString(int(cTick))+ AnsiString(" mS"));

}
//---------------------------------------------------------------------------


//exp(ln(tan(rad(grad(atan(2^log(sqrt(fact(2+2)+exp(1)+sin(rad(90)))^2/(1/3/5*3),2)))))))+exp(ln(tan(rad(grad(atan(2^log(sqrt(fact(2+2)+exp(1)+sin(rad(90)))^2/(1/3/5*3),2)))))))+exp(ln(tan(rad(grad(atan(2^log(sqrt(fact(2+2)+exp(1)+sin(rad(90)))^2/(1/3/5*3),2)))))))/exp(ln(tan(rad(grad(atan(2^log(sqrt(fact(2+2)+exp(1)+sin(rad(90)))^2/(1/3/5*3),2)))))))-exp(ln(tan(rad(grad(atan(2^log(sqrt(fact(2+2)+exp(1)+sin(rad(90)))^2/(1/3/5*3),2)))))))
//grad(asin(sin(rad(exp((pow((1/7-0.00143578661983335224)*10,2)+222*log(345,8.7)-(-1))^(1/(-fact(5))))))))

//sec

static char * errors_text[] =
{
"",
"Отсутствует переменная", //CALC_ERR_VARIABLES   1
"Недостаточно аргументов", //CALC_ERR_NOARGUMENTS 2
"Синтаксическая ошибка", //CALC_ERR_SYNTAX      3
"Ожидается список аргументов ф-ции", //CALC_ERR_MUSTFARGS   4
"Непонятное выражение", //CALC_ERR_EXPRESSION  5
"Деление на 0", //CALC_ERR_ZERODIV     6
"Ошибка FP", //CALC_ERR_FP          7
"Переполнение FP",//CALC_ERR_FPOVERFLOW
};
template <typename v>
inline v __fastcall  min(v a,v b)
{
 return a<b? a:b;
}
template <typename v>
inline v __fastcall  max(v a,v b)
{
 return a>b? a:b;
}

__fastcall TForm1::SetResult(LPVOID calc, long double res)
{
        //TODO: Add your source code here
         char format[16];
         if(HexOut->Checked)
         sprintf(format,"0x%cLX",'%');
         else
         sprintf(format,"%c.%dLf",'%',max(1,_wtoi(Edit1->Text.c_str())));

         AnsiString str;
         int error = GetCalcError(calc);
         if(error)
         {
          str = AnsiString(errors_text[error]);
         }
         else
         {
          if(HexOut->Checked)
          str.sprintf(format,__int64(res));
          else
          str.sprintf(format,res);
         }
         Results->Lines->Add(str);
        return 0;
}
void __fastcall TForm1::FormCreate(TObject *Sender)
{
 Calc = CreateCalculator();
 Application->Icon = Icon;        
}
//---------------------------------------------------------------------------

void __fastcall TForm1::FormDestroy(TObject *Sender)
{
 if(Calc)
  DestroyCalculator(Calc);       
}
//---------------------------------------------------------------------------

