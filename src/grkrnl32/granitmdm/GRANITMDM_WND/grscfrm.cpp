//---------------------------------------------------------------------------

#pragma warn -8058
#include <vcl.h>
#pragma warn .8058
#pragma hdrstop

#include "grscfrm.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "gkmodule_form"
#pragma link "cp_frame"
#pragma link "sc_base_frame"
#pragma link "floor_frame"
#pragma link "settings_frame1"
#pragma link "grp_frame"
#pragma link "sc_base_data_frame"
#pragma link "sc_grp_data_frame"
#pragma link "sc_cp_data_frame"
#pragma link "about_common_frame"
#pragma resource "*.dfm"
TSCGranFrm *SCGranFrm;
//---------------------------------------------------------------------------


__fastcall TSCGranFrm::TSCGranFrm(TComponent* Owner)
        : TGKModuleForm(Owner)
{}

__fastcall TSCGranFrm::TSCGranFrm(TComponent* Owner,HWND owner_hwnd,GKHANDLE handle)
        : TGKModuleForm(Owner,owner_hwnd,handle)
{
 sc_monitor = NULL;
 SCTree->Items->Clear();
 ch_mask = 0;
 selected_type = -1;
 prop_frame = NULL;
 data_frame = NULL;
 BYTE data[128];
 gr_proto gp;
 gp.dw_size = sizeof(gp);
 gr_proto_format(2,(GRKT_DATAIN<<4)|1,(GRINF_TS<<4)|1,data,128,&gp);
 memset(gp.data,0,16);
 memset(gp.data,-1,8);
 gr_proto_protect(&gp);
 gr_proto_format(2,(GRKT_DATAIN<<4)|1,(GRINF_TIT<<4)|1,data,128,&gp);
 memset(gp.data,0xA0,16);
 gr_proto_protect(&gp);
 gr_proto_format(2,(GRKT_DATAIN<<4)|1,(GRINF_TS<<4)|1,data,128,&gp);
 #ifdef COMERCIAL_BUILD
 //Label30->Caption = "ООО \"Компания промышленной автоматизации\"";
 #endif
}
//---------------------------------------------------------------------------

void     __fastcall TSCGranFrm::after_set_gkhandle()
{
 Inherit::after_set_gkhandle();
 on_module_state(mod_iface(),mod_iface.get_module_state());
 SCSettingsFrm1->on_change  = on_changes;
 SCSettingsFrm1->mod_handle = mod_iface();
 SCSettingsFrm1->Align      = alClient;
 SCSettingsFrm1->set_inactive();

 GrpDataFrm->mod_handle = mod_iface();
 GrpDataFrm->on_change = on_changes;
 GrpDataFrm->set_inactive();

 CpDataFrm->mod_handle = mod_iface();
 CpDataFrm->on_change  = on_changes;
 CpDataFrm->set_inactive();

 CPFrame->on_change  = on_changes;
 CPFrame->mod_handle = mod_iface();
 CPFrame->Align      = alClient;
 CPFrame->set_inactive();

 GroupFrame1->on_change  = on_changes;
 GroupFrame1->mod_handle = mod_iface();
 GroupFrame1->Align      = alClient;
 GroupFrame1->set_inactive();


 FloorFrame->on_change  = on_changes;
 FloorFrame->mod_handle = mod_iface();
 FloorFrame->Align      = alClient;
 //FloorFrame->set_inactive();
 init_form();
 set_notify(mod_iface(),MNF_COMMON|MNF_MODEM|MNF_SC);
 TGKModuleInterface m(this->form_gkhandle,true);
 char text[256];
 m.get_module_name(text,sizeof(text));
}


void     __fastcall TSCGranFrm::on_module_state(GKHANDLE mod,DWORD state)
{
  bool running = ((state & MODULE_STATE_RUNNING_MASK )== MODULE_STATE_RUNNING) ? true:false;
  bool stopped  = (state & MODULE_STATE_RUNNING_MASK ) ? false:true;
  miStart->Enabled   = stopped;
  StartBtn->Enabled  = stopped;
  miStop->Enabled    = running;
  StopBtn->Enabled   = running;
  MNCODE_LINE_STATE;
}

void     __fastcall TSCGranFrm::on_module_config_change(GKHANDLE mod,LPMODULE_CONFIG_DATA mcd)
{
 SCSettingsFrm1->config_change(mcd->mask,(lpgrsc_settings)mcd->data);
}


LRESULT  __fastcall TSCGranFrm::on_gkhandle_notify (GKHANDLE from,LPNOTIFY_CODE nc,DWORD mask)
{
  /*if(this->sc_monitor)
    {
     AnsiString str;
     str.printf("NOTIFY GKHANDLE %08X mask - %08X, CODE = %08X data size = %d",(DWORD)from,mask,nc->notify_code,nc->dw_size);
     sc_monitor->add_string(str.c_str());
    }*/

  if(mask == MNF_MODEM  )
  {
    switch(nc->notify_code)
    {
     case  MNCODE_MODEM_NUMBER: /*on_modem_number_change */ break;
     case  MNCODE_LINE_STATE  : on_floor_state((LPMODEM_LINE_STATE)nc->notify_data);break;
    }
    return GKH_RET_SUCCESS;
  }
  if(mask == MNF_SC)
  {
    
   switch(nc->notify_code)
   {
      case MNCODE_SC_ADDED      : on_sc_added        ((LPGRSC_ACR)nc->notify_data);break;
      case MNCODE_SC_CHANGED    : on_sc_changed      ((LPGRSC_ACR)nc->notify_data);break;
      case MNCODE_SC_REMOVED    : on_sc_removed      ((LPGRSC_ACR)nc->notify_data);break;
      case MNCODE_SC_DATACHANGED: on_sc_data_changed ((LPGRSC_ACR)nc->notify_data);break;
   }
   return GKH_RET_SUCCESS;
  }

  return Inherit::on_gkhandle_notify(from,nc,mask);
}

void     __fastcall TSCGranFrm::on_floor_state( LPMODEM_LINE_STATE mls)
{

 if(mls->state_changes&MODEM_LINE_STATE_EXIST)
 {
   TTreeNode * floor = find_floor_node(mls->addr.line);
   if(mls->line_state & MODEM_LINE_STATE_EXIST)
     {
       if(!floor)
         {
          add_floor(sc_root,mls->addr.line);
          sc_root->Expand(false);
         }
     }
     else
     if(floor) floor->Delete();
 }
}


void __fastcall TSCGranFrm::miCloseClick(TObject *Sender)
{
 this->Close();
}
//---------------------------------------------------------------------------
void __fastcall TSCGranFrm::on_monitor_destroy(TObject * sender)
{
  if(sender == sc_monitor)
     sc_monitor = NULL;
}




void __fastcall TSCGranFrm::MonitorBtnClick(TObject *Sender)
{
  if(!this->sc_monitor)
      {
       MonitorBtn->Down = true;
       sc_monitor = new TSCMonitor(MonitorSheet,mod_iface());
       sc_monitor->Align = alClient;
       DWORD filter = -1;
       if(SCTree->Selected)
          filter = (DWORD)SCTree->Selected->Data;
       sc_monitor->change_filter(filter);
       sc_monitor->OnDestroy = on_monitor_destroy;
       MonitorSheet->TabVisible = true;
       PageControl1->ActivePageIndex = MonitorSheet->TabIndex;

       sc_monitor->ManualDock(MonitorSheet,NULL,alClient); //Dock(MonitorSheet,MonitorSheet->ClientRect);
       sc_monitor->Show();
      }
      else
      {
       MonitorSheet->TabVisible = false;
       sc_monitor->Hide();
       delete sc_monitor;
       sc_monitor = 0;
      }
}
//---------------------------------------------------------------------------

void __fastcall TSCGranFrm::NewWndBtnClick(TObject *Sender)
{
  TSCGranFrm * frm  = new TSCGranFrm(this,NULL,mod_iface());
  frm->Caption = frm->Caption+"[Копия]";
  frm->Show();
}
//---------------------------------------------------------------------------


void __fastcall TSCGranFrm::FormClose(TObject *Sender,
      TCloseAction &Action)
{
  if(Action == caHide && !owner_window)
       Action =  caFree;
}
//---------------------------------------------------------------------------


void __fastcall TSCGranFrm::Setch_mask(DWORD value)
{
        //TODO: Add your source code here
        Fch_mask = value;
        bool enabled = value ? true:false;
        ApplyBtn->Enabled = enabled;
        UndoBtn ->Enabled = enabled;
}


grsc_settings &  __fastcall TSCGranFrm::get_settings()
{

 return this->SCSettingsFrm1->get_settings();
}



void        __fastcall TSCGranFrm::init_form()
{
 AnsiString    root_name = get_settings().pu_name;
 root_name += " Системная централь";
 sc_root       = SCTree->Items->Add(NULL,root_name);
 sc_root->Data = (LPVOID)-1;
 enum_floors(sc_root);
 sc_root->Selected = true;
 sc_root->Expand(false);
}

void        __fastcall TSCGranFrm::enum_floors(TTreeNode * root)
{
  //Перечисление этажей
  DWORD index = 0;
  MODEM_LINE_STATS mls;
  mls.dw_size      = sizeof(mls);

  while(GKH_RET_SUCCESS==mod_iface.call(MODEMCM_ENUM_LINES,index++,(LPARAM)&mls))
  {
   add_floor(root,mls.addr.line);
  }
}

TTreeNode *   __fastcall TSCGranFrm::add_floor(TTreeNode * root,DWORD num)
{
  BYTE  buf[1024];
  LPMODEM_LINE_PROPS mlp =(LPMODEM_LINE_PROPS)buf;
  mlp->dw_size = sizeof(buf);
  mlp->line_text_sz  =(mlp->dw_size-sizeof(*mlp))/sizeof(wchar_t);
  AnsiString text;
  if(GKH_RET_SUCCESS == mod_iface.call(MODEMCM_GET_LINE_PROPS,num,(LPARAM)mlp))
     text = mlp->line_text;
   else
     text.printf("Этаж № %d",num);
  TTreeNode * floor = SCTree->Items->AddChild(root,text);
  floor->Data =(LPVOID) sotd_addr(0,GRPROTO_MK_CPNUM(num,0),-1,-1).addr;
  set_node_image(floor);
  enum_cp(floor);
  root ->CustomSort(node_comparer,0,false);
  return floor;
}

void   __fastcall TSCGranFrm::enum_cp(TTreeNode * floor)
{
  GR_CP_PROPS cpp;
  cpp.dw_size = sizeof(cpp);
  cpp.mask         = -1;
  cpp.addr.addr    = (DWORD)floor->Data;
  DWORD       index = 0;
  while(mod_iface.call(SCCM_ENUM_CP,index++,(LPARAM)&cpp)==GKH_RET_SUCCESS)
  {
    add_cp(cpp,floor);
  }
  floor->CustomSort(node_comparer,0,false);
}

TTreeNode * __fastcall TSCGranFrm::add_cp(GR_CP_PROPS & cpp,TTreeNode * floor)
{
  TTreeNode * cp ;
  if(!floor)
     floor = find_floor_node(GRPROTO_GET_FLOOR_NUM(cpp.addr.cp));
  if(floor)
  {
  AnsiString str  ;
  str.printf("КП №-%d ",(DWORD)cpp.addr.cp);
  str+=cpp.name;
  cp = SCTree->Items->AddChild(floor,str);
  cp->Data  = (LPVOID)cpp.addr.addr;
  set_node_image(cp);
  enum_group  (cp);
  floor->CustomSort(node_comparer,0,false);
  }
  else
   cp = find_cp_node(cpp.addr.cp,add_floor(this->sc_root,GRPROTO_GET_FLOOR_NUM(cpp.addr.cp)));
  return cp;
}

TTreeNode * __fastcall TSCGranFrm::add_cp(DWORD addr)
{
 sotd_addr     sa(addr);
 TTreeNode * floor = this->find_floor_node(GRPROTO_GET_FLOOR_NUM(sa.cp));
 TTreeNode * cp = NULL;
 if(floor)
  {
   cp = find_cp_node(sa.cp,floor);
   if(!cp)
   {
      GR_CP_PROPS   cpp;
      cpp.dw_size   = sizeof(cpp);
      if(GKH_RET_SUCCESS == mod_iface.call(SCCM_GET_PROPS,addr,(LPARAM)&cpp))
         cp = add_cp(cpp,floor);
   }
   else
   {
    this->set_node_image(cp);
   }
  }
  return cp;
}

void        __fastcall TSCGranFrm::enum_group     (TTreeNode * cp)
{
  if(cp)
  {
   GR_GRP_PROPS  grp;
   grp.dw_size   = sizeof(grp);
   grp.mask      = -1;
   DWORD index   =  0;
   grp.addr.addr =  (DWORD)cp->Data;
   while(GKH_RET_SUCCESS == mod_iface.call(SCCM_ENUM_GRP,index++,(LPARAM)&grp))
        add_group(grp,cp);

  }
}

char * fa_texts[] =
{
 "ТС",
 "ТИТ",
 "ТИИ"
} ;

TTreeNode * __fastcall TSCGranFrm::add_group      (GR_GRP_PROPS & grp,TTreeNode * cp)
{
 /*Добавление группы*/
 TTreeNode * node ;
 if(!cp)
    cp = find_cp_node(grp.addr.cp);
 if(cp)
 {
  node = find_group_node(grp.addr.addr,cp);
  if(!node)
  {
   AnsiString text;
   text.printf("Группа %s №%02d ",fa_texts[grp.addr.fa],grp.addr.sb);
   node = SCTree->Items->AddChild(cp,text);
   node->Data = (LPVOID)grp.addr.addr;
   set_node_image(node);
   cp->CustomSort(node_comparer,0,false);
  }
 }
 else
  {
   cp   = add_cp(grp.addr.addr);
   node = find_group_node(grp.addr.addr,cp);
   if(node)
     set_node_image(node);
  }
 return node;
}

TTreeNode * __fastcall TSCGranFrm::add_group      (DWORD addr)
{

 GR_GRP_PROPS    props;
 props.dw_size = sizeof(props);
 props.mask    = -1;
 props.addr.addr = addr;
 if(GKH_RET_SUCCESS == mod_iface.call(SCCM_GET_PROPS,addr,(LPARAM)&props))
    return add_group(props);
 return NULL;
}

TTreeNode * __fastcall TSCGranFrm::find_group_node(DWORD addr,TTreeNode * cp_node )
{
 /*Поиск группы*/
 TTreeNode * ret = NULL;
 sotd_addr sa(addr);
 if(!cp_node)
     cp_node = find_cp_node(sa.cp);
 if(cp_node)
   {
    ret = cp_node->getFirstChild();
    while(ret && sotd_addr((DWORD)ret->Data)!=sa)
          ret = ret->getNextSibling();
   }

   return ret;
}




TTreeNode * __fastcall TSCGranFrm::find_floor_node(DWORD num)
{
 TTreeNode * floor = sc_root ? sc_root->getFirstChild():NULL;
 otd_addr addr;
 while(floor)
 {
  addr.addr = (DWORD)floor->Data;
  if((DWORD)GRPROTO_GET_FLOOR_NUM(addr.cp)==num)
     break;
  floor = floor->getNextSibling();
 }
 return floor;
}

TTreeNode * __fastcall TSCGranFrm::find_cp_node(DWORD num,TTreeNode * floor)
{
 TTreeNode * ret = NULL;
 if(!floor)
    floor = find_floor_node(GRPROTO_GET_FLOOR_NUM(num));
  if(floor)
   {
     ret = floor->getFirstChild();
     while(ret && (DWORD)sotd_addr((DWORD)ret->Data).cp!=num)
           ret = ret->getNextSibling();
   }
 return ret;
}

TTreeNode * __fastcall TSCGranFrm::find_node      (DWORD addr)
{
 TTreeNode * ret ;
 sotd_addr sa(addr);
 ret = find_floor_node(GRPROTO_GET_FLOOR_NUM(sa.cp));
 if(ret && GRPROTO_GET_LU_NUM(sa.cp))
 {
    ret = find_cp_node(sa.cp,ret);
    if(ret && sa.sb <OTD_ADDR_MAXVALUE)
      ret = find_group_node(addr,ret);
 }

 return ret;
}




void __fastcall TSCGranFrm::RefreshBtnClick(TObject *Sender)
{
  /*Обновление*/

  sotd_addr addr((DWORD)SCTree->Selected->Data);
  mod_iface.call(SCCM_REFRESH,addr.addr,0);

}
//---------------------------------------------------------------------------

void __fastcall TSCGranFrm::SelectFrames(otd_addr & addr)
{
 switch(selected_type)
 {
  case
  OTD_ADDR_TYPE_PU       :
  break;
  case
  OTD_ADDR_TYPE_CP       : prop_frame = GRPROTO_GET_LU_NUM(addr.cp) ? (TSCBaseFrame*)CPFrame:(TSCBaseFrame*)FloorFrame;
                           data_frame = CpDataFrm;
  break;
  case
  OTD_ADDR_TYPE_FA       :
  break;
  case
  OTD_ADDR_TYPE_SB       :prop_frame = GroupFrame1;
                          data_frame = GrpDataFrm;
  break;
  case
  OTD_ADDR_TYPE_BROADCAST: prop_frame = SCSettingsFrm1;
                           data_frame = NULL;

  break;
 }
}

void __fastcall TSCGranFrm::SCTreeChange(TObject *Sender, TTreeNode *Node)
{
 begin_call_gkhandle("");
 sotd_addr addr((DWORD)Node->Data);
 if(sc_monitor)
    sc_monitor->change_filter(addr.addr);
 if(!create_mode)
    selected_type =  addr.addrtype();

    SelectFrames(addr);

  if(PageControl1->ActivePageIndex)
    {
     if(data_frame)
        data_frame->set_active(addr.addr);
    }
    else
    {
     if(prop_frame )
      {
       prop_frame->create_mode = create_mode;
       prop_frame->set_active(addr.addr);
      }
    }

}
//---------------------------------------------------------------------------

void __fastcall TSCGranFrm::SCTreeChanging(TObject *Sender,
      TTreeNode *Node, bool &AllowChange)
{

 if( prop_frame)
    {
     if(prop_frame->create_mode)
        AllowChange = false;
        else
     prop_frame->set_inactive();
    }

 if(PageControl1->ActivePageIndex)
     if(data_frame) data_frame->set_inactive();
}
//---------------------------------------------------------------------------

void __fastcall TSCGranFrm::StartBtnClick(TObject *Sender)
{
  begin_call_gkhandle("Запуск ситемной централи");
  this->done_call_gkhandle(mod_iface.start(MODULE_START_REQUEST,0));;
}
//---------------------------------------------------------------------------

void __fastcall TSCGranFrm::StopBtnClick(TObject *Sender)
{
  begin_call_gkhandle("Останов ситемной централи");
  done_call_gkhandle ( mod_iface.stop(MODULE_STOP_REQUEST));
  
}
//---------------------------------------------------------------------------

void     __fastcall TSCGranFrm::on_sc_added  (LPGRSC_ACR acr)
{
 sotd_addr     sa(acr->addr);
 DWORD type  = sa.addrtype();
 TTreeNode * node = NULL;
 switch (type)
 {
  case OTD_ADDR_TYPE_CP:
       node = add_cp(sa.addr);
       break;
  case OTD_ADDR_TYPE_SB:
       node = add_group(sa.addr);
       break;
 }
 if(node)
  {
   node = node->Parent;
   while (node)
   {
    node->Expand(false);
    node = node->Parent;
   };

  } 
}

void     __fastcall TSCGranFrm::on_sc_changed(LPGRSC_ACR acr)
{
 if(prop_frame && prop_frame->get_addr() == acr->addr)
   prop_frame->update_changes(acr);
}

void     __fastcall TSCGranFrm::on_sc_data_changed(LPGRSC_ACR acr)
{
 sotd_addr addr(acr->addr);
 if(data_frame )
 {
    sotd_addr a1(data_frame->get_addr());
    if(addr == a1 ) 
     data_frame->update_data_changes(acr);
     else
     {
      if(addr.is_include(a1) && (acr->parts&OTD_PROTO_PART_DIAG))
      {
       GRSC_ACR  dacr;
       dacr.parts = OTD_PROTO_PART_DIAG|OTD_PROTO_PART_TIMESTAMP;
       dacr.addr  = a1.addr;
       dacr.loN   = 0;
       dacr.hiN   = -1;
       dacr.what  = acr->what;
       data_frame->update_data_changes(&dacr); 
      }
     }
 }
    
    if(!acr->what && (acr->parts&OTD_PROTO_PART_DIAG))
    {
     TTreeNode * node = find_node(acr->addr);
     if(node)
      {
       set_node_image(node);
       TTreeNode * parent = node->Parent,*child = node->getFirstChild();
       while(parent && (DWORD)parent->Data!= (DWORD)-1)
             {
              set_node_image(parent);
              parent = parent->Parent;
             }
       while(child)
       {
        set_node_image(child);
        child = child->getNextSibling();
       }

      }
    }
}


void     __fastcall TSCGranFrm::on_sc_removed(LPGRSC_ACR acr)
{
 TTreeNode * node = find_node(acr->addr);
 if(node)
    node->Delete();
}



void __fastcall TSCGranFrm::DelBtnClick(TObject *Sender)
{
 TTreeNode * node = SCTree->Selected;
 if(node && node!=sc_root)
 {
  //Удаление элемента БД
  if(create_mode)
    {
     TTreeNode * node = SCTree->Selected;
     create_mode = false;
     node->Parent->Selected = true;
     node->Delete();
    }
    else
    {
     if(MessageBox(Handle,"Действительно удалить?",NULL,MB_ICONQUESTION|MB_YESNO)==ID_YES)
     {
      LRESULT ret = mod_iface.call(SCCM_REMOVE,(LPARAM)node->Data,0);
      if(ret == GKH_RET_SUCCESS)
           node->Delete();
     }   
    }

 }
}
//---------------------------------------------------------------------------

void __fastcall TSCGranFrm::CreateBtnClick(TObject *Sender)
{
 //Создание модуля
  create_mode = true;
  begin_call_gkhandle(SCTree->Selected ? AnsiString(SCTree->Selected->Text).c_str() :(char*) "Создание объекта...");
}
//---------------------------------------------------------------------------


char * create_text[] =
{
 "Нового этажа",
 "Нового КП",
 "Новой группы"
};

void __fastcall TSCGranFrm::Setcreate_mode(bool value)
{
        //TODO: Add your source code here
        if(Fcreate_mode != value)
        {
         Fcreate_mode = value;
         if(value)
         {
          //Установка режима создания
         PageControl1->ActivePageIndex = 0;
         if(data_frame)
            data_frame->set_inactive();

          TTreeNode * parent_node = SCTree->Selected;
          sotd_addr   sa((DWORD)parent_node->Data);

          DWORD       new_type ;
          AnsiString str = "Создание ";
          switch(selected_type)
          {
            case OTD_ADDR_TYPE_CP       : if(sa.cp&0x0F)
                                          {
                                          new_type = OTD_ADDR_TYPE_SB;
                                          str+="группы";
                                          }
                                          else
                                          {
                                           new_type =OTD_ADDR_TYPE_CP ;sa.cp |= 0x0F;
                                           str+="КП";
                                          }


            break;
            case OTD_ADDR_TYPE_SB       : new_type =  OTD_ADDR_TYPE_SB;
                                          parent_node = parent_node->Parent;
                                          str+="группы";
            break;
            case OTD_ADDR_TYPE_BROADCAST: new_type = OTD_ADDR_TYPE_CP;
                                          sa.pu =0; sa.cp=0xF0;
                                          str+="этажа";
            break;

          }


          TTreeNode * new_node = SCTree->Items->AddChild(parent_node,str);
          new_node->Data = (LPVOID)sa.addr;
          selected_type  = new_type;
          set_node_image(new_node);
          new_node->Selected = true;
          

         }
         else
         {
          if(prop_frame)
             prop_frame->create_mode = false;
         }

        }
}

void __fastcall TSCGranFrm::PageControl1Changing(TObject *Sender,
      bool &AllowChange)
{
 AllowChange=!create_mode;
 if(AllowChange)
 if(PageControl1->ActivePageIndex)
    if(data_frame) data_frame->set_inactive();
}
//---------------------------------------------------------------------------

void    __fastcall TSCGranFrm::sort_nodes(TTreeNode * node)
{
 node->CustomSort(node_comparer,0,false);
}

void __fastcall TSCGranFrm::ApplyBtnClick(TObject *Sender)
{
  AnsiString err_str;
  if(!create_mode)
    begin_call_gkhandle("Применение изменений");
 if(prop_frame->apply_changes())
 {
  done_call_gkhandle(0);
  TTreeNode * node = SCTree->Selected;
  if(create_mode)
    {
     sotd_addr  addr(prop_frame->get_addr());
     node->Data =(LPVOID)addr.addr;
     create_mode = false;
     SelectFrames(addr);
     if(prop_frame)
        prop_frame->set_active(addr.addr);
     node->Parent->CustomSort(node_comparer,0,false);
    }
     node->Text = prop_frame->get_text();
 }
 else
   {
    done_call_gkhandle(GKH_RET_ERROR);
    prop_frame->undo_changes();
   }

}
//---------------------------------------------------------------------------

void __fastcall TSCGranFrm::UndoBtnClick(TObject *Sender)
{

 if(!create_mode)
   {
    begin_call_gkhandle("Отмена изменений");
    done_call_gkhandle(prop_frame->undo_changes() ? GKH_RET_SUCCESS:GKH_RET_ERROR);
   } 
    else
    DelBtnClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TSCGranFrm::PageControl1Change(TObject *Sender)
{

 DWORD addr = SCTree->Selected ? (DWORD)SCTree->Selected->Data :(DWORD)-1;
 switch(PageControl1->ActivePageIndex)
 {
  case 0:   if(prop_frame ) prop_frame->set_active(addr);
            break;
  case 1:   if(data_frame) data_frame->set_active(addr);
            break;

 }
}
//---------------------------------------------------------------------------





void __fastcall TSCGranFrm::Splitter1Moved(TObject *Sender)
{
  StatusBar1->Panels->Items[0]->Width =  SCTree->Width;
}
//---------------------------------------------------------------------------


void __fastcall TSCGranFrm::miAboutClick(TObject *Sender)
{
 PageControl1->ActivePageIndex = 2;
}
//---------------------------------------------------------------------------

void     __fastcall TSCGranFrm::begin_call_gkhandle(char * cmd_name)
{

 StatusBar1->Panels->Items[0]->Text =  cmd_name;
 StatusBar1->Panels->Items[1]->Text =  AnsiString();
}

void     __fastcall TSCGranFrm::done_call_gkhandle(LRESULT ret,char * str)
{
 DWORD last_error = ret ? GetLastError():0;
 if(str)
 {
  StatusBar1->Panels->Items[0]->Text = str;
 }
 switch(ret)
 {

  case GKH_RET_SUCCESS: 
  case GKH_RET_ERROR  :
                      {
                      wchar_t  err_text[MAX_PATH];
                      mod_iface.get_error_text(last_error,err_text,sizeof(err_text)/sizeof(wchar_t));
                      StatusBar1->Panels->Items[1]->Text = AnsiString(err_text);
                      }
                      break;
                      
  
 }
}



void __fastcall TSCGranFrm::FormKeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
 switch(Key)
 {
  case  0x0D : if(ch_mask) ApplyBtnClick(this); break;
  /*case  0x1B : if(ch_mask && Shift.Contains(ssAlt)) UndoBtnClick(this) ; break;
  case  0x2D : if(!create_mode ) CreateBtnClick(this); break;
  case  0x2E : if(Shift.Contains(ssCtrl)) {DelBtnClick(this);Key = 0;} break;*/
  default : break;
 }
}
//---------------------------------------------------------------------------

void __fastcall TSCGranFrm::FormDestroy(TObject *Sender)
{
  if(sc_monitor)
     delete sc_monitor;
}
//---------------------------------------------------------------------------

void     __fastcall TSCGranFrm::set_node_image(TTreeNode * node,DWORD diag)
{
 DWORD imgidx ;
 if(diag & OTD_DIAG_FULL_MASK)
   {
    if(diag & OTD_DIAG_MASK)
      imgidx = 2;
      else
      imgidx = 1;
   }
   else
   imgidx = 0;
   node->SelectedIndex = imgidx;
   node->StateIndex    = imgidx;
   node->ImageIndex    = imgidx;

}
void     __fastcall TSCGranFrm::set_node_image(TTreeNode * node)
{

 BYTE buffer[256];
 LPSC_OTDDATA scod = (LPSC_OTDDATA)buffer;  //granit_modem.h
 ZeroMemory(scod,sizeof(*scod));
 scod->dw_size = sizeof(buffer);
 scod->parts   = OTD_PROTO_PART_DIAG;
 scod->data_size = sizeof(buffer)-sizeof(*scod)+1;
 if(mod_iface.call(SCCM_GET_OTDDATA,(LPARAM)node->Data,(LPARAM)scod) == GKH_RET_SUCCESS)
 {
   DWORD diag;
   otd_proto op;
   op.dw_size = sizeof(op);
     if(otd_proto_parser(scod->data,scod->data_len,&op,&diag))
     {
        set_node_image(node,(op.diag) ? *op.diag :-1);
     }
 }
 else
 set_node_image(node,OTD_DIAG_NODATA);
}



void __fastcall TSCGranFrm::FlushConfigBtnClick(TObject *Sender)
{
 mod_iface.flush_config();
}
//---------------------------------------------------------------------------



void __fastcall TSCGranFrm::OnSysCommand(TMessage & msg)
{
        //TODO: Add your source code here
        switch(msg.WParam)
        {
         case SC_MINIMIZE:
                        if(this->sc_monitor)
                            sc_monitor->Hide();

         break;
         case SC_RESTORE :

                          PostMessage(Handle,MSG_SHOW_MONITOR,0,0);
         break;
        }
    this->DefaultHandler(&msg);
}

void __fastcall TSCGranFrm::OnMsgShowMonitor(TMessage & msg)
{
        //TODO: Add your source code here
       if(sc_monitor)
          sc_monitor->Show();
}
