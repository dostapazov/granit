/*
 Приведение матрицы к нормальному виду Фроббениуса
 Остапенко Д.В.
*/


#ifndef _M_FROBBENIUS_HPP
#define _M_FROBBENIUS_HPP

#ifndef _MATRIX_HPP_INC
#include "matrix.hpp"
#endif


template <typename T,typename allocator>
void __fastcall frobbenius_form(matrix<T,allocator> & A)
{
 cout<<A<<endl;
 matrix<T,allocator> save(A.rows(),1);
 for(int i = A.max_row();i>0;)
 {
   A.div_row(i,A(i,i-1));
   i--;
   save.set_col(A,i,0);
   for(int j = 0;j<A.columns();j++)
   {
    if(i!=j)
    {
     A.mul_col(i,A(i+1,j));
     A.sub_cols(i,j);
     A.set_col(save,0,i);
    }
    cout<<A<<endl;
   }
 }
}


#endif


 