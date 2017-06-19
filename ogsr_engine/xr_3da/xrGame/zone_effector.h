#pragma once

class CActor;
class CPostprocessAnimatorLerp;

class CZoneEffector {
	float						r_min_perc;
	float						r_max_perc;
	float						radius;
	float						m_factor;
	CPostprocessAnimatorLerp*	m_pp_effector;
	shared_str					m_pp_fname;
public:
			CZoneEffector		();
			~CZoneEffector		();

	CActor*						m_pActor;

	void	Load				(LPCSTR section);
	void	SetRadius			(float r);
	void	Update				(float dist);
	void	Stop				();
	float xr_stdcall GetFactor	();

private:
	void	Activate			();


};



