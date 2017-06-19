#include "StdAfx.h"
#include "UIOptionsItem.h"
#include "UIOptionsManager.h"
#include "../../xr_ioconsole.h"

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

xr_token* CUIOptionsItem::GetOptToken()
{
	return Console->GetXRToken(m_entry.c_str());
}

void CUIOptionsItem::SaveOptTokenValue(const char* val){
	SaveOptStringValue(val);
}

void CUIOptionsItem::SaveValue(){
	if (	m_entry == "vid_mode"		|| 
			m_entry == "_preset"		|| 
			m_entry == "rs_fullscreen" 	||	
			m_entry == "rs_fullscreen"	||
			m_entry == "r__supersample"	|| 
			m_entry == "rs_refresh_60hz"||
			m_entry == "rs_no_v_sync"	||
			m_entry == "texture_lod"	||
			// KD: additional options
		m_entry == "r2_mblur" ||
		m_entry == "r2_aa" ||
		m_entry == "r2_dof_quality" ||
		m_entry == "r2_sunshafts" ||
		m_entry == "r2_sunshafts_mode" ||
		m_entry == "r2_ssao" ||
		m_entry == "r2_dof" ||
		m_entry == "r2_dof_zoom" ||
		m_entry == "r2_dof_reload" ||
		m_entry == "r__detail_radius" ||
		m_entry == "r__detail_density" ||
		m_entry == "r2_lens_flare" ||
		m_entry == "r2_lens_dirt" ||
		m_entry == "r2_volumetric_lights" ||
		m_entry == "r2_lights_shadow_fade" ||
		m_entry == "r2_ao" ||
		m_entry == "r2_ao_intensity")

	m_optionsManager.DoVidRestart();

	if (/*m_entry == "snd_freq" ||*/ m_entry == "snd_efx")
		m_optionsManager.DoSndRestart();
}
