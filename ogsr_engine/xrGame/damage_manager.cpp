////////////////////////////////////////////////////////////////////////////
//	Module 		: damage_manager.cpp
//	Created 	: 02.10.2001
//  Modified 	: 19.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Damage manager
////////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "damage_manager.h"
#include "..\xr_3da\xr_object.h"
#include "../Include/xrRender/Kinematics.h"
#include "../xr_3da/bone.h"

CDamageManager::CDamageManager			()
{
}

CDamageManager::~CDamageManager			()
{
}

DLL_Pure *CDamageManager::_construct	()
{
	m_object				= smart_cast<CObject*>(this);
	VERIFY					(m_object);
	return					(m_object);
}

void CDamageManager::reload				(LPCSTR section,CInifile* ini)
{
	m_default_hit_factor	= 1.f;
	m_default_wound_factor	= 1.f;

	bool section_exist		= ini && ini->section_exist(section);
	
	// прочитать дефолтные параметры
	if (section_exist) {
		string32 buffer;
		if (ini->line_exist(section,"default")) {
			LPCSTR value			= ini->r_string(section,"default");
			m_default_hit_factor	= (float)atof(_GetItem(value,0,buffer));
			m_default_wound_factor  = (float)atof(_GetItem(value,2,buffer));
		}
	}

	//инициализировать default параметрами
	init_bones		(section,ini);

	// записать поверху прописанные параметры
	if (section_exist) {
		load_section	(section,ini);
	}
}

void CDamageManager::reload(LPCSTR section,LPCSTR line,CInifile* ini)
{
	if (ini && ini->section_exist(section) && ini->line_exist(section,line)) 
		reload(ini->r_string(section,line),ini);	
	else 
		reload(section,0);
}

void CDamageManager::init_bones(LPCSTR section,CInifile* ini)
{
	IKinematics				*kinematics = smart_cast<IKinematics*>(m_object->Visual());
	VERIFY					(kinematics);
	for(u16 i = 0; i<kinematics->LL_BoneCount(); i++)
	{
		CBoneInstance			&bone_instance = kinematics->LL_GetBoneInstance(i);
		bone_instance.set_param	(0,m_default_hit_factor);
		bone_instance.set_param	(1,1.f);
		bone_instance.set_param	(2,m_default_wound_factor);
	}
}
void CDamageManager::load_section(LPCSTR section,CInifile* ini)
{
	string32				buffer;
	IKinematics				*kinematics = smart_cast<IKinematics*>(m_object->Visual());
	CInifile::Sect			&damages = ini->r_section(section);
	for ( const auto &i : damages.Data ) {
		if (xr_strcmp(i.first.c_str(),"default")) { // read all except default line
			VERIFY					(m_object);
			int						bone = kinematics->LL_BoneID(i.first);
			ASSERT_FMT( bone != BI_NONE, "[%s]: bone '%s' not found in %s[%s] visual[%s]", __FUNCTION__, i.first.c_str(), m_object->cName().c_str(), section, m_object->cNameVisual().c_str() );
			CBoneInstance			&bone_instance = kinematics->LL_GetBoneInstance(u16(bone));
			bone_instance.set_param	(0,(float)atof(_GetItem(i.second.c_str(),0,buffer)));
			bone_instance.set_param	(1,(float)atoi(_GetItem(i.second.c_str(),1,buffer)));
			bone_instance.set_param	(2,(float)atof(_GetItem(i.second.c_str(),2,buffer)));
			if (_GetItemCount(i.second.c_str()) < 4)
			{
				bone_instance.set_param	(3,(float)atof(_GetItem(i.second.c_str(),0,buffer)));
			}
			else
			{
				bone_instance.set_param	(3,(float)atof(_GetItem(i.second.c_str(),3,buffer)));
			}
			if(0==bone && (fis_zero(bone_instance.get_param(0)) || fis_zero(bone_instance.get_param(2)) ) ){
				string256 error_str;
				sprintf_s(error_str,"hit_scale and wound_scale for root bone cannot be zero. see section [%s]",section);
				R_ASSERT2(0,error_str);
			}
		}
	}
}


void  CDamageManager::HitScale			(const int element, float& hit_scale, float& wound_scale, bool aim_bullet)
{
	if(BI_NONE == u16(element))
	{
		//считаем что параметры для BI_NONE заданы как 1.f 
		hit_scale = 1.f * m_default_hit_factor;
		wound_scale = 1.f * m_default_wound_factor;
		return;
	}

	IKinematics* V		= smart_cast<IKinematics*>(m_object->Visual());			VERIFY(V);
	// get hit scale
	float scale;			
	if (aim_bullet)
	{
		scale			= V->LL_GetBoneInstance(u16(element)).get_param(3);
	}
	else
	{
		scale			= V->LL_GetBoneInstance(u16(element)).get_param(0);
	}
	hit_scale			= scale;
	
	// get wound scale
	scale				= V->LL_GetBoneInstance(u16(element)).get_param(2);
	wound_scale			= scale;
}
