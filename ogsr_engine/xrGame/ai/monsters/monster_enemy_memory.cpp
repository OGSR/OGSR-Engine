#include "pch_script.h"
#include "monster_enemy_memory.h"
#include "BaseMonster/base_monster.h"
#include "../../memory_manager.h"
#include "../../visual_memory_manager.h"
#include "../../enemy_manager.h"
#include "../../ai_object_location.h"
#include "monster_home.h"

CMonsterEnemyMemory::CMonsterEnemyMemory()
{
	monster			= 0;
	time_memory		= 10000; 
}

CMonsterEnemyMemory::~CMonsterEnemyMemory()
{
}

void CMonsterEnemyMemory::init_external(CBaseMonster *M, TTime mem_time) 
{
	monster = M; 
	time_memory = mem_time;
}


void CMonsterEnemyMemory::update() 
{
	VERIFY		(monster->g_Alive());
	// Обновить врагов
	for (xr_vector<const CEntityAlive *>::const_iterator I = monster->memory().enemy().objects().begin(); I != monster->memory().enemy().objects().end(); ++I) {
		if (monster->memory().visual().visible_now(*I)) add_enemy(*I);
	}

	// удалить устаревших врагов
	remove_non_actual();

	// обновить опасность 
	for (ENEMIES_MAP_IT it = m_objects.begin(); it != m_objects.end(); it++) {
		u8		relation_value = u8(monster->tfGetRelationType(it->first));
		float	dist = monster->Position().distance_to(it->second.position);
		it->second.danger = (1 + relation_value*relation_value*relation_value) / (1 + dist);
	}
}

void CMonsterEnemyMemory::add_enemy(const CEntityAlive *enemy)
{
	SMonsterEnemy enemy_info;
	enemy_info.position = enemy->Position();
	enemy_info.vertex   = enemy->ai_location().level_vertex_id();
	enemy_info.time		= Device.dwTimeGlobal;
	enemy_info.danger	= 0.f;

	ENEMIES_MAP_IT it = m_objects.find(enemy);
	if (it != m_objects.end()) {
		// обновить данные о враге
		it->second = enemy_info;
	} else {
		// добавить врага в список объектов
		m_objects.insert(mk_pair(enemy, enemy_info));
	}
}

void CMonsterEnemyMemory::add_enemy(const CEntityAlive *enemy, const Fvector &pos, u32 vertex, u32 time)
{
	SMonsterEnemy enemy_info;
	enemy_info.position = pos;
	enemy_info.vertex   = vertex;
	enemy_info.time		= time;
	enemy_info.danger	= 0.f;

	ENEMIES_MAP_IT it = m_objects.find(enemy);
	if (it != m_objects.end()) {
		// обновить данные о враге
		if (it->second.time < enemy_info.time) it->second = enemy_info;
	} else {
		// добавить врага в список объектов
		m_objects.insert(mk_pair(enemy, enemy_info));
	}
}

void CMonsterEnemyMemory::remove_non_actual() 
{
	TTime cur_time = Device.dwTimeGlobal;

	// удалить 'старых' врагов и тех, расстояние до которых > 30м и др.
	for (ENEMIES_MAP_IT it = m_objects.begin(), nit; it != m_objects.end(); it = nit)
	{
		nit = it; ++nit;
		// проверить условия удаления
		if (	!it->first					|| 
			!it->first->g_Alive()		|| 
			it->first->getDestroy()		||
			(it->second.time + time_memory < cur_time) ||
			(it->first->g_Team() == monster->g_Team())
			) 
			m_objects.erase (it);
	}
}

const CEntityAlive *CMonsterEnemyMemory::get_enemy()
{
	ENEMIES_MAP_IT	it = find_best_enemy();
	if (it != m_objects.end()) return it->first;
	return (0);
}

SMonsterEnemy CMonsterEnemyMemory::get_enemy_info()
{
	SMonsterEnemy ret_val;
	ret_val.time = 0;

	ENEMIES_MAP_IT	it = find_best_enemy();
	if (it != m_objects.end()) ret_val = it->second;

	return ret_val;
}

ENEMIES_MAP_IT CMonsterEnemyMemory::find_best_enemy()
{
	ENEMIES_MAP_IT	it = m_objects.end();
	float			max_value = 0.f;

	// find best at home first
	for (ENEMIES_MAP_IT I = m_objects.begin(); I != m_objects.end(); I++) {
		if (!monster->Home->at_home(I->second.position)) continue;
		if (I->second.danger > max_value) {
			max_value = I->second.danger;
			it = I;
		}
	}

	// there is no best enemies at home
	if (it == m_objects.end()) {
		// find any
		max_value = 0.f;
		for (ENEMIES_MAP_IT I = m_objects.begin(); I != m_objects.end(); I++) {
			if (I->second.danger > max_value) {
				max_value = I->second.danger;
				it = I;
			}
		}
	}

	return it;
}

void CMonsterEnemyMemory::remove_links(CObject *O)
{
	for (ENEMIES_MAP_IT	I = m_objects.begin();I!=m_objects.end();++I) {
		if ((*I).first == O) {
			m_objects.erase(I);
			break;
		}
	}
}

