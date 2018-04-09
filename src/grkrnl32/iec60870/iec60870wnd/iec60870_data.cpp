//---------------------------------------------------------------------------
#pragma hdrstop
#include <vcl.h>
#pragma hdrstop

#include "iec60870_data.h"
#include "mon_frame.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "gkmod_frame"
#pragma resource "*.dfm"

TIEC60870DataFrame *IEC60870DataFrame;

int __fastcall TIEC60870DataFrame::command2index(int command)
{
  int index = 0;
  switch (command)
  {
    case C_SC_NA_1: index = 1; break;
    case C_DC_NA_1: index = 2; break;
    case C_RC_NA_1: index = 3; break;
  }
 return index;
}

int __fastcall TIEC60870DataFrame::index2command(int idx    )
{
  int command = -1;
  switch (idx)
  {
    case 1: command = C_SC_NA_1; break;
    case 2: command = C_DC_NA_1; break;
    case 4: command = C_RC_NA_1; break;
  }
 return command;

}

//---------------------------------------------------------------------------
__fastcall TIEC60870DataFrame::TIEC60870DataFrame(TComponent* Owner)
	: TGKModuleFrame(Owner)
{
 this->Funassigned_count = this->Fassigned_count = 0;
}

//---------------------------------------------------------------------------
void     __fastcall  TIEC60870DataFrame::current_object_change (TObject * value)
{
  TGKModuleFrame::current_object_change(value);
  setup_controls ();
}

DWORD    __fastcall TIEC60870DataFrame::get_line_number()
{
  return this->current_tree_node ? (DWORD)current_tree_node->Data : (DWORD)-1;
}


void     __fastcall  TIEC60870DataFrame::setup_controls ()
{
   if(line_number!=DWORD(-1))
   {
    data_typesClick(this);
   }
   else
   ASDU_Records->Clear() ;

}


void    __fastcall  TIEC60870DataFrame::notify_record(DWORD line_num ,LPBYTE data,DWORD data_size )
{
  iec60870_record & rec = *(lpiec60870_record )data;
  if(data_size == sizeof(rec) &&  line_num == this->line_number && rec.otd_fa == data_types->ItemIndex )
  {
    TListItem * item = ASDU_Records->FindData(0,(LPVOID)rec.number,true,false);
    if(!item)
      {
       if(!TabControl1->TabIndex || (rec.otd_number == DWORD(-1) && 2 == TabControl1->TabIndex ) || (rec.otd_number != DWORD(-1) && 1 == TabControl1->TabIndex ))
          item = create_record_item();
      }

   if(item)
    {
       if(!TabControl1->TabIndex || (rec.otd_number == DWORD(-1) && 2 == TabControl1->TabIndex ) || (rec.otd_number != DWORD(-1) && 1 == TabControl1->TabIndex ))
         update_item(item,rec);
         else
         item->Delete();
    }

  }
}

void __fastcall TIEC60870DataFrame::update_item(TListItem *Item, const iec60870_record & rec )
{
          Item->Data    = (LPVOID)rec.number;
          Item->Caption = rec.number;
          if(rec.changes_mask&(IEC60870_REC_FL_VALUE|IEC60870_REC_FL_IS_FLOAT))
          {
           AnsiString str;
           if(rec.is_float)
             str.printf("%.2f",rec.fl_value);
            else
             str.printf("%u",rec.dw_value);
           Item->SubItems->Strings[REC_VALUE_SUBITEM] =  str ;
            str.Delete(1,1+str.Length());
            if(rec.is_float)
              str = '+';
           Item->SubItems->Strings[REC_FLOAT_SUBITEM] = (str);
          }


          if(rec.changes_mask&IEC60870_REC_FL_QUALITY)
          {
              UnicodeString str_plus = L'+';
              UnicodeString str_empt;
              Item->SubItems->Strings[REC_IV_SUBITEM] = rec.quality.iv ? str_plus : str_empt;
              Item->SubItems->Strings[REC_BL_SUBITEM] = rec.quality.bl ? str_plus : str_empt;
              Item->SubItems->Strings[REC_SB_SUBITEM] = rec.quality.sb ? str_plus : str_empt;
              Item->SubItems->Strings[REC_NP_SUBITEM] = rec.quality.nt ? str_plus : str_empt;
              Item->SubItems->Strings[REC_OV_SUBITEM] = rec.is_float && (rec.quality.var&1) ? str_plus : str_empt;
           }

         if(rec.changes_mask & IEC60870_REC_FL_TIMESTAMP)
         {
           UnicodeString str;
           if(rec.timestamp)
           {
            SYSTEMTIME st;
            FileTimeToSystemTime((LPFILETIME)&rec.timestamp,&st);
            str.printf(L"%04d-%02d-%02d %02d:%02d:%02d.%03d"
                       ,(int)st.wYear
                       ,(int)st.wMonth
                       ,(int)st.wDay
                       ,(int)st.wHour
                       ,(int)st.wMinute
                       ,(int)st.wSecond
                       ,(int)st.wMilliseconds
                       );


           }
           Item->SubItems->Strings[REC_TIME_SUBITEM] = str;
         }



        if(rec.changes_mask & (IEC60870_REC_FL_OTD_GROUP|IEC60870_REC_FL_OTD_NUMBER))
        {
          UnicodeString str;

          if(rec.otd_number!=DWORD(-1))
           {
            str.printf(L"%2u-%4u",(unsigned)rec.otd_group,(unsigned)rec.otd_number);
           }
           Item->SubItems->Strings[REC_OTD_ADDR_SUBITEM] = str;
        }

        if(Item->Owner->Owner->Selected == Item &&  rec.changes_mask & (IEC60870_REC_FL_RC_COMMAND|IEC60870_REC_FL_RC_NUMBER))
        {
          setup_rc_params(rec);
        }

        if(rec.changes_mask & IEC60870_REC_FL_RC_STATE)
        {
          UnicodeString str;
          if(rec.rc_state&(OTD_PSTATE_TUTR_ACTIVE|OTD_PSTATE_TUTR_PREPARE))
            str += L'У';
            else
            {
             if(rec.rc_state&(OTD_PDIAG_TUTR_DESCRIPT|OTD_PDIAG_TUTR_FAIL))
               str += L"Err";
            }

          Item->SubItems->Strings[REC_OTD_RC_STATE_SUBITEM] = str;
        }

        if(rec.changes_mask & IEC60870_REC_FL_ASDU_TYPE)
        {
          UnicodeString str;
          if(rec.asdu_type)
             str.printf(L"%d [ %s ]",(DWORD)rec.asdu_type,UnicodeString(TIEC60870MonFrame::get_asdu_type_name((DWORD)rec.asdu_type)).c_str());
          Item->SubItems->Strings[REC_ASDU_TYPE_SUBITEM] = str;
        }


}



TListItem * __fastcall TIEC60870DataFrame::create_record_item()
{
 TListItem * item =  ASDU_Records->Items->Add();
 for(int i = 0;i<REC_MAX_SUBITEMS;i++)
     item->SubItems->Add(UnicodeString());
 return item;
}

void     __fastcall  TIEC60870DataFrame::setup_records(int otd_fa,int items_count)
{
  ASDU_Records->Items->BeginUpdate();
  ASDU_Records->Clear();
  iec60870_record rec;
  rec.otd_fa  =  otd_fa;
  int index   =  0;
  while(index < items_count)
  {
    rec.number = index++;
    if(GKH_RET_ERROR!= mod_iface(CMD_IEC60870_LINE_ENUM_ASDU_RECORDS,line_number,(LPARAM)&rec))
     {
        if(!TabControl1->TabIndex || (rec.otd_number == DWORD(-1) && 2 == TabControl1->TabIndex ) || (rec.otd_number != DWORD(-1) && 1 == TabControl1->TabIndex )  )
        {
         TListItem * Item = create_record_item();
         rec.changes_mask = -1;
         update_item(Item,rec);
        }
     }
  }
  ASDU_Records->Items->EndUpdate();


}


void __fastcall TIEC60870DataFrame::data_typesClick(TObject *Sender)
{
   ASDU_Records->Items->BeginUpdate();
   ASDU_Records->Clear();
   unassigned_count = 0;
   assigned_count   = 0;
   DWORD items_count = mod_iface.call(CMD_IEC60870_LINE_GET_ASDU_RECORDS_COUNT,line_number,data_types->ItemIndex);
   if((DWORD)GKH_RET_ERROR != items_count)
     {
       setup_records(data_types->ItemIndex,items_count);
     }
   ASDU_Records->Items->EndUpdate();
}
//---------------------------------------------------------------------------

void __fastcall TIEC60870DataFrame::set_unassigned(int v)
{
  if(Funassigned_count!=v)
    {
      Funassigned_count=std::max(0,v);
      obj_count->Caption = Funassigned_count;
    }
   bAssignGroup->Enabled = Funassigned_count ? true : false;
}

void __fastcall TIEC60870DataFrame::set_assigned(int v)
{
  if(Fassigned_count!=v)
    {
      Fassigned_count=std::max(0,v);
    }
   bRemoveGroup->Enabled   = Fassigned_count ? true : false;
}


void     __fastcall  TIEC60870DataFrame::calc_unassigned_count()
{
    int uc = 0;
    int ac = 0;
    for(int i = 0;i<ASDU_Records->Items->Count;i++)
       {
         TListItem * item = ASDU_Records->Items->Item[i];
         if(item->Selected)
         {
          if( item->SubItems->Strings[REC_OTD_ADDR_SUBITEM].IsEmpty())
            ++uc;
            else
            ++ac;
         }
       }
    unassigned_count = uc;
    assigned_count   = ac;
}

void __fastcall TIEC60870DataFrame::ASDU_RecordsSelectItem(TObject *Sender, TListItem *Item,
          bool Selected)
{
    if(Item )
    {
     if(Item->SubItems->Strings[REC_OTD_ADDR_SUBITEM].IsEmpty())
         {
          unassigned_count+= Selected ? 1:-1;
         }
         else
         {
          assigned_count += Selected ? 1:-1;
          iec60870_record  rec(data_types->ItemIndex,(DWORD)Item->Data);
          mod_iface.call(CMD_IEC60870_LINE_GET_ASDU_RECORD,line_number,(LPARAM)&rec);
          setup_rc_params(rec);
         }
    }
    else
    {
      unassigned_count = 0;
      assigned_count   = 0;
    }

}
//---------------------------------------------------------------------------
void     __fastcall  TIEC60870DataFrame::setup_rc_params(const iec60870_record & rec)
{
  cbRcCommand->ItemIndex = command2index(rec.rc_command);
  if(rec.rc_number !=(DWORD)-1)
	 {
	  rc_object->Text        = rec.rc_number;
	  current_cqu            = (iec60870_command_quality)rec.rc_ctrl.qu;
	  this->rc_timeout->Text = rec.rc_timer;
	 }
	 else
	 rc_object->Text = UnicodeString();


}

void __fastcall TIEC60870DataFrame::otd_groupChange(TObject *Sender)
{
  //UnicodeString capt = otd_group->Text.Trim().IsEmpty() ? L"Убрать" :L"Назначить";
  //capt+= L" группу";
  ///bAssignGroup->Caption  = capt;
}
//---------------------------------------------------------------------------

void __fastcall TIEC60870DataFrame::assign_otd(DWORD otd_fa,DWORD otd_group,DWORD otd_number)
{
 bool unassign = otd_number == DWORD(-1) ? true : false;
 IEC60870_LINE_GROUP_PARAM lgp(otd_fa,otd_group);
 iec60870_record rec (otd_fa,0);
 rec.otd_group = otd_group;

 TListItem * item =  ASDU_Records->Selected;
 int cnt = 0;
 do
 {
   bool not_assigned = item->SubItems->Strings[REC_OTD_ADDR_SUBITEM].IsEmpty();
   if(unassign == !not_assigned  )
    {
     rec.number      = (DWORD)item->Data;
     rec.otd_group   = unassign ? -1 : otd_group;
     rec.otd_number  = unassign ? -1 : otd_number;

     if(GKH_RET_SUCCESS == mod_iface.call(CMD_IEC60870_LINE_ASDU_RECORD_BIND_OTD,line_number,(LPARAM)&rec))
      {
        ++cnt;
        unassigned_count+= unassign ?  1 :-1;
        assigned_count  += unassign ? -1 : 1;
        item = ASDU_Records->GetNextItem(item,sdAll,TItemStates()<<isSelected);

      }
      ++otd_number;
     }
    else
     item = ASDU_Records->GetNextItem(item,sdAll,TItemStates()<<isSelected);

 }while(item);

 if(cnt && !unassign)
    mod_iface.call(CMD_IEC60870_LINE_GROUP_CREATE,line_number,(LPARAM)&lgp);
}

void __fastcall TIEC60870DataFrame::bAssignGroupClick(TObject *Sender)
{
 UnicodeString num_str;
 num_str = otd_group->Text.Trim();
 IEC60870_LINE_GROUP_PARAM lgp(data_types->ItemIndex,_wtoi(num_str.c_str()));
 num_str = otd_number->Text.Trim();
 DWORD    otd_number = _wtoi(num_str.c_str());
 assign_otd(lgp.otd_fa,lgp.otd_group,otd_number);

}
//---------------------------------------------------------------------------

void __fastcall TIEC60870DataFrame::bRemoveGroupClick(TObject *Sender)
{
 UnicodeString num_str;
 num_str = otd_group->Text.Trim();
 IEC60870_LINE_GROUP_PARAM lgp(data_types->ItemIndex,_wtoi(num_str.c_str()));
 //num_str = otd_number->Text.Trim();
 //DWORD    otd_number = _wtoi(num_str.c_str());
 assign_otd(lgp.otd_fa,-1,-1);
}
//---------------------------------------------------------------------------



bool __fastcall TIEC60870DataFrame::assign_rc_command(DWORD rec_number,DWORD otd_fa,DWORD rc_command)
{
 iec60870_record rec (otd_fa,rec_number);
 if(rc_command != (DWORD)-1)
 {
  rec.rc_command  = rc_command;
  rec.rc_number   = _wtoi(rc_object->Text.Trim().c_str());
  rec.rc_timer    = _wtoi(rc_timeout->Text.Trim().c_str());
  rec.rc_ctrl.qu  = current_cqu;
 }

 //int cnt = 0;
 if(GKH_RET_SUCCESS == mod_iface.call(CMD_IEC60870_LINE_ASDU_RECORD_BIND_OTD_RC_CMD,line_number,(LPARAM)&rec))
    {
     return true;
    }
 return false;
}

void __fastcall TIEC60870DataFrame::bAssignTuTrClick(TObject *Sender)
{
 ;//Назначение Команды ТУ
 if(ASDU_Records->Selected )
    {
      DWORD otd_fa     = data_types->ItemIndex;
      DWORD rc_command = index2command(cbRcCommand->ItemIndex);
      assign_rc_command((DWORD)ASDU_Records->Selected->Data,otd_fa,rc_command);
    }
}
//---------------------------------------------------------------------------

void     __fastcall TIEC60870DataFrame::set_current_cqu (iec60870_command_quality cqu)
{
    if(Fcurrent_cqu!= cqu )
    {
      Fcurrent_cqu = cqu;
      switch (cqu) {
          case qu_short_pulse: rb_imp_short->Checked   = true; break;
          case qu_long_pulse : rb_imp_long ->Checked   = true; break;
          case qu_persistent : rb_imp_persist->Checked = true; break;
          default            : rb_imp_user->Checked    = true; break;
      }
      rc_imp_param->Text = cqu;
    }
}


void __fastcall TIEC60870DataFrame::rb_imp_shortClick(TObject *Sender)
{
  TRadioButton * rb = dynamic_cast<TRadioButton*>(Sender);
  if(rb)
     {
      this->current_cqu = (iec60870_command_quality)rb->Tag;
     }
}
//---------------------------------------------------------------------------

void __fastcall TIEC60870DataFrame::rc_imp_paramChange(TObject *Sender)
{
   int value = _wtoi(rc_imp_param->Text.Trim().c_str());
   value = std::max(0,std::min((int)qu_max_private_reserv,value));
   current_cqu = (iec60870_command_quality)value;
   if( (int)current_cqu != _wtoi(rc_imp_param->Text.Trim().c_str()))
      rc_imp_param->Text = current_cqu;
}
//---------------------------------------------------------------------------




