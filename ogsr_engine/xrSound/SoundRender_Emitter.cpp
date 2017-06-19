#include "stdafx.h"
#pragma hdrstop

#include "soundrender_emitter.h"
#include "soundrender_core.h"

extern	u32				psSoundModel;
extern	u32				psSoundFreq;
extern	float			psSoundVEffects;

//////////////////////////////////////////////////////////////////////
CSoundRender_Emitter::CSoundRender_Emitter(void)
{

#ifdef DEBUG
	static	u32			incrementalID = 0;
	dbg_ID				= ++incrementalID;
#endif
	target				= NULL;
	source				= NULL;
	owner_data			= NULL;
	smooth_volume		= 1.f;
	occluder_volume		= 1.f;
	fade_volume			= 1.f;
	occluder[0].set		(0,0,0);
	occluder[1].set		(0,0,0);
	occluder[2].set		(0,0,0);
	state				= stStopped;
	position			= 0;
	bMoved				= TRUE;
	b2D					= FALSE;
	bStopping			= FALSE;
	bRewind				= FALSE;
	iPaused				= 0;
	dwTimeStarted		= 0;
	dwTimeToStop		= 0;
	dwTimeToPropagade	= 0;
	marker				= 0xabababab;
	starting_delay		= 0.f;
	priority_scale		= 1.f;
}

CSoundRender_Emitter::~CSoundRender_Emitter(void)
{
	// try to release dependencies, events, for example
	Event_ReleaseOwner	();
}

//////////////////////////////////////////////////////////////////////
void CSoundRender_Emitter::Event_ReleaseOwner()
{
	if	(!(owner_data))			return;

	for (u32 it=0; it<SoundRender->s_events.size(); it++){
		if (owner_data == SoundRender->s_events[it].first){
			SoundRender->s_events.erase(SoundRender->s_events.begin()+it);
			it	--;
		}
	}
}

void CSoundRender_Emitter::Event_Propagade	()
{
	dwTimeToPropagade			+= ::Random.randI	(sdef_event_pulse-30,sdef_event_pulse+30);
	if (!(owner_data))			return;
	if (0==owner_data->g_type)	return;
	if (0==owner_data->g_object)return;
	if (0==SoundRender->Handler)return;

	VERIFY						(_valid(p_source.volume));
	// Calculate range
	float	clip				= p_source.max_ai_distance*p_source.volume;
	float	range				= _min(p_source.max_ai_distance,clip);
	if (range<0.1f)				return;

	// Inform objects
	SoundRender->s_events.push_back	(mk_pair(owner_data,range));
}

void CSoundRender_Emitter::switch_to_2D()
{
 	b2D 						= TRUE;	
	set_priority				(100.f);
}

void CSoundRender_Emitter::switch_to_3D()						
{ 	
	b2D 						= FALSE;											
}

u32	CSoundRender_Emitter::play_time	( )
{ 
	return (state==stPlaying		|| 
			state==stPlayingLooped	|| 
			state==stSimulating		|| 
			state==stSimulatingLooped)?SoundRender->Timer_Value-dwTimeStarted:0; 
}
