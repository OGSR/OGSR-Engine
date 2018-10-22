///////////////////////////////////////////////////////////////
// Medkit.cpp
// Medkit - аптечка, повышающая здоровье
///////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "medkit.h"
#include "PhysicsShell.h"

CMedkit::CMedkit(void) 
{
}

CMedkit::~CMedkit(void) 
{
}

BOOL CMedkit::net_Spawn(CSE_Abstract* DC) 
{
	return		(inherited::net_Spawn(DC));
}

void CMedkit::Load(LPCSTR section) 
{
	inherited::Load(section);
}

void CMedkit::net_Destroy() 
{
	inherited::net_Destroy();
}

void CMedkit::shedule_Update(u32 dt) 
{
	inherited::shedule_Update(dt);

}

void CMedkit::UpdateCL() 
{
	inherited::UpdateCL();
}


void CMedkit::OnH_A_Chield() 
{
	inherited::OnH_A_Chield		();
}

void CMedkit::OnH_B_Independent(bool just_before_destroy) 
{
	inherited::OnH_B_Independent(just_before_destroy);
}

void CMedkit::renderable_Render() 
{
	inherited::renderable_Render();
}