
#ifdef __WIN32__
  #ifndef STRICT
   #define STRICT
  #endif
 #include <windows.h>
#endif

#include <lzw.h>    
#include <string.h>
#include <stdlib.h>

#define CLEAR_TABLE 256    /* Code to flush the string table */
#define TERMINATOR  257    /* To mark EOF Condition, instead of MAX_VALUE */
#define FIRST_CODE  258    /* First available code for code_value table */
#define CHECK_TIME  100    /* Check comp ratio every CHECK_TIME chars input */

#define MAXVAL(n) (( 1 <<( n )) -1)   /* max_value formula macro */
#define  STACK_SIZE  4000

struct LZWState
{
   //For decode;
   unsigned short next_code;
   unsigned short new_code;
   unsigned short old_code;
   short character,clear_flag;          // Флаг очистки
   unsigned char *string;
   int WhyBreak;

   //For  encode

 unsigned short string_code;
 unsigned short index;
 int i,
 ratio_new,         // Коэффициент сжатия
 ratio_old;         //
 bool CopyEncode;
};



 void  __fastcall TLzw::OutByte(unsigned char b)
 {
  *OutputBuffer = (char)b;
  total_bytes_out++;
  bytes_out++;
  OutputBuffer++;
  TotalOut++;
 }

char *__fastcall TLzw::DecodeString(unsigned char *buffer, unsigned short code)
{
   int i=0;

   while(code > 255 ) {
      *buffer++ = append_character[code];
      code=prefix_code[code];
      if (i++ >= 4000 ) {
         throw (char*)("Ошибка при декодировании");
      }
   }
   *buffer=code;
   return(char*)(buffer);
}



 unsigned short __fastcall TLzw::GetNextByte()
 {
  if(InCount)
  {
   InCount--;
   bytes_in++;
   total_bytes_in++;
   if(ProgressFunction && ProgressTimes)
   {
    if(div(bytes_in,ProgressTimes).rem ==0)
      {
       ProgressFunction(progress_user_data,total_bytes_in,total_bytes_out);
      }
   }
   unsigned char * p = (unsigned char *)InputBuffer++;
   return (unsigned short)*p;
  }
  return TERMINATOR;
 }

#pragma warn - sig
#pragma warn - aus

unsigned short __fastcall TLzw::GetNextCode()
{
   unsigned short return_value = TERMINATOR;

   while (InCount && internal_bit_count <= 24 )
   {
      unsigned short rdByte = TERMINATOR;
      //if(InCount)
      {
       rdByte = *((unsigned char*)InputBuffer);
       InputBuffer++;
       InCount--;
       bytes_in++;
       total_bytes_in++;

       if(ProgressFunction && ProgressTimes)
       {
        if(div(bytes_in,ProgressTimes).rem ==0)
          {
           ProgressFunction(progress_user_data,total_bytes_in,total_bytes_out);
          }
       }
      }
      //else
       //break;
     internal_bit_buffer |= ((unsigned long) (rdByte)) << (24 - internal_bit_count);
     internal_bit_count += 8;
   }
   if(internal_bit_count>=num_bits)
   {
    return_value=internal_bit_buffer >> (32-num_bits);
    internal_bit_buffer <<= num_bits;
    internal_bit_count -= num_bits;
   }
   else
    return_value = TERMINATOR;

   return return_value;
}

unsigned long __fastcall TLzw::Decode(char * In,unsigned long len,char * Out,unsigned long aOutSize,bool Continue)
{
   unsigned short next_code;
   unsigned short new_code;
   unsigned short old_code;
   short character,clear_flag=0;
   unsigned char *string;

 TotalOut  = 0;
 InputBuffer = In;OutputBuffer = Out;
 InCount = len;
 OutSize = aOutSize;
 LZWState * st = (LZWState*)state;
 int OutBufferExceed = 0;


  if(!DecodeProcess && !Continue)
  {
  memset(code_value,0,TABLE_SIZE*sizeof(unsigned short));
  memset(prefix_code,0,TABLE_SIZE*sizeof(unsigned short));
  memset(append_character,0,TABLE_SIZE);
  memset(decode_stack,0,STACK_SIZE);


  memset(st,0,sizeof(*st));
  //code_value = 0;prefix_code = 0;append_character=0;
  num_bits = INIT_BITS;
  max_code = MAXVAL(num_bits);
  bytes_out = bytes_in = 0;
  total_bytes_out = total_bytes_in = 0;
  if(ProgressFunction)
     ProgressFunction(progress_user_data,0,0);

  internal_bit_count=0;
  internal_bit_buffer=0L;
  TotalOut = 0;
  DecodeProcess = true;

   next_code=FIRST_CODE;
   clear_flag=0;
   }
   else
   {
      next_code = st->next_code;
      new_code  = st->new_code;
      old_code  = st->old_code;
      character = st->character;
      clear_flag= st->clear_flag;
      string    = st->string;
    switch(st->WhyBreak)
    {
     case 1:OutByte(old_code);break;
     case 2:
     {
      while (string >= decode_stack)
         {
          if(TotalOut<OutSize)
             OutByte(*string--);            //здесь проверка
             else
             {OutBufferExceed = 2;break;}
         }

      if(!OutBufferExceed)
      {
      if (next_code <= max_code)
      {      /* Add to string table if not full */
         prefix_code[next_code]=old_code;
         append_character[next_code++]=character;
         if (next_code == max_code && num_bits < MAX_BITS)
             max_code = MAXVAL(++num_bits);
      }
      old_code=new_code;
     }
     }
     break;
    }
   }



   while(!OutBufferExceed && (new_code=GetNextCode()) != TERMINATOR)
   {
      if (clear_flag)
      {       /* Initialize or Re-Initialize */
         clear_flag=0;
         old_code=new_code;   /* The next three lines have been moved */
         character=old_code;  /* from the original */
         if(TotalOut<OutSize)
            OutByte(old_code);
           else
            OutBufferExceed = 1;
         continue;
      }
      if (new_code == CLEAR_TABLE)
      {     /* Clear string table */
         clear_flag=1;
         num_bits=INIT_BITS;
         next_code=FIRST_CODE;
         max_code = MAXVAL(num_bits);
	 //memset(code_value,-1,TABLE_SIZE*sizeof(unsigned short));

         continue;
      }
      if (new_code >= next_code)
      {       /* Check for string+char+string */
         *decode_stack=character;
         string=(unsigned char*)DecodeString(decode_stack+1,old_code);
      }
      else
         string=(unsigned char*)DecodeString(decode_stack,new_code);

      character = *string;              /* Output decoded string in reverse */
      while (string >= decode_stack)
         {
          if(TotalOut<OutSize)
             OutByte(*string--);            //здесь проверка
             else
             {OutBufferExceed = 2;break;}
         }

      if(!OutBufferExceed)
      {
      if (next_code <= max_code)
      {      /* Add to string table if not full */
         prefix_code[next_code]=old_code;
         append_character[next_code++]=character;
         if (next_code == max_code && num_bits < MAX_BITS)
             max_code = MAXVAL(++num_bits);
      }
      old_code=new_code;
     }
   }

      st->next_code =  next_code;
      st->new_code  =  new_code;
      st->old_code  =  old_code;
      st->character =  character;
      st->clear_flag=  clear_flag;
      st->string    =  string;
      st->WhyBreak = OutBufferExceed;

  if(OutBufferExceed)
   {

      if(FlushFunction)
        {
         FlushFunction(user_data,Out,TotalOut);
         return Decode(InputBuffer,InCount,Out,OutSize,Continue);
        }

   }
   else
   {
    DecodeProcess = false;
      if(FlushFunction)
        FlushFunction(user_data,Out,TotalOut);
      if(ProgressFunction)
        ProgressFunction(progress_user_data,total_bytes_in,total_bytes_out);

   }
 return TotalOut;
}


unsigned long  __fastcall TLzw::Encode(char * InBuffer,unsigned long len,char *OutBuffer,unsigned long aOutSize,bool Continue)
{

 //Начало;
 LZWState * st = (LZWState*)state;
 unsigned short next_code;
 unsigned short character;
 unsigned short string_code;
 unsigned short index;
 int i,             // All purpose integer
 ratio_new,         // New compression ratio as a percentage
 ratio_old;     // Original ratio at 100%

 InputBuffer = InBuffer;OutputBuffer = OutBuffer;
 InCount = len;

 OutSize = aOutSize;
 TotalOut = 0;

 if(!EncodeProcess && !Continue)
 {
  //code_value = 0;prefix_code = 0;append_character=0;
  bytes_out = bytes_in = 0;
  total_bytes_out = total_bytes_in = 0;
  if(ProgressFunction)
     ProgressFunction(progress_user_data,0,0);
  next_code = FIRST_CODE;
  num_bits = INIT_BITS;
  max_code = MAXVAL(num_bits);
  EncodeProcess = true;
   internal_bit_count=0;
   internal_bit_buffer=0L;
  ratio_old = 100;
  memset(code_value,-1,sizeof(short)*TABLE_SIZE);
  //memset(prefix_code,-1,sizeof(short)*TABLE_SIZE);
  memset(st,0,sizeof(*st));
  OutCode(CLEAR_TABLE);
  string_code=GetNextByte();     // Get the first code

 }
 else
 {

   next_code   =st->next_code;
   character   =st->character;
   string_code =st->string_code;
   index       =st->index    ;
   i           =st->i        ;
   ratio_new   =st->ratio_new;
   ratio_old   =st->ratio_old;
 }

 bool OutBufExceed = false;
 //Процесс пошел
   while((OutBufExceed = bool(!((TotalOut+8) < OutSize)))==false &&
          (character=GetNextByte()) != (unsigned)TERMINATOR) {
      index=FindMatch(string_code,character);
      if (code_value[index] != -1)
         string_code=code_value[index];
      else {
         if (next_code <= max_code ) {
            code_value[index]=next_code++;
            prefix_code[index]=string_code;
            append_character[index]=character;
         }
         OutCode(string_code);   // Send out current code
         string_code=character;
         if (next_code > max_code) {      // Is table Full?
            if ( num_bits < MAX_BITS)      // Any more bits?
               max_code = MAXVAL(++num_bits);  // Increment code size then
            else if (bytes_in > checkpoint) {         // At checkpoint?
               if (num_bits == MAX_BITS ) {
                ratio_new = bytes_out*100/bytes_in; // New compression ratio
                if (ratio_new > ratio_old) {        // Has ratio degraded?
                  OutCode(CLEAR_TABLE);        // YES,flush string table
                  num_bits=INIT_BITS;
                  next_code=FIRST_CODE;        // Reset to FIRST_CODE
                  max_code = MAXVAL(num_bits); // Re-Initialize this stuff
                  bytes_in = bytes_out = 0;
                  ratio_old=100;               // Reset compression ratio
                  //for (i=0;i<TABLE_SIZE;i++)   // Reset code value array
                        //code_value[i]=-1;
                  memset(code_value,-1,sizeof(code_value[0])*TABLE_SIZE);      
               }
               else                                // NO, then save new
               ratio_old = ratio_new;            // compression ratio
            }
            checkpoint = bytes_in + CHECK_TIME;    // Set new checkpoint
            }
         }
      }
   }
    //Сохранить состояние
      st->next_code   =next_code;
      st->character   =character;
      st->string_code =string_code;
      st->index       =index    ;
      st->i           =i        ;
      st->ratio_new   =ratio_new;
      st->ratio_old   =ratio_old;

   if(!OutBufExceed)
   {
    EncodeProcess = false;
      if(FlushFunction)
           FlushFunction(user_data,OutBuffer,TotalOut);

      if(ProgressFunction)
        ProgressFunction(progress_user_data,total_bytes_in,total_bytes_out);
   }
   else
   {
      if(FlushFunction)
      {
       FlushFunction(user_data,OutBuffer,TotalOut);
       return Encode(InputBuffer,InCount,OutBuffer,OutSize,Continue);
      }
   }
    //Завершение
 return TotalOut;
}




unsigned long __fastcall TLzw::StopEncode(char * Out)
{
 EncodeProcess = false;
 TotalOut = 0;
 OutputBuffer = Out;
 LZWState * st = (LZWState*)state;
   OutCode(st->string_code);   /* Output the last code */
   if (st->next_code == max_code)         /* Handles special case for bit */
    {
       ++num_bits;                     /* increment on EOF */
      max_code = MAXVAL(num_bits);    /* Re-Initialize this stuff */
    }
 OutCode(TERMINATOR);
 OutCode(TERMINATOR);
 Flush();
   if(FlushFunction)
       FlushFunction(user_data,OutputBuffer-TotalOut,TotalOut);
   if(ProgressFunction)
      ProgressFunction(progress_user_data,total_bytes_in,total_bytes_out); 
 FlushFunction = 0;
 ProgressFunction = 0;

 return TotalOut;
}

short __fastcall TLzw::FindMatch(short hash_prefix, unsigned short hash_character)
{
   short index, offset;

   index = (short)(hash_character << HASHING_SHIFT ) ^ hash_prefix;
   if (index == 0 )
      offset=1;
   else
      offset = TABLE_SIZE - index;
   while(1)
   {
      if (code_value[index] == -1 )
         return(index);
      if (prefix_code[index] == hash_prefix &&
               append_character[index] == hash_character)
         return(index);
      index -= offset;
      if (index < 0)
         index += TABLE_SIZE;
   }
}


void __fastcall TLzw::OutCode(unsigned short code)
{

   internal_bit_buffer |= (unsigned long) code << (32 - num_bits
   							- internal_bit_count);
   internal_bit_count += num_bits;
   while (internal_bit_count >= 8)
   {
      *OutputBuffer = (char)((internal_bit_buffer >> 24)&0xFF);

       internal_bit_buffer <<= 8;
       internal_bit_count -= 8;
      OutputBuffer++;
      bytes_out++;                    /* ADDED for compression monitoring */
      total_bytes_out++;
      TotalOut++;
   }
}


void __fastcall TLzw::SetBitLimit( lint dp)
{
  if(!IsBusy())
   {
     switch(dp)
     {
      case 13: MAX_BITS = bDeep13;TABLE_SIZE=9029;break;
      case 14: MAX_BITS = bDeep14;TABLE_SIZE=18041;break;
      default: MAX_BITS = bDeep12;TABLE_SIZE=5021;break;
     }
     HASHING_SHIFT = int(MAX_BITS)-8;
     #if defined __WIN32__
      code_value =  (short*)LocalLock(LocalAlloc(LMEM_ZEROINIT,TABLE_SIZE*sizeof(short)));
      prefix_code = (unsigned short* )LocalLock(LocalAlloc(LMEM_ZEROINIT,TABLE_SIZE*sizeof(unsigned short)));
      append_character = (unsigned char *)LocalLock(LocalAlloc(LMEM_ZEROINIT,TABLE_SIZE*sizeof(unsigned char)));
     #else
      code_value = new short[TABLE_SIZE];
      prefix_code = new unsigned short [TABLE_SIZE];
      append_character = new unsigned char [TABLE_SIZE];
     #endif
   }
}


void __fastcall TLzw::FreeBuffers()
{
     #if defined __WIN32__
      HANDLE mh;
      if(append_character)
      { mh = LocalHandle(append_character);LocalUnlock(mh);LocalFree(mh);append_character = 0;}
      if(prefix_code)
      { mh = LocalHandle(prefix_code);LocalUnlock(mh);LocalFree(mh);prefix_code = 0;}
      if(code_value)
      {mh = LocalHandle(code_value);LocalUnlock(mh);LocalFree(mh);code_value = 0;}

     #else
      if(append_character)
      { delete []append_character;append_character = 0;}
      if(prefix_code)
      { delete []prefix_code;prefix_code = 0;}
      if(code_value)
      {delete []code_value;code_value = 0;}

     #endif
}


 void __fastcall TLzw::Flush()
 {
  while(internal_bit_count>=8)
     OutCode(0);
 }

TLzw::TLzw(lint bd)
{
 user_data = 0;
 FlushFunction = 0;
 ProgressFunction = 0;
 progress_user_data = 0;
 ProgressTimes = 0;
 EncodeProcess=DecodeProcess=false;
 InputBuffer = OutputBuffer = 0;
 InCount = 0;
 bytes_in=bytes_out = 0;
 total_bytes_in=total_bytes_out = 0;

 code_value = 0;prefix_code = 0;append_character=0; num_bits = INIT_BITS ;
 checkpoint = CHECK_TIME;
 SetBitLimit(bd);
 #if defined __WIN32__
 decode_stack = (unsigned char *)LocalLock(LocalAlloc(LMEM_ZEROINIT,4016));
 state = LocalLock(LocalAlloc(LMEM_ZEROINIT,sizeof(LZWState)));
 #else
 decode_stack = new unsigned char[4016];
 state = (void*) new TLZWState;
 #endif

}

TLzw::~TLzw()
{
 DecodeProcess = EncodeProcess = false;
 FreeBuffers();
#if defined __WIN32__
 HANDLE hMem = LocalHandle(state);
 LocalUnlock(hMem);LocalFree(hMem);
 hMem = LocalHandle(decode_stack);
 LocalUnlock(hMem);LocalFree(hMem);
#else
 delete state;
 delete decode_stack;
#endif
}


