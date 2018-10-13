// ---------------------------------------------------------------------------
#pragma hdrstop

#include <vcl.h>
#include "opc_config_editor.h"
#include <kertl.hpp>
#include <oleauto.h>
#include "gkopcitem_view_frame.h"
#include <clipbrd.hpp>
#include <boost/algorithm/string/replace.hpp>
// ---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

// #define FOLDER_NODE_ATTR  ((LPVOID)-1)
// #define FOLDER_TEMPL_ATTR ((LPVOID)-2)

struct tree_item_def
{
	bool is_branch;
	bool in_use;
	UnicodeString item_name;

	tree_item_def() : is_branch(true), in_use(false)
	{
	}

	tree_item_def(UnicodeString _name) : is_branch(false), in_use(false), item_name(_name)
	{
	}
};

inline tree_item_def * __fastcall get_tree_item_def(TTreeNode * node)
{
	return static_cast<tree_item_def*>(node ? node->Data : NULL);
}

inline void __fastcall delete_item_def(TTreeNode * node)
{
	tree_item_def * idef = get_tree_item_def(node);
	node->Data = NULL;
	if (idef)
		delete idef;
}

TOpcConfigEditor *OpcConfigEditor;

// ---------------------------------------------------------------------------
__fastcall TOpcConfigEditor::TOpcConfigEditor(TComponent* Owner) : TForm(Owner), opc_server(NULL), opc_browser(NULL),
	opc_group(NULL)
{
	discrete_node = OtdTree->Items->Add(NULL, L"ТС");
	discrete_node->Data = (LPVOID)group_param_t(OTD_FA_DISCRETE, -1).group_data;
	analog1_node = OtdTree->Items->Add(NULL, L"ТИТ");
	analog1_node->Data = (LPVOID)group_param_t(OTD_FA_ANALOG, -1).group_data;
	analog2_node = OtdTree->Items->Add(NULL, L"ТИИ");
	analog2_node->Data = (LPVOID)group_param_t(OTD_FA_ANALOG_INTEGR, -1).group_data;
	// divisor = UnicodeString();
	// prefix  = UnicodeString();
	progress_top = progress_current = 0;
}

// ---------------------------------------------------------------------------
void __fastcall TOpcConfigEditor::opc_close_server()
{
	if (iprops.IsBound())
		iprops.Reset();
	if (opc_browser)
	{
		delete opc_browser;
		opc_browser = NULL;
	}
	if (opc_group)
	{
		delete opc_group;
		opc_group = NULL;
	}

	if (opc_server)
	{
		delete opc_server;
		opc_server = NULL;
	}
	Timer1->Enabled = false;
}

void __fastcall TOpcConfigEditor::set_opc_server_name(UnicodeString str)
{
	if (opc_server_name != str)
	{
		FOpcServerName = str;
		opc_open_server();
		if (!(opc_server && opc_server->IsBound()))
			str += L"Ошибка запуска OPC сервера";
		else
			this->fill_opc_items();
		Caption = str;

	}
}

UnicodeString __fastcall TOpcConfigEditor::get_opc_server_name()
{
	return FOpcServerName;
}

void __fastcall TOpcConfigEditor::set_opc_server_guid(GUID _guid)
{
	if (memcmp(&FOpcServerGuid, &_guid, sizeof(_guid)))
	{
		memcpy(&FOpcServerGuid, &_guid, sizeof(_guid));
		opc_open_server();
	}
}

GUID __fastcall TOpcConfigEditor::get_opc_server_guid()
{
	return FOpcServerGuid;
}

UnicodeString __fastcall TOpcConfigEditor::get_opc_server_host()
{
	return FOpcServerHost;
}

void __fastcall TOpcConfigEditor::set_opc_server_host(UnicodeString str)
{
	if (opc_server_host != str)
	{
		FOpcServerHost = str;
		opc_open_server();
		if (!(opc_server && opc_server->IsBound()))
			str += L"Ошибка запуска OPC сервера";
		else
			this->fill_opc_items();
		Caption = str;

	}
}

void __fastcall TOpcConfigEditor::set_opc_config_name(UnicodeString str)
{
	str = str.Trim();
	if (FConfigFile != str)
	{
		FConfigFile = str;
		read_config();
	}
}

UnicodeString __fastcall TOpcConfigEditor::get_opc_config_name()
{

	return FConfigFile;
}

void __fastcall TOpcConfigEditor::opc_open_server(GUID _guid, UnicodeString host, UnicodeString ProgId)
{
	memcpy(&FOpcServerGuid, &_guid, sizeof(_guid));
	this->FOpcServerHost = host;
	this->FOpcServerName = ProgId;
	opc_open_server();
}

void __fastcall TOpcConfigEditor::opc_open_server()
{
	opc_close_server();
	opc_server = new opc::TOpcServer;
	if (opc_server)
	{
		HRESULT res = opc_server->create_instance(FOpcServerGuid, FOpcServerHost.c_str());
		if (S_OK == res)
		{
			Sleep(1000);
			Timer1->Enabled = true;
			fill_opc_items();
		}
		else
		{
			UnicodeString str(_com_error(res).ErrorMessage());
			MessageBox(Handle, str.c_str(), _T("Ошибка отгрытия сервера"), MB_OK);
		}
	}
	opc_server_refresh_info();
}

void __fastcall TOpcConfigEditor::opc_server_refresh_info()
{
	if (opc_server && opc_server->IsBound())
	{
		__int64 timestamp;

		opc_server->update_status();
		timestamp = opc_server->get_start_time();
		ServerStartTime->Caption = get_time_str(timestamp);

		timestamp = opc_server->get_server_time();
		ServerCurrentTime->Caption = get_time_str(timestamp);

		timestamp = opc_server->get_update_time();
		ServerUpdateTime->Caption = get_time_str(timestamp);
		set_opc_state_text(opc_server->get_state());
		GroupCount->Caption = opc_server->get_group_count();
	}
}

void __fastcall TOpcConfigEditor::set_opc_state_text(OPCSERVERSTATE state)
{
	wchar_t * state_text[] =
	{L"Неизвестно", L"Работает", L"Ошибка запуска", L"Не сконфигурирован", L"Приостановлен", L"", L""};
	ServerStateText->Caption = state_text[state];
}

UnicodeString __fastcall TOpcConfigEditor::get_time_str(__int64 timestamp, bool local)
{
	UnicodeString str;
	if (timestamp)
	{
		SYSTEMTIME st;
		FileTimeToSystemTime((LPFILETIME)&timestamp, &st);

		if (local)
		{
			TIME_ZONE_INFORMATION tzi;
			GetTimeZoneInformation(&tzi);
			SYSTEMTIME ust;
			memcpy(&ust, &st, sizeof(ust));
			SystemTimeToTzSpecificLocalTime(&tzi, &ust, &st);
		}

		str.printf(L"%04d-%02d-%02d %02d:%02d:%02d.%03d", (int)st.wYear, (int)st.wMonth, (int)st.wDay, (int)st.wHour,
			(int)st.wMinute, (int)st.wSecond, (int)st.wMilliseconds);
	}
	return str;

}

void __fastcall TOpcConfigEditor::FormClose(TObject *Sender, TCloseAction &Action)
{
	Action = caFree;
	opc_close_server();
}
// ---------------------------------------------------------------------------

void __fastcall TOpcConfigEditor::fill_opc_items()
{
	OpcServerItemsTree->Items->BeginUpdate();
	OpcServerItemsTree->Items->Clear();
	// divisor = UnicodeString();
	// prefix  = UnicodeString();

	if (opc_server && opc_server->IsBound())
	{
		if (!opc_browser)
			opc_browser = new opc::TOpcBrowseServerAddrSpace(*opc_server);
		is_tree_model = opc_browser->is_tree_model();
		if (is_tree_model)
			fill_opc_items_tree(NULL);
		else
			fill_opc_items_flat();

	}
	OpcServerItemsTree->Items->EndUpdate();
}

void __fastcall TOpcConfigEditor::fill_opc_items_tree(TTreeNode * node)
{
	OpcServerItemsTree->Items->BeginUpdate();
	set_current_group(node, opc_browser);

	if (SUCCEEDED(opc_browser->items_enum(OPC_BRANCH, L"", VT_EMPTY, 0)))
	{
		UnicodeString str;
		while (S_OK == opc_browser->item_next(str))
		{
			TTreeNode * folder_node = OpcServerItemsTree->Items->AddChild(node, str);
			folder_node->Data = new tree_item_def;
			OpcServerItemsTree->Items->AddChild(folder_node, UnicodeString());
		}
	}

	if (SUCCEEDED(opc_browser->items_enum(node ? OPC_LEAF : OPC_LEAF, edFilter->Text.c_str())))
	{
		UnicodeString str;
		while (S_OK == opc_browser->item_next(str))
		{
			TTreeNode * item_node = OpcServerItemsTree->Items->AddChild(node, str);
			UnicodeString id;
			opc_browser->item_id(str.c_str(), id);
			item_node->Data = new tree_item_def(id);
			check_opc_item_in_use(item_node);
		}
	}

	OpcServerItemsTree->Items->EndUpdate();
}

// void    __fastcall TOpcConfigEditor::fill_opc_items_tree(TTreeNode * parent)
// {
// if(parent)
// opc_browser->group_down(parent->Text.c_str());
//
// if(S_OK == opc_browser->items_enum(OPC_BRANCH,L"", VT_EMPTY,0))
// {
// UnicodeString str;
// while(S_OK == opc_browser->item_next(str))
// {
// TTreeNode * node = OpcServerItemsTree->Items->AddChild(parent,str);
// node->Data = (LPVOID)-1;
// }
// }
//
// TTreeNode * group_node = parent ? parent->getFirstChild() : OpcServerItemsTree->Items->GetFirstNode();
// while(group_node)
// {
//
// fill_opc_items_tree(group_node);
// group_node = group_node->getNextSibling();
// }
//
// if(opc_browser->items_enum(OPC_LEAF)== S_OK)
// {
// UnicodeString str;
// while(S_OK == opc_browser->item_next(str))
// {
// OpcServerItemsTree->Items->AddChild(parent,str);
// }
// }
//
// if(parent)
// opc_browser->group_up(parent->Text.c_str());
//
////    {
////     OpcServerItemsTree->Items->EndUpdate() ;
////     Application->ProcessMessages() ;
////     OpcServerItemsTree->Items->BeginUpdate() ;
////    }
//
// }

void __fastcall TOpcConfigEditor::fill_opc_items_flat()
{
	OpcServerItemsTree->Items->BeginUpdate();
	this->set_current_group(NULL, opc_browser);
	HRESULT res = opc_browser->items_enum(OPC_FLAT, edFilter->Text.c_str());
	if (SUCCEEDED(res))
	{
		UnicodeString str;
		while (S_OK == opc_browser->item_next(str))
		{

			TTreeNode * node = OpcServerItemsTree->Items->Add(NULL, str);
			node->Data = new tree_item_def(str);
			check_opc_item_in_use(node);

		}
	}
	OpcServerItemsTree->Items->EndUpdate();

}

void __fastcall TOpcConfigEditor::tbRefreshOpcItemsClick(TObject *Sender)
{
	if (!opc_server || !opc_server->IsBound())
		this->opc_open_server();

	TTreeNode * node = OpcServerItemsTree->Selected;

	if (!is_tree_model)
		fill_opc_items();
	else
	{
		OpcServerItemsTree->Items->BeginUpdate();
		node->DeleteChildren();
		OpcServerItemsTree->Items->AddChild(node, UnicodeString());

		bool allow_expand = true;
		OpcServerItemsTreeExpanding(OpcServerItemsTree, node, allow_expand);
		node->Expand(false);
		OpcServerItemsTree->Items->EndUpdate();
	}
}

// ---------------------------------------------------------------------------
void __fastcall TOpcConfigEditor::Timer1Timer(TObject *Sender)
{
	opc_server_refresh_info();
}

// ---------------------------------------------------------------------------
TListItem * __fastcall get_list_item(TListView * lv, int idx = -1)
{
	TListItem * item;
	if (idx >= 0 && idx < lv->Items->Count)
		item = lv->Items->Item[idx];
	else
	{
		item = lv->Items->Add();
		item->SubItems->Add(UnicodeString());
	}
	return item;
}

void __fastcall TOpcConfigEditor::setup_item_result(OPCITEMRESULT * iresult)
{
	TListItem * item;
	item = get_list_item(opc_item_props_view);
	item->Caption = L"HSERVER";
	item->SubItems->Strings[0] = iresult->hServer;
	item = get_list_item(opc_item_props_view);
	item->Caption = L"Canonical Data Type";
	item->SubItems->Strings[0] = variant_type_name((DWORD)iresult->vtCanonicalDataType);
	item = get_list_item(opc_item_props_view);
	item->Caption = L"Access Rights";
	item->SubItems->Strings[0] = iresult->dwAccessRights;
}

// ---------------------------------------------------------------------------
void __fastcall TOpcConfigEditor::setup_opc_item_props_list(DWORD prop_count, LPDWORD prop_ids, LPWSTR * prop_descr,
	LPVARIANT prop_values)
{
	for (DWORD i = 0; i < prop_count; i++)
	{
		TListItem * item;
		item = get_list_item(opc_item_props_view);
		item->Caption = *prop_descr;
		OleVariant ov(*prop_values);
		UnicodeString prop_str;
		if (ov.IsArray())
		{
			int array_count = ov.ArrayDimCount();

			for (int i = 0; i < array_count; i++)
			{
				if (!prop_str.IsEmpty())
					prop_str += ", ";
				try
				{
					TVariant v = ov.GetElement(i);
					prop_str += v;
				}
				catch (...)
				{
					prop_str += "?";
				}
			}
		}
		else
		{

			switch (*prop_ids)
			{
			case OPC_PROPERTY_DATATYPE:
				prop_str = variant_type_name((DWORD)ov);
				break;
			default:
				prop_str = ov;
				break;
			};
		}
		if (item->SubItems->Count)
			item->SubItems->Strings[0] = prop_str;
		else
			item->SubItems->Add(prop_str);

		++prop_ids;
		++prop_descr;
		++prop_values;
	}

	while (opc_item_props_view->Items->Count > (int)prop_count)
		opc_item_props_view->Items->Item[opc_item_props_view->Items->Count - 1]->Delete();

}

void __fastcall TOpcConfigEditor::com_mem_free(DWORD count, LPWSTR * ptr)
{
	LPWSTR * _ptr = ptr;
	LPWSTR *_ptr_end = _ptr + count;
	while (ptr && _ptr < _ptr_end)
	{
		com_mem_free(*_ptr);
		++_ptr;
	}
	com_mem_free(ptr);
}

void __fastcall TOpcConfigEditor::com_mem_free(DWORD count, LPVARIANT ptr)
{

	LPVARIANT _ptr = ptr;
	LPVARIANT _ptr_end = _ptr + count;
	while (ptr && _ptr < _ptr_end)
	{
		VariantClear(_ptr);
		// if(_ptr->vt&VT_ARRAY)
		// {
		// SafeArrayDestroy(_ptr->parray);
		// }
		// else
		// {
		// if(_ptr->vt == VT_BSTR)
		// SysFreeString(_ptr->bstrVal);
		// }
		++_ptr;
	}
	com_mem_free(ptr);
}

// void    __fastcall TOpcConfigEditor::setup_opc_item_props_list(DWORD prop_count,LPWSTR * prop_descr,LPVARIANT  prop_values)
// {
//
// }

void __fastcall TOpcConfigEditor::show_opc_error(HRESULT res)
{
	UnicodeString str;
	if (FAILED(res) && opc_server->IsBound())
	{
		wchar_t *err_str = NULL;
		(*opc_server)->GetErrorString(res, 0, &err_str);
		if (err_str)
		{
			str = err_str;
			CoTaskMemFree(err_str);
		}
	}
	StatusBar1->Panels->Items[1]->Text = str;
}

HRESULT __fastcall TOpcConfigEditor::add_to_opc_group(const wchar_t * item_id, OPCITEMRESULT * iresult)
{
	if (!opc_group)
	{
		opc_group = new opc::TOpcGroup(*opc_server);
		opc_group->create_group(L"TestGroup", 0, 0, 0);
		opc_group->set_active(true);
	}

	return opc_group->add_item(opc_group->get_group_count(), item_id, NULL, iresult);
}

void __fastcall TOpcConfigEditor::OpcServerItemsTreeChange(TObject *Sender, TTreeNode *Node)

{
	// Выбор другого
	if (!iprops.IsBound())
		iprops = *opc_server;
	HRESULT res = S_OK;
	tree_item_def * idef = get_tree_item_def(Node);
	if (idef && !idef->is_branch)
	{
		DWORD prop_count = 0;
		LPDWORD prop_ids = NULL;
		LPWSTR * prop_descr = NULL;
		LPWORD prop_data_types = NULL;
		UnicodeString opc_id = get_item_opc_id(Node);
		OPCITEMRESULT iresult;
		opc_item_props_view->Clear();
		res = add_to_opc_group(opc_id.c_str(), &iresult);
		StatusBar1->Panels->Items[0]->Text = opc_id;
		StatusBar1->Panels->Items[1]->Text = UnicodeString();

		if (SUCCEEDED(res))
		{
			setup_item_result(&iresult);
			res = iprops->QueryAvailableProperties(opc_id.c_str(), &prop_count, &prop_ids, &prop_descr,
				&prop_data_types);
			if (SUCCEEDED(res))
			{
				LPVARIANT prop_values = NULL;
				LPLONG errors;
				res = iprops->GetItemProperties(opc_id.c_str(), prop_count, prop_ids, &prop_values, &errors);
				// if(res)
				// {
				// res = iprops->GetItemProperties(opc_id.c_str(),prop_count,prop_ids,&prop_values,&errors);
				// }
				if (SUCCEEDED(res))
					setup_opc_item_props_list(prop_count, prop_ids, prop_descr, prop_values);
				com_mem_free(prop_count, prop_values);
				com_mem_free(errors);
			}
			opc_group->remove_item(0);
		}

		com_mem_free(prop_ids);
		com_mem_free(prop_data_types);
		com_mem_free(prop_count, prop_descr);
	}
	else
		opc_item_props_view->Clear();

	show_opc_error(res);
	tbAddItem->Enabled = check_enable_add_item();
	tbRemove->Enabled = check_enable_delete_item();

}

// ---------------------------------------------------------------------------
void __fastcall TOpcConfigEditor::FormResize(TObject *Sender)
{
	StatusBar1->Panels->Items[0]->Width = OpcServerItemsTree->Width;
}

// ---------------------------------------------------------------------------
void __fastcall TOpcConfigEditor::OtdTreeChange(TObject *Sender, TTreeNode *Node)

{
	UnicodeString str;
	bool item_panel_visible = false;

	if (Node)
	{
		group_param_t gp(Node->Data);
		sotd_param_addr sap = gp.get_param_addr();
		wchar_t * fa_names[] =
		{L"Дискретные", L"Аналоговые", L"Интегральные"};
		str += fa_names[sap.addr.fa];
		if (is_group(Node))
		{
			str.cat_printf(L", Группа № %d", (DWORD)sap.addr.sb);
			tbRenumber->Enabled = true;
		}
		else
			tbRenumber->Enabled = false;

		if (is_param(Node))
		{
			str.cat_printf(L", Группа № %d - %d", (DWORD)sap.addr.sb, (DWORD)sap.param_number);
			gkopc_items_vector::index_iterator iptr;
			gkopc_item_def_vcl oi(UnicodeString(), UnicodeString(), gp);
			if (opc_items.find(oi, iptr))
			{
				gkopc_item_def_vcl & item = opc_items.at(*iptr);
				str.cat_printf(L"{%s}", item.id.c_str());
				if (item.opc_node && item.opc_node->Owner->Owner->SelectionCount < 2)
				{
					item.opc_node->MakeVisible();
					item.opc_node->Selected = true;
				}
				item_panel_visible = true;
				set_item_personal_params(item, false);
			}
		}
	}
	else
		tbRenumber->Enabled = false;

	tbAddItem->Enabled = check_enable_add_item();
	tbRemove->Enabled = check_enable_delete_item();
	tbMoveUp->Enabled = tbRemove->Enabled;
	tbMoveDown->Enabled = tbMoveUp->Enabled;
	item_panel->Visible = item_panel_visible;
	miCopyTagName->Enabled = item_panel_visible;
	ApplyPersonal->Visible = item_panel_visible;
	UndoPersonal->Visible = item_panel_visible;
	StatusBar1->Panels->Items[3]->Text = str;
}
// ---------------------------------------------------------------------------

TTreeNode * __fastcall TOpcConfigEditor::get_root_node(TTreeNode * node)
{
	while (node && node->Parent)
		node = node->Parent;
	return node;
}

TTreeNode * __fastcall TOpcConfigEditor::create_group_node(TTreeNode * node, DWORD num)
{
	// Создание группы
	TTreeNode * root_node = get_root_node(node);
	if (root_node)
	{
		group_param_t gp(root_node->Data);
		UnicodeString str;
		if (num == (DWORD) - 1)
		{
			if (root_node->Count)
				gp.sb = group_param_t(root_node->Item[root_node->Count - 1]->Data).sb + 1;
			else
				gp.sb = 0;
		}
		else
			gp.sb = num;
		str.cat_printf(L"Группа № %d", DWORD(gp.sb));
		node = OtdTree->Items->AddChild(root_node, str);
		node->Data = (LPVOID)gp.group_data;
		root_node->Expand(false);
		return node;
	}
	return NULL;
}

void __fastcall TOpcConfigEditor::tbCreateGroupClick(TObject *Sender)
{
	create_group_node(OtdTree->Selected);
}
// ---------------------------------------------------------------------------

void __fastcall TOpcConfigEditor::tbRemoveClick(TObject *Sender)
{
	TTreeNode * node = OtdTree->Selected;
	if (node && !is_group_root(node))
	{
		node->DeleteChildren();
		node->Delete();
		changes = true;
	}

}
// ---------------------------------------------------------------------------

bool __fastcall TOpcConfigEditor::check_enable_add_item()
{
	bool ret = false;
	if (OpcServerItemsTree->Selected && OtdTree->Selected && !is_group_root(OtdTree->Selected))
	{
		tree_item_def * idef = get_tree_item_def(OpcServerItemsTree->Selected);
		if (idef)
			ret = !idef->is_branch;

	}
	return ret;
}

bool __fastcall TOpcConfigEditor::check_enable_delete_item()
{
	return OtdTree->Selected && !is_group_root(OtdTree->Selected);
}

bool __fastcall TOpcConfigEditor::add_gkopc_item(UnicodeString id, UnicodeString access_path, group_param_t & gp,
	gkopc_items_vector::iterator & ptr)
{
	gkopc_item_def_vcl oi(id, access_path, gp);
	gkopc_items_vector::index_iterator iptr;
	if (!opc_items.find(oi, ptr) && !opc_items.find(oi, iptr))
	{
		ptr = opc_items.insert(ptr, oi);
		changes = true;
		return true;
	}
	return false;
}

UnicodeString __fastcall TOpcConfigEditor::get_param_text(DWORD pn)
{
	UnicodeString str;
	str.printf(L"Параметр %u", pn);
	return str;
}

gkopc_items_vector::iterator __fastcall TOpcConfigEditor::add_gkopc_item(UnicodeString id, UnicodeString access_path,
	group_param_t & gp, TTreeNode * opc_node, TTreeNode * otd_node)
{

	gkopc_items_vector::iterator ptr;

	if (add_gkopc_item(id, access_path, gp, ptr))
	{
		if (opc_node)
		{
			// opc_node->Text = id;
			ptr->opc_node = opc_node;
			tree_item_def * idef = get_tree_item_def(opc_node);
			if (idef)
				idef->in_use = true;
		}
		if (otd_node)
		{
			otd_node->Text = get_param_text((DWORD)gp.param);
			otd_node->Data = (LPVOID)gp.group_data;
			ptr->otd_node = otd_node;
		}
	}

	return ptr;
}

void __fastcall TOpcConfigEditor::add_gkopc_item(TTreeNode * src_node, TTreeNode * dst_node)
{
	if (!is_group(dst_node))
		dst_node = dst_node->Parent;
	group_param_t gp(dst_node->Data);

	gp.param = 0;
	if (dst_node->Count)
		gp.param = group_param_t(dst_node->Item[dst_node->Count - 1]->Data).param + 1;
	TTreeNode * otd_node = OtdTree->Items->AddChild(dst_node, UnicodeString());
	add_gkopc_item(get_item_opc_id(src_node), UnicodeString(), gp, src_node, otd_node);
}

void __fastcall TOpcConfigEditor::tbAddItemClick(TObject *Sender)
{
	// Добаваление элемента
	TTreeNode * node = OtdTree->Selected;
	for (DWORD i = 0; i < OpcServerItemsTree->SelectionCount; i++)
	{
		TTreeNode * src_node = OpcServerItemsTree->Selections[i];
		tree_item_def * idef = get_tree_item_def(src_node);
		if (idef && !idef->is_branch && !idef->in_use)
			add_gkopc_item(src_node, node);
	}
	node->Expanded = true;

}
// ---------------------------------------------------------------------------

void __fastcall TOpcConfigEditor::OpcServerItemsTreeCustomDrawItem(TCustomTreeView *Sender, TTreeNode *Node,
	TCustomDrawState State, bool &DefaultDraw)

{
	TTreeView * trees = dynamic_cast<TTreeView*>(Sender);
	TFontStyles fs = trees->Font->Style;
	TColor color = trees->Font->Color;
	tree_item_def * idef = get_tree_item_def(Node);
	if (idef)
	{
		if (!State.Contains(cdsSelected))
		{
			if (idef->in_use)
				color = clBlue;
			trees->Canvas->Font->Color = color;
		}

		if (idef->is_branch)
			fs << fsBold;
		trees->Canvas->Font->Style = fs;
	}

	DefaultDraw = true;
}
// ---------------------------------------------------------------------------

void __fastcall TOpcConfigEditor::check_opc_item_in_use(TTreeNode *Node)
{
	tree_item_def * idef = get_tree_item_def(Node);
	if (idef)
	{

		gkopc_item_def_vcl oi(get_item_opc_id(Node), UnicodeString(), group_param_t());
		gkopc_items_vector::iterator ptr;
		if (opc_items.find(oi, ptr))
		{
			ptr->opc_node = Node;
			idef->in_use = true;
		}
	}
}

void __fastcall TOpcConfigEditor::OpcServerItemsTreeDeletion(TObject *Sender, TTreeNode *Node)

{
	tree_item_def * idef = get_tree_item_def(Node);
	if (idef)
	{
		if (idef->is_branch)
		{
			gkopc_item_def_vcl oi(Node->Text, UnicodeString(), group_param_t());
			gkopc_items_vector::iterator ptr;
			if (opc_items.find(oi, ptr))
				ptr->opc_node = NULL;
		}
		delete_item_def(Node);
	}

}
// ---------------------------------------------------------------------------

void __fastcall TOpcConfigEditor::OtdTreeDeletion(TObject *Sender, TTreeNode *Node)

{
	group_param_t gp(Node->Data);
	gkopc_items_vector::index_iterator iptr;
	gkopc_item_def_vcl oi(UnicodeString(), UnicodeString(), gp);
	if (opc_items.find(oi, iptr))
	{
		gkopc_items_vector::iterator ptr = opc_items.begin();
		std::advance(ptr, *iptr);
		tree_item_def * idef = get_tree_item_def(ptr->opc_node);
		if (idef)
			idef->in_use = false;
		opc_items.erase(ptr);
	}
}
// ---------------------------------------------------------------------------

void __fastcall TOpcConfigEditor::set_changes(bool ch)
{
	miSave->Enabled = ch;
}

bool __fastcall TOpcConfigEditor::get_changes()
{
	return miSave->Enabled;
}

void __fastcall TOpcConfigEditor::read_config()
{
	OtdTree->Items->BeginUpdate();
	discrete_node->DeleteChildren();
	analog1_node->DeleteChildren();
	analog2_node->DeleteChildren();
	opc_items.clear();
	if (!opc_config_name.IsEmpty())
	{
		gkopc_items_io io;
		KeRTL::TFileStream fs(opc_config_name.c_str());
		io.read(opc_items, &fs);
		if (opc_items.size())
			build_otd_tree();
	}
	OtdTree->Items->EndUpdate();
}

TTreeNode*__fastcall TOpcConfigEditor::get_otd_root_node(BYTE fa)
{
	TTreeNode * ret = NULL;
	switch (fa)
	{
	case OTD_FA_DISCRETE:
		ret = discrete_node;
		break;
	case OTD_FA_ANALOG:
		ret = analog1_node;
		break;
	case OTD_FA_ANALOG_INTEGR:
		ret = analog2_node;
		break;
	}
	return ret;
}

void __fastcall TOpcConfigEditor::build_otd_tree()
{
	opc_items.index_invalidate();
	opc_items.build_index();
	gkopc_items_vector::index_iterator iptr = opc_items.index_begin();
	gkopc_items_vector::index_iterator iend = opc_items.index_end();
	while (iptr < iend)
	{
		gkopc_item_def_vcl lo_item(UnicodeString(), UnicodeString(), opc_items.at(*iptr).group_param);
		gkopc_item_def_vcl hi_item(lo_item);
		hi_item.group_param.param = -1;
		TTreeNode * root = get_otd_root_node(lo_item.group_param.fa);
		if (root && opc_items.range(lo_item, hi_item, iptr, iend))
		{
			root = create_group_node(root, lo_item.group_param.sb);
			while (iptr < iend)
			{
				gkopc_item_def_vcl * opc_item = &opc_items.at(*iptr);
				TTreeNode * node = root->Owner->AddChild(root, get_param_text((DWORD)opc_item->group_param.param));
				node->Data = (LPVOID)opc_item->group_param.group_data;
				opc_item->otd_node = node;
				++iptr;
			};
		}
		iptr = iend;
		iend = opc_items.index_end();
	}
}

void __fastcall TOpcConfigEditor::write_config()
{
	gkopc_items_io io;
	KeRTL::TFileStream fs(opc_config_name.c_str());
	io.write(opc_items, &fs);
	changes = false;
}

void __fastcall TOpcConfigEditor::miSaveClick(TObject *Sender)
{
	write_config();
}
// ---------------------------------------------------------------------------

void __fastcall TOpcConfigEditor::miSaveAsClick(TObject *Sender)
{
	if (this->SaveDialog1->Execute(Handle))
	{
		FConfigFile = SaveDialog1->FileName;
		write_config();
	}
}
// ---------------------------------------------------------------------------

void __fastcall TOpcConfigEditor::Memo1DblClick(TObject *Sender)
{
	// Memo1->Lines->Clear();
	// UnicodeString str;
	// gkopc_items_vector::iterator ptr =  opc_items.begin();
	// gkopc_items_vector::iterator end =  opc_items.end();
	//
	// gkopc_items_vector::index_iterator iptr =  opc_items.index_begin();
	// gkopc_items_vector::index_iterator iend =  opc_items.index_end();
	// str.printf(L"size %d  iterator distance %d  index distance %d",opc_items.size(),std::distance(ptr,end),std::distance(iptr,iend));
	// Memo1->Lines->Add(str);
	// gkopc_item_def_vcl  oi ;
	// while(ptr<end)
	// {
	// oi = *ptr;
	// str.printf(L"id %s  gp %02u.%02u.%02u",oi.id.c_str(),(DWORD)oi.group_param.fa,(DWORD)oi.group_param.sb,(DWORD)oi.group_param.param);
	// Memo1->Lines->Add(str);
	// ++ptr;
	// }
	// Memo1->Lines->Add(L"----------------");
	// while(iptr<iend)
	// {
	// DWORD idx = *iptr;
	// oi = opc_items.at(idx);
	// str.printf(L"id %s  gp %02u.%02u.%02u",oi.id.c_str(),(DWORD)oi.group_param.fa,(DWORD)oi.group_param.sb,(DWORD)oi.group_param.param);
	// Memo1->Lines->Add(str);
	// ++iptr;
	// }
}

// ---------------------------------------------------------------------------
void __fastcall TOpcConfigEditor::move_otd_group_node(TTreeNode * node, bool down)
{

}

inline bool __fastcall is_param_sequenced(const group_param_t gp1, const group_param_t gp2)
{
	return (gp1.param < gp2.param ? (gp2.param - gp1.param) : (gp1.param - gp2.param)) == 1 ? true : false;
}

void __fastcall TOpcConfigEditor::move_otd_node(TTreeNode * node, bool down)
{
	if (node)
	{
		int delta = down ? 1 : -1;
		gkopc_item_def_vcl i1;
		i1.group_param.group_data = (DWORD)node->Data;
		gkopc_items_vector::index_iterator iptr1;
		if (opc_items.find(i1, iptr1))
		{
			TTreeNode * next_node = down ? node->getNextSibling() : node->getPrevSibling();
			group_param_t gp(next_node ? next_node->Data : NULL);
			if (next_node && is_param_sequenced(i1.group_param.group_data, gp))
			{
				gkopc_item_def_vcl i2;
				i2.group_param.group_data = (DWORD)next_node->Data;
				gkopc_items_vector::index_iterator iptr2;
				if (opc_items.find(i2, iptr2))
				{
					gkopc_item_def_vcl &oi1 = opc_items.at(*iptr1);
					gkopc_item_def_vcl &oi2 = opc_items.at(*iptr2);
					std::swap(oi1.group_param, oi2.group_param);
					std::swap(oi1.otd_node, oi2.otd_node);
					std::swap(*iptr1, *iptr2);
					next_node->Selected = true;
					changes = true;
				}
			}
			else
			{
				if (delta > 0 || i1.group_param.param)
				{
					i1.group_param.param += delta;
					opc_items.at(*iptr1).group_param.group_data = i1.group_param.group_data;
					node->Data = (LPVOID)i1.group_param.group_data;
					// Перестроение индексов не треба !!!
					node->Text = get_param_text(i1.group_param.param);
					changes = true;
					OtdTreeChange(OtdTree, node);
				}
			}

		}
	}
}

void __fastcall TOpcConfigEditor::tbMoveUpClick(TObject *Sender)
{

	move_otd_node(OtdTree->Selected, false);

}
// ---------------------------------------------------------------------------

void __fastcall TOpcConfigEditor::tbMoveDownClick(TObject *Sender)
{
	move_otd_node(OtdTree->Selected, true);
}
// ---------------------------------------------------------------------------

void __fastcall TOpcConfigEditor::set_otd_node_group_param(TTreeNode * node, const group_param_t &gp)
{
	node->Data = (LPVOID)gp.group_data;
	node->Text = get_param_text(gp.param);
}

void __fastcall TOpcConfigEditor::swap_otd_nodes(gkopc_item_def_vcl & i1, gkopc_item_def_vcl & i2)
{
	group_param_t gp1 = i1.group_param;
	group_param_t gp2 = i2.group_param;
	set_otd_node_group_param(i1.otd_node, gp2);
	set_otd_node_group_param(i2.otd_node, gp1);
	opc_items.index_invalidate();
}

void __fastcall TOpcConfigEditor::swap_otd_nodes(TTreeNode * node1, TTreeNode * node2)
{
	if (node1 && node2)
	{

	}
}

void __fastcall TOpcConfigEditor::renumber_otd_group(TTreeNode * group_node)
{
	std::vector<DWORD>ioffsets;
	TTreeNode * item_node;
	gkopc_item_def_vcl item;
	for (DWORD i = 0; i < (DWORD)group_node->Count; i++)
	{
		item_node = group_node->Item[i];
		item.group_param = group_param_t(item_node->Data);
		gkopc_items_vector::index_iterator iptr;
		if (opc_items.find(item, iptr))
			ioffsets.push_back(*iptr);
	}

	std::vector<DWORD>::iterator ptr = ioffsets.begin();
	std::vector<DWORD>::iterator end = ioffsets.end();
	DWORD pn = 0;
	while (ptr < end)
	{
		gkopc_item_def_vcl & _item = opc_items.at(*ptr);
		_item.group_param.param = pn;
		_item.otd_node->Data = (LPVOID) _item.group_param.group_data;
		_item.otd_node->Text = get_param_text(pn);
		++pn;
		++ptr;
		changes = true;
	}
}

void __fastcall TOpcConfigEditor::tbRenumberClick(TObject *Sender)
{
	renumber_otd_group(OtdTree->Selected);
}
// ---------------------------------------------------------------------------

UnicodeString __fastcall get_schneider_opc_id(TTreeNode * node)
{
	UnicodeString str;
	if (node)
	{
		if (!node->Parent)
			return node->Text;
		if (!node->Parent->Parent)
			str.printf(L"%s!%s", node->Parent->Text.c_str(), node->Text.c_str());
		else
		{
			TTreeNode * pnode = node->Parent;
			while (pnode)
			{
				UnicodeString tmp_str = str;
				str = pnode->Text;
				if (!tmp_str.IsEmpty())
					str.cat_printf(L"!%s", tmp_str.c_str());
				pnode = pnode->Parent;
			}
			tree_item_def * idef = get_tree_item_def(node);
			str.cat_printf(L"%c%s", idef && !idef->is_branch ? L'.' : L'!', node->Text);
		}
	}
	return str;
}

UnicodeString __fastcall TOpcConfigEditor::get_item_opc_id(TTreeNode * node)
{
	UnicodeString str;
	if (node->Parent)
	{
		tree_item_def * idef = get_tree_item_def(node);
		if (idef)
			str = idef->item_name;
		else
			str = node->Text;
	}
	else
		str = node->Text;
	return str;
}

void __fastcall TOpcConfigEditor::opc_item_props_viewResize(TObject *Sender)
{
	int col_width = opc_item_props_view->Width - GetSystemMetrics(SM_CXVSCROLL);
	col_width -= opc_item_props_view->Columns->Items[0]->Width;
	opc_item_props_view->Columns->Items[1]->Width = col_width;

}
// ---------------------------------------------------------------------------

void __fastcall TOpcConfigEditor::set_item_personal_params(gkopc_item_def_vcl & item, bool from_controls)
{
	if (from_controls)
	{

		UnicodeString str = tutr_object->Text.Trim();
		DWORD rc_object = (str.IsEmpty() ? -1 : _wtoi(str.c_str()));
		if (item.rc_object != rc_object)
		{
			item.rc_object = rc_object;
			this->changes = true;
		}

		if (wcscmp(item.rc_tag.c_str(), tutr_tag->Text.Trim().c_str()))
		{
			changes = true;
			item.rc_tag = tutr_tag->Text.Trim().c_str();
		}

		AnsiString rc_script(this->TuTrScript->Text.Trim());
		if (strcmp(item.rc_script.c_str(), rc_script.c_str()))
		{
			item.rc_script = rc_script.c_str();
			changes = true;
		}

		AnsiString calc_script(CalcScript->Text.Trim());
		if (strcmp(calc_script.c_str(), item.calc_script.c_str()))
		{
			item.calc_script = calc_script.c_str();
			changes = true;
		}
	}
	else
	{
		if (item.rc_object != DWORD(-1))
			this->tutr_object->Text = item.rc_object;
		else
			this->tutr_object->Text = UnicodeString();
		this->tutr_tag->Text = item.rc_tag.c_str();
		this->CalcScript->Text = UnicodeString(item.calc_script.c_str());
		this->TuTrScript->Text = UnicodeString(item.rc_script.c_str());
	}

}

void __fastcall TOpcConfigEditor::set_item_personal_params(TTreeNode * node, bool from_controls)
{
	group_param_t gp(node->Data);
	gkopc_items_t::index_iterator iptr;
	gkopc_item_def_vcl oi(UnicodeString(), UnicodeString(), gp);
	if (opc_items.find(oi, iptr))
	{
		gkopc_item_def_vcl & item = this->opc_items.at(*iptr);
		set_item_personal_params(item, from_controls);
	}

}

void __fastcall TOpcConfigEditor::tbSaveClick(TObject *Sender)
{
	if (miSave->Enabled)
		miSaveClick(this);
	else
		miSaveAsClick(this);
}
// ---------------------------------------------------------------------------

void __fastcall TOpcConfigEditor::OpcServerItemsTreeExpanding(TObject *Sender, TTreeNode *Node, bool &AllowExpansion)
{
	// Вот тут начинаем  читать все что принадлежит группе
	if (Node->Count < 2)
	{
		TTreeNode * first_child = Node->getFirstChild();
		if (first_child && !first_child->Data)
		{
			OpcServerItemsTree->Cursor = crHourGlass;
			fill_opc_items_tree(Node);
			if (Node->Count > 1)
				first_child->Delete();
			else
				AllowExpansion = false;
			OpcServerItemsTree->Cursor = crDefault;

		}
	}

	// if(Node->Data == FOLDER_NODE_ATTR)
	// {
	// Node->Owner->BeginUpdate();
	// TTreeNode *  first_child =  Node->getFirstChild();
	// if(!first_child || ( first_child && first_child->Text.IsEmpty() &&  first_child->Data == FOLDER_TEMPL_ATTR))
	// {
	// OpcServerItemsTree->Cursor = crHourGlass;
	// fill_opc_items_tree(Node);
	// if(first_child ) first_child->Delete();
	// OpcServerItemsTree->Cursor = crDefault;
	// }
	// Node->Owner->EndUpdate();
	// }

}
// ---------------------------------------------------------------------------

void __fastcall TOpcConfigEditor::set_current_group(TTreeNode * node, opc::TOpcBrowseServerAddrSpace * opc_browser)
{
	if (node)
	{
		set_current_group(node->Parent, opc_browser);
		opc_browser->group_down(node->Text.c_str());
	}
	else
	{
		while (opc_browser->group_up(L"") == S_OK);
	}

}

void __fastcall TOpcConfigEditor::miCopyTagNameClick(TObject *Sender)
{
	TTreeNode * node = OpcServerItemsTree->Selected;
	if (node)
	{
		tutr_tag->Text = get_item_opc_id(node);
		changes = true;
	}

}
// ---------------------------------------------------------------------------

void __fastcall TOpcConfigEditor::ApplyPersonalClick(TObject *Sender)
{
	this->set_item_personal_params(OtdTree->Selected, true);
}
// ---------------------------------------------------------------------------

void __fastcall TOpcConfigEditor::UndoPersonalClick(TObject *Sender)
{
	this->set_item_personal_params(OtdTree->Selected, false);
}
// ---------------------------------------------------------------------------

std::string screen_lf(std::string src, bool revert)
{

	std::string res = src;
	static char line_feed_1[] = "\n";
	static char line_feed_2[] = "\r";
	static char lf_screen_1[] = "-n";
	static char lf_screen_2[] = "-l";
	static char comma_text[] = ",";
	static char comma_screen[] = "\\comma";
	if (revert)
	{
		boost::replace_all(res, lf_screen_1, line_feed_1);
		boost::replace_all(res, lf_screen_2, line_feed_2);
		boost::replace_all(res, comma_screen, comma_text);
	}
	else
	{
		boost::replace_all(res, line_feed_1, lf_screen_1);
		boost::replace_all(res, line_feed_2, lf_screen_2);
		boost::replace_all(res, comma_text, comma_screen);
	}
	return res;
}

std::wstring screen_lf(std::wstring src, bool revert)
{

	std::wstring res = src;
	static wchar_t line_feed_1[] = L"\n";
	static wchar_t line_feed_2[] = L"\r";
	static wchar_t lf_screen_1[] = L"-n";
	static wchar_t lf_screen_2[] = L"-l";
	static wchar_t comma_text[] = L",";
	static wchar_t comma_screen[] = L"\\comma";
	if (revert)
	{
		boost::replace_all(res, lf_screen_1, line_feed_1);
		boost::replace_all(res, lf_screen_2, line_feed_2);
		boost::replace_all(res, comma_screen, comma_text);
	}
	else
	{
		boost::replace_all(res, line_feed_1, lf_screen_1);
		boost::replace_all(res, line_feed_2, lf_screen_2);
		boost::replace_all(res, comma_text, comma_screen);
	}
	return res;
}

void __fastcall TOpcConfigEditor::do_export(UnicodeString file_name)
{

	std::auto_ptr<TStringList>strings(new TStringList);
	std::auto_ptr<TStringList>line(new TStringList);
	strings->QuoteChar = L'\'';
	strings->Delimiter = L',';
	line->QuoteChar = L'\'';
	line->Delimiter = L',';
	strings->Add(L"#OPC-Tag_Name,access_path,fa,sb,param,rc_object,rc_tag,calc_script,rc_script");
	gkopc_items_vector::iterator ptr = opc_items.begin();
	gkopc_items_vector::iterator end = opc_items.end();
	while (ptr < end)
	{
		line->Clear();
		UnicodeString str;
		str = ptr->id.c_str();
		line->Add(str.Trim());
		str = ptr->access_path.c_str();
		line->Add(str.Trim());
		str = ptr->group_param.fa;
		line->Add(str.Trim());
		str = ptr->group_param.sb;
		line->Add(str.Trim());
		str = ptr->group_param.param;
		line->Add(str.Trim());
		str = ptr->rc_object;
		line->Add(str.Trim());
		str = ptr->rc_tag.c_str();
		line->Add(str.Trim());
		str = screen_lf(ptr->calc_script, false).c_str();
		str = str.Trim();
		line->Add(str);
		str = screen_lf(ptr->rc_script, false).c_str();
		str = str.Trim();
		line->Add(str);
		line->Count;
		strings->Add(line->CommaText);
		++ptr;
	}

	if (strings->Count)
		strings->SaveToFile(file_name);
}

void __fastcall TOpcConfigEditor::do_import(UnicodeString file_name)
{
	std::auto_ptr<TStringList>strings(new TStringList);
	strings->QuoteChar = L'\'';
	strings->Delimiter = L',';
	strings->LoadFromFile(file_name);

	if (strings->Count)
	{
		OtdTree->Items->BeginUpdate();
		discrete_node->DeleteChildren();
		analog1_node->DeleteChildren();
		analog2_node->DeleteChildren();
		opc_items.clear();
		opc_items.index_invalidate();
		gkopc_item_def_vcl opc_item;

		std::auto_ptr<TStringList>line(new TStringList);
		line->QuoteChar = L'\'';
		line->Delimiter = L',';
		for (int i = 0; i < strings->Count; i++)
		{
			UnicodeString line_str = strings->Strings[i];
			if (line_str.Length() > 0 && *line_str.c_str() != L'#')
			{
				line->Clear();
				line->CommaText = line_str;
				if (line->Count > 8)
				{
					opc_item.id = line->Strings[0].c_str();
					opc_item.access_path = line->Strings[1].c_str();
					opc_item.group_param.fa = _wtoi(line->Strings[2].c_str());
					opc_item.group_param.sb = _wtoi(line->Strings[3].c_str());
					opc_item.group_param.param = _wtoi(line->Strings[4].c_str());
					opc_item.rc_object = _wtoi(line->Strings[5].c_str());
					opc_item.rc_tag = line->Strings[6].c_str();
					AnsiString astr;
					astr = screen_lf(std::wstring(line->Strings[7].c_str()), true).c_str();
					opc_item.calc_script = astr.c_str();
					astr = screen_lf(std::wstring(line->Strings[8].c_str()), true).c_str();
					opc_item.rc_script = astr.c_str();
					opc_items.insert(opc_item);
				}
			}
		}
		build_otd_tree();
		OtdTree->Items->EndUpdate();
	}
}

void __fastcall TOpcConfigEditor::bExportClick(TObject *Sender)
{
	// Экспорт списка элементов
	if (ExportDlg->Execute(Handle))
	{
		do_export(ExportDlg->FileName);
	}
}
// ---------------------------------------------------------------------------

float __fastcall TOpcConfigEditor::get_progress_percent()
{
	float perc = .0;
	if (this->progress_top)
		perc = float(100)*float(this->progress_current) / float(progress_top);
	return perc;
}

void __fastcall TOpcConfigEditor::StatusBar1DrawPanel(TStatusBar *StatusBar, TStatusPanel *Panel, const TRect &Rect)
{
	// Отрисовка панели прогресса
}
// ---------------------------------------------------------------------------

void __fastcall TOpcConfigEditor::bImportClick(TObject *Sender)
{
	if (ImportDlg->Execute(Handle))
	{
		do_import(ImportDlg->FileName);
	}

}
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------

void __fastcall TOpcConfigEditor::OpcServerItemsTreeCollapsing(TObject *Sender, TTreeNode *Node, bool &AllowCollapse)
{
	// while(Node->Count>1)
	// {
	// Node->GetLastChild()->Delete();
	// }
	// TTreeNode * child = Node->getFirstChild();
	// child->DeleteChildren();
	// delete_item_def(child);
}
// ---------------------------------------------------------------------------
