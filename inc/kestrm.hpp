#ifndef  _KE_STREAM_H_
#define  _KE_STREAM_H_



#include <kertlio.hpp>

#ifndef NO_KERTL_STREAM_WITH_STD_STRING
#include <string>
#endif




#ifndef _NO_NAMESPACE
namespace KeRTL
{
#endif

KERTL_CLASS DECLSPEC_NOVTABLE TStream
{
 protected:
 DWORD      inc_by;
 DWORD      stream_size;      //размер выделенной пам€ти
 DWORD      using_size;       //количество байт записанных в поток
 DWORD      stream_position;  //“екуща€ позици€

 public :
 enum SeekMethod {smFromBegin = FILE_BEGIN ,smFromCurr= FILE_CURRENT,smFromEnd=FILE_END};
 virtual int    __fastcall Write(void * data,DWORD size,bool Wait = true)=0;
 virtual int    __fastcall Read (void * buffer,DWORD sz,bool Wait = true)=0;
 virtual DWORD  __fastcall Seek(DWORD pos,SeekMethod sm)=0;
 virtual DWORD  __fastcall GetPosition()=0;
 virtual DWORD  __fastcall WaitEndWrite(DWORD to=INFINITE){return true;}
 virtual DWORD  __fastcall WaitEndRead (DWORD to=INFINITE){return true;}
 virtual bool   __fastcall IsGood();
 virtual void   __fastcall Flush (){};
 virtual DWORD  __fastcall GetSize()=0;
 virtual DWORD  __fastcall Reserve(DWORD sz) = 0;
 virtual bool   __fastcall SetSize(DWORD size) = 0;
         void   __fastcall Rewind(){Seek(0,smFromBegin);}
         bool   __fastcall Eos(){return GetPosition()>=GetSize() ? true:false;}
         DWORD  __fastcall get_using_size(){return using_size;}         
         DWORD  __fastcall get_stream_size(){return stream_size;}
         void   __fastcall set_inc_by(DWORD n_val){inc_by = n_val;}         
 virtual ~TStream(){Flush();};
      int  read_string  (wchar_t *& str);
      int  read_string  (char    *& str);
#ifndef NO_KERTL_STREAM_WITH_STD_STRING

      int  read_string  (std::string  & str);
      int  read_string  (std::wstring & str);
      void write_string (const std::string & str);
      void write_string (const std::wstring & str);
#endif
      void write_string (const wchar_t * str);
      void write_string (const char    * str);

};


KERTL_CLASS TFileStream:public TStream,protected TFile
{
 public:
 TFileStream(const char    * file_name,bool read_only = false):
 TFile(file_name,(read_only ? OPEN_EXISTING:OPEN_ALWAYS),FILE_SHARE_READ,
                 GENERIC_READ | (read_only ? 0:GENERIC_WRITE))
 { inc_by = 16384;  using_size = stream_size = TFile::GetSize(); Seek(0,smFromBegin); }

 TFileStream(const wchar_t * file_name,bool read_only = false):
 TFile(file_name,(read_only ? OPEN_EXISTING:OPEN_ALWAYS),FILE_SHARE_READ,
                 GENERIC_READ | (read_only ? 0:GENERIC_WRITE))
 {  inc_by = 16384; using_size = stream_size = DWORD(TFile::GetSize());Seek(0,smFromBegin); }
 virtual ~TFileStream(){TFile::SetSize(using_size);Flush();Close();}

 int    __fastcall Write(void * data,DWORD size,bool Wait = true);
 int    __fastcall Read (void * buffer,DWORD sz,bool Wait = true);
 DWORD  __fastcall Seek(DWORD pos,SeekMethod sm);
 DWORD  __fastcall WaitEndWrite(DWORD to=INFINITE){return true;}
 DWORD  __fastcall WaitEndRead (DWORD to=INFINITE){return true;}
 bool   __fastcall IsGood(){return IsValid();}
 void   __fastcall Flush (){TFile::Flush();}
 DWORD  __fastcall GetPosition(){ return stream_position;}
 DWORD  __fastcall GetSize(){return using_size;}
 DWORD  __fastcall Reserve(DWORD sz) ;
 bool   __fastcall SetSize(DWORD size);  

};

KERTL_CLASS TMemoryStream:public TStream,protected THeap
{
 protected:
 BYTE  *    __memptr;  //
 bool       __fastcall resize();
 public:
 TMemoryStream(DWORD   InitSize = 32768,DWORD IncBy = 8192);
 TMemoryStream(LPVOID  src,DWORD cbSize,   DWORD IncBy = 8192,bool no_alloc = false);
 int    __fastcall Write(void * data,DWORD size,bool Wait = true);
 int    __fastcall Read (void * buffer,DWORD sz,bool Wait = true);
 DWORD  __fastcall Seek (DWORD pos,SeekMethod sm);
 bool   __fastcall IsGood(){return __memptr ? true:false;}
 DWORD  __fastcall GetPosition(){return  stream_position;}
 DWORD  __fastcall GetSize(){return using_size;}
 const LPBYTE __fastcall GetMemory(){return __memptr;}
 void   __fastcall Erase(DWORD pos,DWORD len);
 DWORD  __fastcall Reserve(DWORD sz) ;
 bool   __fastcall SetSize(DWORD size);
};



//inlines


//template <typename T>
//inline TStream & operator <<(TStream & s,T  t){s.Write(&t,sizeof(t));return s;}
//
//template <typename T>
//inline TStream & operator >>(TStream & s,T & t){s.Read(&t,sizeof(t));return s;}
//
//template <typename T>
//inline TFileStream & operator <<(TFileStream & s,T  t){s.Write(&t,sizeof(t));return s;}
//
//template <typename T>
//inline TFileStream & operator >>(TFileStream & s,T & t){s.Read(&t,sizeof(t));return s;}


inline TStream & operator <<(TStream & s,unsigned char  t )  {s.Write(&t,sizeof(t)) ;return s;}
inline TStream & operator >>(TStream & s,unsigned char & t)   {s.Read(&t,sizeof(t)) ;return s;}
inline TStream & operator <<(TStream & s,int  t)              {s.Write(&t,sizeof(t));return s;}
inline TStream & operator >>(TStream & s,int & t)             {s.Read(&t,sizeof(t)) ;return s;}
inline TStream & operator <<(TStream & s,unsigned int  t)     {s.Write(&t,sizeof(t));return s;}
inline TStream & operator >>(TStream & s,unsigned int & t)    {s.Read(&t,sizeof(t)) ;return s;}
inline TStream & operator <<(TStream & s,unsigned long  t)    {s.Write(&t,sizeof(t));return s;}
inline TStream & operator >>(TStream & s,unsigned long & t)   {s.Read(&t,sizeof(t)) ;return s;}
inline TStream & operator <<(TStream & s,long  t)             {s.Write(&t,sizeof(t));return s;}
inline TStream & operator >>(TStream & s,long & t)            {s.Read(&t,sizeof(t)) ;return s;}
inline TStream & operator <<(TStream & s,short  t)            {s.Write(&t,sizeof(t));return s;}
inline TStream & operator >>(TStream & s,short & t)           {s.Read(&t,sizeof(t)) ;return s;}
inline TStream & operator <<(TStream & s,unsigned short  t)   {s.Write(&t,sizeof(t));return s;}
inline TStream & operator >>(TStream & s,unsigned short & t)  {s.Read(&t,sizeof(t)) ;return s;}
inline TStream & operator <<(TStream & s,float  t)            {s.Write(&t,sizeof(t));return s;}
inline TStream & operator >>(TStream & s,float & t)           {s.Read(&t,sizeof(t)) ;return s;}
inline TStream & operator <<(TStream & s,double  t)           {s.Write(&t,sizeof(t));return s;}
inline TStream & operator >>(TStream & s,double & t)          {s.Read(&t,sizeof(t)) ;return s;}
inline TStream & operator <<(TStream & s,long double  t)      {s.Write(&t,sizeof(t));return s;}
inline TStream & operator >>(TStream & s,long double & t)     {s.Read(&t,sizeof(t)) ;return s;}
inline TStream & operator <<(TStream & s,__int64  t)          {s.Write(&t,sizeof(t));return s;}
inline TStream & operator >>(TStream & s,__int64 & t)         {s.Read(&t,sizeof(t)) ;return s;}
inline TStream & operator <<(TStream & s,unsigned __int64  t) {s.Write(&t,sizeof(t));return s;}
inline TStream & operator >>(TStream & s,unsigned __int64 & t){s.Read(&t,sizeof(t)) ;return s;}

#ifndef NO_KERTL_STREAM_WITH_STD_STRING

inline TStream & operator >>(TStream & s,      std::string & str) { s.read_string(str);return s; }
inline TStream & operator >>(TStream & s,      std::wstring & str){ s.read_string(str);return s; }

inline TStream & operator <<(TStream & s,const std::string & str) { s.write_string(str);return s; }
inline TStream & operator <<(TStream & s,const std::wstring & str){ s.write_string(str);return s; }

#endif


inline void TStream::write_string (const wchar_t * str)
{
 DWORD len = str ? lstrlenW(str):0;
 Write(&len,sizeof(len));
 Write((void*)str,len*sizeof(wchar_t));
}

inline void TStream::write_string (const char    * str)
{
 DWORD len = str ? lstrlenA(str):0;
 Write(&len,sizeof(len));
 Write((void*)str,len);
}

#ifndef NO_KERTL_STREAM_WITH_STD_STRING
inline void TStream::write_string (const std::string & str)
{
 DWORD len = str.length();
 Write(&len,sizeof(len));
 Write((void*)str.c_str(),len);
}

inline void TStream::write_string (const std::wstring & str)
{
 DWORD len = str.length();
 Write(&len,sizeof(len));
 Write((void*)str.c_str(),len*sizeof(*str.c_str()));
}


inline int TStream::read_string  (std::string & str)
{
 DWORD len(0);
 Read(&len,sizeof(len));
 str.resize(len+1);
 Read((void*)str.c_str(),len);
 return len;
}

inline int TStream::read_string  (std::wstring & str)
{
 DWORD len(0);
 Read(&len,sizeof(len));
 str.resize(len+1);
 Read((void*)str.c_str(),len*sizeof(*str.c_str()));
 return len;
}

#endif

inline int TStream::read_string (wchar_t *& str)
{
 DWORD len(0);
 Read(&len,sizeof(len));
 if(str == 0 && len)
   str = new wchar_t[(len+1)];
 if(len)
 {
  Read(str,len*sizeof(wchar_t));
  str[len] = 0;
 }
 else
 str = 0;
 return len;
}

inline int TStream::read_string (char *& str)
{
 DWORD len(0);
 Read(&len,sizeof(len));
 if(str == 0 && len)
   str = new char[(len+1)];
 if(len)
 {
  Read(str,len);
  str[len] = 0;
 }
 else
 str = 0;
 return len;
}

//
//inline TStream & operator <<(TStream &s, const char * str)
//{
// DWORD len = str ? lstrlenA(str):0;
// s.Write(&len,sizeof(len));
// s.Write((void*)str,len);
// return s;
//}
//
//
//
//inline TStream & operator >>(TStream &s, char *& str)
//{
// DWORD len(0);
// s.Read(&len,sizeof(len));
// if(str == 0 && len)
//   str = new char[(len+1)];
// if(len)
// {
//  s.Read(str,len);
//  str[len] = 0;
// }
// else
// str = 0;
// return s;
//}
//
//inline TStream & operator >>(TStream &s, char ** str)
//{
// DWORD len(0);
// s.Read(&len,sizeof(len));
// if(*str == 0 && len)
//   *str = new char[(len+1)];
// if(len)
// {
//  s.Read(*str,len);
//  (*str)[len] = 0;
// }
// else
// *str = 0;
// return s;
//}
//
//
//inline TStream & operator <<(TStream &s,const wchar_t * str)
//{
// DWORD len = str ? lstrlenW(str):0;
// s.Write(&len,sizeof(len));
// s.Write((void*)str,len*sizeof(wchar_t));
// return s;
//}
//
//inline TStream & operator >>(TStream &s, wchar_t *& str)
//{
// DWORD len;
// s.Read(&len,sizeof(len));
// if(str == 0 && len)
//   str = new wchar_t[len+1];
// if(len)
//  s.Read(str,len*sizeof(wchar_t));
// str[len] = 0;
// return s;
//}
//
//inline TStream & operator >>(TStream &s, wchar_t ** str)
//{
// DWORD len(0);
// s.Read(&len,sizeof(len));
// if(*str == 0 && len)
//   *str = new wchar_t[(len+1)];
// if(len)
// {
//  s.Read(*str,len*sizeof(wchar_t));
//  (*str)[len] = 0;
// }
// else
// *str = 0;
// return s;
//}
//

#ifndef _NO_NAMESPACE
}//end of namespace
#endif


#endif
