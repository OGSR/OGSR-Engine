////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_online_offline_group.cpp
//	Created 	: 25.10.2005
//  Modified 	: 25.10.2005
//	Author		: Dmitriy Iassenev
//	Description : ALife Online Offline Group class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "xrServer_Objects_ALife_Monsters.h"
#include "ai_space.h"
#include "alife_simulator.h"
#include "alife_object_registry.h"
#include "alife_graph_registry.h"
#include "alife_schedule_registry.h"
#include "game_level_cross_table.h"
#include "level_graph.h"

#pragma warning(push)
#pragma warning(disable:4995)
#include <malloc.h>
#pragma warning(pop)

CSE_ALifeItemWeapon *CSE_ALifeOnlineOfflineGroup::tpfGetBestWeapon		(ALife::EHitType &tHitType, float &fHitPower)
{
	return						(0);
}

ALife::EMeetActionType CSE_ALifeOnlineOfflineGroup::tfGetActionType		(CSE_ALifeSchedulable *tpALifeSchedulable, int iGroupIndex, bool bMutualDetection)
{
	return						(ALife::eMeetActionTypeIgnore);
}

bool CSE_ALifeOnlineOfflineGroup::bfActive								()
{
	return						(false);
}

CSE_ALifeDynamicObject *CSE_ALifeOnlineOfflineGroup::tpfGetBestDetector	()
{
	return						(0);
}

void CSE_ALifeOnlineOfflineGroup::update								()
{
	return;
}

void CSE_ALifeOnlineOfflineGroup::register_member						(ALife::_OBJECT_ID member_id)
{
	VERIFY						(m_members.find(member_id) == m_members.end());
	CSE_ALifeDynamicObject		*object = ai().alife().objects().object(member_id);
	CSE_ALifeHumanStalker		*stalker = smart_cast<CSE_ALifeHumanStalker*>(object);
	VERIFY						(stalker);
	VERIFY						(stalker->g_Alive());

	VERIFY						((stalker->m_group_id == 0xffff) || (stalker->m_group_id == ID));
	stalker->m_group_id			= ID;

	bool						empty = m_members.empty();
	m_members.insert			(std::make_pair(member_id,stalker));
	if (empty) {
		o_Position				= stalker->o_Position;
		m_tNodeID				= stalker->m_tNodeID;
		m_tGraphID				= stalker->m_tGraphID;

		m_flags.set				(flUsedAI_Locations,TRUE);

		alife().graph().update	(this);
	}

	if (!object->m_bOnline) {
		alife().graph().remove			(object,object->m_tGraphID);
		alife().scheduled().remove		(object);
	}
	else {
		VERIFY							(object->ID_Parent == 0xffff);
		alife().graph().level().remove	(object);
	}
}

void CSE_ALifeOnlineOfflineGroup::unregister_member						(ALife::_OBJECT_ID member_id)
{
	CALifeGraphRegistry			&graph = alife().graph();
	CALifeLevelRegistry			&level = graph.level();

	MEMBERS::iterator			I = m_members.find(member_id);
	VERIFY						(I != m_members.end());
	VERIFY						((*I).second->m_group_id == ID);
	(*I).second->m_group_id		= 0xffff;

	graph.update				((*I).second);
	alife().scheduled().add		((*I).second);

	m_members.erase				(I);

	if (m_members.empty()) {
		if (!m_bOnline) {
			graph.remove		(this,m_tGraphID);
		}
		else {
			if (ID_Parent == 0xffff)
				level.remove	(this);
		}

		m_flags.set				(flUsedAI_Locations,FALSE);
	}
}

CSE_ALifeOnlineOfflineGroup::MEMBER *CSE_ALifeOnlineOfflineGroup::member(ALife::_OBJECT_ID member_id, bool no_assert)
{
	MEMBERS::iterator			I = m_members.find(member_id);
	if (I == m_members.end()) {
		if (!no_assert)
			Msg					("! There is no member with id %d in the OnlineOfflineGroup id %d",member_id,ID);
		VERIFY					(no_assert);
		return					(0);
	}
	return						((*I).second);
}

bool CSE_ALifeOnlineOfflineGroup::synchronize_location	()
{
	if (m_members.empty())
		return					(true);

	MEMBERS::iterator			I = m_members.begin();
	MEMBERS::iterator			E = m_members.end();
	for ( ; I != E; ++I)
		(*I).second->synchronize_location	();

	MEMBER						&member = *(*m_members.begin()).second;
	o_Position					= member.o_Position;
	m_tNodeID					= member.m_tNodeID;

	if (m_tGraphID != member.m_tGraphID) {
		if (!m_bOnline)
			alife().graph().change	(this,m_tGraphID,member.m_tGraphID);
		else
			m_tGraphID			= member.m_tGraphID;
	}

	m_fDistance					= member.m_fDistance;
	return						(true);
}

void CSE_ALifeOnlineOfflineGroup::try_switch_online		()
{
	if (m_members.empty())
		return;

	if (!can_switch_online())
		return;

	if (!can_switch_offline()) {
		//.
		o_Position				= alife().graph().actor()->o_Position;

        inherited1::try_switch_online	();
		return;
	}
	
	MEMBERS::iterator			I = m_members.begin();
	MEMBERS::iterator			E = m_members.end();
	for ( ; I != E; ++I) {
		VERIFY3					((*I).second->g_Alive(),"Incorrect situation : some of the OnlineOffline group members is dead",(*I).second->name_replace());
		VERIFY3					((*I).second->can_switch_online(),"Incorrect situation : some of the OnlineOffline group members cannot be switched online due to their personal properties",(*I).second->name_replace());
		VERIFY3					((*I).second->can_switch_offline(),"Incorrect situation : some of the OnlineOffline group members cannot be switched online due to their personal properties",(*I).second->name_replace());

		if (alife().graph().actor()->o_Position.distance_to((*I).second->o_Position) > alife().offline_distance())
			continue;

		//.
		o_Position				= (*I).second->o_Position;

        inherited1::try_switch_online	();
		return;
	}
}

void CSE_ALifeOnlineOfflineGroup::try_switch_offline	()
{
	if (m_members.empty())
		return;

	if (!can_switch_offline())
		return;
	
	if (!can_switch_online()) {
		alife().switch_offline	(this);
		return;
	}
	
	MEMBERS::iterator			I = m_members.begin();
	MEMBERS::iterator			E = m_members.end();
	for ( ; I != E; ++I) {
		VERIFY3					((*I).second->g_Alive(),"Incorrect situation : some of the OnlineOffline group members is dead",(*I).second->name_replace());
		VERIFY3					((*I).second->can_switch_offline(),"Incorrect situation : some of the OnlineOffline group members cannot be switched online due to their personal properties",(*I).second->name_replace());
		VERIFY3					((*I).second->can_switch_online(),"Incorrect situation : some of the OnlineOffline group members cannot be switched online due to their personal properties",(*I).second->name_replace());
		
		if (alife().graph().actor()->o_Position.distance_to((*I).second->o_Position) <= alife().offline_distance())
			return;
	}

	alife().switch_offline		(this);
}

void CSE_ALifeOnlineOfflineGroup::switch_online			()
{
	R_ASSERT					(!m_bOnline);
	m_bOnline					= true;

	MEMBERS::iterator			I = m_members.begin();
	MEMBERS::iterator			E = m_members.end();
	for ( ; I != E; ++I)
		alife().add_online		((*I).second, false);

	alife().scheduled().remove	(this);
	alife().graph().remove		(this,m_tGraphID,false);
}

void CSE_ALifeOnlineOfflineGroup::switch_offline		()
{
	R_ASSERT					(m_bOnline);
	m_bOnline					= false;

	if (!m_members.empty()) {
		MEMBER					*member = (*m_members.begin()).second;
		o_Position				= member->o_Position;
		m_tNodeID				= member->m_tNodeID;
		m_tGraphID				= member->m_tGraphID;
		m_fDistance				= member->m_fDistance;
	}

	MEMBERS::iterator			I = m_members.begin();
	MEMBERS::iterator			E = m_members.end();
	for ( ; I != E; ++I)
		alife().remove_online	((*I).second,false);

	alife().scheduled().add		(this);
	alife().graph().add			(this,m_tGraphID,false);
}

bool CSE_ALifeOnlineOfflineGroup::redundant				() const
{
	return						(m_members.empty());
}

void CSE_ALifeOnlineOfflineGroup::notify_on_member_death(MEMBER *member)
{
	unregister_member			(member->ID);
}

void CSE_ALifeOnlineOfflineGroup::on_before_register	()
{
	m_tGraphID					= GameGraph::_GRAPH_ID(-1);
	m_flags.set					(flUsedAI_Locations,FALSE);
}

void CSE_ALifeOnlineOfflineGroup::on_after_game_load	()
{
	if (m_members.empty())
		return;

	ALife::_OBJECT_ID			*temp = (ALife::_OBJECT_ID*)_alloca(m_members.size()*sizeof(ALife::_OBJECT_ID));
	ALife::_OBJECT_ID			*i = temp, *e = temp + m_members.size();

	{
		MEMBERS::const_iterator	I = m_members.begin();
		MEMBERS::const_iterator	E = m_members.end();
		for ( ; I != E; ++I, ++i) {
			VERIFY				(!(*I).second);
			*i					= (*I).first;
		}
	}

	m_members.clear				();

	for (i = temp; i != e; ++i)
		register_member			(*i);
}
