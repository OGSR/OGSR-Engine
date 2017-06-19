#ifndef EnvironmentH
#define EnvironmentH

// refs
class ENGINE_API	IRender_Visual;
class ENGINE_API	CInifile;
class ENGINE_API 	CEnvironment;

// refs - effects
class ENGINE_API	CEnvironment;
class ENGINE_API	CLensFlare;	
class ENGINE_API	CEffect_Rain;
class ENGINE_API	CEffect_Thunderbolt;

class ENGINE_API	CPerlinNoise1D;

#define DAY_LENGTH		86400.f

#include "blenders\blender.h"
#include "Rain.h"
class CBlender_skybox		: public IBlender  
{
public:
	virtual		LPCSTR		getComment()	{ return "INTERNAL: combiner";	}
	virtual		BOOL		canBeDetailed()	{ return FALSE;	}
	virtual		BOOL		canBeLMAPped()	{ return FALSE;	}

	virtual		void		Compile			(CBlender_Compile& C)
	{
		C.r_Pass			("sky2",		"sky2",			FALSE,	TRUE, FALSE);
		C.r_Sampler_clf		("s_sky0",		"$null"			);
		C.r_Sampler_clf		("s_sky1",		"$null"			);
		C.r_Sampler_rtf		("s_tonemap",	"$user$tonemap"	);	//. hack
		C.r_End				();
	}
};

// t-defs
class ENGINE_API	CEnvModifier
{
public:
	Fvector3			position;
	float				radius;
	float				power;

	float				far_plane;
	Fvector3			fog_color;
	float				fog_density;
	Fvector3			ambient;
	Fvector3			sky_color;		
	Fvector3			hemi_color;

	void				load		(IReader*		fs);
	float				sum			(CEnvModifier&	_another, Fvector3& view);
};

class ENGINE_API	CEnvAmbient{
public:
	struct SEffect{
		u32 			life_time;
		ref_sound		sound;		
		shared_str		particles;
		Fvector			offset;
		float			wind_gust_factor;
	};
	DEFINE_VECTOR(SEffect,EffectVec,EffectVecIt);
protected:
	shared_str			section;
	EffectVec			effects;
	xr_vector<ref_sound>sounds;
	Fvector2			sound_dist;
	Ivector2			sound_period;
	Ivector2			effect_period;
public:
	void				load				(const shared_str& section);
	IC SEffect*			get_rnd_effect		(){return effects.empty()?NULL:&effects[::Random.randI(effects.size())];}
	IC ref_sound*		get_rnd_sound		(){return sounds.empty()?NULL:&sounds[::Random.randI(sounds.size())];}
	IC const shared_str&name				(){return section;}
	IC u32				get_rnd_sound_time	(){return ::Random.randI(sound_period.x,sound_period.y);}
	IC float			get_rnd_sound_dist	(){return ::Random.randF(sound_dist.x,sound_dist.y);}
	IC u32				get_rnd_effect_time (){return ::Random.randI(effect_period.x,effect_period.y);}
};

class ENGINE_API	CEnvDescriptor
{
public:
	float				exec_time;
	float				exec_time_loaded;
	
	shared_str			sky_texture_name	;
	shared_str			sky_texture_env_name;
	shared_str			clouds_texture_name	;

	ref_texture			sky_texture		;
	ref_texture			sky_texture_env	;
	ref_texture			clouds_texture	;

	Fvector4			clouds_color	;
	Fvector3			sky_color		;
	float				sky_rotation	;

	float				far_plane;

	Fvector3			fog_color;
	float				fog_density;
	float				fog_distance;

	float				rain_density;
	Fvector3			rain_color;

	float				bolt_period;
	float				bolt_duration;

    float				wind_velocity;
    float				wind_direction;  
    
	Fvector3			ambient		;
	Fvector4			hemi_color	;	// w = R2 correction
	Fvector3			sun_color	;
	Fvector3			sun_dir		;

	float				sun_shafts;
	float				sun_shafts_length;
	float				rain_max_drop_angle;
	float				rain_increase_speed;
	float				moon_road_intensity;

    int					lens_flare_id;
	int					tb_id;
    
	CEnvAmbient*		env_ambient;

#ifdef DEBUG
	shared_str			sect_name;
#endif	
						CEnvDescriptor	();

	void				load			(LPCSTR exec_tm, LPCSTR sect, CEnvironment* parent);
	void				load			(float exec_tm, LPCSTR sect, CEnvironment* parent);
	void				copy			(const CEnvDescriptor& src)
	{
		float tm0		= exec_time;
		float tm1		= exec_time_loaded; 
		*this			= src;
		exec_time		= tm0;
		exec_time_loaded= tm1;
	}

	void				on_device_create	();
	void				on_device_destroy	();
};

class ENGINE_API		CEnvDescriptorMixer: public CEnvDescriptor{
public:
	STextureList		sky_r_textures;		
	STextureList		sky_r_textures_env;	
	STextureList		clouds_r_textures;	
	float				weight;				

	float				fog_near;		
	float				fog_far;		
public:
	void				lerp			(CEnvironment* parent, CEnvDescriptor& A, CEnvDescriptor& B, float f, CEnvModifier& M, float m_power);
	void				clear			();
	void				destroy			();
};

class ENGINE_API	CEnvironment
{
	struct str_pred : public std::binary_function<shared_str, shared_str, bool>	{	
		IC bool operator()(const shared_str& x, const shared_str& y) const
		{	return xr_strcmp(x,y)<0;	}
	};
public:
	DEFINE_VECTOR			(CEnvAmbient*,EnvAmbVec,EnvAmbVecIt);
	DEFINE_VECTOR			(CEnvDescriptor*,EnvVec,EnvIt);
	DEFINE_MAP_PRED			(shared_str,EnvVec,EnvsMap,EnvsMapIt,str_pred);
private:
	// clouds
	FvectorVec				CloudsVerts;
	U16Vec					CloudsIndices;
private:
	float					NormalizeTime	(float tm);
	float					TimeDiff		(float prev, float cur);
	float					TimeWeight		(float val, float min_t, float max_t);
	void					SelectEnvs		(EnvVec* envs, CEnvDescriptor*& e0, CEnvDescriptor*& e1, float tm);
	void					SelectEnv		(EnvVec* envs, CEnvDescriptor*& e, float tm);
	void					StopWFX			();
public:
#ifndef _EDITOR
	void					ForceReselectEnvs() { SelectEnvs(CurrentWeather, Current[0], Current[1], fGameTime); eff_Rain->set_state(CEffect_Rain::States::stIdle);
	};	// KD: временая штука для скриптовой погоды
#endif
	static bool sort_env_pred	(const CEnvDescriptor* x, const CEnvDescriptor* y)
	{	return x->exec_time < y->exec_time;	}
	static bool sort_env_etl_pred	(const CEnvDescriptor* x, const CEnvDescriptor* y)
	{	return x->exec_time_loaded < y->exec_time_loaded;	}
protected:
	CBlender_skybox			m_b_skybox;
	CPerlinNoise1D*			PerlinNoise1D;

	float					fGameTime;
public:
	float					wind_strength_factor;	
	float					wind_gust_factor;
	// Environments
	CEnvDescriptorMixer		CurrentEnv;
	CEnvDescriptor*			Current[2];

	bool					bWFX;
	float					wfx_time;
	CEnvDescriptor*			WFX_end_desc[2];
    
    EnvVec*					CurrentWeather;
    shared_str				CurrentWeatherName;
	shared_str				CurrentCycleName;

	EnvsMap					WeatherCycles;
	EnvsMap					WeatherFXs;
	xr_vector<CEnvModifier>	Modifiers;
	EnvAmbVec				Ambients;

	ref_shader				sh_2sky;
	ref_geom				sh_2geom;

	ref_shader				clouds_sh;
	ref_geom				clouds_geom;

	CEffect_Rain*			eff_Rain;
	CLensFlare*				eff_LensFlare;
	CEffect_Thunderbolt*	eff_Thunderbolt;

	float					fTimeFactor;
	ref_texture				tonemap;
	ref_texture				tsky0,tsky1;

    void					SelectEnvs			(float gt);

	void					UpdateAmbient		();
	CEnvAmbient*			AppendEnvAmb		(const shared_str& sect);

	void					Invalidate			();
public:
							CEnvironment		();
							~CEnvironment		();

	void					load				();
    void					unload				();

	void					mods_load			();
	void					mods_unload			();

	void					OnFrame				();

	void					RenderSky			();
	void					RenderClouds		();
	void					RenderFlares		();
	void					RenderLast			();

	bool					SetWeatherFX		(shared_str name);
	bool					IsWFXPlaying		(){return bWFX;}
    void					SetWeather			(shared_str name, bool forced=false);
    shared_str				GetWeather			()					{ return CurrentWeatherName;}
	void					SetGameTime			(float game_time, float time_factor);

	void					OnDeviceCreate		();
	void					OnDeviceDestroy		();

	// editor-related
#ifdef _EDITOR
public:
	float					ed_from_time		;
	float					ed_to_time			;
public:
    void					ED_Reload			();
    float					GetGameTime			(){return fGameTime;}
#endif
};

ENGINE_API extern Flags32	psEnvFlags;
ENGINE_API extern float		psVisDistance;

#endif //EnvironmentH
