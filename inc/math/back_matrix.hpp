/*
 Вычисление обратной матрицы
*/

#ifndef _BACK_MATRIX_HPP
#define _BACK_MATRIX_HPP

#ifndef _MATRIX_HPP_INC
#include "matrix.hpp"
#endif


template <class T, class alloc,class method>
matrix<T,alloc> back_matrix(const matrix<T,alloc> & src, method &F)
{
  matrix<T,alloc> result;
  if(src.rows() == src.columns() )
  {
    int m_size = src.rows();
    matrix<T,alloc> B(m_size,1),X;
    result.set_size(m_size,m_size);
    for(int i = 0;i<m_size;i++)
    {
     B(i,0) = T(1);
      X = F(src,B);
      result.set_col(X,0,i);
     B(i,0) = T(0);
    }
  }
  return result;
}
#endif

