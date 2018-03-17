#ifdef __linux__
 #include <gklib/otd_proto.h>
#else
 #include <otd_proto.h>
#endif

 void    WINAPI otd_code_proc        (void * data,unsigned int sz,void * key,unsigned int key_size)
 {
    //Ўифруетс€ исключающим или по ключу
    //чет текущего значение
     if(data && sz && key && key_size)
     {
      LPBYTE d_ptr = (LPBYTE)data;
      LPBYTE d_end = d_ptr+sz;
      DWORD  curr = 0;
      LPBYTE key_ptr = (LPBYTE)key;
      LPBYTE key_end = key_ptr+key_size;
      bool   dir  = true;
      do
      {
       BYTE key_val = *key_ptr;
       if(!key_val || (curr&1))
           {
            key_val = ~key_val;
           }
       (*d_ptr)^=key_val;
       ++curr;
       key_ptr+= dir ? 1:-1;
       if((dir && key_ptr == key_end) || (!dir && key_ptr == key))
         {
          dir = !dir; if(!dir) --key_ptr;
         }
       ++d_ptr;
      }while(d_ptr<d_end);
     }
 }

