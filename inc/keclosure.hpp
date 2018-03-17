// Интерфейс аналогов __closure
// Остапенко Д.В. KeRTL V 1.0 27.02.2001

#ifndef _CLOSURE_METOD_INC
#define _CLOSURE_METOD_INC



#include <KeRTL.hpp>


template <class UD,class CF,class CD=LPVOID,class RD=DWORD>
class TClosureMethod
{
 public:
 typedef RD ( __stdcall  * closure_method)(UD userData,CF closureFrom,CD closureData);
 protected:
 UD  UserData;
 CD  ClosureData;
 RD  CallResult;
 CF  ClosureFrom;
 
 closure_method cMethod;
 public:
 TClosureMethod(closure_method ,UD,CF,CD);
 TClosureMethod():cMethod (NULL){}
 void  __fastcall SetUserData(UD ud){UserData = ud;};
 void  __fastcall SetClosureData(CF cf,CD cd){ClosureFrom,cf,ClosureData = cd;};
 void  __fastcall SetClosureMethod(closure_method cm){cMethod = cm;};
 void  __fastcall SetClosure(UD ud,closure_method cm,CF cf = 0,CD cd = 0){UserData=ud;cMethod = cm;ClosureFrom = cf;ClosureData = cd;};
 RD &  __fastcall GetResult(){return CallResult;};
 RD &  __fastcall CallMethod();
 RD &  __fastcall CallMethod(CF cf ,CD cd);
 bool  __fastcall HaveMethod(){return cMethod ? true:false;}
 closure_method __fastcall GetMethod(){return cMethod;}
 RD & operator()(CF cf ,CD cd){return CallMethod(cf,cd);}
};



template <class UD,class CF,class CD,class RD>
   TClosureMethod<UD,CF,CD,RD>::TClosureMethod(closure_method cm ,UD ud ,CF cf,CD cd)
   {cMethod = cm;UserData = ud;ClosureFrom = cf,ClosureData = cd;}

template <class UD,class CF,class CD, class RD>
 RD &  __fastcall TClosureMethod<UD,CF,CD,RD>::CallMethod()
 {
     if(cMethod)
     {
       __try{
       CallResult = cMethod(UserData,ClosureFrom,ClosureData);
            }
            __except(EXCEPTION_EXECUTE_HANDLER){return CallResult;}

      }

     return CallResult;

 }

 template <class UD,class CF,class CD,class RD>
 RD &  __fastcall TClosureMethod<UD,CF,CD,RD>::CallMethod(CF cf,CD cd)
 {
     if(cMethod)
      {
       __try{
       CallResult = cMethod(UserData,cf,cd);
            }
            __except(EXCEPTION_EXECUTE_HANDLER){return CallResult;};
      }
     return CallResult;
 }

 typedef TClosureMethod<LPVOID,LPVOID> TStdClosure;

#endif