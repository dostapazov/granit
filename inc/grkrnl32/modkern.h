#ifndef __MODKERN_INC
#define __MODKERN_INC

#include <GKMODULES.H>

#define MODKERN_NAME_A  "MOD_KERN_MODULE"
#define MODKERN_NAME_W L"MOD_KERN_MODULE"


typedef struct _MK_START_PARAM
{
 DWORD     dw_size;
 wchar_t * app_name;
 wchar_t * app_mutex;
 DWORD     wait_mutex_timeout;//����� ��������
 wchar_t * log_folder;
 DWORD     reg_key;
 wchar_t * reg_path;
 GKHANDLE  reporter; //���� NULL �� ��������� ������ MOD_KERN
 LPVOID                hook_data;
 pfn_entry_function    hook_entry;//������� ���������� ��� �������� ������, � ��.��.
}MK_START_PARAM,*LPMK_START_PARAM;


#endif
 