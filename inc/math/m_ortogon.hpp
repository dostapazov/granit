
/*
 Решение СЛАУ методом ортогонализации
 A матрица коэффициентов;
 D матрица свободных членов
*/

#ifndef _M_ORTOGON_HPP
#define _M_ORTOGON_HPP

#ifndef _MATRIX_HPP_INC
#include "matrix.hpp"
#endif
#include "m_backtrace.hpp"

template<class T,class Allocator>
void __fastcall sub_columns(matrix<T,Allocator>& dst,int c1,matrix<T,Allocator> & src,int c2)
{
 for(int i = 0;i<dst.rows();i++)
    dst(i,c1)-=src(i,c2);
}

template<class T,class Allocator>
void __fastcall ortogonalize( matrix<T,Allocator> & A,matrix<T,Allocator> &r,matrix<T,Allocator> &t)
{
 int i,j,k;
 t.set_size(r.rows(),r.columns());
 diagonal(t);
 matrix<T,Allocator> temp(A.rows(),1);

 for(j = 0;j< A.columns();j++)
 {
    //По всем колонкам
    r.set_col(A,j,j);

    for(k = 0;k<j;k++)
    {
     temp.set_col(r,k,0);
     temp.mul_col(0,t(k,j));
     sub_columns(r,j,temp,0);
     }

    for(i=j+1;i<A.columns();i++)
       {
        T val1 = svect_multiple_columns(A,i,r,j),val2 = svect_multiple_columns(r,j,r,j);
        t(j,i) = val1/val2;
       }
 }

}


template<class _Type,class Allocator>
matrix<_Type,Allocator> __fastcall slau_ortogon( matrix<_Type,Allocator> A,
                                             matrix<_Type,Allocator> B)
{
 matrix<_Type,Allocator> X(B);
 if( A.rows() == B.rows() && B.columns() == 1)
 {
 int i;
  matrix<_Type,Allocator> _R,_Rt,mT;
  _R = A;
  ortogonalize(A,_R,mT);
  _Rt = _R.transp();
  _R = _Rt*_R;
  B = _Rt*B;
  for(i = 0;i<_R.rows();i++)
      _R(i,i)= _Type(1)/_R(i,i);
  B=_R*B;
  // mT * X = B;
  triangle_back_trace(mT,B,X);
 }
 return X;
}



template <class T,class alloc _RWSTD_COMPLEX_DEFAULT(std::allocator<T>)>
struct    ortogon_method:public unary_function<T,T>
{
  public:
  matrix<T,alloc> operator ()(const matrix<T,alloc> & a,const  matrix<T,alloc> & b)
  {
   return  slau_ortogon(a,b);
  }
};
#endif

