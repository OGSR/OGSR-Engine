#include "stdafx.h"
#include "PHShellCreator.h"
#include "PhysicsShell.h"
#include "gameobject.h"
#include "physicsshellholder.h"
#include "../skeletoncustom.h"

void CPHShellSimpleCreator::CreatePhysicsShell()
{
	CPhysicsShellHolder* owner = smart_cast<CPhysicsShellHolder*>(this); VERIFY(owner);
	if (!owner->Visual()) return;
	
	CKinematics* pKinematics		= smart_cast<CKinematics*>(owner->Visual());
	VERIFY							(pKinematics);

	if(owner->PPhysicsShell())		return;
	owner->PPhysicsShell()			= P_create_Shell();
#ifdef DEBUG
	owner->PPhysicsShell()->dbg_obj=owner;
#endif
	owner->m_pPhysicsShell->build_FromKinematics	(pKinematics,0);

	owner->PPhysicsShell()->set_PhysicsRefObject	(owner);
	//m_pPhysicsShell->SmoothElementsInertia(0.3f);
	owner->PPhysicsShell()->mXFORM.set				(owner->XFORM());
	owner->PPhysicsShell()->SetAirResistance		(0.001f, 0.02f);
}