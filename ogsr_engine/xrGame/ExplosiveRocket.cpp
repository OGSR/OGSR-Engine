//////////////////////////////////////////////////////////////////////
// ExplosiveRocket.cpp:	ракета, которой стреляет RocketLauncher 
//						взрывается при столкновении
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "ExplosiveRocket.h"


CExplosiveRocket::CExplosiveRocket() 
{
}

CExplosiveRocket::~CExplosiveRocket() 
{
}

DLL_Pure *CExplosiveRocket::_construct	()
{
	CCustomRocket::_construct	();
	CInventoryItem::_construct	();
	return						(this);
}

void CExplosiveRocket::Load(LPCSTR section) 
{
	inherited::Load(section);
	CInventoryItem::Load(section);
	CExplosive::Load(section);
}

BOOL CExplosiveRocket::net_Spawn(CSE_Abstract* DC) 
{
	BOOL result = inherited::net_Spawn(DC);
	result=result&&CInventoryItem::net_Spawn(DC);
	Fvector box;BoundingBox().getsize(box);
	float max_size=_max(_max(box.x,box.y),box.z);
	box.set(max_size,max_size,max_size);
	box.mul(3.f);
	CExplosive::SetExplosionSize(box);
	return result;
}

void CExplosiveRocket::Contact(const Fvector &pos, const Fvector &normal)
{
	if(eCollide == m_eState) return;

	if(m_bLaunched)
		CExplosive::GenExplodeEvent(pos,normal);

	inherited::Contact(pos, normal);
}

void CExplosiveRocket::net_Destroy() 
{
	CInventoryItem::net_Destroy();
	CExplosive::net_Destroy();
	inherited::net_Destroy();
}

void CExplosiveRocket::OnH_A_Independent() 
{
	inherited::OnH_A_Independent();
}

void CExplosiveRocket::OnH_B_Independent(bool just_before_destroy) 
{
	CInventoryItem::OnH_B_Independent(just_before_destroy);
	inherited::OnH_B_Independent(just_before_destroy);
}

void CExplosiveRocket::UpdateCL() 
{
	if(eCollide == m_eState)
	{
		CExplosive::UpdateCL();
		inherited::UpdateCL();
	}
	else
		inherited::UpdateCL();
}


void  CExplosiveRocket::OnEvent (NET_Packet& P, u16 type) 
{
	CExplosive::OnEvent(P, type);
	inherited::OnEvent(P,type);
}


void CExplosiveRocket::make_Interpolation ()
{
	inherited::make_Interpolation();
}

void CExplosiveRocket::PH_B_CrPr			()
{
	inherited::PH_B_CrPr		();
}

void CExplosiveRocket::PH_I_CrPr			()
{
	inherited::PH_I_CrPr		();
}

void CExplosiveRocket::PH_A_CrPr			()
{
	inherited::PH_A_CrPr		();
}

#ifdef DEBUG
void CExplosiveRocket::PH_Ch_CrPr			()
{
	inherited::PH_Ch_CrPr		();
}

void CExplosiveRocket::OnRender				()
{
	inherited::OnRender			();
}
#endif

void CExplosiveRocket::reinit				()
{
	inherited::reinit			();
	CInventoryItem::reinit			();
}

void CExplosiveRocket::reload					(LPCSTR section)
{
	inherited::reload				(section);
	CInventoryItem::reload			(section);
}

void CExplosiveRocket::activate_physic_shell	()
{
	inherited::activate_physic_shell();
}

void CExplosiveRocket::on_activate_physic_shell	()
{
	CCustomRocket::activate_physic_shell();
}

void CExplosiveRocket::setup_physic_shell		()
{
	inherited::setup_physic_shell();
}

void CExplosiveRocket::create_physic_shell		()
{
	inherited::create_physic_shell();
}

bool CExplosiveRocket::Useful					() const
{
	return			(inherited::Useful());
}
void CExplosiveRocket::net_Relcase(CObject* O )
{
	CExplosive::net_Relcase(O);
	inherited::net_Relcase(O);
}