#ifndef _MATH_EQUATION
#define _MATH_EQUATION
/*Решение уравнений с одной переменной*/

/*
 основан на функциональном объекте
 оператор вызова принимает значение аргумента X, и номер производной
 возарващает значение функции F(x) либо dF(x)/dx

 Функциональный объект решения уравнения:
 оператор вызова принимает функциональный объект функции, начало конец отрезка,точность
*/

#include <vector>
using namespace std;

template <typename T>
 T __fastcall abs_value(T v)
 {return v>T(0) ? v : -v ;}

template <typename T,typename Function,typename Method>
int __fastcall equation( Function & f,Method & m, vector<T> & result,
                         T begin,T end,T step = 1,T precision = .000001)
{
  result.resize(0);
  //1) отделение корней

  T next;
  while(begin<end)
  {
   next = begin+step;
 __try
  {
   if(f(begin,0)*f(next,0) <T(0))
   {
      result.push_back(m(f,begin,next,precision));
   }

   }
     __finally{   begin=next;}
  }

  return int(result.size());
}

/* Метод Ньютона */
template <typename T,typename Function>
struct equation_newton
{
 T operator ()(Function & f,T a,T b, T prec );
};

template <typename T,typename Function>
T equation_newton<T,Function>::operator ()(Function & func,T a,T b, T prec )
{
 T x1(a),x2(a*a),res = func(a,0);
 //Выбор начальных условий
 x1 = a-(b-a)*res/(func(b,0)*res);

 res *=x2;
  if (x1 >0 && x2*func(x1,0)<0)
   x1 = a;
   else
   x1 = b;
   while(abs_value(res) > prec && abs_value(x2-x1) > prec)
   {
    x2 = x1;
    x1 = x2 - func(x2,0)/func(x2,1);
    res = func(x1,0);
   }
 return x1;
}

template <typename T,typename Function>
struct equation_iteration
{
 T operator ()(Function & f,T a,T b, T prec );
};

template <typename T,typename Function>
T equation_iteration<T,Function>::operator ()(Function & f,T a,T b, T prec )
{
T Fa = f(a,1),Fb = f(b,1),X;
do{

    if (Fa>Fb)
      {
         a= a-f(a,0)/Fa;
         X = a;
         Fa= f(a,1);
      }
     else
      {
        b= b-f(b,0)/Fb;
        X=b;
        Fb= f(b,1);
      }
    }while (abs_value(f(X,0)) > prec) ;
    return X;
}


#endif


