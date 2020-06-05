#include "stdafx.h"
#include "..\Include/xrRender/KinematicsAnimated.h"
#include "step_manager_defs.h"
#include "step_manager.h"
#include "entity_alive.h"
#include "level.h"
#include "gamepersistent.h"
#include "material_manager.h"
#include "profiler.h"
#include "IKLimbsController.h"

CStepManager::CStepManager()
{
}

CStepManager::~CStepManager()
{
}

DLL_Pure *CStepManager::_construct	()
{
	m_object			= smart_cast<CEntityAlive*>(this);
	VERIFY				(m_object);
	return				(m_object);
}

void CStepManager::reload(LPCSTR section)
{
	m_legs_count		= pSettings->r_u8		(section, "LegsCount");
	LPCSTR anim_section = pSettings->r_string	(section, "step_params");

	if (!pSettings->section_exist(anim_section)) return;
	VERIFY((m_legs_count>=MIN_LEGS_COUNT) && (m_legs_count<=MAX_LEGS_COUNT));

	SStepParam			param; 
	param.step[0].time = 0.1f;	// avoid warning

	LPCSTR				anim_name, val;
	string16			cur_elem;

	m_steps_map.clear();

	IKinematicsAnimated	*skeleton_animated = smart_cast<IKinematicsAnimated*>(m_object->Visual());

	for (u32 i=0; pSettings->r_line(anim_section,i,&anim_name,&val); ++i) {
		_GetItem (val,0,cur_elem);

		param.cycles = u8(atoi(cur_elem));
		R_ASSERT(param.cycles >= 1);

		for (u32 j=0;j<m_legs_count;j++) {
			_GetItem	(val,1+j*2,		cur_elem);		param.step[j].time	= float(atof(cur_elem));
			_GetItem	(val,1+j*2+1,	cur_elem);		param.step[j].power	= float(atof(cur_elem));
			VERIFY		(_valid(param.step[j].power));			
		}
		
		MotionID motion_id = skeleton_animated->ID_Cycle_Safe(anim_name);
		if (!motion_id) continue;
		
		m_steps_map.insert(mk_pair(motion_id, param));
	}
	
	// reload foot bones
	for (u32 i = 0; i < MAX_LEGS_COUNT; i++) m_foot_bones[i] = BI_NONE;
	reload_foot_bones	();

	
	m_time_anim_started	= 0;
	m_blend				= 0;
}

void CStepManager::on_animation_start(MotionID motion_id, CBlend *blend)
{
	m_blend	= blend;
	if (!m_blend) return;

	if(m_object->character_ik_controller	())
		m_object->character_ik_controller	()->PlayLegs(blend);

	m_time_anim_started = Device.dwTimeGlobal; 
	
	// искать текущую анимацию в STEPS_MAP
	STEPS_MAP_IT it = m_steps_map.find(motion_id);
	if (it == m_steps_map.end()) {
		m_step_info.disable = true;
		return;
	}

	m_step_info.disable		= false;
	m_step_info.params		= it->second;
	m_step_info.cur_cycle	= 1;					// all cycles are 1-based

	for (u32 i=0; i<m_legs_count; i++) {
		m_step_info.activity[i].handled	= false;
		m_step_info.activity[i].cycle	= m_step_info.cur_cycle;
	}


	VERIFY					(m_blend);
}


void CStepManager::update()
{
	START_PROFILE("Step Manager")

	if (m_step_info.disable)	return;
	if (!m_blend)				return;

	SGameMtlPair* mtl_pair		= m_object->material().get_current_pair();
	if (!mtl_pair)				return;

	// получить параметры шага
	SStepParam	&step		= m_step_info.params;
	u32		cur_time		= Device.dwTimeGlobal;

	// время одного цикла анимации
	float cycle_anim_time	= get_blend_time() / step.cycles;

	// пройти по всем ногам и проверить время
	for (u32 i=0; i<m_legs_count; i++) {

		// если событие уже обработано для этой ноги, то skip
		if (m_step_info.activity[i].handled && (m_step_info.activity[i].cycle == m_step_info.cur_cycle)) continue;

		// вычислить смещённое время шага в соответствии с параметрами анимации ходьбы
		u32 offset_time = m_time_anim_started + u32(1000 * (cycle_anim_time * (m_step_info.cur_cycle-1) + cycle_anim_time * step.step[i].time));
		if (offset_time <= cur_time){

			// Играть звук
			if (!mtl_pair->StepSounds.empty() && is_on_ground() ) 
			{
				Fvector sound_pos = m_object->Position();
				sound_pos.y += 0.5;
				GET_RANDOM(mtl_pair->StepSounds).play_no_feedback(m_object,0,0,&sound_pos,&m_step_info.params.step[i].power);
			}

			// Играть партиклы
			if (!mtl_pair->CollideParticles.empty())	{
				LPCSTR ps_name = *mtl_pair->CollideParticles[::Random.randI(0,mtl_pair->CollideParticles.size())];

				//отыграть партиклы столкновения материалов
				CParticlesObject* ps = CParticlesObject::Create(ps_name,TRUE);

				// вычислить позицию и направленность партикла
				Fmatrix pos; 

				// установить направление
				pos.k.set(Fvector().set(0.0f,1.0f,0.0f));
				Fvector::generate_orthonormal_basis(pos.k, pos.j, pos.i);

				// установить позицию
				pos.c.set(get_foot_position(ELegType(i)));

				ps->UpdateParent(pos,Fvector().set(0.f,0.f,0.f));
				GamePersistent().ps_needtoplay.push_back(ps);
			}

			// Play Camera FXs
			event_on_step();

			// обновить поле handle
			m_step_info.activity[i].handled	= true;
			m_step_info.activity[i].cycle	= m_step_info.cur_cycle;
		}
	}

	// определить текущий цикл
	if (m_step_info.cur_cycle < step.cycles) m_step_info.cur_cycle = 1 + u8(float(cur_time - m_time_anim_started) / (1000.f * cycle_anim_time));

	// если анимация циклическая...
	u32 time_anim_end = m_time_anim_started + u32(get_blend_time() * 1000);		// время завершения работы анимации
	if (!m_blend->stop_at_end && (time_anim_end < cur_time)) {
		
		m_time_anim_started		= time_anim_end;
		m_step_info.cur_cycle	= 1;

		for (u32 i=0; i<m_legs_count; i++) {
			m_step_info.activity[i].handled	= false;
			m_step_info.activity[i].cycle	= m_step_info.cur_cycle;
		}
	}
	STOP_PROFILE
}

//////////////////////////////////////////////////////////////////////////
// Function for foot processing
//////////////////////////////////////////////////////////////////////////
Fvector	CStepManager::get_foot_position(ELegType leg_type)
{
	R_ASSERT2(m_foot_bones[leg_type] != BI_NONE, make_string("[%s] foot bone had not been set", m_object->Name()));

	IKinematics *pK					= smart_cast<IKinematics*>(m_object->Visual());
	const Fmatrix& bone_transform = pK->LL_GetBoneInstance(m_foot_bones[leg_type]).mTransform;	

	Fmatrix					global_transform;
	global_transform.mul_43	(m_object->XFORM(),bone_transform);

	return global_transform.c;
}

void CStepManager::load_foot_bones	(CInifile::Sect &data)
{
	for ( const auto &item : data.Data ) {
		u16 index = smart_cast<IKinematics*>(m_object->Visual())->LL_BoneID(item.second.c_str());
		VERIFY3(index != BI_NONE, "foot bone not found", item.second.c_str());

		if (xr_strcmp(item.first.c_str(), "front_left") == 0) 			m_foot_bones[eFrontLeft]	= index;
		else if (xr_strcmp(item.first.c_str(), "front_right")== 0)		m_foot_bones[eFrontRight]	= index;
		else if (xr_strcmp(item.first.c_str(), "back_right")== 0)		m_foot_bones[eBackRight]	= index;
		else if (xr_strcmp(item.first.c_str(), "back_left")== 0)		m_foot_bones[eBackLeft]		= index;
	}
}

void CStepManager::reload_foot_bones()
{
	CInifile* ini = smart_cast<IKinematics*>(m_object->Visual())->LL_UserData();
	if(ini&&ini->section_exist("foot_bones")){
		load_foot_bones(ini->r_section("foot_bones"));
	}
	else {
		if (!pSettings->line_exist(*m_object->cNameSect(),"foot_bones"))
			R_ASSERT2(false,"section [foot_bones] not found in monster user_data");
		load_foot_bones(pSettings->r_section(pSettings->r_string(*m_object->cNameSect(),"foot_bones")));
	}

	// проверка на соответсвие
	int count = 0;
	for (u32 i = 0; i < MAX_LEGS_COUNT; i++) 
		if (m_foot_bones[i] != BI_NONE) count++;

	VERIFY(count == m_legs_count);
}

float CStepManager::get_blend_time()
{
	return 	(m_blend->timeTotal / m_blend->speed);
}
