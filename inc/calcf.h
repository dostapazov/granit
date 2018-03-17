#ifndef _CALCF_H_
#define _CALCF_H_

 #ifndef _WINDOWS_
   #ifndef STRICT
    #define STRICT
   #endif
   #define WIN32_LEAN_AND_MEAN
   #include <Windows.h>
 #endif


 #define CALC_NO_ERROR        0
 #define CALC_ERR_VARIABLES   1   //Отсутствует переменная
 #define CALC_ERR_NOARGUMENTS 2   //Недостаточно аргументов
 #define CALC_ERR_SYNTAX      3   //Синтаксическая ошибка
 #define CALC_ERR_MUSTFARGS   4   //Ожидается список аргументов ф-ции
 #define CALC_ERR_EXPRESSION  5   //Непонятное выражение
 #define CALC_ERR_ZERODIV     6   //Деление на 0
 #define CALC_ERR_FP          7   //Ошибка FP
 #define CALC_ERR_FPOVERFLOW  8   //Переполнение
 

 
 #define CALC_ERR_INVALID_OBJECT DWORD(-1)

typedef long double  LDOUBLE;
//typedef float  LDOUBLE;



#ifdef __cplusplus
extern "C"{
#endif

LPVOID WINAPI CreateCalculator();
BOOL WINAPI DestroyCalculator( LPVOID obj );
int  WINAPI AddVariable(LPVOID  obj,float v);
int  WINAPI AddVariableFromString(LPVOID  obj,char * str);
BOOL WINAPI SetVariable(LPVOID  obj,int idx,float v);
BOOL WINAPI SetVariableFromString(LPVOID  obj,int idx,char * str);
BOOL WINAPI ClearVariables(LPVOID obj);
LDOUBLE WINAPI CalcFormula(LPVOID obj,char * formula);
LDOUBLE WINAPI CalcFormulaVars(LPVOID obj,char * formula,DWORD var_count,LDOUBLE *var_array);
DWORD   WINAPI GetCalcError(LPVOID obj);

#ifdef __cplusplus
}
#endif

#endif



