#include "stdafx.h"
#include "CarLights.h"
#ifdef DEBUG
#include "ode_include.h"
#include "../xr_3da/StatGraph.h"
#include "PHDebug.h"
#endif
#include "alife_space.h"
#include "hit.h"
#include "PHDestroyable.h"
#include "Car.h"
#include "../Include/xrRender/Kinematics.h"
#include "PHWorld.h"
extern CPHWorld*	ph_world;

SCarLight::SCarLight()
{
	light_render	=NULL;
	glow_render		=NULL;
	bone_id			=BI_NONE;
	m_holder		=NULL;
}

SCarLight::~SCarLight()
{

	light_render.destroy	()	;
	glow_render.destroy		()	;
	bone_id			=	BI_NONE	;
}

void SCarLight::Init(CCarLights* holder)
{
	m_holder=holder;
}

void SCarLight::ParseDefinitions(LPCSTR section)
{

	light_render			= ::Render->light_create();
	light_render->set_type	(IRender_Light::SPOT);
	light_render->set_shadow(true);
	glow_render				= ::Render->glow_create();
	//	lanim					= 0;
	//	time2hide				= 0;

	// set bone id
	IKinematics*			pKinematics=smart_cast<IKinematics*>(m_holder->PCar()->Visual());
	CInifile* ini		=	pKinematics->LL_UserData();
	
	Fcolor					clr;
	clr.set					(ini->r_fcolor(section,"color"));
	//clr.mul_rgb				(torch->spot_brightness);
	//fBrightness				= torch->spot_brightness;
	light_render->set_range	(ini->r_float(section,"range"));
	light_render->set_color	(clr);
	light_render->set_cone	(deg2rad(ini->r_float(section,"cone_angle")));
	light_render->set_texture(ini->r_string(section,"spot_texture"));

	glow_render->set_texture(ini->r_string(section,"glow_texture"));
	glow_render->set_color	(clr);
	glow_render->set_radius	(ini->r_float(section,"glow_radius"));
	
	bone_id	= pKinematics->LL_BoneID(ini->r_string(section,"bone"));
	glow_render ->set_active(false);
	light_render->set_active(false);
	pKinematics->LL_SetBoneVisible(bone_id,FALSE,TRUE);

	//lanim					= LALib.FindItem(ini->r_string(section,"animator"));
	
}

void SCarLight::Switch()
{
	VERIFY(!ph_world->Processing());
	if(isOn())TurnOff();
	else	  TurnOn();
}
void SCarLight::TurnOn()
{
	VERIFY(!ph_world->Processing());
	if(isOn()) return;
	IKinematics* K=smart_cast<IKinematics*>(m_holder->PCar()->Visual());
	K->LL_SetBoneVisible(bone_id,TRUE,TRUE);
	K->CalculateBones_Invalidate	();
	K->CalculateBones();	
	glow_render ->set_active(true);
	light_render->set_active(true);
	Update();

}
void SCarLight::TurnOff()
{
	VERIFY(!ph_world->Processing());
	if(!isOn()) return;
 	glow_render ->set_active(false);
	light_render->set_active(false);
	smart_cast<IKinematics*>(m_holder->PCar()->Visual())->LL_SetBoneVisible(bone_id,FALSE,TRUE);
}

bool SCarLight::isOn()
{
	VERIFY(!ph_world->Processing());
	VERIFY(light_render->get_active()==glow_render->get_active());
	return light_render->get_active();
}

void SCarLight::Update()
{
	VERIFY(!ph_world->Processing());
	if(!isOn()) return;
	CCar* pcar=m_holder->PCar();
	CBoneInstance& BI = smart_cast<IKinematics*>(pcar->Visual())->LL_GetBoneInstance(bone_id);
	Fmatrix M;
	M.mul(pcar->XFORM(),BI.mTransform);
	light_render->set_rotation	(M.k,M.i);
	glow_render->set_direction(M.k);
	glow_render->set_position	(M.c);
	light_render->set_position	(M.c);

}


CCarLights::CCarLights()
{
	m_pcar=NULL;
	m_is_on = false;
}

void CCarLights::Init(CCar* pcar)
{
	m_pcar=pcar;
	m_lights.clear();
}

void CCarLights::ParseDefinitions()
{
	CInifile* ini= smart_cast<IKinematics*>(m_pcar->Visual())->LL_UserData();
	if(!ini->section_exist("lights")) return;
	LPCSTR S=  ini->r_string("lights","headlights");
	string64					S1;
	int count =					_GetItemCount(S);
	for (int i=0 ;i<count; ++i) 
	{
		_GetItem					(S,i,S1);
		m_lights.push_back(xr_new<SCarLight>());
		m_lights.back()->Init(this);
		m_lights.back()->ParseDefinitions(S1);
	}
	
}

void CCarLights::Update()
{
	VERIFY(!ph_world->Processing());
	LIGHTS_I i =m_lights.begin(),e=m_lights.end();
	for(;i!=e;++i) (*i)->Update();
}

void CCarLights::SwitchHeadLights()
{
	
	VERIFY(!ph_world->Processing());
	LIGHTS_I i =m_lights.begin(),e=m_lights.end();
	for(;i!=e;++i) (*i)->Switch();
	if (m_is_on)
		m_is_on = false;
	else
		m_is_on = true;
}

bool CCarLights::IsOn()
{
	return m_is_on;
}

void CCarLights::TurnOnHeadLights()
{

	VERIFY(!ph_world->Processing());
	LIGHTS_I i =m_lights.begin(),e=m_lights.end();
	for(;i!=e;++i) (*i)->TurnOn();
	m_is_on = true;
}
void CCarLights::TurnOffHeadLights()
{
	VERIFY(!ph_world->Processing());
	LIGHTS_I i =m_lights.begin(),e=m_lights.end();
	for(;i!=e;++i) (*i)->TurnOff();
	m_is_on = false;
}

bool CCarLights::IsLight(u16 bone_id)
{
	SCarLight* light=NULL;
	return findLight(bone_id,light);
}
bool CCarLights::findLight(u16 bone_id,SCarLight* &light)
{
	LIGHTS_I i,e=m_lights.end();
	SCarLight find_light;
	find_light.bone_id=bone_id;
	i=std::find_if(m_lights.begin(),e,SFindLightPredicate(&find_light));
	light=*i;
	return i!=e; //-V783
}
CCarLights::~CCarLights()
{
	LIGHTS_I i =m_lights.begin(),e=m_lights.end();
	for(;i!=e;++i) xr_delete(*i);
	m_lights.clear();
}