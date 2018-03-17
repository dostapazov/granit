/*
 Решение СЛАУ методом простой итерации
 A матрица коэффициентов;
 B матрица свободных членов
*/

#ifndef _M_ITERATION_HPP
#define _M_ITERATION_HPP

#ifndef _MATRIX_HPP_INC
#include "matrix.hpp"
#endif



/*
  X(k+1) = b + aX(k)  ; b =  B(i)/A(i,i); X(0) = b;
*/

#define INCREMENT_ITERATION(x) {if(x) (*x)++;}

template<class T,class Allocator>
bool   __fastcall check_iteration_condition(matrix<T,Allocator> &A)
{
 //Проверка условий сходимости
 for(int i = 0;i<A.rows();i++)
  {
   T diag_elem(abs(A(i,i)));
   T sum = A.sum_of_row(i,true) - diag_elem;
   if(diag_elem<=sum)
    return false;
  }
  return true;
}

template<class I>
bool __fastcall check_sequences(vector<I> v)
{
 sort(v.begin(),v.end());
 vector<I>::iterator b = v.begin(),c=b+1,e = v.end();
 while(c<e)
 {
  if(*c-*b != I(1))
    return false;
    c++;
    b++;
 }
 return true;
}


template<class T,class Allocator>
bool __fastcall try_normalize(matrix<T,Allocator> &A,
                              matrix<T,Allocator> &B)
{
 //попытка привести к виду , когда максимальные элементы находятся на диагонали
 vector<int> indexes(A.rows());
 int i,j;
 for( i = 0;i<A.rows();i++)
 {
   int idx(0);
   for( j = 1;j<A.rows();)
    {
      idx = (A.abs(A(i,idx))<A.abs(A(i,j))) ? j:idx;
      j++;
    }
    indexes[i]=idx;
 }

 if(check_sequences(indexes))
 {
  vector<int>::iterator c = indexes.begin();
  for(i=0;c<indexes.end()-1;)
  {
   if(i!=*c)
     {
      A.swap_rows(i,*c);
      B.swap_rows(i,*c);
      indexes.erase(c);
      indexes.erase(find(indexes.begin(),indexes.end(),i));
      //cout<<A<<endl;
     }
     else
      c++;
     i++;
  }
  return true;
 }
 return false;
}



template<typename T,typename Allocator>
void  __fastcall iteration_convert(matrix<T,Allocator> &A,
                                   matrix<T,Allocator> &B)
{
 if(A.norm1()<T(1))
 {
  //Норма удовлетворяет условию теоремы о сходимости итерационного метода
  /*
  matrix<T,Allocator> P,D,E;
  diagonal(P,T(1),A.rows());
  E = P;
  for(int i = 0;i<P.rows();i++)
    P(i,i) = svect_multiple_columns(A,i,A,i);
  D = P*(A.transp());
  B = D*B;
  A = E-D*A;
  */
 }
 else
 {
 cout<<
 "В данной реализации система система не приведена к диагональному преобладанию"<<endl<<
 "Поэтому итерационный процесс будет расходиться "<<endl;
 diagonal(A);
 fill_matrix(B,.0);
 }
}


template<class T,class Allocator>
void  __fastcall prepare_iteration(matrix<T,Allocator> &A,
                                   matrix<T,Allocator> &B/*,
                                   matrix<T,Allocator> & alpha,
                                   matrix<T,Allocator> & X*/)
{

 //Переставить строки матрицы A, так чтобы  на диагонали стояли максимальные элементы
 int i,j,m_size = A.rows();
 if(!check_iteration_condition(A))
 {
   if(!try_normalize(A,B))
   {
    //Преобразование к виду x = Qx+c
    iteration_convert(A,B);
   }
 }

 for(i = 0;i< m_size;i++)
 {
  T diag(A(i,i));
  B(i,0)/=diag;
  for(j=0;j<m_size;j++)
    {
     T & elem = A(i,j);
     elem     = (i-j) ? -elem/diag : T(0);
    }
 }
    //cout<<"alpha = "<<endl<<A<<endl<<"betta = "<<endl<<B<<endl;

}


template<class T,class Allocator>
T __fastcall get_max(matrix<T,Allocator> & m)
{
 T v(0);
 for(int i = 0;i<m.rows();i++)
  for(int j = 0;j<m.columns();j++)
      v = max(m.abs(m(i,j)),v);
 return v;
}

template <class T,class Allocator>
bool __fastcall check_iteration_end(matrix<T,Allocator> X,matrix<T,Allocator> X1,T prec)
{
   X1-=X;
   return  X1.sum_of_column(0,true)<T(prec) ? true:false;
}



template<class T,class Allocator>
matrix<T,Allocator> __fastcall slau_iteration(matrix<T,Allocator> A, matrix<T,Allocator> B, T precision = T(.001),int * iter_count=0)
{
  matrix<T,Allocator> X;
 if(iter_count)
    *iter_count=0;

 if(A.rows()==A.columns() && B.rows() == A.columns() && B.columns()==1)
 {
  matrix<T,Allocator> alpha(A),betta(B),X1;
  prepare_iteration(alpha,betta);
  X1 = betta;

  do{
    INCREMENT_ITERATION(iter_count);
    X  = X1;
    X1 = (alpha*X);
    X1+=betta;
    //cout<<X1<<endl<<X<<endl;
   }while( !check_iteration_end(X,X1,precision) /* || 0 >precision*/);
  X = X1;
 }
 return X;
}

template <class T,class alloc _RWSTD_COMPLEX_DEFAULT(std::allocator<T>)>
struct    iteration_method:public unary_function<T,T>
{
  T prec;
  int iter_count;
  public:
  iteration_method(T _prec = T(0.0001)):prec(_prec),iter_count(0){};
  matrix<T,alloc> operator ()(const matrix<T,alloc> & a,const  matrix<T,alloc> & b)
  {
   return  slau_iteration(a,b,prec,&iter_count);
  }
};
#endif

