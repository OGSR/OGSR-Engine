#pragma once

class CBaseMonster;
class CPatrolPath;

class CMonsterHome {
	CBaseMonster		*m_object;
	const CPatrolPath	*m_path;

	float				m_radius_min;
	float				m_radius_max;

	bool				m_aggressive;

public:
				CMonsterHome		(CBaseMonster *obj) {m_object = obj;}

		void	load				(LPCSTR line);
		void	setup				(LPCSTR path_name, float min_radius, float max_radius, bool aggressive = false);
		void	remove_home			();

		u32		get_place			();
		u32		get_place_in_cover	();
		bool	at_home				();
		bool	at_home				(const Fvector &pos);
	IC	bool	has_home			() {return (m_path != 0);}
	IC	bool	is_aggressive		() {return m_aggressive;}
};