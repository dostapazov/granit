#ifndef __OPC_HELPER_HPP__
#define __OPC_HELPER_HPP__

#include <comdef.h>
#include <utilcls.h>
#include <oleauto.h>

#include <opcda.h>
#include <opcenum.h>

//#ifdef __OPC_INC_IC__
//
//    #if !defined (__OPCDA_I_C__)
//        #define   __OPCDA_I_C__
//        #include <opcda_i.c>
//    #endif
//
//
//    #if !defined (__OPCENUM_I_C__)
//        #define   __OPCENUM_I_C__
//        #include <opcenum_i.c>
//    #endif
//
//    #if !defined (__OPCCOMN_I_C__)
//        #define   __OPCCOMN_I_C__
//        #include <opccomn_i.c>
//    #endif
//#endif


#include <string>
#include <vector>
#include <functional>
#include <algorithm>
#include <ke_defs.h>

namespace opc
{

 using namespace std;

 struct TVariant:public VARIANT
 {
   ~TVariant()  {clear();}
   TVariant()   {bzero(this,sizeof(*this)); VariantInit(this);  }
   TVariant(const TVariant & src) {bzero(this,sizeof(*this));VariantInit(this);*this = src;}
   void __fastcall clear();

   TVariant(const VARIANT & t)
   {
    bzero(this,sizeof(*this));
    VariantInit(this);
    *this = t;
   }

   template <typename T>
   TVariant(const T & t)
   {
    bzero(this,sizeof(*this));
    VariantInit(this);
    *this = t;
   }



   TVariant & operator = (const VARIANT & src)
   {
      if(vt == VT_EMPTY || vt == VT_NULL || vt == src.vt)
          VariantCopy(this,&src);
          else
          {
           DWORD _vt = vt;
           clear();
           VariantChangeType(this,&src,0,_vt);
          }
      return * this;
   }

   TVariant & operator = (const float  val)    {clear();vt = VT_R4;fltVal  = val; return *this;}
   TVariant & operator = (const double val)    {clear();vt = VT_R8;dblVal  = val; return *this;}

   TVariant & operator = (const char   val)    {clear();vt = VT_I1;cVal  = val; return *this;}
   TVariant & operator = (const short  val)    {clear();vt = VT_I2;iVal  = val; return *this;}
   TVariant & operator = (const long   val)    {clear();vt = VT_I4;lVal  = val; return *this;}
   TVariant & operator = (const long long val) {clear();vt = VT_I8;llVal = val; return *this;}
   TVariant & operator = (const int val)
   {

    #ifdef _WIN64
    return (*this) = (long long)val;
    #else
    return (*this) = (long     )val;
    #endif
   }
   TVariant & operator = (const unsigned char  val)    {clear();vt = VT_UI1;bVal   = val; return *this;}
   TVariant & operator = (const unsigned short val)    {clear();vt = VT_UI2;uiVal  = val; return *this;}
   TVariant & operator = (const unsigned long  val)    {clear();vt = VT_UI4;ulVal  = val; return *this;}
   TVariant & operator = (const unsigned long long val){clear();vt = VT_UI8;ullVal = val; return *this;}
   TVariant & operator = (const unsigned int val)
   {
    clear();
    #ifdef _WIN64
    return (*this) = (unsigned long long)val;
    #else
    return (*this) = (unsigned long     )val;
    #endif
   }

  template <typename T> operator T ()
  {
   T ret (0);
   switch(vt)
   {
     case  VT_I2       :ret = (T)iVal   ; break;
     case  VT_I4       :ret = (T)lVal   ; break;
     case  VT_R4       :ret = (T)fltVal ; break;
     case  VT_R8       :ret = (T)dblVal ; break;
     case  VT_BOOL     :ret = (T)boolVal; break;
     case  VT_I1       :ret = (T)cVal   ; break;
     case  VT_UI1      :ret = (T)bVal   ; break;
     case  VT_UI2      :ret = (T)uiVal  ; break;
     case  VT_UI4      :ret = (T)ulVal  ; break;
     case  VT_I8       :ret = (T)cVal   ; break;
     case  VT_UI8      :ret = (T)bVal   ; break;
     case  VT_INT      :ret = (T)intVal ; break;
     case  VT_UINT     :ret = (T)ulVal  ; break;
     case  VT_HRESULT  :ret = (T)ulVal  ; break;
     case  VT_VOID     :ret = (T)(LONG)byref; break;
   }
   return ret;
  }


 };

inline  void __fastcall TVariant::clear()
{
  if(vt>VT_NULL)
  {
   VariantClear(this);
  }
  bzero(this,sizeof(*this));
  vt = VT_EMPTY;
}


 enum class opc_server_type_t
 {
   opc_server_da10    = 0,
   opc_server_da20    = 1,
   opc_server_da30    = 2,
   opc_server_xmlda10 = 3,
 };


 class TOpcServer:public TComInterface<IOPCServer>
 {
   OPCSERVERSTATUS * status;
   void __fastcall   release_status();

   public:
   TOpcServer():status(NULL){}
   TOpcServer(const GUID & server_guid,wchar_t * remote_host = NULL):status(NULL)
   {
     this->create_instance(server_guid,remote_host);
   }

   TOpcServer(const wchar_t * server_name):status(NULL)
   {
     CreateInstance(const_cast<wchar_t*>(server_name));
   }

   ~TOpcServer()
   {
      release_status();
      Reset();
   }

   bool    __fastcall  update_status  (){ release_status(); return get_status();}
   bool    __fastcall  get_status     (){if(!status) (*this)->GetStatus(&status); return status ? true : false;}
   void    __fastcall  get_vendor     (std::wstring & vendor){if(get_status()) vendor = status->szVendorInfo; else vendor.clear();}
   __int64 __fastcall  get_server_time(){ return get_status() ? *((__int64*)&status->ftCurrentTime   ) : __int64(0);}
   __int64 __fastcall  get_start_time (){ return get_status() ? *((__int64*)&status->ftStartTime     ) : __int64(0);}
   __int64 __fastcall  get_update_time(){ return get_status() ? *((__int64*)&status->ftLastUpdateTime) : __int64(0);}
   OPCSERVERSTATE __fastcall get_state(){return get_status()  ?  status->dwServerState : OPC_STATUS_FAILED;}
   DWORD   __fastcall  get_group_count(){return get_status()  ?  status->dwGroupCount : 0;}
   DWORD   __fastcall  get_band_width (){return get_status()  ?  status->dwBandWidth  : 0;}
   HRESULT __fastcall  create_instance(const GUID & guid,wchar_t * host);
 };


inline HRESULT __fastcall  TOpcServer::create_instance(const GUID & guid,wchar_t * host)
{
     HRESULT ret = -1;
     if(host && host[0])
        {
         //ret = CoInitializeSecurity(NULL,-1,NULL,NULL,RPC_C_AUTHN_LEVEL_DEFAULT, RPC_C_IMP_LEVEL_IMPERSONATE,NULL,EOAC_NONE,NULL);
         ret = RemoteInstance(host,guid);
        }
        else
        ret = CreateInstance(guid);
   return ret;
}



 class TOpcBrowseServerAddrSpace
 {
   OPCNAMESPACETYPE model;
   TComInterface<IOPCBrowseServerAddressSpace> browser;
   TComInterface<IEnumString>                  enum_group;
   TComInterface<IEnumString>                  enum_items;

   TOpcBrowseServerAddrSpace(){}
   public :
   TOpcBrowseServerAddrSpace(TOpcServer & src);
   OPCNAMESPACETYPE __fastcall get_model() const {return model;}
   template <typename StringType>
   HRESULT          __fastcall get_groups (std::vector<StringType> & grp_list);
   HRESULT          __fastcall group_up   (const wchar_t * grp);
   HRESULT          __fastcall group_down (const wchar_t * grp);
   HRESULT          __fastcall groups_enum(const wchar_t * grp);
   template <typename StringType>
   HRESULT          __fastcall group_next (StringType & grp_name);

   HRESULT          __fastcall items_enum (OPCBROWSETYPE browse_type = OPC_BRANCH,const wchar_t * szFilter = NULL,WORD type = VT_EMPTY,DWORD access_right = 0 );
   template <typename StringType>
   HRESULT          __fastcall item_next (StringType & item_name);
   template <typename StringType>
   HRESULT          __fastcall item_id   (wchar_t * item_data, StringType & item_id);
   HRESULT          __fastcall browse_path(wchar_t * path);
   bool             __fastcall is_tree_model(){return (model == OPC_NS_HIERARCHIAL) ? true : false;}
   bool             __fastcall is_flat_model(){return (model == OPC_NS_FLAT       ) ? true : false;}


 };

 struct  opc_server_entry
 {
   opc_server_entry()     {}
   opc_server_entry(const opc_server_type_t _type,const GUID & _guid, const std::wstring & _prog_id)
   :type   (_type)
   ,guid   (_guid)
   ,prog_id(_prog_id)
   {}

   opc_server_entry(const opc_server_entry & src)
   {
     *this =src;
   }

   opc_server_entry& operator = (const opc_server_entry & src)
   {
     type    = src.type;
     guid    = src.guid;
     prog_id = src.prog_id;
     return *this;
   }

   opc_server_type_t  type;
   GUID               guid;
   std::wstring       prog_id;
 };



template <typename ListClass, typename EnumClass>
class TOpcServerListBase:public TComInterface<ListClass>
{
  public:
  TOpcServerListBase()
  {}

  HRESULT __fastcall create_instance(wchar_t * host = NULL)
  {
   GUID guid = CLSID_OpcServerList;
   if(host && host[0])
     {
        HRESULT res;
//       res = CoInitializeSecurity(NULL,-1,NULL,NULL,RPC_C_AUTHN_LEVEL_DEFAULT, RPC_C_IMP_LEVEL_IMPERSONATE,NULL,EOAC_APPID,NULL);
//      if(res == S_OK)
         res = RemoteInstance(host,guid);

      return res;
     }
      else
      return CreateInstance(guid);
  }

  typedef std::vector<opc_server_entry> server_entryes_t;
  HRESULT   __fastcall enum_servers(const opc_server_type_t type,server_entryes_t & entryes, int * count = NULL);

};

 typedef TOpcServerListBase<IOPCServerList,IEnumGUID>     TOpcServerList;
 typedef TOpcServerListBase<IOPCServerList2,IOPCEnumGUID> TOpcServerList2;

 struct TOpcItemResult:public OPCITEMRESULT
 {
    TOpcItemResult() {ZeroMemory(this,sizeof(*this));}
    TOpcItemResult(const OPCITEMRESULT & res) {*this = res;}
    TOpcItemResult & operator = (const OPCITEMRESULT & res)
    {
     this->hServer             = res.hServer;
     this->vtCanonicalDataType = res.vtCanonicalDataType;
     this->wReserved           = res.wReserved;
     this->dwAccessRights      = res.dwAccessRights;
     this->dwBlobSize          = (res.pBlob) ? res.dwBlobSize : 0;
     this->pBlob               = NULL;
     if(dwBlobSize){pBlob = (LPBYTE)CoTaskMemAlloc(dwBlobSize); if(pBlob) memcpy(pBlob,res.pBlob,dwBlobSize);}
     return * this;
    }
   ~TOpcItemResult()
   {
    if(this->pBlob) CoTaskMemFree(pBlob);
   }
   operator OPCHANDLE () {return hServer;}
 };

 struct TOpcItemState:public OPCITEMSTATE
 {
    TOpcItemState () {bzero(this,sizeof(*this));VariantInit(&vDataValue);}
    TOpcItemState (const OPCITEMSTATE & src){*this = src;}
    TOpcItemState & operator = (const OPCITEMSTATE & src)
    {
      hClient  = src.hClient ;ftTimeStamp = src.ftTimeStamp;
      wQuality = src.wQuality;wReserved   = src.wReserved;
      VariantClear(&vDataValue);
      VariantCopy (&vDataValue,&src.vDataValue);
      return *this;
    }
   ~TOpcItemState()  {VariantClear(&vDataValue);}
   template <typename T> operator T ();
   bool __fastcall is_value_integer(LPDWORD sz = NULL,bool * is_signed = NULL );
   bool __fastcall is_value_float  (LPDWORD sz = NULL);
   bool __fastcall is_value_bool   () {return (is_value_integer() && vDataValue.vt == VT_BOOL) ? true : false;};


 };

  template <typename T> TOpcItemState::operator T ()
  {
   T ret (0);
   switch(vDataValue.vt)
   {
     case  VT_I2       :ret = (T)vDataValue.iVal   ; break;
     case  VT_I4       :ret = (T)vDataValue.lVal   ; break;
     case  VT_R4       :ret = (T)vDataValue.fltVal ; break;
     case  VT_R8       :ret = (T)vDataValue.dblVal ; break;
     case  VT_BOOL     :ret = (T)vDataValue.boolVal; break;
     case  VT_I1       :ret = (T)vDataValue.cVal   ; break;
     case  VT_UI1      :ret = (T)vDataValue.bVal   ; break;
     case  VT_UI2      :ret = (T)vDataValue.uiVal  ; break;
     case  VT_UI4      :ret = (T)vDataValue.ulVal  ; break;
     case  VT_I8       :ret = (T)vDataValue.cVal   ; break;
     case  VT_UI8      :ret = (T)vDataValue.bVal   ; break;
     case  VT_INT      :ret = (T)vDataValue.intVal ; break;
     case  VT_UINT     :ret = (T)vDataValue.ulVal  ; break;
     case  VT_HRESULT  :ret = (T)vDataValue.ulVal  ; break;
     case  VT_VOID     :ret = (T)(LONG)vDataValue.byref; break;
   }
   return ret;
  }

   inline bool __fastcall TOpcItemState::is_value_integer(LPDWORD sz,bool * is_signed)
   {
       bool  ret (false);
       DWORD _sz = 0;
       bool _is_signed = false;

       switch(vDataValue.vt)
       {
         case  VT_I2       :ret = true ;_sz = sizeof(vDataValue.iVal   );_is_signed = true; break;
         case  VT_I4       :ret = true ;_sz = sizeof(vDataValue.lVal   );_is_signed = true; break;
         case  VT_BOOL     :ret = true ;_sz = sizeof(vDataValue.boolVal); break;
         case  VT_I1       :ret = true ;_sz = sizeof(vDataValue.cVal   );_is_signed = true; break;
         case  VT_UI1      :ret = true ;_sz = sizeof(vDataValue.bVal   ); break;
         case  VT_UI2      :ret = true ;_sz = sizeof(vDataValue.uiVal  ); break;
         case  VT_UI4      :ret = true ;_sz = sizeof(vDataValue.ulVal  ); break;
         case  VT_I8       :ret = true ;_sz = sizeof(vDataValue.cVal   );_is_signed = true; break;
         case  VT_UI8      :ret = true ;_sz = sizeof(vDataValue.bVal   ); break;
         case  VT_INT      :ret = true ;_sz = sizeof(vDataValue.intVal );_is_signed = true; break;
         case  VT_UINT     :ret = true ;_sz = sizeof(vDataValue.ulVal  ); break;
         case  VT_HRESULT  :ret = true ;_sz = sizeof(vDataValue.lVal   );_is_signed = true; break;
         case  VT_VOID     :ret = true ;_sz = sizeof((LONG)vDataValue.byref); break;
       }
       if(sz) * sz = _sz;
       if(is_signed) *is_signed = _is_signed;
       return ret;
   }

   inline bool __fastcall TOpcItemState::is_value_float  (LPDWORD sz)
   {
       bool  ret (false);
       DWORD _sz = 0;
       switch(vDataValue.vt)
       {
         case  VT_R4       :ret = true ;_sz = sizeof(vDataValue.fltVal ); break;
         case  VT_R8       :ret = true ;_sz = sizeof(vDataValue.dblVal ); break;
       }
       if(sz) * sz = _sz;
       return ret;
   }



 class TOpcGroup
 {
   public:

   typedef TComInterface<IOPCItemMgt>       item_mgt_t ;
   typedef TComInterface<IOPCSyncIO>        sync_io_t  ;
   typedef TComInterface<IOPCAsyncIO>       async_io_t ;
   typedef TComInterface<IOPCAsyncIO2>      async_io2_t;
   typedef TComInterface<IOPCGroupStateMgt> group_mgt_t;
   typedef std::vector<OPCHANDLE>      opchandles_t;

   protected:
   item_mgt_t                  item_mgt;
   sync_io_t                   sync_io;
   async_io_t                  async_io;
   async_io2_t                 async_io2;
   group_mgt_t                 group_mgt;
   LONG                        transaction_id;

   TOpcServer               &  server;
   TComInterface<IUnknown>     group;
   int                         active;
   DWORD                       rate;
   LONG                        time_bias;
   float                       dead_band;
   DWORD                       server_handle;
   DWORD                       client_handle;
   DWORD                       lcid;
   wstring                     group_name;
   std::vector<TOpcItemResult> item_results;
   HRESULT __fastcall remove_items  (std::vector<TOpcItemResult>::iterator beg,std::vector<TOpcItemResult>::iterator end);

   public:
   TOpcGroup(TOpcServer & _server):server(_server) , transaction_id(0)
   {}

   ~TOpcGroup()
   {
    remove_group   ();
    sync_io.Reset  ();
    async_io.Reset ();
    async_io2.Reset();
    item_mgt.Reset ();
    group_mgt.Reset();
   }

   HRESULT __fastcall create_group    (const wchar_t * group_name,DWORD _rate,float _dead_band,DWORD lcid = 0);
   HRESULT __fastcall group_get_status();
   HRESULT __fastcall group_set_status();
   bool    __fastcall is_active       (){return active ? true : false;}
   HRESULT __fastcall set_active      (bool active);
   HRESULT __fastcall add_item        (OPCHANDLE  hClient,const wchar_t * item_id,const wchar_t * access_path = NULL ,OPCITEMRESULT * res_ptr = NULL);
   HRESULT __fastcall remove_item     (DWORD item_idx);
   HRESULT __fastcall remove_group    ();
   DWORD   __fastcall get_group_count(){return item_results.size();}
   template <typename IfaceType>
   bool    __fastcall get_iface(TComInterface<IfaceType> & _iface);
   HRESULT __fastcall read(bool from_device,DWORD count,OPCHANDLE * handles,OPCITEMSTATE ** states,LONG ** errors);
   HRESULT __fastcall read(bool from_device,DWORD * count,OPCITEMSTATE ** states,LONG ** errors);
   HRESULT __fastcall write(DWORD count,OPCHANDLE * handles,VARIANT * values,LONG ** errors);
   HRESULT __fastcall async2_refresh(OPCDATASOURCE data_source,DWORD & cancel_id);
   bool    __fastcall async2_callback_enable(bool enable);
   bool    __fastcall async2_callback_is_enabled();
   HRESULT __fastcall set_items_active_state(DWORD dw_count,OPCHANDLE * hServer,bool active_state);
   TOpcServer &  __fastcall get_server(){return server;}
 };

inline    HRESULT __fastcall TOpcGroup::async2_refresh(OPCDATASOURCE data_source,DWORD & cancel_id)
{
  HRESULT ret = -1;
  DWORD trans_id = InterlockedIncrement(&transaction_id);
  if(get_iface(async_io2))
  {
   ret = async_io2->Refresh2(data_source,trans_id,&cancel_id);
  }
  return ret;
}

inline HRESULT __fastcall TOpcGroup::set_items_active_state(DWORD dw_count,OPCHANDLE * hServer,bool active_state)
{
  HRESULT ret = -1;
  if(get_iface(item_mgt))
  {
    LONG * errors = NULL;
    ret = item_mgt->SetActiveState(dw_count,hServer,int(active_state),&errors);
    if(errors)
       CoTaskMemFree(errors);
  }
  return ret;
}


inline bool    __fastcall TOpcGroup::async2_callback_enable(bool enable)
{

  if(get_iface(async_io2))
  {
    if(SUCCEEDED(async_io2->SetEnable(enable)))
       return true;
  }

  return false;
}

inline bool    __fastcall TOpcGroup::async2_callback_is_enabled()
{
  int ret = 0;
  if(get_iface(async_io2))
  {
    async_io2->GetEnable(&ret);
  }
  return ret ? true:false;
}


template <typename NOTIFY_RECEIVER>
class  TOpcDataCallback:public IOPCDataCallback
{
  ULONG       ref_count;
  DWORD       conn_id;
  TOpcGroup        * group;
  IConnectionPoint * connection_point;
  NOTIFY_RECEIVER  * nrecv;

  public:
  TOpcDataCallback():ref_count(0),conn_id(0),group(NULL),connection_point(NULL),nrecv(NULL)
  {}

   // AddRef
   STDMETHODIMP_(ULONG) AddRef()
   {
        return InterlockedIncrement((LONG*)&ref_count);
   }

   // Release
   STDMETHODIMP_(ULONG) Release()
   {
        ULONG ulRefs = InterlockedDecrement((LONG*)&ref_count);
        if (ulRefs == 0)
        {
            delete this;
            return 0;
        }
       return ulRefs;
   }

  // QueryInterface
   STDMETHODIMP QueryInterface(REFIID iid, LPVOID* ppInterface)
   {
      if (ppInterface == NULL)
      {
         return E_INVALIDARG;
      }

      if (iid == IID_IUnknown)
      {
         *ppInterface = dynamic_cast<IUnknown*>(this);
         AddRef();
         return S_OK;
      }

      if (iid == IID_IOPCDataCallback)
      {
         *ppInterface = dynamic_cast<IOPCDataCallback*>(this);
         AddRef();
         return S_OK;
      }
      return E_NOINTERFACE;
   }

STDMETHODIMP OnDataChange(
        DWORD       dwTransid,
        OPCHANDLE   hGroup,
        HRESULT     hrMasterquality,
        HRESULT     hrMastererror,
        DWORD       dwCount,
        OPCHANDLE * phClientItems,
        VARIANT   * pvValues,
        WORD      * pwQualities,
        FILETIME  * pftTimeStamps,
        HRESULT   * pErrors)
   {
      if(nrecv) nrecv->on_data_change(conn_id,group,dwTransid,hGroup,hrMasterquality,hrMastererror,dwCount,phClientItems,pvValues,pwQualities,pftTimeStamps,pErrors);
      return S_OK;
   }

    // OnReadComplete
    STDMETHODIMP OnReadComplete(
        DWORD       dwTransid,
        OPCHANDLE   hGroup,
        HRESULT     hrMasterquality,
        HRESULT     hrMastererror,
        DWORD       dwCount,
        OPCHANDLE * phClientItems,
        VARIANT   * pvValues,
        WORD      * pwQualities,
        FILETIME  * pftTimeStamps,
        HRESULT   * pErrors)
   {
      if(nrecv) nrecv->on_read_complete(conn_id,group,dwTransid,hGroup,hrMasterquality,hrMastererror,dwCount,phClientItems,pvValues,pwQualities,pftTimeStamps,pErrors);
      return S_OK;
   }

    // OnWriteComplete
    STDMETHODIMP OnWriteComplete(
        DWORD       dwTransid,
        OPCHANDLE   hGroup,
        HRESULT     hrMastererr,
        DWORD       dwCount,
        OPCHANDLE * pClienthandles,
        HRESULT   * pErrors)
   {
      if(nrecv) nrecv->on_write_complete(conn_id,group,dwTransid,hGroup,hrMastererr,dwCount,pClienthandles,pErrors);
      return S_OK;
   }


    // OnCancelComplete
    STDMETHODIMP OnCancelComplete(
        DWORD       dwTransid,
        OPCHANDLE   hGroup)
   {
      if(nrecv) nrecv->on_cancel_complete(conn_id,group,dwTransid,hGroup);
      return S_OK;
   }

   HRESULT __stdcall start(TOpcGroup * grp,NOTIFY_RECEIVER * _nrecv);
   HRESULT __stdcall stop ();
};

   template <typename NOTIFY_RECEIVER>
   HRESULT __stdcall TOpcDataCallback<NOTIFY_RECEIVER>::start(TOpcGroup * grp,NOTIFY_RECEIVER * _nrecv)
   {
    HRESULT  ret = S_OK;
    if(group )
      ret = stop();
    if(S_OK == ret)
    {
      ret = -1;
      TOpcGroup::item_mgt_t mgt;
      if(grp->get_iface(mgt))
      {
        TComInterface<IConnectionPointContainer> cpc(mgt);
        if(cpc.IsBound())
        {
           ret = cpc->FindConnectionPoint(IID_IOPCDataCallback,&connection_point);
           if(S_OK == ret)
           {
               ret = connection_point->Advise(this,&conn_id);
               if(S_OK == ret)
                   {
                    group = grp;
                    nrecv = _nrecv;
                    AddRef();
                   }
                   else
                   connection_point->Release();
           }
         cpc.Reset();
        }
        mgt.Reset();
      }
    }
    return ret;
   }

   template <typename NOTIFY_RECEIVER>
   HRESULT __stdcall TOpcDataCallback<NOTIFY_RECEIVER>::stop ()
   {
     if(group )
     {
        group   = NULL;
        if(connection_point)
        {
         HRESULT ret = connection_point->Unadvise(conn_id);
         if(S_OK == ret)
           {
              conn_id = 0;
              nrecv   = NULL;

             __try{
                   connection_point->Release();
                  }
             __finally{
                 connection_point = NULL;
             }
            }
         return  ret;
        }
     }
     return -1;
   }






inline  HRESULT __fastcall TOpcGroup::read(bool from_device,DWORD count,OPCHANDLE * handles,OPCITEMSTATE ** states,LONG ** errors)
{
    HRESULT res = -1;
    if(get_iface(sync_io))
    {
       res = sync_io->Read(from_device ? OPC_DS_DEVICE : OPC_DS_CACHE,count,handles,states,errors);
    }
   return res;
}

inline  HRESULT __fastcall TOpcGroup::write(DWORD count,OPCHANDLE * handles,VARIANT * values,LONG ** errors)
{
    HRESULT res = -1;
    if(get_iface(sync_io))
        res = sync_io->Write(count,handles,values,errors);
    return res;
}


inline  HRESULT __fastcall TOpcGroup::read(bool from_device,DWORD * count,OPCITEMSTATE ** states,LONG ** errors)
{
   TOpcGroup::opchandles_t handles;
   handles.resize(this->item_results.size());
   TOpcGroup::opchandles_t::iterator   hptr = handles.begin();
   std::vector<TOpcItemResult>::iterator beg = item_results.begin();
   std::vector<TOpcItemResult>::iterator end = item_results.end  ();
   while(beg<end)
        {
         *hptr = *beg;
         ++hptr;
         ++beg;
        }
   if(count) *count = handles.size();
   hptr = handles.begin();
   return read(from_device,handles.size(),&*hptr,states,errors);
}

 inline HRESULT __fastcall TOpcGroup::create_group(const wchar_t * _group_name,DWORD _rate,float _dead_band,DWORD _lcid)
 {
   HRESULT ret = -1;
   rate = _rate; dead_band = _dead_band;
   if(server.IsBound())
    {
     IUnknown * punk;
     lcid = _lcid;
     client_handle = 0;
     server_handle = 0;
     time_bias     = 0;
     group_name    = _group_name;
     active        = TRUE;
     ret = server->AddGroup(_group_name,active,_rate,client_handle,&time_bias,&dead_band,_lcid,&server_handle,&rate,IID_IUnknown,&punk);
     if(SUCCEEDED(ret))
         {
          group.Bind(punk,true);
          punk->Release     ();
          item_results.clear();
         }
    }
   return ret;
 }

   HRESULT __fastcall TOpcGroup::group_get_status()
   {
     HRESULT res = -1;
     if(get_iface(this->group_mgt))
     {
        wchar_t * name = NULL;
        res = group_mgt->GetState(&rate,&active,&name,&time_bias,&dead_band,&lcid,&client_handle,&server_handle);
        if(FAILED(res))
           active = 0;
        if(name)
           {
            group_name = name;
            CoTaskMemFree(name);
           }
     }
     return res;
   }

   #pragma warn -8004
   HRESULT  __fastcall TOpcGroup::set_active     (bool active)
   {
     HRESULT res = -1;
     if(get_iface(group_mgt))
     {
       int new_active = 0;
       res = group_mgt->SetState(&rate,&rate,&new_active,&time_bias,&dead_band,&lcid,&client_handle);
       if(SUCCEEDED(res))
          active = new_active;

     }
    return res;
   }
   #pragma warn .8004

   HRESULT  __fastcall TOpcGroup::group_set_status ()
   {
     HRESULT res = -1;
     if(get_iface(group_mgt))
     {
       res = group_mgt->SetState(&rate,&rate,&active,&time_bias,&dead_band,&lcid,&client_handle);
     }
    return res;
   }

inline HRESULT __fastcall TOpcGroup::remove_items  (std::vector<TOpcItemResult>::iterator beg,std::vector<TOpcItemResult>::iterator end)
 {
   DWORD count = std::distance(beg,end);
   if( count && get_iface(item_mgt))
   {
    std::vector<DWORD> shandles;
    shandles.resize(count);
    std::vector<DWORD>::iterator h_ptr = shandles.begin();
    std::vector<DWORD>::iterator h_end = shandles.end();
    std::vector<TOpcItemResult>::iterator ptr = beg;
    while(h_ptr<h_end) {*h_ptr = ptr->hServer; ++ptr,++h_ptr;}
    LPLONG errors = 0;
    HRESULT ret = item_mgt->RemoveItems(count,&*shandles.begin(),&errors);
    if(errors) CoTaskMemFree(errors);
    if(SUCCEEDED(ret))
       item_results.erase(beg,end);
       return ret;
   }
   return -1;
 }

 HRESULT __fastcall TOpcGroup::remove_item  (DWORD item_idx)
 {
   if(item_idx<item_results.size())
      {
        std::vector<TOpcItemResult>::iterator ptr = item_results.begin();
        std::vector<TOpcItemResult>::iterator end = ptr;
        ++end;
        remove_items(ptr,end);
      }
   return -1;
 }

 inline HRESULT __fastcall TOpcGroup::remove_group ()
{
   if(item_mgt)
   {
     HRESULT ret = remove_items(item_results.begin(),item_results.end());
     if(SUCCEEDED(ret))
        ret = server->RemoveGroup(server_handle,FALSE);
     if(SUCCEEDED(ret))
        client_handle = server_handle = 0;
     return ret;
   }
   return -1;
}

template <typename IfaceType>
bool    __fastcall TOpcGroup::get_iface(TComInterface<IfaceType> & _iface)
{
 if(!_iface.IsBound())
      _iface = group;
   return _iface.IsBound();
}

inline  HRESULT __fastcall TOpcGroup::add_item    (OPCHANDLE  hClient,const wchar_t * item_id,const wchar_t * access_path,OPCITEMRESULT * res_ptr)
{

  HRESULT ret = 0;
  if(res_ptr) bzero(res_ptr,sizeof(*res_ptr));
  if(get_iface(item_mgt))
  {
    OPCITEMDEF item_def; ZeroMemory(&item_def,sizeof(item_def));
    item_def.szAccessPath = const_cast<wchar_t*>(access_path);
    item_def.szItemID     = const_cast<wchar_t*>(item_id);
    item_def.bActive      = TRUE;
    item_def.hClient      = hClient;
    OPCITEMRESULT    *res = NULL;
    LPLONG         errors = NULL;
    ret = item_mgt->AddItems(1,&item_def,&res,&errors);
    if(SUCCEEDED(ret))
      {
        if(SUCCEEDED(*errors))
           {
            if(res_ptr) memcpy(res_ptr,res,sizeof(*res_ptr));
            item_results.push_back(*res);
            if(res->pBlob) CoTaskMemFree(res->pBlob);
           }

            CoTaskMemFree(res);
            CoTaskMemFree(errors);
      }
  }
  return ret;
}


/***                                   Реализация                           **/

  inline void __fastcall   TOpcServer::release_status()
   {
     if(status)
       {
         if(status->szVendorInfo)
            ::CoTaskMemFree(status->szVendorInfo);
         CoTaskMemFree(status);
         status = NULL;
       }
   }

 inline
  TOpcBrowseServerAddrSpace::TOpcBrowseServerAddrSpace(TOpcServer & src)
   : browser(src)
   {
     if(browser) browser->QueryOrganization(&model);
   }


  inline
  HRESULT          __fastcall TOpcBrowseServerAddrSpace::browse_path(wchar_t * path)
  {
      HRESULT ret = -1;
      if(browser)
      {
       IEnumString * es = NULL;
       ret = browser->BrowseAccessPaths(path,&es);
       if(SUCCEEDED(ret))
        {
          enum_items.Reset(es);
          enum_items->Reset();
        }
      }
      return ret;
  }

 inline
   HRESULT             __fastcall TOpcBrowseServerAddrSpace::group_up  (const wchar_t * grp)
   {
     HRESULT     ret = -1;
     if(browser) ret = browser->ChangeBrowsePosition(OPC_BROWSE_UP,grp);
       return    ret;
   }

 inline
   HRESULT             __fastcall TOpcBrowseServerAddrSpace::group_down(const wchar_t * grp)
   {

     HRESULT     ret = -1;
     if(browser) ret = browser->ChangeBrowsePosition(OPC_BROWSE_DOWN,grp);
       return    ret;
   }

 inline
   HRESULT             __fastcall TOpcBrowseServerAddrSpace::groups_enum(const wchar_t * grp)
   {
     HRESULT ret = -1;
     if(browser)
     {
      IEnumString * es = NULL;
      ret = browser->BrowseAccessPaths(grp,&es);
      if(SUCCEEDED(ret))
         {
          enum_group.Reset(es);
          ret = enum_group->Reset();
         }
     }
     return ret;
   }

   template <typename StringType>
   HRESULT             __fastcall TOpcBrowseServerAddrSpace::group_next (StringType & grp_name)
   {

    HRESULT   ret = -1;
    if(enum_group)
    {
      wchar_t * text     = NULL;
      DWORD     fetched  = 0;
      ret =  enum_group->Next(1,&text,&fetched);
      grp_name = SUCCEEDED(ret) ? text : L"";
      if(text) CoTaskMemFree(text);
    }
    return ret;
   }

 inline
   HRESULT          __fastcall TOpcBrowseServerAddrSpace::items_enum (OPCBROWSETYPE browse_type,const wchar_t * szFilter ,WORD var_type ,DWORD access_right  )
   {
     HRESULT ret = -1;
     if(browser)
      {
       IEnumString * es = NULL;
       ret = browser->BrowseOPCItemIDs(browse_type,szFilter ? szFilter :L"",var_type,access_right,&es);
       if(SUCCEEDED(ret))
           {
			enum_items.Reset(es);
            enum_items->Reset();
           }
      }

     return ret;
   }

   template <typename StringType>
   HRESULT          __fastcall TOpcBrowseServerAddrSpace::item_next (StringType & item_name)
   {

      HRESULT ret = -1;
      if(enum_items)
      {
		DWORD     fetched = 0;
		wchar_t * text = NULL;
		ret = enum_items->Next(1,&text,&fetched);
		item_name = SUCCEEDED(ret) ? text : L"";
		if(text) ::CoTaskMemFree(text);
        return fetched ? S_OK : S_FALSE;
	  }
	  return ret;
   }

   template <typename StringType>
   HRESULT          __fastcall TOpcBrowseServerAddrSpace::item_id   (wchar_t * item_data, StringType & item_id)
   {
     HRESULT ret = -1;
     if(browser)
     {
       wchar_t * id = NULL;
       ret = browser->GetItemID(item_data,&id);
       item_id = (S_OK == ret) ? id : L"";
       if(id) ::CoTaskMemFree(id);

     }
     return ret;
   }


 template <typename ListClass, typename EnumClass>
 HRESULT   __fastcall TOpcServerListBase<ListClass,EnumClass>::enum_servers(const opc_server_type_t type,server_entryes_t & entryes,int * count)
 {
  HRESULT res = -1;
  if((*this))
  {
   EnumClass * enumerator;
   GUID   server_type_guid;
   switch(type)
   {
       case opc_server_type_t::opc_server_da10    : server_type_guid = CATID_OPCDAServer10;break;
       case opc_server_type_t::opc_server_da20    : server_type_guid = CATID_OPCDAServer20;break;
       case opc_server_type_t::opc_server_da30    : server_type_guid = CATID_OPCDAServer30;break;
       case opc_server_type_t::opc_server_xmlda10 : server_type_guid = CATID_XMLDAServer10;break;
       default : return 0;
   }

   res  =  (*this)->EnumClassesOfCategories(1,&server_type_guid,0,NULL,&enumerator);
   if(SUCCEEDED( res ))
    {
         enumerator->Reset();
         GUID      server_guid;
         wchar_t * prog_id = NULL;
         wchar_t * user_type = NULL;
         DWORD     fetched = 0;
         enumerator->Reset();

         do{
           res = enumerator->Next(1,&server_guid,&fetched);
           if(res == S_OK )
             {

               res = (*this)->GetClassDetails(server_guid,&prog_id,&user_type);
               if(FAILED(res))
                  res = ProgIDFromCLSID(server_guid,&prog_id);
               if(FAILED(res))
                  res = StringFromIID(server_guid,&prog_id);
              if(prog_id)
              {
               opc_server_entry entry(type,server_guid,std::wstring(prog_id));
               entryes.push_back  (entry);
               ::CoTaskMemFree (prog_id);
               prog_id = NULL;
              }
              if(user_type){CoTaskMemFree(user_type); user_type = NULL;}
            }
         }while(S_OK == res);
         res = 0;
         enumerator->Release();
    }
  }
  if(count)  * count = (int)entryes.size();
  return res;
 }

}

#endif
