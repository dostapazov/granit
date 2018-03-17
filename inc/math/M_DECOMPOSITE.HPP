/*
 Решение СЛАУ методом декомпозиции
 A матрица коэффициентов;
 D матрица свободных членов
*/
#ifndef _M_DECOMPOSITE_HPP
#define _M_DECOMPOSITE_HPP

#ifndef _MATRIX_HPP_INC
#include "matrix.hpp"
#endif


template<class T,class Allocator>
T __fastcall decomposite_sum1(matrix<T,Allocator> & B,
                              matrix<T,Allocator> & C,int i,int j)
{
  T res(0);
  int k = 0;
  for(;k<j;k++)
     res+=B(i,k)*C(k,j);
  return res;
}

template<class T,class Allocator>
T __fastcall decomposite_sum2(matrix<T,Allocator> & B,
                              matrix<T,Allocator> & C,int i,int j)
{
  T res(0);
  int k = 0;
  for(;k<i;k++)
     res+=B(i,k)*C(k,j);
  return res;
}



template<class T,class Allocator>
void __fastcall decomposite(matrix<T,Allocator> & A,
                            matrix<T,Allocator> & B,
                            matrix<T,Allocator> & C)
{
 fill_matrix(B,T(0),A.rows());
 diagonal(C,T(1),A.rows());

 int i,j;/*MSVC compatible*/
 for( i = 0;i<B.rows();i++)
    {
     B(i,0) = A(i,0);            //B[i,1] = A[i,1]
     C(0,i) = A(0,i);
    }
    C.div_row(0,B(0,0));



 for( i = 1;i<A.rows();i++)
 {
  for( j = 1;j<A.columns();j++)
  {
   if(j<=i)
     B(i,j) = A(i,j)- decomposite_sum1(B,C,i,j);
   if(i<j)
         C(i,j) = B(i,i) ? (A(i,j)-decomposite_sum2(B,C,i,j))/B(i,i):0;

  }
 }

}


template<class T,class Allocator>
matrix<T,Allocator> __fastcall slau_decomposite(matrix<T,Allocator> A,
                                          matrix<T,Allocator> D)
{
 matrix<T,Allocator>  X(D.rows(),D.columns());/*Матрица результатов*/
 matrix<T,Allocator>  Y(D.rows(),D.columns());
 matrix<T,Allocator> B,C;
 if( A.rows() == D.rows() && D.columns() == 1)
 {
  //Проверка правильных размеров матриц

  decomposite(A,B,C); //разложение исходной матрицы на две треугольных B*C = A
  //cout<<B<<endl<<C<<endl;
  Y(0,0) = D(0,0)/B(0,0);
  T temp_value,divider;
  int i,k;
  for( i = 1;i<Y.rows();i++)
  {
    temp_value = T(0);
    divider = B(i,i);
    for(k = 0;k<i;k++)
      temp_value+=B(i,k)*Y(k,0);
    Y(i,0) = (divider == T(.000000)) ? 0: (D(i,0)-temp_value)/divider;
  }

 X(X.max_row(),0) = Y(Y.max_row(),0);
  for( i = X.max_row()-1;i>=0;i--)
  {
    temp_value = T(0);
    for( k = i+1;k<X.rows();k++)
        temp_value+= C(i,k)*X(k,0);
    X(i,0) = Y(i,0)-temp_value;
  }

 }
 return X;
}

template <class T,class alloc _RWSTD_COMPLEX_DEFAULT(std::allocator<T>)>
struct    decomposite_method:public unary_function<T,T>
{
  public:
  matrix<T,alloc> operator ()(const matrix<T,alloc> & a,const  matrix<T,alloc> & b)
  {
   return  slau_decomposite(a,b);
  }
};



#endif
