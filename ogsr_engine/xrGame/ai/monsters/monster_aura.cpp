#include "stdafx.h"
#include "monster_aura.h"
#include "basemonster/base_monster.h"
#include "../../Actor.h"
#include "../../ActorEffector.h"
//#include "../../HudSound.h"

namespace detail
{
	static pcstr const	s_pp_effector_name_string	=	"_pp_effector_name";
	static pcstr const	s_pp_highest_at_string		=	"_pp_highest_at";
	static pcstr const	s_linear_factor_string		=	"_linear_factor";
	static pcstr const	s_quadratic_factor_string	=	"_quadratic_factor";
	static pcstr const	s_max_power_string			=	"_max_power";
	static pcstr const	s_max_distance_string		=	"_max_distance";
	static pcstr const	s_sound_string				=	"_sound";
	static pcstr const	s_detect_sound_string		=	"_detect_sound";
	static pcstr const	s_enable_for_dead_string	=	"_enable_for_dead";

} // namespace detail

monster_aura::monster_aura (CBaseMonster* const object, pcstr const name) 
						: m_object(object), m_pp_effector_name(NULL), m_pp_index(0)
{
	xr_strcpy							(m_name, sizeof(m_name), name); 
	m_detect_snd_time	=	0.0f;
	m_enabled			=	false;
	m_enable_for_dead	=	false;
}

monster_aura::~monster_aura ()
{
	remove_pp_effector();
}

float   monster_aura::calculate () const
{
	float const distance			=	m_object->Position().distance_to(Actor()->Position());

	float const epsilon				=	0.0001f;

	using namespace						detail;
	float linear_factor				=	m_linear_factor;
	float quadratic_factor			=	m_quadratic_factor;
	float max_power					=	m_max_power;
	float max_distance				=	m_max_distance;

	if ( distance > max_distance )
	{
		return							0;
	}

	if ( distance < epsilon )
	{
		return							(linear_factor > epsilon) || (quadratic_factor > epsilon) 
											? 
										 max_power : 0;
	}

	float const power				=	linear_factor/distance + quadratic_factor/distance*distance;
	
	return								(power < max_power) ? power : max_power;
}

void   monster_aura::load_from_ini (CInifile* ini, pcstr const section, bool enable_for_dead_default)
{
	using namespace						detail;
	string512			tempBuffer;
	xr_strconcat		(tempBuffer, m_name, s_pp_effector_name_string);
	m_pp_effector_name = READ_IF_EXISTS(ini, r_string, section, tempBuffer, NULL);
	
	xr_strconcat		(tempBuffer, m_name, s_pp_highest_at_string);
	m_pp_highest_at = READ_IF_EXISTS(ini, r_float, section, tempBuffer, 1.f);

	xr_strconcat		(tempBuffer, m_name, s_linear_factor_string);
	m_linear_factor = READ_IF_EXISTS(ini, r_float, section, tempBuffer, 0.f);

	xr_strconcat		(tempBuffer, m_name, s_quadratic_factor_string);
	m_quadratic_factor = READ_IF_EXISTS(ini, r_float, section, tempBuffer, 0.f);

	xr_strconcat		(tempBuffer, m_name, s_max_power_string);
	m_max_power = READ_IF_EXISTS(ini, r_float, section, tempBuffer, 0.f);

	xr_strconcat		(tempBuffer, m_name, s_max_distance_string);
	m_max_distance = READ_IF_EXISTS(ini, r_float, section, tempBuffer, 0.f);

	xr_strconcat		(tempBuffer, m_name, s_sound_string);
	LPCSTR sound_name = READ_IF_EXISTS(ini, r_string, section, tempBuffer, NULL);
	
	xr_strconcat		(tempBuffer, m_name, s_detect_sound_string);
	LPCSTR detect_sound_name = READ_IF_EXISTS(ini, r_string, section, tempBuffer, NULL);

	xr_strconcat		(tempBuffer, m_name, s_enable_for_dead_string);

	m_enable_for_dead	= !!READ_IF_EXISTS(ini, r_bool, section, tempBuffer, enable_for_dead_default);

	if(sound_name)
		m_sound.create					(sound_name, st_Effect, sg_SourceType);

	if(detect_sound_name)
		m_detect_sound.create			(detect_sound_name, st_Effect, sg_SourceType);

	if(m_pp_effector_name || m_max_power || m_max_distance || sound_name || detect_sound_name)
		m_enabled = true;
}

bool   monster_aura::check_work_condition () const
{
	if ( !m_enable_for_dead && !m_object->g_Alive() )
		return							false;
	
	return								m_enabled			&&
										Actor()				&&
										Actor()->g_Alive();
}

void   monster_aura::remove_pp_effector ()
{
	if ( m_pp_index != 0 && Actor() )
	{
		RemoveEffector					(Actor(), m_pp_index);
		m_pp_index					=	0;

		m_sound.stop					();
		m_detect_sound.stop				();
	}
}

float   monster_aura::get_post_process_factor () const
{
	using	namespace					detail;
	VERIFY								(m_pp_highest_at != 0.f);

	float 	factor					=	calculate() / m_pp_highest_at;
	clamp								(factor, 0.f, 1.f);
	return								factor;
}

void   monster_aura::play_detector_sound()
{
	if(!check_work_condition())
		return;

	float distance = m_object->Position().distance_to(Actor()->Position());
	if(distance<m_max_distance)
	{
		float power = get_post_process_factor();
		float freq = (distance/m_max_distance)*(m_max_power/(power?power:1.0f));
		if(distance>0.65f*m_max_distance)
			freq *= 0.5f;
		float cur_period = 0.1f + (2.0f - 0.1f) * freq ;

		if(m_detect_snd_time > cur_period)
		{
			m_detect_sound.play_at_pos		(Actor(), Fvector().set(0.f, 1.f, 0.f), sm_2D);
			m_detect_snd_time = 0.0f;
		} 
		else
			m_detect_snd_time += Device.fTimeDelta;
	}
}

void   monster_aura::on_monster_death ()
{
	m_sound.stop();
	m_detect_sound.stop();
}

void   monster_aura::update_schedule ()
{
	if ( !check_work_condition() )
	{
		remove_pp_effector();
		return;
	}

	float const pp_factor			=	get_post_process_factor();

	if ( !m_sound._feedback() )
	{
		m_sound.play_at_pos				(Actor(), Fvector().set(0.f, 1.f, 0.f), sm_Looped | sm_2D);
	} 

	if ( m_sound._feedback() )
	{
		m_sound.set_volume				(pp_factor);
	}

	if ( !m_pp_effector_name )
	{
		return;
	}

	if ( pp_factor > 0.01f )
	{
		if ( !m_pp_index )
		{
			m_pp_index				=	Actor()->Cameras().RequestPPEffectorId();
			AddEffector					(Actor(), m_pp_index, m_pp_effector_name, 
				fastdelegate::MakeDelegate(this, &monster_aura::get_post_process_factor));
		}
	}
	else if ( m_pp_index != 0 )
	{
		RemoveEffector					(Actor(), m_pp_index);
		m_pp_index					=	0;
	}
}
