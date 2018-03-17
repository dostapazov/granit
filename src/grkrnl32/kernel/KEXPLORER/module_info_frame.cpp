//---------------------------------------------------------------------------
#pragma hdrstop
#include <vcl.h>
#include <KeRTL.hpp>

#include "module_info_frame.h"
#include "gkexplorer_rc.h"
#include <gkmodules.hpp>
//---------------------------------------------------------------------------
#include "gkmodule_form.h"

#pragma package(smart_init)
#pragma resource "*.dfm"



TModuleInfoFrame *ModuleInfoFrame;
//---------------------------------------------------------------------------
__fastcall TModuleInfoFrame::TModuleInfoFrame(TComponent* Owner)
        : TFrame(Owner)
{

  ListView1->Items->Clear();
  TListItem * item;
  AnsiString str;
  for(int i = IDS_MHANDLE;i<=IDS_REG_INFO;i++)
  {
   item = ListView1->Items->Add();
   item->SubItems->Add(str);
   AnsiString    capt = TGKModuleForm::load_rcstring(ModuleInstance,i);
   item->Caption = capt;
  }
}
//---------------------------------------------------------------------------

void    __fastcall TModuleInfoFrame::set_work_time(__int64  start,__int64 stop,bool running)
{

  __int64 t = stop;
  if(running)
  {
   SYSTEMTIME st;
   GetSystemTime(&st);
   SystemTimeToFileTime(&st,(FILETIME*)&t);
  }
  work_time =   KeRTL::MAX((__int64)0,t-start);
  work_time/=10000000;
  work_time--;
  Timer1Timer(this);//Отрисовать

}


void    __fastcall TModuleInfoFrame::setup_frame(GKHANDLE h)
{
 _handle = h;
 AnsiString str;
 TListItem * item;
 wchar_t text[MAX_PATH<<1];
 get_gkhandle_nameA(h,(char*)text,sizeof(text));
 str.printf("%s %X  счётчик открытий %d",text,DWORD(h),get_gkhandle_usage(h));
 item = ListView1->Items->Item[0];if(item) item->SubItems->Strings[0] = str;

 MODULE_INFO mi;
 mi.dw_size = sizeof(mi);
 mi.mi_mask = MI_FL_TYPE |
              MI_FL_VERSION |
              MI_FL_ICON    |
              /*MI_FL_ICONSM  |*/
              MI_FL_MODULE_TIMES|
              MI_FL_MODULE_STATE
              ;
 call_gkhandle(h,MCMD_GETMODULE_INFO,LPARAM(&mi),0);
 ModIcon->Picture->Icon->Handle = mi.icon;
 str.printf("%d.%d",LOWORD(mi.version),(HIWORD(mi.version))&0x7FFF);
 if(mi.version&0x80000000)
   str.cat_printf(" [ с отладочной информацией ]");
   item = ListView1->Items->Item[1];if(item) item->SubItems->Strings[0] = str;

   int idx = IDS_MOD_TYPE - IDS_BEGIN;
   item = ListView1->Items->Item[idx];
   if(item){str.printf("%04X H",(DWORD)mi.type);item->SubItems->Strings[0] = str;}


 __int64  t1 = *(__int64*)&mi.start_time,t2 = *(__int64*)&mi.stop_time,t3;
 work_time = 0;
 item = ListView1->Items->Item[8];
  if(item) item->SubItems->Strings[0] = ' ';
 Timer1->Enabled = false;
 if(mi.module_state&MODULE_STATE_RUNNING_MASK)
 {
  set_work_time(t1,t2,true);
  Timer1->Enabled =  true;
 }
 else
 set_work_time(t1,t2,false);

 mi.mi_mask = MI_FL_PROVIDER;
 mi.provider = text;
 mi.provider_cbsz = sizeof(text)/sizeof(wchar_t);
 call_gkhandle(h,MCMD_GETMODULE_INFO,LPARAM(&mi),0);
 item = ListView1->Items->Item[6];
  if(item) item->SubItems->Strings[0] = text;

 mi.mi_mask = MI_FL_DESCRIPTION;
 mi.description = text;
 mi.description_cbsz = sizeof(text)/sizeof(wchar_t);
 call_gkhandle(h,MCMD_GETMODULE_INFO,LPARAM(&mi),0);
 //ModDescript->Caption = text;
 item = ListView1->Items->Item[7];
  if(item) item->SubItems->Strings[0] = text;

 mi.mi_mask = MI_FL_DLL_NAME;
 mi.dll_name = text;
 mi.dll_name_cbsz = sizeof(text)/sizeof(wchar_t);
 call_gkhandle(h,MCMD_GETMODULE_INFO,LPARAM(&mi),0);
 //ModDLL->Caption = lstrlenW(text) ? text:L"Встроенный";
 item = ListView1->Items->Item[2];
  if(item) item->SubItems->Strings[0] = text;


 mi.mi_mask = MI_FL_LANGINFO;
 mi.lang_dll_name = text;
 mi.lang_dll_name_cbsz = sizeof(text)/sizeof(wchar_t);
 call_gkhandle(h,MCMD_GETMODULE_INFO,LPARAM(&mi),0);
 item = ListView1->Items->Item[3];
  if(item) item->SubItems->Strings[0] = text;

 CONFIG_PARAMS mcp;
 mcp.dw_size = sizeof(mcp);
 wchar_t reg_key [MAX_PATH<<2];
 wchar_t conffold[MAX_PATH<<1];
 reg_key[0] = conffold[0] = 0;
 mcp.reg_key_name_size = sizeof(reg_key)/sizeof(wchar_t);
 mcp.reg_key_name = reg_key;
 mcp.config_folder_size = sizeof(conffold)/sizeof(wchar_t);
 mcp.config_folder      = conffold;
 call_gkhandle(h,MCMD_GETCONFIG_PARAMS,(LPARAM)&mcp,0);

 item = ListView1->Items->Item[4];
  if(item) item->SubItems->Strings[0] = reg_key;
 item = ListView1->Items->Item[5];
  if(item) item->SubItems->Strings[0] = conffold;
  mi.mi_mask = MI_FL_LICENSE;
  mi.reg_info_cbsz = KERTL_ARRAY_COUNT(text);
  mi.reg_info      = text;
  call_gkhandle(h,MCMD_GETMODULE_INFO,LPARAM(&mi),0);
  item = ListView1->Items->Item[11];
  if(item) item->SubItems->Strings[0] = text;
  Flicensed = mi.license_type == MOD_LICENSE_REGISTERED ? true : false;




 switch(GetPriorityClass(GetCurrentProcess()))
 {
  case IDLE_PRIORITY_CLASS    :  TrackBar1->Position = 0;break;
  case NORMAL_PRIORITY_CLASS  :  TrackBar1->Position = 1;break;
  case HIGH_PRIORITY_CLASS    :  TrackBar1->Position = 2;break;
  case REALTIME_PRIORITY_CLASS:  TrackBar1->Position = 3;break;
 }
}

bool    __fastcall GetCPUTimes(__int64 & idle_time,__int64 & user_time,__int64 & kernel_time);
double  __fastcall CPUUsage   (__int64 & idle_time,__int64 & user_time,__int64 & kernel_time);
double  __fastcall CPUUsage   ();

#pragma warn -8057

void __fastcall TModuleInfoFrame::Timer1Timer(TObject *Sender)
{
 MODULE_INFO mi;
 mi.dw_size = sizeof(mi);
 mi.mi_mask = MI_FL_MEMUSED;
 call_gkhandle(_handle ,MCMD_GETMODULE_INFO,LPARAM(&mi),0);
 wchar_t buf[256];
 *buf = 0;
 TListItem * item  = ListView1->Items->Item[9];
 if(item)
 {
    if(mi.mem_used!=(DWORD)-1)
     wsprintfW(buf,L"%d байт",mi.mem_used);
    item->SubItems->Strings[0] = buf;
 }

 work_time++;
 KeRTL::secunds2HMS(work_time,buf,sizeof(buf));
 try{
     if(ListView1->Items->Count>8)
        ListView1->Items->Item[8]->SubItems->Strings[0] = buf;
    }
    catch(...)
    {
     *buf = 0;
    }
 __int64 cr_time,k_time,user_time;
 GetProcessTimes(GetCurrentProcess(),(LPFILETIME)&cr_time,(LPFILETIME)&cr_time,(LPFILETIME)&k_time,(LPFILETIME)&user_time);
 cr_time = NS100_MSEC(k_time+user_time);
 cr_time/=1000;
 KeRTL::secunds2HMS((DWORD)cr_time,buf,sizeof(buf));
 StaticText2->Caption = buf;

}
//---------------------------------------------------------------------------

void __fastcall TModuleInfoFrame::FrameResize(TObject *Sender)
{
  ListView1->Columns->Items[1]->Width = ListView1->Width - ListView1->Columns->Items[0]->Width-4;
}
//---------------------------------------------------------------------------


char * process_priority_classes[] = {"Низкий","Нормальный","Высокий","Реального времени"};

void __fastcall TModuleInfoFrame::TrackBar1Change(TObject *Sender)
{
  DWORD ppc = HIGH_PRIORITY_CLASS;
  switch(TrackBar1->Position)
  {
   case  0: ppc = IDLE_PRIORITY_CLASS;break;
   case  1: ppc = NORMAL_PRIORITY_CLASS;break;
   case  2: ppc = HIGH_PRIORITY_CLASS;break;
   case  3: ppc = REALTIME_PRIORITY_CLASS;break;
  }
  ProcessPriorityClass->Caption = process_priority_classes[TrackBar1->Position];
  SetPriorityClass(GetCurrentProcess(),ppc);

}
#pragma warn .8057
//---------------------------------------------------------------------------


