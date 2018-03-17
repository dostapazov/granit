/*Поддержка для Interbase UDF's*/

#include <kertl.hpp>
#include <calcf.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#include <stack>
#include <vector>
using namespace std;

#pragma pack (push,1)

struct ib_c_string
{
 
 WORD size;
 char text[1];
 const char * c_str(){return text;}
 ib_c_string & operator = (const char * src)
 {  lstrcpyn(text,src,size-1);*(text+size-1) = 0; return * this; }

 void __fastcall GetString(char * dest,int maxLen)
 {
  lstrcpyn(dest,text,maxLen);
  *(dest+maxLen-1)=0;
 }

};
#pragma pack (pop)

#ifdef __BORLANDC__
#pragma options push
#pragma options -u-
#endif

 extern "C"   int  ib_AllocCalculator()
 {
   int ret =  int (CreateCalculator());
   return ret;
 }

 extern "C" int ib_FreeCalculator(int * CalcHandle)
 {
   return DestroyCalculator((LPVOID)(*CalcHandle));
 }

 extern "C" int ib_AddCalculatorVariable( int * CalcHandle,ib_c_string * var_str)
 {
  LPVOID calc = LPVOID(*CalcHandle);
  return AddVariableFromString(calc,var_str->text);
 }

extern "C" int ib_CalcSetVariable( int * CalcHandle,int index,ib_c_string * var_str)
 {
  LPVOID calc = LPVOID(*CalcHandle);
  return SetVariableFromString(calc,index,var_str->text);
 }

 extern "C" int ib_CalcClearVariables(int * CalcHandle)
 {
    return ClearVariables((LPVOID)(*CalcHandle));
 }

 extern "C" int ib_CalcGetError(int * CalcHandle)
 {
   return GetCalcError((LPVOID)(*CalcHandle));
 }

 extern "C"  long double ib_CalcFormula(int *CalcHandle,ib_c_string * formula)
 {
    long double cval = CalcFormula((LPVOID)(*CalcHandle),formula->text);
    return double(cval);
 }

#ifdef __BORLANDC__
#pragma options pop
#endif



