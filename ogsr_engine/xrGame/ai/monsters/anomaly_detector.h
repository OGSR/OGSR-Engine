#pragma once

class CCustomMonster;

class CAnomalyDetector {
	CCustomMonster			*m_object;

public:
	float					m_radius;
	u32						m_time_to_rememeber;
	float					m_detect_probability;

	bool					m_active;
	bool					m_forced;

private:
        struct SAnomalyInfo {
          u16 id;
          u32 time_registered;

          bool operator == ( CObject *obj ) {
            return ( obj->ID() == id );
          }
        };

	struct remove_predicate {
		u32		time_remember;
		remove_predicate (u32 time) : time_remember(time){}

		IC bool	operator() (const SAnomalyInfo &info) {
			return (info.time_registered + time_remember < Device.dwTimeGlobal);
		}
	};

	DEFINE_VECTOR			(SAnomalyInfo, ANOMALY_INFO_VEC, ANOMALY_INFO_VEC_IT);
	ANOMALY_INFO_VEC		m_storage;

public:
				CAnomalyDetector	(CCustomMonster *monster);
	virtual		~CAnomalyDetector	();

	void		load				(LPCSTR section);
	void		reinit				();

	void		update_schedule		();
	void		on_contact			(CObject *obj);
	
	void		activate( bool = false );
	void		deactivate( bool = false );
	void		remove_all_restrictions();
	void		remove_restriction( u16 );
};
