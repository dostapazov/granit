//---------------------------------------------------------------------------

#pragma hdrstop
#include <vcl.h>

#include "record_frame.h"
#include "mediumdb_frm.h"
#include <stdio.h>
#include <scale.h>
#include <Math.hpp>
#include <limits>

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TRecordFrame *RecordFrame;
//---------------------------------------------------------------------------
__fastcall TRecordFrame::TRecordFrame(TComponent* Owner)
        : TFrame(Owner)
{
     Fapply_mask = 0;
     DoubleBuffered = true;
     scale.open(SCALE_NAME_W);

     StateDiscreteTU->Visible    = IsDebuggerPresent();
     StateDiscreteTUERR->Visible = StateDiscreteTU->Visible;

}
//---------------------------------------------------------------------------

#pragma warn -8019
void      __fastcall TRecordFrame::set_gkhandle(GKHANDLE handle)
{
 mod_iface.open(handle,true);
}
#pragma warn .8019



mdb_record   __fastcall TRecordFrame::Getrecord()
{
        return Frecord;
}

void       __fastcall TRecordFrame::Setrecord(mdb_record & value)
{
 if(Frecord.rec_id!= value.rec_id || create_mode)
   {
    Frecord = value;
    setup_controls(-1);
   }
   apply_mask = 0;
}

void __fastcall TRecordFrame::update_record(DWORD rec_id,DWORD mask)
{
  Frecord.rec_id   = rec_id;
  if(rec_id!=(DWORD)-1  )
  {

   if(apply_mask) return;
   mod_iface.call(MDBCM_RECORD_GET,rec_id,(LPARAM)&Frecord);
  }
  else
  Visible = false;
  setup_controls(mask);
}

AnsiString __fastcall param_addr2str(otd_param_addr & pa)
{
  AnsiString str;
  if(pa.addr.pu<OTD_ADDR_MAXVALUE)
    {
     str.printf("%03d.%03d.%03d.%03d",(DWORD)pa.addr.pu,(DWORD)pa.addr.cp,(DWORD)pa.addr.fa,(DWORD)pa.addr.sb);
     if(pa.param_number<OTD_ADDR_MAXVALUE)
       str.cat_printf("-%03d",(DWORD)pa.param_number);
    }
    else
    str = "Не назначен";
    return str;

}

WideString __fastcall TRecordFrame::get_record_name(DWORD id,bool short_name)
{
  wchar_t buffer[2048];
  LPMDB_RECORD_TEXT rt = (LPMDB_RECORD_TEXT)buffer;
  rt->dw_size  = sizeof(buffer);
  rt->text_size = (sizeof(buffer)-sizeof(*rt))/sizeof(wchar_t);
  rt->text_len  = 0;
  rt->text[0]   = 0;
  mod_iface.call(short_name? MDBCM_RECORD_GET_SNAME:MDBCM_RECORD_GET_NAME,id,(LPARAM)rt);
  return WideString(rt->text,rt->text_len);
}

DWORD      __fastcall TRecordFrame::set_record_name(DWORD id,UnicodeString & str,bool short_name)
{

  wchar_t buffer[2048];
  LPMDB_RECORD_TEXT rt = (LPMDB_RECORD_TEXT)buffer;
  rt->dw_size   = sizeof(buffer);
  rt->text_size = (sizeof(buffer)-sizeof(*rt))/sizeof(wchar_t);
  rt->text_len  = str.Length();
  if(rt->text_len)
   wcsncpy(rt->text,str.c_str(),rt->text_size);
   else
  rt->text[0]   = 0;
  return (DWORD)mod_iface.call(short_name? MDBCM_RECORD_SET_SNAME:MDBCM_RECORD_SET_NAME,id,(LPARAM)rt);
}


void __fastcall TRecordFrame::set_float(TEdit * edit,float val)
{
  UnicodeString text;
  if(!IsInfinite(val))
      text.sprintf(L"%.3f",val);
  edit->Text = text;
}

float __fastcall TRecordFrame::get_float(TEdit * edit)
{
  float ret = std::numeric_limits<float>::infinity();
  if(edit  )
  {
   UnicodeString text = edit->Text.Trim();
   if(!text.IsEmpty())
       ret = _wtof(text.c_str());
  }
  return ret;
}

void       __fastcall TRecordFrame::setup_controls(DWORD mask)
{
  setup_active = TRUE;
  if(create_mode || Frecord.rec_id !=(DWORD)-1 )
  {
  if(mask&~(MDBR_FIELD_LASTVALUE|MDBR_FIELD_VALUE|MDBR_FIELD_DIAG|MDBR_FIELD_TIME|MDBR_FIELD_STATE))
  {
  ++setup_active;
  mdb_record & rec = Frecord;


  if(IsNan( rec.max_alarm))  rec.max_alarm   = std::numeric_limits<float>::infinity();
  if(IsNan( rec.min_alarm))  rec.min_alarm   = std::numeric_limits<float>::infinity();
  if(IsNan( rec.max_danger)) rec.max_danger =  std::numeric_limits<float>::infinity();
  if(IsNan( rec.min_danger)) rec.min_danger =  std::numeric_limits<float>::infinity();

  if(create_mode || Frecord.rec_id ==(DWORD)-1 )
    {
     rec.kvants = rec.addr.addr.fa == OTD_FA_DISCRETE ? 1 : 250;
     rec.max_value =rec.kvants;
    }

  if(rec.options&MDBR_OPT_COMBINED)
  {

   ScaleParam->Visible = false;
   ScaleLabel->Visible = false;
   ScaleId->Visible    = false;
   CombinedRecordParam->Visible = true;
   OptCombineCalcByLast->Checked = (Frecord.options&MDBR_OPT_COMBINE_CALC_BY_LAST) ? true : false;
   OtdAddr->Enabled = false;

   if(mask & (MDBR_COMBINE_LIST|MDBR_COMBINE_FORMULA))
      setup_combine_param();
  }
  else
  {
   OtdAddr->Enabled     = true;
   bool param_visible   = rec.addr.param_number !=(DWORD)-1 ? true:false;
   ScaleLabel->Visible  = param_visible;
   ScaleId->Visible     = param_visible;
   CombinedRecordParam->Visible = false;


   if(mask & MDBR_FIELD_KVANTS)
	  {
	   Kvants->Text =  rec.kvants ;
	  }

   if(mask&MDBR_FIELD_MIN_VALUE )
      set_float(ValueMin,rec.min_value);

   if(mask& MDBR_FIELD_MAX_VALUE)
      set_float(ValueMax,rec.max_value);


     if(mask&MDBR_FIELD_SCALEID)
     {
      SCALE_ENUM scen;
      scen.dw_size = sizeof(scen);
      scen.name[0] = 0;
      if(rec.scale_id!=(DWORD)-1 && scale.is_valid())
      {
       scale.call(CM_SC_GET,rec.scale_id,(LPARAM)&scen);
       ScaleParam->Visible = false;
      }
      else
      ScaleParam->Visible = param_visible  ;
      ScaleId->Text = scen.name;
     }

  }


  if(mask & MDBR_FIELD_OPTIONS)
   {
    OptDanger->Checked   = rec.options&MDBR_OPT_DANGER;
    OptAlarm->Checked    = rec.options&MDBR_OPT_ALARM;
    optInvScale->Checked = rec.options&MDBR_OPT_INVERSE_SCALE;
    optSigned->Checked   = rec.is_signed_kvants();
    OptPseudo->Checked   = rec.is_pseudo ();
    OptCaution->Checked  = rec.is_caution();
   }



  AnsiString str;
  if(mask & MDBR_FIELD_ID)
   ID->Caption = rec.rec_id;
  if(mask & MDBR_FIELD_ADDR)
    OtdAddr->Text = param_addr2str(rec.addr);
  if(mask & MDBR_FIELD_STATE)
    {
     StateWorkOut->Checked       = (rec.state&MDBR_STATE_WORKOUT ) ? true:false;
     StateHasTuTr->Checked       = (rec.state&MDBR_STATE_TUFLAG  ) ? true:false;

     /*Разрешение изменять дискретный параметр*/
     /*
      Дискретные параметры это груповые объекты
      ТС и комбинированные в случае если назначен адрес ТС
     */
     bool is_discrete =(rec.state&MDBR_STATE_DISCRETE) ? true:false;
     bool discrete_state_enabled = true;

     if(rec.options&MDBR_OPT_COMBINED)
     {
      if(rec.addr.param_number != (DWORD)-1 && rec.addr.addr.fa ==(BYTE) OTD_DISCRETE)
         discrete_state_enabled = false;

     }
     else
     {
      if(rec.addr.param_number == (DWORD)-1 || rec.addr.addr.fa == (BYTE)OTD_FA_DISCRETE)
          {
           is_discrete = true;
           discrete_state_enabled = false;
          }
      if(rec.addr.addr.fa != (BYTE)OTD_FA_DISCRETE)
           discrete_state_enabled = false;
     }

       StateDiscreteValue->Checked = is_discrete;
       StateDiscreteValue->Enabled = discrete_state_enabled;
       RecordParam->Visible = is_discrete && (rec.addr.param_number!=DWORD(-1) || rec.options&MDBR_OPT_COMBINED);

    StateDiscreteStatic->Checked = (rec.state&MDBR_STATE_STATIC)   ? true:false;
    StateDiscreteHidden->Checked = (rec.state&MDBR_STATE_HIDDEN)   ? true:false;
    StateDiscreteFixed->Checked  = (rec.state&MDBR_STATE_NORECEIPT)? true:false;
    StateDiscreteNormal->Checked = (rec.state&MDBR_STATE_NORMAL   )? true:false;
    StateDiscreteTU->Checked     = (rec.state&MDBR_STATE_TUTR_ACTIVE)? true:false;
    StateDiscreteTUERR->Checked  = (rec.state&MDBR_STATE_TUTR_ERROR  )? true:false;

    }

   if(mask& MDBR_FIELD_MIN_DANGER)
      set_float(MinDanger,rec.min_danger);

   if(mask& MDBR_FIELD_MAX_DANGER)
      set_float(MaxDanger,rec.max_danger);

   if(mask& MDBR_FIELD_MIN_ALARM)
      set_float(MinAlarm,rec.min_alarm);
   if(mask& MDBR_FIELD_MAX_ALARM)
      set_float(MaxAlarm,rec.max_alarm);

   DangerBox->Visible = OptDanger->Checked && !StateDiscreteValue->Checked;
   AlarmBox->Visible  = OptAlarm ->Checked && !StateDiscreteValue->Checked;


 if(mask & MDBR_FIELD_PERCENT)
  {
  float prc = (float)rec.percent;
  prc/=100.0;
  str.printf("%.2f",prc);
  Percent->Text =str;
  }
 if(mask & MDBR_FIELD_SNAME)
   {
    ShortName->Text = get_record_name(rec.rec_id,true);
    ShortName->Tag  = rec.sname;
    UnicodeString hs;
    if(rec.sname!=DWORD(-1))
       hs.printf(L"sname id = %d",rec.sname);
       ShortName->Hint = hs;
   }
 if(mask & MDBR_FIELD_NAME)
   {
    FullName->Text = get_record_name(rec.rec_id,false);
    FullName->Tag  = rec.name;
    UnicodeString hs;
    if(rec.name!=DWORD(-1))
       hs.printf(L"name id = %d",rec.name);
       FullName->Hint = hs;
   }
   apply_mask &=~mask;
   Visible = true;
   --setup_active;
   }
  }
  else
  Visible = false;
  setup_active = FALSE;
}

void       __fastcall TRecordFrame::combine_list_add(DWORD id)
{
       mdb_record rec ;
       mod_iface.call(MDBCM_RECORD_GET,id,(LPARAM)&rec);
       if(rec.rec_id!=(DWORD)-1)
       {
       TListItem * item = CombineList->Items->Add();
       item->Caption = rec.rec_id;
       item->Data = (LPVOID)rec.rec_id;
       AnsiString str;
       if(rec.addr.addr.addr!=(DWORD)-1)
          str.printf("%u.%u.%u.%u-%u"
          ,(unsigned)rec.addr.addr.pu
          ,(unsigned)rec.addr.addr.cp
          ,(unsigned)rec.addr.addr.fa
          ,(unsigned)rec.addr.addr.sb
          ,(unsigned)rec.addr.param_number
          );
       item->SubItems->Add(str);
       str = get_record_name(rec.rec_id,false);
       if(!str.Length())
          str = get_record_name(rec.rec_id,true);
       item->SubItems->Add(str);
       }
       
}

void       __fastcall TRecordFrame::setup_combine_list()
{
  DWORD obj_count = mod_iface.call(MDBCM_COMBINE_GET_COUNT,Frecord.rec_id,0);
  CombineList->Clear();
   CombObjId->Text = AnsiString();

  if(obj_count)
  {
    obj_count<<=1;
    DWORD size = sizeof(MDB_COMBINE_LIST)+sizeof(DWORD)*obj_count;
    LPBYTE buffer = new BYTE[size];
    LPMDB_COMBINE_LIST lst = (LPMDB_COMBINE_LIST) buffer;
    lst->dw_size  = size;
    lst->capacity = obj_count+1;
    lst->start    = 0;
    if(mod_iface.call(MDBCM_COMBINE_GET_LIST,Frecord.rec_id,(LPARAM)lst)==GKH_RET_SUCCESS)
    {
     for(DWORD i = 0;i<lst->count;++i)
     {
      combine_list_add(lst->obj_id[i]);
     }
    }
    delete [] buffer;
    if(CombineList->Items->Count)
       CombineList->Items->Item[0]->Selected = true;
  }


}


void       __fastcall TRecordFrame::setup_combine_param()
{
  wchar_t buffer[2048];
  LPMDB_RECORD_TEXT rt = (LPMDB_RECORD_TEXT)buffer;
  rt->dw_size  = sizeof(buffer);
  rt->text_size = (sizeof(buffer)-sizeof(*rt))/sizeof(wchar_t);
  rt->text_len  = 0;
  rt->text[0]   = 0;
  mod_iface.call(MDBCM_COMBINE_GET_FORMULA,Frecord.rec_id,(LPARAM)rt);
  Formula->Text = rt->text;
  setup_combine_list();
}
void __fastcall TRecordFrame::CombineListSelectItem(TObject *Sender,
      TListItem *Item, bool Selected)
{
  if(Selected)
    {
     CombObjId->Text = Item->Caption;
     CombObjRemove->Enabled = true;
    }
    else
    {
     CombObjRemove->Enabled = false;
    }
}
//---------------------------------------------------------------------------

void __fastcall TRecordFrame::CombineListDragOver(TObject *Sender,
      TObject *Source, int X, int Y, TDragState State, bool &Accept)
{
    AnsiString str = Source->ClassName();
    TControl * ctrl = dynamic_cast<TControl*>(Source);
    Accept = ctrl && ctrl->Name == "MdbTree";

}
//---------------------------------------------------------------------------

void __fastcall TRecordFrame::CombineListDragDrop(TObject *Sender,
      TObject *Source, int X, int Y)
{
  TTreeView * tv = dynamic_cast<TTreeView*>(Source);

  if(tv)
    {
     TTreeNode * node = tv->Selected;
     if(node)
      combine_list_add((DWORD)node->Data);
      //combine_list_add(frm->DragRecordId);
    }
}
//---------------------------------------------------------------------------



DWORD __fastcall TRecordFrame::Getapply_mask()
{
     DWORD ret = Fapply_mask;
           TMediumDBFrm * frm = dynamic_cast<TMediumDBFrm *>(Owner);
           if(frm)
           ret = frm->apply_mask;
     return ret;
}

void  __fastcall TRecordFrame::Setapply_mask(DWORD value)
{
        //TODO: Add your source code here
        if(apply_mask!=value)
        {
         TMediumDBFrm * frm = dynamic_cast<TMediumDBFrm *>(Owner);
         if(frm)
         frm->apply_mask = value;
         else
         Fapply_mask = value;

        }

}
void __fastcall TRecordFrame::OtdAddrChange(TObject *Sender)
{
  apply_mask|=MDBR_FIELD_ADDR;
  sotd_param_addr addr = make_record_addr();
  if(addr.param_number != DWORD(-1)  )
     StateDiscreteValue->Checked = addr.addr.fa ? false : true;
     else
     {
      if(this->record.options & MDBR_OPT_COMBINED)
         StateDiscreteValue->Enabled = true;
         else
         {
           StateDiscreteValue->Checked = true;
           StateDiscreteValue->Enabled = false;
         }
     }

}
//---------------------------------------------------------------------------

void __fastcall TRecordFrame::ShortNameChange(TObject *Sender)
{
 apply_mask|=MDBR_FIELD_SNAME;
}
//---------------------------------------------------------------------------

void __fastcall TRecordFrame::FullNameChange(TObject *Sender)
{
 apply_mask|=MDBR_FIELD_NAME;
}
//---------------------------------------------------------------------------

void __fastcall TRecordFrame::FormulaChange(TObject *Sender)
{
  apply_mask|=MDBR_COMBINE_FORMULA;
}
//---------------------------------------------------------------------------

void __fastcall TRecordFrame::CombineListDeletion(TObject *Sender,
      TListItem *Item)
{
 apply_mask|=MDBR_COMBINE_LIST;
}
//---------------------------------------------------------------------------

void __fastcall TRecordFrame::CombineListInsert(TObject *Sender,
      TListItem *Item)
{
 apply_mask|=MDBR_COMBINE_LIST;
}
//---------------------------------------------------------------------------

sotd_param_addr __fastcall  TRecordFrame::make_record_addr()
{
 sotd_param_addr addr;
 AnsiString str  = OtdAddr->Text ;
 if(str.Length() && isdigit(*str.c_str()))
 {
  str2addr(str.c_str(),&addr.addr.pu,sizeof(addr.addr),'.');
  char * param_number = strchr(str.c_str(),'-');
  if(param_number)
    {
     str = AnsiString(++param_number).Trim();
     if(str.Length())
        addr.param_number = atoi(str.c_str());
    }
 }
 return addr;
}


DWORD __fastcall TRecordFrame::apply_changes(DWORD mask)
{
 DWORD ret = -1;
 UnicodeString str;
  if(mask&MDBR_FIELD_ADDR)
     Frecord.addr = make_record_addr();
  if(mask&(MDBR_FIELD_MAX_ALARM|MDBR_FIELD_MIN_ALARM|MDBR_FIELD_MAX_DANGER|MDBR_FIELD_MIN_DANGER|MDBR_FIELD_OPTIONS) )
  {
       Frecord.min_danger  = get_min_danger();
       Frecord.max_danger  = get_max_danger();
       Frecord.min_alarm   = get_min_alarm ();
       Frecord.max_alarm   = get_max_alarm ();
       if(!Frecord.is_discrete())
       {
         OptDanger->Checked = !(IsInfinite(Frecord.min_danger) && IsInfinite(Frecord.max_danger));
         OptAlarm ->Checked = !(IsInfinite(Frecord.min_alarm ) && IsInfinite(Frecord.max_alarm ));
       }
   }

  adjust_alarms_dangers();

  if(mask & MDBR_FIELD_PERCENT)
    {
     #ifdef __CODEGEARC__
     Frecord.percent = (DWORD)(100.0*_wtof(Percent->Text.c_str()));
     #else
     Frecord.percent = (DWORD)(100.0*atof(Percent->Text.c_str()));
     #endif
    }

 if(mask & MDBR_FIELD_SCALEID)
  {
    if(GKH_RET_SUCCESS!=scale.call(CM_SC_SELECT_A,(LPARAM)ScaleId->Text.c_str(),(LPARAM)&Frecord.scale_id))
      Frecord.scale_id = -1;

  }

 if(mask & MDBR_FIELD_STATE)
    {
     Frecord.state &= MDBR_STATE_DYN_MASK;
     if(StateWorkOut->Checked)
       Frecord.state|=MDBR_STATE_WORKOUT;
     if(StateHasTuTr->Checked)
       Frecord.state|=MDBR_STATE_TUFLAG;
     if(StateDiscreteValue->Checked)
       Frecord.state|=MDBR_STATE_DISCRETE;
     if(StateDiscreteStatic->Checked)
       Frecord.state|=MDBR_STATE_STATIC;
     if(StateDiscreteHidden->Checked)
       Frecord.state|=MDBR_STATE_HIDDEN;
     if(StateDiscreteFixed ->Checked)
       Frecord.state|=MDBR_STATE_NORECEIPT;
     if(StateDiscreteNormal->Checked)
       Frecord.state|=MDBR_STATE_NORMAL   ;


     #ifdef _DEBUG
     Frecord.state &=~(MDBR_STATE_TUTR_PREPARE|MDBR_STATE_TUTR_ACTIVE|MDBR_STATE_TUTR_ERROR);
     if(StateDiscreteTU->Checked)
       Frecord.state|=MDBR_STATE_TUTR_PREPARE   ;
     if(StateDiscreteTUERR->Checked)
       Frecord.state|=MDBR_STATE_TUTR_ERROR   ;
     #endif
    }

 if(mask & MDBR_FIELD_OPTIONS)
    {
     Frecord.options&=MDBR_OPT_COMBINED;
	 if(OptDanger->Checked  ) Frecord.options |= MDBR_OPT_DANGER;
	 if(OptAlarm ->Checked  )
         {
          Frecord.options |= MDBR_OPT_ALARM ;
          if(StateDiscreteValue->Checked)
            {
             Frecord.min_alarm = Frecord.max_alarm =  optInvScale->Checked ? 1:0;
            }

         }
     if(optInvScale->Checked  ) Frecord.options |= MDBR_OPT_INVERSE_SCALE;
     if(optSigned->  Checked  ) Frecord.options |= MDBR_OPT_SIGNED_KVANTS;
     if(OptPseudo->  Checked  ) Frecord.options |= MDBR_OPT_PSEUDO;
     if(OptCaution-> Checked  ) Frecord.options |= MDBR_OPT_CAUTION;
     if(OptCombineCalcByLast->Checked )
       Frecord.options |= MDBR_OPT_COMBINE_CALC_BY_LAST;
       else
       Frecord.options &=~MDBR_OPT_COMBINE_CALC_BY_LAST;

     Frecord.options   |= MDBR_OPT_CONST_MINMAX;
    }


  if(mask & (MDBR_FIELD_KVANTS|MDBR_FIELD_MIN_VALUE|MDBR_FIELD_MAX_VALUE))
   {
   float scale_begin,scale_end;
   DWORD kvants;
   scale_begin =    get_min_value();
   scale_end   =    get_max_value();

   #ifdef __CODEGEARC__
    kvants  = (DWORD)_wtoi(this->Kvants->Text.c_str());
    #else
    kvants  = (DWORD)atoi(this->Kvants->Text.c_str());
    #endif
    Frecord.set_scale(scale_begin,scale_end,kvants);
   }


 if(create_mode)
 {
  //Создание нового объекта
  Frecord.rec_id = mod_iface.call(MDBCM_RECORD_CREATE,(LPARAM)&Frecord,0);
  ret    = Frecord.rec_id;
  if(ret == (DWORD)-1)
   {
    return ret;
   }
 }
 else
  ret = mod_iface.call(MDBCM_RECORD_UPDATE,(LPARAM)&Frecord,apply_mask);


 if(mask&MDBR_FIELD_SNAME)
   {
    str = ShortName->Text;
    DWORD n_id = set_record_name(record.rec_id,str,true);
    if(Frecord.sname != n_id)
      Frecord.sname = n_id;
      else
      mask &=~MDBR_FIELD_SNAME;
   }

 if(mask&MDBR_FIELD_NAME)
   {
    str = FullName->Text;
    DWORD n_id = set_record_name(record.rec_id,str,false);
    if(Frecord.name != n_id)
      Frecord.name = n_id;
      else
      mask &=~MDBR_FIELD_NAME;
   }

 if(mask&MDBR_COMBINE_FORMULA)
   {
     wchar_t buffer[2048];
     LPMDB_RECORD_TEXT rt = (LPMDB_RECORD_TEXT)buffer;
     rt->dw_size  = sizeof(buffer);
     rt->text_size = (sizeof(buffer)-sizeof(*rt))/sizeof(wchar_t);
     rt->text_len  = Formula->Text.Length();
     rt->text[0]   = 0;
     #ifdef __CODEGEARC__
     _wcspcpy(rt->text,Formula->Text.c_str());
     #else
     Ansi2Unicode(rt->text,Formula->Text.c_str());
     #endif
     mod_iface.call(MDBCM_COMBINE_SET_FORMULA,Frecord.rec_id,(LPARAM)rt);
   }

 if(mask&MDBR_COMBINE_LIST)
 {
    DWORD obj_count = CombineList->Items->Count;
    DWORD size = sizeof(MDB_COMBINE_LIST)+sizeof(DWORD)*obj_count;
    LPBYTE buffer = new BYTE[size];
    LPMDB_COMBINE_LIST lst = (LPMDB_COMBINE_LIST) buffer;
    lst->dw_size  = size;
    lst->capacity = obj_count+1;
    lst->start    = 0;
    lst->count    = obj_count;
     for(DWORD i = 0;i<obj_count;++i)
     {
      TListItem * item = CombineList->Items->Item[i];
      lst->obj_id[i] = (DWORD)item->Data;
     }
     mod_iface.call(MDBCM_COMBINE_SET_LIST,Frecord.rec_id,(LPARAM)lst);
    delete [] buffer;
 }
 return  ret;
}

DWORD __fastcall TRecordFrame::undo_changes (DWORD mask)
{
 apply_mask = 0;
 update_record(Frecord.rec_id, mask);
 return 0;
}


void __fastcall TRecordFrame::CombObjRemoveClick(TObject *Sender)
{
 if(CombineList->SelCount)
   {
    CombineList->Selected->Delete();
   } 
}
//---------------------------------------------------------------------------


void __fastcall TRecordFrame::CombObjIdChange(TObject *Sender)
{
  bool id_exist = CombObjId->Text.Length() ? true:false;
  DWORD rec_id  = _wtoi(CombObjId->Text.c_str());;
  if(id_exist)
     {
      mdb_record rec;
      id_exist = mod_iface.call(MDBCM_RECORD_GET,rec_id,(LPARAM)&rec) == GKH_RET_SUCCESS ? true:false;
     }
  CombObjAdd->Enabled = id_exist && (rec_id!= this->Frecord.rec_id);
  AnsiString str ;
  if(id_exist)
  {
    str = get_record_name(rec_id,false);
    if(!str.Length())
    str = get_record_name(rec_id,true);
  }
  CombEntryName->Caption = str;
}
//---------------------------------------------------------------------------

void __fastcall TRecordFrame::CombObjAddClick(TObject *Sender)
{
 #ifdef __CODEGEARC__
  DWORD comb_obj_id = _wtoi(CombObjId->Text.c_str());
 #else
  DWORD comb_obj_id = atoi(CombObjId->Text.c_str());
 #endif
  combine_list_add(comb_obj_id);
}
//---------------------------------------------------------------------------

void __fastcall TRecordFrame::CombineListCompare(TObject *Sender,
      TListItem *Item1, TListItem *Item2, int Data, int &Compare)
{
   Compare = (int)Item1->Data - (int) Item2->Data;
}
//---------------------------------------------------------------------------



bool __fastcall TRecordFrame::Getcreate_mode()
{
 TMediumDBFrm * frm = dynamic_cast<TMediumDBFrm *>(Owner);
 if(frm)
 {
  return  frm->create_mode;
 }
 return  false;
}
void __fastcall TRecordFrame::PercentChange(TObject *Sender)
{
 this->apply_mask|=MDBR_FIELD_PERCENT;
}
//---------------------------------------------------------------------------

void __fastcall TRecordFrame::CombineListResize(TObject *Sender)
{
   CombineList->Columns->Items[2]->Width = CombineList->Width - CombineList->Columns->Items[0]->Width- CombineList->Columns->Items[1]->Width - 16;
        
}
//---------------------------------------------------------------------------

void __fastcall TRecordFrame::StateWorkOutClick(TObject *Sender)
{
  apply_mask|=MDBR_FIELD_STATE;
  //OptAlarmClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TRecordFrame::ScaleIdChange(TObject *Sender)
{
  apply_mask |= MDBR_FIELD_SCALEID;
  bool rp = (ScaleId->Visible &&  ScaleId->Text.Length()) ? false:true;
  ScaleParam->Visible = rp;
}
//---------------------------------------------------------------------------


void __fastcall TRecordFrame::OptAlarmClick(TObject *Sender)
{
 apply_mask|=MDBR_FIELD_OPTIONS;
 DangerBox->Visible = OptDanger->Checked && !StateDiscreteValue->Checked;
 AlarmBox->Visible  = OptAlarm->Checked  && !StateDiscreteValue->Checked;
 TCheckBox * cb = dynamic_cast<TCheckBox * >(Sender);
 if(cb )
 {
   if(cb->Checked)
   {
    StateDiscreteFixed->Checked  = true;
    StateDiscreteNormal->Checked = false;
   }
   StateDiscreteFixed->Enabled = !cb->Checked;
   StateDiscreteNormal->Enabled = !cb->Checked;
 }

}
//---------------------------------------------------------------------------

void __fastcall TRecordFrame::ValueMinChange(TObject *Sender)
{
  //Вычисление шкалы
  #ifdef __CODEGEARC__
  DWORD kvants = _wtoi(Kvants->Text.c_str());
  #else
  DWORD kvants = atoi(Kvants->Text.c_str());
  #endif
  if(!setup_active)
  {
  float min_alarm,max_alarm;
  float min_danger,max_danger;
  #ifdef __CODEGEARC__
   float scale_begin  = _wtof(ValueMin->Text.c_str());
   float scale_end    = _wtof(ValueMax->Text.c_str());
  #else
   float scale_begin =  atof(ValueMin->Text.c_str());
   float scale_end    = atof(ValueMax->Text.c_str());
  #endif
	  if(kvants!=Frecord.kvants)
		 {
		  apply_mask|=MDBR_FIELD_KVANTS;
		 }
      if(optSigned->Checked)
      {
        scale_end = fabs(scale_end);
        scale_begin = -scale_end;
        setup_active = TRUE;
        set_float(ValueMin,scale_begin);
        setup_active = FALSE;

      }
      if(scale_begin!=Frecord.min_value)
         apply_mask|=MDBR_FIELD_MIN_VALUE;
      if(scale_end!=Frecord.max_value)
         apply_mask|=MDBR_FIELD_MAX_VALUE;
   Frecord.set_scale(scale_begin,scale_end,kvants);
   start_adjust_timer();
  }
}
//---------------------------------------------------------------------------

void __fastcall TRecordFrame::ScaleIdDropDown(TObject *Sender)
{
  ScaleId->Items->Clear();
  DWORD   index = 0;
  SCALE_ENUM scen;
  scen.dw_size = sizeof(scen);
  while((DWORD)scale.call(CM_SC_ENUM,index++,(LPARAM)&scen)<(DWORD)GKH_RET_EXCEPTION)
  {
    ScaleId->Items->Add(AnsiString(scen.name));
    scen.dw_size = sizeof(scen);
  }
  ScaleId->DropDownCount = KeRTL::MAX(1,ScaleId->Items->Count);
}
//---------------------------------------------------------------------------


void __fastcall TRecordFrame::MaxDangerChange(TObject *Sender)
{
  if(!setup_active)
  {
   apply_mask|=MDBR_FIELD_MAX_DANGER;
   start_adjust_timer();
  }
}
//---------------------------------------------------------------------------

void __fastcall TRecordFrame::MinDangerChange(TObject *Sender)
{
  if(!setup_active)
  {
   apply_mask|= MDBR_FIELD_MIN_DANGER;
   start_adjust_timer();
  }
}
//---------------------------------------------------------------------------

void __fastcall TRecordFrame::StateDiscreteStaticClick(TObject *Sender)
{
  apply_mask|= MDBR_FIELD_STATE;
}
//---------------------------------------------------------------------------


void __fastcall TRecordFrame::MinAlarmChange(TObject *Sender)
{
  if(!setup_active)
  {
   apply_mask|=MDBR_FIELD_MIN_ALARM;
   start_adjust_timer();
  }

}
//---------------------------------------------------------------------------

void __fastcall TRecordFrame::MaxAlarmChange(TObject *Sender)
{
  if(!setup_active)
  {
   apply_mask|=MDBR_FIELD_MAX_ALARM;
   start_adjust_timer();
  }
}
//---------------------------------------------------------------------------



 void  __fastcall  TRecordFrame::adjust_alarms_dangers()
 {
  float min_v     =  get_min_value();
  float max_v     =  get_max_value();
  float min_a     =  get_min_alarm();
  float max_a     =  get_max_alarm();
  float min_d     =  get_min_danger();
  float max_d     =  get_max_danger();

  if(!IsInfinite(max_a) && !IsInfinite(min_a) && max_a<min_a)
        std::swap(min_a,max_a);
   if(!IsInfinite(max_d) && !IsInfinite(min_d) && max_d<min_d)
        std::swap(min_d,max_d);

   if(!OptAlarm->Checked)
   {
     min_a = std::numeric_limits<float>::infinity();
     max_a = std::numeric_limits<float>::infinity();
   }

   if(!OptDanger->Checked)
   {
     min_d = std::numeric_limits<float>::infinity();
     max_d = std::numeric_limits<float>::infinity();
   }

   if(!IsInfinite(min_d) && !IsInfinite(min_a) && min_d < min_a)
    std::swap(min_d,min_a);
   if(!IsInfinite(max_d) && !IsInfinite(max_a) && max_d > max_a)
       std::swap(max_d,max_a);

   ++setup_active;
   set_min_danger(min_d);
   set_max_danger(max_d);
   set_min_alarm (min_a);
   set_max_alarm (max_a);
   --setup_active;
 }

 float __fastcall  TRecordFrame::get_min_danger()
 {
   return get_float(OptDanger->Checked ? MinDanger: NULL);
 }

 float __fastcall  TRecordFrame::get_max_danger()
 {
   return get_float(OptDanger->Checked ? MaxDanger: NULL);
 }

 float __fastcall  TRecordFrame::get_min_alarm ()
 {
     return get_float(OptAlarm->Checked ? MinAlarm : NULL);
 }

 float __fastcall  TRecordFrame::get_max_alarm ()
 {
     return get_float(OptAlarm->Checked ? MaxAlarm : NULL);
 }

 float __fastcall  TRecordFrame::get_min_value ()
 {
     return
       #ifdef __CODEGEARC__
        _wtof(ValueMin->Text.c_str());
       #else
        atof(ValueMin->Text.c_str());
       #endif
 }

 float __fastcall  TRecordFrame::get_max_value ()
 {
     return
       #ifdef __CODEGEARC__
        _wtof(ValueMax->Text.c_str());
       #else
        atof(ValueMax->Text.c_str());
       #endif

 }

 void  __fastcall  TRecordFrame::set_min_danger(float val)
 {
   //if(CompareValue( get_min_danger(),val))
   {
      set_float(MinDanger,val)  ;
      apply_mask |= MDBR_FIELD_MIN_DANGER;
   }
 }

 void  __fastcall  TRecordFrame::set_max_danger(float val)
 {
   //if(CompareValue( get_max_danger(),val))
   {
      set_float(MaxDanger,val);
      apply_mask |= MDBR_FIELD_MAX_DANGER;
   }

 }

 void  __fastcall  TRecordFrame::set_min_alarm (float val)
 {
   //if(CompareValue( get_min_alarm(),val))
   {
      set_float(MinAlarm, val);
      apply_mask |= MDBR_FIELD_MIN_ALARM;
   }

 }

 void  __fastcall  TRecordFrame::set_max_alarm (float val)
 {
   //if(CompareValue( get_max_alarm(),val))
   {
      set_float(MaxAlarm, val);
      apply_mask |= MDBR_FIELD_MAX_ALARM;
   }
 }

 void  __fastcall  TRecordFrame::set_min_value (float val)
 {
   //if(CompareValue( get_min_value(),val))
      {
      apply_mask |= MDBR_FIELD_MIN_VALUE;
      ValueMin->Text = val;
      }

 }

 void  __fastcall  TRecordFrame::set_max_value (float val)
 {
   if(CompareValue( get_max_value(),val))
      {
       apply_mask |= MDBR_FIELD_MAX_VALUE;
       ValueMax->Text = val;
      }
 }


void __fastcall TRecordFrame::optInvScaleClick(TObject *Sender)
{
 apply_mask|=MDBR_FIELD_OPTIONS;
}
//---------------------------------------------------------------------------

void __fastcall TRecordFrame::StateDiscreteValueClick(TObject *Sender)
{
 apply_mask|=MDBR_FIELD_OPTIONS|MDBR_FIELD_STATE;
 RecordParam->Visible = StateDiscreteValue->Checked;
 OptPseudo->Visible   = RecordParam->Visible;
 OptCaution->Visible  = RecordParam->Visible;
}
//---------------------------------------------------------------------------

void __fastcall TRecordFrame::start_adjust_timer()
{
 adjust_timer->Enabled = false;
 adjust_timer->Enabled = true;
}

void __fastcall TRecordFrame::adjust_timerTimer(TObject *Sender)
{
  adjust_alarms_dangers();
  adjust_timer->Enabled = false;
}
//---------------------------------------------------------------------------

void __fastcall TRecordFrame::optSignedClick(TObject *Sender)
{
 apply_mask|=MDBR_FIELD_OPTIONS;
 if(optSigned->Checked)
    set_float(ValueMin,-get_max_value());
    ValueMin->Enabled = !optSigned->Checked;
}
//---------------------------------------------------------------------------

void __fastcall TRecordFrame::OptCombineCalcByLastClick(TObject *Sender)
{
  //
  apply_mask|=MDBR_FIELD_OPTIONS;
}
//---------------------------------------------------------------------------

void __fastcall TRecordFrame::OptPseudoClick(TObject *Sender)
{
 apply_mask|=MDBR_FIELD_OPTIONS;
}
//---------------------------------------------------------------------------


void __fastcall TRecordFrame::ShortNameCloseUp(TObject *Sender)
{
  TComboBox * cb  = dynamic_cast<TComboBox *>(Sender);
  if(cb)
     cb->Tag = cb->ItemIndex;
}
//---------------------------------------------------------------------------

void __fastcall TRecordFrame::get_mdb_strings(TStrings * strings)
{
  DWORD str_id  = 0;
  UnicodeString str;
  do{
     str = TMediumDBFrm::get_text(mod_iface,str_id++).TrimRight();
     if(!str.IsEmpty())
        {
         strings->Add(str);
        }
    }while(!str.IsEmpty());

}

void __fastcall TRecordFrame::ShortNameDropDown(TObject *Sender)
{
  TComboBox * cb  = dynamic_cast<TComboBox *>(Sender);
  if(cb)
    {
     cb->Items->Clear();
     get_mdb_strings(cb->Items);
    }
}
//---------------------------------------------------------------------------

void __fastcall TRecordFrame::ShortNameSelect(TObject *Sender)
{
  TComboBox * cb  = dynamic_cast<TComboBox *>(Sender);
  if(cb)
     {
      cb->Tag = cb->ItemIndex;
//      if(cb == ShortName)
//         apply_mask|=MDBR_FIELD_SNAME;
//         else
//         apply_mask|=MDBR_FIELD_NAME;
     }

}
//---------------------------------------------------------------------------

void __fastcall TRecordFrame::OptCautionClick(TObject *Sender)
{
 OptPseudoClick(this);
 bool caution = OptCaution->Checked;
 if(caution)
 {
     OptDanger->Checked = false;
     OptAlarm->Checked  = false;
     OptPseudo->Checked = true;
     StateDiscreteStatic->Checked = true;
     StateDiscreteHidden->Checked = true;
     StateDiscreteFixed->Checked  = true;
     StateDiscreteNormal->Checked = false;
 }

 OptDanger->Enabled = !caution;
 OptAlarm->Enabled  = !caution;
 OptPseudo->Enabled = !caution;
 StateDiscreteStatic->Enabled = !caution;
 StateDiscreteHidden->Enabled = !caution;
 StateDiscreteFixed->Enabled  = !caution;
 StateDiscreteNormal->Enabled = !caution;

}
//---------------------------------------------------------------------------

