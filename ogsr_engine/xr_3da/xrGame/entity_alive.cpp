#include "pch_script.h"
#include "entity_alive.h"
#include "inventoryowner.h"
#include "inventory.h"
#include "physicsshell.h"
#include "gamemtllib.h"
#include "phmovementcontrol.h"
#include "wound.h"
#include "xrmessages.h"
#include "level.h"
#include "../skeletoncustom.h"
#include "relation_registry.h"
#include "monster_community.h"
#include "entitycondition.h"
#include "script_game_object.h"
#include "hit.h"
#include "PHDestroyable.h"
#include "CharacterPhysicsSupport.h"
#include "script_callback_ex.h"
#include "game_object_space.h"
#include "material_manager.h"
#include "game_base_space.h"

#define SMALL_ENTITY_RADIUS		0.6f
#define BLOOD_MARKS_SECT		"bloody_marks"

//отметки крови на стенах 
SHADER_VECTOR* CEntityAlive::m_pBloodMarksVector = NULL;
float CEntityAlive::m_fBloodMarkSizeMin = 0.f;
float CEntityAlive::m_fBloodMarkSizeMax = 0.f;
float CEntityAlive::m_fBloodMarkDistance = 0.f;
float CEntityAlive::m_fNominalHit = 0.f;

//капание крови
SHADER_VECTOR* CEntityAlive::m_pBloodDropsVector = NULL;
float CEntityAlive::m_fStartBloodWoundSize = 0.3f;
float CEntityAlive::m_fStopBloodWoundSize = 0.1f;
float CEntityAlive::m_fBloodDropSize = 0.03f;


//минимальный размер ожега, после которого горят партиклы
//минимальное время горения
u32	  CEntityAlive::m_dwMinBurnTime = 10000;
//размер раны, чтоб запустить партиклы
float CEntityAlive::m_fStartBurnWoundSize = 0.3f;
//размер раны, чтоб остановить партиклы
float CEntityAlive::m_fStopBurnWoundSize = 0.1f;

STR_VECTOR* CEntityAlive::m_pFireParticlesVector = NULL;

/////////////////////////////////////////////
// CEntityAlive
/////////////////////////////////////////////
CEntityAlive::CEntityAlive()
{
	
	monster_community		= xr_new<MONSTER_COMMUNITY>	();

	m_ef_weapon_type		= u32(-1);
	m_ef_detector_type		= u32(-1);

	m_material_manager		= 0;
}

CEntityAlive::~CEntityAlive()
{

	xr_delete				(monster_community);
	xr_delete				(m_material_manager);
}

void CEntityAlive::Load		(LPCSTR section)
{
	CEntity::Load					(section);
	conditions().LoadCondition		(section);
	conditions().LoadImmunities		(pSettings->r_string(section,"immunities_sect"),pSettings);

	m_fFood					= 100*pSettings->r_float	(section,"ph_mass");

	//bloody wallmarks
	if(0==m_pBloodMarksVector)
		LoadBloodyWallmarks (BLOOD_MARKS_SECT);

	if(0==m_pFireParticlesVector)
		LoadFireParticles	("entity_fire_particles");

	//биолог. вид к торому принадлежит монстр или персонаж
	monster_community->set	(pSettings->r_string(section, "species"));
}

void CEntityAlive::LoadBloodyWallmarks (LPCSTR section)
{
	VERIFY					(0==m_pBloodMarksVector);
	VERIFY					(0==m_pBloodDropsVector);
	m_pBloodMarksVector		= xr_new<SHADER_VECTOR>();
	m_pBloodDropsVector		= xr_new<SHADER_VECTOR>();
	
	//кровавые отметки на стенах
	string256	tmp;
	LPCSTR wallmarks_name = pSettings->r_string(section, "wallmarks"); 
	
	int cnt		=_GetItemCount(wallmarks_name);
	
	ref_shader	s;
	for (int k=0; k<cnt; ++k)
	{
		s.create ("effects\\wallmark",_GetItem(wallmarks_name,k,tmp));
		m_pBloodMarksVector->push_back	(s);
	}

	
	m_fBloodMarkSizeMin = pSettings->r_float(section, "min_size"); 
	m_fBloodMarkSizeMax = pSettings->r_float(section, "max_size"); 
	m_fBloodMarkDistance = pSettings->r_float(section, "dist"); 
	m_fNominalHit = pSettings->r_float(section, "nominal_hit"); 



	//капли крови с открытых ран
	wallmarks_name = pSettings->r_string(section, "blood_drops");
	cnt		=_GetItemCount(wallmarks_name);

	for (int k=0; k<cnt; ++k)
	{
		s.create ("effects\\wallmark",_GetItem(wallmarks_name,k,tmp));
		m_pBloodDropsVector->push_back	(s);
	}


	m_fStartBloodWoundSize  = pSettings->r_float(section, "start_blood_size");
	m_fStopBloodWoundSize   = pSettings->r_float(section, "stop_blood_size");
	m_fBloodDropSize		= pSettings->r_float(section, "blood_drop_size");
}

void CEntityAlive::UnloadBloodyWallmarks	()
{
	if (m_pBloodMarksVector){ 
		m_pBloodMarksVector->clear	();
		xr_delete					(m_pBloodMarksVector);
	}
	if (m_pBloodDropsVector){
		m_pBloodDropsVector->clear	();
		xr_delete					(m_pBloodDropsVector);
	}
}

void CEntityAlive::LoadFireParticles(LPCSTR section)
{
	m_pFireParticlesVector = xr_new<STR_VECTOR>();

	string256	tmp;
	LPCSTR particles_name = pSettings->r_string(section, "fire_particles"); 

	int cnt		=_GetItemCount(particles_name);

	shared_str	s;
	for (int k=0; k<cnt; ++k)
	{
		s  = _GetItem(particles_name,k,tmp);
		m_pFireParticlesVector->push_back	(s);
	}

	m_fStartBurnWoundSize  = pSettings->r_float(section, "start_burn_size");
	m_fStopBurnWoundSize   = pSettings->r_float(section, "stop_burn_size");
	
	m_dwMinBurnTime		   = pSettings->r_u32(section, "min_burn_time");
}

void CEntityAlive::UnloadFireParticles()
{
	if (m_pFireParticlesVector)	{
		m_pFireParticlesVector->clear();
		xr_delete(m_pFireParticlesVector);
	}
}

void CEntityAlive::reinit()
{
	CEntity::reinit			();

	m_fAccuracy				= 25.f;
	m_fIntelligence			= 25.f;
}

void CEntityAlive::reload		(LPCSTR section)
{
	CEntity::reload			(section);
//	CEntityCondition::reload(section);

	m_ef_creature_type		= pSettings->r_u32		(section,"ef_creature_type");
	m_ef_weapon_type		= READ_IF_EXISTS(pSettings,r_u32,section,"ef_weapon_type",u32(-1));
	m_ef_detector_type		= READ_IF_EXISTS(pSettings,r_u32,section,"ef_detector_type",u32(-1));

	m_fFood					= 100*pSettings->r_float	(section,"ph_mass");
}

void CEntityAlive::shedule_Update(u32 dt)
{
	inherited::shedule_Update	(dt);

	//condition update with the game time pass
	conditions().UpdateConditionTime	();
	conditions().UpdateCondition		();
	//Обновление партиклов огня
	UpdateFireParticles	();
	//капли крови
	UpdateBloodDrops	();
	//обновить раны
	conditions().UpdateWounds		();

	//убить сущность
	if(Local() && !g_Alive() && !AlreadyDie())
	{
		if(conditions().GetWhoHitLastTime()) {
//			Msg			("%6d : KillEntity from CEntityAlive (using who hit last time) for object %s",Device.dwTimeGlobal,*cName());
			KillEntity	(conditions().GetWhoHitLastTimeID());
		}
		else {
//			Msg			("%6d : KillEntity from CEntityAlive for object %s",Device.dwTimeGlobal,*cName());
			KillEntity	(ID());
		}
	}
}

BOOL CEntityAlive::net_Spawn	(CSE_Abstract* DC)
{
	//установить команду в соответствии с community
/*	if(monster_community->team() != 255)
		id_Team = monster_community->team();*/

	conditions().reinit			();
	inherited::net_Spawn		(DC);

	m_BloodWounds.clear			();
	m_ParticleWounds.clear		();

	//добавить кровь и огонь на партиклы, если нужно
	for(WOUND_VECTOR::const_iterator it = conditions().wounds().begin(); conditions().wounds().end() != it; ++it)
	{
		CWound					*pWound = *it;
		StartFireParticles		(pWound);
		StartBloodDrops			(pWound);
	}

	return						(TRUE);
}

void CEntityAlive::net_Destroy	()
{
	inherited::net_Destroy		();
}

void CEntityAlive::HitImpulse	(float /**amount/**/, Fvector& /**vWorldDir/**/, Fvector& /**vLocalDir/**/)
{
	//	float Q					= 2*float(amount)/m_PhysicMovementControl->GetMass();
	//	m_PhysicMovementControl->vExternalImpulse.mad	(vWorldDir,Q);
}

//void CEntityAlive::Hit(float P, Fvector &dir,CObject* who, s16 element,Fvector position_in_object_space, float impulse, ALife::EHitType hit_type, float AP)
void CEntityAlive::Hit(SHit* pHDS)
{
	SHit HDS = *pHDS;
	//-------------------------------------------------------------------
	if (HDS.hit_type == ALife::eHitTypeWound_2)
		HDS.hit_type = ALife::eHitTypeWound;
	//-------------------------------------------------------------------
	CDamageManager::HitScale(HDS.boneID, conditions().hit_bone_scale(), conditions().wound_bone_scale(),pHDS->aim_bullet);
	
	//изменить состояние, перед тем как родительский класс обработает хит
	CWound* pWound = conditions().ConditionHit(&HDS);

	if(pWound){
		if(ALife::eHitTypeBurn == HDS.hit_type)
			StartFireParticles(pWound);
		else if(ALife::eHitTypeWound == HDS.hit_type || ALife::eHitTypeFireWound == HDS.hit_type)
			StartBloodDrops(pWound);
	}

	if (HDS.hit_type != ALife::eHitTypeTelepatic){
		//добавить кровь на стены
		if (!use_simplified_visual())
			BloodyWallmarks (HDS.damage(), HDS.dir, HDS.bone(), HDS.p_in_bone_space);
	}

	//-------------------------------------------
	conditions().SetConditionDeltaTime(0);
	//-------------------------------------------
	inherited::Hit(&HDS);

	if (g_Alive()&&IsGameTypeSingle()) {
		CEntityAlive* EA = smart_cast<CEntityAlive*>(HDS.who);
		if(EA && EA->g_Alive() && EA->ID() != ID())
		{
			RELATION_REGISTRY().FightRegister(EA->ID(), ID(), this->tfGetRelationType(EA), HDS.damage());
			RELATION_REGISTRY().Action(EA, this, RELATION_REGISTRY::ATTACK);
		}
	}

}

void CEntityAlive::Die	(CObject* who)
{
	if(IsGameTypeSingle())
		RELATION_REGISTRY().Action(smart_cast<CEntityAlive*>(who), this, RELATION_REGISTRY::KILL);
	inherited::Die(who);
	
	const CGameObject *who_object = smart_cast<const CGameObject*>(who);
	callback(GameObject::eDeath)(lua_game_object(), who_object ? who_object->lua_game_object() : 0);

	if (!getDestroy() && (GameID() == GAME_SINGLE)) {
		NET_Packet		P;
		u_EventGen		(P,GE_ASSIGN_KILLER,ID());
		P.w_u16			(u16(who->ID()));
		u_EventSend		(P);
	}

	// disable react to sound
	ISpatial* self	= smart_cast<ISpatial*> (this);
	if (self)		self->spatial.type &=~STYPE_REACTTOSOUND;
}

//вывзывает при подсчете хита
float CEntityAlive::CalcCondition(float /**hit/**/)
{	
	conditions().UpdateCondition();

	//dont call inherited::CalcCondition it will be meaningless
	return conditions().GetHealthLost();//*100.f;
}

///////////////////////////////////////////////////////////////////////
u16	CEntityAlive::PHGetSyncItemsNumber()
{
	if(character_physics_support()->movement()->CharacterExist()) return 1;
	else										  return inherited::PHGetSyncItemsNumber();
}
CPHSynchronize* CEntityAlive::PHGetSyncItem	(u16 item)
{
	if(character_physics_support()->movement()->CharacterExist()) return character_physics_support()->movement()->GetSyncItem();
	else										 return inherited::PHGetSyncItem(item);
}
void CEntityAlive::PHUnFreeze()
{
	if(character_physics_support()->movement()->CharacterExist()) character_physics_support()->movement()->UnFreeze();
	else if(m_pPhysicsShell) m_pPhysicsShell->UnFreeze();
}
void CEntityAlive::PHFreeze()
{
	if(character_physics_support()->movement()->CharacterExist()) character_physics_support()->movement()->Freeze();
	else if(m_pPhysicsShell) m_pPhysicsShell->Freeze();
}
//////////////////////////////////////////////////////////////////////

//добавление кровавых отметок на стенах, после получения хита
void CEntityAlive::BloodyWallmarks (float P, const Fvector &dir, s16 element, 
									const Fvector& position_in_object_space)
{
	if(BI_NONE == (u16)element)
		return;

	//вычислить координаты попадания
	CKinematics* V = smart_cast<CKinematics*>(Visual());
		
	Fvector start_pos = position_in_object_space;
	if(V)
	{
		Fmatrix& m_bone = (V->LL_GetBoneInstance(u16(element))).mTransform;
		m_bone.transform_tiny(start_pos);
	}
	XFORM().transform_tiny(start_pos);

	float small_entity = 1.f;
	if(Radius()<SMALL_ENTITY_RADIUS) small_entity = 0.5;


	float wallmark_size = m_fBloodMarkSizeMax;
	wallmark_size *= (P/m_fNominalHit);
	wallmark_size *= small_entity;
	clamp(wallmark_size, m_fBloodMarkSizeMin, m_fBloodMarkSizeMax);

	VERIFY(m_pBloodMarksVector);
	PlaceBloodWallmark(dir, start_pos, m_fBloodMarkDistance, 
						wallmark_size, *m_pBloodMarksVector);

}

void CEntityAlive::PlaceBloodWallmark(const Fvector& dir, const Fvector& start_pos, 
									  float trace_dist, float wallmark_size,
									  SHADER_VECTOR& wallmarks_vector)
{
	collide::rq_result	result;
	BOOL				reach_wall = 
		Level().ObjectSpace.RayPick(
			start_pos,
			dir,
			trace_dist, 
			collide::rqtBoth,
			result,
			this
		)
		&&
		!result.O;

	//если кровь долетела до статического объекта
	if(reach_wall)
	{
		CDB::TRI*	pTri	= Level().ObjectSpace.GetStaticTris()+result.element;
		SGameMtl*	pMaterial = GMLib.GetMaterialByIdx(pTri->material);

		if(pMaterial->Flags.is(SGameMtl::flBloodmark))
		{
			//вычислить нормаль к пораженной поверхности
			Fvector*	pVerts	= Level().ObjectSpace.GetStaticVerts();

			//вычислить точку попадания
			Fvector end_point;
			end_point.set(0,0,0);
			end_point.mad(start_pos, dir, result.range);

			ref_shader wallmarkShader = wallmarks_vector[::Random.randI(wallmarks_vector.size())];

			{
				//добавить отметку на материале
				::Render->add_StaticWallmark(wallmarkShader, end_point, wallmark_size, pTri, pVerts);
			}
		}
	}
}



void CEntityAlive::StartFireParticles(CWound* pWound)
{
	if(pWound->TypeSize(ALife::eHitTypeBurn)>m_fStartBurnWoundSize)
	{
		if(std::find(m_ParticleWounds.begin(),
			m_ParticleWounds.end(),
			pWound) == m_ParticleWounds.end())
		{
			m_ParticleWounds.push_back(pWound);
		}

		CKinematics* V = smart_cast<CKinematics*>(Visual());

		u16 particle_bone = CParticlesPlayer::GetNearestBone(V, pWound->GetBoneNum());
		VERIFY(particle_bone  < 64 || BI_NONE == particle_bone);

		pWound->SetParticleBoneNum(particle_bone);
		pWound->SetParticleName((*m_pFireParticlesVector)[::Random.randI(0,m_pFireParticlesVector->size())]);

		if(BI_NONE != particle_bone)
		{
			CParticlesPlayer::StartParticles(pWound->GetParticleName(), 
				pWound->GetParticleBoneNum(),
				Fvector().set(0,1,0),
				ID(), 
				u32(float(m_dwMinBurnTime)*::Random.randF(0.5f,1.5f)), false);
		}
		else
		{
			CParticlesPlayer::StartParticles(pWound->GetParticleName(), 
				Fvector().set(0,1,0),
				ID(), 
				u32(float(m_dwMinBurnTime)*::Random.randF(0.5f,1.5f)), false);
		}
	}
}

void CEntityAlive::UpdateFireParticles()
{
	if(m_ParticleWounds.empty()) return;
	
//	WOUND_VECTOR_IT last_it;

	for(WOUND_VECTOR_IT it = m_ParticleWounds.begin(); 
					  it != m_ParticleWounds.end();)
	{
		CWound* pWound = *it;
		float burn_size = pWound->TypeSize(ALife::eHitTypeBurn);

		if(pWound->GetDestroy() || 
			(burn_size>0 && (burn_size<m_fStopBurnWoundSize || !g_Alive())))
		{
			CParticlesPlayer::AutoStopParticles(pWound->GetParticleName(),
												pWound->GetParticleBoneNum(),
												u32(float(m_dwMinBurnTime)*::Random.randF(0.5f,1.5f))
												);
			it = m_ParticleWounds.erase(it);
			continue;
		}
		it++;
	}
}

ALife::ERelationType CEntityAlive::tfGetRelationType	(const CEntityAlive *tpEntityAlive) const
{
	int relation = MONSTER_COMMUNITY::relation(this->monster_community->index(), tpEntityAlive->monster_community->index());

	switch(relation) {
		case 1:		return(ALife::eRelationTypeFriend);		break;
		case 0:		return(ALife::eRelationTypeNeutral);	break;
		case -1:	return(ALife::eRelationTypeEnemy);		break;
		case -2:	return(ALife::eRelationTypeWorstEnemy);	break;
		
		default:	return(ALife::eRelationTypeDummy);		break;
	}
};

bool CEntityAlive::is_relation_enemy(const CEntityAlive *tpEntityAlive) const
{
	return ((tfGetRelationType(tpEntityAlive) == ALife::eRelationTypeEnemy) ||  
		(tfGetRelationType(tpEntityAlive) == ALife::eRelationTypeWorstEnemy));
}

void CEntityAlive::StartBloodDrops			(CWound* pWound)
{
	if(pWound->BloodSize()>m_fStartBloodWoundSize)
	{
		if(std::find(m_BloodWounds.begin(), m_BloodWounds.end(),
			  pWound) == m_BloodWounds.end())
		{
			m_BloodWounds.push_back(pWound);
			pWound->m_fDropTime = 0.f;
		}
	}
}

void CEntityAlive::UpdateBloodDrops()
{
	static float m_fBloodDropTimeMax = pSettings->r_float(BLOOD_MARKS_SECT, "blood_drop_time_max");
	static float m_fBloodDropTimeMin = pSettings->r_float(BLOOD_MARKS_SECT, "blood_drop_time_min");

	if(m_BloodWounds.empty()) return;

	if(!g_Alive())
	{
		m_BloodWounds.clear();
		return;
	}

//	WOUND_VECTOR_IT last_it;

	for(WOUND_VECTOR_IT it = m_BloodWounds.begin(); 
		it != m_BloodWounds.end();)
	{
		CWound* pWound = *it;
		float blood_size = pWound->BloodSize();

		if(pWound->GetDestroy() || blood_size < m_fStopBloodWoundSize)
		{
			it =  m_BloodWounds.erase(it);
			continue;
		}

		if(pWound->m_fDropTime<Device.fTimeGlobal)
		{
			float size_k = blood_size - m_fStopBloodWoundSize;
			size_k = size_k<1.f?size_k:1.f;
			pWound->m_fDropTime = Device.fTimeGlobal + (m_fBloodDropTimeMax - (m_fBloodDropTimeMax-m_fBloodDropTimeMin)*size_k)*Random.randF(0.8f, 1.2f);
			VERIFY(m_pBloodDropsVector);
			if(pWound->GetBoneNum() != BI_NONE)
			{
				Fvector pos;
				Fvector pos_distort;
				pos_distort.random_dir();
				pos_distort.mul(0.15f);
				CParticlesPlayer::GetBonePos(this, pWound->GetBoneNum(), Fvector().set(0,0,0), pos);
				pos.add(pos_distort);
				PlaceBloodWallmark(Fvector().set(0.f, -1.f, 0.f),
								pos, m_fBloodMarkDistance, 
								m_fBloodDropSize, *m_pBloodDropsVector);
			}
		}
		it++;
	}
}

void CEntityAlive::save	(NET_Packet &output_packet)
{
	inherited::save(output_packet);
	conditions().save(output_packet);
}

void CEntityAlive::load	(IReader &input_packet)
{
	inherited::load(input_packet);
	conditions().load(input_packet);
}

BOOL	CEntityAlive::net_SaveRelevant		()
{
	return		(TRUE);
}

CEntityConditionSimple* CEntityAlive::create_entity_condition	(CEntityConditionSimple* ec)
{
	if(!ec)
		m_entity_condition		= xr_new<CEntityCondition>(this);
	else
		m_entity_condition		= smart_cast<CEntityCondition*>(ec);
	
	return		(inherited::create_entity_condition(m_entity_condition));
}

/*
float CEntityAlive::GetfHealth	() const
{
	return conditions().health()*100.f;
}

float CEntityAlive::SetfHealth	(float value)
{
	conditions().health() = value/100.f;
	return value;
}
*/
float CEntityAlive::SetfRadiation		(float value)
{
	conditions().radiation() = value/100.f;
	return value;
}
/*
float CEntityAlive::g_Health	() const
{
	return conditions().GetHealth()*100.f;
}
float CEntityAlive::g_MaxHealth	() const
{
	return conditions().GetMaxHealth()*100.f;
}
*/
float CEntityAlive::g_Radiation	()	const
{
	return conditions().GetRadiation()*100.f;
}


DLL_Pure *CEntityAlive::_construct	()
{
	inherited::_construct	();
	if(character_physics_support())m_material_manager		= xr_new<CMaterialManager>(this,character_physics_support()->movement());
	return					(this);
}

u32	CEntityAlive::ef_creature_type	() const
{
	return	(m_ef_creature_type);
}

u32	CEntityAlive::ef_weapon_type	() const
{
	VERIFY	(m_ef_weapon_type != u32(-1));
	return	(m_ef_weapon_type);
}

u32	 CEntityAlive::ef_detector_type	() const
{
	VERIFY	(m_ef_detector_type != u32(-1));
	return	(m_ef_detector_type);
}
void CEntityAlive::PHGetLinearVell(Fvector& velocity)
{
	if(character_physics_support())
	{
		character_physics_support()->PHGetLinearVell(velocity);
	}
	else
		inherited::PHGetLinearVell(velocity);

}
CIKLimbsController*	CEntityAlive::character_ik_controller()
{
	if(character_physics_support())
	{
		return character_physics_support()->ik_controller();
	}
	else
	{
		return NULL;
	}
}
CPHSoundPlayer* CEntityAlive::ph_sound_player()
{
	if(character_physics_support())
	{
		return character_physics_support()->ph_sound_player();
	}
	else
	{
		return NULL;
	}
}

SCollisionHitCallback*	CEntityAlive::	get_collision_hit_callback		()
{
  CCharacterPhysicsSupport *cs=character_physics_support();
  if(cs)return cs->get_collision_hit_callback();
  else return false;
}

bool					CEntityAlive::	set_collision_hit_callback		(SCollisionHitCallback *cc)
{
	CCharacterPhysicsSupport* cs=character_physics_support();
	if(cs)return cs->set_collision_hit_callback(cc);
	else return false;
}

void CEntityAlive::net_Relcase	(CObject *object)
{
	inherited::net_Relcase		(object);
	conditions().remove_links	(object);
}

void	CEntityAlive::		create_anim_mov_ctrl	( CBlend* b )
{
	inherited::create_anim_mov_ctrl( b ); 
	CCharacterPhysicsSupport *cs = character_physics_support( );
	 if( cs )
		 cs->on_create_anim_mov_ctrl( );
}
void	CEntityAlive::	destroy_anim_mov_ctrl( )
{
	 inherited::destroy_anim_mov_ctrl(); 
	 CCharacterPhysicsSupport *cs =character_physics_support( );
	 if( cs )
		cs->on_destroy_anim_mov_ctrl( );
}