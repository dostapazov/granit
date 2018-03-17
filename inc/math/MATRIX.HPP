#ifndef _MATRIX_HPP_INC
#define _MATRIX_HPP_INC

#ifndef __VECTOR_H
#include <vector>
#endif

#ifndef __STD_ALGORITHM
#include <algorithm>
#endif

#ifndef __STD_FUNCTIONAL__
#include <functional>
#endif


using namespace std;

/*
 класс matrix основан на std::vector;
 первый элемент всгда равен 0 и на нкго ссылаются при превышении номера индекса
*/

#ifndef _RWSTD_COMPLEX_DEFAULT
#define _RWSTD_COMPLEX_DEFAULT(x) =x
#endif


template <class T,class Allocator _RWSTD_COMPLEX_DEFAULT(allocator<T>)>
class matrix:protected vector<T,Allocator>
{
 protected:
 int _rows,_cols;//Строк и колонок
 int __fastcall get_square(){return _rows*_cols;}
 friend insert_iterator< matrix<T,Allocator> >;
 public:
 matrix(const matrix & m){(*this)=m;}
 matrix():_rows(0),_cols(0){resize(0,0);};
 matrix(int r,int c):vector<T,Allocator>(0),_rows(0),_cols(0)
 {set_size(r,c);};
 matrix(int n):vector<T,Allocator>(0),_rows(0),_cols(0)
 {set_size(n,n);};

 T * __fastcall get_row(int r)
 {
  r = r >-1 && r<_rows?
  r:_rows;
  return begin()+r*_cols+1;
 }

 T & operator ()(int row, int col) const
 {return  (T&)((row<_rows && col<_cols) ? (*this)[(row)*_cols+col+1] :(*this)[0]); };

 int __fastcall set_size(int row,int col)
 {  resize(row*col+1);*begin()=0; _rows = row;_cols = col; return get_square(); }

 int  __fastcall columns()const{return _cols;} ;
 int  __fastcall rows()   const{return _rows;}

 void __fastcall set_values(T * array) // установка значений из памяти
 {copy(array,array+get_square(), begin()+1);}

 matrix & __fastcall insert_columns(int count=1,int to = -1);
 matrix & __fastcall insert_rows   (int count=1,int to = -1);
 matrix & __fastcall grow          (int count=1);
 int      __fastcall max_row    (){return _rows-1;}
 int      __fastcall max_column(){return _cols-1;}
 matrix & operator = (const matrix & m)
 {
  int r = m.rows(); int c = m.columns();
  set_size(r,c);
  copy(m.begin(),m.end(),begin());return *this;
 }
 T      __fastcall track(); //След матрицы
 matrix & operator+=(T val);
 matrix & operator-=(T val);
 matrix & operator*=(T val);
 matrix & operator/=(T val);
 matrix & operator*=(const matrix & m);
 matrix & operator+=(const matrix & m);
 matrix & operator-=(const matrix & m);

 matrix   __fastcall transp        ();
 matrix & __fastcall mul_row  (int row,T val);
 matrix & __fastcall div_row  (int row,T val);
 matrix & __fastcall mul_col  (int row,T val);
 matrix & __fastcall div_col  (int row,T val);
 matrix & __fastcall add_rows (int row_op,int row_dest);
 matrix & __fastcall sub_rows (int row_op,int row_dest);
 matrix & __fastcall add_cols (int row_op,int row_dest);
 matrix & __fastcall sub_cols (int row_op,int row_dest);
 matrix & __fastcall swap_rows(int row1,  int row2);
 matrix & __fastcall swap_columns(int c1,  int c2);
 matrix & __fastcall set_row(matrix & src,int s_row,int d_row);
 matrix & __fastcall set_col(matrix & src,int s_col,int d_col);

 T        __fastcall sum_of_row   (int row,bool abs = false);
 T        __fastcall sum_of_column(int column,bool abs = false);

 T        __fastcall r_vectlen   (int row);
 T        __fastcall c_vectlen(int column);


 T        __fastcall norm1();//Норма 1 по mathcad norm1 = max(sum_of_column)
 T        __fastcall norm2();//Норма 2 по mathcad normi = max(sum_of_column)
 T        __fastcall norm3();//Норма 1 по mathcad


 struct  multiplier:public unary_function<T,void>
 {
  T val;
  multiplier(T v):val(v){};
  void operator()( T & x){x*=val;}
 };

 struct  adder:public unary_function<T,void>
 {
  T val;
  adder(T v):val(v){};
  void operator()( T & x){x+=val;}
 };

 struct  divisor:public unary_function<T,void>
 {
  T val;
  divisor(T v):val(v){};
  void operator()( T & x){x/=val;}
 };
 vector<T,Allocator>::begin;
 vector<T,Allocator>::end  ;

 T __fastcall abs(T & v){return (v < T(0)) ? -v :v;}
};



/*Implementation*/

template <class T,class Allocator>
 matrix<T,Allocator> & __fastcall matrix<T,Allocator>::insert_rows(int count,int to)
 {
  //вставка  строки (по умолчанию в конец)
  set_size(_rows+count,_cols);
  return * this;
 };

template <class T,class Allocator>
matrix<T,Allocator> & __fastcall matrix<T,Allocator>::insert_columns(int count ,int to )
 {
  //втавка колонки  (по умолчанию в конец)
  to = ((to>-1 && to< _cols) ? to:_cols);

  int new_cols = _cols+count;//Количество колонок в новой матрице
  resize(1+_rows*(new_cols));
  iterator  c1 = get_row(_rows-1)/*,c2= end()-new_cols*/;
  int i(1);
  vector<T> store(_cols);
  do
  {
   copy(c1,c1+_cols,store.begin());
   fill(c1,c1+_cols,T(0));
   copy(store.begin(),store.end(),c1+_cols-i);
   c1-=_cols;
  }while(++i<_rows);
   _cols =new_cols;
  return * this;
 }

template <class T,class Allocator>
matrix<T,Allocator> & __fastcall matrix<T,Allocator>::grow(int d )
 {
  insert_columns(d);
  return insert_rows(d);
 }

template <class T,class Allocator>
matrix<T,Allocator>  __fastcall matrix<T,Allocator>::transp()
 {
  //транспонирование
  matrix<T,Allocator> m(_cols,_rows);
  for(int i=0;i < _rows;i++)
    for(int j=0;j<_cols;j++)
      m(j,i)=(*this)(i,j);
   return m;
 }

template <class T,class Allocator>
matrix<T,Allocator> & matrix<T,Allocator>::operator+=(T val)
{
 for_each(begin(),end(),adder(val));
 return *this;
}

template <class T,class Allocator>
matrix<T,Allocator> & matrix<T,Allocator>::operator-=(T val)
{
 for_each(begin(),end(),adder(-val));
 return * this;
}
template <class T,class Allocator>
matrix<T,Allocator> & matrix<T,Allocator>::operator*=(T val)
{
 for_each(begin(),end(),multiplier(val));
 return * this;
}
template <class T,class Allocator>
matrix<T,Allocator> & matrix<T,Allocator>::operator/=(T val)
{
 for_each(begin(),end(),divisor(val));
 return * this;
}


template <class T,class Allocator>
matrix<T,Allocator>  operator * (const matrix<T,Allocator> & m1,const matrix<T,Allocator>& m2)
{
 //Умножение матриц
 matrix<T,Allocator> result;
 if(m1.columns()==m2.rows())
 {
  result.set_size(m1.rows(),m2.columns());
  for(int i = 0;i<result.rows();i++)
   for(int j = 0; j<result.columns();j++)
    {
    T x(0);
    for(int k = 0;k<m1.columns();k++)
      x+= m1(i,k)*m2(k,j);
    result(i,j) = x;
    }
 }
 return result;
}

template <class T,class Allocator>
matrix<T,Allocator>  operator + (const matrix<T,Allocator> & m1,const matrix<T,Allocator>& m2)
{
 //Сложение матриц
 matrix<T,Allocator> result;
 if(m1.columns()==m2.columns() && m1.rows()==m2.rows())
 {
  result = m1;
  result+=m2;
 }
 return result;
}

template <class T,class Allocator>
matrix<T,Allocator>  operator - (const matrix<T,Allocator> & m1,const matrix<T,Allocator>& m2)
{
 //Вычитание матриц
 matrix<T,Allocator> result;
 if(m1.columns()==m2.columns() && m1.rows()==m2.rows())
 {
  result = m1;
  result-=m2;
 }
 return result;
}


template <class T,class Allocator>
matrix<T,Allocator>  & matrix<T,Allocator>::operator *=(const matrix<T,Allocator> & m1)
{
 *this = (*this)*m1;
 return *this;
}

template <class T,class Allocator>
matrix<T,Allocator>  & __fastcall matrix<T,Allocator>::mul_row(int row,T val)
{
    iterator i = get_row(row);
    if(i<end())
    for_each( i,i+_cols,multiplier(val));
   return *this;
}

template <class T,class Allocator>
matrix<T,Allocator>  & __fastcall matrix<T,Allocator>::div_row(int row,T val)
{
    iterator i = get_row(row);
    if(i<end())
     for_each( i,i+_cols,divisor(val));
 return *this;
}

template <class T,class Allocator>
matrix<T,Allocator>  & __fastcall matrix<T,Allocator>::mul_col(int col,T val)
{
   for(int i = 0;i<rows();i++)
      (*this)(i,col)*=val;
   return *this;
}

template <class T,class Allocator>
matrix<T,Allocator>  & __fastcall matrix<T,Allocator>::div_col(int col,T val)
{
   for(int i = 0;i<rows();i++)
      (*this)(i,col)/=val;
 return *this;
}


template <class T,class Allocator>
matrix<T,Allocator>  & __fastcall matrix<T,Allocator>::add_rows(int row_op,int row_dest)
{
 iterator op   = get_row(row_op);
 iterator dest = get_row(row_dest);
 if(op<end() && dest<end())
   transform(op,op+_cols,dest,dest,plus<T>());
 return * this;
}

template <class T,class Allocator>
matrix<T,Allocator>  & __fastcall matrix<T,Allocator>::sub_rows(int row_op,int row_dest)
{
 iterator op   = get_row(row_op);
 iterator dest = get_row(row_dest);
 iterator last = op + _cols;
 if(op<end() && dest<end())
   transform(op,last,dest,dest,minus<T>());
   /*while(op<last)
   (*dest++)-=(*op++);*/
 return * this;
}

template <class T,class Allocator>
matrix<T,Allocator>  & __fastcall matrix<T,Allocator>::add_cols(int col_op,int col_dest)
{
 for(int i = 0;i<rows();i++)
     (*this)(i,col_dest) +=(*this)(i,col_op);
 return * this;
}

template <class T,class Allocator>
matrix<T,Allocator>  & __fastcall matrix<T,Allocator>::sub_cols(int col_op,int col_dest)
{
 for(int i = 0;i<rows();i++)
     (*this)(i,col_dest) -=(*this)(i,col_op);
 return * this;
}


template <class T,class Allocator>
matrix<T,Allocator>  & __fastcall matrix<T,Allocator>::swap_rows(int row1,int row2)
{
 iterator i1   = get_row(row1);
 iterator i2 = get_row(row2);
 if(i1<end() && i2<end())
   swap_ranges(i1,i1+_cols,i2);
 return * this;
}


template <class T,class Allocator>
matrix<T,Allocator>  & __fastcall matrix<T,Allocator>:: set_row(matrix<T,Allocator> & src,int s_row,int d_row)
{
 //установка строки
 iterator s =src.get_row(s_row);
 copy(s,s+min(src.columns(),columns()), get_row(d_row));
  return *this;
}

template <class T,class Allocator>
matrix<T,Allocator>  & __fastcall matrix<T,Allocator>:: set_col(matrix<T,Allocator> & src,int s_col,int d_col)
{
 for(int i = 0;i<rows();i++)
  {
   (*this)(i,d_col)= src(i,s_col);
  }
  return *this;
}



template <class T,class Allocator>
matrix<T,Allocator>  & __fastcall matrix<T,Allocator>::swap_columns(int c1,int c2)
{
 if(c1>=0 && c1<_cols && c2>=0 && c2<_cols)
 {
  for (int r = 0;r<_rows;r++)
     {
      ::swap((*this)(r,c1),(*this)(r,c2));
     }
 }
 return * this;
}

template <class T,class Allocator>
matrix<T,Allocator>  &  matrix<T,Allocator>::operator+=(const matrix<T,Allocator> & m)
{
 if(rows() == m.rows() && columns()==m.columns())
  transform(m.begin(),m.end(),begin(),begin(),plus<T>());
 return *this;
}

template <class T,class Allocator>
matrix<T,Allocator>  &  matrix<T,Allocator>::operator-=(const matrix<T,Allocator> & m)
{
 if(rows() == m.rows() && columns()==m.columns())
  transform(m.begin(),m.end(),begin(),begin(),minus<T>());
 return *this;
}

template <class T,class Allocator>
 T      __fastcall matrix<T,Allocator>::track()
 {
  T tr(0);
  if(rows()==columns())
    for(int i = 0;i<_rows;i++)
       tr+=(*this)(i,i);
  return tr;
 }

template <class T,class Allocator>
 T        __fastcall matrix<T,Allocator>::sum_of_row   (int row,bool mod)
 {
  T sum(0);
  for(int j = 0;j<columns();j++)
     sum+=(mod ? abs((*this)(row,j)):(*this)(row,j));
  return sum;
 }

template <class T,class Allocator>
 T        __fastcall matrix<T,Allocator>::sum_of_column(int column,bool mod)
 {
  T sum(0);
  for(int i = 0;i<rows();i++)
     sum+=( mod ? abs((*this)(i,column)):(*this)(i,column));
  return sum;
 }

template <class T,class Allocator>
 T        __fastcall matrix<T,Allocator>::r_vectlen   (int row)
 {
  T res(0),elem;
  for(int j = 0;j<columns();j++)
     {
      elem = (*this)(row,j);
      res+=elem*elem;
     }
  return sqrt(res);
 }

template <class T,class Allocator>
 T        __fastcall matrix<T,Allocator>::c_vectlen(int column)
 {
  T res(0),elem;
  for(int i = 0;i<rows();i++)
    {
     elem = (*this)(i,column);
     res+=elem*elem;
    }
  return sqrt(res);
 }


template <class T,class Allocator>
 T        __fastcall matrix<T,Allocator>::norm1()//Норма 1 по mathcad max(sum_of_column)
 {
  T n(0);
  if(_rows == _cols)
  {
    for(int i = 0;i<_cols;i++)
      n = max(sum_of_column(i,true),n);
  }
  return n;
 }

template <class T,class Allocator>
 T        __fastcall matrix<T,Allocator>::norm2()//Норма 1 по mathcad normi max(sum_of_rows)
 {
  T n(0);
  if(_rows == _cols)
  {
    for(int i = 0;i<_rows;i++)
      n = max(sum_of_row(i,true),n);
  }
  return n;
 }

template <class T,class Allocator>
 T        __fastcall matrix<T,Allocator>::norm3()//Норма 3 по mathcad norme
 {
  T n(0);
  T v;
  if(_rows == _cols)
  {
   for(int i = 0;i<_rows;i++)
    for(int j = 0;j<_cols;j++)
     {
      v = (*this)(i,j);
      n+=v*v;
     }
   n = sqrt(n);
  }
  return n;
 }

template <class T,class Allocator>
bool      operator == (const matrix<T,Allocator>& m1, const matrix<T,Allocator>& m2)
{
  if(m1.rows() == m2.rows() && m1.columns() == m2.columns())
  {
   return equal(m1.begin(),m1.end(),m2.begin());
  }
  return false;
}

template <class T,class Allocator>
bool      operator != (const matrix<T,Allocator>& m1, const matrix<T,Allocator>& m2)
{
   return !(m1==m2);
}



//Умножит любые матрицы
template <class T,class Allocator>
matrix<T,Allocator> adjust_mul(const matrix<T,Allocator>& m1, const matrix<T,Allocator>& m2)
{
 matrix<T,Allocator> _m1(m1),_m2(m2);
 int d = _m1.rows()-_m2.columns();
 if(d)
 {
  if(d>0)
   _m2.insert_columns(d);
   else
   _m1.insert_rows(-d);
 }
  return _m1*_m2;
}

template <class T,class Allocator>
matrix<T,Allocator> & __fastcall diagonal(matrix<T,Allocator>& m , T fill = 1,int n = 0)
{
 if(n>0)
   m.set_size(n,n);
 for(int i = 0;i<m.rows();i++)
   for(int j = 0;j<m.columns();j++)
       m(i,j) =  i==j ? fill:T(0);
  return m;
}

template <class T,class Allocator>
matrix<T,Allocator> & __fastcall reverse_diagonal(matrix<T,Allocator>& m, T fill = 1,int n = 0)
{
 if(n>0)
   m.set_size(n,n);
 int k = m.rows();
 int z = 0;
 for(int i = 0;i<m.rows();i++)
 { k--;
   for(int j = 0;j<m.columns();j++)
       m(i,j) =  i==z && j == k ? fill:T(0);
   z++;
 }
  return m;
}

template <class T,class Allocator>
matrix<T,Allocator> & __fastcall fill_matrix(matrix<T,Allocator>& m,T v, int n = 0)
{
 if(n>0)
   m.set_size(n,n);
 fill(m.begin(),m.end(),v);
 return m;
}


template <class T,class Allocator>
void  __fastcall frame(matrix<T,Allocator> & src,
                       matrix<T,Allocator> & dest,
                       int from_row,
                       int from_col,
                       int rows,
                       int cols )
{
 //Получение окна
 dest.set_size(0,0);
 if(src.rows()>from_row && from_row+rows<=src.rows())
  if(src.columns() > from_col && from_col+cols <=src.columns())
  {
   dest.set_size(rows,cols);
   for(int i = 0;i<rows;i++)
     for(int j = 0;j<cols;j++)
      dest(i,j) = src(i+from_row,j+from_col);
  }
}

 template  <class T,class Allocator>
 T        __fastcall svect_multiple_rows(matrix<T,Allocator> & one,int r1,matrix<T,Allocator> & two,int r2)
 {
   //скалярное произведение по строк-векторов
   T res(0);
   int end = min (one.columns(),two.columns());
   for(int i = 0;i< end;i++)
       res+=one(r1,i)*two(r2,i);

   return res;
 }

 template  <class T,class Allocator>
 T        __fastcall svect_multiple_columns(matrix<T,Allocator> & one,int c1,matrix<T,Allocator> & two,int c2)
 {
   //скалярное произведение по строк-векторов
   T res(0);
   int end = min (one.rows(),two.rows());
   for(int i = 0;i< end;i++)
       res+=one(i,c1)*two(i,c2);
   return res;
 }

template  <class T,class Allocator>
T __fastcall rvect_angle(matrix<T,Allocator> & one,int r1,matrix<T,Allocator> & two,int r2)
{
 return acos(svect_multiple_rows(one,r1,two,r2)/(one.r_vectlen(r1)*two.r_vectlen(r2)));
}

template  <class T,class Allocator>
T __fastcall cvect_angle(matrix<T,Allocator> & one,int c1,matrix<T,Allocator> & two,int c2)
{
 return acos(cvect_multiple_rows(one,c1,two,c2)/(one.c_vectlen(c1)*two.c_vectlen(c2)));
}

/*Проверка на единичную диагональную матрицу*/
template  <class T,class Allocator>
bool __fastcall is_E_matrix(matrix<T,Allocator>  & m, T prec = T(.001))
{
  for(int i = 0;i<m.rows();i++)
  {
   for(int j = 0;j<m.columns();j++)
   {
     T sub_result =  m(i,j)- ((i-j) ? T(0):T(1));
     if(prec < T(abs(sub_result)))
       return false;
   }
  }
  return true;
}


//тест на нижнюю треугольную матрицу
template <typename T,typename allocator>
bool __fastcall is_triangle_matrix(matrix<T,allocator> & m,T prec = T(0.001))
{
  for(int j=1;i<m.columns();i++)
  {
          for(int i=0;i<j ;i++)
          {
            T sub_res = A(i,j)-T(0);
                if(abs(sub_result)>=prec)
           return false;
          }
  }
 return true;
}
;
#endif

