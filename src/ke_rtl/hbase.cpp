 #include <Kertl.hpp>

 #ifndef _NO_NAMESPACE
 namespace KeRTL
 {
 #endif


    /*inline void * TKeRTLBase::operator new (size_t sz)
    {
     TKeRTLBase * ptr = (TKeRTLBase *)::new char[sz];
     if(ptr)
       ptr->Flags = FLBASE_NEWALLOC;
     return ptr;
    }

    inline void  TKeRTLBase::operator delete(void * ptr)
    {
      Release(*((TKeRTLBase*)ptr));
    }
    */


 void __fastcall THandleBased::Close()
 {
   __try{
  if(IsValid())
    {
     CloseHandle(Handle);
     Handle = INVALID_HANDLE_VALUE;
     }
    }
    __except(EXCEPTION_EXECUTE_HANDLER){
        Handle = INVALID_HANDLE_VALUE;
        GetLastError();
    };

}

 #ifndef _NO_NAMESPACE
 //namespace KeRTL
 }
 #endif
