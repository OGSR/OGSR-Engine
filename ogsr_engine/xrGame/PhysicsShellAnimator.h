#pragma once

#include "PhysicsShell.h"
#include "PhysicsShellAnimatorBoneData.h"
class animation_movement_controller;
class CPhysicsShellAnimator
{
	friend class CPhysicsShellAnimatorBoneData;
	xr_vector<CPhysicsShellAnimatorBoneData>		m_bones_data;
	CPhysicsShell*									m_pPhysicsShell;


public:
													CPhysicsShellAnimator		(CPhysicsShell* _pPhysicsShell);
													~CPhysicsShellAnimator		();
	void											OnFrame						();
};
	

