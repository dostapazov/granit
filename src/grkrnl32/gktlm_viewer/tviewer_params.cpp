#pragma hdrstop
#include "gktlm_viewer.h"
#include <kertl.hpp>
#include <tregstry.hpp>

  bool __fastcall Tviewer_parameters::write(HKEY reg_key)
  {
     KeRTL::TRegsWriter rw(KeRTL::DupKey(reg_key));
     if(rw.IsKeyOpen())
     {

         rw.WriteString(REGSTR_DATA_FOLDER   ,data_dir.c_str()        ,true);
         rw.WriteString(REGSTR_EXPORT_FOLDER ,export_folder.c_str()     ,true);
         rw.WriteString(REGSTR_EXPORT_VIEWER ,export_viewer.c_str()     ,true);
         rw.WriteDword (REGDW_EXTERNAL_SOURCE,external_source           ,true);
         rw.WriteDword (REGDW_DOUBLE_BUFFERED,enable_double_buffered    ,true);
         rw.WriteDword (REGDW_TIME_BEFORE    ,history_time_before       ,true);
         rw.WriteDword (REGDW_TIME_AFTER     ,history_time_after        ,true);
         rw.WriteDword (REGDW_READY_COLOR    ,(DWORD)ready_color        ,true);

         rw.WriteString(REGSTR_AUTO_OPEN_LAYOUT,auto_open_layout.c_str(),true);
         rw.WriteDword (REGDW_HKEY_CLEARALL  ,hot_key_clear_all         ,true);
         rw.WriteDword (REGDW_HKEY_RECEIPTALL,hot_key_receipt_all       ,true);
         rw.WriteDword (REGDW_HKEY_ORDER_PERIOD,order_period            ,true);
         rw.WriteDword (REGDW_HKEY_STABLE_READY_TIMER,stable_ready_timer,true);
         rw.WriteBytes (REGDW_HKEY_READY_COLORS,&ready_colors,sizeof(ready_colors),true);

         rw.WriteString(REGSTR_SND_DISPARITY   ,snd_disparity.c_str()   ,true);
         rw.WriteString(REGSTR_SND_BACK        ,snd_back.c_str()        ,true);
         rw.WriteDword (REGDW_DISPLAY_NUMBER   ,main_form_monitor       ,true);
         rw.WriteDword (REGDW_READY_SPACER     ,ready_spacer            ,true);
         rw.WriteBytes (REGBIN_READY_FONT,&ready_font,sizeof(ready_font),true);
         return true;
     }
    return false;
  }

  bool __fastcall Tviewer_parameters::read (HKEY reg_key)
  {

       KeRTL::TRegsReader rd(KeRTL::DupKey(reg_key));
       if(rd.IsRootOnly())
       {
         wchar_t buf[4096];
         DWORD   bsz = KERTL_ARRAY_COUNT(buf);

         buf[rd.ReadString(REGSTR_DATA_FOLDER,buf,bsz,true)   ]=0;
         if(*buf) data_dir = buf;
         buf[rd.ReadString(REGSTR_EXPORT_FOLDER,buf,bsz,true) ]=0;
         if(buf)  export_folder = buf;
         buf[rd.ReadString(REGSTR_EXPORT_VIEWER,buf,bsz,true) ]=0;
         if(buf)  export_viewer = buf;
         buf[rd.ReadString(REGSTR_AUTO_OPEN_LAYOUT,buf,bsz,true)]=0;
         if(*buf) auto_open_layout = buf;
         buf[rd.ReadString(REGSTR_SND_DISPARITY,buf,bsz,true)]=0;
         if(*buf) snd_disparity = buf;
         buf[rd.ReadString(REGSTR_SND_BACK,buf,bsz,true)]=0;
         if(*buf) snd_back = buf;


         external_source         = rd.ReadDword (REGDW_EXTERNAL_SOURCE,0,true);
         enable_double_buffered  = rd.ReadDword (REGDW_DOUBLE_BUFFERED,0,true);
         history_time_before     = rd.ReadDword (REGDW_TIME_BEFORE,5000  ,true);
         history_time_after      = rd.ReadDword (REGDW_TIME_AFTER ,100000,true);
         ready_color             = rd.ReadDword (REGDW_READY_COLOR    ,(DWORD)32768,true);
         hot_key_clear_all       = rd.ReadDword (REGDW_HKEY_CLEARALL  ,0,true);
         hot_key_receipt_all     = rd.ReadDword (REGDW_HKEY_RECEIPTALL,0,true);

         order_period            = rd.ReadDword (REGDW_HKEY_ORDER_PERIOD,0,true);
         stable_ready_timer      = rd.ReadDword (REGDW_HKEY_STABLE_READY_TIMER,5,true);
         ready_colors.dw_size    = sizeof(ready_colors);
         ready_colors.dw_size = rd.ReadBytes(REGDW_HKEY_READY_COLORS,&ready_colors,sizeof(ready_colors),true);
         main_form_monitor = rd.ReadDword (REGDW_DISPLAY_NUMBER,0,true);
         ready_spacer      = rd.ReadDword (REGDW_READY_SPACER  ,2,true);
         rd.ReadBytes(REGBIN_READY_FONT,&ready_font,sizeof(ready_font),true);
       return true;
       }
       return false;

  }

