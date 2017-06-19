#include "StdAfx.h"

#include "UIStatsPlayerInfo.h"
#include "UIStatic.h"
#include "../game_cl_base.h"
#include "UIStatsIcon.h"
#include "../game_cl_artefacthunt.h"
#include "../level.h"
#include "../string_table.h"

CUIStatsPlayerInfo::CUIStatsPlayerInfo(xr_vector<PI_FIELD_INFO>* info, CGameFont* pF, u32 text_col)
{
	m_field_info = info;

	m_pF = pF;
	m_text_col = text_col;

	m_pBackground = xr_new<CUIStatic>();	
	AttachChild(m_pBackground);

	R_ASSERT(!info->empty());
}

CUIStatsPlayerInfo::~CUIStatsPlayerInfo()
{
	for (u32 i = 0; i<m_fields.size(); i++)
		xr_delete(m_fields[i]);

	xr_delete(m_pBackground);
}

void CUIStatsPlayerInfo::Init(float x, float y, float width, float height){
	CUIWindow::Init(x,y,width,height);
	
	m_pBackground->SetStretchTexture(true);
	m_pBackground->Init(0,0, width, height);
	m_pBackground->InitTexture("ui\\ui_mp_frags_selection");
	

	xr_vector<PI_FIELD_INFO>&	field_info = *m_field_info;
	for (u32 i = 0; i<field_info.size(); i++)
	{
		bool pic;
		if (0 == xr_strcmp(field_info[i].name, "rank"))
			pic = true;
		else if (0 == xr_strcmp(field_info[i].name, "death_atf"))
            pic = true;
		else pic = false;

		AddField(field_info[i].width, m_pF, m_text_col, pic);
	}
}

void CUIStatsPlayerInfo::SetInfo(game_PlayerState* pInfo){
	m_pPlayerInfo = pInfo;
	if (Level().CurrentViewEntity() && Level().CurrentViewEntity()->ID() == pInfo->GameID)
		m_pBackground->SetVisible(true);
	else
        m_pBackground->SetVisible(false);
}

void CUIStatsPlayerInfo::Update(){
	if (!m_pPlayerInfo)
		return;

	xr_vector<PI_FIELD_INFO>&	field_info = *m_field_info;

	for (u32 i = 0; i<m_fields.size(); i++)
		m_fields[i]->SetText(GetInfoByID(*field_info[i].name));

	m_pPlayerInfo = NULL;
}

void CUIStatsPlayerInfo::AddField(float len, CGameFont* pF, u32 text_col, bool icon){
	CUIStatic* wnd = icon ? xr_new<CUIStatsIcon>() : xr_new<CUIStatic>();

	if (m_fields.empty())
		wnd->Init(5,0,len,this->GetHeight());
	else
	{
		wnd->Init(m_fields.back()->GetWndRect().right,0,len,this->GetHeight());
		wnd->SetTextAlignment(CGameFont::alCenter);
	}
	if (pF)
		wnd->SetFont(pF);
	wnd->SetTextColor(text_col);
	wnd->SetTextComplexMode(false);
	m_fields.push_back(wnd);
	AttachChild(wnd);
}

const char* CUIStatsPlayerInfo::GetInfoByID(const char* id){
	static string64 ans;
	CStringTable st;

	if (0 == xr_strcmp(id,"name"))
		strcpy(ans,m_pPlayerInfo->name);
	else if (0 == xr_strcmp(id,"frags"))
		sprintf_s(ans,"%d",(int)m_pPlayerInfo->frags());
	else if (0 == xr_strcmp(id,"deaths"))
		sprintf_s(ans,"%d",(int)m_pPlayerInfo->m_iDeaths);
	else if (0 == xr_strcmp(id,"ping"))
		sprintf_s(ans,"%d",(int)m_pPlayerInfo->ping);
	else if (0 == xr_strcmp(id,"artefacts"))
		sprintf_s(ans,"%d",(int)m_pPlayerInfo->af_count);
	else if (0 == xr_strcmp(id,"rank"))
	{
		int team = m_pPlayerInfo->team;
		if (GameID() != GAME_DEATHMATCH)
			team -= 1;

		if (0 == team)
            sprintf_s(ans,"ui_hud_status_green_0%d",(int)m_pPlayerInfo->rank + 1);
		else
			sprintf_s(ans,"ui_hud_status_blue_0%d",(int)m_pPlayerInfo->rank + 1);

	}
	else if (0 == xr_strcmp(id, "death_atf"))
	{		
		if (m_pPlayerInfo->testFlag(GAME_PLAYER_FLAG_VERY_VERY_DEAD))
			strcpy(ans,"death");
		else if (GameID() == GAME_ARTEFACTHUNT)
		{
			game_cl_ArtefactHunt* pGameAHunt = smart_cast<game_cl_ArtefactHunt*>(&(Game()));
			R_ASSERT(pGameAHunt);
			if (m_pPlayerInfo->GameID == pGameAHunt->artefactBearerID)
				strcpy(ans,"artefact");
			else
				strcpy(ans,"");
		}
		else
			strcpy(ans,"");
		
	}
	else if (0 == xr_strcmp(id, "status"))
	{
		if (m_pPlayerInfo->testFlag(GAME_PLAYER_FLAG_READY))
			strcpy(ans,*st.translate("st_mp_ready"));
		else
			strcpy(ans,"");
	}
	else
		R_ASSERT2(false, "invalid info ID");

    return ans;
}
