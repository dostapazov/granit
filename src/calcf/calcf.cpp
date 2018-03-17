//Вычислитель формул.

#include <math.h>
#include <ctype.h>
#include <stdlib.h>
#pragma hdrstop

#include "calcf.hpp"
#include <stack>
#ifdef __BORLANDC__
#include <fastmath.h>
#endif




#ifndef __BORLANDC__
//extern "C" long double  __cdecl _atoldbl(const char  *);
//#define _atold(c) _atoldbl(c)
#define _atold(c) atof(c)

#endif

#ifndef _NO_NAMESPACE
//namespace KeRTL
//{
#endif
/*
Формат записи чисел

  шестнадцатеричные
  0x1ABF
  01ABFh

     восьмеричные
     0123777o

       двоичные
       010101b

          десятичные
          156.765
          .643

     */



#define CHECK(condition, except_code) { if(!(condition)) throw _calc_except(*&string(),except_code);}


#pragma warn -8057
        LDOUBLE __fastcall TMathFuncs::Assign  (int n, LDOUBLE * v,LPDWORD  Error)
        {
         //Присваивание 2 аргумента
         //1 - адрес переменной
         //2 - значение;
         CHECK(n == 2 && v!=0,CALC_ERR_NOARGUMENTS);
         LDOUBLE * vptr = (LDOUBLE*)DWORD(*v);
         *vptr = *(v+1);
         return *vptr;
        }

     LDOUBLE __fastcall TMathFuncs::Add(int n,LDOUBLE * v,LPDWORD Error)
     {
          CHECK(n >0  && v!=0,CALC_ERR_NOARGUMENTS);
          LDOUBLE result = .0;
          for(int i = 0;i<n;i++)
             result+=*v,v++;
          return  result;
     }

     LDOUBLE __fastcall TMathFuncs::Sub(int n,LDOUBLE * v,LPDWORD Error)
     {
          CHECK(n == 2 && v!=0,CALC_ERR_NOARGUMENTS);
          long double o1 = (long double)(*v),o2 =(long double)(v[1]);
          o1-=o2;
          LDOUBLE ret = (LDOUBLE)o1;
          return ret;
     }

     LDOUBLE __fastcall TMathFuncs::Mul(int n,LDOUBLE * v,LPDWORD Error)
     {
          CHECK(n > 0 && v!=0,CALC_ERR_NOARGUMENTS);
          LDOUBLE result = 1.0;
          for(int i = 0;i<n;i++)
             result*=(*v),v++;
          return result;
     }

    LDOUBLE __fastcall TMathFuncs::pi      (int n, LDOUBLE * v,LPDWORD Error)
    {
          CHECK(n >= 0 && v!=0,CALC_ERR_NOARGUMENTS);
          LDOUBLE result = M_PI;
          for(int i = 0;i<n;i++)
             result*=(*v),v++;
          return result;
    }


     LDOUBLE __fastcall TMathFuncs::Div(int n,LDOUBLE * v,LPDWORD Error)
     {
          CHECK(n == 2 && v!=0,CALC_ERR_NOARGUMENTS);
          CHECK(*(v+1) != 0,CALC_ERR_ZERODIV);
          return *v / (*(v+1));

     }

     LDOUBLE __fastcall TMathFuncs::Remind  (int n, LDOUBLE * v,LPDWORD Error)
     {
       CHECK(n == 2 && v!=0,CALC_ERR_NOARGUMENTS);
       int v1 = v[0];
       int v2 = v[1];
       v1%=v2;
       return  (LDOUBLE)v1;
     }

    LDOUBLE __fastcall TMathFuncs::Equ   (int n, LDOUBLE * v,LPDWORD Error)
    {
          CHECK(n == 2 ,CALC_ERR_NOARGUMENTS);
          return LDOUBLE((*v == *(v+1)) ? 1:0);
    }

    LDOUBLE __fastcall TMathFuncs::NotEqu   (int n, LDOUBLE * v,LPDWORD Error)
    {
          CHECK(n == 2 ,CALC_ERR_NOARGUMENTS);
          return LDOUBLE((*v == *(v+1)) ? 1:0);
    }

    LDOUBLE __fastcall TMathFuncs::Less  (int n, LDOUBLE * v,LPDWORD Error)
    {
          CHECK(n == 2 ,CALC_ERR_NOARGUMENTS);
          return LDOUBLE((*v < *(v+1)) ? 1:0);
    }
    LDOUBLE __fastcall TMathFuncs::Above (int n, LDOUBLE * v,LPDWORD Error)
    {
          CHECK(n == 2 ,CALC_ERR_NOARGUMENTS);
          return LDOUBLE((*v > *(v+1)) ? 1:0);
    }

    LDOUBLE __fastcall TMathFuncs::LessEqu  (int n, LDOUBLE * v,LPDWORD Error)
    {
          CHECK(n == 2 ,CALC_ERR_NOARGUMENTS);
          return LDOUBLE((*v <= *(v+1)) ? 1:0);
    }

    LDOUBLE __fastcall TMathFuncs::AboveEqu (int n, LDOUBLE * v,LPDWORD Error)
    {
          CHECK(n == 2 ,CALC_ERR_NOARGUMENTS);
          return LDOUBLE((*v >= *(v+1)) ? 1:0);
    }

   LDOUBLE __fastcall TMathFuncs::LShift  (int n, LDOUBLE * v,LPDWORD Error)
   {
    CHECK(n == 2 ,CALC_ERR_NOARGUMENTS);
    __int64 p1  = (__int64)v[0];
    DWORD shift = (DWORD)  v[1];
    return LDOUBLE(p1<<shift);
   }
   LDOUBLE __fastcall TMathFuncs::RShift  (int n, LDOUBLE * v,LPDWORD Error)
   {
    CHECK(n == 2 ,CALC_ERR_NOARGUMENTS);
    __int64 p1  = (__int64)v[0];
    DWORD shift = (DWORD)  v[1];
    return LDOUBLE(p1>>shift);
   }



#ifdef __BORLANDC__
#pragma warn -8008
#endif

     LDOUBLE __fastcall TMathFuncs::Pow(int n,LDOUBLE * v,LPDWORD Error)
     {
          LDOUBLE ret = 1;
          CHECK(n == 2 && v!=0,CALC_ERR_NOARGUMENTS);
          try{
              ret = pow(*v,*(v+1));
             }
          catch(...)
          {
               CHECK(0,CALC_ERR_FP);
          }

          return ret; 
     }


#ifdef __BORLANDC__
#pragma warn .8008
#endif

     LDOUBLE __fastcall TMathFuncs::Sqrt(int n,LDOUBLE * v,LPDWORD Error)
     {
          CHECK(n == 1 && v!=0,CALC_ERR_NOARGUMENTS);
          return powl(*v,0.5);
     }

     LDOUBLE __fastcall TMathFuncs::Lg(int n,LDOUBLE * v,LPDWORD Error)
     {
          CHECK(n == 1 && v!=0,CALC_ERR_NOARGUMENTS);
          return log10l(*v);
     }

     LDOUBLE __fastcall TMathFuncs::Ln(int n,LDOUBLE * v,LPDWORD Error)
     {
          CHECK(n == 1 && v!=0,CALC_ERR_NOARGUMENTS);
          return logl(*v);
     }

     LDOUBLE __fastcall TMathFuncs::Log(int n,LDOUBLE * v,LPDWORD Error)
     {
          CHECK(n == 2 && v!=0,CALC_ERR_NOARGUMENTS);
          return log(*v)/log(*(v+1));
     }

     LDOUBLE __fastcall TMathFuncs::Exp(int n,LDOUBLE * v,LPDWORD Error)
     {
          CHECK(n == 1 && v!=0,CALC_ERR_NOARGUMENTS);
          return expl(*v);
     }

     LDOUBLE __fastcall TMathFuncs::Cos(int n,LDOUBLE * v,LPDWORD Error)
     {
          CHECK(n == 1 && v!=0,CALC_ERR_NOARGUMENTS);
          return cosl(*v);
     }

     LDOUBLE __fastcall TMathFuncs::Cosh(int n,LDOUBLE * v,LPDWORD Error)
     {
          CHECK(n == 1 && v!=0,CALC_ERR_NOARGUMENTS);
          return coshl(*v);
     }

     LDOUBLE __fastcall TMathFuncs::ACos(int n,LDOUBLE * v,LPDWORD Error)
     {
          CHECK(n == 1 && v!=0,CALC_ERR_NOARGUMENTS);
          return acosl(*v);
     }

     LDOUBLE __fastcall TMathFuncs::Sin(int n,LDOUBLE * v,LPDWORD Error)
     {
          CHECK(n == 1 && v!=0,CALC_ERR_NOARGUMENTS);
          return   sinl(*v);

     }

     LDOUBLE __fastcall TMathFuncs::Sinh(int n,LDOUBLE * v,LPDWORD Error)
     {
          CHECK(n == 1 && v!=0,CALC_ERR_NOARGUMENTS);
          return sinhl(*v);
     }

     LDOUBLE __fastcall TMathFuncs::ASin(int n,LDOUBLE * v,LPDWORD Error)
     {
          CHECK(n == 1 && v!=0,CALC_ERR_NOARGUMENTS);
          return asinl(*v);
     }

     LDOUBLE __fastcall TMathFuncs::Tan(int n,LDOUBLE * v,LPDWORD Error)
     {
          CHECK(n == 1 && v!=0,CALC_ERR_NOARGUMENTS);
          return tanl(*v);
     }

     LDOUBLE __fastcall TMathFuncs::ATan(int n,LDOUBLE * v,LPDWORD Error)
     {
          CHECK(n == 1 && v!=0,CALC_ERR_NOARGUMENTS);
          return atanl(*v);
     }

     LDOUBLE __fastcall TMathFuncs::Ctg(int n,LDOUBLE * v,LPDWORD Error)
     {
          CHECK(n == 1 && v!=0,CALC_ERR_NOARGUMENTS);
          return (1.0)/tanl(*v);
     };


     LDOUBLE __fastcall TMathFuncs::Round(int n,LDOUBLE * v,LPDWORD Error)
     {
          CHECK(n == 2 && v!=0,CALC_ERR_NOARGUMENTS);
          //Округление до знака
          __int64 a1,a2;
          int c_sign =*(v+1);
          if(c_sign>=0)
          {
               LDOUBLE mul = ::pow(10.0,c_sign);
               a1 = (*v) * mul*10;
               a2 = (*v) * mul;
               a2*=10;
               if(abs(a1-a2 )>= 5)
                    a2+=10;
               return LDOUBLE(a2)/(mul*10);

          }
          return *v;
     };

     LDOUBLE __fastcall TMathFuncs::Grad(int n,LDOUBLE * v,LPDWORD Error)
     {
          CHECK(n == 1 && v!=0,CALC_ERR_NOARGUMENTS);
          return *(v)*180.0/M_PI;
     }

     LDOUBLE __fastcall TMathFuncs::Rad(int n,LDOUBLE * v,LPDWORD Error)
     {
          CHECK(n == 1 && v!=0,CALC_ERR_NOARGUMENTS);
          return (*v)*M_PI/180.0;
     }


     LDOUBLE  _fact(int top,LPDWORD x);

	 LDOUBLE __fastcall TMathFuncs::Fact(int n, LDOUBLE * v,LPDWORD Error)
     {
          CHECK(n == 1 && v!=0,CALC_ERR_NOARGUMENTS);
          DWORD x;
          LDOUBLE ret =  _fact((int)*v,&x);
          CHECK(x,CALC_ERR_FPOVERFLOW);
		  return ret;


     }



     LDOUBLE __fastcall TMathFuncs::Abs(int n, LDOUBLE * v,LPDWORD Error)
     {
          CHECK(n == 1 && v!=0,CALC_ERR_NOARGUMENTS);
          return (*v < 0) ? -(*v): *v;
     }

     LDOUBLE __fastcall TMathFuncs::B_And(int n, LDOUBLE * v,LPDWORD Error)
     {
          CHECK(n == 2 && v!=0,CALC_ERR_NOARGUMENTS);
          //Двоичное "И"
          return LDOUBLE( __int64(*v) & __int64(*(v+1)));
     }

     LDOUBLE __fastcall TMathFuncs::L_And(int n, LDOUBLE * v,LPDWORD Error)
     {
          CHECK(n == 2 && v!=0,CALC_ERR_NOARGUMENTS);
          return LDOUBLE( (__int64(*v) && __int64(*(v+1))));
     }

     LDOUBLE __fastcall TMathFuncs::B_Or(int n, LDOUBLE * v,LPDWORD Error)
     {
          CHECK(n == 2 && v!=0,CALC_ERR_NOARGUMENTS);
          return LDOUBLE( __int64(*v) | __int64(*(v+1)));
     }

     LDOUBLE __fastcall TMathFuncs::L_Or(int n, LDOUBLE * v,LPDWORD Error)
     {
          CHECK(n == 2 && v!=0,CALC_ERR_NOARGUMENTS);
          return LDOUBLE( (__int64(*v) || __int64(*(v+1))));
     }

     LDOUBLE __fastcall TMathFuncs::B_Not(int n, LDOUBLE * v,LPDWORD Error)
     {
          CHECK(n == 1 && v!=0,CALC_ERR_NOARGUMENTS);
          return LDOUBLE( ~(__int64(*v)) );
     }

     LDOUBLE __fastcall TMathFuncs::L_Not(int n, LDOUBLE * v,LPDWORD Error)
     {
          CHECK(n == 1 && v!=0,CALC_ERR_NOARGUMENTS);
          return LDOUBLE( ( __int64(*v) ? 0 : 1) );
     }

     LDOUBLE __fastcall TMathFuncs::B_Xor(int n, LDOUBLE * v,LPDWORD Error)
     {
          CHECK(n == 2 && v!=0,CALC_ERR_NOARGUMENTS);
          return LDOUBLE( __int64(*v) ^ __int64(*(v+1)));
     }

#pragma warn .8057



     bool __fastcall TMathFuncs::GetFunc(const char * str,math_func & mf)
     {
          iterator  i = find( string(str));
          if(i!=end())
          {
               mf = i->second;
               return true;
          }
          return false;
     }


     TMathFuncs TCalculator::math_funcs;

     void __fastcall TCalculator::SkipBlanks(string::iterator & i)
     {
          while(*i==' ' || *i == '\t' || *i == '\n' || *i == '\r')
               i++;
     }


     bool __fastcall is_hex_numstr(string::iterator  i)
     {
      bool  ret (false);
      while(*i && !ret)
       {ret = (*i == 'x' || *i == 'h');i++;};
      return ret;
     }

     string __fastcall TCalculator::GetDigitStr(string::iterator & i)
     {
          string s;
          bool  do_break (false);
          bool  hex_num (is_hex_numstr(i));
          while (!do_break)
          {
               char c = *i;
               switch(c)
               {
               case '.':
                    s+='.';break;
               case 'h':
                    s = string("0x")+s;
                    do_break = true;
                    i++;
                    break;

               case 'b':
                 s+=c;
                 if(!hex_num)
                 {i++;do_break = true;}
                 break;
               case 'o':
                    s+=c;
                    {i++;do_break = true;}
                    break;
               case 'x':
                    hex_num = true;
                    s+=c;
                    break;
               default :
                    if(isxdigit(c))
                         s+=c;
                    else
                         do_break = true;
               }
               if(!do_break)
                    i++;

          }

          return s;
     }



     string __fastcall TCalculator::GetSubString(string::iterator & i)
     {
          SkipBlanks(i);
          if(isdigit(*i) || *i == 'x')
        {
               return GetDigitStr(i);
        }

          string str;

          while(*i)
          {

               switch(*i)
               {
               case '+':
               case '-':
               case '/':
               case '*':
               case '^':
               case '=':
               case '|':
               case '&':
               case '>':
               case '<':
               case '!':
               case '%':

                    if(str.size() == 0)
                         str+=(*i++);

                    {
                         char c = *i;
                         if( c == '=' || c == '|' ||  c == '&' || c == '<' || c == '>')
                              str+=(*i++);
                    }
                    return str;
               case ' ':
                    i++;
                    break;
               case '(':
               case ')':
               case 0  :
               case ',':
                    return str;
               default:
                    str+=*i++;
               }

          }

          return str;
     }


     LDOUBLE __fastcall TCalculator::HexNumeric  (string & s)
     {
          __int64 res = 0;
          string::iterator i = s.begin();
          do{
               char  c = toupper(*i++);
               res<<=4;
               res|= (c <'A') ? int(c-'0') : int(c-'A'+10);
          }while(i<s.end());
          return LDOUBLE(res);
     }

     LDOUBLE __fastcall TCalculator::OctalNumeric(string & s)
     {
          __int64 res = 0;
          string::iterator i = s.begin();
          do{
               char  c = *i++;
               res<<=3;
               res|= int(c-'0');
          }while(isdigit(*i));

          return LDOUBLE(res);
     }

     LDOUBLE __fastcall TCalculator::BinNumeric  (string & s)
     {
          __int64 res = 0;
          string::iterator i = s.begin();
          do{
               char  c = *i++;
               res<<=1;
               c-='0';
               CHECK((c==1 || c==0),CALC_ERR_SYNTAX);
               res|= int(c);
          }while(isdigit(*i));

          return LDOUBLE(res);
     }

     LDOUBLE __fastcall TCalculator::StringToNumeric(string &str,DWORD var_count,LDOUBLE * var_array)
     {
          string::iterator i = str.begin();
          if(*i == '0')
          {
               i++;

               if(*i == 'x')
               {
                    i++;
                    string _s(i,str.end());
                    return HexNumeric(_s);
               }
               else
               {
                    i = str.end();
                    i--;
                    switch(*i)
                    {
                    case 'h':
                         return HexNumeric(str);
                    case 'o':
                         return OctalNumeric(str);
                    case 'b':
                         return BinNumeric(str);


                    default :return _atold(str.c_str());

                    }
               }
          }
          else
          {
               if(*i == 'x')
               {
                       i++;
                       int curr_variable = atoi(&*i);
                       i++;
                       return GetVariable(curr_variable ,var_count,var_array);
               }
               else
                    return _atold(str.c_str());
          }
#ifndef __BORLANDC__
          return 0;
#endif
     }

     LDOUBLE __fastcall TCalculator::ExecuteMathFunc(math_func & mf,d_vect & arguments,string::iterator & i,DWORD var_count,LDOUBLE * var_array)
     {
          if(mf.is_operator==false)
          {
               //функция
               SkipBlanks(i);
               if(*i == '(' )
               {
                    //for(int j = 0;j<mf.n_args;)
                    while(*i == ',' ||  !IsTerminateCalc(i))
                    {
                         i++;
                         if(!IsTerminateCalc(i))
                         {
                         arguments.push_back(DoCalc(i,0,var_count,var_array));
                         SkipBlanks(i);
                         //if((++j)<mf.n_args && ((*i)!=','))
                         //throw _calc_except(*&string(i),CALC_ERR_SYNTAX);//Должны быть ещё аргументы
                         }
                    }

                    if((*i) == ')')
                         i++;
                    else
                         throw _calc_except(*&string(&*i),CALC_ERR_SYNTAX); //Слишком много аргументов
               }
               else
               {
                    throw _calc_except(*&string(&*i),CALC_ERR_MUSTFARGS);
               }

          }

          return ExecuteOp(mf,arguments);
     }

     LDOUBLE __fastcall TCalculator::ExecuteOp(math_func & mf,d_vect & arguments)
     {

          int arg_count = arguments.size();
          if(mf.n_args>0)
             arg_count = mf.n_args;
          if(arg_count== mf.n_args || mf.n_args<0)
          {
               d_vect parg;
               //parg.resize(arg_count);
               for(int i = 0;i< arg_count;i++)
               {
                    parg.insert(parg.begin(),arguments.at(arguments.size()-1));
                    arguments.pop_back();
                    //copy(arguments.begin(),arguments.end(),parg.begin());
               }
               return mf.mf(arg_count,&*parg.begin(),0);
          }
          else
               throw _calc_except(*&string(""),CALC_ERR_NOARGUMENTS);
#ifndef __BORLANDC__
          return 0;
#endif
     }

     bool   __fastcall TCalculator::IsTerminateCalc(string::iterator &i)
     {
          char c = *i;
          switch(c)
          {
          case ';':
          case '\n':
          case '\r':
                        i++;
          case ')':
          case ',':
          case 0:
               return true;
          }
          return false;
     }

     void __fastcall TCalculator::clear_ops()
     {
      /*while(!operators.empty())
             operators.pop();
      */       

     }

/*
x0=10;x1=2;
x1+x2;
log(pow(10,2),10)
*/

     LDOUBLE  TCalculator::DoCalc(string::iterator & formula,d_vect * args,DWORD var_count,LDOUBLE * var_array)
     {
          CalcState = 0;
          math_func mf;
          d_vect  arguments ;
          stack<math_func> operators;
          int curr_variable = -1;

          //arguments.push_back(0);
          string::iterator &i = formula;

          try{
                        string sub_str;
                        LDOUBLE ret(0);

               while(!IsTerminateCalc(i))
               {
                    sub_str = GetSubString(i);
                    //SkipBlanks(i);
                    if(sub_str.empty()  )
                    {
                         if(*i == '(')
                         {
                              //Вычисление скобки
                              i++;
                              arguments.push_back(DoCalc(i,args,var_count,var_array));
                              i++;
                         }
                    }
                    else
                    {
                     if(math_funcs.GetFunc(sub_str.c_str(),mf))
                     {
                        if(mf.is_operator)
                        {
                             if(!arguments.size())
                                 arguments.push_back(0);
                             while(operators.size())
                             {
                                  math_func _mf = operators.top();
                                  if(_mf.priority<mf.priority)
                                       break;
                                  arguments.push_back(this->ExecuteMathFunc(_mf,arguments,i,var_count,var_array));
                                  operators.pop();
                             }
                             operators.push(mf);
                        }
                        else
                          arguments.push_back(ExecuteMathFunc(mf,arguments,i,var_count,var_array));
                     }
                     else
                     {
                          if(IsNumeric(sub_str))
                          {
                               if(*i=='=' && *sub_str.begin() == 'x')
                                 {
                                  curr_variable = atoi((sub_str.c_str()+1));
                                  ret = DoCalc(++i,0,var_count,var_array);
                                  SetVariable(curr_variable,ret,var_count,var_array);
                                 }
                                 else
                                {
                                ret = StringToNumeric(sub_str,var_count,var_array);
                                arguments.push_back(ret);
                                }
                          }
                          else
                             {
                               if(sub_str == "=" && curr_variable >=0)
                               {
                                 //Оператор присваивания
                                int c_var = curr_variable;
                                    curr_variable = -1;
                                    ret = DoCalc(i,&arguments,var_count,var_array);
                                    arguments.push_back(ret);
                                    SetVariable(c_var,ret,var_count,var_array);
                               }
                               else
                               throw _calc_except(sub_str,CALC_ERR_SYNTAX);
                             }
                     }

                         sub_str.erase(0,sub_str.size());
                         //i++;
                    }
               }
               while(operators.size())
               {
                    math_func _mf = operators.top();
                    string::iterator* i = 0;
                    arguments.push_back(this->ExecuteMathFunc(_mf,arguments,*i,var_count,var_array));
                    operators.pop();
               }

               if(arguments.size())
                    ret = arguments.at(arguments.size()-1);


               return  ret;
          }
          catch(_calc_except & ce)
          {
               this->ErrorString = ce.error_str;
               this->ErrorCode   = ce.error_code;
               throw int(ce.error_code);
          }
         #ifndef __BORLANDC__
         return 0;
         #endif
     }

     LDOUBLE __fastcall TCalculator::Calc(const char * formula,DWORD var_count,LDOUBLE * var_array)
     {
          {
               ErrorCode = 0;
               ErrorString.erase(0,ErrorString.size());
               try{
                    string str;
					while (*formula)//Копирование строки с удалением пробелов;
					{
						if(*formula !=' ')
							str+=*formula;
						formula++;
					}
                   string::iterator i = str.begin();
                   clear_args();
                   clear_ops();
				   ErrorCode = 0;//Очистьть код ошибки;

                    CharLowerBuff((char*)str.c_str(),str.size());
                                LDOUBLE ret ;
                                do{
                                   ret = DoCalc(i,0,var_count,var_array);
                                   if(*i == ';')
                                    i++;
                                  }while(*i && ErrorCode==0);
                                  return ret;

               }
               catch (_calc_except & )
               {
               }
               catch(...)
               {
               }
          }
        return 0;
     }

     bool   __fastcall  TCalculator::IsNumeric(string & str)
     {
          string::iterator i = str.begin();
          while(i<str.end())
          {
               char  c = *i++;
               if(!isxdigit(c) && !(  c== '.' || c=='h'|| c=='x' || c=='b' || c=='o' || !this->IsTerminateCalc(i) ))
                    return false;
          }
          return true;

     }

     void   __fastcall TCalculator::ClearVariables()
     {
          pd_vect::reverse_iterator i = variables.rbegin(),e = variables.rend();
          while(i<e)
          {
            LDOUBLE * vptr = *i;
            if(vptr)
              delete vptr;
             i++;
          }
          variables.erase(variables.begin(),variables.end());
     }

     int   __fastcall TCalculator::AddVariable(LDOUBLE x)
     {
          LDOUBLE * var = new LDOUBLE;
          *var = x;
          variables.push_back(var);
          return variables.size()-1;
     }
                                           
     int   __fastcall TCalculator::AddVariable(char *str)
     {
          //TCalculator calc;
          try{

              LDOUBLE var = this->Calc(str,0,0);
               return     AddVariable(var);
        }

          catch(...)
          {
               return -1;
          }
     }

   bool   __fastcall TCalculator::SetVariable(int idx,LDOUBLE  x,DWORD var_count,LDOUBLE * var_array)
   {
     if(idx>=0)
      {
        if(var_array && idx<(int)var_count)
           {
            var_array[idx]= x;
            return true;
           }

           if(idx<int(variables.size()))
           {
                  LDOUBLE * var_ptr = *(variables.begin()+idx);
                if(var_ptr == NULL)
                   {
                                  var_ptr = new LDOUBLE;
                                 *(variables.begin()+idx) = var_ptr;
                            }
                *(var_ptr)=x;

                return true;
           }
           else
           {
                variables.resize(idx+1);
                         return SetVariable(idx,x,0,0);
           }

      }
      return false;
   }

   LDOUBLE __fastcall  TCalculator::GetVariable(int index,DWORD var_count,LDOUBLE * var_array)
   {

    LDOUBLE * var_ptr = NULL;
    if(index<(int)var_count && var_array)
       var_ptr = var_array+index;  
    if(index<int(variables.size()))
       var_ptr = variables[index];
    if(var_ptr)
      return *var_ptr;
    else
    throw _calc_except(*&string("expect variable"),CALC_ERR_VARIABLES);

   }

   bool   __fastcall TCalculator::SetVariable(int idx,const char * str,DWORD var_count,LDOUBLE * var_array)
   {

        try{
             return SetVariable(idx,Calc(str,var_count,var_array),var_count,var_array);
        }
        catch(...)
        {
         return false;
        }
   }


#ifndef _NO_NAMESPACE
//}
#endif


