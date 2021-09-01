// file:		UITextureMaster.h
// description:	holds info about shared textures. able to initialize external controls
//				through IUITextureControl interface
// created:		11.05.2005
// author:		Serge Vynnychenko
// mail:		narrator@gsc-game.kiev.ua
//
// copyright 2005 GSC Game World


#include "StdAfx.h"
#include "UITextureMaster.h"
#include "uiabstract.h"
#include "xrUIXmlParser.h"

xr_map<shared_str, TEX_INFO>	CUITextureMaster::m_textures;
#ifdef DEBUG
u32									CUITextureMaster::m_time = 0;
#endif

void CUITextureMaster::WriteLog(){
#ifdef DEBUG
	Msg("UI texture manager work time is %d ms", m_time);
#endif
}
void CUITextureMaster::ParseShTexInfo(LPCSTR xml_file){
	CUIXml xml;
	xml.Init(CONFIG_PATH, UI_PATH, xml_file);
	shared_str file = xml.Read("file_name",0,""); 

//	shared_textures_it	sht_it = m_shTex.find(texture);
//	if (m_shTex.end() == sht_it)
//	{
		int num = xml.GetNodesNum("",0,"texture");
//		regions regs;
		for (int i = 0; i<num; i++)
		{
			TEX_INFO info;

			info.file = file;

			info.rect.x1 = xml.ReadAttribFlt("texture",i,"x");
			info.rect.x2 = xml.ReadAttribFlt("texture",i,"width") + info.rect.x1;
			info.rect.y1 = xml.ReadAttribFlt("texture",i,"y");
			info.rect.y2 = xml.ReadAttribFlt("texture",i,"height") + info.rect.y1;
			shared_str id = xml.ReadAttrib("texture",i,"id");

			m_textures.insert(mk_pair(id,info));
		}
//		m_shTex.insert(mk_pair(texture, regs));
//	}
}

LPCSTR CUITextureMaster::CheckName(const char* texture_name){
	LPCSTR r;
	if (pSettings->section_exist(texture_name))
	{
		shared_str subsection = pSettings->r_string(texture_name, "icon_section", texture_name);
		subsection = pSettings->r_string(subsection, "used_icon_texture", subsection.c_str());
		r = subsection.c_str();
	}
	else {
		r = texture_name;
	}
	if (!CheckTextureExist(r)) ParseConfigIcon(r);
	if (!CheckTextureExist(r)) r = texture_name;
	return r;
}

void CUITextureMaster::ParseConfigIcon(LPCSTR section){
	if (pSettings->section_exist(section))
	{
		shared_str subsection = pSettings->r_string(section, "icon_section", section);
		subsection = pSettings->r_string(subsection, "used_icon_texture", subsection);
		int place_id = pSettings->r_float(subsection, "icon_group", 0);
		TEX_INFO info;
		shared_str file = "ui\\ui_icon_equipment";
		if (place_id != 0) file.sprintf("ui\\ui_icon_equipment_%u", place_id);
		info.file = file;
			info.rect.x1 = pSettings->r_float(subsection, "inv_grid_x", 0) * 50;
			info.rect.x2 = (pSettings->r_float(subsection, "inv_grid_width", 0) * 50) + info.rect.x1;
			info.rect.y1 = pSettings->r_float(subsection, "inv_grid_y", 0) * 50;
			info.rect.y2 = (pSettings->r_float(subsection, "inv_grid_height", 0) * 50) + info.rect.y1;
			//shared_str id = subsection;
		if (!CheckTextureExist(subsection.c_str())) m_textures.insert(mk_pair(subsection,info));
	}
}

bool CUITextureMaster::IsSh(const char* texture_name){
	return !strchr(texture_name, '\\');
}

void CUITextureMaster::InitTexture(const char* texture_name, IUISimpleTextureControl* tc){
	const char* ntn = CheckName(texture_name);
#ifdef DEBUG
	CTimer T;
	T.Start();
#endif

	xr_map<shared_str, TEX_INFO>::iterator	it;

	it = m_textures.find(ntn);

	if (it != m_textures.end())
	{
		tc->CreateShader(*((*it).second.file));
		tc->SetOriginalRectEx((*it).second.rect);
#ifdef DEBUG
		m_time += T.GetElapsed_ms();
#endif
		return;
	}
	tc->CreateShader(ntn);
#ifdef DEBUG
	m_time += T.GetElapsed_ms();
#endif
}

void CUITextureMaster::InitTexture(const char* texture_name, const char* shader_name, IUISimpleTextureControl* tc){
	const char* ntn = CheckName(texture_name);
#ifdef DEBUG
	CTimer T;
	T.Start();
#endif

	xr_map<shared_str, TEX_INFO>::iterator	it;

	it = m_textures.find(ntn);

	if (it != m_textures.end())
	{
		tc->CreateShader(*((*it).second.file), shader_name);
		tc->SetOriginalRectEx((*it).second.rect);
#ifdef DEBUG
		m_time += T.GetElapsed_ms();
#endif
		return;
	}
	tc->CreateShader(ntn, shader_name);
#ifdef DEBUG
	m_time += T.GetElapsed_ms();
#endif
}

float CUITextureMaster::GetTextureHeight(const char* texture_name){
	const char* ntn = CheckName(texture_name);
	xr_map<shared_str, TEX_INFO>::iterator	it;
	it = m_textures.find(ntn);

	if (it != m_textures.end())
		return (*it).second.rect.height();
	// KD: we don't need to die :)
//	R_ASSERT3(false,"CUITextureMaster::GetTextureHeight Can't find texture", texture_name);
	Msg("! CUITextureMaster::GetTextureHeight Can't find texture", texture_name);
	return 0;
}

bool CUITextureMaster::CheckTextureExist(const char* texture_name){
	//const char* ntn = CheckName(texture_name);
	xr_map<shared_str, TEX_INFO>::iterator	it;
	it = m_textures.find(texture_name);

	if (it != m_textures.end())
		return true;
	// Zander: I want to know if such a texture exists in advance
	return false;
}

Frect CUITextureMaster::GetTextureRect(const char* texture_name){
	const char* ntn = CheckName(texture_name);
	xr_map<shared_str, TEX_INFO>::iterator	it;
	it = m_textures.find(ntn);
	if (it != m_textures.end())
		return (*it).second.rect;

	// KD: we don't need to die :)
//	R_ASSERT3(false,"CUITextureMaster::GetTextureHeight Can't find texture", texture_name);
	//ParseConfigIcon(texture_name);
	Msg("! CUITextureMaster::GetTextureRect Can't find texture '%s'", texture_name);
	Frect emergency_r = Frect();
	emergency_r.set(0,0,0,0);
	return emergency_r;
}

float CUITextureMaster::GetTextureWidth(const char* texture_name){
	const char* ntn = CheckName(texture_name);
	xr_map<shared_str, TEX_INFO>::iterator	it;
	it = m_textures.find(ntn);

	if (it != m_textures.end())
		return (*it).second.rect.width();
	// KD: we don't need to die :)
//	R_ASSERT3(false,"CUITextureMaster::GetTextureHeight Can't find texture", texture_name);
	Msg("! CUITextureMaster::GetTextureWidth Can't find texture", texture_name);
	return 0;
}

LPCSTR CUITextureMaster::GetTextureFileName(const char* texture_name){
	const char* ntn = CheckName(texture_name);
	xr_map<shared_str, TEX_INFO>::iterator	it;
	it = m_textures.find(ntn);

	if (it != m_textures.end())
		return *((*it).second.file);
	// KD: we don't need to die :)
//	R_ASSERT3(false,"CUITextureMaster::GetTextureHeight Can't find texture", texture_name);
	Msg("! CUITextureMaster::GetTextureFileName Can't find texture", texture_name);
	return 0;
}

TEX_INFO CUITextureMaster::FindItem(LPCSTR texture_name, LPCSTR def_texture_name)
{
	const char* ntn = CheckName(texture_name);
	xr_map<shared_str, TEX_INFO>::iterator	it;
	it = m_textures.find(ntn);

	if (it != m_textures.end())
		return (it->second);
	else{
		R_ASSERT2(m_textures.find(def_texture_name)!=m_textures.end(),ntn);
		return FindItem	(def_texture_name,NULL);
	}
}

void CUITextureMaster::GetTextureShader(LPCSTR texture_name, ui_shader& sh){
	const char* ntn = CheckName(texture_name);
	xr_map<shared_str, TEX_INFO>::iterator	it;
	it = m_textures.find(ntn);

//	R_ASSERT3(it != m_textures.end(), "can't find texture", texture_name);
	if (it == m_textures.end())
		Msg("! CUITextureMaster::GetTextureShader Can't find texture", texture_name);

	sh->create("hud\\default", *((*it).second.file));	
}
