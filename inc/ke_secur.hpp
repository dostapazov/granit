/*
 Обертки для системы безопасности
*/

#ifndef _KE_SECUR_HPP
#define _KE_SECUR_HPP

#include <ke_defs.h>

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


#endif
