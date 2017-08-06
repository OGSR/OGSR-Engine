#include "stdafx.h"
#include "game_sv_event_queue.h"


// 
GameEventQueue::GameEventQueue()		
#ifdef PROFILE_CRITICAL_SECTIONS
	:cs(MUTEX_PROFILE_ID(GameEventQueue))
#endif // PROFILE_CRITICAL_SECTIONS
{
	unused.reserve	(128);
	for (int i=0; i<16; i++)
		unused.push_back	(xr_new<GameEvent>());
}
GameEventQueue::~GameEventQueue()
{
	cs.Enter		();
	u32				it;
	for				(it=0; it<unused.size(); it++)	xr_delete(unused[it]);
	for				(it=0; it<ready.size(); it++)	xr_delete(ready[it]);
	cs.Leave		();
}

static u32 LastTimeCreate = 0;
GameEvent*		GameEventQueue::Create	()
{
	GameEvent*	ge			= 0;
	cs.Enter		();
	if (unused.empty())	
	{
		ready.push_back		(xr_new<GameEvent> ());
		ge					= ready.back	();
		//---------------------------------------------
#ifdef _DEBUG
//		Msg ("* GameEventQueue::Create - ready %d, unused %d", ready.size(), unused.size());
#endif
		LastTimeCreate = GetTickCount();
		//---------------------------------------------
	} else {
		ready.push_back		(unused.back());
		unused.pop_back		();
		ge					= ready.back	();
	}
	cs.Leave		();
	return	ge;
}
GameEvent*		GameEventQueue::Create	(NET_Packet& P, u16 type, u32 time, ClientID clientID)
{
	GameEvent*	ge			= 0;
	cs.Enter		();
	if (unused.empty())	
	{
		ready.push_back		(xr_new<GameEvent> ());
		ge					= ready.back	();
		//---------------------------------------------
#ifdef _DEBUG
//		Msg ("* GameEventQueue::Create - ready %d, unused %d", ready.size(), unused.size());
#endif
		LastTimeCreate = GetTickCount();
		//---------------------------------------------
	} else {
		ready.push_back		(unused.back());
		unused.pop_back		();
		ge					= ready.back	();
	}
	CopyMemory	(&(ge->P),&P,sizeof(NET_Packet));
	ge->sender	= clientID;
	ge->time	= time;
	ge->type	= type;

	cs.Leave		();
	return			ge;
}
GameEvent*		GameEventQueue::Retreive	()
{
	GameEvent*	ge			= 0;
	cs.Enter		();
	if (!ready.empty())		ge = ready.front();
	//---------------------------------------------	
	else
	{
		u32 tmp_time = GetTickCount()-60000;
		u32 size = unused.size();
		if ((LastTimeCreate < tmp_time) &&  (size > 32))
		{
			xr_delete(unused.back());
			unused.pop_back();
#ifdef _DEBUG
//			Msg ("GameEventQueue::Retreive - ready %d, unused %d", ready.size(), unused.size());
#endif
		}		
	}
	//---------------------------------------------	
	cs.Leave		();
	return	ge;
}

void			GameEventQueue::Release	()
{
	cs.Enter		();
	R_ASSERT		(!ready.empty());
	//---------------------------------------------
	u32 tmp_time = GetTickCount()-60000;
	u32 size = unused.size();
	if ((LastTimeCreate < tmp_time) &&  (size > 32))
	{
		xr_delete(ready.front());
#ifdef _DEBUG
//		Msg ("GameEventQueue::Release - ready %d, unused %d", ready.size(), unused.size());
#endif
	}
	else
		unused.push_back(ready.front());
	//---------------------------------------------		
	ready.pop_front	();
	cs.Leave		();
}
