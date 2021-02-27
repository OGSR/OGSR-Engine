// Entity.cpp: implementation of the CEntity class.

//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "hudmanager.h"
#include "Entity.h"
#include "actor.h"
#include "xrserver_objects_alife_monsters.h"
#include "entity.h"
#include "level.h"
#include "seniority_hierarchy_holder.h"
#include "team_hierarchy_holder.h"
#include "squad_hierarchy_holder.h"
#include "group_hierarchy_holder.h"
#include "clsid_game.h"
#include "../Include/xrRender/Kinematics.h"
#include "monster_community.h"
#include "ai_space.h"
#include "memory_space.h"

#define BODY_REMOVE_TIME		600000
#define FORGET_KILLER_TIME 180


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEntity::CEntity()
{
	m_registered_member		= false;
}

CEntity::~CEntity()
{	
	xr_delete				(m_entity_condition);
}


CEntityConditionSimple *CEntity::create_entity_condition(CEntityConditionSimple* ec)
{
	if(!ec)
		m_entity_condition		= xr_new<CEntityConditionSimple>();
	else
		m_entity_condition		= smart_cast<CEntityCondition*>(ec);
	
	return		m_entity_condition;
}

void CEntity::OnEvent		(NET_Packet& P, u16 type)
{
	inherited::OnEvent		(P,type);

	switch (type)
	{

	case GE_DIE:
		{
			u16				id;
			u32				cl;
			P.r_u16			(id);
			P.r_u32			(cl);
			CObject			*who = Level().Objects.net_Find	(id);
			if (who) {
				if (this!=who) {
					if(bDebug)
						Msg("%s %s %s %s",*cName(),"Killed by ",*(who->cName()), "...");
				}
				else
					if(bDebug)
						Msg("%s %s",*cName(),"Crashed...");
			};
			Die				(who);
		}
		break;
	}
}

void CEntity::Die(CObject* who)
{
	if (!AlreadyDie()) set_death_time();
	SetfHealth			(-1.f);

	VERIFY(m_registered_member);

	if (m_registered_member)
	{
		m_registered_member = false;
		Level().seniority_holder().team(g_Team()).squad(g_Squad()).group(g_Group()).unregister_member(this);
	}
}

//обновление состояния
float CEntity::CalcCondition(float hit)
{

	// If Local() - perform some logic
	if (Local() && g_Alive()) {
		SetfHealth			(GetfHealth()-hit);
		SetfHealth			((GetfHealth()<-1000)?-1000:GetfHealth());
	}
	return hit;
}




//void CEntity::Hit			(float perc, Fvector &dir, CObject* who, s16 element,Fvector position_in_object_space, float impulse, ALife::EHitType hit_type) 
void	CEntity::Hit		(SHit* pHDS)
{

	if (bDebug)				Log("Process HIT: ", *cName());

	// *** process hit calculations
	// Calc impulse
	Fvector					vLocalDir;

	VERIFY(pHDS->dir.magnitude() > EPS);
	
	// convert impulse into local coordinate system
	Fmatrix					mInvXForm;
	mInvXForm.invert		(XFORM());
	mInvXForm.transform_dir	(vLocalDir,pHDS->dir);
	vLocalDir.invert		();

	// hit impulse
	if(pHDS->impulse) HitImpulse				(pHDS->impulse,pHDS->dir,vLocalDir); // @@@: WT
	
	// Calc amount (correct only on local player)
	float lost_health = CalcCondition(pHDS->damage());

	// Signal hit
	if(BI_NONE!=pHDS->bone())	HitSignal(lost_health,vLocalDir,pHDS->who,pHDS->boneID);

	// If Local() - perform some logic
	if (Local() && !g_Alive() && !AlreadyDie() && (m_killer_id == ALife::_OBJECT_ID(-1))) {
		KillEntity	(pHDS->whoID);
	}
	//must be last!!! @slipch
	inherited::Hit(pHDS);
}

void CEntity::Load		(LPCSTR section)
{
	inherited::Load		(section);

	setVisible			(FALSE);
	
	// Team params
	id_Team				= READ_IF_EXISTS(pSettings,r_s32,section,"team",-1);
	id_Squad			= READ_IF_EXISTS(pSettings,r_s32,section,"squad",-1);
	id_Group			= READ_IF_EXISTS(pSettings,r_s32,section,"group",-1);
	
#pragma todo("Jim to Dima: no specific figures or comments needed")	
	m_fMorale			= 66.f;

	m_forget_killer_time = READ_IF_EXISTS( pSettings, r_u32, section, "forget_killer_time", FORGET_KILLER_TIME ) * 1000;
}

BOOL CEntity::net_Spawn		(CSE_Abstract* DC)
{
	m_level_death_time		= 0;
	m_game_death_time		= 0;
	m_killer_id				= ALife::_OBJECT_ID(-1);

	CSE_Abstract				*e	= (CSE_Abstract*)(DC);
	CSE_ALifeCreatureAbstract	*E	= smart_cast<CSE_ALifeCreatureAbstract*>(e);

	// Initialize variables
	if (E) {
		SetfHealth			(E->fHealth);
		VERIFY				((E->m_killer_id == ALife::_OBJECT_ID(-1)) || !g_Alive());
		m_killer_id			= E->m_killer_id;
		if (m_killer_id == ID())
			m_killer_id		= ALife::_OBJECT_ID(-1);
	}
	else
		SetfHealth			(1.0f);

	// load damage params
	if (!E) {
		// Car or trader only!!!!
		CSE_ALifeCar		*C	= smart_cast<CSE_ALifeCar*>(e);
		CSE_ALifeTrader		*T	= smart_cast<CSE_ALifeTrader*>(e);
		CSE_ALifeHelicopter	*H	= smart_cast<CSE_ALifeHelicopter*>(e);

		R_ASSERT2			(C||T||H,"Invalid entity (no inheritance from CSE_CreatureAbstract, CSE_ALifeItemCar and CSE_ALifeTrader and CSE_ALifeHelicopter)!");
		id_Team				= id_Squad = id_Group = 0;
	}
	else {
		id_Team				= E->g_team();
		id_Squad			= E->g_squad();
		id_Group			= E->g_group();

		CSE_ALifeMonsterBase	*monster	= smart_cast<CSE_ALifeMonsterBase*>(E);
		if (monster) {
			MONSTER_COMMUNITY		monster_community;
			monster_community.set	(pSettings->r_string(*cNameSect(), "species"));

			if(monster_community.team() != 255)
				id_Team = monster_community.team();
		}
	}

	if ( g_Alive() ) {
          m_registered_member = true;
          auto& squad = Level().seniority_holder().team( g_Team() ).squad( g_Squad() );
          while ( squad.group( g_Group() ).members().size() == sizeof( squad_mask_type ) * 8 ) {
            Msg( "* [%s]: [%s]: group [team:%u][squad:%u][group:%u] is full (%u), try next group %u", __FUNCTION__, ( E && E->name_replace()[ 0 ] ) ? E->name_replace() : cName().c_str(), g_Team(), g_Squad(), g_Group(), squad.group( g_Group() ).members().size(), g_Group() + 1 );
            ++id_Group;
          }
          squad.group( g_Group() ).register_member( this );
          ++squad.group( g_Group() ).m_dwAliveCount;
	}

	if(!g_Alive())
	{
		m_level_death_time		= Device.dwTimeGlobal;
		m_game_death_time		= E->m_game_death_time;;
	}

	if (!inherited::net_Spawn(DC))
		return				(FALSE);

//	SetfHealth			(E->fHealth);
	IKinematics* pKinematics=smart_cast<IKinematics*>(Visual());
	CInifile* ini = NULL;

	if(pKinematics) ini = pKinematics->LL_UserData();
	if (ini) {
		if (ini->section_exist("damage_section") && !use_simplified_visual())
			CDamageManager::reload(pSettings->r_string("damage_section","damage"),ini);

		CParticlesPlayer::LoadParticles(pKinematics);
	}
	return					TRUE;
}

void CEntity::net_Destroy	()
{
	if (m_registered_member) {
		m_registered_member	= false;
		Level().seniority_holder().team(g_Team()).squad(g_Squad()).group(g_Group()).unregister_member(this);
	}

	inherited::net_Destroy	();
}

void CEntity::KillEntity(u16 whoID)
{
	if (whoID != ID()) {
#ifdef DEBUG
		if (m_killer_id != ALife::_OBJECT_ID(-1)) {
			Msg			("! Entity [%s][%s] already has killer with id %d, but new killer id arrived - %d",*cNameSect(),*cName(),m_killer_id,whoID);

			CObject		*old_killer = Level().Objects.net_Find(m_killer_id);
			Msg			("! Old killer is %s",old_killer ? *old_killer->cName() : "unknown");

			CObject		*new_killer = Level().Objects.net_Find(whoID);
			Msg			("! New killer is %s",new_killer ? *new_killer->cName() : "unknown");

			VERIFY		(m_killer_id == ALife::_OBJECT_ID(-1));
		}
#endif
	}
	else {
		if (m_killer_id != ALife::_OBJECT_ID(-1))
			return;
	}

	m_killer_id			= whoID;

	set_death_time		();

	if (!getDestroy()){
		NET_Packet		P;
		u_EventGen		(P,GE_DIE,ID());
		P.w_u16			(u16(whoID));
		P.w_u32			(0);
		if (OnServer())
			u_EventSend	(P, net_flags(TRUE, TRUE, FALSE, TRUE));
	}
};

//void CEntity::KillEntity(CObject* who)
//{
//	VERIFY			(who);
//	if (who) KillEntity(who->ID());	
//}

void CEntity::reinit			()
{
	inherited::reinit			();
}


void CEntity::reload			(LPCSTR section)
{
	inherited::reload			(section);
	if (!use_simplified_visual())
		CDamageManager::reload	(section,"damage",pSettings);
}

void CEntity::set_death_time	()
{
	m_level_death_time	= Device.dwTimeGlobal;
	m_game_death_time	= Level().GetGameTime();
}

bool CEntity::IsFocused			()const	{ return (smart_cast<const CEntity*>(g_pGameLevel->CurrentEntity())==this);		}
bool CEntity::IsMyCamera		()const	{ return (smart_cast<const CEntity*>(g_pGameLevel->CurrentViewEntity())==this);	}

DLL_Pure *CEntity::_construct	()
{
	inherited::_construct		();
	CDamageManager::_construct	();
	m_entity_condition			= create_entity_condition(NULL);
	return						(this);
}

void CEntity::shedule_Update	(u32 dt)
{
	inherited::shedule_Update	(dt);
	if (!getDestroy() && !g_Alive() && (m_killer_id != u16(-1))) {
		if ( Device.dwTimeGlobal > m_level_death_time + m_forget_killer_time ) {
			m_killer_id			= u16(-1);
			NET_Packet			P;
			u_EventGen			(P,GE_ASSIGN_KILLER,ID());
			P.w_u16				(u16(-1));
			u_EventSend			(P);
		}
	}
}

void CEntity::on_before_change_team	()
{
}

void CEntity::on_after_change_team	()
{
}

void CEntity::ChangeTeam(int team, int squad, int group)
{
	if ((team == g_Team()) && (squad == g_Squad()) && (group == g_Group())) return;

	VERIFY2					(g_Alive(), "Try to change team of a dead object");
	
	VERIFY					(m_registered_member);
	// remove from current team
	on_before_change_team	();
	Level().seniority_holder().team(g_Team()).squad(g_Squad()).group(g_Group()).unregister_member	(this);

	id_Team					= team;
	id_Squad				= squad;
	id_Group				= group;

	// add to new team
	Level().seniority_holder().team(g_Team()).squad(g_Squad()).group(g_Group()).register_member		(this);
	on_after_change_team	();
}
