//---------------------------------------------------------------------------

#pragma hdrstop
#include <vcl.h>

#include "modbus_group_frame.h"
#include "modbus_mdm_form.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "gkmod_frame"
#pragma resource "*.dfm"
TModbusGroupFrm *ModbusGroupFrm;
//---------------------------------------------------------------------------
__fastcall TModbusGroupFrm::TModbusGroupFrm(TComponent* Owner)
	: TGKModuleFrame(Owner)
{
 Fin_setup   = 0;
 Ftree_node  = NULL;
 Fon_changed = NULL;

}
//---------------------------------------------------------------------------
void __fastcall TModbusGroupFrm::set_ch_mask(DWORD mask)
{
    if(ch_mask!=mask)
    {
     Fch_mask=mask;
     if(on_changed) on_changed(this);
    }
}

LPMODBUS_GROUP  __fastcall TModbusGroupFrm::get_modbus_group()
{
  if(Ftree_node)
    return (LPMODBUS_GROUP)Ftree_node->Data;
  return (LPMODBUS_GROUP)NULL;
}

void __fastcall TModbusGroupFrm::begin_setup  ()
{
   ++Fin_setup;
}
void __fastcall TModbusGroupFrm::end_setup    ()
{
  Fin_setup = max(0,--Fin_setup);
}

 bool __fastcall TModbusGroupFrm::modbus_group_get(LPMODBUS_GROUP grp)
 {
  return this->mod_iface.call(MBCM_GET_GROUP,(LPARAM) grp,0) == GKH_RET_SUCCESS ? true:false;
 }

const wchar_t * TModbusGroupFrm::modbus_errors_text[] =
{
L"",
L"Ошибочная функция",//
L"Ошибка адреса данных",//MODBUS_ERR_ILLEGAL DATA_ADDRESS
L"Ошибочное значение данных",//MODBUS_ERR_ILLEGAL_DATA_VALUE
L"Сбой починенного устройства",//ODBUS_ERR_SLAVE_DEVICE_FAILURE
L"Ошибка подтверждения",//ODBUS_ERR_ACKNOWLEDGE
L"Подчиненное устройство занято",//ODBUS_ERR_SLAVE_DEVICE_BUSY
L"Отсутствие подтверждения",//ODBUS_ERR_NEGATIVE_ACKNOWLEDGE
L"Ошибка четности памяти",//ODBUS_ERR_MEMORY_PARITY_ERROR
L""

};

void __fastcall TModbusGroupFrm::setup_controls()
{
  begin_setup();
  MODBUS_GROUP &mg = *modbus_group;
  if(modbus_group)
  {

   bool is_new = is_new_group();
   GroupNum->Text    = is_new ? UnicodeString() :UnicodeString( mg.group_no );
   FirstParam->Text  = is_new ? 0 : mg.start_no;
   CountParam->Text  = is_new ? 8 : mg.count;
   ModbusCommand->Text = is_new ? UnicodeString() : UnicodeString(mg.command);
   RefreshTime->Text = is_new ? 1000 : mg.refresh_time;
   DevNo->Text       = is_new ? UnicodeString() : UnicodeString(mg.dev_no);
   RespondTime->Text = is_new ? 2000 : mg.respond_time;
   RealRespondTime->AutoSize  = false;
   RealRespondTime->Caption      = is_new ? UnicodeString(L"?")     : UnicodeString(mg.real_respond_time);
   RealRespondTimeMin->Caption   = is_new ? UnicodeString(L"?")  : UnicodeString(mg.real_respond_time_min);
   RealRespondTimeMax->Caption   = is_new ? UnicodeString(L"?")  : UnicodeString(mg.real_respond_time_max);
   if(is_new)
   {
     LastError->Caption = UnicodeString();
     ErrCount->Caption  = UnicodeString();
   }
   else
   {
     LastError->Caption =  modbus_errors_text[mg.last_error];
     ErrCount->Caption  =  mg.error_count;
   }

   update_otd_addr();
  }
  ch_mask = 0;
  end_setup  ();
}

void __fastcall TModbusGroupFrm::set_tree_node(TTreeNode * node)
{
 if(Ftree_node != node)
 {
   Ftree_node = node;
   modbus_group_get(get_modbus_group());
   setup_controls();
 }
}

void __fastcall TModbusGroupFrm::update_otd_addr()
{

   UnicodeString s;
   otd_addr addr;
   s.printf(L"ПУ.%d.%d.%d [%s]"
                         ,modbus_group->point_no
                         ,rg1->ItemIndex<2 ? OTD_FA_DISCRETE : OTD_FA_ANALOG
                         ,atoi(GroupNum->Text.c_str())
                         ,rg1->ItemIndex<2 ? L"Дискретный":L"Аналоговый"
   );
   this->otd_addr_text->Caption = s;
}

void __fastcall TModbusGroupFrm::GroupNumChange(TObject *Sender)
{

  if(!in_setup  )
  {
    TEdit * edit     = dynamic_cast<TEdit*>(Sender);
    TRadioGroup * rg = dynamic_cast<TRadioGroup *>(Sender);
    LPMODBUS_GROUP  mg = modbus_group;
    int tag = -1;
    if(edit)
       tag = edit->Tag;
    if(rg)
       tag = rg->Tag;

    switch(tag)
    {
     case 1:
      if(mg->group_no     != (WORD)atoi(edit->Text.c_str()))
      ch_mask |=edit->Tag;
      else
      ch_mask &=~edit->Tag;
     break;
     case 2:
      if(mg->start_no     != (WORD)atoi(edit->Text.c_str()))
      ch_mask |=edit->Tag;
      else
      ch_mask &=~edit->Tag;
     break;
     case 4:
      if(mg->count        != (WORD)atoi(edit->Text.c_str()))
      ch_mask |=edit->Tag;
      else
      ch_mask &=~edit->Tag;
     break;
     case 8:
      if(mg->refresh_time != (DWORD)atoi(edit->Text.c_str()))
      ch_mask |=edit->Tag;
      else
      ch_mask &=~edit->Tag;
     break;
     case 0x10:

     if(mg->command != (WORD)atoi(ModbusCommand->Text.c_str()))
      ch_mask |=tag;
      else
      ch_mask &=~tag;

     break;
     case 0x20:
     if(mg->dev_no != (WORD)atoi(edit->Text.c_str()))
      ch_mask |=tag;
      else
      ch_mask &=~tag;
      break;
     case 0x40:
     if(mg->respond_time != (DWORD)atoi(edit->Text.c_str()))
      ch_mask |=tag;
      else
      ch_mask &=~tag;
      break;

    }
    update_otd_addr();
  }
}
//---------------------------------------------------------------------------
const wchar_t * TModbusGroupFrm::group_names[] =
{
 L"?",
 L"Группа № %d Дискретный выход ",
 L"Группа № %d Дискретный вход  ",
 L"Группа № %d Holding register ",
 L"Группа № %d Input register   ",
 L"Задано пользователем %d"
};

bool __fastcall TModbusGroupFrm::apply_changes ()
{
  bool ret = true;
  if(ModbusCommand->Text.Length())
  {
    MODBUS_GROUP mg ;
    mg.point_no = modbus_group->point_no;
    mg.group_no = atoi(GroupNum->Text.c_str());
    mg.command   = atoi(ModbusCommand->Text.c_str());
    mg.dev_no   = atoi(DevNo->Text.c_str());
    mg.start_no = atoi(FirstParam->Text.c_str());
    mg.count    = atoi(CountParam->Text.c_str());
    mg.refresh_time = atoi(RefreshTime->Text.c_str());
    mg.respond_time = atoi(RespondTime->Text.c_str());
    DWORD cmd = MBCM_UPDATE_GROUP ;
    if(is_new_group() )
       cmd = MBCM_CREATE_GROUP ;
    else
    {
     if(modbus_group->group_no != mg.group_no || mg.command != modbus_group->command)
     {
      //Смена номера группы
      if(mod_iface.call(MBCM_GET_GROUP,(LPARAM)&mg,0))
        {
          //Такой группы нет, создаем новую
          cmd = MBCM_CREATE_GROUP;
        }
        else
        {
         ret = false;
         SetLastError(MBERR_GROUP_ALREADY_EXIST);
        }
      }
    }

    if(ret && mod_iface.call(cmd,LPARAM(&mg),0))
       ret = false;
    else
    {
     if(modbus_group->group_no != mg.group_no ||  mg.command != modbus_group->command) // Новая группа создана
       {
         mod_iface.call(MBCM_DELETE_GROUP,(LPARAM)&modbus_group,0);
       }

	 LPMODBUS_GROUP  grp = modbus_group;
	 if(grp)
	    memcpy(grp,&mg,sizeof(mg));
     Ftree_node->Text = get_group_name(&mg);
     ch_mask = 0;
    }
   }
   else
    {
     MessageBox(Handle, _T("Не указана тип группы"),_T("Ошибка"),MB_ICONSTOP);
     ret = false;
    }
    return ret;
}

bool __fastcall TModbusGroupFrm::undo_changes  ()
{
    bool ret = true;
    if(is_new_group())
      ret = delete_current();
      else
      setup_controls();
    if(ret ) ch_mask = 0;
    return ret;
}

bool __fastcall TModbusGroupFrm::delete_current()
{
    bool ret = true;
    if(!is_new_group())
      ret = mod_iface.call(MBCM_DELETE_GROUP,(LPARAM)&modbus_group,0) ? false : true ;
    if(ret)
       Ftree_node->Delete();
    ch_mask = 0;
    return ret;
}

UnicodeString  __fastcall TModbusGroupFrm::get_group_name(LPMODBUS_GROUP grp)
{
 UnicodeString s;

 s.printf(TModbusGroupFrm::group_names[min((WORD)5,grp->command) ],grp->group_no);
 return s;
}


void __fastcall TModbusGroupFrm::bGetCommEventClick(TObject *Sender)
{
  UnicodeString text = "-";
  CommState->Caption = text;
  CommEvent->Caption = text;

  if(mod_iface.call(MBCM_SERVICE_FETCH_COMMEVENT_CTRL,this->modbus_group->point_no,modbus_group->dev_no)==GKH_RET_SUCCESS)
  {
     TModbusModemForm * mod_form = dynamic_cast<TModbusModemForm *>(Owner);
     if(mod_form)
       {
          TModbusModemForm * mod_form = dynamic_cast<TModbusModemForm *>(Owner);
          if(mod_form)
             mod_form->mon_func = monitor_txrx;

       }

  }

}
//---------------------------------------------------------------------------

struct mb_event_ctrl_log
{
 BYTE bytes;
 MODBUS_SHORT state;
 MODBUS_SHORT evt_count;
 MODBUS_SHORT msg_count;
 BYTE   events[ANYSIZE_ARRAY];
};

void __fastcall TModbusGroupFrm::monitor_txrx(LPVOID data,DWORD sz,DWORD code)
{
 LPMODBUS_FRAME mf = NULL;
 if(code == MBNC_RXTCP)
 {
   LPMODBUS_TCP_FRAME mft = (LPMODBUS_TCP_FRAME) data;
   mf = &mft->modbus_frame;
 }

 if(code == MBNC_TX)
   mf = (LPMODBUS_FRAME)data;        //modbus_proto.h

 if(mf )
 {
  bool done = false;
  if(mf->data.command ==  MODBUS_FETCH_COMMEVENT_CTRL)
  {
   LPMODBUS_SHORT val =  (LPMODBUS_SHORT)mf->data.data;
   CommState->Caption = modbus_word(val->val);
   ++val;
   CommEvent->Caption = modbus_word(val->val);
   //if(mod_iface.call(MBCM_SERVICE_FETCH_COMMEVENT_LOG,this->modbus_group.point_no,modbus_group.dev_no))
      done = true;
  }

  if(mf->data.command ==  MODBUS_FETCH_COMMEVENT_LOG)
  {
//   mb_event_ctrl_log * ecl  = (mb_event_ctrl_log *)mf->data.data;
//   LogCommState->Caption = modbus_word(ecl->state.val);
//   LogCommEvent->Caption = modbus_word(ecl->evt_count.val);
//   LogMsgCount->Caption  = modbus_word(ecl->msg_count.val);
//   LPBYTE  events     =  ecl->events;
//   DWORD    evt_count =  ecl->bytes - sizeof(mb_event_ctrl_log)+1;
//   AnsiString bs;
//   while(evt_count)
//   {
//    bs.cat_printf("0x%02X ",(DWORD)*events);
//    --evt_count;
//    events++;
//   }
//   events_text->Caption = bs;

   done = true;
  }

  if(done)
  {
          TModbusModemForm * mod_form = dynamic_cast<TModbusModemForm *>(Owner);
          if(mod_form)
             mod_form->mon_func = NULL;
  }

 }

}


void __fastcall TModbusGroupFrm::rg1Click(TObject *Sender)
{
  if(rg1->ItemIndex<4)
     {
      ModbusCommand->Text    = rg1->ItemIndex+1;
      ModbusCommand->Visible = false;
     }
     else
     {
      ModbusCommand->Text = UnicodeString();
      ModbusCommand->Visible = true;
     }
}
//---------------------------------------------------------------------------

void __fastcall TModbusGroupFrm::ModbusCommandChange(TObject *Sender)
{
  DWORD cmd = atoi(ModbusCommand->Text.c_str());
  if(!cmd) this->rg1->ItemIndex = -1;
  else
  {
   rg1->ItemIndex = min(DWORD(4),cmd-1);
  }
  this->GroupNumChange(Sender);
}
//---------------------------------------------------------------------------

