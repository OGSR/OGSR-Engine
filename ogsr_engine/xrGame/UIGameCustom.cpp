#include "stdafx.h"
#include <numeric>
#include "UIGameCustom.h"
#include "ui.h"
#include "level.h"
#include "hudmanager.h"
#include "ui/UIMultiTextStatic.h"
#include "ui/UIXmlInit.h"
#include "object_broker.h"
#include "string_table.h"

void Remove_all_statics()
{
	delete_data(HUD().GetUI()->UIGame()->m_custom_statics);
	delete_data(HUD().GetUI()->UIGame()->m_custom_statics_sorted);
}

struct predicate_remove_stat {
	bool	operator() (SDrawStaticStruct& s) {
		return ( !s.IsActual() );
	}
};

CUIGameCustom::CUIGameCustom()
{
	uFlags					= 0;
	shedule.t_min			= 5;
	shedule.t_max			= 20;
	shedule_register		();
	m_pgameCaptions			= xr_new<CUICaption>();
	m_msgs_xml				= xr_new<CUIXml>();
	m_msgs_xml->Init		(CONFIG_PATH, UI_PATH, "ui_custom_msgs.xml");
}

CUIGameCustom::~CUIGameCustom()
{
	delete_data				(m_pgameCaptions);
	shedule_unregister		();
	delete_data				(m_custom_statics);
	delete_data				(m_custom_statics_sorted);
	delete_data				(m_msgs_xml);
}


float CUIGameCustom::shedule_Scale		() 
{
	return 0.5f;
};

void CUIGameCustom::shedule_Update		(u32 dt)
{
	inherited::shedule_Update(dt);
}

void CUIGameCustom::OnFrame() 
{
	st_vec::iterator it = m_custom_statics.begin();
	for(;it!=m_custom_statics.end();++it)
		(*it).Update();

	m_custom_statics.erase(
		std::remove_if(
			m_custom_statics.begin(),
			m_custom_statics.end(),
			predicate_remove_stat()
		),
		m_custom_statics.end()
	);

	UpdateStaticPriorityIndex();
}

void CUIGameCustom::UpdateStaticPriorityIndex()
{
	if (m_custom_statics_sorted.size() != m_custom_statics.size())
	{
		m_custom_statics_sorted.resize(m_custom_statics.size());

		std::iota(m_custom_statics_sorted.begin(), m_custom_statics_sorted.end(), 0);
		std::sort(m_custom_statics_sorted.begin(), m_custom_statics_sorted.end(), [this](auto lhs, auto rhs) {
			return m_custom_statics[lhs].m_priority < m_custom_statics[rhs].m_priority;
		});
	}
}

void CUIGameCustom::Render()
{
	GameCaptions()->Draw();

	if (!m_custom_statics_sorted.empty())
	{
		auto it = m_custom_statics_sorted.begin();
		for (; it != m_custom_statics_sorted.end(); ++it)
			m_custom_statics[*it].Draw();
	}
}

bool CUIGameCustom::IR_OnKeyboardPress(int dik) 
{
	return false;
}

bool CUIGameCustom::IR_OnKeyboardRelease(int dik) 
{
	return false;
}

bool CUIGameCustom::IR_OnMouseMove(int dx,int dy)
{
	return false;
}
bool CUIGameCustom::IR_OnMouseWheel			(int direction)
{
	return false;
}

void CUIGameCustom::AddDialogToRender(CUIWindow* pDialog)
{
	HUD().GetUI()->AddDialogToRender(pDialog);
}

void CUIGameCustom::RemoveDialogToRender(CUIWindow* pDialog)
{
	HUD().GetUI()->RemoveDialogToRender(pDialog);
}

CUIDialogWnd* CUIGameCustom::MainInputReceiver	()
{ 
	return HUD().GetUI()->MainInputReceiver();
};

void CUIGameCustom::AddCustomMessage		(LPCSTR id, float x, float y, float font_size, CGameFont *pFont, u16 alignment, u32 color/* LPCSTR def_text*/ )
{
	GameCaptions()->addCustomMessage(id,x,y,font_size,pFont,(CGameFont::EAligment)alignment,color,"");
}

void CUIGameCustom::AddCustomMessage		(LPCSTR id, float x, float y, float font_size, CGameFont *pFont, u16 alignment, u32 color, /*LPCSTR def_text,*/ float flicker )
{
	AddCustomMessage(id,x,y,font_size, pFont, alignment, color);
	GameCaptions()->customizeMessage(id, CUITextBanner::tbsFlicker)->fPeriod = flicker;
}

void CUIGameCustom::CustomMessageOut(LPCSTR id, LPCSTR msg, u32 color)
{
	GameCaptions()->setCaption(id,msg,color,true);
}

void CUIGameCustom::RemoveCustomMessage		(LPCSTR id)
{
	GameCaptions()->removeCustomMessage(id);
}

SDrawStaticStruct* CUIGameCustom::AddCustomStatic			(LPCSTR id, bool bSingleInstance)
{
	if(bSingleInstance){
		st_vec::iterator it = std::find(m_custom_statics.begin(),m_custom_statics.end(), id);
		if(it!=m_custom_statics.end())
			return &(*it);
	}
	
	CUIXmlInit xml_init;
	m_custom_statics.push_back		(SDrawStaticStruct());
	SDrawStaticStruct& sss			= m_custom_statics.back();

	sss.m_static					= xr_new<CUIStatic>();
	sss.m_name						= id;
	xml_init.InitStatic				(*m_msgs_xml, id, 0, sss.m_static);
	float ttl						= m_msgs_xml->ReadAttribFlt(id, 0, "ttl", -1);
	if(ttl>0.0f)
		sss.m_endTime				= Device.fTimeGlobal + ttl;
	sss.m_priority					= m_msgs_xml->ReadAttribInt(id, 0, "priority");

	UpdateStaticPriorityIndex();

	return &sss;
}

SDrawStaticStruct* CUIGameCustom::GetCustomStatic		(LPCSTR id)
{
	st_vec::iterator it = std::find(m_custom_statics.begin(),m_custom_statics.end(), id);
	if(it!=m_custom_statics.end()){
		return &(*it);
	}
	return NULL;
}

void CUIGameCustom::RemoveCustomStatic		(LPCSTR id)
{
	st_vec::iterator it = std::find(m_custom_statics.begin(),m_custom_statics.end(), id);
	if(it!=m_custom_statics.end()){
		xr_delete((*it).m_static);
		m_custom_statics.erase(it);

		UpdateStaticPriorityIndex();
	}
}

#include "ui/UIGameTutorial.h"

extern CUISequencer* g_tutorial;
extern CUISequencer* g_tutorial2;

void CUIGameCustom::reset_ui()
{
	if (g_tutorial2)
	{
		g_tutorial2->Destroy();
		xr_delete(g_tutorial2);
	}

	if (g_tutorial)
	{
		g_tutorial->Destroy();
		xr_delete(g_tutorial);
	}
}

SDrawStaticStruct::SDrawStaticStruct	()
{
	m_static	= NULL;
	m_endTime	= -1.0f;	
}

void SDrawStaticStruct::destroy()
{
	delete_data(m_static);
}

bool SDrawStaticStruct::IsActual()
{
	if(m_endTime<0) return true;
	return Device.fTimeGlobal < m_endTime;
}

void SDrawStaticStruct::Draw()
{
	if(m_static)
		m_static->Draw();
}

void SDrawStaticStruct::Update()
{
	if(!IsActual())	
		delete_data(m_static);
	else
		m_static->Update();
}