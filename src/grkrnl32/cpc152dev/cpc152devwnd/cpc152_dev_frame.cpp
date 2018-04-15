//---------------------------------------------------------------------------
#pragma hdrstop
#include <vcl.h>
#include "cpc152_dev_frame.h"
#include <Math.hpp>


//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "gkmod_frame"
#pragma resource "*.dfm"
Tcpc152dev_frame *cpc152dev_frame;
//---------------------------------------------------------------------------
__fastcall Tcpc152dev_frame::Tcpc152dev_frame(TComponent* Owner)
	: TGKModuleFrame(Owner)
{
  memset(&line_config,0,sizeof(line_config));
  line_config.dw_size = sizeof(line_config);
  for(int i = 0;i<CPC152_MAX_GROUP_ANALOG;i++)
     grp_num->Items->Add(i);
     grp_num->ItemIndex = 0;
}
//---------------------------------------------------------------------------



bool     __fastcall  Tcpc152dev_frame::apply_changes ()
{
  bool ret = false;
  CPC152_LINE_CONFIG lc;
  lc.dw_size         = sizeof(lc);
  get_conf_values(&lc);
  int line_num = (int)current_tree_node->Data;
  DWORD result;
  if(line_num>0)
	{
         if(ch_mask & CPC152LINE_MASK_COMMON)
         {
	  lc.mask = ch_mask;
	  result = mod_iface.call(CMD_CPC152DEV_UPD_LINE,(LPARAM)&lc,line_num);
         }
         else
         result = GKH_RET_SUCCESS;
	}
	else
	result  = mod_iface.call(CMD_CPC152DEV_ADD_LINE,(LPARAM)&lc,line_num);

	if(result == GKH_RET_SUCCESS )
	{
	 memcpy(&line_config,&lc,sizeof(line_config));
	 current_tree_node->Text = get_line_text(&lc);
	 current_tree_node->Data = (void*)lc.cp_number;
	 ret = true;
	 ch_mask &= ~CPC152LINE_MASK_COMMON;
	}

	if(ret  && (ch_mask & CPC152LINE_MASK_DFT_ENABLED ))
        {
          if(mod_iface.call(CMD_CPC152_LINE_SET_DFT_ENABLED,line_config.cp_number,(LPARAM)&dft_enabled))
             ret = false;
             else
             ch_mask &= ~CPC152LINE_MASK_DFT_ENABLED;

        }


     if(ret && (ch_mask & CPC152LINE_MASK_ALARM_STARTS ))
     {
	 alarm_start_t::iterator ptr = alarm_starts_deleted.begin(),end = alarm_starts_deleted.end();
	 while(ret && ptr<end)
	 {
	  ret = mod_iface.call(CMD_CPC152_LINE_ALARM_START_DEL,line_config.cp_number,(LPARAM)&*ptr) == GKH_RET_SUCCESS ? true : false;
	  ++ptr;
	 }

	 if(ret)
            alarm_starts_deleted.clear();

	 ptr = alarm_starts.begin(),end = alarm_starts.end();

	 while( ptr<end)
	 {
	  mod_iface.call(CMD_CPC152_LINE_ALARM_START_ADD,line_config.cp_number,(LPARAM)&*ptr);
	  ++ptr;
	 }
	 if(ret) ch_mask &= ~CPC152LINE_MASK_ALARM_STARTS;
   }

  return ret;
}

bool     __fastcall  Tcpc152dev_frame::undo_changes  ()
{
  bool ret = false;
  setup_controls(&line_config);
  int line_num = (int)current_tree_node->Data;
  if(line_num>0)
  {
     mod_iface.call(CMD_CPC152_LINE_GET_DFT_ENABLED,line_config.cp_number,(LPARAM)&dft_enabled);
     lv_dft_enabled->Refresh();
  }

  ch_mask = 0;
  return ret;
}

void     __fastcall  Tcpc152dev_frame::current_object_changed(TObject * obj ,bool &enable_set)
{
    TTreeNode * node = current_tree_node;
    if( node && -1 == (int)node->Data)
         enable_set = false;
}

void     __fastcall  Tcpc152dev_frame::current_object_change (TObject * value)
{
  if(Fcurrent_object!=value)
  {
     Fcurrent_object = value;
     if(value)
     {
      line_config.mask = -1;
      line_config.cp_number = (int)current_tree_node->Data;
      memset(dft_enabled.dft_enabled,-1,sizeof(dft_enabled.dft_enabled));
      read_line_config(&mod_iface,&line_config,&dft_enabled);
      setup_controls  (&line_config);
      page_control->ActivePageIndex = 0;
     }

  }
}

bool     __fastcall  Tcpc152dev_frame::read_line_config(TGKModuleInterface * mod_iface,LPCPC152_LINE_CONFIG lc,LPCPC152_DFT_ENABLED dft_enabled)
{
   lc->dw_size   = sizeof(*lc);
   bool ret = mod_iface->call(CMD_CPC152DEV_GET_LINE,(LPARAM)lc,(LPARAM)lc) == GKH_RET_SUCCESS ? true:false;
   if(dft_enabled)
   {
   if(ret )
       mod_iface->call(CMD_CPC152_LINE_GET_DFT_ENABLED,lc->cp_number,(LPARAM)dft_enabled);
       else
       memset(dft_enabled->dft_enabled,-1,sizeof(dft_enabled->dft_enabled));
   }
   return ret;
}

UnicodeString     __fastcall Tcpc152dev_frame::get_line_text   (LPCPC152_LINE_CONFIG lc)
{
    UnicodeString str;
    str.printf(L"КП %d %s",lc->cp_number,lc->cp_name);
    return str;
}


void     __fastcall  Tcpc152dev_frame::setup_controls(LPCPC152_LINE_CONFIG lc)
{
  ++in_setup;

    ip_addr->Text   = lc->ip_addr;
    if(lc->ip_port)
       ip_port->Text   = lc->ip_port;
       else
       ip_port->Text   = UnicodeString();

    if(lc->cp_number>0)
       cp_number->Text = lc->cp_number;
       else
       cp_number->Text   = UnicodeString();
    cp_name->Text        = lc->cp_name;
    UnicodeString str ;
	wchar_t * ffmt = L"%.5f";
	wchar_t * ifmt = L"%d";
	str.printf(ffmt,lc->freq);
	Freq->Text = str;

	int pos = (int )ceil( log((long double)lc->dft_frame_size)/log((long double)2.0));
        pos = std::max(pos,UpDown1->Min);
        pos = std::min(pos,UpDown1->Max);
	UpDown1->Position = pos;
        FrameSize->Text   = (1<<pos);


	str.printf(ffmt,lc->samples_freq);
	SampleFreq->Text = str;

	str.printf(ifmt,lc->samples_count);
	SamplesCount->Text = str;

	str.printf(ffmt,lc->freq_gate);
	FrqGate->Text     = str;

	str.printf(ffmt,lc->dft_percent);
	AmplPercent->Text  = str;

        CalcVariant->ItemIndex = lc->calc_varint ;

        CorrMaxKvants->Text    = lc->corr_max_kvants;
        CorrMustKvants->Text   = lc->corr_must_kvants;
        CorrZero->Text         = lc->corr_zero;
        if(CorrEnable->Checked != bool(lc->corr_enable))
           CorrEnable->Checked =  bool(lc->corr_enable);


	read_alarm_starts();

  --in_setup;
	ch_mask = 0;

}

void     __fastcall  Tcpc152dev_frame::read_alarm_starts()
{
  alarm_starts.clear();
  CPC152_ALARM_START ast;

  WORD i = 0;
  while(GKH_RET_SUCCESS == mod_iface.call(CMD_CPC152_LINE_ALARM_START_ENUM,MAKELONG(line_config.cp_number,i),(LPARAM)&ast))
  {
	++i;
	this->alarm_starts.push_back(ast);
  }
  AlarmList->Items->Count = alarm_starts.size();
  AlarmList->Refresh();


}

void     __fastcall  Tcpc152dev_frame::get_conf_values(LPCPC152_LINE_CONFIG lc)
{
 lc->dw_size = sizeof(*lc);
 wcsncpy(lc->ip_addr,ip_addr->Text.c_str(),KERTL_ARRAY_COUNT(lc->ip_addr));
 wcsncpy(lc->cp_name,cp_name->Text.c_str(),KERTL_ARRAY_COUNT(lc->cp_name));
 lc->ip_port   = _wtoi(ip_port->Text.c_str());
 lc->cp_number = _wtoi(cp_number->Text.c_str());
 lc->freq         = _wtof(Freq->Text.c_str());
 if(SameValue(lc->freq,.0))
	lc->freq = 50;
 lc->samples_freq = _wtof(SampleFreq->Text.c_str());
 if(SameValue(lc->samples_freq,.0))
	lc->samples_freq = 500;
 lc->samples_count  = _wtoi(SamplesCount->Text.c_str());
 if(!lc->samples_count)
	lc->samples_count = lc->samples_freq;
 lc->dft_frame_size = _wtoi(FrameSize->Text.c_str());
 lc->dft_percent    = _wtof(AmplPercent->Text.c_str());
 if(lc->dft_percent<90.0)
    lc->dft_percent = 90;
 if(lc->dft_percent>=100.0)
    lc->dft_percent = 99.9;

 lc->freq_gate        = _wtof(FrqGate->Text.c_str());
 lc->calc_varint      = CalcVariant->ItemIndex;
 lc->corr_enable      = CorrEnable->Checked;
 lc->corr_max_kvants  = _wtoi(CorrMaxKvants->Text.c_str()) ;
 lc->corr_must_kvants = _wtoi(CorrMustKvants->Text.c_str());
 lc->corr_zero        = _wtoi(CorrZero->Text.c_str());

}

bool     __fastcall  Tcpc152dev_frame::delete_current()
{
 bool ret = true;
 if(current_tree_node)
 {
   int num = (int)current_tree_node->Data;
   if( num >0 )
    {
         // delete real line
      ret = mod_iface.call(CMD_CPC152DEV_DEL_LINE,num,0) == GKH_RET_SUCCESS ? true:false;
    }
  if(ret)
     current_tree_node->Delete();
 }
 return ret;
}

void __fastcall Tcpc152dev_frame::ip_addrChange(TObject *Sender)
{
 if(!in_setup)
 {
  TEdit* edit = dynamic_cast<TEdit*>(Sender);
  if( edit && edit->Tag)
  {
    this->ch_mask|= edit->Tag;
  }
 }
}
//---------------------------------------------------------------------------

int   __fastcall Tcpc152dev_frame::get_cp_number()
{
   return  (current_tree_node) ? (int)current_tree_node->Data : -1;
}


void __fastcall Tcpc152dev_frame::UpDown1ChangingEx(TObject *Sender, bool &AllowChange,
		  int NewValue, TUpDownDirection Direction)
{
 this->FrameSize->Text = (1<<NewValue);
}
//---------------------------------------------------------------------------

void __fastcall Tcpc152dev_frame::AlarmListData(TObject *Sender, TListItem *Item)

{
   if(Item->Index<(int)alarm_starts.size())
   {
	  CPC152_ALARM_START & ast = alarm_starts.at(Item->Index);
	  Item->Caption = ast.dev_num;
	  Item->SubItems->Add(ast.grp_num);
	  Item->SubItems->Add(ast.param_num);
	  Item->SubItems->Add(ast.value ? L"<1":L">0");
	  Item->SubItems->Add(ast.external ? L"V":L"");
   }
}
//---------------------------------------------------------------------------

void __fastcall Tcpc152dev_frame::AlarmListChange(TObject *Sender, TListItem *Item,
          TItemChange Change)
{
  this->bRemove->Enabled = AlarmList->Items->Count ? true : false;
}
//---------------------------------------------------------------------------

void __fastcall Tcpc152dev_frame::sDevChange(TObject *Sender)
{
  bool enabled =  (sDev->Text.IsEmpty() || sGrp->Text.IsEmpty() || sNum->Text.IsEmpty() || sState->ItemIndex<0) ? false : true;
  if(enabled && alarm_starts.size())
  {
   CPC152_ALARM_START ast(_wtoi(sDev->Text.c_str()),
						 _wtoi(sGrp->Text.c_str()),
						 _wtoi(sNum->Text.c_str()),
						 sState->ItemIndex ? 1 : 0
						 ,0
						);
   alarm_start_t::iterator ptr = alarm_starts.begin(), end = alarm_starts.end();
   ptr = lower_bound(ptr,end,ast);
   if(ptr<end && *ptr == ast)
	 {
	  enabled = (ptr->external != ast.external || ptr->value != ast.value) ? true : false;
	 }
  }
  bAdd->Enabled = enabled;
}
//---------------------------------------------------------------------------

void __fastcall Tcpc152dev_frame::bAddClick(TObject *Sender)
{
  CPC152_ALARM_START ast(_wtoi(sDev->Text.c_str()),
						 _wtoi(sGrp->Text.c_str()),
						 _wtoi(sNum->Text.c_str()),
						 sState->ItemIndex ? 1 : 0
						 ,0
						);
  alarm_start_t::iterator ptr = alarm_starts.begin(), end = alarm_starts.end();
  ptr = lower_bound(ptr,end,ast);
  if(ptr<end && *ptr == ast)
	 {
	  if(ptr->external != ast.external || ptr->value != ast.value)
	  {
	   *ptr = ast;
	   this->ch_mask |= CPC152LINE_MASK_ALARM_STARTS;
	  }
	 }
	 else
	 {
	  alarm_starts.insert(ptr,ast);
	  ch_mask |= CPC152LINE_MASK_ALARM_STARTS;
	 }

   AlarmList->Items->Count = alarm_starts.size();
   AlarmList->Refresh();

}
//---------------------------------------------------------------------------


void __fastcall Tcpc152dev_frame::bRemoveClick(TObject *Sender)
{
  if(AlarmList->Selected && AlarmList->Selected->Index <(int) alarm_starts.size())
  {
   alarm_start_t::iterator ptr = alarm_starts.begin();
   std::advance(ptr,AlarmList->Selected->Index);
   alarm_starts_deleted.push_back(*ptr);
   alarm_starts.erase(ptr);
   ch_mask |= CPC152LINE_MASK_ALARM_STARTS;
  }
  AlarmList->Items->Count = alarm_starts.size();
  AlarmList->Refresh();
}
//---------------------------------------------------------------------------
void __fastcall Tcpc152dev_frame::on_alarm_start_notify( LPCPC152_ALARM_START ast, bool add)
{
  alarm_start_t::iterator ptr = alarm_starts.begin(), end = alarm_starts.end();
  ptr = lower_bound(ptr,end,*ast);
  if(add)
  {
    if(ptr<end && *ptr == *ast)
       *ptr = *ast;
       else
       alarm_starts.insert(ptr,*ast);
  }
  else
  {
       if(ptr<end && *ptr == *ast)
          alarm_starts.erase(ptr);

  }

  AlarmList->Items->Count = alarm_starts.size();
  AlarmList->Refresh();
}

void __fastcall Tcpc152dev_frame::CreateOscilogrammClick(TObject *Sender)
{
  mod_iface.call(CMD_CPC152DEV_WRITE_OSCILOGRAMM,this->get_cp_number(),_wtoi(OscLenght->Text.c_str()));
}
//---------------------------------------------------------------------------

void __fastcall Tcpc152dev_frame::CalcVariantClick(TObject *Sender)
{
 if(!in_setup)
   ch_mask |= CPC152LINE_MASK_CALC_VARIANT;
}
//---------------------------------------------------------------------------

void __fastcall Tcpc152dev_frame::CorrEnableClick(TObject *Sender)
{
  int dh = this->CorrectionBox->Height;
  if(!CorrEnable->Checked) dh = -dh;
     GroupBox1->Height += dh;
     GroupBox3->Top    += dh;
     page_control->Top    =  GroupBox3->Top+GroupBox3->Height + 4;
     page_control->Height -= dh;
  if(!in_setup)
      ch_mask |= CPC152LINE_MASK_CORR_ENABLE;
}
//---------------------------------------------------------------------------


void __fastcall Tcpc152dev_frame::page_controlChange(TObject *Sender)
{
  if(page_control->ActivePage == dft_enabled_page)
  {
   lv_dft_enabled->Items->Count = CPC152_MAX_PARAM_ANALOG;
   lv_dft_enabled->Repaint() ;
  }
  else
  lv_dft_enabled->Items->Count = 0;

}
//---------------------------------------------------------------------------


bool __fastcall Tcpc152dev_frame::is_dft_enabled(int dev,int group,int param)
{
  DWORD group_value = this->dft_enabled.dft_enabled[dev][group];
  DWORD test = (1<<param);
  return (group_value&test) ? true : false;
}

void __fastcall Tcpc152dev_frame::set_dft_enabled(int dev,int group,int param,bool enable)
{
  LPDWORD group_value = &dft_enabled.dft_enabled[dev][group];
  DWORD val = (1<<param);
  if(enable)
  *group_value|= val;
  else
  *group_value&= ~val;

}


void __fastcall Tcpc152dev_frame::lv_dft_enabledData(TObject *Sender, TListItem *Item)
{
  Item->Caption = Item->Index;
  bool enabled =  is_dft_enabled(dev_num->ItemIndex,grp_num->ItemIndex,Item->Index);
  Item->SubItems->Add(enabled ? L"Разрешено":L"");
}
//---------------------------------------------------------------------------

void __fastcall Tcpc152dev_frame::grp_numChange(TObject *Sender)
{
  lv_dft_enabled->Repaint();
  //lv_dft_enabled->Items->Item[0]->Selected = true;

}
//---------------------------------------------------------------------------

void __fastcall Tcpc152dev_frame::lv_dft_enabledDblClick(TObject *Sender)
{
  TListItem * Item = lv_dft_enabled->Selected;
  if(Item)
    {
     set_dft_enabled(dev_num->ItemIndex,grp_num->ItemIndex,Item->Index,!is_dft_enabled(dev_num->ItemIndex,grp_num->ItemIndex,Item->Index));
     this->ch_mask |=  CPC152LINE_MASK_DFT_ENABLED;
     lv_dft_enabled->Refresh();
    }
}
//---------------------------------------------------------------------------

void __fastcall Tcpc152dev_frame::lv_dft_enabledKeyPress(TObject *Sender, System::WideChar &Key)

{
  if(Key == L' ')
     {
         Key = 0;
         lv_dft_enabledDblClick(this);
     }
}
//---------------------------------------------------------------------------

void __fastcall Tcpc152dev_frame::SpeedButton1Click(TObject *Sender)
{
  LPDWORD group_value = &dft_enabled.dft_enabled[dev_num->ItemIndex][grp_num->ItemIndex];
  *group_value = Sender == SpeedButton1 ? -1 : 0;
   lv_dft_enabled->Refresh();
   ch_mask |=  CPC152LINE_MASK_DFT_ENABLED;
}
//---------------------------------------------------------------------------

