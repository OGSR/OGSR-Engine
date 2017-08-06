#include "stdafx.h"
#include "ai_monster_squad.h"
#include "../../entity.h"

CMonsterSquad::CMonsterSquad() : leader(0) 
{
	m_locked_covers.reserve	(20);
	m_locked_corpses.reserve(10);
}

CMonsterSquad::~CMonsterSquad() 
{
}

void CMonsterSquad::RegisterMember(CEntity *pE)
{
	// Добавить цель
	SMemberGoal			G;
	m_goals.insert		(mk_pair(pE, G));
	
	// Добавить команду
	SSquadCommand		C;
	C.type				= SC_NONE;
	m_commands.insert	(mk_pair(pE, C));
	
	// установить лидера
	if (!leader) leader = pE;
}

void CMonsterSquad::RemoveMember(CEntity *pE)
{
	// удалить из целей
	MEMBER_GOAL_MAP_IT it_goal = m_goals.find(pE);
	if (it_goal == m_goals.end()) return;	
	m_goals.erase(it_goal);

	// удалить из команд
	MEMBER_COMMAND_MAP_IT it_command = m_commands.find(pE);
	if (it_command == m_commands.end()) return;	
	m_commands.erase(it_command);

	// если удаляемый елемент является лидером - переназначить лидера
	if (leader == pE)  {
		if (m_goals.empty()) leader = 0;
		else leader = m_goals.begin()->first;
	}

	// усли последний элемент, очистить залоченные каверы
	if (m_goals.empty()) {
		m_locked_covers.clear	();
		m_locked_corpses.clear	();
	}
}

bool CMonsterSquad::SquadActive()
{
	if (!leader) return false;

	// проверить количество живых объектов в группе
	u32 alive_num = 0;
	for (MEMBER_GOAL_MAP_IT it = m_goals.begin(); it != m_goals.end(); it++) 
		if (it->first->g_Alive()) alive_num++;
	
	if (alive_num < 2) return false;
	
	return true;
}


void CMonsterSquad::UpdateGoal(CEntity *pE, const SMemberGoal &goal)
{
	MEMBER_GOAL_MAP_IT it = m_goals.find(pE);
	VERIFY(it != m_goals.end());

	it->second = goal;
}

void CMonsterSquad::UpdateCommand(CEntity *pE, const SSquadCommand &com)
{
	MEMBER_COMMAND_MAP_IT it = m_commands.find(pE);
	VERIFY(it != m_commands.end());

	it->second = com;
}

SMemberGoal &CMonsterSquad::GetGoal(CEntity *pE)
{
	MEMBER_GOAL_MAP_IT it = m_goals.find(pE);
	VERIFY(it != m_goals.end());

	return it->second;
}

SSquadCommand &CMonsterSquad::GetCommand(CEntity *pE)
{
	MEMBER_COMMAND_MAP_IT it = m_commands.find(pE);
	VERIFY(it != m_commands.end());
	return it->second;
}

void CMonsterSquad::GetGoal(CEntity *pE, SMemberGoal &goal)
{
	goal = GetGoal(pE);
}

void CMonsterSquad::GetCommand(CEntity *pE, SSquadCommand &com)
{
	com	= GetCommand(pE);
}

void CMonsterSquad::UpdateSquadCommands()
{
	// Отменить все команды в группе
	for (MEMBER_COMMAND_MAP_IT it = m_commands.begin(); it != m_commands.end(); it++) {
		it->second.type = SC_NONE;
	}

	// Удалить все цели, объекты которых невалидны или ушли в оффлайн
	for (MEMBER_GOAL_MAP_IT it_goal = m_goals.begin(); it_goal != m_goals.end(); ++it_goal) {
		SMemberGoal goal = it_goal->second;
		if (!goal.entity || goal.entity->getDestroy()) {
			it_goal->second.type = MG_None;
		}
	}

	ProcessAttack	();
	ProcessIdle		();
}

void CMonsterSquad::remove_links(CObject *O)
{
	// Удалить все цели, объекты которых невалидны или ушли в оффлайн
	for (MEMBER_GOAL_MAP_IT it_goal = m_goals.begin(); it_goal != m_goals.end(); ++it_goal) {
		SMemberGoal goal = it_goal->second;
		if (goal.entity == O) {
			it_goal->second.entity	= 0;
			it_goal->second.type	= MG_None;
		}
	}

	// Удалить все цели, объекты которых невалидны или ушли в оффлайн
	for (MEMBER_COMMAND_MAP_IT it = m_commands.begin(); it != m_commands.end(); it++) {
		SSquadCommand com = it->second;
		if (com.entity == O) {
			it->second.entity	= 0;
			it->second.type		= SC_NONE;
		}
	}
}


bool CMonsterSquad::is_locked_cover(u32 node)
{
	return	(
		std::find(
			m_locked_covers.begin(),
			m_locked_covers.end(),
			node
		)
		!=
		m_locked_covers.end()
	);
}

void CMonsterSquad::lock_cover(u32 node)
{
	m_locked_covers.push_back(node);
}

void CMonsterSquad::unlock_cover(u32 node)
{
	NODES_VECTOR_IT it = std::find(m_locked_covers.begin(), m_locked_covers.end(), node);
	if (it != m_locked_covers.end())
		m_locked_covers.erase(it);
}

u8 CMonsterSquad::get_count(const CEntity *object, float radius)
{
	u8 count = 0;

	for (MEMBER_GOAL_MAP_IT it_goal = m_goals.begin(); it_goal != m_goals.end(); ++it_goal) {
		SMemberGoal goal = it_goal->second;
		if ((goal.entity != 0) && (goal.entity != object) && (goal.entity->g_Alive())) {
			if (goal.entity->Position().distance_to(object->Position()) < radius) count++;
		}
	}

	return count;
}

//////////////////////////////////////////////////////////////////////////
// Corpses
//////////////////////////////////////////////////////////////////////////
bool CMonsterSquad::is_locked_corpse(const CEntityAlive *corpse)
{
	return	(
		std::find(
			m_locked_corpses.begin(),
			m_locked_corpses.end(),
			corpse
		)
		!=
		m_locked_corpses.end()
	);
}

void CMonsterSquad::lock_corpse(const CEntityAlive *corpse)
{
	m_locked_corpses.push_back(corpse);
}

void CMonsterSquad::unlock_corpse(const CEntityAlive *corpse)
{
	CORPSES_VECTOR_IT it = std::find(m_locked_corpses.begin(), m_locked_corpses.end(), corpse);
	if (it != m_locked_corpses.end())
		m_locked_corpses.erase(it);
}
//////////////////////////////////////////////////////////////////////////
