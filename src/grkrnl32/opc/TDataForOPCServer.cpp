
#include <uniopc.h>
#include <fstream>
#include <strstream>

  CRITICAL_SECTION *   TDataForOPCServer:: _locker = getTagLock() ;

  void __fastcall TDataForOPCServer::lock()
  {
   if(_locker)
      EnterCriticalSection(_locker);
  }

  void __fastcall TDataForOPCServer::unlock()
  {
   if(_locker)
      LeaveCriticalSection(_locker);
  }

  TDataForOPCServer::TDataForOPCServer()
  {
   ZeroMemory(this,sizeof(*this));
   
  }


  template <typename T>
  void __fastcall named_tags(int count,T * array,char * name_templ,char * name_prefix = 0)
  {
   char tag_name[MAX_PATH];
   int  pr_len = name_prefix ? lstrlen(name_prefix):0;
   if(pr_len)
      {
       lstrcpy(tag_name,name_prefix);
       tag_name[pr_len++] = '.';
      } 
   char * _tg_name = tag_name+pr_len;
   for(int i =0;i<count && array;i++,array++)
   {
    sprintf(_tg_name,"%s%d",name_templ,i );
    array->setName(tag_name);
   }
  }

  TDataForOPCServer::TDataForOPCServer(const char * dev_name,WORD anTagsCnt,WORD bitTagsCnt,WORD intTagsCnt,char * tag_name_prefix)
  {
   ZeroMemory(this,sizeof(*this));
   structVersionNumber = 4;
   dataIsValid   = FALSE;
   safe_strcpyn(DeviceName,dev_name,sizeof(DeviceName)-1);

   anTagsCount = anTagsCnt;
   if(anTagsCnt)
      {
       anTagsArray = new TAnTag[anTagsCnt];
       named_tags(anTagsCnt,anTagsArray,"analog",tag_name_prefix);
      }
   intTagsCount = intTagsCnt;
   if(intTagsCnt)
      {
       intTagsArray = new TIntTag[intTagsCnt];
       named_tags(intTagsCnt,intTagsArray,"integer",tag_name_prefix);
      }

   bitTagsCount = bitTagsCnt;
   if(bitTagsCnt)
     {
      bitTagsArray = new TBitTag[bitTagsCnt];
      named_tags(bitTagsCnt,bitTagsArray,"bit",tag_name_prefix);
     }

  }

  void __fastcall TDataForOPCServer::release_tags_arrays()
  {
   lock();
   if(anTagsArray)
      delete [] anTagsArray;
   anTagsCount = 0;
   anTagsArray = NULL;

   if(bitTagsArray)
      delete [] bitTagsArray;
   bitTagsCount = 0;
   bitTagsArray = NULL;

   if(intTagsArray)
      delete [] intTagsArray;
   intTagsCount = 0;
   intTagsArray = NULL;
   unlock();
  }

  TDataForOPCServer::TDataForOPCServer(const TDataForOPCServer & src)
  {
   lock();
   ZeroMemory(this,sizeof(*this));
   (*this) = src;
    unlock();
  }

  TDataForOPCServer & TDataForOPCServer::operator = (const TDataForOPCServer & src)
  {
   lock();
   release_tags_arrays();

   dataIsValid = src.dataIsValid;
   memcpy(DeviceName,src.DeviceName,sizeof(DeviceName));
   this->structVersionNumber = src.structVersionNumber;
   anTagsCount = src.anTagsCount;
   if(anTagsCount)
     {
      anTagsArray = new TAnTag[anTagsCount];
      if(anTagsArray)
      {
        std::copy(src.anTagsArray,src.anTagsArray+src.anTagsCount,anTagsArray);
      }
      else
      anTagsCount = 0;
     }

   bitTagsCount = src.bitTagsCount;
   if(bitTagsCount)
     {
      bitTagsArray = new TBitTag[bitTagsCount];
      if(bitTagsArray)
      {
      std::copy(src.bitTagsArray,src.bitTagsArray+src.bitTagsCount,bitTagsArray);
      }
      else
      bitTagsCount = 0;
     }

   intTagsCount = src.intTagsCount;
   if(intTagsCount)
     {
      intTagsArray = new TIntTag[intTagsCount];
      if(intTagsArray)
      {
       std::copy(src.intTagsArray,src.intTagsArray+src.intTagsCount,intTagsArray);
      }
      else
      intTagsCount = 0;
     }

   unlock();
   return *this;
  }

  /*
   ФайлКонфигурации

   Device=
   {
    name        = NameOfDevice1;
    analog_tags_count = 64;
    analog_kadr_id    = 4;
    int_tags_count    = 64;
    int_kadr_id       = 5;
    bit_tags_count    = 64;
    bit_kadr_id       = 6;

   }

   Device=
   {
    name        = NameOfDevice2;
    analog_tags_count = 32;
    analog_kadr_id    = 7;
    int_tags_count    = 0;
    int_kadr_id       = 0;
    bit_tags_count    = 128;
    bit_kadr_id       = 8;

   }
  */

  int __fastcall create_devices(char * config_filename)
  {
    std::fstream fs(config_filename);
    if(fs.is_open())
    {
     std::strstream str;
     while(!fs.eof())
     {
      char ch;
      fs>>ch;
      if(ch == ' ' || ch == '\r' || ch == '\n' || ch == '\t')
        continue;
      str<<ch;  
     }
    }
   return 0;
  }


