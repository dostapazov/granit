/*
 Решение СЛАУ методом Зейделя
*/
#ifndef _M_ZEIDEL_HPP
#define _M_ZEIDEL_HPP

#ifndef _MATRIX_HPP_INC
#include "matrix.hpp"
#endif


template <class T, class alloc>
T __fastcall zeidel_value( const matrix<T,alloc> & C,const matrix<T,alloc> & X,int row,int col ,int col_to)
{
  T result(0);
  T dv = C(row,row);
  for(;col<col_to;col++)
    result+=X(col,0)*C(row,col)/dv;
  return result;
}

template <class T, class alloc>
matrix<T,alloc> __fastcall slau_zeidel( matrix<T,alloc> A,matrix<T,alloc> B, T prec = T(0.001),int * iter_count =0 )
{
 matrix<T,alloc> X;
 if(iter_count)
   * iter_count = 0;
 if(A.rows() == A.columns() && B.rows() == A.rows() && B.columns()==1)
 {
  matrix<T,alloc> At = A.transp(),X1,C;
  int i;
  C = At*A;
  X = X1 = At*B;
  B = X;
  
  for(i = 0;i<A.rows();i++)
    B(i,0)/=C(i,i);

  do{
    X=X1;
    for(i = 0;i< A.rows();i++)
    {
     X1(i,0) =B(i,0)-(zeidel_value(C,X1,i,0,i)+zeidel_value(C,X,i,i+1,A.rows()));
    }
    X-=X1;
   if(iter_count)
     (* iter_count)++ ;
    }while(!(X.sum_of_column(0,true)< T(prec)));
    X = X1;
 }
 return X;
}

template <class T,class alloc _RWSTD_COMPLEX_DEFAULT(std::allocator<T>)>
struct    zeidel_method:public unary_function<T,T>
{
  T prec;
  int iter_count;
  public:
  zeidel_method(T _prec = T(0.0001)):prec(_prec),iter_count(0){};
  matrix<T,alloc> operator ()(const matrix<T,alloc> & a,const  matrix<T,alloc> & b)
  {
   return  slau_zeidel(a,b,prec,&iter_count);
  }
};
#endif

