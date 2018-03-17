//---------------------------------------------------------------------------
#pragma hdrstop
#include <vcl.h>

#include "vtdsetup.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "gkmodule_form"
#pragma link "gkmod_frame"
#pragma link "mod_config_frame"
#pragma link "vtd_line_cfg_frame"
#pragma link "MonFrame"
#pragma link "vtd_group_cfg_frame"
#pragma link "view_frame"
#pragma resource "*.dfm"
TVTDSetupFrm *VTDSetupFrm;
//---------------------------------------------------------------------------
__fastcall TVTDSetupFrm::TVTDSetupFrm(TComponent* Owner,HWND owner_hwnd,GKHANDLE handle )
        : TGKModuleForm(Owner,owner_hwnd,handle)
{
 VtdModConfig1->on_changed = on_changed;
 VtdLineConfig1->on_changed = on_changed;
 VtdGroupConfig1->on_changed = on_changed;
 #ifdef COMERCIAL_BUILD
 Label30->Caption = "ООО \"Компания промышленной автоматизации\"";
 #endif

}
//---------------------------------------------------------------------------

LRESULT  __fastcall TVTDSetupFrm::on_gkhandle_notify (GKHANDLE from,LPNOTIFY_CODE nc,DWORD mask)
{
  LRESULT ret = GKH_RET_SUCCESS;
  switch(mask)
  {
   
   case VTDNF_TXRX       : MonitorFrame1->monitor_vtd_events(nc->notify_code,nc->notify_data,nc->dw_size - sizeof(*nc)+sizeof(nc->notify_data[0]));
                           break;
   case VTDNF_DEV_CHANGED:
        on_device_change(nc->notify_code,LOWORD(nc->notify_data[0]),HIWORD(nc->notify_data[0]));
   break;
   case MNF_MODEM        :
        on_modem_change(nc->notify_code,nc->notify_data,NOTIFY_DATA_SIZE(nc));
    ;
    break;
    default :ret = TGKModuleForm::on_gkhandle_notify(from,nc,mask);
    break;
  }
  return ret;
}

void     __fastcall TVTDSetupFrm::on_module_config_change(GKHANDLE mod,LPMODULE_CONFIG_DATA mcd)
{
 LPVTDCNTMOD_CONFIG mc = (LPVTDCNTMOD_CONFIG)mcd->data;
 VtdModConfig1->setup(mc);
}


void     __fastcall TVTDSetupFrm::on_module_state(GKHANDLE mod,DWORD state)
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

void     __fastcall TVTDSetupFrm::on_device_change(DWORD code,DWORD dev,DWORD grp)
{
 //Обработка изменений линии
 TTreeNode * node ;

 switch(code)
 {
  case VTDNC_GROUP_CREATED :
         node = this->find_line(dev);
         if(node)
         {
          node = find_group(node,grp);
          if(!node)
            {
             VTDDEV_GROUP grp_param;
             grp_param.dw_size = sizeof(grp_param);
             this->VtdGroupConfig1->get_param(&grp_param,MAKELONG(dev,grp));
             add_group(node,&grp_param);
            }
         }
         else
         add_line_node(dev);
       break;
  case VTDNC_GROUP_DELETED :
       node = find_group(find_line(dev),grp);
       if(node)
         {
          if(node->Selected)
          {
          TTreeNode * sel_node = node->getNextSibling();
          if(!sel_node)
              sel_node = node->getPrevSibling();
          if(!sel_node)
              sel_node = node->Parent;
          sel_node->Selected = true;
          }
          node->Delete();
         }
       break;

  case VTDNC_GROUP_UPDATED :
       node = find_group(find_line(dev),grp);
       if(node)
         {
          VtdGroupConfig1->number = MAKELONG(dev,grp);
         }
       break;
    case  VTDCNT_GROUP_DATA_CHANGED:
         DWORD number = MAKELONG(dev,grp);
         if(TreeView1->Selected  && (DWORD)TreeView1->Selected->Data ==  number)
         {
          this->ViewFrame1->set_number(number);
         }
         
      break; 

 }
}

void     __fastcall TVTDSetupFrm::on_modem_change(DWORD _code,LPVOID  data,DWORD sz)
{
  switch(_code)
  {
    case MNCODE_LINE_STATE  : on_modem_line_state((LPMODEM_LINE_STATE)data);
                              break;
    case MNCODE_MODEM_NUMBER: on_modem_change_number((LPMODEM_CHANGE_NUMBER)data);
                              break;
  }
}

void     __fastcall TVTDSetupFrm::on_modem_line_state(LPMODEM_LINE_STATE ls)
{
 if(ls->state_changes&MODEM_LINE_STATE_EXIST)
 {
  TTreeNode * node = find_line(ls->addr.line);
 if(ls->line_state&MODEM_LINE_STATE_EXIST)
    {
     if(!node)
      add_line_node(ls->addr.line);
    }
    else
    {
     if(node)
      {
       if(node->Selected)
       {
          TTreeNode * sel_node = node->getNextSibling();
          if(!sel_node)
              sel_node = node->getPrevSibling();
          if(!sel_node)
              sel_node = node->Parent;
          sel_node->Selected = true;
       }
       node->Delete();
      }
    }
  }  
}

void     __fastcall TVTDSetupFrm::on_modem_change_number(LPMODEM_CHANGE_NUMBER mcn)
{
 VtdModConfig1->mod_config.modem_number = mcn->new_number;
 VtdModConfig1->ModemNumber->Position   = mcn->new_number;

 VtdModConfig1->ch_mask = 0;
}



void     __fastcall TVTDSetupFrm::after_set_gkhandle()
{
  TGKModuleForm::after_set_gkhandle();
  VtdLineConfig1->mod_handle =  mod_handle;
  VtdModConfig1->mod_handle  =  mod_handle;
  VtdGroupConfig1->mod_handle=  mod_handle;
  ViewFrame1->mod_handle     =  mod_handle;  
  setup_mod_tree();
  on_module_state(mod_handle,mod_iface.get_module_state());
  set_notify(mod_iface(),MNF_COMMON|MNF_MODEM|VTDNF_DEV_CHANGED);

}

void     __fastcall TVTDSetupFrm::setup_mod_tree()
{
 TreeView1->Items->Clear();
 module_node = TreeView1->Items->Add(0,"Модем счётчиков ВТД");
 module_node->Data =(LPVOID)-1;
 DWORD idx = 0;
 VTDDEV_PARAM dp;
 dp.dw_size = sizeof(dp);
 while(GKH_RET_SUCCESS == mod_iface.call(VTDCMD_ENUM_DEVICE,(LPARAM)&dp,idx++))
 {
  add_line_node(&dp);
 }
 module_node->Selected = true;
 module_node->Expand(false);
 this->VtdModConfig1->setup();
}

AnsiString __fastcall TVTDSetupFrm::get_line_text(DWORD line_num)
{
 AnsiString ret;
 BYTE buf[1024] = {0}; 
 LPMODEM_LINE_PROPS   mlp = (LPMODEM_LINE_PROPS)buf;
 ZeroMemory(mlp,sizeof(mlp));
 mlp->dw_size = sizeof(buf);
 mlp->line_text_sz = (sizeof(buf)-sizeof(*mlp))/sizeof(wchar_t);

 if(GKH_RET_SUCCESS == mod_iface.call(MODEMCM_GET_LINE_PROPS,line_num,(LPARAM)mlp) && mlp->line_text_len)
    ret =  mlp->line_text;
    else
     ret.printf("Счётчик ВТД № %03d",line_num);
  return ret;
}

TTreeNode * __fastcall TVTDSetupFrm::add_line_node(DWORD num)
{
  TTreeNode * ret = find_line(num);
  if(!ret)
  {
    VTDDEV_PARAM dp;
    dp.dw_size = sizeof(dp);
    if(GKH_RET_SUCCESS == mod_iface.call(VTDCMD_ENUM_DEVICE,(LPARAM)&dp,num))
       ret =  add_line_node(&dp);
  }
  return ret;

}

int WINAPI node_sort_proc(LONG p1,LONG p2,LONG p3)
{
  TTreeNode * node1 = reinterpret_cast<TTreeNode*>(p1);
  TTreeNode * node2 = reinterpret_cast<TTreeNode*>(p2);
  return int(node1->Data) - int(node2->Data);
}


TTreeNode * __fastcall TVTDSetupFrm::add_line_node(LPVTDDEV_PARAM dp)
{
  TTreeNode * ret ;
  ret = TreeView1->Items->AddChild(module_node,get_line_text(dp->number));
  set_dev_grp_number(ret,dp->number);
  module_node->CustomSort(node_sort_proc,0,false);
  DWORD idx = 0;
  VTDDEV_GROUP vdg;
  vdg.dw_size = sizeof(vdg);
  while(mod_iface.call(VTDCMD_ENUM_GROUPS,(LPARAM)&vdg,MAKELONG(dp->number,idx++)) == GKH_RET_SUCCESS)
    add_group(ret,&vdg);

  return ret;
}

TTreeNode * __fastcall TVTDSetupFrm::find_line(DWORD num)
{
 TTreeNode * ret = module_node->getFirstChild();
 while(ret && (DWORD)(LOWORD(ret->Data))!=num)
 {
   ret = ret->getNextSibling();
 }
 return ret;
}

TTreeNode * __fastcall TVTDSetupFrm::find_group(TTreeNode * line,DWORD grp_num)
{
 if(line)
 {
 TTreeNode * child = line->getFirstChild();
 DWORD  ld = (DWORD)line->Data;

 while(child && HIWORD(ld)!=(WORD)grp_num)
 {
  child = child->getNextSibling();
 }
 return child;
 }
 return NULL;
}

TTreeNode * __fastcall TVTDSetupFrm::find_group(DWORD line_num,DWORD grp_num)
{
  return find_group(find_line(line_num),grp_num);
}

TTreeNode * __fastcall TVTDSetupFrm::add_group(TTreeNode * line,LPVTDDEV_GROUP grp)
{
  if(line)
  {
   TTreeNode * child = TreeView1->Items->AddChild(line,VtdGroupConfig1->get_text(grp->group_number));
   DWORD dn,gn;
   get_dev_grp_number(line,dn,gn);
   set_dev_grp_number(child,dn,grp->group_number);
  }
  return NULL;

}




void __fastcall TVTDSetupFrm::TreeView1Change(TObject *Sender,
      TTreeNode *Node)
{
 DWORD num = (DWORD)Node->Data;
 if(num == (DWORD)-1)
 {
  VtdLineConfig1->Visible = false;
  VtdGroupConfig1->Visible = false;
  VtdModConfig1->Visible  = true;
  DelBtn->Enabled = false;
 }
 else
 {
  /*Выбор линии - КП*/
  DWORD dn,gn;
  get_dev_grp_number(Node,dn,gn);
  VtdModConfig1 ->Visible = false;
  if((gn == (WORD)-1 && num!=(DWORD)-3) || num ==(DWORD) -2)
  {
   VtdGroupConfig1->Visible = false;
   VtdLineConfig1->Visible = true;
   VtdLineConfig1->number  = LOWORD(num);
  }
  else
  {
   VtdLineConfig1->Visible = false;
   VtdGroupConfig1->Visible = true;
   VtdGroupConfig1->number  = num;
  }
  DelBtn->Enabled = true;
  
 }
  ViewFrame1->number = num;
}
//---------------------------------------------------------------------------
DWORD    __fastcall TVTDSetupFrm::get_ch_mask()
{
 TTreeNode * node = TreeView1->Selected;
 if(node)
 {
  DWORD dn,gn;
  get_dev_grp_number(node,dn,gn);
  if(dn == (WORD)-1)
    return VtdModConfig1->ch_mask;
    else
    {
     if(gn == (WORD)-1 && dn!=(WORD)-3)
      return VtdLineConfig1->ch_mask;
      else
      return VtdGroupConfig1->ch_mask;

    }

 }
 return 0;
}
//---------------------------------------------------------------------------
void     __fastcall TVTDSetupFrm::on_changed(TObject * Sender)
{
  DWORD  ch_mask = get_ch_mask();
   UndoBtn->Enabled = (ch_mask) ? true:false; 
   ApplyBtn->Enabled = (ch_mask) ? true:false; 
}

void __fastcall TVTDSetupFrm::ApplyBtnClick(TObject *Sender)
{
  TTreeNode * sel = this->TreeView1->Selected;
  if(sel)
  {
    AnsiString cmd;
    DWORD dw_data = (DWORD)sel->Data;
    DWORD dn,gn;
    get_dev_grp_number(sel,dn,gn);

    DWORD apply_err  = 0;
    if(dn == (WORD)-1)
     {
      if(!VtdModConfig1->apply_changes())
         apply_err = GetLastError();
      cmd = "Изменение параметров модуля";
     }
     else
      {
       if(dn!=(WORD)-3 && (gn == (WORD)-1 || dw_data == (DWORD)-2))
       {
       if(!VtdLineConfig1->apply_changes())
           apply_err = GetLastError();
       if((DWORD)sel->Data ==(DWORD)-2)
         {
          //Режим создания
          cmd = "Создание счётчика";

         }
         else
         cmd = "Изменение параметров счётчика";
         if(apply_err == 0)
            {
            set_dev_grp_number(sel,VtdLineConfig1->number);
            update_node_text(sel);
            }

        }
        else
        {
          if(!VtdGroupConfig1->apply_changes())
             apply_err = GetLastError();
          if(apply_err == 0)
             {
              sel->Data = (LPVOID)VtdGroupConfig1->number;
              sel->Text = VtdGroupConfig1->get_text(HIWORD(VtdGroupConfig1->number));
             } 
        }

      }
   show_error(cmd,apply_err);
  }
}
//---------------------------------------------------------------------------

void __fastcall TVTDSetupFrm::UndoBtnClick(TObject *Sender)
{
  TTreeNode * sel = this->TreeView1->Selected;
  if(sel)
  {

    DWORD data =(DWORD)sel->Data;
    DWORD dn,gn;
    get_dev_grp_number(sel,dn,gn);
    if(data == (DWORD )-1)
     {
      VtdModConfig1->undo_changes();
     }
     else
      {
       if(data == (DWORD)-2 || data == (DWORD)-3)
       DelBtnClick(this);
       else
       {
        if(gn == (WORD)-1)
        VtdLineConfig1->undo_changes();
        else
        VtdGroupConfig1->undo_changes();
       }
      }
  }
}
//---------------------------------------------------------------------------

void   __fastcall TVTDSetupFrm::update_node_text(TTreeNode * node)
{
  DWORD dn,gn;
  get_dev_grp_number(node,dn,gn);
  if(gn!=(WORD)-1)
  {
   //Группа
  }
  else
  {
   //счётчик
   node->Text = get_line_text(dn);
  }


}

bool     __fastcall TVTDSetupFrm::is_create_mode()
{
 bool ret = false;

  DWORD dn,gn;
  DWORD dw_data = get_dev_grp_number(TreeView1->Selected,dn,gn);
 if(dw_data)
 {
   if(dw_data == (WORD) -2 || gn == (WORD)-3)
      ret = true;
 }
 return ret;
}

void __fastcall TVTDSetupFrm::NewBtnClick(TObject *Sender)
{
 //Создание линии устройства-счётчика КП
 TTreeNode * node = TreeView1->Selected;
 PageControl1->ActivePageIndex = 0;
 if(!node || node == module_node)
 {
  node = TreeView1->Items->AddChild(module_node,"Создание устройства...");
  node->Data = (LPVOID)-2; // режим создания
  module_node->Expand(true);
  node->Selected = true;
 }
 else
 {
  DWORD dn,gn;
  get_dev_grp_number(node,dn,gn);
  if(gn!=(WORD)-1)
     node = node->Parent;
  TTreeNode * child = TreeView1->Items->AddChild(node,"Создание группы");
  child->Data = (LPVOID)MAKELONG(dn,-3);
  node->Expand(false);
  child->Selected = true;
 }

}
//---------------------------------------------------------------------------


void __fastcall TVTDSetupFrm::TreeView1Changing(TObject *Sender,
      TTreeNode *Node, bool &AllowChange)
{
  TTreeNode * sel = TreeView1->Selected;
  if(sel)
  {
   DWORD dw_data = (DWORD)sel->Data;
   if(dw_data == (DWORD)-2 || HIWORD(dw_data)==WORD(-3))
      AllowChange = false;
  }
}
//---------------------------------------------------------------------------

void __fastcall TVTDSetupFrm::DelBtnClick(TObject *Sender)
{
  TTreeNode * node = TreeView1->Selected;
  DWORD number = (DWORD)node->Data;
  if(number != (DWORD)-2 && number != (DWORD)-3)
  {
   //Реально удалить
   DWORD dn,gn;
   get_dev_grp_number(node,dn,gn);
   if(gn == (WORD)-1)
     mod_iface.call(VTDCMD_DELETE_DEVICE,dn,0);
     else
     mod_iface.call(VTDCMD_DELETE_GROUP,dn,gn);
  }
  TTreeNode * prev = node->getPrevSibling();
  node->Delete();
  if(prev)
     prev->Selected = true;
   else
   module_node->Selected = true;
   UndoBtn->Enabled = false;
   ApplyBtn->Enabled = false;
}
//---------------------------------------------------------------------------

void __fastcall TVTDSetupFrm::StartBtnClick(TObject *Sender)
{
 if(mod_iface.start(MODULE_START_REQUEST,0))
    {
     show_error("Запуск", GetLastError());
    }
}
//---------------------------------------------------------------------------

void __fastcall TVTDSetupFrm::StopBtnClick(TObject *Sender)
{
  if(mod_iface.stop(MODULE_STOP_REQUEST))
  {
   show_error("Останов", GetLastError());
  }
}
//---------------------------------------------------------------------------

void __fastcall TVTDSetupFrm::Splitter1Moved(TObject *Sender)
{
 StatusBar1->Panels->Items[0]->Width = TreeView1->Width;
}

//---------------------------------------------------------------------------
void __fastcall TVTDSetupFrm::show_error(AnsiString cmd, DWORD err)
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

void __fastcall TVTDSetupFrm::FormHide(TObject *Sender)
{
   reset_notify(mod_iface(),MNF_COMMON|MNF_MODEM|VTDNF_DEV_CHANGED|VTDNF_TXRX);
}
//---------------------------------------------------------------------------

void __fastcall TVTDSetupFrm::FormShow(TObject *Sender)
{
   set_notify(mod_iface(),MNF_COMMON|MNF_MODEM|VTDNF_DEV_CHANGED|VTDNF_TXRX);
}
//---------------------------------------------------------------------------

void __fastcall TVTDSetupFrm::RefreshBtnClick(TObject *Sender)
{
 /*Обновление вызов*/
 //modemmer.h
 if(TreeView1->Selected)
 {
  DWORD gn,dn;
  get_dev_grp_number(TreeView1->Selected,dn,gn);
  mod_iface.call(MODEMCM_REFRESH_LINE,dn,gn);
 }        
}
//---------------------------------------------------------------------------

void __fastcall TVTDSetupFrm::PageControl1Changing(TObject *Sender,
      bool &AllowChange)
{
 /*
   При смене Закладки
   Запрещено в режиме создания переключаться с первой закладки
 */
 if(CreateMode && PageControl1->ActivePageIndex == 0)
    AllowChange = false;
  
}
//---------------------------------------------------------------------------


void __fastcall TVTDSetupFrm::FormKeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
 switch(Key)
 {
  case VK_RETURN:  if(this->ApplyBtn->Enabled)
                       ApplyBtnClick(this);
                    break;
  case VK_INSERT:  if(NewBtn->Enabled)
                      NewBtnClick(this);
                      break;
  case VK_DELETE:  if(Shift.Contains(ssCtrl) && DelBtn->Enabled )
                      DelBtnClick(this);
                      break;
  case VK_BACK  : if(Shift.Contains(ssAlt) && UndoBtn->Enabled )
                       UndoBtnClick(this);
                      break;
 }
}
//---------------------------------------------------------------------------

void __fastcall TVTDSetupFrm::N1Click(TObject *Sender)
{
 //Вызов часового архива
 DWORD dn,gn; 
 /*DWORD dw_data  = */get_dev_grp_number(TreeView1->Selected,dn,gn);

 mod_iface.call(VTDCMD_CALL_HOURS_ARCHIVE,dn,FALSE);
}
//---------------------------------------------------------------------------

void __fastcall TVTDSetupFrm::N2Click(TObject *Sender)
{
 //Вызов суточного архива
 DWORD dn,gn;
 /*DWORD dw_data  =*/ get_dev_grp_number(TreeView1->Selected,dn,gn);
 mod_iface.call(VTDCMD_CALL_DAYS_ARCHIVE,dn,TRUE);
}
//---------------------------------------------------------------------------
void __fastcall TVTDSetupFrm::before_show(LPMODULE_WINDOW mw)
{
    PageControl1->ActivePageIndex = mw->window_type-1;
    PageControl1->ActivePage      = PageControl1->Pages[mw->window_type-1];
    if(PageControl1->OnChange)
       PageControl1->OnChange(this);
}
