#include "stdafx.h"

#include "UIInventoryUtilities.h"

#include "uicharacterinfo.h"
#include "../actor.h"
#include "../level.h"
#include "../character_info.h"
#include "../string_table.h"
#include "../relation_registry.h"

#include "xrUIXmlParser.h"
#include "UIXmlInit.h"

#include "uistatic.h"
#include "UIScrollView.h"


#include "../alife_simulator.h"
#include "../ai_space.h"
#include "../alife_object_registry.h"
#include "../xrServer.h"
#include "../xrServer_Objects_ALife_Monsters.h"

using namespace InventoryUtilities;

CSE_ALifeTraderAbstract* ch_info_get_from_id (u16 id)
{
	if( ai().get_alife() && ai().get_game_graph() )
	{
		return	smart_cast<CSE_ALifeTraderAbstract*>(ai().alife().objects().object(id));
	}else{
		return	smart_cast<CSE_ALifeTraderAbstract*>(Level().Server->game->get_entity_from_eid(id));
	}
}

CUICharacterInfo::CUICharacterInfo()
:m_ownerID(u16(-1)),pUIBio(NULL)
{
	ZeroMemory			(m_icons,sizeof(m_icons));
	m_bForceUpdate		= false;
}

CUICharacterInfo::~CUICharacterInfo()
{}

void CUICharacterInfo::Init(float x, float y, float width, float height, CUIXml* xml_doc)
{
	inherited::Init(x, y, width, height);

	CUIXmlInit xml_init;
	CUIStatic*	pItem = NULL;

	if(xml_doc->NavigateToNode("icon_static",0))	
	{
		pItem = m_icons[eUIIcon] = xr_new<CUIStatic>();
		xml_init.InitStatic	(*xml_doc, "icon_static", 0, pItem);
		pItem->ClipperOn	();
		pItem->Show			(true);
		pItem->Enable		(true);
		AttachChild			(pItem);
		pItem->SetAutoDelete(true);
	}

	if(xml_doc->NavigateToNode("name_static", 0)){
		pItem = m_icons[eUIName] = xr_new<CUIStatic>();
		xml_init.InitStatic(*xml_doc, "name_static", 0, pItem);
		pItem->SetElipsis(CUIStatic::eepEnd, 0);
		AttachChild(pItem);
		pItem->SetAutoDelete(true);
	}

	// rank
	if(xml_doc->NavigateToNode("rank_static", 0))
	{
		pItem = m_icons[eUIRank] = xr_new<CUIStatic>();
		xml_init.InitStatic(*xml_doc, "rank_static", 0, pItem);
		pItem->SetElipsis(CUIStatic::eepEnd, 1);
		AttachChild(pItem);
		pItem->SetAutoDelete(true);
	}

	if(xml_doc->NavigateToNode("rank_caption", 0))
	{
		pItem = m_icons[eUIRankCaption] = xr_new<CUIStatic>();
		xml_init.InitStatic(*xml_doc, "rank_caption", 0, pItem);
		AttachChild(pItem);
		pItem->SetAutoDelete(true);
	}

	//community
	if(xml_doc->NavigateToNode("community_static", 0))
	{
		pItem = m_icons[eUICommunity] = xr_new<CUIStatic>();
		xml_init.InitStatic(*xml_doc, "community_static", 0, pItem);
		pItem->SetElipsis(CUIStatic::eepEnd, 1);
		AttachChild(pItem);
		pItem->SetAutoDelete(true);
	}

	if(xml_doc->NavigateToNode("community_caption", 0))
	{
		pItem = m_icons[eUICommunityCaption] = xr_new<CUIStatic>();
		xml_init.InitStatic(*xml_doc, "community_caption", 0, pItem);
		AttachChild(pItem);
		pItem->SetAutoDelete(true);
	}

	//reputation
	if(xml_doc->NavigateToNode("reputation_static", 0))
	{
		pItem = m_icons[eUIReputation] = xr_new<CUIStatic>();
		xml_init.InitStatic(*xml_doc, "reputation_static", 0, pItem);
		pItem->SetElipsis(CUIStatic::eepEnd, 1);
		AttachChild(pItem);
		pItem->SetAutoDelete(true);
	}

	if(xml_doc->NavigateToNode("reputation_caption", 0))
	{
		pItem = m_icons[eUIReputationCaption] = xr_new<CUIStatic>();
		xml_init.InitStatic(*xml_doc, "reputation_caption", 0, pItem);
		AttachChild(pItem);
		pItem->SetAutoDelete(true);
	}

	// relation
	if(xml_doc->NavigateToNode("relation_static", 0))
	{
		pItem = m_icons[eUIRelation] = xr_new<CUIStatic>();
		xml_init.InitStatic(*xml_doc, "relation_static", 0, pItem);
		pItem->SetElipsis(CUIStatic::eepEnd, 1);
		AttachChild(pItem);
		pItem->SetAutoDelete(true);
	}

	if(xml_doc->NavigateToNode("relation_caption", 0))
	{
		pItem = m_icons[eUIRelationCaption] = xr_new<CUIStatic>();
		xml_init.InitStatic(*xml_doc, "relation_caption", 0, pItem);
		AttachChild(pItem);
		pItem->SetAutoDelete(true);
	}

	if (xml_doc->NavigateToNode("biography_list", 0))
	{
		pUIBio = xr_new<CUIScrollView>();pUIBio->SetAutoDelete(true);
		xml_init.InitScrollView(*xml_doc, "biography_list", 0, pUIBio);
		AttachChild(pUIBio);
	}
}

void CUICharacterInfo::Init(float x, float y, float width, float height, const char* xml_name)
{
	CUIXml							uiXml;
	bool xml_result					= uiXml.Init(CONFIG_PATH, UI_PATH, xml_name);
	R_ASSERT3						(xml_result, "xml file not found", xml_name);
	Init							(x,y,width,height,&uiXml);
}

void CUICharacterInfo::InitCharacter(u16 id)
{
	m_ownerID					= id;

	CCharacterInfo				chInfo;
	CSE_ALifeTraderAbstract*	T = ch_info_get_from_id(m_ownerID);

	chInfo.Init					(T);

	CStringTable	stbl;
	string256		str;
	if(m_icons[eUIName]){
		m_icons[eUIName]->SetText	(T->m_character_name.c_str());
	}

	if(m_icons[eUIRank]){
		sprintf_s(str, "%s", *stbl.translate(GetRankAsText(chInfo.Rank().value())));
		m_icons[eUIRank]->SetText(str);
	}


	if(m_icons[eUIReputation]){
		sprintf_s(str, "%s", *stbl.translate(GetReputationAsText(chInfo.Reputation().value())));
		m_icons[eUIReputation]->SetText(str);
	}

	if(m_icons[eUICommunity]){
		sprintf_s(str, "%s", *CStringTable().translate(chInfo.Community().id()));
		m_icons[eUICommunity]->SetText(str);
	}

	m_texture_name										= chInfo.IconName().c_str();
	m_icons[eUIIcon]->InitTexture						( m_texture_name.c_str() );
	m_icons[eUIIcon]->SetStretchTexture					(true);

	// Bio
	if (pUIBio/* && pUIBio->IsEnabled()*/)
	{
		pUIBio->Clear();
		if (chInfo.Bio().size())
		{
			CUIStatic* pItem				= xr_new<CUIStatic>();
			pItem->SetWidth					(pUIBio->GetDesiredChildWidth());
			pItem->SetText					(*(chInfo.Bio()));
			pItem->AdjustHeightToText		();
			pUIBio->AddWindow				(pItem, true);
		}
	}

	m_bForceUpdate	= true;
	for(int i = eUIName; i<eMaxCaption; ++i)
		if(m_icons[i])m_icons[i]->Show(true);
}

void  CUICharacterInfo::SetRelation(ALife::ERelationType relation, CHARACTER_GOODWILL goodwill)
{
	shared_str relation_str;

	CStringTable stbl;

	m_icons[eUIRelation]->SetTextColor(GetRelationColor(relation));
	string256		str;

	sprintf_s(str, "%s", *stbl.translate(GetGoodwillAsText(goodwill)));

	m_icons[eUIRelation]->SetText(str);
}


//////////////////////////////////////////////////////////////////////////

void CUICharacterInfo::ResetAllStrings()
{
	if(m_icons[eUIName])		m_icons[eUIName]->SetText		("");
	if(m_icons[eUIRank])		m_icons[eUIRank]->SetText		("");
	if(m_icons[eUICommunity])	m_icons[eUICommunity]->SetText	("");
	if(m_icons[eUIRelation])	m_icons[eUIRelation]->SetText	("");
	if(m_icons[eUIReputation])	m_icons[eUIReputation]->SetText	("");
}

void CUICharacterInfo::UpdateRelation()
{
	if(!m_icons[eUIRelation] ||!m_icons[eUIRelationCaption]) return;

		if (Actor()->ID()==m_ownerID || !hasOwner())
		{
			if(m_icons[eUIRelationCaption])	m_icons[eUIRelationCaption]->Show	(false);
			if(m_icons[eUIRelation])		m_icons[eUIRelation]->Show			(false);
		}
		else
		{
			if(m_icons[eUIRelationCaption])	m_icons[eUIRelationCaption]->Show	(true);
			if(m_icons[eUIRelation])		m_icons[eUIRelation]->Show			(true);

			CSE_ALifeTraderAbstract* T = ch_info_get_from_id	(m_ownerID);
			CSE_ALifeTraderAbstract* TA = ch_info_get_from_id	(Actor()->ID());

			SetRelation(RELATION_REGISTRY().GetRelationType(T,		TA),
						RELATION_REGISTRY().GetAttitude(T,			TA));
		}
}

void CUICharacterInfo::Update()
{
	inherited::Update();


	if(hasOwner() && (m_bForceUpdate||(Device.dwFrame%100==0))  ){
		m_bForceUpdate = false;
		CSE_ALifeTraderAbstract* T = ch_info_get_from_id	(m_ownerID);
		if (NULL==T){
			m_ownerID = u16(-1);
			return;
		}else
			UpdateRelation();

		if(m_icons[eUIIcon]){
			CSE_ALifeCreatureAbstract*		pCreature = smart_cast<CSE_ALifeCreatureAbstract*>(T);
			if(pCreature && !pCreature->g_Alive())
				m_icons[eUIIcon]->SetColor	(color_argb(255,255,160,160));
		}
	}
}

void CUICharacterInfo::ClearInfo()
{
	ResetAllStrings	();
	
	if (m_icons[eUIIcon]) {
		m_icons[eUIIcon]->GetUIStaticItem().SetOriginalRect(	8*ICON_GRID_WIDTH,0,
			float(CHAR_ICON_WIDTH*ICON_GRID_WIDTH),
			float(CHAR_ICON_HEIGHT*ICON_GRID_HEIGHT));
	}

	for(int i = eUIName; i<eMaxCaption; ++i)
		if(m_icons[i])m_icons[i]->Show(false);
}

