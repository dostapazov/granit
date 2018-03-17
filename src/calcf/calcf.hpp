//Вычисление значения формулы

#ifndef _CALCF_HPP_
#define _CALCF_HPP_

#include <kertl.hpp>

#include <map>
#include <vector>
#include <string>

#ifndef _CALCF_H_
#include <calcf.h> 
#endif


#ifndef M_PI
#define M_PI        3.14159265358979323846
#endif

using namespace std;

 typedef vector<LDOUBLE>  d_vect;
 typedef vector<LDOUBLE*> pd_vect;


 typedef LDOUBLE (__fastcall * MFP)(int n_args,LDOUBLE * args,LPDWORD Error);

 struct  math_func
 {
   int  priority;     //Приоритет операции
   int  n_args;       //Требуемое кол-во аргументов
   bool is_operator;  // оператор ли ?
   MFP mf;            //адрес процедуры обработки
   math_func(){ZeroMemory(this,sizeof(*this));};
   math_func(int p,int a,bool is_op,MFP _mfp):priority(p),n_args(a),is_operator(is_op),mf(_mfp){};
   math_func(const math_func & m){ *this = m;}
   math_func & operator = (const math_func & m){priority = m.priority;n_args = m.n_args;mf = m.mf;is_operator = m.is_operator; return *this;}
 };

 inline int operator < (const math_func & m1,const math_func & m2)
 {return m1.priority < m2.priority;}

 inline int operator == (const math_func & m1,const math_func & m2)
 {return m1.mf == m2.mf;}



 class TMathFuncs:public map<string,math_func>
 {
   public:
   static LDOUBLE __fastcall Add  (int n,LDOUBLE * v,LPDWORD Error);
   static LDOUBLE __fastcall Sub  (int n,LDOUBLE * v,LPDWORD Error);
   static LDOUBLE __fastcall Mul  (int n,LDOUBLE * v,LPDWORD Error);
   static LDOUBLE __fastcall Div  (int n,LDOUBLE * v,LPDWORD Error);
   static LDOUBLE __fastcall Pow  (int n,LDOUBLE * v,LPDWORD Error);
   static LDOUBLE __fastcall Sqrt (int n,LDOUBLE * v,LPDWORD Error);
   static LDOUBLE __fastcall Lg   (int n,LDOUBLE * v,LPDWORD Error);
   static LDOUBLE __fastcall Ln   (int n,LDOUBLE * v,LPDWORD Error);
   static LDOUBLE __fastcall Log  (int n,LDOUBLE * v,LPDWORD Error);
   static LDOUBLE __fastcall Exp  (int n,LDOUBLE * v,LPDWORD Error);
   static LDOUBLE __fastcall Cosh (int n,LDOUBLE * v,LPDWORD Error);
   static LDOUBLE __fastcall Cos  (int n,LDOUBLE * v,LPDWORD Error);
   static LDOUBLE __fastcall ACos (int n,LDOUBLE * v,LPDWORD Error);
   static LDOUBLE __fastcall Sinh (int n,LDOUBLE * v,LPDWORD Error);
   static LDOUBLE __fastcall Sin  (int n,LDOUBLE * v,LPDWORD Error);
   static LDOUBLE __fastcall ASin (int n,LDOUBLE * v,LPDWORD Error);
   static LDOUBLE __fastcall Tan  (int n,LDOUBLE * v,LPDWORD Error);
   static LDOUBLE __fastcall ATan (int n,LDOUBLE * v,LPDWORD Error);
   static LDOUBLE __fastcall Ctg  (int n,LDOUBLE * v,LPDWORD Error);
   static LDOUBLE __fastcall Round(int n,LDOUBLE * v,LPDWORD Error);
   static LDOUBLE __fastcall Grad (int n,LDOUBLE * v,LPDWORD Error);
   static LDOUBLE __fastcall Rad  (int n,LDOUBLE * v,LPDWORD Error);
   static LDOUBLE __fastcall Fact (int n,LDOUBLE * v,LPDWORD Error);
   static LDOUBLE __fastcall Abs  (int n, LDOUBLE * v,LPDWORD Error);

   static LDOUBLE __fastcall B_And(int n, LDOUBLE * v,LPDWORD Error);
   static LDOUBLE __fastcall L_And(int n, LDOUBLE * v,LPDWORD Error);

   static LDOUBLE __fastcall B_Or(int n, LDOUBLE * v,LPDWORD Error);
   static LDOUBLE __fastcall L_Or(int n, LDOUBLE * v,LPDWORD Error);

   static LDOUBLE __fastcall B_Not(int n, LDOUBLE * v,LPDWORD Error);
   static LDOUBLE __fastcall L_Not(int n, LDOUBLE * v,LPDWORD Error);

   static LDOUBLE __fastcall B_Xor(int n, LDOUBLE * v,LPDWORD Error);

   static LDOUBLE __fastcall Equ     (int n, LDOUBLE * v,LPDWORD Error);
   static LDOUBLE __fastcall NotEqu  (int n, LDOUBLE * v,LPDWORD Error);
   static LDOUBLE __fastcall Less    (int n, LDOUBLE * v,LPDWORD Error);
   static LDOUBLE __fastcall Above   (int n, LDOUBLE * v,LPDWORD Error);
   static LDOUBLE __fastcall LessEqu (int n, LDOUBLE * v,LPDWORD Error);
   static LDOUBLE __fastcall AboveEqu(int n, LDOUBLE * v,LPDWORD Error);
   static LDOUBLE __fastcall Assign  (int n, LDOUBLE * v,LPDWORD Error);
   static LDOUBLE __fastcall LShift  (int n, LDOUBLE * v,LPDWORD Error);
   static LDOUBLE __fastcall RShift  (int n, LDOUBLE * v,LPDWORD Error);
   static LDOUBLE __fastcall Remind  (int n, LDOUBLE * v,LPDWORD Error);
   static LDOUBLE __fastcall Sum     (int n, LDOUBLE * v,LPDWORD Error);
   static LDOUBLE __fastcall pi      (int n, LDOUBLE * v,LPDWORD Error);


   TMathFuncs();
   bool __fastcall GetFunc(const char * str,math_func & mf);
   bool __fastcall IsMinus(math_func & m);
 };


 struct _calc_except
 {
  string error_str;
  string error_text;
  DWORD  error_code;
  _calc_except(string & err_str,DWORD err_code,string *text=0):error_str(err_str),error_code(err_code){
   if(text)
     this->error_text=*text;}
 };

 #define SECRET_DWORD 0x674C6567
 KERTL_CLASS TCalculator
 {
  DWORD            Secret;
  string           ErrorString;
  DWORD            ErrorCode;

  static TMathFuncs math_funcs;
  static string __fastcall GetDigitStr    (string::iterator &i);
  static void   __fastcall SkipBlanks     (string::iterator &i );
         LDOUBLE __fastcall HexNumeric  (string &s);
         LDOUBLE __fastcall OctalNumeric(string &s);
         LDOUBLE __fastcall BinNumeric  (string &s);
         LDOUBLE __fastcall StringToNumeric(string &s,DWORD var_count,LDOUBLE * var_array);
         LDOUBLE __fastcall ExecuteMathFunc(math_func &mf,d_vect & arguments,string::iterator & i,DWORD var_count,LDOUBLE * var_array);
         LDOUBLE __fastcall ExecuteOp      (math_func &mf,d_vect & arguments);
         string __fastcall  GetSubString   (string::iterator &i);
         bool   __fastcall  IsTerminateCalc(string::iterator &i);
         bool   __fastcall  IsNumeric(string &str);
         LDOUBLE            DoCalc(string::iterator &formula,d_vect *args,DWORD var_count,LDOUBLE * var_array);
  pd_vect  variables;
  DWORD    CalcState;
  void __fastcall clear_args(){/*arguments.erase(arguments.begin(),arguments.end());*/};
  void __fastcall clear_ops();

  public:
   TCalculator(){Secret = SECRET_DWORD;};
   ~TCalculator(){ClearVariables();clear_args();clear_ops();}
   LDOUBLE __fastcall  Calc(const char * formula,DWORD var_count,LDOUBLE * var_array);
   int     __fastcall  AddVariable(LDOUBLE x);
   int     __fastcall  AddVariable( char *);
   bool    __fastcall  SetVariable(int idx,LDOUBLE  x,DWORD var_count,LDOUBLE * var_array);
   bool    __fastcall  SetVariable(int idx,const char * str,DWORD var_count,LDOUBLE * var_array);
   DWORD   __fastcall  GetError(){return ErrorCode;}
   void    __fastcall  ClearVariables();
   LDOUBLE __fastcall  GetVariable(int index,DWORD var_count,LDOUBLE * var_array);
   bool   __fastcall   IsValid(){return (!IsBadReadPtr(this,sizeof(*this)) && Secret == SECRET_DWORD) ? true:false;}
 };




 inline  bool   __fastcall TMathFuncs::IsMinus(math_func & m)
 { return (m.mf == Sub) ? true:false;}


#endif
