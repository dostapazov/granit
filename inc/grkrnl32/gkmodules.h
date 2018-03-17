/*******************************************************************************
 *
 * ������    : ���������� GKLIB
 * ��������  :
 * �����     : ��������� �.�.
 * ������    : 1
 * ������    : 27.06.2002
 *
 ******************************************************************************/


#ifdef RC_INVOKED
 #include "windows.h"
#endif

#include  "build_rules.inc"

#ifndef GKMODULES_INC_H
#define GKMODULES_INC_H


#define IDI_MODULE_ICON     1    //������������� ������� ������
#define IDI_MODULE_ICONSM   2    //������������� ��������� ������

#define IDS_MODULE_PROVIDER 100  //������������� ������ ����������
#define IDS_MODULE_DESCRIPT 101  //������������� ������ �������� ��������
#define IDS_MODULE_NAME     102  //������������� ������ ����� ������


#define IDS_COMMON_LOAD             10024
#define IDS_COMMON_UNLOAD           10025
#define IDS_COMMON_LOADFAIL         10026
#define IDS_COMMON_STARTSUCCESS     10027
#define IDS_COMMON_STARTFAIL        10028
#define IDS_COMMON_STOPSUCCESS      10029
#define IDS_COMMON_STOPFAIL         10030
#define IDS_COMMON_PLUG_MODULE      10031
#define IDS_COMMON_UNPLUG_MODULE    10032
#define IDS_COMMON_INVALID_GKHANDLE 10033
#define IDS_COMMON_SUCCESS          10034
#define IDS_COMMON_ERROR            10035
#define IDS_COMMON_EXCEPTION        10036

#define IDS_COMMON_MOD_REGISTERED   10050
#define IDS_COMMON_MOD_UNREGISTERED 10051
#define IDS_COMMON_MOD_TRIAL        10052


#define IDS_COMMON_NOTIFY_TEMPL     10100

#define IDS_USER_STR                20000

#include "gkmodules.h"

#ifndef RC_INVOKED

#include <ke_defs.h>
#include <stdarg.h>
#include <gkhandle.h>
#include <gkcomerr.h> //���� ������
#include <gkmod_types.h>




//���������� ������� ������
#ifdef __cplusplus
 extern "C" {
#endif

#ifdef _Windows
extern  HINSTANCE ModuleInstance;
__declspec(dllexport) LRESULT WINAPI gkmodule_entry(DWORD cmd,LPARAM p1, LPARAM p2);

#endif

typedef  LRESULT WINAPI  (  * pfn_gkmodule_entry)(DWORD cmd,LPARAM p1, LPARAM p2);

#define GKMODULE_ENTRY_NAME    "_gkmodule_entry"
#define GKMODULE_ENTRY_NAME_MS "_gkmodule_entry@12"


#define GKME_CMD_CREATE_HANDLE 0 /*������� �������� ������ ������*/
                                 // p1 = wchar_t * name,p2 ��������������� ��������
#define GKME_CMD_GET_MODULETYPE 1 /*������� ��������� ���� ������*/

#define GKME_CMD_UNLOAD_MODULE  ((DWORD)-1)



 extern   LRESULT WINAPI module_main(DWORD cmd,LPARAM p1, LPARAM p2);
 extern   LRESULT WINAPI get_module_type();
     
 
#ifdef __cplusplus
}// end of extern "C"
#endif


    



/*������������ ����� ������ � �������� � ������� */

#define MREG_KEY_SETTINGS        L"Settings"

#define MREG_VALUE_IMAGEPATH     L"ImagePath"
#define MREG_VALUE_REPLACEFROM   L"ReplaceFrom"

#define MREG_VALUE_RUNNING       L"Running"
#define MREG_VALUE_DELETEMODULE  L"DeleteModule"
#define MREG_VALUE_CONFIGFOLDER  L"ConfigFolder"
#define MREG_VALUE_WINDOW_MODULE L"WindowModule"

#define MREG_VALUE_REPORTER      L"External Reporter"

#define GRKERNEL_HANDLE_NAME      L"GRKERNEL32"
#define GRKERNEL_TERMINATE_EVENT  L"TerminateGrKernel32"
#define GRKERNEL_REPORTER_KEY     L"Reporter"

/*#ifdef  MAKELONG
#undef  MAKELONG
#define MAKELONG(a,b) ((DWORD) ((((DWORD)b)<<16)&0xFFFF0000 )|(((DWORD)a)&0xFFFF) )
#endif*/

//������������ ������ ��� ������� ������������� ����
#define   MCMD(type,code) MAKELONG(code,type)
#define   MCMD_TYPE(cmd)   HIWORD(cmd)
#define   MCMD_CODE(cmd)   LOWORD(cmd)

#define   init_data_sz(a,sz) ZeroMemory(a,sz);*((LPDWORD)a)=sz;
#define   init_data(a) init_data_sz(a,sizeof(*a))

#define   MODULE_STATE_STOPPED 0
#define   MODULE_STATE_STOP_PENDING 1
#define   MODULE_STATE_START_PENDING  2
#define   MODULE_STATE_RUNNING      4
#define   MODULE_STATE_PAUSED       8
#define   MODULE_STATE_RUNNING_MASK 0x0000000F


#pragma pack(push,8)

//������� ����������� ������
//p1 = LPMODULE_INIT
#define MCMD_MODULE_INIT               1L

typedef struct _MODULE_INIT
{
 DWORD     dw_size;      //������ ���������
 GKHANDLE  owner;        //��������
 HKEY      reg_key;      //���� � �������
 DWORD     reg_key_name_size;
 wchar_t * reg_key_name; //��� ����� � �������
 DWORD     config_folder_size;
 wchar_t * config_folder;//���� � �������� ������������
}MODULE_INIT,*LPMODULE_INIT,CONFIG_PARAMS,*LPCONFIG_PARAMS;


//������� ��������� �����
//p1 = LANGID
#define MCMD_SETLANGUAGE               2L


// ������� ��������� ���������� � ������
// p1 = LPMODULE_INFO
#define MCMD_GETMODULE_INFO            3L

//��������� ���������� � ������
typedef struct _MODULE_INFO
{
 DWORD      dw_size; //������ ���������
 DWORD      mi_mask; //�����  ��������� �����
 WORD       type;    //��� ������
 DWORD      version; //������
 HICON      icon;    //������ ������
#ifdef _Windows
 FILETIME   start_time;
 FILETIME   stop_time;
#else
 QWORD      start_time;
 QWORD      stop_time;
#endif
 LANGID     lang_id;       //������� ����
 LPARAM     compare_key;
 DWORD      module_state;
 DWORD      provider_cbsz;
 DWORD      description_cbsz;
 DWORD      module_name_cbsz;
 DWORD      dll_name_cbsz;
 DWORD      lang_dll_name_cbsz;
 DWORD      mem_used;
 DWORD      license_type;
 DWORD      reg_info_cbsz;
 LPWSTR     provider;          //���������� � ����������
 LPWSTR     description;       //������� ��������
 LPWSTR     module_name;       //��� ������
 LPWSTR     dll_name;          //��� DLL � ������ ��������� ������
 LPWSTR     lang_dll_name;     //��� DLL � ������ ��������� �������������� ������
 LPWSTR     reg_info;          //���������� � �����������
}MODULE_INFO,*LPMODULE_INFO;


#define MI_FL_TYPE            0x00000001
#define MI_FL_VERSION         0x00000002
#define MI_FL_ICON            0x00000004
#define MI_FL_ICONSM          0x00000008
#define MI_FL_MODULE_TIMES    0x00000010
#define MI_FL_LANGID          0x00000020
#define MI_FL_COMPARE_KEY     0x00000040
#define MI_FL_MODULE_STATE    0x00000080
#define MI_FL_PROVIDER        0x00000100
#define MI_FL_DESCRIPTION     0x00000200
#define MI_FL_MODULE_NAME     0x00000400
#define MI_FL_DLL_NAME        0x00000800
#define MI_FL_LANGDLL_NAME    0x00001000
#define MI_FL_LANGINFO        (MI_FL_LANGDLL_NAME|MI_FL_LANGID)
#define MI_FL_MEMUSED         0x00002000
#define MI_FL_LICENSE         0x00008000

#define MOD_LICENSE_TRIAL              1
#define MOD_LICENSE_REGISTERED         0
#define MOD_LICENSE_UNREGISTERED      ((DWORD)-1)


//������� ��������� ����� ��� ���������
#define MCMD_GETCOMPARE_KEY             4L

//������� ��������� ������
//p1 == key1 ,p2 = key2
//������� int  0 - ���� �����  <0 ���� one<two > 0 one>two
#define MCMD_COMPARE_KEYS               5L


//��������� ������ ���������
//�������   GKHANDLE(owner);
#define MCMD_GETOWNER_HANDLE            6L

//��������� ����� � ������� � ����� ������������
#define MCMD_GETCONFIG_PARAMS           7L

typedef struct _MODULE_CONFIG_DATA
{
 DWORD dw_sz;
 DWORD mask;
 BYTE  data[ANYSIZE_ARRAY];
}MODULE_CONFIG_DATA,*LPMODULE_CONFIG_DATA;


//��������� ��������� ������ ������������ ������
//p1 = LPMODULE_CONFIG_DATA;
//p2 = DWORD bsz - ������ �������
// ret
#define MCMD_GET_MODULECONFIG_DATA       8L
#define MCMD_SET_MODULECONFIG_DATA       9L

#define MCMD_IS_MODULE_RUNNING           10L

#define MCMD_FLUSH_CONFIG                11


//������������ ����������� ��������� ������
//p1 = LPMODULE_COMPONENT, p2 = int(index)
#define MCMD_ENUM_COMPONENTS     20L

typedef struct _MODULE_COMPONENT
{
 DWORD    dw_size;
 DWORD    component_info;
 DWORD    component_ver;
 DWORD    component_name_cbsz;
 LPWSTR   component_name[ANYSIZE_ARRAY];
}MODULE_COMPONENT,*LPMODULE_COMPONENT;


typedef struct _MODULE_CONFIG_ITEM
{
 DWORD dw_size;
 union{
       DWORD item_index;
       struct{BYTE i0,i1,i2,i3;};/*�������������� ������� ��� ������������� � ���� ������*/
      };
 WORD  item_name_sz;
 WORD  item_type;
 WORD  item_sz;
 WORD  item_comment_sz;
 WORD  buffer_size;
 BYTE  buffer[ANYSIZE_ARRAY];
}MODULE_CONFIG_ITEM,*LPMODULE_CONFIG_ITEM;

#define CCFL_MODULE_CONFIG 1

#define MCIT_BYTE          0x01
#define MCIT_WORD          0x02
#define MCIT_DWORD         0x03
#define MCIT_QWORD         0x04
#define MCIT_INTEGER       0x05
#define MCIT_FLOAT         0x06
#define MCIT_MULTIBYTE     0x07
#define MCIT_STRING        0x08
#define MCIT_WSTRING       0x09
#define MCIT_MULTI_STRING  0x0A
#define MCIT_MULTI_WSTRING 0x0B
#define MCIT_GROUP         0x0FFF // ������ ����������
#define MCIT_DYNAMIC       0x8000 // ������������
#define MCIT_READONLY      0x4000 // ������ ������

//������������� ���������
#define INIT_MCITEM(buff,sz)\
ZeroMemory(buff,sz);\
((LPMODULE_CONFIG_ITEM)buff)->dw_size = (WORD)sizeof(MODULE_CONFIG_ITEM),\
((LPMODULE_CONFIG_ITEM)buff)->buffer_size = (WORD)sz-sizeof(MODULE_CONFIG_ITEM)

#define MAKE_ITEM_INDEX(a,b,c,d) (((DWORD)a&0xFF)|(((DWORD)b&0xFF)<<8)|(((DWORD)c&0xFF)<<16)|(((DWORD)d&0xFF)<<24))

#define GET_MCITEM_NAME(mci) (wchar_t*)(((LPMODULE_CONFIG_ITEM)mci)->buffer)
#define GET_MCITEM_VALUE(mci) (LPVOID)(((LPMODULE_CONFIG_ITEM)mci)->buffer+((LPMODULE_CONFIG_ITEM)mci)->item_name_sz)
#define GET_MCITEM_COMMENT(mci) (wchar_t*)((LPBYTE)(GET_MCITEM_VALUE(mci))+ ((LPMODULE_CONFIG_ITEM)mci)->item_sz)
#define GET_MCITEM_SIZE(mci) (DWORD)(((LPMODULE_CONFIG_ITEM)mci)->dw_size+((LPMODULE_CONFIG_ITEM)mci)->item_name_sz+((LPMODULE_CONFIG_ITEM)mci)->item_sz+((LPMODULE_CONFIG_ITEM)mci)->item_comment_sz)

/*
 ��������������� ������� ��� ������ � MODULE_CONFIG_ITEM
*/

extern "C"
LPMODULE_CONFIG_ITEM WINAPI setup_module_config_item(LPVOID buffer,DWORD bsz,
                                                     BYTE i0,BYTE i1,BYTE i2,BYTE i3,wchar_t * iname,
                                                     DWORD item_type,LPVOID item,DWORD item_sz,
                                                     wchar_t * comment);

#define MCMD_ENUM_CONFIG_ITEM   21L
//��������� ������ ��������

#define MCMD_SET_CONFIG_ITEM    22L




//������� ����������� ���������
#define MCMD_NOTIFY_CODE               50L
//p1 = LPNOTIFY_CODE ,p2 = mask
typedef struct _NOTIFY_CODE
{
 DWORD         dw_size;
 GKHANDLE      from_module;
 DWORD         notify_code;
 DWORD         notify_data[ANYSIZE_ARRAY];
}NOTIFY_CODE,*LPNOTIFY_CODE;

#define MNF_COMMON   0x80000000 //����� ����������� ����� ��� ���� �������

#define MNCODE_MODULE_STATE     0  //��� ����������� �� ��������� ��������� ������
#define MNCODE_MODULE_CONFIG    1  //��� ����������� �� ��������� ��������� ������

#define NOTIFY_DATA_SIZE(nc)    (DWORD)((nc)->dw_size - sizeof(NOTIFY_CODE)+sizeof(DWORD))


//��������� ������ ��� ����������� � ����� �����������
//p1 = GKHANDLE p2 = MASK  MASK == 0 ->��������
#define MCMD_SET_NOTIFY_MASK         51L

//����� ����� �����  ����������� ������
//p1 = GKHANDLE p2 = MASK  MASK == 0 ->��������
#define MCMD_RESET_NOTIFY_MASK       52L



//������� ������� ������
#define MCMD_START                     100L

//������� �������� ������
//p1 = DWORD (reason),p2 = BOOL( TRUE - pause,FALSE continue);
#define MCMD_PAUSE                     101L

//������� �������� ������
//p1 = DWORD (reason)
#define MCMD_STOP                      102L

//���� ������ ��������/ �����
#define MODULE_START_REQUEST             1 //������  �� ���������� -> ����������� Running � ����������
#define MODULE_START_PROGRAM_LAUNCH      2 //������ ���������
#define MODULE_START_RECONFIGURE         4 //������� ������ ��� ���������� ����� ��������
#define MODULE_START_OWNER_REQUEST       8 //������ ������ ��� ������� ���������

#define MODULE_STOP_REQUEST              1 //�������/����� �� ���������� -> ����������� Running � ����������
#define MODULE_STOP_PROGRAM_SHUTDOWN     2 //������� �� ������� ���������� ���������
#define MODULE_STOP_RECONFIGURE          4//������� ������ ��� ���������� ����� ��������
#define MODULE_STOP_OWNER_REQUEST        8//������� ������ ��� ��������� ���������
#define MODULE_STOP_RELEASE              0xFFFFFFFF //������� ��� ��������


/*

  ��������� ����� ��� ������������ ��������
  p1 LPWSTR  file_name
*/

#define MCMD_LICENSE_INCTIME           109L

#define MCMD_LICENSE_GEN_REQUEST       110L
//��������� ������ �� ��������
//p1 wchar_t * prefix_name
#define MCMD_LICENSE_ACTIVATE          111L
//����������� ��������
//p1 wchar_t * actiavtion_key   p2 = DWORD length




#define GKWND_PROP_DATA_A  "GKHWND_Associated_Data"
#define GKWND_PROP_DATA_W L"GKHWND_Associated_Data"

#ifdef UNICODE
 #define GKWND_PROP_DATA GKWND_PROP_DATA_A
#else
 #define GKWND_PROP_DATA GKWND_PROP_DATA_W
#endif


//������� ����������� ����
//p1 = MODULE_WINDOW , p2 = GKHANDLE owner
//������� ���� MODULE_WINDOW::modeless == TRUE ��� ����������
//����� ���������� ����
#define MCMD_SHOW_WINDOW                150L

typedef struct _MODULE_WINDOW
{
 DWORD dw_size;
 DWORD window_type;
 HWND  parent_window;
 RECT  region;
 BOOL  modeless;
 GKHANDLE mod_handle;
}MODULE_WINDOW,*LPMODULE_WINDOW;

//���������������� ���� ����
#define MODULE_WND_TYPE_CONFIG   1
#define MODULE_WND_TYPE_VIEWER   2

//������� �������� ����  ������
#define MCMD_CLOSE_WINDOW                151L

//��������� �������� ������ 
#define MCMD_GET_WINDOW_MODULE           152L
//p1 = wchar_t * buff,p2 = buf_cbsz


/*****************************************************************************/
/*                                                                           */
/*                                                                           */
/*    ������� ��� module_main ������� dll                                    */
/*                                                                           */
/*****************************************************************************/

#define MWCM_CREATE_WINDOW               10
//p1 = LPMODULE_WINDOW p2 = BOOL show
//ret = LPVOID WindowHandle
#define MWCM_DESTROY_WINDOW              20
//p1 =  LPVOID WindowHandle

#define MWCM_CLOSE_WINDOW                40
//p1 =  LPVOID WindowHandle

#define MWCM_SHOW_WINDOW                 50
//p1 =  LPVOID WindowHandle , p2 = BOOL modal;

#define MWCM_SETFOCUS_WINDOW             60
//p1 =  LPVOID WindowHandle 

#define MWCM_PROCESS_MESSAGES            100

/*��������� ������� ����������*/
//����������� � �������� / �������� ���� -> ���� ���������
//WPARAM = GKHANDLE handle_for, LPARAM wnd_

#define WGKM_WND_CREATED   (WM_USER+200)
#define WGKM_WND_SHOW      (WM_USER+201)

#define WGKM_WND_HIDE      (WM_USER+202)
#define WGKM_WND_CLOSE     (WM_USER+203)
#define WGKM_WND_DESTROYED (WM_USER+204)

//���������� ���� ��������� ��� ���������
// LPARAM - ����� ������ 
#define WGKM_SHOW_PROPWND  (WM_USER+205) //
#define WGKM_SHOW_VIEWWND  (WM_USER+206)

/*----------------------------------------------------------------------------*/







//������� ����������� ���������
//p1 = ErrorCode , p2 = LPERROR_TEXT
#define MCMD_GETERROR_TEXT               199L

typedef struct _ERROR_TEXT
{
 DWORD    dw_size;
 LPWSTR   error_text;
 DWORD    error_text_size;
 va_list  args;
}ERROR_TEXT,*LPERROR_TEXT;



/************* ������� ������ �������          *****************************/

/* ���� ����������� ������ �������*/

#define MNCODE_ADDED_MODULE   200 //�������� ������
#define MNCODE_REMOVED_MODULE 201 //����� ������
#define MNCODE_RESORT_MODULES 202 //�������������� ������ �������
#define MNCODE_RENAMED_MODULE 203 //������������ ������

//��������� ���������� ������������ � ����������� �������
#define MCMD_GETMODULES_COUNT             200L

//������������ ������������ � ����������� �������
//p1 = int (index);
#define MCMD_ENUM_MODULES                 201L

//����������  ������
//p1 == (wchar_t*)module_image_path
//p2 TRUE - ��������� 
#define MCMD_APPEND_MODULE                202L

//p1 = handle ������������ ������ ,p2 BOOL not_erase
#define MCMD_REMOVE_MODULE                203L

#define MCMD_REPLACE_MODULE               204L
//p1 = handle ����������� ������ ,p2 wchar_t * new_image

#define MCMD_SET_INSTANCE                 205L
#define MCMD_GET_INSTANCE                 206L
//���������/��������� ����� ����������� �����


#pragma pack (pop)

#endif

#endif

