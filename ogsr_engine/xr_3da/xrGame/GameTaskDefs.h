#pragma once

enum ETaskState {
	eTaskStateFail			= 0,
	eTaskStateInProgress,
	eTaskStateCompleted,
//.	eTaskUserDefined,
	eTaskStateDummy			= u32(-1)
};


typedef shared_str		TASK_ID;
DEFINE_VECTOR			(TASK_ID, TASK_ID_VECTOR, TASK_ID_IT);
extern shared_str		g_active_task_id;
extern u16				g_active_task_objective_id;

#include "alife_abstract_registry.h"

class CGameTask;

struct SGameTaskKey : public IPureSerializeObject<IReader,IWriter>,public IPureDestroyableObject {
	TASK_ID			task_id;
	CGameTask*		game_task;
	SGameTaskKey	(TASK_ID t_id):task_id(t_id),game_task(NULL){};
	SGameTaskKey	():task_id(NULL),game_task(NULL){};


	virtual void save								(IWriter &stream);
	virtual void load								(IReader &stream);
	virtual void destroy							();
};

DEFINE_VECTOR (SGameTaskKey, GameTasks, GameTasks_it);

struct CGameTaskRegistry : public CALifeAbstractRegistry<u16, GameTasks> {
	virtual void save(IWriter &stream){
		CALifeAbstractRegistry<u16, GameTasks>::save(stream);
		save_data		(g_active_task_id,				stream);
		save_data		(g_active_task_objective_id,	stream);
	};
	virtual void load(IReader &stream){
		CALifeAbstractRegistry<u16, GameTasks>::load(stream);
		load_data		(g_active_task_id,				stream);
		load_data		(g_active_task_objective_id,	stream);
	};
};
