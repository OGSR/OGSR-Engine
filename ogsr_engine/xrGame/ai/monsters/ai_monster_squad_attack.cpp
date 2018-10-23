#include "stdafx.h"
#include "ai_monster_squad.h"
#include "../../entity.h"

void CMonsterSquad::ProcessAttack()
{
	m_enemy_map.clear		();
	m_temp_entities.clear	();

	// Выделить элементы с общими врагами и состянием атаки 
	for (MEMBER_GOAL_MAP_IT it_goal = m_goals.begin(); it_goal != m_goals.end(); it_goal++) {
//		CEntity *member = it_goal->first;
		SMemberGoal goal = it_goal->second;

		if (goal.type == MG_AttackEnemy) {
			VERIFY(goal.entity && !goal.entity->getDestroy());

			ENEMY_MAP_IT it = m_enemy_map.find(goal.entity);
			if (it != m_enemy_map.end()) {
				it->second.push_back(it_goal->first);
			} else {
				m_temp_entities.push_back	(it_goal->first);
				m_enemy_map.insert			(mk_pair(goal.entity, m_temp_entities));
			}
		}
	}

	// Пройти по всем группам и назначить углы всем елементам в группе
	for (ENEMY_MAP_IT it_enemy = m_enemy_map.begin(); it_enemy != m_enemy_map.end(); it_enemy++) {
		Attack_AssignTargetDir(it_enemy->second,it_enemy->first);
	}
}


struct sort_predicate {
	CEntity *enemy;

			
			sort_predicate	(CEntity *pEnemy) : enemy(pEnemy) {}

	bool	operator()		(CEntity *pE1, CEntity *pE2) const
	{
		return	(pE1->Position().distance_to(enemy->Position()) > 
			pE2->Position().distance_to(enemy->Position()));
	};
};

void CMonsterSquad::Attack_AssignTargetDir(ENTITY_VEC &members, CEntity *enemy)
{
	_elem					first;
	_elem					last;

	lines.clear();

	// сортировать по убыванию расстояния от npc до врага 
	std::sort(members.begin(), members.end(), sort_predicate(enemy));
	if (members.empty()) return;

	float delta_yaw = PI_MUL_2 / members.size();

	// обработать ближний элемент
	first.pE		= members.back();
	first.p_from	= first.pE->Position();
	first.yaw		= 0;
	members.pop_back();

	lines.push_back(first);

	// обработать дальний элемент
	if (!members.empty()) {
		last.pE			= members[0];
		last.p_from		= last.pE->Position();
		last.yaw		= PI;
		members.erase	(members.begin());

		lines.push_back(last);
	}

	Fvector target_pos = enemy->Position();
	float	next_right_yaw	= delta_yaw;
	float	next_left_yaw	= delta_yaw;

	// проходим с конца members в начало (начиная с наименьшего расстояния)
	while (!members.empty()) {
		CEntity *pCur;

		pCur = members.back();
		members.pop_back();

		_elem cur_line;
		cur_line.p_from		= pCur->Position();
		cur_line.pE			= pCur;

		// определить cur_line.yaw

		float h1,p1,h2,p2;
		Fvector dir;
		dir.sub(target_pos, first.p_from);
		dir.getHP(h1,p1);	
		dir.sub(target_pos, cur_line.p_from);
		dir.getHP(h2,p2);

		bool b_add_left = false;

		if (angle_normalize_signed(h2 - h1) > 0)  {		// right
			if ((next_right_yaw < PI) && !fsimilar(next_right_yaw, PI, PI/60.f)) b_add_left = false;
			else b_add_left = true;
		} else {										// left
			if ((next_left_yaw < PI) && !fsimilar(next_left_yaw, PI, PI/60.f)) b_add_left = true;
			else b_add_left = false;
		}

		if (b_add_left) {
			cur_line.yaw = -next_left_yaw;
			next_left_yaw += delta_yaw;
		} else {
			cur_line.yaw = next_right_yaw;
			next_right_yaw += delta_yaw;
		}

		lines.push_back(cur_line);
	}

	// Пройти по всем линиям и заполнить таргеты у npc
	float first_h, first_p;
	Fvector d; d.sub(target_pos,first.p_from);
	d.getHP(first_h, first_p);

	for (u32 i = 0; i < lines.size(); i++){
		SSquadCommand command;
		command.type			= SC_ATTACK;
		command.entity			= enemy;
		command.direction.setHP	(first_h + lines[i].yaw, first_p);
		UpdateCommand(lines[i].pE, command);
	}
}
