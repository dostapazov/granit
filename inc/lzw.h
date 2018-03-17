
#if !defined ( __LZWIMP_INC__ )
 #define __LZWIMP_INC__

#define INIT_BITS 9

#if defined (WIN32) && !defined(__WIN32__)
#define __WIN32__
#endif

#ifndef __WIN32__
#define __stdcall __cdecl
#endif

#ifndef __FLAT__
typedef long int lint;
#else
typedef  int lint;
#endif


#ifdef __WIN32__
  #ifndef STRICT
   #define STRICT
  #endif
 #include <windows.h>
#endif

typedef unsigned long (__stdcall *LZWFlushFunction   )( void * userdata,char * lzw_data,unsigned long lzw_data_len);
typedef void          (__stdcall *LZWProgressFunction)( void * userdata,unsigned long bytes_in,unsigned long bytes_out);

#pragma pack (push,8)


enum TBitDeep {bDeep12=12,bDeep13,bDeep14};
class TLzw
{
 protected:
 LZWFlushFunction  FlushFunction;
 void * user_data;
 LZWProgressFunction ProgressFunction;
 void * progress_user_data;
 lint   ProgressTimes;                      //Через сколько байт
 void * state;
 TBitDeep  MAX_BITS;                       // Максимальная битовая глубина
 lint      HASHING_SHIFT;
 lint      TABLE_SIZE;                     // размер таблицые
 short          * code_value;              // таблица кодов
 unsigned short * prefix_code;             // таблица префиксов
 unsigned char  * append_character;        // таблица суффиксов
 unsigned char  * decode_stack;            // стек для декодирования

 lint internal_bit_count;
 unsigned long internal_bit_buffer;

 lint num_bits;                             // Текущая битовая глубина
 unsigned long bytes_in,bytes_out;
 unsigned long checkpoint;
 lint max_code;

 bool EncodeProcess;
 bool DecodeProcess;
 char *InputBuffer;
 char *OutputBuffer;
 unsigned long InCount;
 unsigned long OutSize;
 unsigned long TotalOut;
 unsigned long total_bytes_in,total_bytes_out;

 void  __fastcall FreeBuffers();
 void  __fastcall OutCode(unsigned short);
 short __fastcall FindMatch(short hash_prefix, unsigned short hash_character);
 void  __fastcall Flush();
 char *__fastcall DecodeString(unsigned char *buffer, unsigned short code);
 void  __fastcall OutByte(unsigned char b);
 unsigned short __fastcall GetNextCode();
 unsigned short __fastcall GetNextByte();
 public:
    TLzw(lint BitLimit = 12);
    ~TLzw();
 unsigned long  __fastcall Encode(char * InBuffer,unsigned long len,char *OutBuffer,unsigned long OutSize,bool Continue = false);
 unsigned long  __fastcall Decode(char * InBuffer,unsigned long len,char *OutBuffer,unsigned long OutSize,bool Continue = false);
 unsigned long  __fastcall StopEncode(char * Out);
 bool __fastcall IsBusy();
 void __fastcall SetFlushFunction   (LZWFlushFunction ff,void * user_data);
 void __fastcall SetProgressFunction(LZWProgressFunction pf,lint Times,void * pfud);
 unsigned long __fastcall GetInputRemain();
 char *        __fastcall GetInputBuffer();
 unsigned long __fastcall GetOutputRemain();
 char *        __fastcall GetOutputBuffer();
 void __fastcall Reset();
 lint  __fastcall GetBitLimit();
 void  __fastcall SetBitLimit(lint deep);
};

inline bool __fastcall TLzw::IsBusy()
{
 return bool(DecodeProcess == true || EncodeProcess == true);
}

inline lint __fastcall TLzw::GetBitLimit()
{ return lint(MAX_BITS);}

 inline void __fastcall TLzw::SetFlushFunction(LZWFlushFunction ff,void * data)
 {
  FlushFunction = ff;user_data = data;
 }

 inline void __fastcall TLzw::SetProgressFunction(LZWProgressFunction ff,lint Times,void * data)
 {
  ProgressFunction = ff;progress_user_data = data;
  ProgressTimes = Times;
 }

 inline unsigned long __fastcall TLzw::GetInputRemain()
 {
  return InCount;
 }

 inline unsigned long __fastcall TLzw::GetOutputRemain()
 {
  return OutSize;
 }

 inline char *        __fastcall TLzw::GetInputBuffer()
 {return InputBuffer;}

 inline char *        __fastcall TLzw::GetOutputBuffer()
 {return InputBuffer;}

 inline  void __fastcall TLzw::Reset()
 {
  EncodeProcess=DecodeProcess=false;
  FlushFunction = 0;user_data = 0;
  ProgressFunction = 0;progress_user_data = 0;
 }

//#if !defined (USEPACKAGES)
#pragma pack(pop)
//#endif
#endif
