#include "StdAfx.h"
#include "UIOptionsItem.h"
#include "UIOptionsManager.h"
#include "..\..\xr_3da\xr_ioconsole.h"

CUIOptionsManager CUIOptionsItem::m_optionsManager;

CUIOptionsItem::~CUIOptionsItem()
{
	m_optionsManager.UnRegisterItem(this);
}

void CUIOptionsItem::Register(const char* entry, const char* group)
{
	m_optionsManager.RegisterItem	(this, group);
	m_entry							= entry;	
}

void CUIOptionsItem::SendMessage2Group(const char* group, const char* message)
{
	m_optionsManager.SendMessage2Group(group,message);
}

void CUIOptionsItem::OnMessage(const char* message)
{
	// do nothing
}

LPCSTR CUIOptionsItem::GetOptStringValue()
{
	return Console->GetString(m_entry.c_str());
}

void CUIOptionsItem::SaveOptStringValue(const char* val)
{
	xr_string command	= m_entry;
	command				+= " ";
	command				+= val;
	Console->Execute	(command.c_str());
}

void CUIOptionsItem::GetOptIntegerValue(int& val, int& min, int& max)
{
	Console->GetInteger(m_entry.c_str(), val, min, max);
}

void CUIOptionsItem::SaveOptIntegerValue(int val)
{
	string512			command;
	sprintf_s				(command, "%s %d", m_entry.c_str(), val);
	Console->Execute	(command);
}

void CUIOptionsItem::GetOptFloatValue(float& val, float& min, float& max)
{
	Console->GetFloat(m_entry.c_str(), val, min, max);
}

void CUIOptionsItem::SaveOptFloatValue(float val)
{
	string512			command;
	sprintf_s				(command, "%s %f", m_entry.c_str(), val);
	Console->Execute	(command);
}

bool CUIOptionsItem::GetOptBoolValue()
{
	BOOL val;
	Console->GetBool(m_entry.c_str(), val);
	return val ? true : false;
}

void CUIOptionsItem::SaveOptBoolValue(bool val)
{
	string512			command;
	sprintf_s				(command, "%s %s", m_entry.c_str(), (val)?"on":"off");
	Console->Execute	(command);
}

char* CUIOptionsItem::GetOptTokenValue()
{
	return Console->GetToken(m_entry.c_str());
}

const xr_token* CUIOptionsItem::GetOptToken()
{
	const auto* token = Console->GetXRToken(m_entry.c_str());
	ASSERT_FMT(token, "Can't find token [%s]", m_entry.c_str());
	return token;
}

void CUIOptionsItem::SaveOptTokenValue(const char* val){
	SaveOptStringValue(val);
}

#pragma todo("KRodin: желательно убрать этот хардкод, как в ЗП, конечно, но пока меня устраивает. Перетаскивать полностью движковые классы опций оттуда я точно не буду.")
void CUIOptionsItem::SaveValue() {
	if (
		m_entry == "vid_mode"
		|| m_entry == "rs_fullscreen"
		|| m_entry == "rs_v_sync"
		|| m_entry == "rs_refresh_60hz"
		|| m_entry == "r2_soft_water"
		|| m_entry == "r2_soft_particles"
		|| m_entry == "r2_dof_enable"
		|| m_entry == "r2_volumetric_lights"
		|| m_entry == "r3_msaa_opt"
		|| m_entry == "r3_dynamic_wet_surfaces"
		|| m_entry == "r__detail_radius"
		|| m_entry == "r__detail_density"
		|| m_entry == "r__detail_scale"
		|| m_entry == "r__no_scale_on_fade"
		|| m_entry == "r2_sun_quality"
	) m_optionsManager.DoVidRestart();
	
	if (m_entry == "snd_efx")
		m_optionsManager.DoSndRestart();
}
