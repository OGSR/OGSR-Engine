#include "stdafx.h"


#include "IGame_Persistent.h"
#include "environment.h"
#	include "x_ray.h"
#	include "IGame_Level.h"
#	include "XR_IOConsole.h"
#	include "Render.h"
#	include "ps_instance.h"
#	include "CustomHUD.h"

ENGINE_API	IGame_Persistent*		g_pGamePersistent	= NULL;

bool IsMainMenuActive() { return  g_pGamePersistent && g_pGamePersistent->m_pMainMenu && g_pGamePersistent->m_pMainMenu->IsActive(); }

IGame_Persistent::IGame_Persistent	()
{
	Device.seqAppStart.Add			(this);
	Device.seqAppEnd.Add			(this);
	Device.seqFrame.Add				(this,REG_PRIORITY_HIGH+1);
	Device.seqAppActivate.Add		(this);
	Device.seqAppDeactivate.Add		(this);

	m_pMainMenu						= NULL;

	pEnvironment					= xr_new<CEnvironment>();

	m_pGShaderConstants = ShadersExternalData(); //--#SM+#--
}

IGame_Persistent::~IGame_Persistent	()
{
	Device.seqFrame.Remove			(this);
	Device.seqAppStart.Remove		(this);
	Device.seqAppEnd.Remove			(this);
	Device.seqAppActivate.Remove	(this);
	Device.seqAppDeactivate.Remove	(this);
	xr_delete						(pEnvironment);
}

void IGame_Persistent::OnAppActivate		()
{
}

void IGame_Persistent::OnAppDeactivate		()
{
}

void IGame_Persistent::OnAppStart	()
{
	Environment().load();
}

void IGame_Persistent::OnAppEnd		()
{
	Environment().unload				();
	OnGameEnd						();

	DEL_INSTANCE					(g_hud);
}


void IGame_Persistent::PreStart		(LPCSTR op)
{
	string256						prev_type;
	params							new_game_params;
	strcpy_s							(prev_type,m_game_params.m_game_type);
	new_game_params.parse_cmd_line	(op);

	// change game type
	if (0!=xr_strcmp(prev_type,new_game_params.m_game_type)){
		OnGameEnd					();
	}
}
void IGame_Persistent::Start		(LPCSTR op)
{
	string256						prev_type;
	strcpy_s							(prev_type,m_game_params.m_game_type);
	m_game_params.parse_cmd_line	(op);
	// change game type
	if ((0!=xr_strcmp(prev_type,m_game_params.m_game_type))) 
	{
		if (*m_game_params.m_game_type)
			OnGameStart					();
		if(g_hud)
			DEL_INSTANCE			(g_hud);
	}
	else UpdateGameType();

	VERIFY							(ps_destroy.empty());
}

void IGame_Persistent::Disconnect	()
{
	// clear "need to play" particles
	destroy_particles(true);

	if(g_hud)
		g_hud->OnDisconnected();

	// Kill object - save memory
	ObjectPool.clear();
	Render->models_Clear(TRUE); // У нас вызывается ещё и в CLevel::remove_objects() Если что - убрать оттуда, пусть будет тут.
}

void IGame_Persistent::OnGameStart()
{
//KRodin: префетчинг выключен ввиду своей бесполезности и прожорливости.
/*
	LoadTitle								("st_prefetching_objects");
	if (strstr(Core.Params,"-noprefetch"))	return;

	// prefetch game objects & models
	float	p_time		=			1000.f*Device.GetTimerGlobal()->GetElapsed_sec();
	u32	mem_0			=			Memory.mem_usage()	;

	Log				("Loading objects...");
	ObjectPool.prefetch					();
	Log				("Loading models...");
	Render->models_Prefetch				();
	Device.Resources->DeferredUpload	();

	p_time				=			1000.f*Device.GetTimerGlobal()->GetElapsed_sec() - p_time;
	u32		p_mem		=			Memory.mem_usage() - mem_0	;

	Msg					("* [prefetch] time:    %d ms",	iFloor(p_time));
	Msg					("* [prefetch] memory:  %dKb",	p_mem/1024);
*/
}

void IGame_Persistent::OnGameEnd	()
{
	ObjectPool.clear					();
	Render->models_Clear				(TRUE);
}

void IGame_Persistent::OnFrame		()
{

	if(!Device.Paused() || Device.dwPrecacheFrame)
		Environment().OnFrame				();

	Device.Statistic->Particles_starting= ps_needtoplay.size	();
	Device.Statistic->Particles_active	= ps_active.size		();
	Device.Statistic->Particles_destroy	= ps_destroy.size		();

	// Play req particle systems
	while (!ps_needtoplay.empty())
	{
		auto& psi = ps_needtoplay.back();
		ps_needtoplay.pop_back();
		psi->Play();
	}
	// Destroy inactive particle systems
	while (!ps_destroy.empty())
	{
		auto& psi = *ps_destroy.begin();
		R_ASSERT(psi);
		if (psi->Locked())
		{
			Log("--locked");
			break;
		}
		psi->PSI_internal_delete();
	}
}

void IGame_Persistent::destroy_particles(const bool& all_particles)
{
	ps_needtoplay.clear();

	while (!ps_destroy.empty())
	{
		auto& psi = *ps_destroy.begin();
		R_ASSERT(psi);
		VERIFY(!psi->Locked());
		psi->PSI_internal_delete();
	}

	// delete active particles
	if (all_particles)
	{
		while (!ps_active.empty())
			(*ps_active.begin())->PSI_internal_delete();
	}
	else
	{
		size_t processed = 0;
		auto iter = ps_active.rbegin();
		while (iter != ps_active.rend()) {
			const auto size = ps_active.size();

			auto& object = *(iter++);

			if (object->destroy_on_game_load())
				object->PSI_internal_delete();

			if (size != ps_active.size()) {
				iter = ps_active.rbegin();
				std::advance(iter, processed);
			}
			else {
				processed++;
			}
		}
	}
}
