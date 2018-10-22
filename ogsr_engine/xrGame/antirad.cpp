///////////////////////////////////////////////////////////////
// Antirad.cpp
// Antirad - таблетки выводящие радиацию
///////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "antirad.h"
#include "PhysicsShell.h"

CAntirad::CAntirad(void) 
{
}

CAntirad::~CAntirad(void) 
{
}


BOOL CAntirad::net_Spawn(CSE_Abstract* DC) 
{
	return		(inherited::net_Spawn(DC));
}

void CAntirad::Load(LPCSTR section) 
{
	inherited::Load(section);
}

void CAntirad::net_Destroy() 
{
	inherited::net_Destroy();
}

void CAntirad::shedule_Update(u32 dt) 
{
	inherited::shedule_Update(dt);
}

void CAntirad::UpdateCL() 
{
	inherited::UpdateCL();
}

void CAntirad::OnH_A_Chield() 
{
	inherited::OnH_A_Chield		();
}

void CAntirad::OnH_B_Independent(bool just_before_destroy) 
{
	inherited::OnH_B_Independent(just_before_destroy);
}

void CAntirad::renderable_Render() 
{
	inherited::renderable_Render();
}