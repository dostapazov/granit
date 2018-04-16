
#ifndef __GKOPC_CLIENT_MODEM_INC__
#define __GKOPC_CLIENT_MODEM_INC__

#include <modemmer.h>


 #ifndef RC_INVOKED

  #include <opcda.h>
  #include <otd_addr.h>

 #pragma pack(push,1)

 #define CCFL_PUNUMBER        0x800000
 #define CCFL_PUNAME          0x400000
 #define CCFL_LINES           0x200000

 #define REGDW_PUNUMBER       L"PuNumber"
 #define REGSTR_PUNAME        L"PuName"
 #define REGBIN_LINES         L"Lines"

 #define OPC_MOD_STREAM_VER_1   1
 #define OPC_MOD_STREAM_VER_2   2
 #define OPC_MOD_STREAM_VER_3   3
 #define OPC_MOD_STREAM_VER     4

 #define MAX_NAME_LEN   96
 #define MNF_LINE_ITEM_CHANGED    0x001000
 #define MNF_NOTIFY_SCRIPT_TRACE  0x000100
 #define MNF_NOTIFY_SCRIPT_ERROR  0x000200

 struct GKOPC_CLIENT_MOD_CONFIG
 {
   DWORD   dw_size;
   DWORD   modem_number;
   DWORD   pu_number;
   wchar_t pu_name[MAX_NAME_LEN];
   #ifdef __cplusplus
   GKOPC_CLIENT_MOD_CONFIG():dw_size(sizeof(*this)),modem_number(-1),pu_number(-1) {bzero(pu_name,sizeof(pu_name));}
   #endif
 };

 typedef GKOPC_CLIENT_MOD_CONFIG* LPGKOPC_CLIENT_MOD_CONFIG;

 #define GKOPCMODEM_MCD_SIZE ( sizeof(MODULE_CONFIG_DATA)+ sizeof(GKOPC_CLIENT_MOD_CONFIG)-sizeof(BYTE))

 struct GKOPC_LINE_CONFIG
 {
   DWORD   dw_size;
   DWORD   line_num;
   GUID    server_guid;
   DWORD   server_type;
   DWORD   rate;
   float   bandwidth;
   wchar_t cp_name       [MAX_NAME_LEN];
   wchar_t config_file   [MAX_PATH<<1];
   wchar_t server_host   [MAX_PATH];
   wchar_t server_prog_id[MAX_PATH];
   DWORD   options;
   #ifdef __cplusplus
   GKOPC_LINE_CONFIG(){bzero(this,sizeof(*this)); dw_size = sizeof(*this);}
   #endif

 };

 typedef GKOPC_LINE_CONFIG * LPGKOPC_LINE_CONFIG;

 #define GKOPC_LINE_OPT_REPLACE_TIME    0x00001 //Заменять время изменения данных на текущее

 #define CMD_OPC_GET_LINE   MCMD(MT_MODEM_OPC,1000)
 //get line config p1 = (LPGKOPC_LINE_CONFIG)

 #define CMD_OPC_ADD_LINE   MCMD(MT_MODEM_OPC,1001)
 //add new line config p1 = (LPGKOPC_LINE_CONFIG)

 #define CMD_OPC_UPD_LINE   MCMD(MT_MODEM_OPC,1002)
 //update line config p1 = (LPGKOPC_LINE_CONFIG) ,p2 = line_num

 #define CMD_OPC_DEL_LINE   MCMD(MT_MODEM_OPC,1003)
 //delete line  p1 = line number

 #define CMD_OPC_RESTART_LINE   MCMD(MT_MODEM_OPC,1004)
 //restart line  p1 = line number


 struct SCRIPT_TEXT
 {
   DWORD dw_size;
   DWORD text_capacity;
   LONG  text_len;
   char  text[ANYSIZE_ARRAY];
 };
 typedef SCRIPT_TEXT* LPSCRIPT_TEXT;

 #define CMD_OPC_LINE_GET_TU_SCRIPT MCMD(MT_MODEM_OPC,1010)
 #define CMD_OPC_LINE_SET_TU_SCRIPT MCMD(MT_MODEM_OPC,1011)
 #define CMD_OPC_LINE_GET_TR_SCRIPT MCMD(MT_MODEM_OPC,1020)
 #define CMD_OPC_LINE_SET_TR_SCRIPT MCMD(MT_MODEM_OPC,1021)
 //p1 - line-no ,p2 = LPSCRIPT_TEXT
 //Если text_len < 0 => недостаточная длина буфера


 #define CMD_OPC_LINE_GET_ITEM      MCMD(MT_MODEM_OPC,2000)
 //p1 LOWORD line_num,HIWORD item_idx , p2 = gkopc_item ** p_item ; on call
 #define CMD_OPC_LINE_RELEASE_ITEM  MCMD(MT_MODEM_OPC,2001)
 //p1 line_num  p2 = gkopc_item * ptr




  #pragma pack(pop)

  #ifdef __cplusplus
   #include <vector_index.hpp>
   #include <string>
   #include <storage.hpp>
   #include <kestrm.hpp>
   #include <opc_helper.hpp>
   #include <winnetwk.h>

#pragma pack (push,1)
    union group_param_t
    {
      struct
           {
             WORD param;
             BYTE sb;
             BYTE fa;
           };
      DWORD group_data;
      group_param_t():group_data(-1){}
      group_param_t(LPVOID data):group_data((DWORD)data){};
      group_param_t(DWORD  data):group_data(data){};
      group_param_t(BYTE _fa,BYTE _sb = -1,WORD _param = -1 ) :fa(_fa),sb(_sb),param(_param){}
      group_param_t(const group_param_t & src){group_data = src.group_data;}

      group_param_t & operator = (const group_param_t & src) {group_data = src.group_data; return *this;}
      sotd_param_addr  get_param_addr(){ sotd_param_addr pa(0,param); pa.addr.fa = fa;pa.addr.sb = sb; return pa; }
      DWORD operator ()() {return group_data; }

    };

    struct group_param_comparer
    {
      int compare (const group_param_t & gp1,const group_param_t & gp2)
      {
         if(gp1.fa == gp2.fa)
            {
             if(gp1.sb == gp2.sb)
                {
                  if(gp1.param == gp2.param) return 0;
                  return  (gp1.param < gp2.param) ? -1 : 1;
                }
              return  (gp1.sb < gp2.sb) ? -1 : 1;
            }
        return (gp1.fa < gp2.fa) ? -1 : 1;
      }
      bool less (const group_param_t & gp1,const group_param_t & gp2) { return compare(gp1,gp2)< 0   ? true : false ;}
      bool more (const group_param_t & gp1,const group_param_t & gp2) { return compare(gp1,gp2)> 0   ? true : false ;}
      bool equal(const group_param_t & gp1,const group_param_t & gp2) { return compare(gp1,gp2) == 0 ? true : false ;}
    };
#pragma pack (pop)


   #define TUTR_PREPERE_SUCCESS       0
   #define TUTR_PREPERE_FAIL          1
   #define TUTR_END_SUCCESS           2
   #define TUTR_UNDO_SUCCESS          3
   #define TUTR_UNDO_FAIL             4

   #define TUTR_CONTROL_VALUES_COUNT  5

   struct gkopc_item
   {
     std::wstring        id;
     std::wstring        access_path;
     group_param_t       group_param;
     opc::TOpcItemResult reg_result;
     opc::TOpcItemState  item_state;
     LONG                last_error;
     std::string         calc_script;

     DWORD               rc_state;
     DWORD               rc_object;
     std::wstring        rc_tag;
     std::string         rc_script;
     otd_tutr            rc_current;
     opc::TVariant       data_value;


     gkopc_item():rc_state(0),rc_object(-1){}
     gkopc_item(const std::wstring & _id):id(_id),rc_state(0),rc_object(-1){}
     gkopc_item(const std::wstring & _id,const std::wstring & _access_path, const group_param_t & _gp)
                  :id(_id),access_path(_access_path),group_param(_gp),rc_state(0),rc_object(-1)
                  {}
     gkopc_item(const wchar_t * _id,const wchar_t * _access_path, const group_param_t & _gp)
                  :id(_id),access_path(_access_path),group_param(_gp),rc_state(0),rc_object(-1)
                  {}

     gkopc_item(const DWORD fa,const DWORD grp, const DWORD param):group_param(fa,grp,param),rc_state(0),rc_object(-1) {}
     gkopc_item(const gkopc_item & src) {(*this) = src;}

     ~gkopc_item(){}

     gkopc_item & operator = (const gkopc_item & src)
     {
       id = src.id; access_path = src.access_path; group_param = src.group_param;
       rc_state    = src.rc_state;
       rc_object   = src.rc_object;
       rc_script   = src.rc_script;
       rc_current  = src.rc_current;
       rc_tag      = src.rc_tag;
       calc_script = src.calc_script;
       data_value  = src.data_value;
       return *this;
     }

     DWORD __fastcall get_personal_diag()
     {
      return ((item_state.wQuality&OPC_QUALITY_MASK) == OPC_QUALITY_GOOD) ? 0|rc_state : OTD_PDIAG_ERROR;
     }
   };

   struct gkopc_item_comparer
   {
     bool by_group_param;
     gkopc_item_comparer(bool _variant_compare = false):by_group_param(_variant_compare){}
     gkopc_item_comparer(const gkopc_item_comparer & src) {by_group_param = src.by_group_param;}

     int compare (const gkopc_item & v1 ,const gkopc_item & v2)
     {
        int ret = 0;
        if(!by_group_param)
            ret = wcscmp(v1.id.c_str(),v2.id.c_str());
        else
           ret = group_param_comparer().compare(v1.group_param,v2.group_param);
        return ret;
     }
     bool less (const gkopc_item & v1 ,const gkopc_item & v2){return compare(v1,v2) <  0 ? true : false;}
     bool more (const gkopc_item & v1 ,const gkopc_item & v2){return compare(v1,v2) >  0 ? true : false;}
     bool equal(const gkopc_item & v1 ,const gkopc_item & v2){return compare(v1,v2) == 0 ? true : false;}
   };

   struct gkopc_item_comparer_group_param:public gkopc_item_comparer
   {
      gkopc_item_comparer_group_param():gkopc_item_comparer(true){};
   };

   typedef vector_index::indexed_vector<gkopc_item,gkopc_item_comparer,gkopc_item_comparer_group_param> gkopc_items_t;


   #define GKOPC_ITEMS_IO_VER_1  1
   #define GKOPC_ITEMS_IO_VER    2

   struct  gkopc_items_io
   {
     char secret_word[4] ;
     gkopc_items_io(){memcpy(secret_word,"OPCI",sizeof(secret_word));}
     template <typename items_t,typename ioclass>
     DWORD __fastcall write(items_t & items,ioclass * io)
     {
        KeRTL::TStorage st(io);
        st.create(GKOPC_ITEMS_IO_VER,secret_word,sizeof(secret_word),1);
        if(st.is_good())
        {
          items_t::iterator beg = items.begin();
          items_t::iterator end = items.end();
          DWORD  max_length = 0;
          DWORD  length     = 0;
          st.add_record(&max_length,sizeof(max_length));
          while(beg<end)
          {
           DWORD rec_id;
           std::wstring::const_pointer  wstr = beg->id.c_str();
           length     = wcslen(wstr);
           max_length = std::max(max_length,length);
           rec_id = st.add_record((void*)wstr,length*sizeof(*wstr));
                    wstr = beg->access_path.c_str();
                    length     = wcslen(wstr);
                    max_length = std::max(max_length,length);
                    st.add_chunk (rec_id,(void*)wstr,length*sizeof(*wstr));
                    st.add_chunk (rec_id,&beg->group_param.group_data,sizeof(beg->group_param.group_data));

                    st.add_chunk (rec_id,&beg->rc_object,sizeof(beg->rc_object));

                    std::string::const_pointer   astr;

                    //Запись скрипта ТУ
                    astr = beg->rc_script.c_str();
                    length     = strlen(astr);
                    st.add_chunk (rec_id,(void*)astr,length);
                    if(length&1) ++length;
                       length/=sizeof(wchar_t);
                    max_length = std::max(max_length,length );

                    //Запись скрипта вычисления значения
                    astr = beg->calc_script.c_str();
                    length     = strlen(astr);
                    st.add_chunk (rec_id,(void*)astr,length);
                    if(length&1) ++length;
                       length/=sizeof(wchar_t);
                    max_length = std::max(max_length,length );

                    //Запись имени тэга
                    wstr = beg->rc_tag.c_str();
                    length     = wcslen(wstr);
                    max_length = std::max(max_length,length);
                    st.add_chunk (rec_id,(void*)wstr,length*sizeof(*wstr));


           ++beg;
          }
          st.modify_record(0,&max_length,sizeof(max_length));
          st.flush();
          return st.get_record_count();
        }
     }

     template <typename items_t,typename ioclass>
     DWORD __fastcall read(items_t & items,ioclass * io)
     {
          KeRTL::TStorage st(io);
          DWORD ver = 0;
          items.clear();
          items_t::value_type val;
          if(st.open(secret_word,sizeof(secret_word),&ver) )
          {
             DWORD rec_count = st.get_record_count();
            if(rec_count>1)
            {
             DWORD max_len = 512;
             st.get_record(0,&max_len,sizeof(max_len));
             wchar_t  *text = new wchar_t[max_len+1];

             max_len*=sizeof(wchar_t);

             for(DWORD i = 1;i<rec_count;i++)
             {

               text[st.get_record(i,text,max_len,0)/sizeof(*text)] = 0;
               val.id = text;
               text[st.get_record(i,text,max_len,1)/sizeof(*text)] = 0;
               val.access_path = text;
               st.get_record(i,&val.group_param.group_data,sizeof(val.group_param.group_data),2);
               st.get_record(i,&val.rc_object,sizeof(val.rc_object),3);
               char *atext = (char*)text;
               atext[st.get_record(i,atext,max_len,4)] = 0;
               val.rc_script = atext;

               atext[st.get_record(i,atext,max_len,5)] = 0;
               val.calc_script = atext;

               text[st.get_record(i,text,max_len,6)/sizeof(*text)] = 0;
               val.rc_tag = text;


               items.insert(items.end(),val);

             }
             delete [] text;
            }
          }
       return items.size();
     }

   };

class net_connection2_t
{
  NETRESOURCE   nr;
  std::wstring  remote_addr;
  bool          connected;
  DWORD last_error;
  public:
  net_connection2_t():connected(false),last_error(0)
  {
   ZeroMemory(&nr,sizeof(nr));
   nr.dwDisplayType = RESOURCEDISPLAYTYPE_GENERIC;
   nr.dwScope       = RESOURCE_GLOBALNET;
   nr.dwType        = RESOURCETYPE_ANY;
   nr.dwUsage       = RESOURCEUSAGE_CONNECTABLE;
  }

  bool __fastcall connect(const wchar_t * _remote_addr,const wchar_t * user_name,const wchar_t * passw)
  {
    remote_addr = L"\\\\";
    remote_addr+=_remote_addr;
    nr.lpRemoteName = const_cast<wchar_t*>(remote_addr.c_str());
    last_error = WNetAddConnection2(&nr,passw,user_name,0);
    connected  = last_error ? false : true;
    return connected;
  }

  void __fastcall disconnect(bool force = false)
  {
    if(connected)
       {
        WNetCancelConnection2(nr.lpRemoteName,0,force);
        connected = false;
       }

  }

};





  #endif

 #endif

#endif
