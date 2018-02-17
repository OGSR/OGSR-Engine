#include "stdafx.h"
#pragma hdrstop

#include "xrLevel.h"
#include "soundrender_core.h"
#include "soundrender_source.h"
#include "soundrender_emitter.h"
#include "soundrender_target.h"
#pragma warning(push)
#pragma warning(disable:4995)
#include <eax.h>
#pragma warning(pop)

int	psSoundTargets = 512; //--#SM+#--
Flags32	psSoundFlags			= {ss_Hardware | ss_EAX};
float	psSoundOcclusionScale	= 0.5f;
float	psSoundCull				= 0.01f;
float	psSoundRolloff			= 0.75f;
u32		psSoundFreq				= sf_44K;
u32		psSoundModel			= 0;
float	psSoundVEffects			= 1.0f;
float	psSoundVFactor			= 1.0f;

float	psSoundVMusic			= 0.7f;
int		psSoundCacheSizeMB		= 32;

CSoundRender_Core*				SoundRender = 0;
CSound_manager_interface*		Sound		= 0;


std::unordered_map <std::string, EFXEAXREVERBPROPERTIES> efx_reverb_presets = {
  { "generic", EFX_REVERB_PRESET_GENERIC },
  { "paddedcell", EFX_REVERB_PRESET_PADDEDCELL },
  { "room", EFX_REVERB_PRESET_ROOM },
  { "bathroom", EFX_REVERB_PRESET_BATHROOM },
  { "livingroom", EFX_REVERB_PRESET_LIVINGROOM },
  { "stoneroom", EFX_REVERB_PRESET_STONEROOM },
  { "auditorium", EFX_REVERB_PRESET_AUDITORIUM },
  { "concerthall", EFX_REVERB_PRESET_CONCERTHALL },
  { "cave", EFX_REVERB_PRESET_CAVE },
  { "arena", EFX_REVERB_PRESET_ARENA },
  { "hangar", EFX_REVERB_PRESET_HANGAR },
  { "carpetedhallway", EFX_REVERB_PRESET_CARPETEDHALLWAY },
  { "hallway", EFX_REVERB_PRESET_HALLWAY },
  { "stonecorridor", EFX_REVERB_PRESET_STONECORRIDOR },
  { "alley", EFX_REVERB_PRESET_ALLEY },
  { "forest", EFX_REVERB_PRESET_FOREST },
  { "city", EFX_REVERB_PRESET_CITY },
  { "mountains", EFX_REVERB_PRESET_MOUNTAINS },
  { "quarry", EFX_REVERB_PRESET_QUARRY },
  { "plain", EFX_REVERB_PRESET_PLAIN },
  { "parkinglot", EFX_REVERB_PRESET_PARKINGLOT },
  { "sewerpipe", EFX_REVERB_PRESET_SEWERPIPE },
  { "underwater", EFX_REVERB_PRESET_UNDERWATER },
  { "drugged", EFX_REVERB_PRESET_DRUGGED },
  { "dizzy", EFX_REVERB_PRESET_DIZZY },
  { "psychotic", EFX_REVERB_PRESET_PSYCHOTIC },

  { "castle_smallroom", EFX_REVERB_PRESET_CASTLE_SMALLROOM },
  { "castle_shortpassage", EFX_REVERB_PRESET_CASTLE_SHORTPASSAGE },
  { "castle_mediumroom", EFX_REVERB_PRESET_CASTLE_MEDIUMROOM },
  { "castle_largeroom", EFX_REVERB_PRESET_CASTLE_LARGEROOM },
  { "castle_longpassage", EFX_REVERB_PRESET_CASTLE_LONGPASSAGE },
  { "castle_hall", EFX_REVERB_PRESET_CASTLE_HALL },
  { "castle_cupboard", EFX_REVERB_PRESET_CASTLE_CUPBOARD },
  { "castle_courtyard", EFX_REVERB_PRESET_CASTLE_COURTYARD },
  { "castle_alcove", EFX_REVERB_PRESET_CASTLE_ALCOVE },

  { "factory_smallroom", EFX_REVERB_PRESET_FACTORY_SMALLROOM },
  { "factory_shortpassage", EFX_REVERB_PRESET_FACTORY_SHORTPASSAGE },
  { "factory_mediumroom", EFX_REVERB_PRESET_FACTORY_MEDIUMROOM },
  { "factory_largeroom", EFX_REVERB_PRESET_FACTORY_LARGEROOM },
  { "factory_longpassage", EFX_REVERB_PRESET_FACTORY_LONGPASSAGE },
  { "factory_hall", EFX_REVERB_PRESET_FACTORY_HALL },
  { "factory_cupboard", EFX_REVERB_PRESET_FACTORY_CUPBOARD },
  { "factory_courtyard", EFX_REVERB_PRESET_FACTORY_COURTYARD },
  { "factory_alcove", EFX_REVERB_PRESET_FACTORY_ALCOVE },

  { "icepalace_smallroom", EFX_REVERB_PRESET_ICEPALACE_SMALLROOM },
  { "icepalace_shortpassage", EFX_REVERB_PRESET_ICEPALACE_SHORTPASSAGE },
  { "icepalace_mediumroom", EFX_REVERB_PRESET_ICEPALACE_MEDIUMROOM },
  { "icepalace_largeroom", EFX_REVERB_PRESET_ICEPALACE_LARGEROOM },
  { "icepalace_longpassage", EFX_REVERB_PRESET_ICEPALACE_LONGPASSAGE },
  { "icepalace_hall", EFX_REVERB_PRESET_ICEPALACE_HALL },
  { "icepalace_cupboard", EFX_REVERB_PRESET_ICEPALACE_CUPBOARD },
  { "icepalace_courtyard", EFX_REVERB_PRESET_ICEPALACE_COURTYARD },
  { "icepalace_alcove", EFX_REVERB_PRESET_ICEPALACE_ALCOVE },

  { "spacestation_smallroom", EFX_REVERB_PRESET_SPACESTATION_SMALLROOM },
  { "spacestation_shortpassage", EFX_REVERB_PRESET_SPACESTATION_SHORTPASSAGE },
  { "spacestation_mediumroom", EFX_REVERB_PRESET_SPACESTATION_MEDIUMROOM },
  { "spacestation_largeroom", EFX_REVERB_PRESET_SPACESTATION_LARGEROOM },
  { "spacestation_longpassage", EFX_REVERB_PRESET_SPACESTATION_LONGPASSAGE },
  { "spacestation_hall", EFX_REVERB_PRESET_SPACESTATION_HALL },
  { "spacestation_cupboard", EFX_REVERB_PRESET_SPACESTATION_CUPBOARD },
  { "spacestation_alcove", EFX_REVERB_PRESET_SPACESTATION_ALCOVE },

  { "wooden_smallroom", EFX_REVERB_PRESET_WOODEN_SMALLROOM },
  { "wooden_shortpassage", EFX_REVERB_PRESET_WOODEN_SHORTPASSAGE },
  { "wooden_mediumroom", EFX_REVERB_PRESET_WOODEN_MEDIUMROOM },
  { "wooden_largeroom", EFX_REVERB_PRESET_WOODEN_LARGEROOM },
  { "wooden_longpassage", EFX_REVERB_PRESET_WOODEN_LONGPASSAGE },
  { "wooden_hall", EFX_REVERB_PRESET_WOODEN_HALL },
  { "wooden_cupboard", EFX_REVERB_PRESET_WOODEN_CUPBOARD },
  { "wooden_courtyard", EFX_REVERB_PRESET_WOODEN_COURTYARD },
  { "wooden_alcove", EFX_REVERB_PRESET_WOODEN_ALCOVE },

  { "sport_emptystadium", EFX_REVERB_PRESET_SPORT_EMPTYSTADIUM },
  { "sport_squashcourt", EFX_REVERB_PRESET_SPORT_SQUASHCOURT },
  { "sport_smallswimmingpool", EFX_REVERB_PRESET_SPORT_SMALLSWIMMINGPOOL },
  { "sport_largeswimmingpool", EFX_REVERB_PRESET_SPORT_LARGESWIMMINGPOOL },
  { "sport_gymnasium", EFX_REVERB_PRESET_SPORT_GYMNASIUM },
  { "sport_fullstadium", EFX_REVERB_PRESET_SPORT_FULLSTADIUM },
  { "sport_stadiumtannoy", EFX_REVERB_PRESET_SPORT_STADIUMTANNOY },

  { "prefab_workshop", EFX_REVERB_PRESET_PREFAB_WORKSHOP },
  { "prefab_schoolroom", EFX_REVERB_PRESET_PREFAB_SCHOOLROOM },
  { "prefab_practiseroom", EFX_REVERB_PRESET_PREFAB_PRACTISEROOM },
  { "prefab_outhouse", EFX_REVERB_PRESET_PREFAB_OUTHOUSE },
  { "prefab_caravan", EFX_REVERB_PRESET_PREFAB_CARAVAN },

  { "dome_tomb", EFX_REVERB_PRESET_DOME_TOMB },
  { "pipe_small", EFX_REVERB_PRESET_PIPE_SMALL },
  { "dome_saintpauls", EFX_REVERB_PRESET_DOME_SAINTPAULS },
  { "pipe_longthin", EFX_REVERB_PRESET_PIPE_LONGTHIN },
  { "pipe_large", EFX_REVERB_PRESET_PIPE_LARGE },
  { "pipe_resonant", EFX_REVERB_PRESET_PIPE_RESONANT },

  { "outdoors_backyard", EFX_REVERB_PRESET_OUTDOORS_BACKYARD },
  { "outdoors_rollingplains", EFX_REVERB_PRESET_OUTDOORS_ROLLINGPLAINS },
  { "outdoors_deepcanyon", EFX_REVERB_PRESET_OUTDOORS_DEEPCANYON },
  { "outdoors_creek", EFX_REVERB_PRESET_OUTDOORS_CREEK },
  { "outdoors_valley", EFX_REVERB_PRESET_OUTDOORS_VALLEY },

  { "mood_heaven", EFX_REVERB_PRESET_MOOD_HEAVEN },
  { "mood_hell", EFX_REVERB_PRESET_MOOD_HELL },
  { "mood_memory", EFX_REVERB_PRESET_MOOD_MEMORY },

  { "driving_commentator", EFX_REVERB_PRESET_DRIVING_COMMENTATOR },
  { "driving_pitgarage", EFX_REVERB_PRESET_DRIVING_PITGARAGE },
  { "driving_incar_racer", EFX_REVERB_PRESET_DRIVING_INCAR_RACER },
  { "driving_incar_sports", EFX_REVERB_PRESET_DRIVING_INCAR_SPORTS },
  { "driving_incar_luxury", EFX_REVERB_PRESET_DRIVING_INCAR_LUXURY },
  { "driving_fullgrandstand", EFX_REVERB_PRESET_DRIVING_FULLGRANDSTAND },
  { "driving_emptygrandstand", EFX_REVERB_PRESET_DRIVING_EMPTYGRANDSTAND },
  { "driving_tunnel", EFX_REVERB_PRESET_DRIVING_TUNNEL },

  { "city_streets", EFX_REVERB_PRESET_CITY_STREETS },
  { "city_subway", EFX_REVERB_PRESET_CITY_SUBWAY },
  { "city_museum", EFX_REVERB_PRESET_CITY_MUSEUM },
  { "city_library", EFX_REVERB_PRESET_CITY_LIBRARY },
  { "city_underpass", EFX_REVERB_PRESET_CITY_UNDERPASS },
  { "city_abandoned", EFX_REVERB_PRESET_CITY_ABANDONED },
  
  { "dustyroom", EFX_REVERB_PRESET_DUSTYROOM },
  { "chapel", EFX_REVERB_PRESET_CHAPEL },
  { "smallwaterroom", EFX_REVERB_PRESET_SMALLWATERROOM },
};


//////////////////////////////////////////////////
#include <efx.h>
#define LOAD_PROC(x, type)  ((x) = (type)alGetProcAddress(#x))
static LPALEFFECTF alEffectf;
static LPALEFFECTI alEffecti;
static LPALDELETEEFFECTS alDeleteEffects;
static LPALISEFFECT alIsEffect;
static LPALGENEFFECTS alGenEffects;
LPALDELETEAUXILIARYEFFECTSLOTS alDeleteAuxiliaryEffectSlots; 
LPALGENAUXILIARYEFFECTSLOTS alGenAuxiliaryEffectSlots;
LPALAUXILIARYEFFECTSLOTI alAuxiliaryEffectSloti;
LPALGETAUXILIARYEFFECTSLOTI alGetAuxiliaryEffectSloti;


CSoundRender_Core::CSoundRender_Core	()
{
	bEFX = false;
	bPresent					= FALSE;
    bEAX						= FALSE;
    bDeferredEAX				= FALSE;
	bUserEnvironment			= FALSE;
	geom_MODEL					= NULL;
	geom_ENV					= nullptr;
	geom_SOM					= nullptr;
	s_environment				= NULL;
	Handler						= NULL;
	s_targets_pu				= 0;
	s_emitters_u				= 0;
    e_current.set_identity		();
    e_target.set_identity		();
    bListenerMoved				= FALSE;
    bReady						= FALSE;
    bLocked						= FALSE;
	Timer_Value					= Timer.GetElapsed_ms();
	Timer_Delta					= 0;
	m_iPauseCounter				= 1;

	effect = 0;
	slot = 0;
	efx_reverb = nullptr;
        efx_def_env_slot = AL_EFFECTSLOT_NULL;
}


CSoundRender_Core::~CSoundRender_Core() {
  if ( bEFX ) {
    if( effect ) alDeleteEffects( 1, &effect );
    if( slot ) alDeleteAuxiliaryEffectSlots( 1, &slot );
    for ( auto s : efx_slots ) {
      ALint eff;
      alGetAuxiliaryEffectSloti( s, AL_EFFECTSLOT_EFFECT, &eff );
      alDeleteEffects( 1, &(ALuint)eff );
      alDeleteAuxiliaryEffectSlots( 1, &s );
    }
  }

  if ( geom_ENV ) xr_delete( geom_ENV );
  if ( geom_SOM ) xr_delete( geom_SOM );
}


void CSoundRender_Core::_initialize	(u64 window)
{
	Timer.Start					( );

    // load environment
	env_load					();
	if ( bEFX ) efx_configure_env_slots();

	bPresent					= TRUE;

	// Cache
	cache_bytes_per_line		= (sdef_target_block/8)*wfm.nAvgBytesPerSec/1000;
    cache.initialize			(psSoundCacheSizeMB*1024,cache_bytes_per_line);

    bReady						= TRUE;
}

extern xr_vector<u8> g_target_temp_data;
void CSoundRender_Core::_clear	()
{
    bReady						= FALSE;
	cache.destroy				();
	env_unload					();

    // remove sources
	for (u32 sit=0; sit<s_sources.size(); sit++)
    	xr_delete				(s_sources[sit]);
    s_sources.clear				();
    
    // remove emmiters
	for (u32 eit=0; eit<s_emitters.size(); eit++)
    	xr_delete				(s_emitters[eit]);
    s_emitters.clear			();

	g_target_temp_data.clear	();
}

void CSoundRender_Core::stop_emitters()
{
	for (u32 eit=0; eit<s_emitters.size(); eit++)
		s_emitters[eit]->stop	(FALSE);
}

int CSoundRender_Core::pause_emitters(bool val)
{
	m_iPauseCounter				+= val?+1:-1;
	VERIFY						(m_iPauseCounter>=0);

	for (u32 it=0; it<s_emitters.size(); it++)
		((CSoundRender_Emitter*)s_emitters[it])->pause	(val,val?m_iPauseCounter:m_iPauseCounter+1);

	return m_iPauseCounter;
}

void CSoundRender_Core::env_load	()
{
	// Load environment
	string_path					fn;
	if (FS.exist(fn,"$game_data$",SNDENV_FILENAME))
	{
		s_environment				= xr_new<SoundEnvironment_LIB>();
		s_environment->Load			(fn);
	}

	// Load geometry

	// Assosiate geometry
}

void CSoundRender_Core::env_unload	()
{
	// Unload 
	if (s_environment)
		s_environment->Unload	();
	xr_delete					(s_environment);

	// Unload geometry
}

void CSoundRender_Core::_restart		()
{
	cache.destroy				();
	cache.initialize			(psSoundCacheSizeMB*1024,cache_bytes_per_line);
	env_apply					();
}

void CSoundRender_Core::set_handler(sound_event* E)
{
	Handler			= E;
}

void CSoundRender_Core::set_geometry_occ(CDB::MODEL* M)
{
	geom_MODEL		= M;
}

void CSoundRender_Core::set_geometry_som(IReader* I)
{
	if ( geom_SOM ) xr_delete( geom_SOM );

	if (!I)
		return;

	// check version
	R_ASSERT		(I->find_chunk(0));
	u32 version		= I->r_u32(); 
	VERIFY2			(version==0,"Invalid SOM version");
	// load geometry	
	IReader* geom	= I->open_chunk(1); 
	VERIFY2			(geom,"Corrupted SOM file");
	// Load tris and merge them
	struct SOM_poly{
		Fvector3	v1;
		Fvector3	v2;
		Fvector3	v3;
		u32			b2sided;
		float		occ;
	};
	// Create AABB-tree
	CDB::Collector				CL;			
	while (!geom->eof()){
		SOM_poly				P;
		geom->r					(&P,sizeof(P));
		CL.add_face_packed_D	(P.v1,P.v2,P.v3,*(size_t*)&P.occ,0.01f);
		if (P.b2sided)
			CL.add_face_packed_D(P.v3,P.v2,P.v1,*(size_t*)&P.occ,0.01f);
	}
	geom_SOM			= xr_new<CDB::MODEL> ();
	geom_SOM->build(CL.getV(),int(CL.getVS()),CL.getT(),int(CL.getTS()), nullptr, nullptr, false);
}

void CSoundRender_Core::set_geometry_env(IReader* I)
{
	if ( geom_ENV ) xr_delete( geom_ENV );

	if (!I || !s_environment)
		return;

	// Assosiate names
	xr_vector<u16>			ids;
	IReader*				names	= I->open_chunk(0);
	while (!names->eof())
	{
		string256			n;
		names->r_stringZ	(n,sizeof(n));
		int id				= s_environment->GetID(n);
		R_ASSERT			(id>=0);
		ids.push_back		(u16(id));
	}
	names->close		();

	// Load geometry
	IReader*			geom_ch	= I->open_chunk(1);
	
	u8*	_data			= (u8*)xr_malloc(geom_ch->length());
	
	std::memcpy(_data, geom_ch->pointer(), geom_ch->length());

	IReader* geom		= xr_new<IReader>(_data, geom_ch->length(), 0);
	
	hdrCFORM realCform;
	geom->r(&realCform, sizeof(hdrCFORM));
	R_ASSERT(realCform.version == CFORM_CURRENT_VERSION);
	auto verts = (Fvector*)geom->pointer();
	auto tris = (CDB::TRI*)(verts + realCform.vertcount);
#ifndef _M_X64 //KRodin: а к чему вообще этот битодроч здесь? Лично мне совершенно не понятно, что он делает.
	for (u32 it=0; it<realCform.facecount; it++)
	{
		CDB::TRI*	T	= tris+it;
		u16		id_front= (u16)((T->dummy&0x0000ffff)>>0);		//	front face
		u16		id_back	= (u16)((T->dummy&0xffff0000)>>16);		//	back face
		R_ASSERT		(id_front<(u16)ids.size());
		R_ASSERT		(id_back<(u16)ids.size());
		T->dummy		= u32(ids[id_back]<<16) | u32(ids[id_front]);
	}
#endif
	geom_ENV = xr_new<CDB::MODEL>();
	geom_ENV->build(verts, realCform.vertcount, tris, realCform.facecount);

	geom_ch->close			();
	geom->close				();
	xr_free					(_data);
}

void	CSoundRender_Core::verify_refsound		( ref_sound& S)
{
/*
#ifdef	DEBUG
	int			local_value		= 0;
	void*		ptr_refsound	= &S;
	void*		ptr_local		= &local_value;
	ptrdiff_t	difference		= (ptrdiff_t)_abs(s64(ptrdiff_t(ptr_local) - ptrdiff_t(ptr_refsound)));
	string256	err_str;
	if(difference < (4*1024))
	{
		sprintf		(err_str,"diff=[%d] local/stack-based ref_sound passed. memory corruption will accur.",difference);
		VERIFY2		(0, err_str);
	}
#endif
*/
}

void	CSoundRender_Core::create				( ref_sound& S, const char* fName, esound_type sound_type, int game_type )
{
	if (!bPresent)		return;
	verify_refsound		(S);
    S._p				= xr_new<ref_sound_data>(fName,sound_type,game_type);
}

void	CSoundRender_Core::clone				( ref_sound& S, const ref_sound& from, esound_type sound_type, int	game_type )
{
	if (!bPresent)		return;
	S._p				= xr_new<ref_sound_data>();
	S._p->handle		= from._p->handle;
	S._p->g_type		= (game_type==sg_SourceType)?S._p->handle->game_type():game_type;
	S._p->s_type		= sound_type;
}


void	CSoundRender_Core::play					( ref_sound& S, CObject* O, u32 flags, float delay)
{
	if (!bPresent || 0==S._handle())return;
	verify_refsound		(S);
	S._p->g_object		= O;
	if (S._feedback())	((CSoundRender_Emitter*)S._feedback())->rewind ();
	else				i_play					(&S,flags&sm_Looped,delay);
	if (flags&sm_2D)	S._feedback()->switch_to_2D();
}
void	CSoundRender_Core::play_no_feedback		( ref_sound& S, CObject* O, u32 flags, float delay, Fvector* pos, float* vol, float* freq, Fvector2* range)
{
	if (!bPresent || 0==S._handle())return;
	verify_refsound		(S);
	ref_sound_data_ptr	orig = S._p;
	S._p				= xr_new<ref_sound_data>();
	S._p->handle		= orig->handle;
	S._p->g_type		= orig->g_type;
	S._p->g_object		= O;
	i_play				(&S,flags&sm_Looped,delay);
	if (flags&sm_2D)	S._feedback()->switch_to_2D();
	if (pos)			S._feedback()->set_position	(*pos);
	if (freq)			S._feedback()->set_frequency(*freq);
	if (range)			S._feedback()->set_range   	((*range)[0],(*range)[1]);
	if (vol)			S._feedback()->set_volume   (*vol);
	S._p				= orig;
}
void	CSoundRender_Core::play_at_pos			( ref_sound& S, CObject* O, const Fvector &pos, u32 flags, float delay)
{
	if (!bPresent || 0==S._handle())return;
	verify_refsound		(S);
	S._p->g_object		= O;
	if (S._feedback())	((CSoundRender_Emitter*)S._feedback())->rewind ();
	else				i_play					(&S,flags&sm_Looped,delay);
	S._feedback()->set_position					(pos);
	if (flags&sm_2D)	S._feedback()->switch_to_2D();
}
void	CSoundRender_Core::destroy	(ref_sound& S )
{
	if (S._feedback()){                   
		CSoundRender_Emitter* E		= (CSoundRender_Emitter*)S._feedback();
		E->stop						(FALSE);
	}
	S._p				= 0;
}                                                    

void CSoundRender_Core::_create_data( ref_sound_data& S, LPCSTR fName, esound_type sound_type, int game_type)
{
	string_path			fn;
	strcpy				(fn,fName);
    if (strext(fn))		*strext(fn)	= 0;
	S.handle			= (CSound_source*)SoundRender->i_create_source(fn);
	S.g_type			= (game_type==sg_SourceType)?S.handle->game_type():game_type;
	S.s_type			= sound_type;
	S.feedback			= 0; 
    S.g_object			= 0; 
    S.g_userdata		= 0;
}
void CSoundRender_Core::_destroy_data( ref_sound_data& S)
{
	if (S.feedback){                   
		CSoundRender_Emitter* E		= (CSoundRender_Emitter*)S.feedback;
		E->stop						(FALSE);
	}
	R_ASSERT						(0==S.feedback);
	SoundRender->i_destroy_source	((CSoundRender_Source*)S.handle);
	S.handle						= NULL;
}

CSoundRender_Environment*	CSoundRender_Core::get_environment			( const Fvector& P )
{
	static CSoundRender_Environment	identity;

	if (bUserEnvironment){
		return &s_user_environment;
	}else{
		if (geom_ENV){
			Fvector	dir				= {0,-1,0};

			geom_DB.ray_options		(CDB::OPT_ONLYNEAREST);
			geom_DB.ray_query		(geom_ENV,P,dir,1000.f);
			if (geom_DB.r_count()){
				CDB::RESULT*		r	= geom_DB.r_begin();

				CDB::TRI*			T	= geom_ENV->get_tris()+r->id;
				Fvector*			V	= geom_ENV->get_verts();
				Fvector tri_norm;
				tri_norm.mknormal		(V[T->verts[0]],V[T->verts[1]],V[T->verts[2]]);
				float	dot				= dir.dotproduct(tri_norm);
				if (dot<0){
					u16		id_front	= (u16)((T->dummy&0x0000ffff)>>0);		//	front face
					return	s_environment->Get(id_front);
				}else{
					u16		id_back		= (u16)((T->dummy&0xffff0000)>>16);	//	back face
					return	s_environment->Get(id_back);
				}
			}else{
				identity.set_identity	();
				return &identity;
			}
		}else{
			identity.set_identity	();
			return &identity;
		}
	}
}

void						CSoundRender_Core::env_apply		()
{
/*
	// Force all sounds to change their environment
	// (set their positions to signal changes in environment)
	for (u32 it=0; it<s_emitters.size(); it++)
	{
		CSoundRender_Emitter*	pEmitter	= s_emitters[it];
		const CSound_params*	pParams		= pEmitter->get_params	();
		pEmitter->set_position	(pParams->position);
	}
*/
    bListenerMoved			= TRUE;
}

void CSoundRender_Core::update_listener( const Fvector& P, const Fvector& D, const Fvector& N, float dt )
{
}


void CSoundRender_Core::InitAlEFXAPI()
{
	LOAD_PROC(alDeleteAuxiliaryEffectSlots, LPALDELETEAUXILIARYEFFECTSLOTS);
	LOAD_PROC(alGenEffects, LPALGENEFFECTS);
	LOAD_PROC(alDeleteEffects, LPALDELETEEFFECTS);
	LOAD_PROC(alIsEffect, LPALISEFFECT);
	LOAD_PROC(alEffecti, LPALEFFECTI);
	LOAD_PROC(alAuxiliaryEffectSloti, LPALAUXILIARYEFFECTSLOTI);
	LOAD_PROC(alGenAuxiliaryEffectSlots, LPALGENAUXILIARYEFFECTSLOTS);
	LOAD_PROC(alGetAuxiliaryEffectSloti, LPALGETAUXILIARYEFFECTSLOTI);
	LOAD_PROC(alEffectf, LPALEFFECTF);
}

bool CSoundRender_Core::EFXTestSupport() {
  alGenEffects( 1, &effect );

  alEffecti( effect, AL_EFFECT_TYPE, AL_EFFECT_REVERB );
  setEFXPreset( "generic" );
  unsetEFXPreset();

  /* Check if an error occured, and clean up if so. */
  ALenum err = alGetError();
  if ( err != AL_NO_ERROR ) {
    Msg( "OpenAL error: %s", alGetString( err ) );
    if ( alIsEffect( effect ) )
      alDeleteEffects( 1, &effect );
      return false;
  }

  alGenAuxiliaryEffectSlots( 1, &slot );
  err = alGetError();
  ASSERT_FMT( err == AL_NO_ERROR, "[OpenAL] EFX error: %s", alGetString( err ) );

  alAuxiliaryEffectSloti( slot, AL_EFFECTSLOT_EFFECT, effect );
  err = alGetError();
  ASSERT_FMT( err == AL_NO_ERROR, "[OpenAL] EFX error: %s", alGetString( err ) );

  return true;
}

void CSoundRender_Core::setEFXPreset( std::string name ) {
  efx_def_env_slot = AL_EFFECTSLOT_NULL;
  efx_reverb       = &efx_reverb_presets.at( name );
  applyEFXPreset();
}

void CSoundRender_Core::unsetEFXPreset() {
  efx_reverb = nullptr;
}

void CSoundRender_Core::applyEFXPreset() {
  alEffectf( effect, AL_REVERB_DENSITY,               efx_reverb->flDensity );
  alEffectf( effect, AL_REVERB_DIFFUSION,             efx_reverb->flDiffusion );
  alEffectf( effect, AL_REVERB_GAIN,                  efx_reverb->flGain );
  alEffectf( effect, AL_REVERB_GAINHF,                efx_reverb->flGainHF );
  alEffectf( effect, AL_REVERB_DECAY_TIME,            efx_reverb->flDecayTime );
  alEffectf( effect, AL_REVERB_DECAY_HFRATIO,         efx_reverb->flDecayHFRatio );
  alEffectf( effect, AL_REVERB_REFLECTIONS_GAIN,      efx_reverb->flReflectionsGain );
  alEffectf( effect, AL_REVERB_REFLECTIONS_DELAY,     efx_reverb->flReflectionsDelay );
  alEffectf( effect, AL_REVERB_LATE_REVERB_GAIN,      efx_reverb->flLateReverbGain );
  alEffectf( effect, AL_REVERB_LATE_REVERB_DELAY,     efx_reverb->flLateReverbDelay );
  alEffectf( effect, AL_REVERB_AIR_ABSORPTION_GAINHF, efx_reverb->flAirAbsorptionGainHF );
  alEffectf( effect, AL_REVERB_ROOM_ROLLOFF_FACTOR,   efx_reverb->flRoomRolloffFactor );
  alEffecti( effect, AL_REVERB_DECAY_HFLIMIT,         efx_reverb->iDecayHFLimit );
}

void	CSoundRender_Core::i_eax_listener_set	(CSound_environment* _E)
{
	VERIFY(bEAX);
    CSoundRender_Environment* E = static_cast<CSoundRender_Environment*>(_E);
    EAXLISTENERPROPERTIES 		ep;
    ep.lRoom					= iFloor(E->Room)				;	// room effect level at low frequencies
    ep.lRoomHF					= iFloor(E->RoomHF)				;   // room effect high-frequency level re. low frequency level
    ep.flRoomRolloffFactor		= E->RoomRolloffFactor			;   // like DS3D flRolloffFactor but for room effect
    ep.flDecayTime				= E->DecayTime					;   // reverberation decay time at low frequencies
    ep.flDecayHFRatio			= E->DecayHFRatio				;   // high-frequency to low-frequency decay time ratio
    ep.lReflections				= iFloor(E->Reflections)		;   // early reflections level relative to room effect
    ep.flReflectionsDelay		= E->ReflectionsDelay			;   // initial reflection delay time
    ep.lReverb					= iFloor(E->Reverb)	 			;   // late reverberation level relative to room effect
    ep.flReverbDelay			= E->ReverbDelay				;   // late reverberation delay time relative to initial reflection
    ep.dwEnvironment			= EAXLISTENER_DEFAULTENVIRONMENT;  	// sets all listener properties
    ep.flEnvironmentSize		= E->EnvironmentSize			;  	// environment size in meters
    ep.flEnvironmentDiffusion	= E->EnvironmentDiffusion		; 	// environment diffusion
    ep.flAirAbsorptionHF		= E->AirAbsorptionHF			;	// change in level per meter at 5 kHz
    ep.dwFlags					= EAXLISTENER_DEFAULTFLAGS		;	// modifies the behavior of properties

    u32 deferred				= bDeferredEAX?DSPROPERTY_EAXLISTENER_DEFERRED:0;
    
    i_eax_set(&DSPROPSETID_EAX_ListenerProperties, deferred | DSPROPERTY_EAXLISTENER_ROOM, 					&ep.lRoom,					sizeof(LONG));
    i_eax_set(&DSPROPSETID_EAX_ListenerProperties, deferred | DSPROPERTY_EAXLISTENER_ROOMHF, 				&ep.lRoomHF,				sizeof(LONG));
    i_eax_set(&DSPROPSETID_EAX_ListenerProperties, deferred | DSPROPERTY_EAXLISTENER_ROOMROLLOFFFACTOR, 	&ep.flRoomRolloffFactor,	sizeof(float));
    i_eax_set(&DSPROPSETID_EAX_ListenerProperties, deferred | DSPROPERTY_EAXLISTENER_DECAYTIME, 		  	&ep.flDecayTime,			sizeof(float));
    i_eax_set(&DSPROPSETID_EAX_ListenerProperties, deferred | DSPROPERTY_EAXLISTENER_DECAYHFRATIO,			&ep.flDecayHFRatio,			sizeof(float));
    i_eax_set(&DSPROPSETID_EAX_ListenerProperties, deferred | DSPROPERTY_EAXLISTENER_REFLECTIONS, 			&ep.lReflections,			sizeof(LONG));
    i_eax_set(&DSPROPSETID_EAX_ListenerProperties, deferred | DSPROPERTY_EAXLISTENER_REFLECTIONSDELAY,    	&ep.flReflectionsDelay,		sizeof(float));
    i_eax_set(&DSPROPSETID_EAX_ListenerProperties, deferred | DSPROPERTY_EAXLISTENER_REVERB, 				&ep.lReverb,				sizeof(LONG));
    i_eax_set(&DSPROPSETID_EAX_ListenerProperties, deferred | DSPROPERTY_EAXLISTENER_REVERBDELAY, 			&ep.flReverbDelay,			sizeof(float));
    i_eax_set(&DSPROPSETID_EAX_ListenerProperties, deferred | DSPROPERTY_EAXLISTENER_ENVIRONMENTDIFFUSION,	&ep.flEnvironmentDiffusion,	sizeof(float));
    i_eax_set(&DSPROPSETID_EAX_ListenerProperties, deferred | DSPROPERTY_EAXLISTENER_AIRABSORPTIONHF, 		&ep.flAirAbsorptionHF,		sizeof(float));
    i_eax_set(&DSPROPSETID_EAX_ListenerProperties, deferred | DSPROPERTY_EAXLISTENER_FLAGS, 				&ep.dwFlags,				sizeof(DWORD));
}

void	CSoundRender_Core::i_eax_listener_get	(CSound_environment* _E)
{
	VERIFY(bEAX);
    CSoundRender_Environment* E = static_cast<CSoundRender_Environment*>(_E);
    EAXLISTENERPROPERTIES 		ep;
	i_eax_get					(&DSPROPSETID_EAX_ListenerProperties, DSPROPERTY_EAXLISTENER_ALLPARAMETERS, &ep, sizeof(EAXLISTENERPROPERTIES));
    E->Room						= (float)ep.lRoom					;
    E->RoomHF					= (float)ep.lRoomHF					;
    E->RoomRolloffFactor		= (float)ep.flRoomRolloffFactor		;
    E->DecayTime			   	= (float)ep.flDecayTime				;
    E->DecayHFRatio				= (float)ep.flDecayHFRatio			;
    E->Reflections				= (float)ep.lReflections			;
    E->ReflectionsDelay			= (float)ep.flReflectionsDelay		;
    E->Reverb					= (float)ep.lReverb					;
    E->ReverbDelay				= (float)ep.flReverbDelay			;
    E->EnvironmentSize			= (float)ep.flEnvironmentSize		;
    E->EnvironmentDiffusion		= (float)ep.flEnvironmentDiffusion	;
    E->AirAbsorptionHF			= (float)ep.flAirAbsorptionHF		;
}

void CSoundRender_Core::i_eax_commit_setting()
{
	// commit eax 
    if (bDeferredEAX)
    	i_eax_set(&DSPROPSETID_EAX_ListenerProperties, DSPROPERTY_EAXLISTENER_COMMITDEFERREDSETTINGS,NULL,0);
}

void CSoundRender_Core::object_relcase( CObject* obj )
{
	if (obj){
		for (u32 eit=0; eit<s_emitters.size(); eit++){
        	if (s_emitters[eit])
                if (s_emitters[eit]->owner_data)
                 	if (obj==s_emitters[eit]->owner_data->g_object) 
	                    s_emitters[eit]->owner_data->g_object	= 0;     
        }
    }
}


void CSoundRender_Core::unset_geometry_som() {
  if ( geom_SOM ) {
    xr_delete( geom_SOM );
    geom_SOM = nullptr;
  }
}


void CSoundRender_Core::unset_geometry_env() {
  if ( geom_ENV ) {
    xr_delete( geom_ENV );
    geom_ENV = nullptr;
  }
}


ALuint CSoundRender_Core::efx_get_env_slot( const Fvector& P ) {
  if ( geom_ENV ) {
    Fvector dir = { 0, -1, 0 };
    geom_DB.ray_options( CDB::OPT_ONLYNEAREST );
    geom_DB.ray_query( geom_ENV, P, dir, 1000.f );
    if ( geom_DB.r_count() ) {
      CDB::RESULT* r = geom_DB.r_begin();
      CDB::TRI*    T = geom_ENV->get_tris() + r->id;
      Fvector*     V = geom_ENV->get_verts();
      Fvector      tri_norm;
      tri_norm.mknormal( V[ T->verts[ 0 ] ],V[ T->verts[ 1 ] ], V[ T->verts[ 2 ] ] );
      float dot = dir.dotproduct( tri_norm );
      if ( dot < 0 ) {
        u16 id_front = (u16)( ( T->dummy & 0x0000ffff ) >> 0 ); // front face
        return efx_slots.at( id_front );
      }
      else {
        u16 id_back = (u16)( ( T->dummy & 0xffff0000) >> 16 ); // back face
        return efx_slots.at( id_back );
      }
    }
  }

  return efx_def_env_slot;
}


inline float mB_to_gain( float mb ) {
  return powf( 10.0f, mb / 2000.0f );
}


void CSoundRender_Core::efx_configure_env_slots() {
  efx_slots.reserve( s_environment->Library().size() );

  for ( u32 i = 0; i < s_environment->Library().size(); i++ ) {
    ALenum err = alGetError();
    ALuint effect;
    alGenEffects( 1, &effect );
    alEffecti( effect, AL_EFFECT_TYPE, AL_EFFECT_REVERB );
    err = alGetError();
    ASSERT_FMT( err == AL_NO_ERROR, "[OpenAL] EFX error: %s", alGetString( err ) );

    ALuint slot;
    alGenAuxiliaryEffectSlots( 1, &slot );
    err = alGetError();
    ASSERT_FMT( err == AL_NO_ERROR, "[OpenAL] EFX error: %s", alGetString( err ) );

    auto E = s_environment->Get( i );
    // http://openal.org/pipermail/openal/2014-March/000083.html
    float density = powf( E->EnvironmentSize, 3.0f ) / 16.0f;
    if( density > 1.0f ) density = 1.0f;
    alEffectf( effect, AL_REVERB_DENSITY,               density );
    alEffectf( effect, AL_REVERB_DIFFUSION,             E->EnvironmentDiffusion );
    alEffectf( effect, AL_REVERB_GAIN,                  mB_to_gain( E->Room ) );
    alEffectf( effect, AL_REVERB_GAINHF,                mB_to_gain( E->RoomHF ) );
    alEffectf( effect, AL_REVERB_DECAY_TIME,            E->DecayTime );
    alEffectf( effect, AL_REVERB_DECAY_HFRATIO,         E->DecayHFRatio );
    alEffectf( effect, AL_REVERB_REFLECTIONS_GAIN,      mB_to_gain( E->Reflections ) );
    alEffectf( effect, AL_REVERB_REFLECTIONS_DELAY,     E->ReflectionsDelay );
    alEffectf( effect, AL_REVERB_LATE_REVERB_DELAY,     E->ReverbDelay );
    alEffectf( effect, AL_REVERB_LATE_REVERB_GAIN,      mB_to_gain( E->Reverb ) );
    alEffectf( effect, AL_REVERB_AIR_ABSORPTION_GAINHF, mB_to_gain( E->AirAbsorptionHF ) );
    alEffectf( effect, AL_REVERB_ROOM_ROLLOFF_FACTOR,   E->RoomRolloffFactor );
    err = alGetError();
    if ( err != AL_NO_ERROR ) {
      Msg( "[OpenAL] EFX: EAX preset '%s' error: %s", E->name.c_str(), alGetString( err ) );
    }

    alGetError();
    alAuxiliaryEffectSloti( slot, AL_EFFECTSLOT_EFFECT, effect );
    err = alGetError();
    ASSERT_FMT( err == AL_NO_ERROR, "[OpenAL] EFX error: %s", alGetString( err ) );
    efx_slots.push_back( slot );
  }

  Msg( "[OpenAL] EFX: %u environment slots configured", efx_slots.size() );
}


void CSoundRender_Core::efx_assing_env_slot( const Fvector& P, CSoundRender_Target* T ) {
  if ( psSoundFlags.test( ss_EAX ) && bEFX && ( efx_reverb || geom_ENV || efx_def_env_slot != AL_EFFECTSLOT_NULL ) ) {
    auto s = efx_get_env_slot( P );
    if ( s == AL_EFFECTSLOT_NULL )
      if ( efx_reverb ) s = slot;
    T->alAuxInit( s );
  }
}



void CSoundRender_Core::setEFXEAXPreset( std::string name ) {
  int env_id = s_environment->GetID( name.c_str() );
  if ( env_id < 0 ) {
    Msg( "[OpenAL] EFX: EAX preset '%s' not found", name.c_str() );
  }
  else {
    efx_def_env_slot = efx_slots.at( env_id );
    efx_reverb       = nullptr;
  }
}

void CSoundRender_Core::unsetEFXEAXPreset() {
  efx_def_env_slot = AL_EFFECTSLOT_NULL;
}
