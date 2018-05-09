//---------------------------------------------------------------------------
#pragma hdrstop
#include <vcl.h>
#include "dblowfrm.h"
#include "gktlmdbwnd.rh"
#include <modem_proto.h>

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "gkmodule_form"
#pragma link "about_common_frame"
#pragma resource "*.dfm"
TDBLowForm *DBLowForm;
//---------------------------------------------------------------------------
__fastcall TDBLowForm::TDBLowForm(TComponent* Owner,HWND parent,GKHANDLE handle)
        : TGKModuleForm(Owner,parent,handle)
{
 filter_addr.addr = -1;
 #ifdef _DEBUG
 Button1->Visible = true;
 Button2->Visible = true;
 Button3->Visible = true;
 #endif

 #ifdef COMERCIAL_BUILD
 //Label30->Caption = "ООО \"Компания промышленной автоматизации\"";
 #endif

}
//---------------------------------------------------------------------------
void        __fastcall TDBLowForm::setup_form_sizes()
{
   TGKModuleForm::setup_form_sizes();
   DbTree->Width = Width/4;
}

bool __fastcall is_static(sotd_proto & sop)
{
 bool ret = false;
 if(sop.op.extension)
 {
  dblow_ext * dbext = (dblow_ext *)sop.op.extension->block_data;
  if(dbext->is_static)
     ret = true;
 }
 return ret;
}

void        __fastcall TDBLowForm::WndProc(Messages::TMessage &Message)
{
 TGKModuleForm::WndProc(Message);
}

void     __fastcall TDBLowForm::on_module_state(GKHANDLE mod,DWORD state)
{
  bool running = ((state & MODULE_STATE_RUNNING_MASK )== MODULE_STATE_RUNNING) ? true:false;
  bool stopped  = (state & MODULE_STATE_RUNNING_MASK ) ? false:true;
  StartBtn->Enabled = stopped;
  StopBtn->Enabled  = running;
  if(running)
     StatusBar1->Panels->Items[0]->Text =  L"Работатет";
  if(stopped)
     StatusBar1->Panels->Items[0]->Text =  L"Останов";
}

void     __fastcall TDBLowForm::on_module_config_change(GKHANDLE mod,LPMODULE_CONFIG_DATA mcd)
{
}

LRESULT     __fastcall TDBLowForm::on_gkhandle_notify (GKHANDLE from,LPNOTIFY_CODE nc,DWORD mask)
{
 if(mask == TLM_DBLOW_NOTIFY_CHANGE)
  {
     on_dblow_change(nc->notify_code,(LPDBLOW_CHANGES_STRUCT)nc->notify_data);
     return GKH_RET_SUCCESS;
  }
 if(mask == TLMDB_DBLOW_MONITOR)
   {
     if(!MonPause->Checked)
     {
       monitor_handler  (nc->notify_code,(PTCHAR)nc->notify_data);
       
     }  
   }
  return TGKModuleForm::on_gkhandle_notify(from,nc,mask);
}

void        __fastcall TDBLowForm::monitor_handler(DWORD code,PTCHAR  text)
{
 if(code == TLMDB_DBLOW_MONITOR_CHANGES && MonChanges->Checked)
    monitor.add_line(text,clBlue);
 if((code == TLMDB_DBLOW_MONITOR_ORDER||code == TLMDB_DBLOW_MONITOR_RESPOND)  && MonCallOrder->Checked)
    monitor.add_line(text);
}

void     __fastcall TDBLowForm::after_set_gkhandle()
{
  TGKModuleForm::after_set_gkhandle();
  on_module_state(mod_iface(),mod_iface.get_module_state());
  setup_db_tree();
  set_notify(mod_iface(),TLM_DBLOW_NOTIFY_CHANGE|MNF_COMMON);
}



int __fastcall TDBLowForm::get_tlmmodule_rctext(lpotd_addr adr, int type,TCHAR * name,int bsz)
{
 int ret ;
 TCHAR buf[MAX_PATH];
         int str_id;
         switch(type)
         {
          case OTD_ADDR_TYPE_PU  : str_id = IDS_PU_TEXT;
          break;
          case OTD_ADDR_TYPE_CP  : str_id = IDS_CP_TEXT;
          break;
          case OTD_ADDR_TYPE_FA  : str_id = IDS_FADISCRETE_TEXT+((LPBYTE)adr)[type];
          break;
          case OTD_ADDR_TYPE_SB  : str_id = IDS_SB_DISCRETE_TEXT+((LPBYTE)adr)[type-1]; ;
          break;
         }
         ret = LoadString(ModuleInstance,str_id,buf,sizeof(buf));
         if(ret)
           ret = wsprintf(name,_T("%s №%03d"),buf,DWORD(((LPBYTE)adr)[type]));
     return ret;
}

UnicodeString __fastcall TDBLowForm::get_diag_text(DWORD diag,bool prn_val)
{
  UnicodeString s;
  DWORD mask = OTD_DIAG_NODATA;
  if(prn_val)
   s.printf(L"%04X ",(DWORD)HIWORD(diag));
  for(int i = 0;i<16;i++)
  {
   if(diag & mask)
   {
    s+= load_rcstring(ModuleInstance,STR_DIAG_NODATA+i);
    break;
   }
   mask>>=1;
  }
  //TRACE(s.c_str(),0);
  return s;
}

UnicodeString __fastcall TDBLowForm::get_part_diag_text(DWORD value,bool prn_val)
{
 UnicodeString s;
 if(prn_val)
  s.printf(L"%04X ",(value&(OTD_DIAG_PART_MASK|OTD_DIAG_STATE_MASK)));
 DWORD  mask = OTD_PART_DIAG_PARAM ;
  for(int i = 0;i<8;i++)
  {
   if(value & mask)
     {
      s+= load_rcstring(ModuleInstance,STR_PART_DIAG_PARAM+i);
      //break;
     }
    mask>>=1;
  }
 return s;
}

UnicodeString __fastcall TDBLowForm::get_state_text(DWORD value,bool prn_val)
{
 UnicodeString s;
 if(prn_val)
 s.printf(L"%04X ",(value&OTD_STATE_MASK));
 DWORD  mask = OTD_STATE_RESERV1;
  for(int i = 0;i<8;i++)
  {
   if(value & mask)
     {
      s+=load_rcstring(ModuleInstance,STR_STATE_CALL_SENDED+i);
      //break;
     }
    mask>>=1;
  }
 return s;
}



UnicodeString __fastcall TDBLowForm::get_pdiag_text(DWORD value,bool prn_val)
{
 UnicodeString s;
 if(prn_val)
  s.printf(L"%02X ",value);
 DWORD  mask = OTD_PDIAG_NODATA;
  for(int i = 0;i<8;i++)
  {
   if(value & mask)
     {
      int str_id = STR_PDIAG_NODATA+i;
      if(mask & (OTD_PDIAG_TUTR_DESCRIPT|OTD_PDIAG_TUTR_FAIL))
       {
        s+=load_rcstring(ModuleInstance,str_id);
       }
       else
       s+=load_rcstring(ModuleInstance,str_id);
     }
    mask>>=1;
  }
 return s;
}


void __fastcall TDBLowForm::SetDiagDword(DWORD value)
{
        //TODO: Add your source code here
        FDiagDword = value;
        char text[MAX_PATH];
        int     len;
        DWORD   mask;
        TColor  text_color ;

        FullDiagText->Caption     = get_diag_text(value);
        FullDiagText->Font->Color = (value&OTD_DIAG_MASK) ?  clRed:clDefault;
        PartDiagText->Caption = get_part_diag_text(value);
        PartDiagText->Font->Color = (value&OTD_DIAG_PART_MASK) ? (TColor)0x003797AF:clDefault;
        StateDiagText->Caption = get_state_text(value);
}

TTreeNode * __fastcall TDBLowForm::find_pu(DWORD addr)
{
 sotd_addr sa(addr);
 TTreeNode * ret =  db_root->getFirstChild();
 while(ret)
 {
  if(sotd_addr((DWORD)ret->Data).pu == sa.pu)
     break;
    ret = ret->getNextSibling();
 }
 return ret;
}



TTreeNode * __fastcall TDBLowForm::find_cp(DWORD addr,TTreeNode * parent )
{
 if(!parent) parent = find_pu(addr);
 TTreeNode * ret = NULL;
 if(parent && parent->Count)
 {
  sotd_addr sa(addr);
  ret = parent->getFirstChild();
  while(ret)
  {
  if(sotd_addr((DWORD)ret->Data).cp == sa.cp)
     break;
    ret = ret->getNextSibling();
  }
 }
 return ret;
}

TTreeNode * __fastcall TDBLowForm::find_sb(DWORD addr,TTreeNode * parent )
{
 if(!parent) parent = find_pu(addr);
 TTreeNode * ret = NULL;
 if(parent && parent->Count)
 {
  sotd_addr sa(addr);
  ret = parent->getFirstChild();
  while(ret)
  {
  sotd_addr sb_addr((DWORD)ret->Data);
  if(sb_addr.fa == sa.fa && sb_addr.sb == sa.sb)
     break;
    ret = ret->getNextSibling();
  }
 }
 return ret;
}

TTreeNode * __fastcall TDBLowForm::find   (DWORD addr)
{
  TTreeNode * ret ;
  if(otd_addr_type((lpotd_addr)&addr) == OTD_ADDR_TYPE_BROADCAST)
    ret = db_root;
  else
  {
  ret = find_pu(addr);
  if(ret && (DWORD)ret->Data!=addr)
  {
     ret = find_cp(addr,ret);
    if(ret && (DWORD)ret->Data!=addr)
       ret = find_sb(addr,ret);
  }
  }
  return ret;
}


TTreeNode * __fastcall TDBLowForm::add_pu (DWORD addr)
{
  sotd_addr sa(-1);
  sa.pu = sotd_addr(addr).pu;
  TTreeNode * node = this->find_pu(sa.addr);
  if(!node)
    {
      node = DbTree->Items->AddChild(db_root,L"");
      if(!db_root->Expanded)
          db_root->Expand(false);
      node->Data = (LPVOID)sa.addr;
      db_root->CustomSort(node_comparer,0,false);
      update_node_text(node);
    }

  return node;
}


TTreeNode * __fastcall TDBLowForm::add_cp (DWORD addr)
{
 TTreeNode * pu = add_pu(addr);
 if(pu)
   {
    sotd_addr sa(-1);
    sa.pu = sotd_addr(addr).pu;
    sa.cp = sotd_addr(addr).cp;
    TTreeNode * cp = find_cp(sa.addr,pu);
    if(!cp)
    {
      cp = DbTree->Items->AddChild(pu,L"");
      cp->Data = (LPVOID)sa.addr;
      pu->CustomSort(node_comparer,0,false);
      update_node_text(cp);
    }
    return cp;
   }
  return NULL;
}

TTreeNode * __fastcall TDBLowForm::add_sb (DWORD addr)
{
 TTreeNode * cp = add_cp(addr);
 if(cp)
   {
    sotd_addr sa(addr);
    TTreeNode * sb = find_sb(sa.addr,cp);
    if(!sb)
    {
     TCHAR str[MAX_PATH];
     get_tlmmodule_rctext(&sa,OTD_ADDR_TYPE_SB,str,sizeof(str));
     sb = DbTree->Items->AddChild(cp,UnicodeString(str));
     sb->Data = (LPVOID)sa.addr;
     cp->CustomSort(node_comparer,0,false);
     update_node_text(sb);
    }
    return sb;
   }
  return NULL;
}


TTreeNode * __fastcall TDBLowForm::add_tree_item(otd_proto & op)
{
 TTreeNode * node = NULL;
 if(otd_addr_include(op.addr,&filter_addr))
 {
 UnicodeString str;
 DWORD type = otd_addr_type(op.addr);
 switch(type)
 {
  case OTD_ADDR_TYPE_BROADCAST: node = db_root;break;
  case OTD_ADDR_TYPE_PU  : node = add_pu(op.addr->addr);break;
  case OTD_ADDR_TYPE_CP  : node = add_cp(op.addr->addr);break;
  case OTD_ADDR_TYPE_SB  : node = add_sb(op.addr->addr);break;
 }

 if(node && op.name)
 {
  wchar_t name[MAX_PATH];
  get_tlmmodule_rctext(op.addr,type,name,KERTL_ARRAY_COUNT(name));
  str = name;
  name[otd_text_getwstr(op.name,name,MAX_PATH)]=0;
  str+=' ';
  str+=name;
  node->Text = str;
 }

 if(op.diag)
   set_node_image(node,*op.diag);
   else
   update_node_image(node);
 }
 return node;
}



void __fastcall TDBLowForm::setup_db_tree()
{
 if(!db_root)
    {
     db_root       = DbTree->Items->Add(0,"Список ПУ");
     db_root->Data = (LPVOID)-1;
     db_root->ImageIndex    = 3;
     db_root->SelectedIndex = 3;
     db_root->StateIndex    = 3;
     db_root->OverlayIndex  = 3;
    }
 db_root->DeleteChildren();
 DbTree->Items->BeginUpdate();
 DWORD index  = 0;
 BYTE  buffer[2048];
 LPTLMDB_LOW_GET_ENTRY ge = (LPTLMDB_LOW_GET_ENTRY)buffer;
 ZeroMemory(ge,sizeof(*ge));
 ge->dw_size = sizeof(*ge);
 ge->data_size = sizeof(buffer)-sizeof(*ge)+sizeof(ge->data);;
 ge->parts|=OTD_PROTO_PART_NAME;
 while(mod_iface.call(TLMDBCM_DBLOW_ENUM_ENTRY,index,(LPARAM)ge)==GKH_RET_SUCCESS)
 {
  sotd_proto sop(ge->data,ge->data_len);
  add_tree_item(sop.op);
  ++index;
 }
  db_root->Expand(false);
  DbTree->Items->EndUpdate();
}

int         __fastcall TDBLowForm::get_dbentry_data(DWORD addr,DWORD parts,DWORD low,DWORD hi,LPBYTE buffer,DWORD bsz,sotd_proto & sop)
{
  int ret = -1;
  LPTLMDB_LOW_GET_ENTRY ge = (LPTLMDB_LOW_GET_ENTRY)buffer;
  ZeroMemory(ge,sizeof(*ge));
  ge->dw_size = sizeof(*ge);
  ge->data_size = bsz-sizeof(*ge)+sizeof(ge->data);
  ge->parts|=parts;
  ge->loN = low;
  ge->hiN = hi;
  if(mod_iface.call(TLMDBCM_DBLOW_GET_ENTRY,addr,(LPARAM)ge)!=GKH_RET_ERROR)
   {
     ret = ge->data_len;
     if(ret>0)
      sop = sotd_proto(ge->data,ret);
   }
   return ret;
}

void        __fastcall TDBLowForm::on_dblow_change(DWORD code,LPDBLOW_CHANGES_STRUCT dcs)
{
  /*Обработка изменений объекта*/
  sotd_addr sa(dcs->addr);

  TTreeNode * node = find(sa.addr);
  if(node && code == TLM_DBLOW_CODE_REMOVED)
  {
    if(node!=this->db_root)
       node->Delete();
  }
  else
  {

      if(!node && code!= TLM_DBLOW_CODE_REMOVED)
        {
         //TODO  добавить элемент в дерево
         BYTE buffer[4096];
         sotd_proto sop;
         if(get_dbentry_data(dcs->addr,OTD_PROTO_PART_NAME,0,-1,buffer,sizeof(buffer),sop)>0)
            node = add_tree_item(sop());
        }

     if(node  )
     {
       if(dcs->parts&(OTD_PROTO_PART_DIAG|OTD_PROTO_PART_PERSONAL_DIAG))
         {
           sotd_addr sa1((DWORD)node->Data);
           if(sa1.is_include(sa) || sa1.is_include(sa,true))
           {
           update_node_image(node);
           update_view(dcs->addr,dcs->parts|OTD_PROTO_PART_DIAG,dcs->loN,dcs->hiN,true);
           }
         }
       if(dcs->parts&OTD_PROTO_PART_NAME)
          update_node_text(node);
     }

       {
        node = this->DbTree->Selected;
        if(node &&  sotd_addr((DWORD)node->Data).is_include(sa))
         update_view(dcs->addr,dcs->parts|OTD_PROTO_PART_DIAG,dcs->loN,dcs->hiN,true);
       }
 }
}

void        __fastcall TDBLowForm::update_node_text (TTreeNode * node)
{
 if(node && node!=db_root)
 {
  sotd_addr sa((DWORD)node->Data);
  BYTE buffer[4096];
  get_tlmmodule_rctext(&sa,sa.addrtype(),(TCHAR*)buffer,sizeof(buffer)/sizeof(TCHAR));
  UnicodeString text = (TCHAR*)buffer;
  sotd_proto sop;
  if(get_dbentry_data(sa.addr,OTD_PROTO_PART_NAME,0,-1,buffer,sizeof(buffer),sop)>0)
  {
    if(sop().name)
     {
      wchar_t name[MAX_PATH];
      name[otd_text_getwstr(sop().name,name,sizeof(name)/sizeof(wchar_t))] =0;
      if(*name)
      {
       text+=' ';
       text+=name;
      } 
     }
  }
  node->Text = text;
  if(node->Selected)
   this->DbTreeChange(this,node);
 }
}


void __fastcall TDBLowForm::DbTreeChanging(TObject *Sender,
      TTreeNode *Node, bool &AllowChange)
{
  TTreeNode * node =   DbTree->Selected;
  if(node)
     show_param_panel(node,false);
}


void __fastcall TDBLowForm::DbTreeChange(TObject *Sender, TTreeNode *Node)
{
  apply_mask = 0;
  switch(PageControl1->TabIndex)
  {
   case    0: //Страница параметров
             show_param_panel(Node,true);
   break;
   case    1: //Страница просмотра
           update_view((DWORD) DbTree->Selected->Data,-1,0,-1,false);
   break;
   default  : break;
  }
 sotd_addr  addr((DWORD)Node->Data);
 DWORD type = addr.addrtype();
 UnicodeString str;
 if(type<OTD_ADDR_TYPE_BROADCAST)
 {
  modem_addr ma((DWORD)mod_iface.call(TLMDBCM_DBLOW_GET_SRCMODEM,addr.addr,0));

  str.printf(L"%03d.%03d.%03d.%03d <- %d.%d",(DWORD)addr.pu,(DWORD)addr.cp,(DWORD)addr.fa,(DWORD)addr.sb,(DWORD)ma.modem,(DWORD)ma.line);

 }
 StatusBar1->Panels->Items[0]->Text = str;
}

void        __fastcall TDBLowForm::show_param_panel(TTreeNode * node,bool show)
{
 sotd_addr  addr((DWORD)node->Data);
 DWORD type = addr.addrtype();
 TPanel * panel ;//= this->Settings_Panel;

 switch(type)
 {
  case OTD_ADDR_TYPE_PU : panel = pu_panel;if(show) setup_pu_panel(addr);break;
  case OTD_ADDR_TYPE_CP : panel = cp_panel;if(show) setup_cp_panel(addr);break;
  case OTD_ADDR_TYPE_SB : panel = sb_panel;if(show) setup_sb_panel(addr);break;
  default               : panel = Settings_Panel;if(show) this->setup_settings_panel();break;
 }

 if(show)
   {
    panel->Align = alClient;
    panel->Caption = UnicodeString();
    apply_mask = 0;
   }
   panel->Visible = show;
}

void        __fastcall TDBLowForm::setup_pu_panel(otd_addr & addr)
{
  BYTE buffer [4096];
  sotd_proto op;
  if(get_dbentry_data(addr.addr,OTD_PROTO_PART_NAME|OTD_PROTO_PART_PROTOEXTENSION,0,-1,buffer,sizeof(buffer),op)>0)
  {
    wchar_t pu_name[MAX_PATH];
    if(op.op.name)
     {
       otd_text_getwstr(op.op.name,pu_name,sizeof(pu_name)/sizeof(wchar_t));
     }
     else
     pu_name[0] = 0;
     PuName->Text = pu_name;
     PuStatic->Checked = is_static(op);
  }
}

void        __fastcall TDBLowForm::setup_cp_panel(otd_addr & addr)
{
  BYTE buffer [4096];
  sotd_proto op;
  if(get_dbentry_data(addr.addr,OTD_PROTO_PART_NAME|OTD_PROTO_PART_PROTOEXTENSION,0,-1,buffer,sizeof(buffer),op)>0)
  {
    wchar_t cp_name[MAX_PATH];
    if(op.op.name)
     {
       otd_text_getwstr(op.op.name,cp_name,sizeof(cp_name)/sizeof(wchar_t));
     }
     else
     cp_name[0] = 0;
     CpName->Text = cp_name;
     CpStatic->Checked = is_static(op);
  }
}

void        __fastcall TDBLowForm::setup_sb_panel(otd_addr & addr)
{
  BYTE buffer [4096];
  sotd_proto op;
  if(get_dbentry_data(addr.addr,OTD_PROTO_PART_DATA|OTD_PROTO_PART_PROTOEXTENSION,0,-1,buffer,sizeof(buffer),op)>0)
  {
   ModuleType->Caption = addr.fa ? "Аналоговый модуль":"Дискретный модуль";
   ModLoNum ->Text = op.op.data->numbers.loN;
   ModHiNum ->Text = op.op.data->numbers.hiN;
   ModStatic->Checked = is_static(op);
  }

}

void        __fastcall TDBLowForm::setup_settings_panel()
{
 BYTE buffer[sizeof(MODULE_CONFIG_DATA)+sizeof(TLMDB_SETTINGS)-1];
 LPMODULE_CONFIG_DATA mcd = (LPMODULE_CONFIG_DATA)buffer;
 mcd->dw_sz = sizeof(buffer);
 mcd->mask  = TLMDB_SMASK_DBF;
 LPTLMDB_SETTINGS settings =(LPTLMDB_SETTINGS)mcd->data;
 if(mod_iface.get_module_config_data(mcd,sizeof(buffer)))
 {
  DBFileName->Text = settings->db_file_name;

 }
}

void        __fastcall TDBLowForm::apply_settings()
{
 BYTE buffer[sizeof(MODULE_CONFIG_DATA)+sizeof(TLMDB_SETTINGS)-1];
 LPMODULE_CONFIG_DATA mcd = (LPMODULE_CONFIG_DATA)buffer;
 mcd->dw_sz = sizeof(buffer);
 mcd->mask  = TLMDB_SMASK_DBF;
 LPTLMDB_SETTINGS settings =(LPTLMDB_SETTINGS)mcd->data;
 WideString wstr = DBFileName->Text;
 safe_strcpyn(settings->db_file_name,wstr.c_bstr(),sizeof(settings->db_file_name)/sizeof(wchar_t));
 mod_iface.set_module_config_data(mcd,sizeof(buffer));
}



//2 - всё работает
//1 - частичная недостоверность
//0 - не работет

void        __fastcall TDBLowForm::set_node_image  (TTreeNode * node,DWORD diag)
{
 if(node != this->db_root)
 {
 DWORD idx = 1;
 DWORD old_idx = node->ImageIndex;
 if((diag&OTD_DIAG_FULL_MASK) == 0)
    idx = 2 ;
    else
     if(diag&OTD_DIAG_MASK)
        idx = 0;
 if(idx !=old_idx)
 {
  node->ImageIndex    = idx;
  node->StateIndex    = idx;
  node->SelectedIndex = idx;
  if(node->Parent && node->Parent!=db_root)
     update_node_image(node->Parent);

  if(node->Count && !idx)
   {
    node = node->getFirstChild();
    do{
       update_node_image(node);
       node = node->getNextSibling();
      }while(node);
   }

 }


 }
}

void        __fastcall TDBLowForm::update_node_image(TTreeNode * node)
{
 if(node)
 {
  if(node == db_root)
  {
   node = node->getFirstChild();
   while(node)
   {
    update_node_image(node);
    node = node->getNextSibling();
   }
  }
  else
  {
   BYTE buffer[1024];
   sotd_proto sop;
   get_dbentry_data((DWORD)node->Data,OTD_PROTO_PART_DIAG,0,0,buffer,sizeof(buffer),sop);
   set_node_image(node, sop.op.diag  ? *sop.op.diag:0);
  }

 }
 else
 DebugBreak();
}

TListItem  * __fastcall TDBLowForm::create_param_item(DWORD number)
{
   UnicodeString str;
   TListItem  * item = ListView1->Items->Add();
   item->Data     = (LPVOID)number;
   item->Caption  =  number;
   item->SubItems->Add(str);
   item->SubItems->Add(str);
   item->SubItems->Add(str);
   return item;
}

void __fastcall TDBLowForm::setup_view(sotd_proto & sop)
{
  DWORD lo = -1 ,hi = 0;
  if(sop.op.data)
  {
   lo = KeRTL::MIN(lo,(DWORD) sop.op.data->numbers.loN);
   hi = KeRTL::MAX(hi,(DWORD) sop.op.data->numbers.hiN);
  }

  if(sop.op.personal_diag)
  {
   lo = KeRTL::MIN(lo,(DWORD) sop.op.personal_diag->numbers.loN);
   hi = KeRTL::MAX(hi,(DWORD) sop.op.personal_diag->numbers.hiN);
  }

  if(sop.op.personal_chmask)
  {
   lo = KeRTL::MIN(lo,(DWORD) sop.op.personal_chmask->numbers.loN);
   hi = KeRTL::MAX(hi,(DWORD) sop.op.personal_chmask->numbers.hiN);
  }

  while(lo<=hi)
  {
   this->create_param_item(lo++);
  }
}

union param
{
 WORD   w_param;
 SHORT  sh_param;
 LONG   l_param;
 DWORD  dw_param;
 QWORD  qw_param;
 float  f_param;
 double d_param;
};

bool    __fastcall TDBLowForm::setup_param(lpotd_data data)
{
  if(data)
  {
    DWORD lo = data->numbers.loN;
    DWORD hi = data->numbers.hiN;
    while(lo<=hi)
    {
     TListItem * item = this->ListView1->FindData(0,(LPVOID)lo,true,false);
     if(!item) item = this->create_param_item(lo);
     param  p ; p.qw_param = 0;
     otd_get_value(data,lo,&p,sizeof(p));
     UnicodeString str;
     WORD type = data->otd_type;
     switch(type)
     {
      case OTD_DISCRETE         :
      case OTD_BYTE             :
      case OTD_ANALOG_WORD      : str.printf(L"%hu",(WORD)p.w_param);
      break;
      case OTD_ANALOG_SHORT     : str.printf(L"%hi",(WORD)p.sh_param);
      break;
      case OTD_ANALOG_DWORD     : str.printf(L"%lu",(DWORD)p.sh_param);
      break;
      case OTD_ANALOG_LONG      : str.printf(L"%ld",(DWORD)p.sh_param);
      break;
      case OTD_ANALOG_QWORD     : str.printf(L"%Lu",(QWORD)p.sh_param);
      break;
      case OTD_ANALOG_LONGLONG  : str.printf(L"%Ld",(QWORD)p.sh_param);
      break;
      case OTD_SIGNED_FLOAT     :
      case OTD_FLOAT            : str.printf(L"%02f",(float)p.f_param);
      break;
      case OTD_SIGNED_DOUBLE    :
      case OTD_DOUBLE           : str.printf(L"%02f",(float)p.d_param);
      break;
      default :
         str.printf(L"?? type %02X",(DWORD)type);
      break;

     }
      item->SubItems->Strings[0] = str;

     if(item->Selected)    calc_value();
     lo++;
    }
   return lo<hi ? false : true;
  }
  return true;
}

void        __fastcall TDBLowForm::setup_pd   (lpotd_data data)
{
  if(data)
  {
    DWORD lo = data->numbers.loN;
    DWORD hi = data->numbers.hiN;
    TListItem * item = this->ListView1->FindData(0,(LPVOID)lo,true,false);
    UnicodeString str;
    while(item && lo<=hi)
    {
     DWORD param = 0;
     otd_get_value(data,lo,&param,sizeof(param));
     str.printf(L"%02X",param);
     item->SubItems->Strings[1] = str;
     if(item->Selected)
       this->ListView1SelectItem(this,item,true);
     item = ListView1->Items->Item[item->Index+1];
     lo++;
    }
 }
}

void        __fastcall TDBLowForm::setup_cp_pd   (lpotd_data data)
{
  if(data)
  {
    DWORD lo = data->numbers.loN;
    DWORD hi = data->numbers.hiN;
    TListItem * item = this->ListView1->FindData(0,(LPVOID)lo,true,false);
    UnicodeString str;
    while(item && lo<=hi)
    {
     otd_group_pd_entry param = {0};
     DWORD sz = sizeof(param);
     otd_get_value(data,lo,&param,sz);
     str.printf(L"%08X",param.diag);
     item->SubItems->Strings[1] = str;

     sotd_addr sa(param.addr);
     str.printf(L"%s № %d",sa.fa ? L"ТИ":L"ТС",(DWORD)sa.sb);
     item->SubItems->Strings[0] = str;
     if(item->Selected)
       this->ListView1SelectItem(this,item,true);
     item = ListView1->Items->Item[item->Index+1];
     lo++;
    }
 }
}

void        __fastcall TDBLowForm::setup_pu_pd   (lpotd_data data)
{
  if(data)
  {
    DWORD lo = data->numbers.loN;
    DWORD hi = data->numbers.hiN;
    TListItem * item = this->ListView1->FindData(0,(LPVOID)lo,true,false);
    UnicodeString str;
    while(item && lo<=hi)
    {
     otd_group_pd_entry param = {0};
     otd_get_value(data,lo,&param,sizeof(param));
     str.printf(L"%08X",param.diag);
     item->SubItems->Strings[1] = str;
     sotd_addr sa(param.addr);
     str.printf(L"КП № %d",(DWORD)sa.cp);
     item->SubItems->Strings[0] = str;
     item = ListView1->Items->Item[item->Index+1];
     lo++;
    }
 }
}


void        __fastcall TDBLowForm::setup_cm   (lpotd_data data)
{
 if(data)
 {

    TListItem * item = this->ListView1->Items->Item[0];
    UnicodeString str;
    while(item )
    {
     DWORD param = 0;
     DWORD index = (DWORD)item->Data;
     if(otd_data_inrange(data,index))
     {
      otd_get_value(data,index,&param,sizeof(param));
      str.printf(L"%d",param);
     }
     else
     str = 0;
      item->SubItems->Strings[2] = str;
      item = ListView1->Items->Item[item->Index+1];
    }
  }
}



void        __fastcall TDBLowForm::setup_timestamp(LPFILETIME ftm)
{
      TCHAR date_time[MAX_PATH];
      if(ftm)
      {
      SYSTEMTIME st;
      FileTimeToSystemTime((LPFILETIME)ftm,&st);
      int dtl = GetDateFormat(LOCALE_USER_DEFAULT,DATE_SHORTDATE,&st,NULL,date_time,sizeof(date_time));
      dtl--;
      *(date_time+dtl) = ' ';
      dtl++;
      dtl+=GetTimeFormat(LOCALE_USER_DEFAULT,0,&st,_T("HH':'mm':'ss"),date_time+dtl,sizeof(date_time)-dtl);
      dtl--;
      wsprintf(date_time+dtl,_T(",%03d"),(DWORD)st.wMilliseconds);
      }
      else
      *date_time = 0;
      TimeStamp->Caption = date_time;
}

void        __fastcall TDBLowForm::setup_diag (LPDWORD diag)
{
 if(diag)
  DiagDword = *diag;
}

void        __fastcall TDBLowForm::update_view(DWORD addr,DWORD parts,DWORD low,DWORD hi,bool upd)
{
 ListView1->Items->BeginUpdate();
 if(!upd)
   {
    ListView1->Clear();
    PDiagText->Caption = UnicodeString();
    low = 0;
    hi  = -1;
   }
  if(DbTree->Selected)
  {
   sotd_addr sa((DWORD)DbTree->Selected->Data);
   if(sa.addr!=addr)
     parts = OTD_PROTO_PART_DIAG|OTD_PROTO_PART_PERSONAL_DIAG;
   BYTE buffer[8192];
   sotd_proto  sop;
   if(get_dbentry_data(sa.addr,parts,low,hi,buffer,sizeof(buffer),sop)>0)
   {
     if(!upd  )
       setup_view(sop);
       setup_param(sop.op.data);
       setup_diag(sop.op.diag);
       if(!upd || sop.op.time_stamp)
          setup_timestamp((LPFILETIME)sop.op.time_stamp);
       DWORD addr_type = sa.addrtype();
       GroupBox2->Visible = addr_type == OTD_ADDR_TYPE_SB ? true:false;
       switch(addr_type)
       {
        case  OTD_ADDR_TYPE_SB:  setup_pd   (sop.op.personal_diag);
                                 setup_cm   (sop.op.personal_chmask);
                                 break;
        case  OTD_ADDR_TYPE_CP:
                                 setup_cp_pd   (sop.op.personal_diag);
                                 break;
        case  OTD_ADDR_TYPE_PU:
                                 setup_pu_pd   (sop.op.personal_diag);
                                 break;
       }


   }
  }
 ListView1->Items->EndUpdate();
}
//---------------------------------------------------------------------------






void __fastcall TDBLowForm::RefreshAddrBtnClick(TObject *Sender)
{
 if(DbTree->Selected)
   mod_iface.call(TLMDBCM_DBLOW_REFRESH_ADDR,(LPARAM)DbTree->Selected->Data,TRUE);
}
//---------------------------------------------------------------------------



void __fastcall TDBLowForm::FilterTextChange(TObject *Sender)
{
  filter_addr.addr  = -1;
  str2addr(FilterText->Text.c_str(),(LPBYTE)&filter_addr.addr,sizeof(filter_addr),'.');
  setup_db_tree();
}
//---------------------------------------------------------------------------


void __fastcall TDBLowForm::OnWMGetMinmaxInfo(TMessage & msg)
{
        //TODO: Add your source code here
        //LPMINMAXINFO  mmi = (LPMINMAXINFO)msg.LParam;
}

void __fastcall TDBLowForm::OnWMMoving(TMessage & msg)
{
        //TODO: Add your source code here
        //LPRECT r = (LPRECT)msg.LParam;
}

void __fastcall TDBLowForm::PageControl1Change(TObject *Sender)
{
 if(DbTree->Selected)
 {
  sotd_addr addr ((DWORD)DbTree->Selected->Data);
  switch(PageControl1->ActivePageIndex)
  {
   case 0: show_param_panel(DbTree->Selected,true);;break;
   case 1: update_view(addr.addr,-1,0,-1,false); break;
   case 2: activate_monitor(true);break;
   case 3: enum_orders();break;
  }
 }
}
//---------------------------------------------------------------------------




void __fastcall TDBLowForm::StartBtnClick(TObject *Sender)
{
  mod_iface.start(MODULE_START_REQUEST,0);
}
//---------------------------------------------------------------------------

void __fastcall TDBLowForm::StopBtnClick(TObject *Sender)
{
  mod_iface.stop(MODULE_STOP_REQUEST);
}
//---------------------------------------------------------------------------

void __fastcall TDBLowForm::DbTreeCollapsing(TObject *Sender,
      TTreeNode *Node, bool &AllowCollapse)
{
 if(Node == db_root)
   AllowCollapse = false;       
}
//---------------------------------------------------------------------------




void __fastcall TDBLowForm::ListView1SelectItem(TObject *Sender,
      TListItem *Item, bool Selected)
{
  UnicodeString text;
  if(Selected)
  {
  sotd_addr sa ((DWORD)DbTree->Selected->Data);
  DWORD pdiag = hextoi(Item->SubItems->Strings[1].c_str());

  if(sa.addrtype() == OTD_ADDR_TYPE_SB)
   {
    text = get_pdiag_text(pdiag,false)+' '+get_state_text(pdiag,false);
    calc_value();
   }
   else
   {
    text = get_diag_text(pdiag,false)+' '+get_part_diag_text(pdiag,false)+' '+get_state_text(pdiag,false);
   }

  }
  PDiagText->Caption = text;

}
//---------------------------------------------------------------------------


void __fastcall TDBLowForm::ToolButton2Click(TObject *Sender)
{
 mod_iface.call(TLMDCMD_LOWDB_REMOVE_INVALID_DINAMIC,(DWORD)DbTree->Selected->Data,0);
}
//---------------------------------------------------------------------------



void __fastcall TDBLowForm::activate_monitor(bool active)
{
 if(active)
 {
  if(!monitor())
  {
   KeWin::TRect r(0,0,MonitorBox->Width-14,MonitorBox->Height-14);
   r.Move(6,12);
   monitor.DoCreate(MonitorBox->Handle,r,33333,8192,L"DbLowMon");
   monitor.SetFont(MonitorBox->Font->Handle,true);

  }
  set_notify(mod_iface(),TLMDB_DBLOW_MONITOR);
 }
 else
  reset_notify(mod_iface(),TLMDB_DBLOW_MONITOR);

 monitor.ShowWindow( active ? SW_SHOW:SW_HIDE);
}

void __fastcall TDBLowForm::PageControl1Resize(TObject *Sender)
{
  monitor.SetSize(MonitorBox->Width-14,MonitorBox->Height-14);
}
//---------------------------------------------------------------------------


void __fastcall TDBLowForm::PageControl1Changing(TObject *Sender,
      bool &AllowChange)
{
 if(PageControl1->ActivePageIndex == 2)
    activate_monitor(false);        
}
//---------------------------------------------------------------------------

void __fastcall TDBLowForm::Splitter1Moved(TObject *Sender)
{
  StatusBar1->Panels->Items[0]->Width = DbTree->Width;      
}
//---------------------------------------------------------------------------


void __fastcall TDBLowForm::ClearMonClick(TObject *Sender)
{
  monitor.clear();
}
//---------------------------------------------------------------------------
void        __fastcall TDBLowForm::calc_value()
{

 TListItem * item = this->ListView1->Selected;
 if(item)
  {
   UnicodeString str;

   float  kvants = (DWORD)abs(atof(Kvants->Text.c_str()));
   if(kvants)
   {
   float  param  = atof(item->SubItems->Strings[0].c_str());
   float  beg_s  = atof(this->ScaleBeg->Text.c_str());
   float  end_s  = atof(this->ScaleEnd->Text.c_str());
   float  res    = beg_s + param * (end_s - beg_s)/float(kvants);
   str.printf(L"%.3f",res);
   }
   Result->Text = str;
  }
}

void __fastcall TDBLowForm::ScaleBegChange(TObject *Sender)
{
  calc_value();
}
//---------------------------------------------------------------------------


void __fastcall TDBLowForm::Button3Click(TObject *Sender)
{
  sotd_addr addr((DWORD)DbTree->Selected->Data);
  TListItem * item = ListView1->Selected;
  if(addr.addrtype() == OTD_ADDR_TYPE_SB && item)
  {
   mod_iface.call(TLMDBCM_DBLOW_TUUNDO ,addr.addr,(DWORD)item->Data);
  }

}
//---------------------------------------------------------------------------
void __fastcall TDBLowForm::Button1Click(TObject *Sender)
{

  sotd_addr addr((DWORD)DbTree->Selected->Data);
  TListItem * item = ListView1->Selected;
  if(addr.addrtype() == OTD_ADDR_TYPE_SB && item)
  {
   mod_iface.call(TLMDBCM_DBLOW_TUON ,addr.addr,(DWORD)item->Data);
  }

}
//---------------------------------------------------------------------------

void __fastcall TDBLowForm::Button2Click(TObject *Sender)
{
  sotd_addr addr((DWORD)DbTree->Selected->Data);
  TListItem * item = ListView1->Selected;
  if(addr.addrtype() == OTD_ADDR_TYPE_SB && item)
  {
   mod_iface.call(TLMDBCM_DBLOW_TUOFF ,addr.addr,(DWORD)item->Data);
  }
}
//---------------------------------------------------------------------------

void __fastcall TDBLowForm::ApplyBtnClick(TObject *Sender)
{
 sotd_addr sa((DWORD)DbTree->Selected->Data);
 switch(sa.addrtype())
   {
    case OTD_ADDR_TYPE_SB: apply_sb(sa);break;
    case OTD_ADDR_TYPE_CP: apply_cp(sa);break;
    case OTD_ADDR_TYPE_PU: apply_pu(sa);break;
    case OTD_ADDR_TYPE_BROADCAST: apply_settings();break;
   }
}
//---------------------------------------------------------------------------

void        __fastcall TDBLowForm::apply_sb(otd_addr & addr)
{
 TLMDB_LOW_RECREATE rcs;
 
 ZeroMemory(&rcs,sizeof(rcs));
 rcs.dw_size = sizeof(rcs);
 rcs.lo_obj  = atoi(ModLoNum->Text.c_str());
 rcs.hi_obj  = atoi(ModHiNum->Text.c_str());
 mod_iface.call(TLMDBCM_DBLOW_RECREATE,addr.addr,(LPARAM)&rcs);
 if(apply_mask & 0x8)
   {
    //сбросить признак динамического
    mod_iface.call(TLMDBCM_DBLOW_MAKE_STATIC,addr.addr,ModStatic->Checked ? TRUE:FALSE);
   }
 apply_mask = 0;
}

void        __fastcall TDBLowForm::apply_cp(otd_addr & addr)
{
 TLMDB_LOW_RECREATE rcs;
 ZeroMemory(&rcs,sizeof(rcs));
 rcs.dw_size = sizeof(rcs);
 WideString wstr =  this->CpName->Text;
 safe_strcpyn(rcs.name,wstr.c_bstr(),sizeof(rcs.name)/sizeof(wchar_t));
 mod_iface.call(TLMDBCM_DBLOW_RECREATE,addr.addr,(LPARAM)&rcs);
 if(apply_mask & 0x8)
   {
    //сбросить признак динамического
    mod_iface.call(TLMDBCM_DBLOW_MAKE_STATIC,addr.addr,CpStatic->Checked ? TRUE:FALSE);
   }

 apply_mask = 0;
}

void        __fastcall TDBLowForm::apply_pu(otd_addr & addr)
{
 TLMDB_LOW_RECREATE rcs;
 ZeroMemory(&rcs,sizeof(rcs));
 rcs.dw_size = sizeof(rcs);
 WideString wstr =  PuName->Text;
 safe_strcpyn(rcs.name,wstr.c_bstr(),sizeof(rcs.name)/sizeof(wchar_t));
 mod_iface.call(TLMDBCM_DBLOW_RECREATE,addr.addr,(LPARAM)&rcs);
 if(apply_mask & 0x8)
   {
    //сбросить признак динамического
    mod_iface.call(TLMDBCM_DBLOW_MAKE_STATIC,addr.addr,PuStatic->Checked ? TRUE:FALSE);
   }

 apply_mask = 0;
}


void __fastcall TDBLowForm::ModLoNumChange(TObject *Sender)
{
 apply_mask|=4;
}
//---------------------------------------------------------------------------


void __fastcall TDBLowForm::Setapply_mask(DWORD value)
{
        //TODO: Add your source code here
        if(Fapply_mask!=value)
        {
         Fapply_mask=value;
        }
        ApplyBtn->Enabled = apply_mask ? true:false;
        UndoBtn->Enabled = apply_mask ? true:false;
}
void __fastcall TDBLowForm::UndoBtnClick(TObject *Sender)
{
  this->DbTreeChange(this,DbTree->Selected);
}
//---------------------------------------------------------------------------

void __fastcall TDBLowForm::PuNameChange(TObject *Sender)
{
  apply_mask |= 1;
}
//---------------------------------------------------------------------------

void __fastcall TDBLowForm::CpNameChange(TObject *Sender)
{
 apply_mask |= 1;
}
//---------------------------------------------------------------------------

void __fastcall TDBLowForm::FormKeyPress(TObject *Sender, char &Key)
{
 if(Key == 0x0D && apply_mask)
   ApplyBtnClick(this);
 if(Key == VK_NEXT)
   DbTree->Selected = DbTree->Selected->GetNext();
}
//---------------------------------------------------------------------------

void __fastcall TDBLowForm::SpeedButton1Click(TObject *Sender)
{
  if(OpenDialog1->Execute())
   {
    DBFileName->Text = OpenDialog1->FileName;
   }
}
//---------------------------------------------------------------------------

void __fastcall TDBLowForm::DBFileNameChange(TObject *Sender)
{
 apply_mask |=2;
}
//---------------------------------------------------------------------------

void __fastcall TDBLowForm::CpStaticClick(TObject *Sender)
{
   apply_mask |= 8;
}
//---------------------------------------------------------------------------

void __fastcall TDBLowForm::MiAboutClick(TObject *Sender)
{
  PageControl1->ActivePage = AboutSheet;
}
//---------------------------------------------------------------------------

void __fastcall TDBLowForm::enum_orders()
{
  LRESULT ret = RET_ORDERS_SUCCESS;
  orders_list->Items->BeginUpdate() ;
  orders_list->Clear();
  TLMDB_ORDER_ENTRY entry;
  entry.dw_size = sizeof(entry);

  for(WORD m_idx = 0;ret != GKH_RET_ERROR && ret < RET_ORDERS_NO_MORE_MODEMS;m_idx++ )
  {
    ret = RET_ORDERS_SUCCESS;
    for(WORD o_idx = 0 ; ret == RET_ORDERS_SUCCESS;o_idx++)
    {
     ret = mod_iface.call(TLMDBCM_ENUM_ORDERS,MAKELONG(o_idx,m_idx),(LPARAM)&entry);
     if(ret == RET_ORDERS_SUCCESS)
     {
      TListItem * item = orders_list->Items->Add();
      UnicodeString s;
      s.printf(L"%d.%d.%d",(DWORD)entry.addr.sta,(DWORD)entry.addr.modem,(DWORD)entry.addr.line);
      item->Caption = s;
      s.printf(L"%d.%d.%d.%d",(DWORD)entry.ordered_addr.pu,(DWORD)entry.ordered_addr.cp,(DWORD)entry.ordered_addr.fa,(DWORD)entry.ordered_addr.sb);
      item->SubItems->Add(s);
      s.printf(L"%04X",entry.ordered_parts);
      item->SubItems->Add(s);
     }
    }

  }
  orders_list->Items->EndUpdate() ;
}


