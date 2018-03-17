#ifndef _M_GAUSS_HPP_
#define _M_GAUSS_HPP_

#ifndef _MATRIX_HPP_INC
#include "matrix.hpp"
#endif

#include "m_backtrace.hpp"

/*
 ������� ���� ������� ������
 A ������� �������������;
 B ������� ��������� ������

*/


template <class T,class Allocator>
void __fastcall gauss_triangle(matrix<T,Allocator> & A,matrix<T,Allocator> & B,int  begin =0, int end=0)
{
 //���������� � ������� ����������� �������
  if(end < 1)
     end = A.columns();
  matrix<T,Allocator> save_A,save_B;
  for(int i = begin;i<end;i++)
  {
   //�� ���� ��������
   T divider(A(i,i));
   if(!(divider==T(0)))
   {
   A.div_row(i,divider);//��������� ��� �������� �� A[j,j];
   B.div_row(i,divider);

   frame(A,*&save_A,i,0,1,A.columns());
   frame(B,*&save_B,i,0,1,B.columns());

   for(int j=i+1;j<A.rows();j++)
     {
      T multiplier(A(j,i));
      A.mul_row(i,multiplier);
      B.mul_row(i,multiplier);
      A.sub_rows(i,j);
      B.sub_rows(i,j);
      A.set_row(save_A,0,i);
      B.set_row(save_B,0,i);
     }
   }
  }
}

template<class T,class Allocator>
matrix<T,Allocator> __fastcall slau_gauss(matrix<T,Allocator> A,
                                          matrix<T,Allocator> B)
{
 matrix<T,Allocator> & result = B;/*������� �����������*/
 if( A.rows() == B.rows() && B.columns() == 1)
 {
  //�������� ���������� �������� ������
  gauss_triangle(A,B);
  //�������� ���
  triangle_back_trace(A,B,result);
 }
 return result;
}

template<class T,class alloc _RWSTD_COMPLEX_DEFAULT(std::allocator<T>) >
struct    gauss_method:public unary_function<T,T>
{
  public:
  matrix<T,alloc> operator ()(const matrix<T,alloc> & a,const  matrix<T,alloc> & b)
  {
   return  slau_gauss(a,b);
  }
};

#endif

