#include "stdafx.h"

#include "../xr_3da/xrLevel.h"
#include "soundrender_core.h"
#include "soundrender_source.h"
#include "soundrender_emitter.h"

#pragma warning(push)
#pragma warning(disable:4995)
#include <eax.h>
#pragma warning(pop)

int		psSoundTargets = 256; //512; //--#SM+#-- //32;
Flags32	psSoundFlags = { ss_Hardware | ss_EAX };
float	psSoundOcclusionScale = 0.5f;
float	psSoundCull = 0.01f;
float	psSoundRolloff = 0.75f;
u32		psSoundModel = 0;
float	psSoundVEffects = 1.0f;
float	psSoundVFactor = 1.0f;
float psSoundTimeFactor = 1.0f; //--#SM+#--
float	psSoundVMusic = 1.f;
int		psSoundCacheSizeMB = 32;

CSoundRender_Core*				SoundRender = 0;
CSound_manager_interface*		Sound = 0;

CSoundRender_Core::CSoundRender_Core()
{
	bPresent = FALSE;
	bEAX = FALSE;
	bDeferredEAX = FALSE;
	bUserEnvironment = FALSE;
	geom_MODEL = NULL;
	geom_ENV = NULL;
	geom_SOM = NULL;
	s_environment = NULL;
	Handler = NULL;
	s_targets_pu = 0;
	s_emitters_u = 0;
	e_current.set_identity();
	e_target.set_identity();
	bListenerMoved = FALSE;
	bReady = FALSE;
	bLocked = FALSE;
	fTimer_Value = Timer.GetElapsed_sec();
	fTimer_Delta = 0.0f;
	m_iPauseCounter = 1;
}

CSoundRender_Core::~CSoundRender_Core()
{
	xr_delete(geom_ENV);
	xr_delete(geom_SOM);
}

void CSoundRender_Core::_initialize(int stage)
{
	Log("* sound: EAX 2.0 extension:", bEAX ? "present" : "absent");
	Log("* sound: EAX 2.0 deferred:", bDeferredEAX ? "present" : "absent");
	Timer.Start();

	// load environment
	env_load();

	bPresent = TRUE;

	// Cache
	cache_bytes_per_line = (sdef_target_block / 8) * 276400 / 1000;
	cache.initialize(psSoundCacheSizeMB * 1024, cache_bytes_per_line);

	bReady = TRUE;
}

extern xr_vector<u8> g_target_temp_data;
void CSoundRender_Core::_clear()
{
	bReady = FALSE;
	cache.destroy();
	env_unload();

	// remove sources
	for (u32 sit = 0; sit < s_sources.size(); sit++)
		xr_delete(s_sources[sit]);
	s_sources.clear();

	// remove emmiters
	for (u32 eit = 0; eit < s_emitters.size(); eit++)
		xr_delete(s_emitters[eit]);
	s_emitters.clear();

	g_target_temp_data.clear();
}

void CSoundRender_Core::stop_emitters()
{
	for (u32 eit = 0; eit < s_emitters.size(); eit++)
		s_emitters[eit]->stop(FALSE);
}

int CSoundRender_Core::pause_emitters(bool val)
{
	m_iPauseCounter += val ? +1 : -1;
	VERIFY(m_iPauseCounter >= 0);

	for (u32 it = 0; it < s_emitters.size(); it++)
		((CSoundRender_Emitter*)s_emitters[it])->pause(val, val ? m_iPauseCounter : m_iPauseCounter + 1);

	return m_iPauseCounter;
}

void CSoundRender_Core::env_load()
{
	// Load environment
	string_path					fn;
	if (FS.exist(fn, "$game_data$", SNDENV_FILENAME))
	{
		s_environment = xr_new<SoundEnvironment_LIB>();
		s_environment->Load(fn);
	}

	// Load geometry

	// Assosiate geometry
}

void CSoundRender_Core::env_unload()
{
	// Unload 
	if (s_environment)
		s_environment->Unload();
	xr_delete(s_environment);

	// Unload geometry
}

void CSoundRender_Core::_restart()
{
	cache.destroy();
	cache.initialize(psSoundCacheSizeMB * 1024, cache_bytes_per_line);
	env_apply();
}

void CSoundRender_Core::set_handler(sound_event* E)
{
	Handler = E;
}

void CSoundRender_Core::set_geometry_occ(CDB::MODEL* M)
{
	geom_MODEL = M;
}

void CSoundRender_Core::set_geometry_som(IReader* I)
{
	xr_delete(geom_SOM);
	if (0 == I)		return;

	// check version
	R_ASSERT(I->find_chunk(0));

#ifdef DEBUG
	u32 version =
#endif
	I->r_u32();
	VERIFY2(version == 0, "Invalid SOM version");
	// load geometry	
	IReader* geom = I->open_chunk(1);
	VERIFY2(geom, "Corrupted SOM file");
	// Load tris and merge them
	struct SOM_poly {
		Fvector3	v1;
		Fvector3	v2;
		Fvector3	v3;
		u32			b2sided;
		float		occ;
	};
	// Create AABB-tree
	CDB::Collector				CL;
	while (!geom->eof()) {
		SOM_poly				P;
		geom->r(&P, sizeof(P));
		CL.add_face_packed_D(P.v1, P.v2, P.v3, *(size_t*)&P.occ, 0.01f);
		if (P.b2sided)
			CL.add_face_packed_D(P.v3, P.v2, P.v1, *(size_t*)&P.occ, 0.01f);
	}
	geom_SOM = xr_new<CDB::MODEL>();
	geom_SOM->build(CL.getV(), int(CL.getVS()), CL.getT(), int(CL.getTS()), nullptr, nullptr, false);

	geom->close();
}

void CSoundRender_Core::set_geometry_env(IReader* I)
{
	xr_delete(geom_ENV);
	if (0 == I)				return;
	if (0 == s_environment)	return;

	// Assosiate names
	xr_vector<u16>			ids;
	IReader*				names = I->open_chunk(0);
	while (!names->eof())
	{
		string256			n;
		names->r_stringZ(n, sizeof(n));
		int id = s_environment->GetID(n);
		R_ASSERT(id >= 0);
		ids.push_back(u16(id));
	}
	names->close();

	// Load geometry
	IReader*			geom_ch = I->open_chunk(1);

	u8*	_data = (u8*)xr_malloc(geom_ch->length());

	Memory.mem_copy(_data, geom_ch->pointer(), geom_ch->length());

	IReader* geom = xr_new<IReader>(_data, geom_ch->length(), 0);

	hdrCFORM			H;
	geom->r(&H, sizeof(hdrCFORM));
	R_ASSERT(H.version == CFORM_CURRENT_VERSION);
	Fvector*	verts = (Fvector*)geom->pointer();
	CDB::TRI*	tris = (CDB::TRI*)(verts + H.vertcount);
#ifndef _M_X64
	for (u32 it = 0; it < H.facecount; it++)
	{
		CDB::TRI*	T = tris + it;
		u16		id_front = (u16)((T->dummy & 0x0000ffff) >> 0);		//	front face
		u16		id_back = (u16)((T->dummy & 0xffff0000) >> 16);		//	back face
		R_ASSERT(id_front < (u16)ids.size());
		R_ASSERT(id_back < (u16)ids.size());
		T->dummy = u32(ids[id_back] << 16) | u32(ids[id_front]);
	}
#endif
	geom_ENV = xr_new<CDB::MODEL>();
	geom_ENV->build(verts, H.vertcount, tris, H.facecount);
	geom_ch->close();
	geom->close();
	xr_free(_data);
}

void	CSoundRender_Core::create(ref_sound& S, const char* fName, esound_type sound_type, int game_type)
{
	if (!bPresent)		return;
	S._p = xr_new<ref_sound_data>(fName, sound_type, game_type);
}

void	CSoundRender_Core::attach_tail(ref_sound& S, const char* fName)
{
	if (!bPresent)		return;
	string_path			fn;
	xr_strcpy(fn, fName);
	if (strext(fn))		*strext(fn) = 0;
	if (S._p->fn_attached[0].size() && S._p->fn_attached[1].size())
	{
#ifdef DEBUG
		Msg("! 2 file already in queue [%s][%s]", S._p->fn_attached[0].c_str(), S._p->fn_attached[1].c_str());
#endif // #ifdef DEBUG
		return;
	}

	u32 idx = S._p->fn_attached[0].size() ? 1 : 0;

	S._p->fn_attached[idx] = fn;

	CSoundRender_Source* s = SoundRender->i_create_source(fn);
	S._p->dwBytesTotal += s->bytes_total();
	S._p->fTimeTotal += s->length_sec();
	if (S._feedback())
		((CSoundRender_Emitter*)S._feedback())->fTimeToStop += s->length_sec();

	SoundRender->i_destroy_source(s);
}

void	CSoundRender_Core::clone(ref_sound& S, const ref_sound& from, esound_type sound_type, int	game_type)
{
	if (!bPresent)		return;
	S._p = xr_new<ref_sound_data>();
	S._p->handle = from._p->handle;
	S._p->dwBytesTotal = from._p->dwBytesTotal;
	S._p->fTimeTotal = from._p->fTimeTotal;
	S._p->fn_attached[0] = from._p->fn_attached[0];
	S._p->fn_attached[1] = from._p->fn_attached[1];
	S._p->g_type = (game_type == sg_SourceType) ? S._p->handle->game_type() : game_type;
	S._p->s_type = sound_type;
}


void	CSoundRender_Core::play(ref_sound& S, CObject* O, u32 flags, float delay)
{
	if (!bPresent || 0 == S._handle())return;
	S._p->g_object = O;
	if (S._feedback())	((CSoundRender_Emitter*)S._feedback())->rewind();
	else				i_play(&S, flags&sm_Looped, delay);

	if (flags&sm_2D || S._handle()->channels_num() == 2)
		S._feedback()->switch_to_2D();
}

void	CSoundRender_Core::play_no_feedback(ref_sound& S, CObject* O, u32 flags, float delay, Fvector* pos, float* vol, float* freq, Fvector2* range)
{
	if (!bPresent || 0 == S._handle())return;
	ref_sound_data_ptr	orig = S._p;
	S._p = xr_new<ref_sound_data>();
	S._p->handle = orig->handle;
	S._p->g_type = orig->g_type;
	S._p->g_object = O;
	S._p->dwBytesTotal = orig->dwBytesTotal;
	S._p->fTimeTotal = orig->fTimeTotal;
	S._p->fn_attached[0] = orig->fn_attached[0];
	S._p->fn_attached[1] = orig->fn_attached[1];

	i_play(&S, flags&sm_Looped, delay);

	if (flags&sm_2D || S._handle()->channels_num() == 2)
		S._feedback()->switch_to_2D();

	if (pos)			S._feedback()->set_position(*pos);
	if (freq)			S._feedback()->set_frequency(*freq);
	if (range)			S._feedback()->set_range((*range)[0], (*range)[1]);
	if (vol)			S._feedback()->set_volume(*vol);
	S._p = orig;
}

void	CSoundRender_Core::play_at_pos(ref_sound& S, CObject* O, const Fvector &pos, u32 flags, float delay)
{
	if (!bPresent || 0 == S._handle())return;
	S._p->g_object = O;
	if (S._feedback())	((CSoundRender_Emitter*)S._feedback())->rewind();
	else				i_play(&S, flags&sm_Looped, delay);

	S._feedback()->set_position(pos);

	if (flags&sm_2D || S._handle()->channels_num() == 2)
		S._feedback()->switch_to_2D();
}
void	CSoundRender_Core::destroy(ref_sound& S)
{
	if (S._feedback()) {
		CSoundRender_Emitter* E = (CSoundRender_Emitter*)S._feedback();
		E->stop(FALSE);
	}
	S._p = 0;
}

void CSoundRender_Core::_create_data(ref_sound_data& S, LPCSTR fName, esound_type sound_type, int game_type)
{
	string_path			fn;
	xr_strcpy(fn, fName);
	if (strext(fn))		*strext(fn) = 0;
	S.handle = (CSound_source*)SoundRender->i_create_source(fn);
	S.g_type = (game_type == sg_SourceType) ? S.handle->game_type() : game_type;
	S.s_type = sound_type;
	S.feedback = 0;
	S.g_object = 0;
	S.g_userdata = 0;
	S.dwBytesTotal = S.handle->bytes_total();
	S.fTimeTotal = S.handle->length_sec();
}
void CSoundRender_Core::_destroy_data(ref_sound_data& S)
{
	if (S.feedback) {
		CSoundRender_Emitter* E = (CSoundRender_Emitter*)S.feedback;
		E->stop(FALSE);
	}
	R_ASSERT(0 == S.feedback);
	SoundRender->i_destroy_source((CSoundRender_Source*)S.handle);

	S.handle = NULL;
}

CSoundRender_Environment*	CSoundRender_Core::get_environment(const Fvector& P)
{
	static CSoundRender_Environment	identity;

	if (bUserEnvironment) {
		return &s_user_environment;
	}
	else {
		if (geom_ENV) {
			Fvector	dir = { 0,-1,0 };
			geom_DB.ray_options(CDB::OPT_ONLYNEAREST);
			geom_DB.ray_query(geom_ENV, P, dir, 1000.f);
			if (geom_DB.r_count()) {
				CDB::RESULT*		r = geom_DB.r_begin();
				CDB::TRI*			T = geom_ENV->get_tris() + r->id;
				Fvector*			V = geom_ENV->get_verts();
				Fvector tri_norm;
				tri_norm.mknormal(V[T->verts[0]], V[T->verts[1]], V[T->verts[2]]);
				float	dot = dir.dotproduct(tri_norm);
				if (dot < 0) {
					u16		id_front = (u16)((T->dummy & 0x0000ffff) >> 0);		//	front face
					return	s_environment->Get(id_front);
				}
				else {
					u16		id_back = (u16)((T->dummy & 0xffff0000) >> 16);	//	back face
					return	s_environment->Get(id_back);
				}
			}
			else {
				identity.set_identity();
				return &identity;
			}
		}
		else {
			identity.set_identity();
			return &identity;
		}
	}
}

void CSoundRender_Core::env_apply()
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
	bListenerMoved = TRUE;
}

void CSoundRender_Core::update_listener(const Fvector& P, const Fvector& D, const Fvector& N, float dt)
{
}

void CSoundRender_Core::i_eax_listener_set(CSound_environment* _E)
{
	VERIFY(bEAX);
	CSoundRender_Environment* E = static_cast<CSoundRender_Environment*>(_E);
	EAXLISTENERPROPERTIES 		ep;
	ep.lRoom = iFloor(E->Room);	// room effect level at low frequencies
	ep.lRoomHF = iFloor(E->RoomHF);   // room effect high-frequency level re. low frequency level
	ep.flRoomRolloffFactor = E->RoomRolloffFactor;   // like DS3D flRolloffFactor but for room effect
	ep.flDecayTime = E->DecayTime;   // reverberation decay time at low frequencies
	ep.flDecayHFRatio = E->DecayHFRatio;   // high-frequency to low-frequency decay time ratio
	ep.lReflections = iFloor(E->Reflections);   // early reflections level relative to room effect
	ep.flReflectionsDelay = E->ReflectionsDelay;   // initial reflection delay time
	ep.lReverb = iFloor(E->Reverb);   // late reverberation level relative to room effect
	ep.flReverbDelay = E->ReverbDelay;   // late reverberation delay time relative to initial reflection
	ep.dwEnvironment = EAXLISTENER_DEFAULTENVIRONMENT;  	// sets all listener properties
	ep.flEnvironmentSize = E->EnvironmentSize;  	// environment size in meters
	ep.flEnvironmentDiffusion = E->EnvironmentDiffusion; 	// environment diffusion
	ep.flAirAbsorptionHF = E->AirAbsorptionHF;	// change in level per meter at 5 kHz
	ep.dwFlags = EAXLISTENER_DEFAULTFLAGS;	// modifies the behavior of properties

	u32 deferred = bDeferredEAX ? DSPROPERTY_EAXLISTENER_DEFERRED : 0;

	i_eax_set(&DSPROPSETID_EAX_ListenerProperties, deferred | DSPROPERTY_EAXLISTENER_ROOM, &ep.lRoom, sizeof(LONG));
	i_eax_set(&DSPROPSETID_EAX_ListenerProperties, deferred | DSPROPERTY_EAXLISTENER_ROOMHF, &ep.lRoomHF, sizeof(LONG));
	i_eax_set(&DSPROPSETID_EAX_ListenerProperties, deferred | DSPROPERTY_EAXLISTENER_ROOMROLLOFFFACTOR, &ep.flRoomRolloffFactor, sizeof(float));
	i_eax_set(&DSPROPSETID_EAX_ListenerProperties, deferred | DSPROPERTY_EAXLISTENER_DECAYTIME, &ep.flDecayTime, sizeof(float));
	i_eax_set(&DSPROPSETID_EAX_ListenerProperties, deferred | DSPROPERTY_EAXLISTENER_DECAYHFRATIO, &ep.flDecayHFRatio, sizeof(float));
	i_eax_set(&DSPROPSETID_EAX_ListenerProperties, deferred | DSPROPERTY_EAXLISTENER_REFLECTIONS, &ep.lReflections, sizeof(LONG));
	i_eax_set(&DSPROPSETID_EAX_ListenerProperties, deferred | DSPROPERTY_EAXLISTENER_REFLECTIONSDELAY, &ep.flReflectionsDelay, sizeof(float));
	i_eax_set(&DSPROPSETID_EAX_ListenerProperties, deferred | DSPROPERTY_EAXLISTENER_REVERB, &ep.lReverb, sizeof(LONG));
	i_eax_set(&DSPROPSETID_EAX_ListenerProperties, deferred | DSPROPERTY_EAXLISTENER_REVERBDELAY, &ep.flReverbDelay, sizeof(float));
	i_eax_set(&DSPROPSETID_EAX_ListenerProperties, deferred | DSPROPERTY_EAXLISTENER_ENVIRONMENTDIFFUSION, &ep.flEnvironmentDiffusion, sizeof(float));
	i_eax_set(&DSPROPSETID_EAX_ListenerProperties, deferred | DSPROPERTY_EAXLISTENER_AIRABSORPTIONHF, &ep.flAirAbsorptionHF, sizeof(float));
	i_eax_set(&DSPROPSETID_EAX_ListenerProperties, deferred | DSPROPERTY_EAXLISTENER_FLAGS, &ep.dwFlags, sizeof(DWORD));
}

void CSoundRender_Core::i_eax_listener_get(CSound_environment* _E)
{
	VERIFY(bEAX);
	CSoundRender_Environment* E = static_cast<CSoundRender_Environment*>(_E);
	EAXLISTENERPROPERTIES 		ep;
	i_eax_get(&DSPROPSETID_EAX_ListenerProperties, DSPROPERTY_EAXLISTENER_ALLPARAMETERS, &ep, sizeof(EAXLISTENERPROPERTIES));
	E->Room = (float)ep.lRoom;
	E->RoomHF = (float)ep.lRoomHF;
	E->RoomRolloffFactor = (float)ep.flRoomRolloffFactor;
	E->DecayTime = (float)ep.flDecayTime;
	E->DecayHFRatio = (float)ep.flDecayHFRatio;
	E->Reflections = (float)ep.lReflections;
	E->ReflectionsDelay = (float)ep.flReflectionsDelay;
	E->Reverb = (float)ep.lReverb;
	E->ReverbDelay = (float)ep.flReverbDelay;
	E->EnvironmentSize = (float)ep.flEnvironmentSize;
	E->EnvironmentDiffusion = (float)ep.flEnvironmentDiffusion;
	E->AirAbsorptionHF = (float)ep.flAirAbsorptionHF;
}

void CSoundRender_Core::i_eax_commit_setting()
{
	// commit eax 
	if (bDeferredEAX)
		i_eax_set(&DSPROPSETID_EAX_ListenerProperties, DSPROPERTY_EAXLISTENER_COMMITDEFERREDSETTINGS, NULL, 0);
}

void CSoundRender_Core::object_relcase(CObject* obj)
{
	if (obj) {
		for (u32 eit = 0; eit < s_emitters.size(); eit++) {
			if (s_emitters[eit])
				if (s_emitters[eit]->owner_data)
					if (obj == s_emitters[eit]->owner_data->g_object)
						s_emitters[eit]->owner_data->g_object = 0;
		}
	}
}
