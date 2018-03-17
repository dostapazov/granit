//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "mdb_tree_frame.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "gkmod_frame"
#pragma resource "*.dfm"
TMdbTreeFrame *MdbTreeFrame;
//---------------------------------------------------------------------------
__fastcall TMdbTreeFrame::TMdbTreeFrame(TComponent* Owner)
	: TGKModuleFrame(Owner)
{
 db_root          = MdbTree->Items->AddFirst(NULL,"Список Объектов");
 db_root->Data    = (LPVOID)-1;
 db_root_combine  = MdbTree->Items->Add(db_root,  "Вычисляемые");
 db_root_combine->Data    = (LPVOID)-1;
 Fcreate_mode     = false;

}
//---------------------------------------------------------------------------

void     __fastcall  TMdbTreeFrame::after_set_gkhandle()
{
 setup_mdb_tree();
}

void     __fastcall TMdbTreeFrame::set_create_mode(bool value)
{
   Fcreate_mode = value;
}

void     __fastcall  TMdbTreeFrame::setup_mdb_tree()
{
  mdb_record rec;
  TTreeNode * item = MdbTree->Items->AddChild(db_root,"");
  item->Data = (LPVOID)-1;
  db_root->Expand(false);
  MdbTree->Items->AddChild(db_root_combine,"");

}

void __fastcall TMdbTreeFrame::MdbTreeExpanding(TObject *Sender, TTreeNode *Node,
          bool &AllowExpansion)
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



void __fastcall TMdbTreeFrame::set_node_text(TTreeNode * node, mdb_record & rec)
{
  char *types[] = {"ПУ","КП","","Сублок"};
  AnsiString str;
  AnsiString rec_name;
  sotd_addr addr(rec.addr.addr);
  if(rec.sname!=(DWORD)-1)
   rec_name = RecordFrame1->get_record_name(rec.rec_id,true);
  else
   rec_name = RecordFrame1->get_record_name(rec.rec_id,false);

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

TTreeNode * __fastcall TMdbTreeFrame::add_node(TTreeNode * parent,mdb_record & rec)
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


void     __fastcall TMdbTreeFrame::expand_node(TTreeNode * parent,sotd_param_addr & pa)
{

}

void     __fastcall TMdbTreeFrame::expand_combine()
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
}

sotd_param_addr
         __fastcall TMdbTreeFrame::get_node_addr  (TTreeNode * node)
{
 mdb_record rec;
 if(get_node_record(node,rec))
    return rec.addr;
 return sotd_param_addr();
}

bool     __fastcall TMdbTreeFrame::get_node_record(TTreeNode *node, mdb_record & rec)
{
 return get_record((DWORD)node->Data,rec);
}

bool     __fastcall TMdbTreeFrame::get_record(DWORD id, mdb_record & rec)
{
 return mod_iface.call(MDBCM_RECORD_GET,id,(LPARAM)&rec)!=GKH_RET_ERROR ? true:false;
}

bool     __fastcall TMdbTreeFrame::get_record(sotd_param_addr & pa, mdb_record & rec)
{
  rec.addr = pa;
  return get_record(-1,rec);
}

