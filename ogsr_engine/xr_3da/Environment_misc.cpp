#include "stdafx.h"
#pragma hdrstop

#include "Environment.h"
#include "xr_efflensflare.h"
#include "thunderbolt.h"
#include "rain.h"
#include "resourcemanager.h"

#define READ_IF_EXISTS(ltx,method,section,name,default_value)\
	((ltx->line_exist(section,name)) ? (ltx->method(section,name)) : (default_value))

//-----------------------------------------------------------------------------
// Environment modifier
//-----------------------------------------------------------------------------
void	CEnvModifier::load	(IReader* fs)
{
//	Fvector			dummy;
	fs->r_fvector3	(position);
	radius			= fs->r_float	();
	power			= fs->r_float	();
	far_plane		= fs->r_float	();
	fs->r_fvector3	(fog_color);
	fog_density		= fs->r_float	();
	fs->r_fvector3	(ambient);
	fs->r_fvector3	(sky_color);
	fs->r_fvector3	(hemi_color);
}
float	CEnvModifier::sum	(CEnvModifier& M, Fvector3& view)
{
	float	_dist_sq	=	view.distance_to_sqr(M.position);
	if (_dist_sq>=(M.radius*M.radius))	return 0;
	float	_att		=	1-_sqrt(_dist_sq)/M.radius;	//[0..1];
	float	_power		=	M.power*_att;
	far_plane			+=	M.far_plane*_power;
	fog_color.mad		(M.fog_color,_power);
	fog_density			+=	M.fog_density*_power;
	ambient.mad			(M.ambient,_power);
//	lmap_color.mad		(M.lmap_color,_power);
	sky_color.mad		(M.sky_color,_power);
	hemi_color.mad		(M.hemi_color,_power);
	return				_power;
}

//-----------------------------------------------------------------------------
// Environment ambient
//-----------------------------------------------------------------------------
void CEnvAmbient::load(const shared_str& sect)
{
	section				= sect;
	string_path			tmp;
	// sounds
	if (pSettings->line_exist(sect,"sounds")){
		Fvector2 t		= pSettings->r_fvector2	(sect,"sound_period");
		sound_period.set(iFloor(t.x*1000.f),iFloor(t.y*1000.f));
		sound_dist		= pSettings->r_fvector2	(sect,"sound_dist"); if (sound_dist[0]>sound_dist[1]) std::swap(sound_dist[0],sound_dist[1]);
		LPCSTR snds		= pSettings->r_string	(sect,"sounds");
		u32 cnt			= _GetItemCount(snds);
		if (cnt){
			sounds.resize(cnt);
			for (u32 k=0; k<cnt; ++k)
				sounds[k].create(_GetItem(snds,k,tmp),st_Effect,sg_SourceType);
		}
	}
	// effects
	if (pSettings->line_exist(sect,"effects")){
		Fvector2 t		= pSettings->r_fvector2	(sect,"effect_period");
		effect_period.set(iFloor(t.x*1000.f),iFloor(t.y*1000.f));
		LPCSTR effs		= pSettings->r_string	(sect,"effects");
		u32 cnt			= _GetItemCount(effs);
		if (cnt){
			effects.resize(cnt);
			for (u32 k=0; k<cnt; ++k){
				_GetItem(effs,k,tmp);
				effects[k].life_time		= iFloor(pSettings->r_float(tmp,"life_time")*1000.f);
				effects[k].particles		= pSettings->r_string	(tmp,"particles");		VERIFY(effects[k].particles.size());
				effects[k].offset			= pSettings->r_fvector3	(tmp,"offset");
				effects[k].wind_gust_factor	= pSettings->r_float	(tmp,"wind_gust_factor");
				if (pSettings->line_exist(tmp,"sound"))
					effects[k].sound.create	(pSettings->r_string(tmp,"sound"),st_Effect,sg_SourceType);
			}
		}
	}
	VERIFY(!sounds.empty() || !effects.empty());
}

//-----------------------------------------------------------------------------
// Environment descriptor
//-----------------------------------------------------------------------------
CEnvDescriptor::CEnvDescriptor()
{
	exec_time			= 0.0f;
	exec_time_loaded	= 0.0f;
	
	clouds_color.set	(1,1,1,1);
	sky_color.set		(1,1,1);
	sky_rotation		= 0.0f;

	far_plane			= 400.0f;;

	fog_color.set		(1,1,1);
	fog_density			= 0.0f;
	fog_distance		= 400.0f;

	rain_density		= 0.0f;
	rain_color.set		(0,0,0);

	bolt_period			= 0.0f;
	bolt_duration		= 0.0f;

    wind_velocity		= 0.0f;
    wind_direction		= 0.0f;

	rain_max_drop_angle = deg2rad(10.f);
	sun_shafts = 0.f;
	sun_shafts_length = 0.f;
	rain_increase_speed = 1.0;
	moon_road_intensity = 0.f;
    
	ambient.set			(0,0,0);
	hemi_color.set		(1,1,1,1);
	sun_color.set		(1,1,1);
	sun_dir.set			(0,-1,0);

    lens_flare_id		= -1;
	tb_id				= -1;
    
	env_ambient			= NULL;
}

#define	C_CHECK(C)	if (C.x<0 || C.x>2 || C.y<0 || C.y>2 || C.z<0 || C.z>2)	{ Msg("! Invalid '%s' in env-section '%s'",#C,S);}
void CEnvDescriptor::load(float exec_tm, LPCSTR S, CEnvironment* parent)
{
	exec_time = exec_tm;
	exec_time_loaded = exec_time;
	string_path				st, st_env;
	strcpy_s(st, pSettings->r_string(S, "sky_texture"));
	strconcat(sizeof(st_env), st_env, st, "#small");
	sky_texture_name = st;
	sky_texture_env_name = st_env;
	clouds_texture_name = pSettings->r_string(S, "clouds_texture");
	LPCSTR	cldclr = pSettings->r_string(S, "clouds_color");
	float	multiplier = 0, save = 0;
	sscanf(cldclr, "%f,%f,%f,%f,%f", &clouds_color.x, &clouds_color.y, &clouds_color.z, &clouds_color.w, &multiplier);
	save = clouds_color.w;	clouds_color.mul(.5f*multiplier);		clouds_color.w = save;
	sky_color = pSettings->r_fvector3(S, "sky_color");		sky_color.mul(.5f);
	if (pSettings->line_exist(S, "sky_rotation"))	sky_rotation = deg2rad(pSettings->r_float(S, "sky_rotation"));
	else											sky_rotation = 0;
	far_plane = pSettings->r_float(S, "far_plane");
	fog_color = pSettings->r_fvector3(S, "fog_color");
	fog_density = pSettings->r_float(S, "fog_density");
	fog_distance = pSettings->r_float(S, "fog_distance");
	rain_density = pSettings->r_float(S, "rain_density");		clamp(rain_density, 0.f, 1.f);
	rain_color = pSettings->r_fvector3(S, "rain_color");
	wind_velocity = pSettings->r_float(S, "wind_velocity");
	wind_direction = deg2rad(pSettings->r_float(S, "wind_direction"));
	ambient = pSettings->r_fvector3(S, "ambient");
	hemi_color = pSettings->r_fvector4(S, "hemi_color");
	sun_color = pSettings->r_fvector3(S, "sun_color");
	Fvector2 sund = pSettings->r_fvector2(S, "sun_dir");	sun_dir.setHP(deg2rad(sund.y), deg2rad(sund.x));
	VERIFY2(sun_dir.y<0, "Invalid sun direction settings while loading");

	sun_shafts = READ_IF_EXISTS(pSettings, r_float, S, "sun_shafts", 0.f);
	sun_shafts_length = READ_IF_EXISTS(pSettings, r_float, S, "sun_shafts_length", 0.f);
	float rmda = READ_IF_EXISTS(pSettings, r_float, S, "rain_max_drop_angle", 10.f);
	rain_max_drop_angle = deg2rad(rmda);
	rain_increase_speed = READ_IF_EXISTS(pSettings, r_float, S, "rain_increase_speed", 1.f);
	moon_road_intensity = READ_IF_EXISTS(pSettings, r_float, S, "moon_road_intensity", 0.f);

	lens_flare_id = parent->eff_LensFlare->AppendDef(pSettings, pSettings->r_string(S, "flares"));
	tb_id = parent->eff_Thunderbolt->AppendDef(pSettings, pSettings->r_string(S, "thunderbolt"));
	bolt_period = (tb_id >= 0) ? pSettings->r_float(S, "bolt_period") : 0.f;
	bolt_duration = (tb_id >= 0) ? pSettings->r_float(S, "bolt_duration") : 0.f;
	env_ambient = pSettings->line_exist(S, "env_ambient") ? parent->AppendEnvAmb(pSettings->r_string(S, "env_ambient")) : 0;

	C_CHECK(clouds_color);
	C_CHECK(sky_color);
	C_CHECK(fog_color);
	C_CHECK(rain_color);
	C_CHECK(ambient);
	//	C_CHECK					(lmap_color	);
	C_CHECK(hemi_color);
	C_CHECK(sun_color);

	// KD
	on_device_destroy();
	// KD

	on_device_create();
}
void CEnvDescriptor::load	(LPCSTR exec_tm, LPCSTR S, CEnvironment* parent)
{
	Ivector3 tm				={0,0,0};
	sscanf					(exec_tm,"%d:%d:%d",&tm.x,&tm.y,&tm.z);
	R_ASSERT3				((tm.x>=0)&&(tm.x<24)&&(tm.y>=0)&&(tm.y<60)&&(tm.z>=0)&&(tm.z<60),"Incorrect weather time",S);
	load					(tm.x*3600.f + tm.y*60.f + tm.z, S, parent);
}

void CEnvDescriptor::on_device_create	()
{
	if (sky_texture_name.size())	sky_texture.create		(sky_texture_name.c_str());
	if (sky_texture_env_name.size())sky_texture_env.create	(sky_texture_env_name.c_str());
	if (clouds_texture_name.size())	clouds_texture.create	(clouds_texture_name.c_str());
}

void CEnvDescriptor::on_device_destroy	()
{
	sky_texture.destroy		();
	sky_texture_env.destroy	();
	clouds_texture.destroy	();
}

//-----------------------------------------------------------------------------
// Environment Mixer
//-----------------------------------------------------------------------------
void CEnvDescriptorMixer::destroy()
{
	sky_r_textures.clear		();
	sky_r_textures_env.clear	();
	clouds_r_textures.clear		();

	sky_texture.destroy			();
	sky_texture_env.destroy		();
	clouds_texture.destroy		();
}

void CEnvDescriptorMixer::clear	()
{
	std::pair<u32,ref_texture>	zero = mk_pair(u32(0),ref_texture(0));
	sky_r_textures.clear		();
	sky_r_textures.push_back	(zero);
	sky_r_textures.push_back	(zero);
	sky_r_textures.push_back	(zero);

	sky_r_textures_env.clear	();
	sky_r_textures_env.push_back(zero);
	sky_r_textures_env.push_back(zero);
	sky_r_textures_env.push_back(zero);

	clouds_r_textures.clear		();
	clouds_r_textures.push_back	(zero);
	clouds_r_textures.push_back	(zero);
	clouds_r_textures.push_back	(zero);
}
int get_ref_count(IUnknown* ii);
void CEnvDescriptorMixer::lerp	(CEnvironment* , CEnvDescriptor& A, CEnvDescriptor& B, float f, CEnvModifier& M, float m_power)
{
	float	_power			=	1.f/(m_power+1);	// the environment itself
	float	fi				=	1-f;

	sky_r_textures.clear		();
	sky_r_textures.push_back	(mk_pair(0,A.sky_texture));
	sky_r_textures.push_back	(mk_pair(1,B.sky_texture));

	sky_r_textures_env.clear	();

	sky_r_textures_env.push_back(mk_pair(0,A.sky_texture_env));
	sky_r_textures_env.push_back(mk_pair(1,B.sky_texture_env));

	clouds_r_textures.clear		();
	clouds_r_textures.push_back	(mk_pair(0,A.clouds_texture));
	clouds_r_textures.push_back	(mk_pair(1,B.clouds_texture));

	weight					=	f;

	clouds_color.lerp		(A.clouds_color,B.clouds_color,f);
	sky_rotation			=	(fi*A.sky_rotation + f*B.sky_rotation);
	far_plane				=	(fi*A.far_plane + f*B.far_plane + M.far_plane)*psVisDistance*_power;
	fog_color.lerp			(A.fog_color,B.fog_color,f).add(M.fog_color).mul(_power);
	fog_density				=	(fi*A.fog_density + f*B.fog_density + M.fog_density)*_power;
	fog_distance			=	(fi*A.fog_distance + f*B.fog_distance);
	fog_near				=	(1.0f - fog_density)*0.85f * fog_distance;
	fog_far					=	0.99f * fog_distance;
	rain_density			=	fi*A.rain_density + f*B.rain_density;
	rain_color.lerp			(A.rain_color,B.rain_color,f);
	bolt_period				=	fi*A.bolt_period + f*B.bolt_period;
	bolt_duration			=	fi*A.bolt_duration + f*B.bolt_duration;
	// wind
	wind_velocity			=	fi*A.wind_velocity + f*B.wind_velocity;
	wind_direction			=	fi*A.wind_direction + f*B.wind_direction;

	// rain
	rain_max_drop_angle = fi*A.rain_max_drop_angle + f*B.rain_max_drop_angle;
	rain_increase_speed = fi*A.rain_increase_speed + f*B.rain_increase_speed;

	//sunshafts
	sun_shafts = fi*A.sun_shafts + f*B.sun_shafts;
	sun_shafts_length = fi*A.sun_shafts_length + f*B.sun_shafts_length;
	
	// other
	moon_road_intensity = fi*A.moon_road_intensity + f*B.moon_road_intensity;

	// colors
	sky_color.lerp			(A.sky_color,B.sky_color,f).add(M.sky_color).mul(_power);
	ambient.lerp			(A.ambient,B.ambient,f).add(M.ambient).mul(_power);
	hemi_color.lerp			(A.hemi_color,B.hemi_color,f);
	hemi_color.x			+= M.hemi_color.x;
	hemi_color.y			+= M.hemi_color.y; 
	hemi_color.z			+= M.hemi_color.z;
	hemi_color.x			*= _power;
	hemi_color.y			*= _power;
	hemi_color.z			*= _power;
	sun_color.lerp			(A.sun_color,B.sun_color,f);
	sun_dir.lerp			(A.sun_dir,B.sun_dir,f).normalize();
	VERIFY2					(sun_dir.y<0,"Invalid sun direction settings while lerp");

}

//-----------------------------------------------------------------------------
// Environment IO
//-----------------------------------------------------------------------------
CEnvAmbient* CEnvironment::AppendEnvAmb		(const shared_str& sect)
{
	for (EnvAmbVecIt it=Ambients.begin(); it!=Ambients.end(); it++)
		if ((*it)->name().equal(sect)) return *it;
	Ambients.push_back		(xr_new<CEnvAmbient>());
	Ambients.back()->load	(sect);
	return Ambients.back();
}

void	CEnvironment::mods_load			()
{
	Modifiers.clear_and_free			();
	string_path							path;
	if (FS.exist(path,"$level$","level.env_mod"))	
	{
		IReader*	fs	= FS.r_open		(path);
		u32			id	= 0;
		while		(fs->find_chunk(id))	
		{
			CEnvModifier		E;
			E.load				(fs);
			Modifiers.push_back	(E);
			id					++;
		}
		FS.r_close	(fs);
	}
}
void	CEnvironment::mods_unload		()
{
	Modifiers.clear_and_free			();
}

void CEnvironment::load		()
{
	tonemap					= Device.Resources->_CreateTexture("$user$tonemap");	//. hack
	if (!eff_Rain)    		eff_Rain 		= xr_new<CEffect_Rain>();
	if (!eff_LensFlare)		eff_LensFlare 	= xr_new<CLensFlare>();
	if (!eff_Thunderbolt)	eff_Thunderbolt	= xr_new<CEffect_Thunderbolt>();
	// load weathers
	if (WeatherCycles.empty()){
		LPCSTR first_weather=0;
		int weather_count	= pSettings->line_count("weathers");
		for (int w_idx=0; w_idx<weather_count; w_idx++){
			LPCSTR weather, sect_w;
			if (pSettings->r_line("weathers",w_idx,&weather,&sect_w)){
				if (0==first_weather) first_weather=weather;
				int env_count	= pSettings->line_count(sect_w);
				LPCSTR exec_tm, sect_e;
				for (int env_idx=0; env_idx<env_count; env_idx++){
					if (pSettings->r_line(sect_w,env_idx,&exec_tm,&sect_e)){
						CEnvDescriptor*		D=xr_new<CEnvDescriptor>();
						D->load				(exec_tm,sect_e,this);
						WeatherCycles[weather].push_back	(D);
#ifdef DEBUG
						D->sect_name		= sect_e;
#endif
					}
				}
			}
		}
		// sorting weather envs
		EnvsMapIt _I=WeatherCycles.begin();
		EnvsMapIt _E=WeatherCycles.end();
		for (; _I!=_E; _I++){
			R_ASSERT3	(_I->second.size()>1,"Environment in weather must >=2",*_I->first);
			std::sort(_I->second.begin(),_I->second.end(),sort_env_etl_pred);
		}
		R_ASSERT2	(!WeatherCycles.empty(),"Empty weathers.");
		SetWeather	(first_weather);
	}
	// load weather effects
	if (WeatherFXs.empty()){
		int line_count	= pSettings->line_count("weather_effects");
		for (int w_idx=0; w_idx<line_count; w_idx++){
			LPCSTR weather, sect_w;
			if (pSettings->r_line("weather_effects",w_idx,&weather,&sect_w)){
				EnvVec& env		= WeatherFXs[weather];
				env.push_back	(xr_new<CEnvDescriptor>()); env.back()->exec_time_loaded = 0;
				env.push_back	(xr_new<CEnvDescriptor>()); env.back()->exec_time_loaded = 0;
				int env_count	= pSettings->line_count(sect_w);
				LPCSTR exec_tm, sect_e;
				for (int env_idx=0; env_idx<env_count; env_idx++){
					if (pSettings->r_line(sect_w,env_idx,&exec_tm,&sect_e)){
						CEnvDescriptor*	D=xr_new<CEnvDescriptor>();
						D->load			(exec_tm,sect_e,this);
						env.push_back	(D);
#ifdef DEBUG
						D->sect_name		= sect_e;
#endif
					}
				}
				env.push_back	(xr_new<CEnvDescriptor>()); env.back()->exec_time_loaded = DAY_LENGTH;
			}
		}
		// sorting weather envs
		EnvsMapIt _I=WeatherFXs.begin();
		EnvsMapIt _E=WeatherFXs.end();
		for (; _I!=_E; _I++){
			R_ASSERT3	(_I->second.size()>1,"Environment in weather must >=2",*_I->first);
			std::sort(_I->second.begin(),_I->second.end(),sort_env_etl_pred);
		}
	}
}

void CEnvironment::unload	()
{
	EnvsMapIt _I,_E;
	// clear weathers
	_I		= WeatherCycles.begin();
	_E		= WeatherCycles.end();
	for (; _I!=_E; _I++){
		for (EnvIt it=_I->second.begin(); it!=_I->second.end(); it++)
			xr_delete	(*it);
	}

	WeatherCycles.clear		();
	// clear weather effect
	_I		= WeatherFXs.begin();
	_E		= WeatherFXs.end();
	for (; _I!=_E; _I++){
		for (EnvIt it=_I->second.begin(); it!=_I->second.end(); it++)
			xr_delete	(*it);
	}
	WeatherFXs.clear		();
	// clear ambient
	for (EnvAmbVecIt it=Ambients.begin(); it!=Ambients.end(); it++)
		xr_delete		(*it);
	Ambients.clear		();
	// misc
	xr_delete			(eff_Rain);
	xr_delete			(eff_LensFlare);
	xr_delete			(eff_Thunderbolt);
	CurrentWeather		= 0;
	CurrentWeatherName	= 0;
	CurrentEnv.clear	();
	Invalidate			();
	tonemap				= 0;
}
