//---------------------------------------------------------------------------

#pragma hdrstop
#include <vcl.h>
#include "mediumdb_frm.h"
#include "inpaddtmask.h"
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>
#include <set>
#include "import_csv.h"
#include "entry_props.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "gkmodule_form"
#pragma link "record_frame"
#pragma link "about_common_frame"
#pragma resource "*.dfm"
TMediumDBFrm *MediumDBFrm;

TKadrsEditor     * TMediumDBFrm::kadrs_editor = NULL;

//---------------------------------------------------------------------------
__fastcall TMediumDBFrm::TMediumDBFrm(TComponent* Owner,HWND wnd,GKHANDLE handle)
        : TGKModuleForm(Owner,wnd,handle)
{
 db_root          = NULL;
 db_root_combine  = NULL;
 comb_frm         = NULL;

 OrdersSheet->TabVisible = false;
 AboutSheet->TabVisible  = false;

 Fcreate_mode = false;
 DoubleBuffered = true;
 current_states = NULL;
 Lmb =  Rmb = false;

 #ifdef COMERCIAL_BUILD
 //Label30->Caption = "ООО \"Компания промышленной автоматизации\"";
 #endif
 
}
//---------------------------------------------------------------------------
void __fastcall TMediumDBFrm::BeforeDestruction()
{
  if(current_states)
     {
      delete current_states;
      current_states = NULL;
     }
 TGKModuleForm::BeforeDestruction();
}


void     __fastcall TMediumDBFrm::on_module_config_change(GKHANDLE mod,LPMODULE_CONFIG_DATA mcd)
{
}

void     __fastcall TMediumDBFrm::on_module_state(GKHANDLE mod,DWORD state)
{
  if(mod == mod_iface())
  {
  bool running = ((state & MODULE_STATE_RUNNING_MASK )== MODULE_STATE_RUNNING) ? true:false;
  bool stopped  = (state & MODULE_STATE_RUNNING_MASK ) ? false:true;
  StartBtn->Enabled = stopped;
  miStart->Enabled  = stopped;
  StopBtn->Enabled  = running;
  miStop->Enabled   = running;
  if(running)
     StatusBar1->Panels->Items[0]->Text =  "Работатет";
  if(stopped)
     StatusBar1->Panels->Items[0]->Text =  "Останов";
  }   
}

void     __fastcall TMediumDBFrm::after_set_gkhandle()
{
  TGKModuleForm::after_set_gkhandle();
  RecordFrame1->Align = alClient;
  RecordFrame1->gk_handle = mod_iface();
  LPMODULE_CONFIG_DATA mcd;

  BYTE buffer[sizeof(MODULE_CONFIG_DATA)-sizeof(mcd->data[0])+sizeof(MDB_SETTINGS)];
  mcd =(LPMODULE_CONFIG_DATA)buffer;
  mcd->dw_sz = sizeof(buffer);
  mcd->mask  = -1;
  LPMDB_SETTINGS _settings = (LPMDB_SETTINGS)mcd->data;
  mod_iface.get_module_config_data(mcd,sizeof(buffer));
  memcpy(&settings,_settings,sizeof(settings));
  EditMdbPath->Text = settings.mdb_path;

  on_module_state(mod_iface(),mod_iface.get_module_state());
  set_notify(mod_iface(),MNF_COMMON|MDB_NOTIFY_MASK_RECORD);
}

LRESULT  __fastcall TMediumDBFrm::on_gkhandle_notify (GKHANDLE from,LPNOTIFY_CODE nc,DWORD mask)
{
// AnsiString trace_str;
 if(mask == MDB_NOTIFY_MASK_PAINTVAR)
    return  on_notify_paintvar(mod_iface,nc);
 if(mask == MDB_NOTIFY_MASK_RECORD)
 {
  switch(nc->notify_code)
  {
   case
   MDB_NOTIFY_CODE_RECORD_REMOVED :
//   trace_str.printf("MDB_NOTIFY_CODE_RECORD_REMOVED %d",nc->notify_data[0]);
   on_record_removed(nc->notify_data[0]);
   break;
   case
   MDB_NOTIFY_CODE_RECORD_CREATED :
//   trace_str.printf("MDB_NOTIFY_CODE_RECORD_CREATED %d",nc->notify_data[0]);
   on_record_created(nc->notify_data[0]);
   break;
   case
   MDB_NOTIFY_CODE_RECORD_CHANGED :
      {
       LPMDB_RECORD_CHANGED rc =(LPMDB_RECORD_CHANGED)nc->notify_data;
//       trace_str.printf("MDB_NOTIFY_CODE_RECORD_CHANGED rec_id %d mask %X ",rc->rec_id,rc->mask);
       on_record_changed(rc->rec_id,rc->mask);
      }
   break;
   case MDB_NOTIFY_CODE_DBREOPEN:
        if(create_mode    ) DeleteBtnClick(this);
        if(db_root        ) mdb_tree_records_enable (true);
        if(db_root_combine) mdb_tree_combines_enable(true);
        if(kadrs_editor   ) kadrs_editor->reload();
        break;
  }
 }
 else
 TGKModuleForm::on_gkhandle_notify(from,nc,mask);
// if(!trace_str.IsEmpty())
//    TRACE(trace_str.c_str(),0);
 return GKH_RET_SUCCESS;
}

void     __fastcall TMediumDBFrm::on_record_removed(DWORD id)
{
  TTreeNode * node = find_by_id(db_root,id);
  if(node)
     node->Delete();
  node = find_by_id(db_root_combine,id);   
  if(node)
     node->Delete();
}

#pragma warn -8004
void     __fastcall TMediumDBFrm::on_record_created(DWORD id)
{
  mdb_record rec ;
  get_record(id,rec);
  //this->MdbTree->Items->BeginUpdate();
  if(rec.options & MDBR_OPT_COMBINED)
    {
      if(db_root_combine && db_root_combine->Expanded)
        {
         if(!find_by_id(db_root_combine,id))
             add_node(db_root_combine,rec);
        }
      //else
      //db_root_combine->Expanded = true;
    }

  if(rec.addr.addr.pu<OTD_ADDR_MAXVALUE && db_root)
  {
    if(!db_root->Expanded)
        db_root->Expand(false);
    TTreeNode * pu = find_pu(rec.addr);
    TTreeNode * cp = 0,*sb = 0,*obj = 0;
    if(!pu)
      {
       mdb_record pu_rec;
       pu_rec.addr.addr.pu = rec.addr.addr.pu;
       get_record(-1,pu_rec);
       pu = add_node(db_root,pu_rec);
      }
    if(rec.addr.addr.cp<OTD_ADDR_MAXVALUE && pu && pu->Expanded)
    {
      cp = find_cp(pu,rec.addr);
      if(!cp)
      {
       mdb_record cp_rec;
       cp_rec.addr.addr.pu = rec.addr.addr.pu;
       cp_rec.addr.addr.cp = rec.addr.addr.cp;
       get_record(-1,cp_rec);
       cp = add_node(pu,cp_rec);
      }
      if(rec.addr.addr.sb<OTD_ADDR_MAXVALUE && cp && cp->Expanded)
      {
          sb = find_sb(cp,rec.addr);
          if(!sb)
          {
           mdb_record sb_rec;
           sb_rec.addr.addr.addr = rec.addr.addr.addr;
           get_record(-1,sb_rec);
           sb = add_node(cp,sb_rec);
          }
        if(rec.addr.param_number!=(DWORD)-1 && sb &&sb->Expanded)
        {
          obj = find_obj(sb,rec.addr);
          if(!obj)
          {
           obj = add_node(sb,rec);
           obj = obj;
          }
        }
      }
    }

  }
   //MdbTree->Items->EndUpdate();
}

#pragma warn .8004


TTreeNode * __fastcall TMediumDBFrm::find_by_id(TTreeNode * root,DWORD id,bool make_visible )
{
  TTreeNode * ret = NULL;
  if(root)
  {
  bool save_expanded = root->Expanded;
  if(!save_expanded && root->Count && make_visible)
     root->Expand(false);

  if(root->Expanded && root->Count)
   {
     TTreeNode * child = root->getFirstChild();
     while(child &&!ret)
      {
       if((DWORD)child->Data == id)
          ret = child;
          else
          ret = find_by_id(child,id,make_visible);
          child = child->getNextSibling();
      }
//      if(!ret && !save_expanded)
//          root->Collapse(true);

   }
  }
 return ret;
}

TTreeNode * __fastcall TMediumDBFrm::find_by_addr(otd_param_addr & addr,bool make_visible )
{

 TTreeNode * ret = find_pu(addr);
 if(ret &&  addr.addr.cp<OTD_ADDR_MAXVALUE)
   {
    if(make_visible && !ret->Expanded)
       ret->Expand(false);
    ret = find_cp(ret,addr);
    if(ret && addr.addr.sb<OTD_ADDR_MAXVALUE)
       {
        if(make_visible && !ret->Expanded)
           ret->Expand(false);
           ret = find_sb(ret,addr);
       }
    if(ret && addr.param_number!=(DWORD)-1)
      {
        if(make_visible && !ret->Expanded)
           ret->Expand(false);
       ret = find_obj(ret,addr);
      }
   }
 return ret;

}

TTreeNode * __fastcall TMediumDBFrm::find_pu     (otd_param_addr & addr)
{
 TTreeNode * ret = NULL;
 if(db_root)
 {
     TTreeNode * child = db_root->getFirstChild();
     while(child && !ret)
     {
       mdb_record rec;
       get_node_record(child,rec);
       if(rec.addr.addr.pu == addr.addr.pu)
         ret = child;
       child = child->getNextSibling();
     }
 }
 return ret;
}

TTreeNode * __fastcall TMediumDBFrm::find_cp     (TTreeNode * root,otd_param_addr & addr)
{
  TTreeNode * ret = NULL;
  if(!root)
    root = find_pu(addr);
  if(root && root->Expanded)
  {
   TTreeNode * child = root->getFirstChild();
   while(child && !ret)
   {
    mdb_record rec;
    get_node_record(child,rec);
    if(rec.addr.addr.cp == addr.addr.cp)
       ret = child;
    child = child->getNextSibling();
   }
  }
  return ret;
}

TTreeNode * __fastcall TMediumDBFrm::find_sb     (TTreeNode * root,otd_param_addr & addr)
{
  TTreeNode * ret = NULL;
  if(!root)
    root = find_cp(NULL,addr);
  if(root && root->Expanded)
  {
   TTreeNode * child = root->getFirstChild();
   while(child && !ret)
   {
    mdb_record rec;
    get_node_record(child,rec);
    if(rec.addr.addr.fa == addr.addr.fa && rec.addr.addr.sb == addr.addr.sb)
       ret = child;
    child = child->getNextSibling();
   }
  }
  return ret;
}

TTreeNode * __fastcall TMediumDBFrm::find_obj    (TTreeNode * root,otd_param_addr & addr)
{
  TTreeNode * ret = NULL;
  if(!root)
    root = find_sb(NULL,addr);
  if(root && root->Expanded)
  {
   TTreeNode * child = root->getFirstChild();
   while(child && !ret)
   {
    mdb_record rec;
    get_node_record(child,rec);
    if(rec.addr.param_number == addr.param_number)
       ret = child;
    child = child->getNextSibling();
   }
  }
  return ret;
}





void     __fastcall TMediumDBFrm::on_record_changed(DWORD id,DWORD mask)
{
 TTreeNode * node = MdbTree->Selected;
 if(node  && (DWORD)node->Data == id )
   {
     DWORD save_apply_mask = apply_mask;
     RecordFrame1->update_record(id,mask);
     apply_mask = save_apply_mask;
   }
   else
   node = NULL;

    if(mask & (MDBR_FIELD_SNAME|MDBR_FIELD_NAME))
    {
    mdb_record rec;
    get_record(id,rec);
    if(!node)
       node = this->find_by_id(db_root,id);
    if(node)
      set_node_text(node,rec);
    node = this->find_by_id(db_root_combine,id);
    if(node)
      set_node_text(node,rec);
    }
   
}




sotd_param_addr
         __fastcall TMediumDBFrm::get_node_addr  (TTreeNode * node)
{
 mdb_record rec;
 if(get_node_record(node,rec))
    return rec.addr;
 return sotd_param_addr();   
}

bool     __fastcall TMediumDBFrm::get_node_record(TTreeNode * node, mdb_record & rec)
{
 return get_record((DWORD)node->Data,rec);
}


bool     __fastcall TMediumDBFrm::get_record(TGKModuleInterface & mod_iface,DWORD id, mdb_record & rec)
{
 return mod_iface.call(MDBCM_RECORD_GET,id,(LPARAM)&rec)!=GKH_RET_ERROR ? true:false;
}

bool     __fastcall TMediumDBFrm::get_record(TGKModuleInterface & mod_iface,sotd_param_addr & pa, mdb_record & rec)
{
  rec.addr = pa;
  return get_record(mod_iface,-1,rec);
}


void     __fastcall TMediumDBFrm::mdb_tree_records_enable (bool value)
{
   OrdersSheet->TabVisible = value;

  if(value)
  {
    if(!db_root)
    {
        db_root          = MdbTree->Items->AddFirst(NULL,"Список Объектов");
        db_root->Data    = (LPVOID)-1;
    }

       db_root->DeleteChildren();
       TTreeNode * item = MdbTree->Items->AddChild(db_root,"");
       item->Data = (LPVOID)-1;
       db_root->Expand(false);
  }
  else
  {
   if(db_root)
     db_root->Delete() ;
     db_root = NULL;
  }

  OrdersSheet->TabVisible = value;
  AboutSheet->TabVisible  = value;

}

void     __fastcall TMediumDBFrm::mdb_tree_combines_enable(bool value)
{
  if(value)
  {
    if(!db_root_combine)
    {
     db_root_combine  = MdbTree->Items->Add(db_root,  "Вычисляемые");
     db_root_combine->Data    = (LPVOID)-1;

    }
    db_root_combine->DeleteChildren();
    MdbTree->Items->AddChild(db_root_combine,"");
    db_root_combine->Expand(false);
    if(!db_root)
      Caption = L"Редактор вычиcляемых значений";
  }
  else
  {
   if(db_root_combine)
     db_root_combine->Delete() ;
     db_root_combine = NULL;
  }

}




char *types[] = {"ПУ","КП","","Сублок"};

void __fastcall TMediumDBFrm::set_node_text(TTreeNode * node, mdb_record & rec)
{
  AnsiString str;
  AnsiString rec_name;
  sotd_addr addr(rec.addr.addr);
  if(rec.name!=(DWORD)-1)
   rec_name = RecordFrame1->get_record_name(rec.rec_id,false);
  else
   rec_name = RecordFrame1->get_record_name(rec.rec_id,true);

  DWORD a_type = addr.addrtype();
  if(a_type == OTD_ADDR_TYPE_SB && rec.addr.param_number!=(DWORD)-1)
  {
    str.printf("№ - %03d ",rec.addr.param_number);
  }
  else
  {
   if(a_type<OTD_ADDR_TYPE_BROADCAST)
    {
    str.printf("%s № - %03d ",types[a_type],addr.number());
   if(a_type == OTD_ADDR_TYPE_SB)
     str += addr.fa ? "ТИТ ":"ТС ";
    }
    else
    str.printf("ID %03d ",rec.rec_id);
  }
  str+=rec_name;
  node->Text = str;
}

TTreeNode * __fastcall TMediumDBFrm::add_node(TTreeNode * parent,mdb_record & rec)
{
  AnsiString str;
  str;
  TTreeNode * node = MdbTree->Items->AddChild(parent,str);
  node->Data  = (LPVOID)rec.rec_id;
  set_node_text(node,rec);
  if(rec.addr.param_number == (DWORD)-1 && !(rec.options&MDBR_OPT_COMBINED))
     {
      MdbTree->Items->AddChild(node,str);
     }

  return node;
}

void __fastcall TMediumDBFrm::add_current_states()
{
  if(!current_states)
  {
   CurrStateSheet->Visible = true;
   current_states = new TMDBCurrentState(this,mod_iface());
   //current_states->Parent = this;
   mdb_record rec;

   if(MdbTree->Selected)
     {
      current_states->only_combined = (db_root_combine && (MdbTree->Selected == db_root_combine || MdbTree->Selected->Parent == db_root_combine)) ? true: false;
      get_node_record(MdbTree->Selected,rec);
     }
   current_states->set_filter_addr(rec.addr.addr);
   current_states->Left = 0;
   current_states->Top  = 0;
   current_states->Align = alClient;
   current_states->ManualDock(StatePanel,0,alClient);
   current_states->Visible = true;
  }
  
}


void __fastcall TMediumDBFrm::MdbTreeCollapsing(TObject *Sender,
      TTreeNode *Node, bool &AllowCollapse)
{
  AllowCollapse = !create_mode;
  if(Node == db_root )
   {
    if(Node->Count > 1 || Node->getFirstChild()->Data != (LPVOID)-1)
       AllowCollapse = false;
   } 
}


void __fastcall TMediumDBFrm::MdbTreeExpanding(TObject *Sender,
      TTreeNode *Node, bool &AllowExpansion)
{
  if(Node == this->db_root_combine)
    {
      DWORD comb_count = mod_iface.call(MDBCM_COMBINE_GET_COUNT,-1,0);
      if(comb_count || create_mode)
        expand_combine();
        else
        AllowExpansion = false;
    }
    else
    {
      sotd_param_addr pa = get_node_addr(Node);
      DWORD cnt = mod_iface.call(MDBCM_GET_COUNT,pa.addr.addr,0);
      if(cnt || create_mode)
      {
       expand_node(Node,pa);
      }
      else
      AllowExpansion = false;
    }
}
//---------------------------------------------------------------------------
#pragma warn -8004

void     __fastcall TMediumDBFrm::expand_node(TTreeNode * parent,sotd_param_addr & pa)
{
 DWORD idx   = 0;
 mdb_record  rec;
 DWORD id ;
 TTreeNode * child = parent->getFirstChild();
 do{
    id = mod_iface.call(MDBCM_ENUM_OBJECTS,pa.addr.addr,idx++);
    if(id!=(DWORD)GKH_RET_ERROR)
      {
       get_record(id,rec);
       if(!child)
          child = add_node(parent,rec);
          else
          {
           child->Data = (LPVOID) rec.rec_id;
           set_node_text(child,rec);
           if(rec.addr.param_number == (DWORD)-1 && child->Count == 0)
             MdbTree->Items->AddChild(child,"");

          }
       child = NULL;
      }
   }while(id!=(DWORD)GKH_RET_ERROR);

}
#pragma warn .8004

#pragma warn -8004
void __fastcall TMediumDBFrm::expand_combine()
{
 DWORD idx = 0;
 DWORD comb_id;
 mdb_record  rec;

 TTreeNode * child =  db_root_combine->getFirstChild();
 do{
    comb_id = mod_iface.call(MDBCM_COMBINE_ENUM,idx++,(LPARAM)&rec);
    if(comb_id!=(DWORD)-1)
      {
       if(!child)
         child = add_node(db_root_combine,rec);
         else
         {
          child->Data = (LPVOID) rec.rec_id;
          set_node_text(child,rec);
         } 
        child = 0;
      }
   }while(comb_id!=(DWORD)-1);
 //db_root_combine->Collapse(true);
}
#pragma warn .8004


void __fastcall TMediumDBFrm::MdbTreeCollapsed(TObject *Sender,
      TTreeNode *Node)
{
  TTreeNode * child = Node->getFirstChild();
  child->Data = (LPVOID)-1;
  while(Node->Count>1)
       {
        child->Delete();
        child = Node->getFirstChild();
       }
   child->Data = (LPVOID)-1;    

}
//---------------------------------------------------------------------------


void __fastcall TMediumDBFrm::MdbTreeChange(TObject *Sender,
	  TTreeNode *Node)
{
  DWORD rec_id = (DWORD)Node->Data;
  RecordFrame1->Align = alClient;
  RecordFrame1->update_record(rec_id,-1);
  sotd_addr filter(-1);
  if(rec_id!=(DWORD)-1 || create_mode )
  {
   DeleteBtn->Enabled = true;
   miDelete->Enabled  = true;
   mdb_record rec = RecordFrame1->record;
   filter = rec.addr.addr;
   SettingsPanel->Visible = false;
   miCreateSet->Enabled = otd_addr_type(&rec.addr.addr) == OTD_ADDR_TYPE_SB ? true : false;
  }
  else
  {
   DeleteBtn->Enabled = false;
   miDelete->Enabled  = false;
   UndoBtn->Enabled   = false;
   if( Node == db_root)
   {
    SettingsPanel->Align = alClient;
    SettingsPanel->Visible = true;
    EditMdbPathChange(this);
    setup_mdb_info();
   }
  }
    if(!create_mode && current_states)
    {
       bool only_comb = (Node ==  db_root_combine || (Node->Parent && Node->Parent == db_root_combine)) ? true:false;
       if(only_comb )
           filter.addr = 0,filter.fa = -1;
       current_states->only_combined = only_comb;
       current_states->filter_addr = filter;
       current_states->select_item_byid(rec_id);

    }

}
//---------------------------------------------------------------------------
void     __fastcall TMediumDBFrm::setup_mdb_info()
{
 MDB_DBINFO mdbi;
 mdbi.dw_size = sizeof(mdbi);
 if(mod_iface.call(MDBCM_GET_DBINFO,(LPARAM)&mdbi,0) == GKH_RET_SUCCESS)
 {

  RecCountInfo->Caption   = mdbi.records_count;
  NextRecInfo->Caption    = mdbi.gen_record_id;
  KadrsCountInfo->Caption = mdbi.kadrs_count;
  NextKadrInfo->Caption   = mdbi.gen_kadr_id;
  NamesCountInfo->Caption = mdbi.names_count;
  CombCountInfo->Caption  = mdbi.combines_count;
 }
}





void __fastcall TMediumDBFrm::MdbTreeMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
  TTreeNode * node = MdbTree->GetNodeAt(X,Y);

       DragRecordId = (node) ? (DWORD)node->Data : (DWORD)-1;
  if(Button == mbRight /*&& (RecordFrame1->record.options&MDBR_OPT_COMBINED)*/)
  {
   Rmb = true;
    if(DragRecordId!=(DWORD)-1)// && node!=db_root && node!=db_root_combine && !node->Selected)
      {
       DragRecordId = (DWORD)node->Data;
       MdbTree->BeginDrag(false,16);
      }
  }

 if(Button == mbLeft)
   {
    Lmb = true;
   }
}
//---------------------------------------------------------------------------
void __fastcall TMediumDBFrm::MdbTreeMouseMove(TObject *Sender,
      TShiftState Shift, int X, int Y)
{
 if(Lmb)
 {
   if(DragRecordId!=(DWORD)-1)
      MdbTree->BeginDrag(false,0);
 }
}
//---------------------------------------------------------------------------

void __fastcall TMediumDBFrm::MdbTreeMouseUp(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
 if(Button == mbLeft)
   {
    Lmb = false;
   }

 if(Button == mbRight)
   {
    Rmb = false;
   }

}
//---------------------------------------------------------------------------




void __fastcall TMediumDBFrm::Setapply_mask(DWORD value)
{
        //TODO: Add your source code here
        if(Fapply_mask!=value)
        {
         Fapply_mask = value;
         ApplyBtn->Enabled = value ? true:false;
         UndoBtn->Enabled  = value ? true:false;
        }
}


void __fastcall TMediumDBFrm::apply_settings()
{
   WideString wstr = EditMdbPath->Text.Trim();
   if(wstr.Length() && wstr[wstr.Length()]!=L'\\')
      wstr+=L"\\";
  LPMODULE_CONFIG_DATA mcd;

  BYTE buffer[sizeof(MODULE_CONFIG_DATA)-sizeof(mcd->data[0])+sizeof(MDB_SETTINGS)];
  mcd =(LPMODULE_CONFIG_DATA)buffer;
  mcd->dw_sz = sizeof(buffer);
  mcd->mask  = -1;
  LPMDB_SETTINGS _settings = (LPMDB_SETTINGS)mcd->data;
  memset(_settings,0,sizeof(*_settings));
  lstrcpyW(_settings->mdb_path,wstr.c_bstr());//,sizeof(_settings->mdb_path));
  MdbTree->Items->BeginUpdate();
  if(mod_iface.set_module_config_data(mcd,sizeof(buffer)))
    {
     memcpy(&settings,_settings,sizeof(*_settings));
     setup_mdb_info();
    }
   MdbTree->Items->EndUpdate();
}

void __fastcall TMediumDBFrm::ApplyBtnClick(TObject *Sender)
{
  if(SettingsPanel->Visible)
  {
   ApplyBtn->Enabled = false;
   UndoBtn->Enabled  = false;
   apply_settings();
   return;
  }

  DWORD id = RecordFrame1->apply_changes(apply_mask);
  DWORD last_error = id == DWORD(-1) ?  GetLastError():0;
  if(create_mode)
    {
     create_mode = false;
     TTreeNode * node = MdbTree->Selected;
     if(id!=(DWORD)-1)
     {

      mdb_record rec,parent_rec;
      get_record(id,rec);
      get_record((DWORD)node->Parent->Data,parent_rec);
      if(parent_rec.addr.addr.addr == rec.addr.addr.addr)
      {
      if(node->Parent->Expanded)
      {
      TTreeNode * new_node = add_node(node->Parent,rec);
      new_node->Selected = true;
      }
      else
      {
       node->Parent->Expand(false);
       TTreeNode * new_node = find_by_addr(rec.addr);
       if(new_node)
          new_node->Selected = true;
          else
          node->Parent->Selected = true;
      }
      }
      else
      node->Parent->Selected = true;
     }
     else
        node->Parent->Selected = true;
     node->Delete();
    }
  apply_mask = 0;
  wchar_t err_text[MAX_PATH<<1];
  mod_iface.get_error_text(last_error,err_text,sizeof(err_text)/sizeof(wchar_t));
  StatusBar1->Panels->Items[1]->Text = err_text;
}
//---------------------------------------------------------------------------


void __fastcall TMediumDBFrm::UndoBtnClick(TObject *Sender)
{
  if(SettingsPanel->Visible)
  {
   ApplyBtn->Enabled = false;
   UndoBtn->Enabled  = false;
   EditMdbPath->Text = settings.mdb_path;
   return;
  }

 if(create_mode)
   DeleteBtnClick(this);
   else
   {
   RecordFrame1->undo_changes(apply_mask);
   }
   apply_mask = 0;
}
//---------------------------------------------------------------------------


void __fastcall TMediumDBFrm::DeleteBtnClick(TObject *Sender)
{
 TTreeNode * node = MdbTree->Selected;

     TTreeNode * sel_node = node->getNextSibling();
     if(!sel_node)
        sel_node = node->getPrevSibling();
      if(!sel_node)
          sel_node = node->Parent;

 if(create_mode)
    {
     create_mode = false;
     MdbTree->Selected = sel_node;
     if(node->Parent->Count>1)
      node->Delete();
      else
      {
       node->Parent->Collapse(true);
       node->Data = (LPVOID)-1;
      }
     apply_mask = 0;
    }
  else
  {
   UnicodeString str;

   str.printf(L"Удалить объект ID-% -u %s",(DWORD)node->Data, node->Text.c_str());
   DWORD qr = MessageBoxW(Handle,str.c_str(),L"Удаление из базы...",MB_YESNO);
   if(qr == IDYES && mod_iface.call(MDBCM_RECORD_REMOVE,(DWORD)node->Data,0) == GKH_RET_SUCCESS)
   {
    TTreeNode * parent = node->Parent;
    node->DeleteChildren();
    if(parent == db_root || parent == db_root_combine)
     {
        node->Delete(); // ?
     }
     else
     {
      if(parent->Count==1)
        parent->Collapse(true);
        else
        node->Delete();
     }
   }
 }
 MdbTree->Selected = sel_node;

}
//---------------------------------------------------------------------------
void __fastcall TMediumDBFrm::Setcreate_mode(bool value)
{
 Fcreate_mode = value;
 CreateBtn->Enabled   = !create_mode;
 miNew->Enabled       = !create_mode;
 miNewByMask->Enabled = !create_mode;
 SettingsPanel->Visible = false;
}

void __fastcall TMediumDBFrm::CreateBtnClick(TObject *Sender)
{
 //Создание объекта
 TTreeNode * node = MdbTree->Selected;
 TTreeNode * parent = node->Parent;
 mdb_record rec;
 AnsiString str = "Создание ...";
 if(db_root_combine && (node == db_root_combine || parent == db_root_combine))
    {
     str = "Созданине вычисляемого...";
     rec.options = MDBR_OPT_COMBINED;
    }

 if(node->Count || node == db_root || node == db_root_combine)
    {
     if(!node->Expanded)
        node->Expand(false);
       
     TTreeNode * child = node->getFirstChild();
     if(child && (DWORD)child->Data == (DWORD)-1)
        child->Text = str;
       else
        child = MdbTree->Items->AddChild(node,str);
        child->Data = (LPVOID)-1;
     create_mode = true;
     if(!node->Expanded)
        node->Expand(false);
     create_mode = false;   
     MdbTree->Selected = child;
    }
    else
    {
     if(parent && !parent->Expanded)
        parent->Expand(false);
     MdbTree->Selected = MdbTree->Items->AddChild(parent,str);
     MdbTree->Selected->MakeVisible();
    }

 //Выбор того, что создаем

 if(db_root_combine && (node == db_root_combine || parent == db_root_combine))
   {
    rec.options = MDBR_OPT_COMBINED;
   }
   else
    rec.addr = this->get_node_addr(node);
  create_mode = true;
  RecordFrame1->record = rec;
  UndoBtn->Enabled = true;
  DeleteBtn->Enabled = true;
  miDelete->Enabled = true;
  PageControl1->ActivePageIndex = 0;

}
//---------------------------------------------------------------------------

void __fastcall TMediumDBFrm::MdbTreeChanging(TObject *Sender,
      TTreeNode *Node, bool &AllowChange)
{
  if(create_mode)
     AllowChange = false;
     else
     {
      apply_mask = 0;
      AllowChange = true;//!create_mode && apply_mask == 0;
     }

}
//---------------------------------------------------------------------------


void __fastcall TMediumDBFrm::StopBtnClick(TObject *Sender)
{
  mod_iface.stop(MODULE_STOP_REQUEST);
}
//---------------------------------------------------------------------------

void __fastcall TMediumDBFrm::StartBtnClick(TObject *Sender)
{
  mod_iface.start(MODULE_START_REQUEST,0);
}
//---------------------------------------------------------------------------

void __fastcall TMediumDBFrm::RecordFrame1PercentChange(TObject *Sender)
{
  RecordFrame1->OtdAddrChange(Sender);

}
//---------------------------------------------------------------------------






void __fastcall TMediumDBFrm::FormClose(TObject *Sender,
      TCloseAction &Action)
{
  if(kadrs_editor)
     delete kadrs_editor;
     kadrs_editor = 0;
}
//---------------------------------------------------------------------------

void __fastcall TMediumDBFrm::tbShowKadrsEditorClick(TObject *Sender)
{
    if(!kadrs_editor)
       kadrs_editor = new TKadrsEditor(this,mod_iface());
    kadrs_editor->Show();
    kadrs_editor->SetFocus();
    kadrs_editor->Icon = Icon;
}
//---------------------------------------------------------------------------


void __fastcall TMediumDBFrm::FormKeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
 bool AltPressed  =  Shift.Contains(ssAlt);
 bool CtrlPressed =  Shift.Contains(ssCtrl);
 switch(Key)
 {
  case VK_RETURN:
                 if(CtrlPressed && apply_mask)
                   {Key = 0; ApplyBtnClick(this);}
                  break;
  case VK_BACK:
        if(AltPressed && (apply_mask || create_mode) )
           {Key = 0;UndoBtnClick(this);}
       break;
  case VK_DOWN:
  case VK_UP:
       if(CtrlPressed)
       {
        Key = 0;
        MdbTree->Perform(WM_KEYDOWN,(WPARAM)Key,(LPARAM)1);
       }
  break;
 }


}
//---------------------------------------------------------------------------

void __fastcall TMediumDBFrm::ToolButton3Click(TObject *Sender)
{
  mod_iface.flush_config();      
}
//---------------------------------------------------------------------------


void __fastcall TMediumDBFrm::PageControl1Changing(TObject *Sender,
      bool &AllowChange)
{
    add_current_states();
}
//---------------------------------------------------------------------------

void __fastcall TMediumDBFrm::MdbTreeAddition(TObject *Sender,
      TTreeNode *Node)
{
  Node->Data = (LPVOID)-1;        
}
//---------------------------------------------------------------------------

void __fastcall TMediumDBFrm::miAboutClick(TObject *Sender)
{
  if(!create_mode)
    PageControl1->ActivePageIndex = 2;    
}
//---------------------------------------------------------------------------

void __fastcall TMediumDBFrm::miCloseClick(TObject *Sender)
{
  Close();      
}
//---------------------------------------------------------------------------

void __fastcall TMediumDBFrm::miNewByMaskClick(TObject *Sender)
{
 /*ДОБАВЛЕНИЕ По маске*/
 TInputAddrMask * iam = new TInputAddrMask(this);
 if(this->MdbTree->Selected)
 {
  DWORD rec_id = (DWORD)MdbTree->Selected;
  if(RecordFrame && RecordFrame->record.rec_id == rec_id)
     iam->Edit1->Text  = RecordFrame->OtdAddr->Text;
 }

 if(iam->ShowModal() == mrOk)
 {
  otd_addr addr;

  if(iam->Edit1->Text.Length())
  {
   addr.addr = -1;
   str2addr(iam->Edit1->Text.c_str(),(BYTE*)&addr,sizeof(addr),'.');
  }
  else
  addr.addr = 0;
  mod_iface.call(MDBCM_ADDRECORDS_BYMASK,addr.addr,0);
 }
 delete iam;
}
//---------------------------------------------------------------------------

void __fastcall TMediumDBFrm::EditMdbPathChange(TObject *Sender)
{
  if(SettingsPanel->Visible)
  {
  if(AnsiString(settings.mdb_path).AnsiCompareIC(AnsiString(EditMdbPath->Text)) )
     {
      ApplyBtn->Enabled = true;
      UndoBtn->Enabled  = true;
     }
     else
     {
      ApplyBtn->Enabled = false;
      UndoBtn->Enabled  = false;
     }

  }    

        
}
//---------------------------------------------------------------------------

void __fastcall TMediumDBFrm::N5Click(TObject *Sender)
{
//Проверка уникальности признаков имен
  DWORD idx = 0;
  mdb_record  rec;
  std::map<DWORD,DWORD> strid_map;
  UnicodeString str;
  DWORD err_count   = 0;
  DWORD err_correct = 0;

  mdb_paint_variant    mdb_pvar;
  while(GKH_RET_SUCCESS == mod_iface.call(MDBCM_PAINTVARS_ENUM,idx++,(LPARAM)&mdb_pvar))
  {
     if(mdb_pvar.name_id  != DWORD(-1)) strid_map[mdb_pvar.name_id ] = -1;
     if(mdb_pvar.image_id != DWORD(-1)) strid_map[mdb_pvar.image_id] = -1;
     if(mdb_pvar.mask_id  != DWORD(-1)) strid_map[mdb_pvar.mask_id ] = -1;
  }
  idx = 0;

  while(mod_iface.call(MDBCM_RECORD_ENUM_BYID,idx++,(LPARAM)&rec)!=GKH_RET_ERROR)
  {
   if(rec.sname!=(DWORD)-1)
   {
    if(strid_map.count(rec.sname)==0)
       strid_map[rec.sname] = rec.rec_id;
       else
       {
         err_count++;
         str.printf(_T("Короткое имя объекта ID %d уже используется объектом %d"),rec.rec_id,strid_map[rec.sname]);
         if(MessageBox(Handle,str.c_str(),_T("Запрос на исправление"),MB_YESNO)==ID_YES)
           {
             rec.sname = -1;
             err_correct ++;
             mod_iface.call(MDBCM_RECORD_UPDATE,(LPARAM)&rec,MDBR_FIELD_SNAME);
           }
       }
   }

   if(rec.name!=(DWORD)-1)
   {
    if(strid_map.count(rec.name)==0)
       strid_map[rec.name] = rec.rec_id;
       else
       {
        //Добавить в лог
         err_count++;
         str.printf(_T("Имя объекта ID %d уже используется объектом %d"),rec.rec_id,strid_map[rec.sname]);
         if(MessageBox(Handle,str.c_str(),_T("Запрос на исправление"),MB_YESNO)==ID_YES)
           {
            rec.name = -1;
            err_correct ++;
            mod_iface.call(MDBCM_RECORD_UPDATE,(LPARAM)&rec,MDBR_FIELD_NAME);
           }
       }
   }
  }

  mdb_kadr kadr;
  idx = 0;

  while(mod_iface.call(MDBCM_KADR_ENUM,idx++,(LPARAM)&kadr)!=GKH_RET_ERROR)
  {
    if(kadr.name_id!=(DWORD)-1)
    {
    if(strid_map.count(kadr.name_id)==0)
       strid_map[kadr.name_id] = -2;
       else
       {
         err_count++;
         str.printf(_T("Имя  кадра ID %d уже используется "),kadr.kadr_id);
         if(MessageBox(Handle,str.c_str(),_T("Запрос на исправление"),MB_YESNO)==ID_YES)
           {
             kadr.name_id = -1;
             err_correct ++;
             mod_iface.call(MDBCM_KADR_UPDATE,(LPARAM)&kadr,MDB_KADR_FIELD_NAME);
           }
       }

    }

    if(kadr.sname_id!=(DWORD)-1)
    {
    if(strid_map.count(kadr.sname_id)==0)
       strid_map[kadr.sname_id] = -2;
       else
       {
         err_count++;
         str.printf(_T("Короткое имя кадра ID %d уже используется "),kadr.kadr_id);
         if(MessageBox(Handle,str.c_str(),_T("Запрос на исправление"),MB_YESNO)==ID_YES)
           {
             kadr.sname_id = -1;
             err_correct ++;
             mod_iface.call(MDBCM_KADR_UPDATE,(LPARAM)&kadr,MDB_KADR_FIELD_SNAME);
           }
       }

    }
    if(kadr.image_id !=(DWORD)-1)
    {
    if(strid_map.count(kadr.image_id)==0)
       strid_map[kadr.image_id] = -2;
       else
       {
         err_count++;
         str.printf(_T("Изображение кадра ID %d уже используется "),kadr.kadr_id);
         if(MessageBox(Handle,str.c_str(),_T("Запрос на исправление"),MB_YESNO)==ID_YES)
           {
             kadr.image_id = -1;
             err_correct ++;
             mod_iface.call(MDBCM_KADR_UPDATE,(LPARAM)&kadr,MDB_KADR_FIELD_SNAME);
           }
       }

    }

   }


  str.printf(_T("Проверка завершена : ошибок %d"),err_count);
  if(err_count)
  {
   str.cat_printf(_T(" исправлено %d"),err_correct);
  }
  this->StatusBar1->Panels->Items[1]->Text = str;
}
//---------------------------------------------------------------------------




void __fastcall TMediumDBFrm::miCollapseAllClick(TObject *Sender)
{
  this->db_root->Collapse(true);
  db_root_combine->Collapse(false);      
}
//---------------------------------------------------------------------------

void __fastcall TMediumDBFrm::miFindByAddrClick(TObject *Sender)
{
  TInputAddrMask * iam = new TInputAddrMask(this);
  iam->Caption = "Поиск по адресу";
  if(iam->ShowModal() && iam->Edit1->Text.Length())
  {
   sotd_param_addr pa;
   str2addr(iam->Edit1->Text.c_str(),(BYTE*)&pa.addr.addr,sizeof(pa.addr.addr),'.');
   char * p_ptr = strchr(AnsiString(iam->Edit1->Text).c_str(),'-');
   if(p_ptr)
      pa.param_number = ::atoi(++p_ptr);
   TTreeNode* node = this->find_by_addr(pa,true);


   if(node )
     {
      node->MakeVisible();
      if(!create_mode)
      node->Selected = true;
     }
  }
}
//---------------------------------------------------------------------------

bool __fastcall TMediumDBFrm::select_node_by_id(DWORD id)
{
   bool ret = false;
   mdb_record rec;
   if(this->get_record(mod_iface,id,rec))
   {
     TTreeNode* node = NULL;
     MdbTree->Items->BeginUpdate();
   if(rec.options & MDBR_OPT_COMBINED)
   {
     if(db_root_combine)
        node = find_by_id(db_root_combine,id,true);
   }
   else
   {

     if(db_root)
     {
      node = find_by_addr(rec.addr,true);
     }
   }
   if(node )
     {
      node->MakeVisible();
      if(!create_mode)
      node->Selected = true;
      ret = true;
     }
     MdbTree->Items->EndUpdate();
   }

   return ret;
}

void __fastcall TMediumDBFrm::miFindByIdClick(TObject *Sender)
{
  TInputAddrMask * iam = new TInputAddrMask(this);
  iam->Caption = "Поиск по ID";
  iam->Label1->Caption  = "ID";
  if(iam->ShowModal() && iam->Edit1->Text.Length())
  {
  #ifdef __CODEGEARC__
   DWORD id = atoi(iam->Edit1->Text.c_str());
  #else
   DWORD id = ::atoi(iam->Edit1->Text.c_str());
  #endif
   Cursor = crHourGlass;
   select_node_by_id(id);
   Cursor = crDefault;

  }

}
//---------------------------------------------------------------------------

void __fastcall TMediumDBFrm::N10Click(TObject *Sender)
{
 //ЗАПУСК проверки БД
 mod_iface.call(MDBCM_CHECK_REPAIR,0,0);
}
//---------------------------------------------------------------------------


string make_std_string(UnicodeString src)
{
  string res(AnsiString(src).c_str());
  return res;
}

void __fastcall put_id(std::set<int> & ids,int id)
{
	if(!ids.count(id))
		ids.insert(id);
}

int __fastcall  make_int_ids(std::string & s,std::set<int> & ids)
{
  std::vector<string> strs;
  boost::split(strs,s,boost::is_any_of("-"));
  if(strs.size())
  {

   int id = atoi(s.c_str());
   put_id(ids,id);
   if(strs.size()>1)
	 {
	   int end_id = atoi(strs.at(1).c_str());
	   while(++id<=end_id)
	      put_id(ids,id);

	 }
  }
 return ids.size();
}

void __fastcall TMediumDBFrm::miCreateSetClick(TObject *Sender)
{
  TInputAddrMask * iam = new TInputAddrMask(this,L"Сознание набора",L"Номера объектов (1-5,8,10,15-22)");
  if(iam->ShowModal() == mrOk && !iam->Edit1->Text.Trim().IsEmpty() )
  {
	std::string str = make_std_string(iam->Edit1->Text.Trim());
	std::vector<string> strs;
	boost::split(strs,str,boost::is_any_of(","));
	std::set<int> ids;
	std::vector<string>::iterator bptr = strs.begin(),eptr = strs.end();
	while(bptr<eptr)
	{
      make_int_ids(*bptr,ids);
	  ++bptr;
	}
   //Сформировали список идентификаторов
   mdb_record nrec;
   nrec.addr.addr = RecordFrame1->record.addr.addr;
   nrec.min_value =.0;
   if(nrec.addr.addr.fa == OTD_FA_DISCRETE)
	  {
	   nrec.state     = MDBR_STATE_DISCRETE;
	   nrec.kvants = nrec.max_value = 1.0;
	  }
	  else
	  {
	   nrec.kvants = nrec.max_value = 250;
	  }

   std::set<int>::iterator bs = ids.begin(),es = ids.end();
   while(bs!=es)
   {
	nrec.addr.param_number = *bs;
	mod_iface.call(MDBCM_RECORD_CREATE,(LPARAM)&nrec,0);
	++bs;
   }
  }
  delete iam;
}
//---------------------------------------------------------------------------




UnicodeString __fastcall TMediumDBFrm::get_text(TGKModuleInterface & mod_iface,DWORD id)
{
 UnicodeString ret;
 if( id!=(DWORD)-1)
 {
    BYTE buffer[2048];
    LPMDB_RECORD_TEXT rct = (LPMDB_RECORD_TEXT)buffer;
    rct->dw_size   = sizeof(buffer);
    rct->text_len  = 0;
    rct->text_size = (rct->dw_size-sizeof(*rct))/sizeof(wchar_t)+1;
    if(mod_iface.call(MDBCM_GET_TEXT,id,(LPARAM)rct) && rct->text_len)
       ret = UnicodeString(rct->text,rct->text_len);
 }
 return ret;
}

DWORD            __fastcall TMediumDBFrm::set_text(TGKModuleInterface & mod_iface,DWORD id,UnicodeString  str)
{
    BYTE buffer[2048];
    LPMDB_RECORD_TEXT rct = (LPMDB_RECORD_TEXT)buffer;
    rct->dw_size   = sizeof(buffer);
    rct->text_size = (rct->dw_size-sizeof(*rct))/sizeof(wchar_t)+1;
    rct->text_len  = str.Length();
    wcsncpy(rct->text,str.c_str(),rct->text_size);
    return mod_iface.call(MDBCM_UPDATE_TEXT,id,(LPARAM)rct);

}



LRESULT  __fastcall TMediumDBFrm::on_notify_paintvar(TGKModuleInterface & mod_iface,LPNOTIFY_CODE nc)
{
//  if(painter_dll.is_loaded())
//  {
//    LPMDB_PAINT_VARIANT pv = (LPMDB_PAINT_VARIANT )nc->notify_data;
//    UnicodeString name = TMediumDBFrm::get_text(mod_iface,pv->name_id);
//    set_paint_variant(mod_iface,name,pv,nc->notify_code == MDB_NOTIFY_CODE_PAINTVAR_UPD ? true : false);
//  }
  return GKH_RET_SUCCESS;
}

void __fastcall TMediumDBFrm::miExportCSVClick(TObject *Sender)
{
  //Экспротируем содержимое в CSV
  TTreeNode * node = MdbTree->Selected;
  TImportCSV * ic = new TImportCSV(this);
  ic->gkmodule  = mod_handle;
  ic->tree_node = node;
  ic->Caption = L"Экспорт данных";
  ic->ShowModal();
  delete ic ;


}
//---------------------------------------------------------------------------
void __fastcall TMediumDBFrm::miImportCSVClick(TObject *Sender)
{
  TImportCSV * ic = new TImportCSV(this);
  ic->Icon->Assign(Icon);
  if(ic->select_file(Handle))
  {
     ic->rbAddRecords->Visible = true;
     ic->rbUpdateRecords->Visible = true;
     ic->gkmodule = mod_handle;
     ic->Caption = L"Импорт данных";
     ic->ShowModal();
  }
  delete ic;
}
//---------------------------------------------------------------------------





void __fastcall TMediumDBFrm::PageControl1Change(TObject *Sender)
{
  if(PageControl1->ActivePage == this->OrdersSheet)
     setup_orders_tree();
}
//---------------------------------------------------------------------------

void     __fastcall TMediumDBFrm::setup_orders_tree()
{
  OrdersTree->Items->BeginUpdate();
  OrdersTree->Items->Clear();
  DWORD idx = 0;
  ORDER_ITEM oi;
  oi.dw_size = sizeof(oi);
  TTreeNode * current_modem = NULL;
  while(GKH_RET_SUCCESS == mod_iface.call(MDBCM_ENUM_ORDERS,idx,(LPARAM)&oi))
  {
    UnicodeString item_text;
    ++idx;
    if(!current_modem || DWORD(current_modem->Data) != oi.dest_addr.addr)
    {
        item_text.printf(L"Модем %02d.%02d ",(DWORD)oi.dest_addr.modem,(DWORD)oi.dest_addr.line);
        current_modem = OrdersTree->Items->Add(NULL,item_text);
        current_modem->Data = (LPVOID)oi.dest_addr.addr;
    }
    mdb_kadr kdr;
    mod_iface.call(MDBCM_KADR_GET,oi.kadr_id,(LPARAM)&kdr);
    UnicodeString kname = get_text(mod_iface,kdr.sname_id) ;
    if(kname.IsEmpty())
                  kname = get_text(mod_iface,kdr.name_id) ;

    item_text.printf(L"Кадр %3d маска %08X %s",oi.kadr_id,oi.order_mask,kname.c_str());
    OrdersTree->Items->AddChild(current_modem,item_text);

  }
  OrdersTree->Items->EndUpdate();
}


void __fastcall TMediumDBFrm::FormShow(TObject *Sender)
{
 if(!records_enable && !combine_enable)
     records_enable = true;
  this->PageControl1->ActivePage = ParamsSheet;
}
//---------------------------------------------------------------------------

void __fastcall TMediumDBFrm::comb_frm_close(TObject *Sender, TCloseAction & ca)
{
  comb_frm = NULL;
  ca = caFree;
}

void __fastcall TMediumDBFrm::tbCombinesEditClick(TObject *Sender)
{
  if(!comb_frm)
  {
   comb_frm = new TMediumDBFrm(this,Handle,mod_iface());
   comb_frm->combine_enable = true;
   comb_frm->tbCombinesEdit ->Visible = false;
   comb_frm->Visible = true;
   comb_frm->OnClose = comb_frm_close;
  }
  comb_frm->SetFocus();
}
//---------------------------------------------------------------------------


