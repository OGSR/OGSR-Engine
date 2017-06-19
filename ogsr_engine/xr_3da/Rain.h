// Rain.h: interface for the CRain class.
//
//////////////////////////////////////////////////////////////////////

#ifndef RainH
#define RainH
#pragma once

#include "xr_collide_defs.h"

//refs
class ENGINE_API IRender_DetailModel;


//
class ENGINE_API CEffect_Rain
{
private:
	struct	Item
	{
		Fvector			P;
		Fvector			Phit;
		Fvector			D;
		float			fSpeed;
		u32				dwTime_Life;
		u32				dwTime_Hit;
		u32				uv_set;
		void			invalidate	()
		{
			dwTime_Life	= 0;
		}
	};
	struct	Particle
	{
		Particle		*next,*prev;
		Fmatrix			mXForm;
		Fsphere			bounds;
		float			time;
	};
public:
	enum	States
	{
		stIdle		= 0,
		stWorking
	};
private:
	// Visualization	(rain)
	ref_shader						SH_Rain;
	ref_geom						hGeom_Rain;

	// Visualization	(drops)
	IRender_DetailModel*			DM_Drop;
	ref_geom						hGeom_Drops;
	
	// Data and logic
	xr_vector<Item>					items;
	States							state;

	// Particles
	xr_vector<Particle>				particle_pool;
	Particle*						particle_active;
	Particle*						particle_idle;

	// Sounds
	ref_sound						snd_Ambient;

	// Utilities
	void							p_create		();
	void							p_destroy		();

	void							p_remove		(Particle* P, Particle* &LST);
	void							p_insert		(Particle* P, Particle* &LST);
	int								p_size			(Particle* LST);
	Particle*						p_allocate		();
	void							p_free			(Particle* P);

	// Some methods
	void							Born			(Item& dest, float radius);
	void							Hit				(Fvector& pos);
	BOOL							RayPick			(const Fvector& s, const Fvector& d, float& range, collide::rq_target tgt);
	void							RenewItem		(Item& dest, float height, BOOL bHit);
public:
									CEffect_Rain	();
									~CEffect_Rain	();

	void							Render			();
	void							OnFrame			();
	void							phase_rmap		();
	void							set_state		(States _state) {	state = _state;	};
};
#ifndef _EDITOR
class rain_timer_params
{
public:
	Fvector		timer;
	bool		not_first_frame;
	float		rain_timestamp;
	float		rain_drop_time_basic;
	float		previous_frame_time;
	float		last_rain_duration;
	float		rain_drop_time;
	float		rain_timer;

	rain_timer_params() :not_first_frame(FALSE), rain_timestamp(0), rain_drop_time_basic(20.0), previous_frame_time(0), last_rain_duration(0), rain_drop_time(0), rain_timer(0) { timer.set(0.0, 0.0, 0.0); };
	IC void SetDropTime(float time)
	{
		rain_drop_time_basic = time;
	}
	int Update(BOOL state, bool need_raypick);
	BOOL RayPick(const Fvector& s, const Fvector& d, float& range, collide::rq_target tgt);
};

ENGINE_API extern rain_timer_params *rain_timers_raycheck;
ENGINE_API extern rain_timer_params *rain_timers;
ENGINE_API extern Fvector4 *rain_params;
#endif
#endif //RainH
