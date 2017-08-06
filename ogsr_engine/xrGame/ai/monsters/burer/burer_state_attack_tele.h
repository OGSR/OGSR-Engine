#pragma once
#include "../state.h"


template<typename _Object>
class CStateBurerAttackTele : public CState<_Object> {
	typedef CState<_Object> inherited;	

	xr_vector<CPhysicsShellHolder *>	tele_objects;
	CPhysicsShellHolder					*selected_object;
	xr_vector<CObject*>					m_nearest;
	
	u32									time_started;

	enum {
		ACTION_TELE_STARTED,
		ACTION_TELE_CONTINUE,
		ACTION_TELE_FIRE,
		ACTION_WAIT_TRIPLE_END,
		ACTION_COMPLETED,
	} m_action;

public:
						CStateBurerAttackTele	(_Object *obj);

	virtual	void		initialize				();
	virtual	void		execute					();
	virtual void		finalize				();
	virtual void		critical_finalize		();

	virtual bool		check_start_conditions	();
	virtual bool		check_completion		();


private:
			// Поиск объектов для телекинеза	
			void		FindObjects				();

			// выполнять состояние
			void		ExecuteTeleStart		();
			void		ExecuteTeleContinue		();
			void		ExecuteTeleFire			();

			// Проверка, есть ли хоть один объект под контролем
			bool		IsActiveObjects			();

			// Проверить, может ли стартовать телекинез
			bool		CheckTeleStart			();
			// Выбор подходящих объектов для телекинеза
			void		SelectObjects			();

			// internal for FindObjects
			void		FindFreeObjects			(xr_vector<CObject*> &tpObjects, const Fvector &pos);

private:
};

#include "burer_state_attack_tele_inline.h"
