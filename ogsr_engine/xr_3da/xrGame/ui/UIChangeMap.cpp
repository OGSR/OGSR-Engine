#include "StdAfx.h"
#include "UIChangeMap.h"
#include "UIVotingCategory.h"
#include "UIXmlInit.h"
#include "UI3tButton.h"
#include "UIListBox.h"
#include "UIListBoxItem.h"
#include "../level.h"
#include "../game_cl_teamdeathmatch.h"
#include "../../xr_ioconsole.h"
#include "UIMapList.h"
#include "CExtraContentFilter.h"

#include "../object_broker.h"
#include "../UIGameCustom.h"

xr_token	game_types		[];

CUIChangeMap::CUIChangeMap()
{
	m_prev_upd_time = 0;

	bkgrnd = xr_new<CUIStatic>(); bkgrnd->SetAutoDelete(true);
	AttachChild(bkgrnd);

	header = xr_new<CUIStatic>(); header->SetAutoDelete(true);
	AttachChild(header);

	map_pic = xr_new<CUIStatic>(); map_pic->SetAutoDelete(true);
	AttachChild(map_pic);

	map_frame = xr_new<CUIStatic>(); map_frame->SetAutoDelete(true);
	AttachChild(map_frame);

	frame = xr_new<CUIFrameWindow>(); frame->SetAutoDelete(true);
	AttachChild(frame);

	lst_back = xr_new<CUIFrameWindow>(); lst_back->SetAutoDelete(true);
	AttachChild(lst_back);

	lst = xr_new<CUIListBox>(); lst->SetAutoDelete(true);
	AttachChild(lst);

	btn_ok = xr_new<CUI3tButton>(); btn_ok->SetAutoDelete(true);
	AttachChild(btn_ok);

	btn_cancel = xr_new<CUI3tButton>(); btn_cancel->SetAutoDelete(true);
	AttachChild(btn_cancel);

	m_pExtraContentFilter = xr_new<CExtraContentFilter>();
}
CUIChangeMap::~CUIChangeMap()
{
	delete_data(m_pExtraContentFilter);
}
void CUIChangeMap::Init(CUIXml& xml_doc)
{
	CUIXmlInit::InitWindow				(xml_doc,			"change_map", 0, this);
	CUIXmlInit::InitStatic				(xml_doc,			"change_map:header", 0, header);
	CUIXmlInit::InitStatic				(xml_doc,			"change_map:background", 0, bkgrnd);
	CUIXmlInit::InitStatic				(xml_doc,			"change_map:map_frame", 0, map_frame);
	CUIXmlInit::InitStatic				(xml_doc,			"change_map:map_pic", 0, map_pic); map_pic->ClipperOn();
	CUIXmlInit::InitFrameWindow			(xml_doc,			"change_map:list_back", 0, lst_back);
	CUIXmlInit::InitFrameWindow			(xml_doc,			"change_map:frame", 0, frame);
	CUIXmlInit::InitListBox				(xml_doc,			"change_map:list", 0, lst);
	CUIXmlInit::Init3tButton			(xml_doc,			"change_map:btn_ok", 0, btn_ok);
	CUIXmlInit::Init3tButton			(xml_doc,			"change_map:btn_cancel", 0, btn_cancel);

	map_pic->InitTexture				("ui\\ui_noise");

	FillUpList							();
}

#include <dinput.h>
bool CUIChangeMap::OnKeyboard(int dik, EUIMessages keyboard_action)
{
	if (dik == DIK_ESCAPE)
	{
		OnBtnCancel();
		return true;
	}
	return CUIDialogWnd::OnKeyboard(dik, keyboard_action);
}

void CUIChangeMap::SendMessage(CUIWindow* pWnd, s16 msg, void* pData)
{
	if (LIST_ITEM_SELECT==msg && pWnd==lst)
	{		
		OnItemSelect();
	}else
	if (LIST_ITEM_DB_CLICKED==msg && pWnd==lst)
	{		
		OnItemSelect();
		OnBtnOk();
	}
	else if (BUTTON_CLICKED == msg)
	{
		if (pWnd == btn_ok)
			OnBtnOk();
		else if (pWnd == btn_cancel)
			OnBtnCancel();
	}
}

void CUIChangeMap::OnItemSelect()
{
	u32 idx					= lst->GetSelectedIDX();
	if(idx==u32(-1))		return;

	const SGameTypeMaps& M	= gMapListHelper.GetMapListFor( (EGameTypes)GameID() );
	const shared_str& name	= M.m_map_names[idx];

	xr_string map_name		= "intro\\intro_map_pic_";
	map_name				+=	name.c_str();
	xr_string full_name		= map_name + ".dds";

	if (FS.exist("$game_textures$",full_name.c_str()))
		map_pic->InitTexture(map_name.c_str());
	else
		map_pic->InitTexture("ui\\ui_noise");
}

void CUIChangeMap::OnBtnOk()
{
	u32 idx						= lst->GetSelectedIDX();
	const SGameTypeMaps& M		= gMapListHelper.GetMapListFor( (EGameTypes)GameID() );
	if (idx>=0 && idx<M.m_map_names.size())
	{	
		const shared_str& name		= M.m_map_names[idx];

		string512					command;
        sprintf_s						(command, "cl_votestart changemap %s", name.c_str());
		Console->Execute			(command);
		GetHolder()->StartStopMenu	(this, true);
	}
}
#include "../string_table.h"
void CUIChangeMap::FillUpList()
{
	lst->Clear				();

	const SGameTypeMaps& M		= gMapListHelper.GetMapListFor( (EGameTypes)GameID() );
	u32 cnt						= M.m_map_names.size();
	for (u32 i=0; i<cnt; ++i)
	{
		CUIListBoxItem* itm		= lst->AddItem( CStringTable().translate(M.m_map_names[i]).c_str() );
		itm->Enable				(m_pExtraContentFilter->IsDataEnabled(M.m_map_names[i].c_str()));
	}

}

void CUIChangeMap::OnBtnCancel()
{
	GetHolder()->StartStopMenu	(this, true);
}
