#include "stdafx.h"
#pragma hdrstop

#ifndef _EDITOR
    #include "render.h"
#endif

#include "Environment.h"
#include "xr_efflensflare.h"
#include "rain.h"
#include "thunderbolt.h"
#include "xrHemisphere.h"
#include "perlin.h"

#include "xr_input.h"

#include "resourcemanager.h"

#ifndef _EDITOR
	#include "IGame_Level.h"
#endif

#include "D3DUtils.h"
#include "xrCore.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
ENGINE_API	float			psVisDistance	= 1.f;
static const float			MAX_NOISE_FREQ	= 0.03f;

//#define WEATHER_LOGGING

// real WEATHER->WFX transition time
#define WFX_TRANS_TIME		5.f

//////////////////////////////////////////////////////////////////////////
// environment
CEnvironment::CEnvironment	()
{
	bWFX					= false;
	Current[0]				= 0;
	Current[1]				= 0;
    CurrentWeather			= 0;
    CurrentWeatherName		= 0;
	eff_Rain				= 0;
    eff_LensFlare 			= 0;
    eff_Thunderbolt			= 0;
	OnDeviceCreate			();
#ifdef _EDITOR
	ed_from_time			= 0.f;
	ed_to_time				= DAY_LENGTH;
#endif
	fGameTime				= 0.f;
    fTimeFactor				= 12.f;

	wind_strength_factor	= 0.f;
	wind_gust_factor		= 0.f;

	// fill clouds hemi verts & faces 
	const Fvector* verts;
	CloudsVerts.resize		(xrHemisphereVertices(2,verts));
	CopyMemory			(&CloudsVerts.front(),verts,CloudsVerts.size()*sizeof(Fvector));
	const u16* indices;
	CloudsIndices.resize	(xrHemisphereIndices(2,indices));
	CopyMemory			(&CloudsIndices.front(),indices,CloudsIndices.size()*sizeof(u16));

	// perlin noise
	PerlinNoise1D			= xr_new<CPerlinNoise1D>(::Random.randI(0,0xFFFF));
	PerlinNoise1D->SetOctaves(2);
	PerlinNoise1D->SetAmplitude(0.66666f);

	tsky0					= Device.Resources->_CreateTexture("$user$sky0");
	tsky1					= Device.Resources->_CreateTexture("$user$sky1");
}
CEnvironment::~CEnvironment	()
{
	xr_delete				(PerlinNoise1D);
	OnDeviceDestroy			();
}

void CEnvironment::Invalidate()
{
	bWFX					= false;
	Current[0]				= 0;
	Current[1]				= 0;
	if (eff_LensFlare)		eff_LensFlare->Invalidate();
}

float CEnvironment::TimeDiff(float prev, float cur)
{
	if (prev>cur)	return	(DAY_LENGTH-prev)+cur;
	else			return	cur-prev;
}

float CEnvironment::TimeWeight(float val, float min_t, float max_t)
{
	float weight	= 0.f;
	float length	= TimeDiff(min_t,max_t);
	if (!fis_zero(length,EPS)){
		if (min_t>max_t){
			if ((val>=min_t)||(val<=max_t))	weight = TimeDiff(min_t,val)/length;
		}else{
			if ((val>=min_t)&&(val<=max_t))	weight = TimeDiff(min_t,val)/length;
		}
		clamp		(weight,0.f,1.f);
	}
	return			weight;
}

void CEnvironment::SetGameTime(float game_time, float time_factor)
{ 
	if (bWFX)
		wfx_time			-= TimeDiff(fGameTime,game_time);
	fGameTime				= game_time;  
	fTimeFactor				= time_factor;	
}

float CEnvironment::NormalizeTime(float tm)
{
	if (tm<0.f)				return tm+DAY_LENGTH;
	else if (tm>DAY_LENGTH)	return tm-DAY_LENGTH;
	else					return tm;
}

void CEnvironment::SetWeather(shared_str name, bool forced)
{
//.	static BOOL bAlready = FALSE;
//.	if(bAlready)	return;
	if (name.size())	{
//.		bAlready = TRUE;
        EnvsMapIt it		= WeatherCycles.find(name);
		if (it == WeatherCycles.end())
		{
			Msg("! Invalid weather name: %s", name.c_str());
			return;
		}
        R_ASSERT3			(it!=WeatherCycles.end(),"Invalid weather name.",*name);
		CurrentCycleName	= it->first;
		if (forced)			{Invalidate();			}
		if (!bWFX){
			CurrentWeather		= &it->second;
			CurrentWeatherName	= it->first;
		}
		if (forced)			{SelectEnvs(fGameTime);	}
#ifdef WEATHER_LOGGING
		Msg					("Starting Cycle: %s [%s]",*name,forced?"forced":"deferred");
#endif
    }else{
#ifndef _EDITOR
		FATAL				("! Empty weather name");
#endif
    }
}

bool CEnvironment::SetWeatherFX(shared_str name)
{
	if (bWFX)				return false;
	if (name.size()){
		EnvsMapIt it		= WeatherFXs.find(name);
		R_ASSERT3			(it!=WeatherFXs.end(),"Invalid weather effect name.",*name);
		EnvVec* PrevWeather = CurrentWeather; VERIFY(PrevWeather);
		CurrentWeather		= &it->second;
		CurrentWeatherName	= it->first;

		float rewind_tm		= WFX_TRANS_TIME*fTimeFactor;
		float start_tm		= fGameTime+rewind_tm;
		float current_length;
		float current_weight;
		if (Current[0]->exec_time>Current[1]->exec_time){
			float x			= fGameTime>Current[0]->exec_time?fGameTime-Current[0]->exec_time:(DAY_LENGTH-Current[0]->exec_time)+fGameTime;
			current_length	= (DAY_LENGTH-Current[0]->exec_time)+Current[1]->exec_time;
			current_weight	= x/current_length; 
		}else{
			current_length	= Current[1]->exec_time-Current[0]->exec_time;
			current_weight	= (fGameTime-Current[0]->exec_time)/current_length; 
		}
		clamp				(current_weight,0.f,1.f);

		std::sort			(CurrentWeather->begin(),CurrentWeather->end(),sort_env_etl_pred);
		CEnvDescriptor* C0	= CurrentWeather->at(0);
		CEnvDescriptor* C1	= CurrentWeather->at(1);
		CEnvDescriptor* CE	= CurrentWeather->at(CurrentWeather->size()-2);
		CEnvDescriptor* CT	= CurrentWeather->at(CurrentWeather->size()-1);
		C0->copy			(*Current[0]);	C0->exec_time = NormalizeTime(fGameTime-((rewind_tm/(Current[1]->exec_time-fGameTime))*current_length-rewind_tm));
		C1->copy			(*Current[1]);	C1->exec_time = NormalizeTime(start_tm);
		for (EnvIt t_it=CurrentWeather->begin()+2; t_it!=CurrentWeather->end()-1; t_it++)
			(*t_it)->exec_time= NormalizeTime(start_tm+(*t_it)->exec_time_loaded);
		SelectEnv			(PrevWeather,WFX_end_desc[0],CE->exec_time);
		SelectEnv			(PrevWeather,WFX_end_desc[1],WFX_end_desc[0]->exec_time+0.5f);
		CT->copy			(*WFX_end_desc[0]);CT->exec_time = NormalizeTime(CE->exec_time+rewind_tm);
		wfx_time			= TimeDiff(fGameTime,CT->exec_time);
		bWFX				= true;

		// sort wfx envs
		std::sort			(CurrentWeather->begin(),CurrentWeather->end(),sort_env_pred);

		Current[0]			= C0;
		Current[1]			= C1;
#ifdef WEATHER_LOGGING
		Msg					("Starting WFX: '%s' - %3.2f sec",*name,wfx_time);
		for (EnvIt l_it=CurrentWeather->begin(); l_it!=CurrentWeather->end(); l_it++)
			Msg				(". Env: '%s' Tm: %3.2f",*(*l_it)->sect_name,(*l_it)->exec_time);
#endif
	}else{
#ifndef _EDITOR
		FATAL				("! Empty weather effect name");
#endif
	}
	return true;
}

void CEnvironment::StopWFX	()
{
	VERIFY					(CurrentCycleName.size());
	bWFX					= false;
	SetWeather				(CurrentCycleName,false);
	Current[0]				= WFX_end_desc[0];
	Current[1]				= WFX_end_desc[1];
#ifdef WEATHER_LOGGING
	Msg						("WFX - end. Weather: '%s' Desc: '%s'/'%s' GameTime: %3.2f",CurrentWeatherName.c_str(),Current[0]->sect_name.c_str(),Current[1]->sect_name.c_str(),fGameTime);
#endif
}

IC bool lb_env_pred(const CEnvDescriptor* x, float val)
{	return x->exec_time < val;	}

void CEnvironment::SelectEnv(EnvVec* envs, CEnvDescriptor*& e, float gt)
{
	EnvIt env		= std::lower_bound(envs->begin(),envs->end(),gt,lb_env_pred);
	if (env==envs->end()){
		e			= envs->front();
	}else{
		e			= *env;
	}
}

void CEnvironment::SelectEnvs(EnvVec* envs, CEnvDescriptor*& e0, CEnvDescriptor*& e1, float gt)
{
	EnvIt env		= std::lower_bound(envs->begin(),envs->end(),gt,lb_env_pred);
	if (env==envs->end()){
		e0			= *(envs->end()-1);
		e1			= envs->front();
	}else{
		e1			= *env;
		if (env==envs->begin())	e0 = *(envs->end()-1);
		else					e0 = *(env-1);
	}
}

void CEnvironment::SelectEnvs(float gt)
{
	VERIFY				(CurrentWeather);
    if ((Current[0]==Current[1])&&(Current[0]==0)){
		VERIFY			(!bWFX);
		// first or forced start
		SelectEnvs		(CurrentWeather,Current[0],Current[1],gt);
    }else{
		bool bSelect	= false;
		if (Current[0]->exec_time>Current[1]->exec_time){
			// terminator
			bSelect		= (gt>Current[1]->exec_time)&&(gt<Current[0]->exec_time);
		}else{
			bSelect		= (gt>Current[1]->exec_time);
		}
		if (bSelect){
			Current[0]	= Current[1];
			SelectEnv	(CurrentWeather,Current[1],gt);
#ifdef WEATHER_LOGGING
			Msg			("Weather: '%s' Desc: '%s' Time: %3.2f/%3.2f",CurrentWeatherName.c_str(),Current[1]->sect_name.c_str(),Current[1]->exec_time,fGameTime);
#endif
		}
    }
}

int get_ref_count(IUnknown* ii)
{
	if(ii){
		ii->AddRef();
		return ii->Release();
	}else
	return 0;
}

void CEnvironment::OnFrame()
{
#ifdef _EDITOR
	SetGameTime				(fGameTime+Device.fTimeDelta*fTimeFactor,fTimeFactor);
    if (fsimilar(ed_to_time,DAY_LENGTH)&&fsimilar(ed_from_time,0.f)){
	    if (fGameTime>DAY_LENGTH)	fGameTime-=DAY_LENGTH;
    }else{
	    if (fGameTime>ed_to_time){	
        	fGameTime=fGameTime-ed_to_time+ed_from_time;
            Current[0]=Current[1]=0;
        }
    	if (fGameTime<ed_from_time){	
        	fGameTime=ed_from_time;
            Current[0]=Current[1]=0;
        }
    }
	if (!psDeviceFlags.is(rsEnvironment))		return;
#else
	if (!g_pGameLevel)		return;
#endif

//	if (pInput->iGetAsyncKeyState(DIK_O))		SetWeatherFX("surge_day"); 

	if (bWFX&&(wfx_time<=0.f)) StopWFX();

	SelectEnvs				(fGameTime);
    VERIFY					(Current[0]&&Current[1]);

	float current_weight	= TimeWeight(fGameTime,Current[0]->exec_time,Current[1]->exec_time);

	// modifiers
	CEnvModifier			EM;
	EM.far_plane			= 0;
	EM.fog_color.set		( 0,0,0 );
	EM.fog_density			= 0;
	EM.ambient.set			( 0,0,0 );
	EM.sky_color.set		( 0,0,0 );
	EM.hemi_color.set		( 0,0,0 );
	Fvector	view			= Device.vCameraPosition;
	float	mpower			= 1;
	for (xr_vector<CEnvModifier>::iterator mit=Modifiers.begin(); mit!=Modifiers.end(); mit++)
		mpower				+= EM.sum(*mit,view);

	// final lerp
	CurrentEnv.lerp				(this,*Current[0],*Current[1],current_weight,EM,mpower);
/*	if(CurrentEnv.sun_dir.y>0)
	{
		Log("CurrentEnv.sun_dir", CurrentEnv.sun_dir);
		Log("current_weight", current_weight);
		Log("mpower", mpower);

		Log("Current[0]->sun_dir", Current[0]->sun_dir);
		Log("Current[1]->sun_dir", Current[1]->sun_dir);

	}*/
	VERIFY2						(CurrentEnv.sun_dir.y<0,"Invalid sun direction settings in lerp");

	if (::Render->get_generation()==IRender_interface::GENERATION_R2){
		//. very very ugly hack
		if (HW.Caps.raster_major >= 3 && HW.Caps.geometry.bVTF){
			// tonemapping in VS
			CurrentEnv.sky_r_textures.push_back		(mk_pair(u32(D3DVERTEXTEXTURESAMPLER0),tonemap));	//. hack
			CurrentEnv.sky_r_textures_env.push_back	(mk_pair(u32(D3DVERTEXTEXTURESAMPLER0),tonemap));	//. hack
			CurrentEnv.clouds_r_textures.push_back	(mk_pair(u32(D3DVERTEXTEXTURESAMPLER0),tonemap));	//. hack
		} else {
			// tonemapping in PS
			CurrentEnv.sky_r_textures.push_back		(mk_pair(2,tonemap));								//. hack
			CurrentEnv.sky_r_textures_env.push_back	(mk_pair(2,tonemap));								//. hack
			CurrentEnv.clouds_r_textures.push_back	(mk_pair(2,tonemap));								//. hack
		}
		
	}

	//. Setup skybox textures, somewhat ugly
	IDirect3DBaseTexture9*	e0	= CurrentEnv.sky_r_textures[0].second->surface_get();
	IDirect3DBaseTexture9*	e1	= CurrentEnv.sky_r_textures[1].second->surface_get();
	
	tsky0->surface_set		(e0);	_RELEASE(e0);
	tsky1->surface_set		(e1);	_RELEASE(e1);

	PerlinNoise1D->SetFrequency		(wind_gust_factor*MAX_NOISE_FREQ);
	wind_strength_factor			= clampr(PerlinNoise1D->GetContinious(Device.fTimeGlobal)+0.5f,0.f,1.f); 

    int l_id							=	(current_weight<0.5f)?Current[0]->lens_flare_id:Current[1]->lens_flare_id;
	eff_LensFlare->OnFrame				(l_id);
	int t_id							=	(current_weight<0.5f)?Current[0]->tb_id:Current[1]->tb_id;
    eff_Thunderbolt->OnFrame			(t_id,CurrentEnv.bolt_period,CurrentEnv.bolt_duration);
	eff_Rain->OnFrame					();

	// ******************** Environment params (setting)
	CHK_DX(HW.pDevice->SetRenderState( D3DRS_FOGCOLOR,	color_rgba_f(CurrentEnv.fog_color.x,CurrentEnv.fog_color.y,CurrentEnv.fog_color.z,0) )); 
	CHK_DX(HW.pDevice->SetRenderState( D3DRS_FOGSTART,	*(u32 *)(&CurrentEnv.fog_near)	));
	CHK_DX(HW.pDevice->SetRenderState( D3DRS_FOGEND,	*(u32 *)(&CurrentEnv.fog_far)	));
}

