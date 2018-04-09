//---------------------------------------------------------------------------
#pragma hdrstop
#include <vcl.h>
#include "mon_frame.h"
#include "gkmodule_form.h"
#include <clipbrd.hpp>

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "gkmod_frame"
#pragma resource "*.dfm"
TIEC60870MonFrame *IEC60870MonFrame;
//---------------------------------------------------------------------------
__fastcall TIEC60870MonFrame::TIEC60870MonFrame(TComponent* Owner)
	: TGKModuleFrame(Owner)
{
  print_radix = 16;
}
//---------------------------------------------------------------------------
void __fastcall TIEC60870MonFrame::BeforeDestruction()
{
  if(mon()) mon.Destroy();
}

//---------------------------------------------------------------------------
void __fastcall TIEC60870MonFrame::Panel2Resize(TObject *Sender)
{
  if(   !mon() )
   create_mon_instance();
   else
   mon.SetSize(Panel2->Width-12,Panel2->Height-12);
}
//---------------------------------------------------------------------------

void __fastcall TIEC60870MonFrame::create_mon_instance()
{
  if(NULL == mon())
  {
  KeWin::TRect r(0,0,Panel2->Width,Panel2->Height);
  r.Grow(-12,-12);
  r.Move(4,4);
  try{
      wchar_t txtv_name [MAX_PATH];
      safe_strcpy(txtv_name,L"IEC60870Monitor");
      int name_len = safe_strlen(txtv_name);
      int create_limit = 128;
      int create_cntr = 0;
      while(!mon.DoCreate(Panel2->Handle,r,-1,1024,txtv_name) && create_cntr<create_limit)
      {
        wsprintfW(txtv_name+name_len,_T("%d"),create_cntr++);
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
        safe_strcpy(log_font.lfFaceName ,_TEXT("Courier New"));
        if(log_font.lfFaceName[0])
         font = CreateFontIndirect(&log_font);
        if(!font)
        {
        font = CreateFont(-12,0,0,0,FW_NORMAL ,0,0,0,
                                Font->Charset,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH,
                                _TEXT("Courier New"));
         GetObject(font,sizeof(log_font),&log_font);
        }
        font = mon.SetFont(font,false);

        //mon.SetBkColor(mon_bkgnd_color);
        mon.set_lines_limit(8192);
        DeleteObject(font);
     }
}



void __fastcall TIEC60870MonFrame::do_print_bytes(LPBYTE ptr,int len,int radix)
{
  char * fmt = "%02X";
  switch(radix)
  {
    case 10: fmt = "%03d";break;
    case  8: fmt = "%03o";break;
  }
   AnsiString str;
   LPBYTE _ptr_beg = ptr;
   LPBYTE _ptr_end = _ptr_beg + len;
   while(_ptr_beg< _ptr_end)
   {
    if(!str.IsEmpty()) str+=' ';
    str.cat_printf(fmt,(DWORD)*_ptr_beg);
    ++_ptr_beg;
   }
   mon.add_line(str.c_str());

}

void __fastcall TIEC60870MonFrame::print_common(DWORD line,LPBYTE ptr,int len,bool tx)
{
       AnsiString str;
       SYSTEMTIME st;
       GetLocalTime(&st);
       str.printf("[ %04u.%02u.%02u %02u:%02u:%02u.%03u ] КП № %d : %s (%d байт)"
                  ,(DWORD)st.wYear,(DWORD)st.wMonth,(DWORD)st.wDay
                  ,(DWORD)st.wHour,(DWORD)st.wMinute,(DWORD)st.wSecond,(DWORD)st.wMilliseconds
                  ,line,tx ? "передача" : "прием",len);
       mon.add_line(str.c_str());
       do_print_bytes(ptr,len,print_radix);
}

void __fastcall TIEC60870MonFrame::print_common_end()
{
 mon.add_line("-----------------------------------------------");
}


void __fastcall TIEC60870MonFrame::do_monotir(DWORD line,LPBYTE ptr,int len,bool tx)
{
   if(bPause->Down || !current_tree_node || DWORD(current_tree_node->Data)!=line ) return;
      print_proto_details(line,ptr,len,tx);
}



void __fastcall TIEC60870MonFrame::bClearClick(TObject *Sender)
{
  mon.clear();
}
//---------------------------------------------------------------------------
void __fastcall TIEC60870MonFrame::print_proto_details(DWORD line,LPBYTE ptr,int len,bool tx)
{
   lpiec60870_proto_header_t phdr = (lpiec60870_proto_header_t)ptr;
   if(len == phdr->get_frame_lenght())
   {
    switch(phdr->get_format())
    {
     case acpi_type_I:
     print_type_I(line,phdr,tx);
     break;
     case acpi_type_S:
        print_type_S(line,phdr,tx);
     break;
     case acpi_type_U:
        print_type_U(line,phdr,tx);
     break;
    }
   }
   else
   {
     mon.add_line("!!!!!!!!!!!!!!!!!!!!!!");
     mon.add_line("Ошибка длины");
     do_print_bytes(ptr,len,this->print_radix);
     mon.add_line("!!!!!!!!!!!!!!!!!!!!!!");
   }
}
//---------------------------------------------------------------------------
 void __fastcall TIEC60870MonFrame::print_type_S       (DWORD line,lpiec60870_proto_header_t phdr,bool tx)
 {
    if(this->tbEnableTypeS->Down)
    {
     print_common(line,(LPBYTE)phdr,iec60870_proto_get_frame_length(phdr),tx);
     if(this->bDetails->Down)
     {
      AnsiString str = "acpi type S ";
      str.cat_printf(" rx_number = %d",phdr->acpiS.rx_number.get_number());
      mon.add_line(str.c_str());
     }
     print_common_end();
    }

 }

 void __fastcall TIEC60870MonFrame::print_type_U       (DWORD line,lpiec60870_proto_header_t phdr,bool tx)
 {
 if(this->tbEnableTypeU->Down)
  {
   print_common(line,(LPBYTE)phdr,iec60870_proto_get_frame_length(phdr),tx);
   if(this->bDetails->Down)
   {
   AnsiString str = "acpi type U ";
   char * sact = "act ";
   char * scon = "con ";
   if(phdr->acpiU.startdt)
   {
     str += "STARTDT ";
     str += (phdr->acpiU.startdt == at_act) ? sact : scon;
   }
   if(phdr->acpiU.stopdt)
   {
     str += "STOPDT " ;
     str += (phdr->acpiU.stopdt == at_act) ? sact : scon;
   }

   if(phdr->acpiU.testfr)
   {
     str += "TESTFR ";
     str += (phdr->acpiU.startdt == at_act) ? sact : scon;
   }
   mon.add_line(str.c_str());
   }
   print_common_end();
  }
 }



AnsiString time2str::operator ()(lpiec60870_cp24time2a tm)
{
  AnsiString str;
  if(tm)
  {
    str.printf("%02u:%02u.%03u ",(unsigned)tm->min,unsigned(tm->msec)/1000,unsigned(tm->msec)%1000);
  }
  return str;
}

AnsiString time2str::operator ()    (lpiec60870_cp56time2a tm)
{
  AnsiString str;
  if(tm)
  {
  str.printf("%04d-%02d-%02d  %02u:%02u:%02u.%03u "
            ,2000+(unsigned)tm->year,(unsigned)tm->month,(unsigned)tm->mday
            ,(unsigned)tm->hour,(unsigned)tm->min,unsigned(tm->msec)/1000,unsigned(tm->msec)%1000);
  }
  return str;
}

 AnsiString quality2str::operator () (lpiec60870_quality_mes quality)
 {
  AnsiString str;
  if(quality)
  {
   str.printf("OV(%2u %s) ",(unsigned)quality->overflow,quality->overflow ? "Переполнение":"Норма");
   str+=(*this)((lpiec60870_quality)quality);
  }
  return str;
 }

 AnsiString quality2str::operator () (lpiec60870_quality     quality)
 {
  AnsiString str;
  if(quality)
  {
   str.printf("IV(%2u %s) "
              "BL(%2d %s) "
              "SB(%2d %s) "
              "NT(%2d %s) "
             ,unsigned(quality->iv)
             ,quality->iv ? "Недостоверно" : "Достоверно"
             ,unsigned(quality->bl)
             ,unsigned(quality->bl) ? "Блокирован" : "Не блокирован"
             ,unsigned(quality->sb)
			 ,unsigned(quality->sb) ? "SB" : ""
			 ,unsigned(quality->nt)
			 ,unsigned(quality->nt) ? "NP" : ""
             );
   }
    return str;
 }


      template <typename obj_t,typename val2str_t>
      AnsiString __fastcall print_object(DWORD obj_number,obj_t * obj,val2str_t vs)
      {
        AnsiString str;
        if(obj)
          {
           str.printf("№-%4u ",obj_number);
           str+=vs(obj);
           str+=time2str ()(obj->get_time());
           str+=quality2str()(obj->get_quality());
          }
        return     str;
      }

      template <typename obj_t,typename val2str_t,typename mon_t >
      void __fastcall print_objects(DWORD obj_count,LPBYTE ptr,DWORD addr_sz,bool seq,val2str_t vs,mon_t * mon)
      {
        AnsiString str;
        obj_t * obj;
        DWORD obj_num;
        obj_num = iec60870_proto_get_obj_addr(ptr,addr_sz);
        ptr+=addr_sz;
        while(obj_count)
        {
          obj = (obj_t *)ptr;
          str = print_object(obj_num,obj,vs);
          mon->add_line(str.c_str());
          ptr+=sizeof(*obj);
          if(--obj_count)
             {
              if(seq)
				 ++obj_num;
				 else
				 {
				  obj_num = iec60870_proto_get_obj_addr(ptr,addr_sz);
				  ptr+=addr_sz;
				 }
			 }
		}
	  }

 void __fastcall TIEC60870MonFrame::print_remote_control(DWORD asdu_type,DWORD obj_count,LPBYTE ptr,int obj_addr_size)
 {
 	 DWORD obj_num = iec60870_proto_get_obj_addr(ptr,obj_addr_size);
	 ptr+=obj_addr_size;
	 iec60870_rctrl * rctrl =  (iec60870_rctrl *) ptr;
	 AnsiString str;
     str.printf("obj-number %d [%s] qu = %d ",obj_num,rctrl->se ? "Select":"Execute",(DWORD)rctrl->qu);
	 switch(asdu_type)
	 {
	   case C_SC_NA_1: str.cat_printf("SWITCH-%s  ",rctrl->var == sp_on ? "ON" :"OFF" );break;
	   case C_DC_NA_1: str.cat_printf("SWITCH-%s ",rctrl->var == dp_on ? "ON" : (rctrl->var == dp_off ? "OFF":"???") );break;
	   case C_RC_NA_1: str.cat_printf("STEP-%s ",rctrl->var == rcs_increment ? "INCR" : (rctrl->var == rcs_decrement ? "DECR":"???") );break;
	 }
	 mon.add_line(str.c_str());
 }

 void __fastcall TIEC60870MonFrame::print_type_I       (DWORD line,lpiec60870_proto_header_t phdr,bool tx)
 {
 if(this->tbEnableTypeI->Down)
 {
  lpiec60870_asdu_header ah = iec60870_proto_get_asdu_header(phdr);
  DWORD type_filter = _wtoi(this->TypeFilter->Text.c_str());
  if(type_filter && type_filter!=(DWORD)ah->asdu_type) return;

  print_common(line,(LPBYTE)phdr,iec60870_proto_get_frame_length(phdr),tx);
  if(bDetails->Down)
  {
   if(ah)
	{
	AnsiString str = "acpi type I ";
	str.cat_printf(" %s rx-кадр № %u tx-кадр %u",tx ? "передача":"прием", (unsigned)phdr->acpiI.rx_number.get_number() , (unsigned)phdr->acpiI.tx_number.get_number());
	mon.add_line(str.c_str());
	DWORD obj_count = (unsigned)ah->obj_count;
	do_print_bytes((LPBYTE)ah,sizeof(*ah),print_radix);
	str.printf("originator %2u  common addr %u"
								  ,(unsigned)ah->origin_addr
								  ,(unsigned)ah->common_addr
								  );
	 mon.add_line(str.c_str());
	 str.printf("ASDU : type %u [%s] Кол-во %3u %s %s"
								 ,(unsigned)ah->asdu_type
                                                                 ,get_asdu_type_name((unsigned)ah->asdu_type)
								 ,obj_count
								 ,ah->seq ? "SQ=1(Инкремент номера объекта)":"SQ=0"
								 ,ah->test ? "**TEST**":""
								 );
	 mon.add_line(str.c_str());

	 str.printf("Причина %3u-%s PN(%2u %s)  "
								  ,(unsigned)ah->cause,this->get_transmit_cause_string((unsigned)ah->cause).c_str()
								  ,(unsigned)ah->pn
								  ,(unsigned)ah->pn ? "Negative":"Positive"
			   );
	   mon.add_line(str.c_str());
	   LPBYTE ptr = (LPBYTE)ah;
	   ptr+=sizeof (*ah);
	   if(obj_count)
	   {
		bool is_sequenced = ah->seq ? true : false;
		switch(ah->asdu_type)
		{
		 case M_SP_NA_1: print_objects<iec60870_type_M_SP_NA_1>(obj_count,ptr,obj_addr_size ,is_sequenced,sp_discrete2str<iec60870_type_M_SP_NA_1>(),&mon)  ;break;
		 case M_SP_TA_1: print_objects<iec60870_type_M_SP_TA_1>(obj_count,ptr,obj_addr_size,is_sequenced,sp_discrete2str<iec60870_type_M_SP_TA_1>() ,&mon)  ;break;
		 case M_DP_NA_1: print_objects<iec60870_type_M_DP_NA_1>(obj_count,ptr,obj_addr_size,is_sequenced,dp_discrete2str<iec60870_type_M_DP_NA_1>() ,&mon)  ;break;
		 case M_DP_TA_1: print_objects<iec60870_type_M_DP_TA_1>(obj_count,ptr,obj_addr_size,is_sequenced,dp_discrete2str<iec60870_type_M_DP_TA_1>() ,&mon)  ;break;
		 case M_ST_NA_1: print_objects<iec60870_type_M_ST_NA_1>(obj_count,ptr,obj_addr_size,is_sequenced,st_discrete2str<iec60870_type_M_ST_NA_1>() ,&mon)  ;break;
		 case M_ST_TA_1: print_objects<iec60870_type_M_ST_TA_1>(obj_count,ptr,obj_addr_size,is_sequenced,st_discrete2str<iec60870_type_M_ST_TA_1>() ,&mon)  ;break;
		 case M_ME_NB_1: print_objects<iec60870_type_M_ME_NB_1>(obj_count,ptr,obj_addr_size,is_sequenced,meashure_short2str<iec60870_type_M_ME_NB_1>(),&mon);break;
		 case M_ME_TB_1: print_objects<iec60870_type_M_ME_TB_1>(obj_count,ptr,obj_addr_size,is_sequenced,meashure_short2str<iec60870_type_M_ME_TB_1>(),&mon);break;
		 case M_ME_NA_1: print_objects<iec60870_type_M_ME_NA_1>(obj_count,ptr,obj_addr_size,is_sequenced,meashure2str<iec60870_type_M_ME_NA_1>()   ,&mon)   ;break;
		 case M_ME_TA_1: print_objects<iec60870_type_M_ME_TA_1>(obj_count,ptr,obj_addr_size,is_sequenced,meashure2str<iec60870_type_M_ME_TA_1>()   ,&mon)   ;break;
		 case M_ME_NC_1: print_objects<iec60870_type_M_ME_NC_1>(obj_count,ptr,obj_addr_size,is_sequenced,meashure2str<iec60870_type_M_ME_NC_1>()   ,&mon)   ;break;
		 case M_ME_TC_1: print_objects<iec60870_type_M_ME_TC_1>(obj_count,ptr,obj_addr_size,is_sequenced,meashure2str<iec60870_type_M_ME_TC_1>()   ,&mon)   ;break;
		 case M_SP_TB_1: print_objects<iec60870_type_M_SP_TB_1>(obj_count,ptr,obj_addr_size,is_sequenced,sp_discrete2str<iec60870_type_M_SP_TB_1>(),&mon)   ;break;
		 case M_DP_TB_1: print_objects<iec60870_type_M_DP_TB_1>(obj_count,ptr,obj_addr_size,is_sequenced,sp_discrete2str<iec60870_type_M_DP_TB_1>(),&mon)   ;break;
		 case M_МЕ_ТD_1: print_objects<iec60870_type_M_ME_TD_1>(obj_count,ptr,obj_addr_size,is_sequenced,meashure2str<iec60870_type_M_ME_TD_1>()   ,&mon)   ;break;
		 case M_МЕ_ТЕ_1: print_objects<iec60870_type_M_ME_TE_1>(obj_count,ptr,obj_addr_size,is_sequenced,meashure2str<iec60870_type_M_ME_TE_1>()   ,&mon)   ;break;
		 case M_ME_TF_1: print_objects<iec60870_type_M_ME_TF_1>(obj_count,ptr,obj_addr_size,is_sequenced,meashure2str<iec60870_type_M_ME_TF_1>()   ,&mon)   ;break;
		 case C_IC_NA_1: mon.add_line("Команда опроса");break;
		 case C_CI_NA_1: mon.add_line("Команда опроса счетчиков"); break;
		 case C_RD_NA_1: mon.add_line("Команда чтения"); break;
		 case C_CS_NA_1: mon.add_line("Команда синхронизации часов"); break;
		 case C_TS_NA_1: mon.add_line("Тестовая команда"); break;
		 case C_RP_NA_1: mon.add_line("Команда установки процесса в исходное состояние Reset"); break;
		 case C_CD_NA_1: mon.add_line("Команда задержки опроса"); break;
		 case C_TS_TA_1: mon.add_line("Тестовая команда с меткой времени СР56Время2а"); break;
		 case C_SC_NA_1:
		 case C_DC_NA_1:
		 case C_RC_NA_1: print_remote_control(ah->asdu_type,obj_count,ptr,obj_addr_size);  break;
		 default       :
		 {
		  AnsiString str;
          str.printf("Неизвестный тип ASDU %u",unsigned(ah->asdu_type));
          mon.add_line(str.c_str());
         }break;

        }
       }
     }
    }
   print_common_end();
  }
 }


void __fastcall TIEC60870MonFrame::SpeedButton1Click(TObject *Sender)
{
  TComponent * comp = dynamic_cast<TComponent*>(Sender);
  if(comp)
	 print_radix = comp->Tag;
}
//---------------------------------------------------------------------------


 AnsiString __fastcall TIEC60870MonFrame::get_transmit_cause_string(DWORD tc)
 {
	const char * tc_cause_strings[] =
	{
	   "По умолчанию"//tc_default = 0
	  ,"Периодически"//tc_periodic =1
	  ,"Фоновое сканирование"//tc_background_scan=2
	  ,"Спорадически"//tc_sporadic=3
	  ,"Сообщение об инициализации"//tc_init_msg=4
	  ,"Опрос"//tc_interrogation = 5
	  ,"Активация"//tc_activation
	  ,"Подтверждение активации"//tc_activation_cfrm
	  ,"Деактивация"//tc_deactivation
	  ,"Подтверждение деактивации"//tc_deactivation_cfrm
	  ,"Завершение деактивации"//tc_deactivation_end
	  ,"Обратка удаленной команды"//tc_back_info_remote_command
	  ,"Обратка локальной команды"//tc_back_info_local_command
	  ,"Передача файла"//tc_file_transmit
	  ,"Резерв 14"//tc_reserv_beg = 14
	  ,"Резерв 15"//tc_reserv_beg = 15
	  ,"Резерв 16"//tc_reserv_beg = 16
	  ,"Резерв 17"//tc_reserv_beg = 17
	  ,"Резерв 18"//tc_reserv_beg = 18
	  ,"Резерв 19"//tc_reserv_beg = 19
	  ,"Ответ на опрос"//tc_interrogation_respond
	  ,"Ответ группы 1"//tc_repond_group1
	  ,"Ответ группы 2"//tc_repond_group2
	  ,"Ответ группы 3"//tc_repond_group3
	  ,"Ответ группы 4"//tc_repond_group4
	  ,"Ответ группы 5"//tc_repond_group5
	  ,"Ответ группы 6"//tc_repond_group6
	  ,"Ответ группы 7"//tc_repond_group7
	  ,"Ответ группы 8"//tc_repond_group8
	  ,"Ответ группы 9"//tc_repond_group9
	  ,"Ответ группы 10"//tc_repond_group10
	  ,"Ответ группы 11"//tc_repond_group11
	  ,"Ответ группы 12"//tc_repond_group12
	  ,"Ответ группы 13"//tc_repond_group13
	  ,"Ответ группы 14"//tc_repond_group14
	  ,"Ответ группы 15"//tc_repond_group15
	  ,"Ответ группы 16"//tc_repond_group16
	  ,"Общий ответ опроса счетчиков"//tc_interrogation_respond_counters
	  ,"Ответ опроса счетчиков"//tc_repond_counters1
          ,"Ответ опроса счетчиков"//tc_repond_counters2
          ,"Ответ опроса счетчиков"//tc_repond_counters3
          ,"Ответ опроса счетчиков"//tc_repond_counters4
          ,"Резерв 42"//tc_reserv1_beg = 42
          ,"Резерв 43"//tc_reserv1_end = 43
          ,"Неизвестый тип"//tc_unknown_type_id = 44
          ,"Неизвестная причина передачи"//tc_unknown_transmission_cause = 45,
          ,"Неизвестный общий адрес ASDU"//tc_unknown_asdu_common_addr   = 46
          ,"Неизвестный адрес объекта"//tc_unknown_object_addr        = 47
    };
  AnsiString str;
  if(tc<tc_max_number)
	  str = tc_cause_strings[tc];
      else
      str = "????";
   return str;
 }



void __fastcall TIEC60870MonFrame::tbEnableTypeIClick(TObject *Sender)
{
 TypeLabel->Visible   = tbEnableTypeI->Down;
 TypeFilter->Visible  = tbEnableTypeI->Down;
}
//---------------------------------------------------------------------------


AnsiString __fastcall TIEC60870MonFrame::get_asdu_type_name       (DWORD asdu_type)
{
const char * asdu_type_text[] =
{
  "Not used",
  "M_SP_NA_1",// = 1 ,//информация одноэлементная
  "M_SP_TA_1",// = 2 ,//информация одноэлементная     c меткой времени
  "M_DP_NA_1",// = 3 ,//информация двухэлементная
  "M_DP_TA_1",// = 4 ,//информация двухэлементная     c меткой времени
  "M_ST_NA_1",// = 5 ,//информация положение отпаек
  "M_ST_TA_1",// = 6 ,//информация положение отпаек  c меткой времени
  "M_BO_NA_1",// = 7 ,//Строка 32 бит
  "M_BO_TA_1",// = 8 ,//Строка 32 бит   с меткой времени
  "M_ME_NA_1",// = 9 ,//Значение измеряемой величины нормализованное
  "M_ME_TA_1",// = 10,//Значение измеряемой величины нормализованное    c меткой времени
  "M_ME_NB_1",// = 11,//Значение измеряемой величины масштабированное
  "M_ME_TB_1",// = 12,//Значение измеряемой величины масштабированное   c меткой времени
  "M_ME_NC_1",// = 13,//Значение измеряемой величины short float
  "M_ME_TC_1",// = 14,//Значение измеряемой величины short float   c меткой времени
  "M_IT_NA_1",// = 15,//Интегральные суммы
  "M_IT_TA_1",// = 16,//Интегральные суммы c меткой времени
  "M_EP_TA_1",// = 17,//Работа устройств защиты с меткой времени
  "M_EP_TB_1",// = 18,//Упакованная информация о срабатывании пусковых органов устройств защиты с меткой времени
  "M_EP_TC_1",// = 19,//Упакованная информация о срабатывании в выходных цепях устройства защиты с меткой времени
  "M_PS_NA_1",// = 20,//Упакованная одноэлементная информация с определением изменения состояния
  "M_ME_ND_1",// = 21,//Значение измеряемой величины, нормализованное значение без описателя качества

  "ASDU_TYPE_22",//,
  "ASDU_TYPE_23",//,
  "ASDU_TYPE_24",//,
  "ASDU_TYPE_25",//,
  "ASDU_TYPE_26",//,
  "ASDU_TYPE_27",//,
  "ASDU_TYPE_28",//,
  "ASDU_TYPE_29",//,

  "M_SP_TB_1",// =30 ,//Одноэлементная информация с меткой времени СР56Время2а
  "M_DP_TB_1",// =31 ,//Двухэлементная информация с меткой времени СР56Время2а
  "M_ST_TB_1",// =32 ,//Информация о положении отпаек с меткой времени СР56Время2а
  "M_BO_TB_1",// =33 ,//Строка из 32 битов с меткой времени СР56Время2а
  "M_МЕ_ТD_1",// =34 ,//Значение измеряемой величины, нормализованное значение с меткой времени СР56Время2а
  "M_МЕ_ТЕ_1",// =35 ,//Значение измеряемой величины масштабированное значение с меткой времени СР56Время2а
  "M_ME_TF_1",// =36 ,//Значение измеряемой величины, короткий формат с плавающей запятой с меткой времени СР56Время2а
  "M_IT_TB_1",// =37 ,//Интегральные суммы с меткой времени СР56Время2а
  "M_EP_TD_1",// =38 ,//Работа устройств защиты с меткой времени СР56Время2а
  "M_EP_TE_1",// =39 ,//Упакованное сообщение о срабатывании пусковых органов устройств защиты с меткой времени СР56Время2а
  "M_EP_TF_1",// =40 ,//Упакованная информация о срабатывании в выходных цепях устройства защиты с меткой времени СР56Время2а

  "ASDU_TYPE_41",//,
  "ASDU_TYPE_42",//,
  "ASDU_TYPE_43",//,
  "ASDU_TYPE_44",//,

  "C_SC_NA_1 single point remote control",// =45 ,// Однопозиционная команда
  "C_DC_NA_1 double point remote control",// =46 ,// Двухпозиционная команда
  "C_RC_NA_1 step regulation",// =47 ,// Команда пошагового регулирования
  "C_SE_NA_1",// =48 ,// Команда уставки, нормализованное значение
  "C_SE_NB_1",// =49 ,// Команда уставки, масштабированное значение
  "C_SE_NC_1",// =50 ,// Команда уставки, короткий формат с плавающей запятой
  "C_BO_NA_1",// =51 ,// Строка из 32 битов

  "ASDU_TYPE_52",//  ,
  "ASDU_TYPE_53",//  ,
  "ASDU_TYPE_54",//  ,
  "ASDU_TYPE_55",//  ,
  "ASDU_TYPE_56",//  ,
  "ASDU_TYPE_57",//  ,

  "C_SC_TA_1",//, //(* (058, 0x3A) Single command with time tag CP56Time2a *)
  "C_DC_TA_1",//, //(* (059, 0x3B) Double command with time tag CP56Time2a *)
  "C_RC_TA_1",//, //(* (060, 0x3C) Regulating step command with time tag CP56Time2a *)
  "C_SE_TA_1",//, //(* (061, 0x3D) Measured value, normalised value command with time tag CP56Time2a *)
  "C_SE_TB_1",//, //(* (062, 0x3E) Measured value, scaled value command with time tag CP56Time2a *)
  "C_SE_TC_1",//, //(* (063, 0x3F) Measured value, short floating point number command with time tag CP56Time2a *)
  "C_BO_TA_1",//, //(* (064, 0x40) Bitstring of 32 bit command with time tag CP56Time2a *)

  "ASDU_TYPE_65",//,
  "ASDU_TYPE_66",//,
  "ASDU_TYPE_67",//,
  "ASDU_TYPE_68",//,
  "ASDU_TYPE_69",//,

  "M_EI_NA_1",// =70 ,//Окончание инициализации

  "C_IC_NA_1",// =100,// Команда опроса
  "C_CI_NA_1",// =101,// Команда опроса счетчиков
  "C_RD_NA_1",// =102,// Команда чтения
  "C_CS_NA_1",// =103,// Команда синхронизации часов
  "C_TS_NA_1",// =104,// Тестовая команда
  "C_RP_NA_1",// =105,// Команда установки процесса в исходное состояние Reset
  "C_CD_NA_1",// =106,// Команда задержки опроса
  "C_TS_TA_1",// =107,// Тестовая команда с меткой времени СР56Время2а

  "ASDU_TYPE",//_108,
  "ASDU_TYPE",//_109,

  "P_ME_NA_1",// =110,// Параметры измеряемых величин,нормализованное значение
  "P_ME_NB_1",// =111,// Параметры измеряемых величин,масштабированное значение
  "P_ME_NC_1",// =112,// Параметры измеряемых величин, короткий формат с плавающей запятой
  "P_AC_NA_1",// =113,// Активация параметра

  "ASDU_TYPE_114",//,
  "ASDU_TYPE_115",//,
  "ASDU_TYPE_116",//,
  "ASDU_TYPE_117",//,
  "ASDU_TYPE_118",//,
  "ASDU_TYPE_119",//,

  "F_FR_NA_1",//    , //(* (120, 0x78) File ready *)
  "F_SR_NA_1",//    , //(* (121, 0x79) Section ready *)
  "F_SC_NA_1",//    , //(* (122, 0x7A) Call directory, select file, call file, call section *)
  "F_LS_NA_1",//    , //(* (123, 0x7B) Last section, last segment *)
  "F_FA_NA_1",//    , //(* (124, 0x7C) ACK file, ACK section *)
  "F_SG_NA_1",//    , //(* (125, 0x7D) Segment *)
  "F_DR_TA_1",//    , //(* (126, 0x7E) Directory *)
  "ASDU_TYPE_127",//,
  "ASDU_TYPE_RESERV_FIRST",// = 128,
  "ASDU_TYPE_RESERV_LAST",//  = 255,
  "ASDU_TYPE_MAX"//
};
 AnsiString str;
 if(asdu_type < ASDU_TYPE_MAX)
  str = asdu_type_text[asdu_type];
  else
  str = "type-?";
  return str;
}

void __fastcall TIEC60870MonFrame::bClipbrdClick(TObject *Sender)
{
  //
  TStringList * sl = new TStringList;
  unsigned lines = mon.get_lines_count();
  for(unsigned i = 0;i<lines;i++)
  {
    char buffer[8192];
    mon.get_line_text(i,buffer,sizeof(buffer));
    sl->Add(UnicodeString(buffer));
  }

  Clipboard()->AsText = sl->Text;
  delete sl;
}
//---------------------------------------------------------------------------


void __fastcall TIEC60870MonFrame::bDetailsClick(TObject *Sender)
{
//  BYTE data [6] = {0x68,4,1,0,2,0};
//  this->do_monotir((DWORD)current_tree_node->Data,data,sizeof(data),false);
}
//---------------------------------------------------------------------------

