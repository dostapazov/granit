//---------------------------------------------------------------------------

#pragma hdrstop
#include <vcl.h>

#include "modbus_mdm_form.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "modbus_mon_frame"
#pragma link "gkmodule_form"
#pragma link "gkmod_frame"
#pragma link "mdbus_mod_cfg_frame"
#pragma link "modbus_device_frame"
#pragma link "modbus_group_frame"
#pragma link "about_common_frame"
#pragma resource "*.dfm"

TModbusModemForm *ModbusModemForm;
//---------------------------------------------------------------------------
__fastcall TModbusModemForm::TModbusModemForm(TComponent* Owner,HWND _hwnd,GKHANDLE _gkh)
        : TGKModuleForm(Owner,_hwnd,_gkh)
{
 Fcurrent_frame = NULL;
 mon_func       = NULL;
 ModbusModConfig1->on_changed = on_config_change;
 ModbusDevFrame1->on_changed  = on_config_change;
 ModbusGroupFrm1->on_changed  = on_config_change;
 MonFrame1 = NULL;

}
//---------------------------------------------------------------------------

void __fastcall TModbusModemForm::show_error(AnsiString cmd, DWORD err)
{
 if(err)
 {
  wchar_t err_text[MAX_PATH<<1];
  err_text[0] = 0;
  mod_iface.get_error_text(err,err_text,KERTL_ARRAY_COUNT(err_text));
  if(err_text[0])
     {
     cmd+=" : ";cmd += err_text;
     }
     else
     cmd.cat_printf(" : Завершено с ошибкой %X",err);
 }
 else
 {
  if(cmd.Length())
     cmd+=" выполнено успешно";
 }
 StatusBar1->Panels->Items[1]->Text = cmd;
}


void     __fastcall TModbusModemForm::on_module_state(GKHANDLE mod,DWORD state)
{

  bool running = ((state & MODULE_STATE_RUNNING_MASK )== MODULE_STATE_RUNNING) ? true:false;
  bool stopped  = (state & MODULE_STATE_RUNNING_MASK ) ? false:true;
  StartBtn->Enabled = stopped;
  StopBtn->Enabled = running;
  AnsiString str;
  if(stopped)
     str  =  "Остановлен";
  if(running)
     str  =  "Работает";
   StatusBar1->Panels->Items[0]->Text = str;
}

void     __fastcall TModbusModemForm::on_module_config_change(GKHANDLE mod,LPMODULE_CONFIG_DATA mcd)
{
  //Обработка изменения настроек
}


LRESULT  __fastcall TModbusModemForm::on_gkhandle_notify (GKHANDLE from,LPNOTIFY_CODE nc,DWORD mask)
{
 LRESULT ret = GKH_RET_SUCCESS;
   switch(mask)
   {

    case MBNF_TXRX:
         if(mon_func)
          mon_func(nc->notify_data,NOTIFY_DATA_SIZE(nc),nc->notify_code );
    break;
    default:
      ret = TGKModuleForm::on_gkhandle_notify(from,nc,mask);
      break;
   }
 return ret;
}

void     __fastcall TModbusModemForm::after_set_gkhandle()
{
  TGKModuleForm::after_set_gkhandle();
  on_module_state(mod_handle,mod_iface.get_module_state());
  set_notify(mod_iface(),MNF_COMMON|MNF_MODEM);
  setup_dev_tree();
}



void __fastcall TModbusModemForm::StartBtnClick(TObject *Sender)
{
  mod_iface.start(MODULE_START_REQUEST,0);
}
//---------------------------------------------------------------------------

void __fastcall TModbusModemForm::StopBtnClick(TObject *Sender)
{
 mod_iface.stop(MODULE_STOP_REQUEST);

}
//---------------------------------------------------------------------------

void __fastcall TModbusModemForm::Splitter1Moved(TObject *Sender)
{
  StatusBar1->Panels->Items[0]->Width = TreeView1->Width;
}
//---------------------------------------------------------------------------

void     __fastcall TModbusModemForm::setup_dev_tree()
{
    TreeView1->Items->Clear();
    TTreeNode * node = TreeView1->Items->Add(NULL,"MODBUS");
    TreeView1Change(this,node);
    MODBUS_POINT mdev;
    mdev.dw_size = sizeof(mdev);
    
    int i = 0;
    while(GKH_RET_SUCCESS ==  mod_iface.call(MBCM_ENUM_POINT,(LPARAM)&mdev,i))
    {
     TTreeNode * dev_node = TreeView1->Items->AddChild(node,mdev.point_name);
     dev_node->Data = new MODBUS_POINT(mdev);
     scan_groups(dev_node);
     ++i;
    }
    if(node->Count)
       node->Expand(false);
}


void     __fastcall TModbusModemForm::scan_groups(TTreeNode * device)
{
    LPMODBUS_POINT dev = (LPMODBUS_POINT)device->Data;
    MODBUS_GROUP    grp;
    grp.point_no      = dev->point_no;
    DWORD i         = 0;
    while(GKH_RET_SUCCESS == mod_iface.call(MBCM_ENUM_GROUP,(LPARAM)&grp,i))
    {


       TTreeNode * node   = device->Owner->AddChild(device,TModbusGroupFrm::get_group_name(&grp));
       node->Data = new MODBUS_GROUP(grp);
       ++i;
    }
}

void     __fastcall TModbusModemForm::set_current_frame(TGKModuleFrame * frame)
{
  if(Fcurrent_frame !=frame)
  {
    if(Fcurrent_frame)
       Fcurrent_frame->Visible = false;

    Fcurrent_frame =frame;

    if(Fcurrent_frame)
       {
        Fcurrent_frame->Visible    = true;
        Fcurrent_frame->mod_handle = mod_handle;
       }
  }
}



void __fastcall TModbusModemForm::TreeView1Change(TObject *Sender,
      TTreeNode *Node)
{
  if(Node)
  {
    LPDWORD dw_size = (LPDWORD)Node->Data;
    if(!dw_size)
       {
        current_frame = ModbusModConfig1;
        tbDel->Enabled = false;
       }
       else
       {
        tbDel->Enabled = true;
        if(*dw_size == sizeof(MODBUS_POINT))
        {
         current_frame = ModbusDevFrame1;
         ModbusDevFrame1->tree_node = Node;
        }
        else
        {
         current_frame = ModbusGroupFrm1;
         ModbusGroupFrm1->tree_node = Node;
        }
       }
     current_frame->Align = alClient;
  }
  else
  current_frame = NULL;
}
//---------------------------------------------------------------------------
void     __fastcall TModbusModemForm::on_config_change(TObject * sender)
{
 DWORD ch_mask = 0;
 if(sender == current_frame)
 {
  if(sender == ModbusModConfig1)
     ch_mask = ModbusModConfig1->ch_mask; 
  if(sender == this->ModbusDevFrame1)
     ch_mask = ModbusDevFrame1->ch_mask;
  if(sender == this->ModbusGroupFrm1)
     ch_mask = ModbusGroupFrm1->ch_mask;
 }

  ApplyBtn->Enabled = ch_mask ? true:false;
  UndoBtn->Enabled = ApplyBtn->Enabled;
}




void __fastcall TModbusModemForm::ApplyBtnClick(TObject *Sender)
{
 if(current_frame )
   {
    AnsiString cmd = current_frame == ModbusDevFrame1 ? (ModbusDevFrame1->is_new_point() ? "Создание":"Изменения") : "Переметры модуля";
    DWORD error = current_frame->apply_changes() ? 0 : GetLastError();
    show_error("Изменения", error);
    if(!error)
    {
      TreeView1->CustomSort(NULL,0,false);
    }
   }
}
//---------------------------------------------------------------------------

void __fastcall TModbusModemForm::UndoBtnClick(TObject *Sender)
{
 if(current_frame )
    {
       DWORD error = 0;
       AnsiString cmd = "Отмена изменений";
       if(!current_frame->undo_changes())
          error = GetLastError();
       show_error(cmd,error);
    }
}
//---------------------------------------------------------------------------


void __fastcall TModbusModemForm::tbNewClick(TObject *Sender)
{
  TTreeNode * pnode = TreeView1->Selected;
  UnicodeString s;
  TTreeNode * node;
  LPDWORD  dw_size = (LPDWORD)pnode->Data;

  if(dw_size)
  {
   if(*dw_size == sizeof(MODBUS_GROUP))
      pnode = pnode->Parent;
   node = TreeView1->Items->AddChild(pnode,s);
   LPMODBUS_POINT  mdev = (LPMODBUS_POINT)pnode->Data;
   LPMODBUS_GROUP mg = new MODBUS_GROUP;
   mg->point_no = mdev->point_no;
   node->Data = (void*)mg;
   s = L"Новaя группа";
  }
  else
  {

   node = TreeView1->Items->AddChild(pnode,s);
   node->Data = (void*) new MODBUS_POINT;
   s = L"Новое КП";
  }
  node->Text     = s;
  node->Selected = true;
  UndoBtn->Enabled = true;
  pgc1->ActivePage = TabSheet1;
}
//---------------------------------------------------------------------------


void __fastcall TModbusModemForm::FormHide(TObject *Sender)
{
  reset_notify(mod_iface(),MNF_COMMON|MNF_MODEM|MBNF_TXRX);
}
//---------------------------------------------------------------------------


void __fastcall TModbusModemForm::FormShow(TObject *Sender)
{
  set_notify(mod_iface(),MNF_COMMON|MNF_MODEM);
}
//---------------------------------------------------------------------------

TTreeNode * __fastcall TModbusModemForm::get_nearest_node(TTreeNode * curr)
{
  if(curr)
  {
    TTreeNode * node = curr->getNextSibling();
    if(!node)
       node = curr->getPrevSibling();
    if(!node)
       node = curr->Parent;
   return node;
  }
  return TreeView1->Items->GetFirstNode();

}

void __fastcall TModbusModemForm::tbDelClick(TObject *Sender)
{
  //Удаление устройства

  AnsiString cmd;
  DWORD error = 0;
  TTreeNode * node = get_nearest_node(ModbusDevFrame1->tree_node);

  if(current_frame )
  {
   if(Application->MessageBox(L"Подтверждаете удаление элемента?",L"Удаление ;-(",MB_ICONQUESTION|MB_YESNO) == ID_YES)
   {
   cmd = "Удаление элемента";
   if(!current_frame->delete_current())
      error = GetLastError();
   }
   else
   return;
  }
  show_error(cmd,error);
  if(!error)
   {
    node->Selected = true;
    ApplyBtn->Enabled = false;
    UndoBtn->Enabled = false;
   }

}
//---------------------------------------------------------------------------
void __fastcall TModbusModemForm::free_node(TTreeNode * Node)
{
  if(Node && Node->Data)
  {
      LPDWORD dw_size = (LPDWORD)Node->Data;
      if(*dw_size == sizeof(MODBUS_POINT))
        delete (LPMODBUS_POINT)Node->Data;
      if(*dw_size == sizeof(MODBUS_GROUP))
        delete (LPMODBUS_GROUP)Node->Data;
  }

}

void __fastcall TModbusModemForm::TreeView1Deletion(TObject *Sender, TTreeNode *Node)
{
  free_node(Node);
}
//---------------------------------------------------------------------------

void __fastcall TModbusModemForm::TreeView1Compare(TObject *Sender, TTreeNode *Node1,
          TTreeNode *Node2, int Data, int &Compare)
{
  DWORD dw1 = *(LPDWORD)Node1->Data;
  DWORD dw2 = *(LPDWORD)Node2->Data;
  if(dw1 != dw2)
     Compare = dw1<dw2 ? -1:1;
     else
     {
      if(dw1 == sizeof(MODBUS_POINT))
        Compare = (int)((LPMODBUS_POINT)Node1->Data)->point_no - (int)((LPMODBUS_POINT)Node2->Data)->point_no;
      if(dw1 == sizeof(MODBUS_GROUP))
        {
         LPMODBUS_GROUP g1 = (LPMODBUS_GROUP)Node1->Data;
         LPMODBUS_GROUP g2 = (LPMODBUS_GROUP)Node2->Data;
         int fa1 = OTD_FA_FROM_MODBUS_CMD(g1->command);
         int fa2 = OTD_FA_FROM_MODBUS_CMD(g2->command);
         if( fa1 == fa2)
           Compare = (int)(g1)->group_no - (int)(g2)->group_no;
           else
           Compare = fa1-fa2;
        }
     }
}
//---------------------------------------------------------------------------

void __fastcall TModbusModemForm::pgc1Change(TObject *Sender)
{
  if(pgc1->ActivePage == MonSheet)
     {
       if(!MonFrame1)
         {
           MonFrame1 = new TMonFrame(this);
           MonSheet->InsertControl(MonFrame1);
           MonFrame1->Align = alClient;
         }
       mon_func = MonFrame1->monitor_txrx;
     }
}
//---------------------------------------------------------------------------

void __fastcall TModbusModemForm::pgc1Changing(TObject *Sender, bool &AllowChange)

{
  mon_func = NULL;


}
//---------------------------------------------------------------------------

void __fastcall TModbusModemForm::set_mon_func(Tmon_func mf)
{
    if(Fmon_func != mf)
    {
     Fmon_func = mf;
     if(mf)
        set_notify(mod_iface(),MBNF_TXRX);
        else
        reset_notify(mod_iface(),MBNF_TXRX);

    }

}





void __fastcall TModbusModemForm::FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)

{
  switch(Key)
  {
      case VK_RETURN:  if(Shift.Contains(ssCtrl) && ApplyBtn->Enabled)
                           {
                            ApplyBtnClick(this);
                            Key = 0;
                           }

                        break;
      case VK_INSERT:  if(tbNew->Enabled)
                         {
                          tbNewClick(this);
                          Key = 0;
                         }

                          break;
      case VK_DELETE:  if(Shift.Contains(ssCtrl) && tbDel->Enabled )
                       {
                          tbDelClick(this);
                          Key = 0;
                       }
                          break;
      case VK_BACK  : if(Shift.Contains(ssAlt) && UndoBtn->Enabled )
                      {
                           UndoBtnClick(this);
                           Key = 0;
                      }
                          break;
  }
}
//---------------------------------------------------------------------------

