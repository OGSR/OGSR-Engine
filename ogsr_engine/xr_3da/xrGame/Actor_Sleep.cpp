#include "stdafx.h"
/*
#include "actor.h"
#include "SleepEffector.h"
#include "game_sv_single.h"
#include "alife_simulator.h"
#include "level.h"
#include "xrmessages.h"
#include "game_cl_base.h"
#include "xrServer.h"
#include "autosave_manager.h"
#include "ai_space.h"
#include "actorcondition.h"
#include "ui\UIVideoPlayerWnd.h"

#define ONLINE_RADIUS				2.f
#define MIN_SPRING_TO_SLEEP			0.8f	

namespace ACTOR_DEFS{
	const char * easCanSleepResult		= "can_sleep";
};

void CActor::UpdateSleep()
{
	if(!conditions().IsSleeping()) return;


	VERIFY(this == smart_cast<CActor*>(Level().CurrentEntity()));
	VERIFY(m_pSleepEffectorPP);

	if(CSleepEffectorPP::BEFORE_SLEEPING == m_pSleepEffectorPP->m_eSleepState)
	{
		m_fOldTimeFactor = Level().GetGameTimeFactor();
		Level().Server->game->SetGameTimeFactor(m_fSleepTimeFactor);
		
		if (ai().get_alife()) {
			m_fOldOnlineRadius = ai().alife().switch_distance();
			NET_Packet		P;
			P.w_begin		(M_SWITCH_DISTANCE);
			P.w_float		(ONLINE_RADIUS);
			Level().Send	(P,net_flags(TRUE,TRUE));
		}

		m_pSleepEffectorPP->m_eSleepState = CSleepEffectorPP::SLEEPING;
		
		VERIFY(!conditions().m_actor_sleep_wnd->IsPlaying());
		
		conditions().m_actor_sleep_wnd->Play		();
		conditions().m_actor_sleep_wnd->Activate	();
	}


	//разбудить актера, если он проспал столько сколько задумал

	if(Level().GetGameTime()>m_dwWakeUpTime){
		Level().Server->game->SetGameTimeFactor(m_fOldTimeFactor);

		if( !conditions().m_actor_sleep_wnd->IsPlaying())
			conditions().Awoke();
	}
}*/