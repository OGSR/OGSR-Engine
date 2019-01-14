//#pragma once
//#include "../states/monster_state_attack.h"
//
//template<typename _Object>
//class	CBloodsuckerStateAttack : public CStateMonsterAttack<_Object> {
//	typedef CStateMonsterAttack<_Object> inherited_attack;
//
//	u32				m_time_stop_invis;
//	Fvector			m_dir_point;
//
//	float           m_last_health;
//	bool            m_start_with_encircle;
//
//public:
//					CBloodsuckerStateAttack		(_Object *obj);
//	virtual			~CBloodsuckerStateAttack	();
//
//	virtual	void	initialize					();
//	virtual	void	execute						();
//	virtual	void	finalize					();
//	virtual	void	critical_finalize			();
//	
//	virtual void	setup_substates				();
//private:
//			bool	check_hiding				();
//			bool	check_vampire				();
//};
//
//
//#include "bloodsucker_attack_state_inline.h"
