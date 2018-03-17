/*
 ¬ычисление определител€ матрицы
*/

#ifndef _MATRIX_HPP_INC
#include "matrix.hpp"
#endif

#include "m_decomposite.hpp"
#include "m_gauss.hpp"


template <typename T,typename allocator>
T __fastcall det_decomp(matrix<T,allocator> & A)
{
  matrix<T,allocator> B,C;
  decomposite(A,B,C);
  //cout<<B<<endl<<C<<endl;
  T the_det(B(0,0));
  for( int i = 1;the_det!=T(0) && i<B.rows();i++)
    the_det*=B(i,i);
  return the_det;
}

template <typename T,typename allocator>
T __fastcall det_gauss(matrix<T,allocator>  A)
{
 T the_det(0);
  if(A.rows())
  {
  the_det = T(1);
  matrix<T,allocator> B(A.rows(),1);// фиктивна€
  for(int i = 0;the_det!=T(0) && i<A.rows();i++)
  {
   the_det*=A(i,i);
   if(i&1)
   the_det*=T(-1);
   gauss_triangle(A,B,i,i+1);
  }
  }
  return the_det;
}
