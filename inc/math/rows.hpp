#ifndef _ROW_HPP
#define _ROW_HPP


//Вычисление ряда
template <typename T>
T __fastcall row(T val,int count,T * values)
{
 T result = 0;
 T p_val  = 1;
 for(int i = 0;i<count;i++)
 {
   result+= p_val*(*values);
   p_val*=val;
   values++;
 }
 return result;
}

//вычисление диффиренциала ряда порядка N
template <typename T>
int __fastcall diff_row(int count,T * values)
{

 for(int i = 1;i<count;i++)
 {
   *values = *(values+1) * T(i);
   values++;
 }
 *values = T(0);
 return count-1;
}



#endif
 