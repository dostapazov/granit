//---------------------------------------------------------------------------

#include <windows.h>
#include <vcl.h>

#include "modbus_mon_frame.h"
#include "..\modbusmod.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TMonFrame *MonFrame;
//---------------------------------------------------------------------------
__fastcall TMonFrame::TMonFrame(TComponent* Owner)
	: TFrame(Owner)
{
}
//---------------------------------------------------------------------------

void __fastcall TMonFrame::create_mon_instance()
{
  if(NULL == mon())
  {
  KeWin::TRect r(0,0,MonBox->Width,MonBox->Height);
  r.Grow(-12,-12);
  r.Move(4,12);
  try{
	  wchar_t txtv_name [MAX_PATH];
      safe_strcpy(txtv_name,L"ModbusMonitor");
      int name_len = lstrlenW(txtv_name);
      int create_limit = 128;
      int create_cntr = 0;
      while(!mon.DoCreate(MonBox->Handle,r,-1,1024,txtv_name) && create_cntr<create_limit)
      {
        wsprintfW(txtv_name+name_len,L"%d",create_cntr++);
      }
     }
    catch(...)
     { }
   }
      if(mon())
      {
        HFONT font = NULL;
        LOGFONT log_font = {0};
        log_font.lfHeight   = -12;
        log_font.lfCharSet  = RUSSIAN_CHARSET;
        wcscpy(log_font.lfFaceName ,_T("Courier New"));
        if(log_font.lfFaceName[0])
         font = CreateFontIndirect(&log_font);
        if(!font)
        {
        font = CreateFont(-12,0,0,0,FW_NORMAL ,0,0,0,
                                Font->Charset,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH,
                                log_font.lfFaceName);
         GetObject(font,sizeof(log_font),&log_font);
        }
        font = mon.SetFont(font,false);

        //mon.SetBkColor(mon_bkgnd_color);
        mon.set_lines_limit(8192);
        DeleteObject(font);
     }
}
void __fastcall TMonFrame::FrameResize(TObject *Sender)
{
 if(mon())
 {
   mon.SetSize(MonBox->Width-12,MonBox->Height-24);
 }
 else
 create_mon_instance();
}
//---------------------------------------------------------------------------

void __fastcall TMonFrame::mon_raw_data(LPVOID data,DWORD sz)
{
    LPBYTE ptr     = (LPBYTE) data;
    LPBYTE end_ptr = ptr+sz;
    UnicodeString s;
    while(ptr<end_ptr)
    {
     s.cat_printf(L"%02X ",(DWORD)*ptr);
     ++ptr;
    }
    mon.add_line(s.c_str());
}

void __fastcall TMonFrame::mon_header  (DWORD code,DWORD sz)
{
  static wchar_t * what_text[] =
  {
     L""
    ,L"Передача COM-Порт"
    ,L"Прием    COM-Порт"
    ,L"Передача TCP"
    ,L"Прием    TCP"
    ,L"Передача кадра"
    ,L"Прием    кадра"
  };

    mon.add_line(L"--------------------------------------");
    UnicodeString w = what_text[code];
    w.cat_printf(L" длина %d",sz);
    mon.add_line(w.c_str());
}

void __fastcall TMonFrame::monitor_txrx(LPVOID data,DWORD sz,DWORD code)
{
  if(pause_btn->Down) return;

  if(data && sz)
  {
       switch(code)
       {
        case MBNC_TX:
        case MBNC_TXTCP:
          if(this->EnableTX->Checked)
          {
            mon_header(code,sz);
            if(cbShowDetails->Checked)
              mon_details_tx(data,sz,code);
              mon_raw_data(data,sz);
          }
        break;
        case MBNC_RX:
        case MBNC_RXTCP:
        if(EnableRX->Checked)
        {
            mon_header(code,sz);
            if(cbShowDetails->Checked)
               mon_details_rx(data,sz,code);
               mon_raw_data(data,sz);
        }
        break;
      }
  }
}

wchar_t * __fastcall get_modbus_command_name(BYTE cmd)
{
  static wchar_t * cmd_name [] =
    {
     L"???"
     ,L"Read output status"
     ,L"Read input  status"
     ,L"Read holding registers"
     ,L"Read input registers"
     ,L"Force single coil"
     ,L"Preset single register"
     ,L"Read exception status"
     ,L"Diagnostic" //8
     ,L"?"
     ,L"?"
     ,L"Fetch Comm Event Ctrl" //11
     ,L"Fetch Comm Event Log" //12
     ,L"?" //13
     ,L"?" //14
     ,L"Force multiple coils" //15
     ,L"Preset Multiple Regs" //16
     ,L"Read slave id" //17
     ,L"?" //14
     ,L"?" //14
     ,L"Read general reference" //20
     ,L"?" //21
     ,L"Mask write 4XXX register" //22
     ,L"Mask read/write 4XXX register" //23
     ,L"Read Fifo queue" //24
    };
    cmd = MODBUS_RESPOND_COMMAND_MASK(cmd);
    return cmd_name[cmd < 25 ? cmd : 0];
}

int __fastcall get_modbus_command_name(BYTE cmd,wchar_t * buffer,int blen)
{
  return snwprintf(buffer,blen,L"%s%s",get_modbus_command_name(cmd),(cmd&MODBUS_RESPOND_ERROR_FLAG) ? L"-Ошибка" : L"");
}


void __fastcall TMonFrame::mon_details_tx(LPVOID data,DWORD sz,DWORD code)
{
      LPMODBUS_FRAME     mf ;
      UnicodeString s;
      if(code == MBNC_TXTCP)
      {
       LPMODBUS_TCP_FRAME mft = (LPMODBUS_TCP_FRAME)(data);
       mf  = &mft->modbus_frame;

       s.printf(L"Номер кадра %d длина %d",(int)mft->hdr.frame_number,modbus_word(mft->hdr.frame_len));
       mon.add_line(s.c_str());
      }
      else
      mf = (LPMODBUS_FRAME)data ;
      if(mf)
      {

        s.printf(L"Номер устройства %d команда  %d(%s)",(int)mf->addr,mf->data.command,get_modbus_command_name(mf->data.command));
        mon.add_line(s.c_str());
      }
}

void __fastcall TMonFrame::mon_details_rx(LPVOID data,DWORD sz,DWORD code)
{
      LPMODBUS_FRAME     mf ;
      UnicodeString s;
      if(code == MBNC_RXTCP)
      {
       LPMODBUS_TCP_FRAME mft = (LPMODBUS_TCP_FRAME)(data);
       mf  = &mft->modbus_frame;

       s.printf(L"Номер кадра %d длина %d",(int)mft->hdr.frame_number,modbus_word(mft->hdr.frame_len));
       mon.add_line(s.c_str());
      }
      else
      mf = (LPMODBUS_FRAME)data ;

      if(mf)
      {
        wchar_t buffer[512];
        get_modbus_command_name(mf->data.command,buffer,sizeof(buffer)/sizeof(buffer[0]));
        s.printf(L"Номер устройства %d команда  %d(%s)",(int)mf->addr,mf->data.command,buffer);
        mon.add_line(s.c_str());
      }
}


void __fastcall TMonFrame::clr_btnClick(TObject *Sender)
{
 mon.clear();
}
//---------------------------------------------------------------------------
void __fastcall TMonFrame::pause_btnClick(TObject *Sender)
{
  //pause_btn->Down = !pause_btn->Down;
}
//---------------------------------------------------------------------------
