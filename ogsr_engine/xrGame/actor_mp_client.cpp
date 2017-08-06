#include "stdafx.h"
#include "actor_mp_client.h"
#include "actorcondition.h"

CActorMP::CActorMP			()
{
	m_i_am_dead				= false;
}

void CActorMP::OnEvent		( NET_Packet &P, u16 type)
{
	inherited::OnEvent		(P,type);
}

void CActorMP::Die			(CObject *killer)
{
	m_i_am_dead				= true;
	conditions().health()	= 0.f;
	inherited::Die			(killer);

	if(OnServer())
	{ //transfer all items to bag
	
	}
}
