#pragma hdrstop
#include <calcf.h>
	 

LDOUBLE  _fact(int top,LPDWORD x)
	 {
		 LDOUBLE res = 1; 
		 *x = 1;
		 __try{
		 for(__int64 mul = 1;mul<=top;mul++)
               res*=mul;
		 }
		  __except(EXCEPTION_EXECUTE_HANDLER)
		  {
			 *x = 0;
		  }
		  

	   return res;
	 }
