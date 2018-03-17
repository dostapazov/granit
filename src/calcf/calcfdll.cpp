#include <kertl.hpp>
#include "calcf.hpp"


#ifndef _NO_NAMESPACE
using namespace KeRTL;
#endif

#ifdef _MSC_VER
//#pragma comment(linker,"/opt:ref")
#pragma comment(linker,"/opt:nowin98")
#endif


LPVOID WINAPI CreateCalculator()
{
  return new TCalculator;
}

BOOL WINAPI DestroyCalculator( LPVOID obj )
{
  TCalculator * calc = reinterpret_cast<TCalculator*>(obj);
  
  if(calc && calc->IsValid())
    {
     delete calc;
     
     return TRUE;
    }
    return FALSE;
}

int WINAPI AddVariable(LPVOID  obj,float v)
{
  TCalculator * calc = reinterpret_cast<TCalculator*>(obj);
  if(calc&& calc->IsValid())
    {
     return calc->AddVariable(v);
    }
    return -1;
}

int WINAPI AddVariableFromString(LPVOID  obj,char * str)
{
  TCalculator * calc = reinterpret_cast<TCalculator*>(obj);
  if(calc && calc->IsValid())
    {
     return calc->AddVariable(str);
	    
    }
    return -1;
}


BOOL WINAPI SetVariable(LPVOID  obj,int idx,float v)
{
  TCalculator * calc = reinterpret_cast<TCalculator*>(obj);
  if(calc&& calc->IsValid())
    {
	  return calc->SetVariable(idx,v,0,0) ? TRUE:FALSE;
     
  }
    return FALSE;
}

BOOL WINAPI SetVariableFromString(LPVOID  obj,int idx,char * str)
{
  TCalculator * calc = reinterpret_cast<TCalculator*>(obj);
  if(calc && calc->IsValid())
    {
     
	  return  calc->SetVariable(idx,str,0,0) ? TRUE:FALSE;
  }
    return FALSE;
}


BOOL WINAPI ClearVariables(LPVOID obj)
{
  TCalculator * calc = reinterpret_cast<TCalculator*>(obj);
  if(calc && calc->IsValid())
    {
     calc->ClearVariables();
     return TRUE;
    }
    return FALSE;
}

LDOUBLE WINAPI CalcFormula(LPVOID obj,char * formula)
{
  TCalculator * calc = reinterpret_cast<TCalculator*>(obj);
  if(calc && calc->IsValid())
    {
     try{
         return (float)calc->Calc(formula,0,0);
        }
        catch(...)
        {
         //Error
        }
    }
    return 0;
}

LDOUBLE WINAPI CalcFormulaVars(LPVOID obj,char * formula,DWORD var_count,LDOUBLE *var_array)
{
  TCalculator * calc = reinterpret_cast<TCalculator*>(obj);
  if(calc && calc->IsValid())
    {
     try{
         return (float)calc->Calc(formula,var_count,var_array);
        }
        catch(...)
        {
         //Error
        }
    }
    return 0;
}




DWORD WINAPI GetCalcError(LPVOID obj)
{
  TCalculator * calc = reinterpret_cast<TCalculator*>(obj);
  if(calc && calc->IsValid())
  {
    return calc->GetError();
  }
  return CALC_ERR_INVALID_OBJECT;
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fwdreason, LPVOID lpvReserved)
{
        return 1;
}
extern "C" int _libmain(unsigned long reason)
{
	return 1;
}



/*
#define TEST_TYPE float


extern "C" TEST_TYPE WINAPI __TestLD(TEST_TYPE var)
{
	char  buffer[512];
	sprintf(buffer,"Аргумент %.30G\n Добавили 10 %.30G ",var,var + 10);
	MessageBox(0,buffer,"",MB_OK);
	var+=10;
	return var;
}
*/
