#ifndef _M_TRIANGLEBACKTRACE_HPP
#define _M_TRIANGLEBACKTRACE_HPP

#ifndef _MATRIX_HPP_INC
#include "matrix.hpp"
#endif

//Процедура обратного хода для верхней треугольной матрицы

template <typename T,typename allocator>
void __fastcall    triangle_back_trace(matrix<T,allocator> & A,matrix<T,allocator> &B,matrix<T,allocator> & X)
{
 X.set_size(B.rows(),B.columns());
 int i,j,n;
 i = A.max_row();
 n = A.max_column();
 T   xj;
 for(;i>=0;i--)
 {
   xj=B(i,0);
   for(j = n;j>i;j--)
     xj -= X(j,0)*A(i,j);
   X(i,0) = A(i,i)? xj/A(i,i):T(0);
 }
}

//Приведение матриц коэффициентов и свободных членов к верхнему треугольному виду

template <typename T,typename allocator>
void __fastcall down_triangle2upper(matrix<T,allocator> & A,matrix<T,allocator> & B)
{
 int i = 0,j = A.max_row();
 while(i<j)
 {
  A.swap_rows(i,j);
  B.swap_rows(i++,j--);
 }
}
#endif

