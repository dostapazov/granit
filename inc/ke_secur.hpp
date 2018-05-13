/*
 Обертки для системы безопасности
*/

#ifndef _KE_SECUR_HPP
#define _KE_SECUR_HPP

#include <kertl.hpp>


KERTL_CLASS TSecuritiDescrptor:public SECURITY_DESCRIPTOR
{
 public:
 TSecuritiDescrptor(DWORD revision= SECURITY_DESCRIPTOR_REVISION){InitializeSecurityDescriptor(this,revision);}
 bool __fastcall is_valid (){return IsValidSecurityDescriptor(this) ? true:false; }
 bool __fastcall set_Sacl (BOOL present = TRUE,PACL acl = NULL,BOOL def = FALSE){return SetSecurityDescriptorSacl(this,present,acl,def) ? true:false;};
 bool __fastcall set_Dacl (BOOL present = TRUE,PACL acl = NULL,BOOL def = FALSE){return SetSecurityDescriptorDacl(this,present,acl,def) ? true:false;};
 bool __fastcall set_owner(PSID owner,BOOL def = FALSE){return SetSecurityDescriptorOwner(this,owner,def) ? true:false;}
 bool __fastcall set_group(PSID group,BOOL def = FALSE){return SetSecurityDescriptorGroup(this,group,def) ? true:false;}
};

KERTL_CLASS TProcessToken:public THandleBased
{
  public:
    TProcessToken(HANDLE hProcess,DWORD dwAccess = TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY);
    HRESULT modify_privilege(const LPCTSTR szPrivilege,bool fEnable);
};

 inline TProcessToken::TProcessToken(HANDLE hProcess,DWORD dwAccess):THandleBased(NULL)
 {
     OpenProcessToken(hProcess,dwAccess,&Handle);
 }

inline  HRESULT TProcessToken::modify_privilege(const LPCTSTR szPrivilege, bool fEnable)
{
    TOKEN_PRIVILEGES new_state;
    if( LookupPrivilegeValue( NULL,szPrivilege,&new_state.Privileges[0].Luid ))
    {
        new_state.PrivilegeCount = 1;
        new_state.Privileges[0].Attributes = (fEnable ? SE_PRIVILEGE_ENABLED : 0);
        if(AdjustTokenPrivileges(Handle,FALSE,&new_state,0,NULL,NULL)) return S_OK ;

    }
    return ERROR_FUNCTION_FAILED;
}



#endif
