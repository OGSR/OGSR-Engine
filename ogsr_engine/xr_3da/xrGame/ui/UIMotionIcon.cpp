#include "stdafx.h"
#include "UIMainIngameWnd.h"
#include "UIMotionIcon.h"
#include "UIXmlInit.h"
const LPCSTR MOTION_ICON_XML = "motion_icon.xml";

CUIMotionIcon::CUIMotionIcon()
{
	m_curren_state	= stLast;
	m_bchanged		= false;
	m_luminosity	= 0.0f;
}

CUIMotionIcon::~CUIMotionIcon()
{

}

void CUIMotionIcon::ResetVisibility()
{
	m_npc_visibility.clear	();
	m_bchanged				= true;
}

void CUIMotionIcon::Init()
{
	CUIXml uiXml;
	bool result = uiXml.Init(CONFIG_PATH, UI_PATH, MOTION_ICON_XML);
	R_ASSERT3(result, "xml file not found", MOTION_ICON_XML);

	CUIXmlInit	xml_init;

	xml_init.InitStatic			(uiXml, "background", 0, this);	

	AttachChild					(&m_power_progress);
	xml_init.InitProgressBar	(uiXml, "power_progress", 0, &m_power_progress);	

	AttachChild					(&m_luminosity_progress);
	xml_init.InitProgressBar	(uiXml, "luminosity_progress", 0, &m_luminosity_progress);	

	AttachChild					(&m_noise_progress);
	xml_init.InitProgressBar	(uiXml, "noise_progress", 0, &m_noise_progress);	
	
	AttachChild					(&m_states[stNormal]);
	xml_init.InitStatic			(uiXml, "state_normal", 0, &m_states[stNormal]);
	m_states[stNormal].Show		(false);

	AttachChild					(&m_states[stCrouch]);
	xml_init.InitStatic			(uiXml, "state_crouch", 0, &m_states[stCrouch]);	
	m_states[stCrouch].Show		(false);

	AttachChild					(&m_states[stCreep]);
	xml_init.InitStatic			(uiXml, "state_creep", 0, &m_states[stCreep]);	
	m_states[stCreep].Show		(false);

	AttachChild					(&m_states[stClimb]);
	xml_init.InitStatic			(uiXml, "state_climb", 0, &m_states[stClimb]);	
	m_states[stClimb].Show		(false);

	AttachChild					(&m_states[stRun]);
	xml_init.InitStatic			(uiXml, "state_run", 0, &m_states[stRun]);	
	m_states[stRun].Show		(false);

	AttachChild					(&m_states[stSprint]);
	xml_init.InitStatic			(uiXml, "state_sprint", 0, &m_states[stSprint]);	
	m_states[stSprint].Show		(false);

	ShowState					(stNormal);
}

void CUIMotionIcon::ShowState(EState state)
{
	if(m_curren_state==state)			return;
	if(m_curren_state!=stLast)
	{
	
		m_states[m_curren_state].Show	(false);
		m_states[m_curren_state].Enable	(false);
	}
	m_states[state].Show				(true);
	m_states[state].Enable				(true);

	m_curren_state=state;
}

void CUIMotionIcon::SetPower(float Pos)
{
	m_power_progress.SetProgressPos(Pos);
}

void CUIMotionIcon::SetNoise(float Pos)
{
	Pos	= clampr(Pos, m_noise_progress.GetRange_min(), m_noise_progress.GetRange_max());
	m_noise_progress.SetProgressPos(Pos);
}

void CUIMotionIcon::SetLuminosity(float Pos)
{
	Pos						= clampr(Pos, m_luminosity_progress.GetRange_min(), m_luminosity_progress.GetRange_max());
	m_luminosity			= Pos;
}

void CUIMotionIcon::Update()
{
	if(m_bchanged){
		m_bchanged = false;
		if( m_npc_visibility.size() )
		{
			std::sort					(m_npc_visibility.begin(), m_npc_visibility.end());
			SetLuminosity				(m_npc_visibility.back().value);
		}else
			SetLuminosity				(m_luminosity_progress.GetRange_min() );
	}
	inherited::Update();
	
	//m_luminosity_progress 
	{
		float len					= m_noise_progress.GetRange_max()-m_noise_progress.GetRange_min();
		float cur_pos				= m_luminosity_progress.GetProgressPos();
		if(cur_pos!=m_luminosity){
			float _diff = _abs(m_luminosity-cur_pos);
			if(m_luminosity>cur_pos){
				cur_pos				+= _min(len*Device.fTimeDelta, _diff);
			}else{
				cur_pos				-= _min(len*Device.fTimeDelta, _diff);
			}
			clamp(cur_pos, m_noise_progress.GetRange_min(), m_noise_progress.GetRange_max());
			m_luminosity_progress.SetProgressPos(cur_pos);
		}
	}
}

void CUIMotionIcon::SetActorVisibility		(u16 who_id, float value)
{
	float v		= float(m_luminosity_progress.GetRange_max() - m_luminosity_progress.GetRange_min());
	value		*= v;
	value		+= m_luminosity_progress.GetRange_min();

	xr_vector<_npc_visibility>::iterator it = std::find(m_npc_visibility.begin(), 
														m_npc_visibility.end(),
														who_id);

	if(it==m_npc_visibility.end() && value!=0)
	{
		m_npc_visibility.resize	(m_npc_visibility.size()+1);
		_npc_visibility& v		= m_npc_visibility.back();
		v.id					= who_id;
		v.value					= value;
	}
	else if( fis_zero(value) )
	{
		if (it!=m_npc_visibility.end())
			m_npc_visibility.erase	(it);
	}
	else
	{
		(*it).value				= value;
	}

	m_bchanged = true;
}
