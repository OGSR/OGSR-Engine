#include "StdAfx.h"
#include "PhysicsShellAnimator.h"
#include "PhysicsShellAnimatorBoneData.h"
#include "../SkeletonCustom.h"
#include "PHDynamicData.h"
#include "game_object_space.h"
#include "PhysicsShellHolder.h"


ICF bool no_physics_shape( const SBoneShape& shape );

CPhysicsShellAnimator::CPhysicsShellAnimator( CPhysicsShell* _pPhysicsShell ) : m_pPhysicsShell( _pPhysicsShell ) 
{
	
	for (xr_vector<CPHElement*>::iterator i=m_pPhysicsShell->Elements().begin();i!=m_pPhysicsShell->Elements().end();i++)
	{
		CPhysicsShellAnimatorBoneData PhysicsShellAnimatorBoneDataC;
		PhysicsShellAnimatorBoneDataC.m_element=*i;
		CBoneInstance& B=m_pPhysicsShell->PKinematics()->LL_GetBoneInstance(PhysicsShellAnimatorBoneDataC.m_element->m_SelfID);
		B.reset_callback();
		PhysicsShellAnimatorBoneDataC.m_anim_fixed_dJointID=dJointCreateFixed(0,0);
		((CPHShell*)(m_pPhysicsShell))->Island().DActiveIsland()->AddJoint(PhysicsShellAnimatorBoneDataC.m_anim_fixed_dJointID);				
		dJointAttach(PhysicsShellAnimatorBoneDataC.m_anim_fixed_dJointID,PhysicsShellAnimatorBoneDataC.m_element->get_body(),0);
		dJointSetFixed(PhysicsShellAnimatorBoneDataC.m_anim_fixed_dJointID);
		m_bones_data.push_back(PhysicsShellAnimatorBoneDataC);
	}

	for (u16 i=0;i<m_pPhysicsShell->get_JointsNumber();i++)
	{
		((CPHShell*)(m_pPhysicsShell))->DeleteJoint(i);
	}

}

CPhysicsShellAnimator::~CPhysicsShellAnimator()
{
	for (xr_vector<CPhysicsShellAnimatorBoneData>::iterator i=m_bones_data.begin();i!=m_bones_data.end();i++)
	{
		((CPHShell*)(m_pPhysicsShell))->Island().DActiveIsland()->RemoveJoint(i->m_anim_fixed_dJointID);
		dJointDestroy(i->m_anim_fixed_dJointID);
	}
}

void CPhysicsShellAnimator::OnFrame()
{

	
	m_pPhysicsShell->Enable();
	
	for (xr_vector<CPhysicsShellAnimatorBoneData>::iterator i=m_bones_data.begin();i!=m_bones_data.end();i++)
	{
		Fmatrix target_obj_posFmatrixS;
		CBoneInstance& B=m_pPhysicsShell->PKinematics()->LL_GetBoneInstance(i->m_element->m_SelfID);
		
		target_obj_posFmatrixS.mul_43((*(m_pPhysicsShell->Elements().begin()))->PhysicsRefObject()->XFORM(),B.mTransform);
				
		Fmatrix parent;
		parent.invert		(m_pPhysicsShell->mXFORM);
		B.mTransform.mul_43(parent,i->m_element->mXFORM);//restore actual physic position for display
		
		dQuaternion target_obj_quat_dQuaternionS;
		dMatrix3 ph_mat;
		PHDynamicData::FMXtoDMX(target_obj_posFmatrixS,ph_mat);
		dQfromR(target_obj_quat_dQuaternionS,ph_mat);
		Fvector mc;
		i->m_element->CPHGeometryOwner::get_mc_vs_transform(mc,target_obj_posFmatrixS);
		dJointSetFixedQuaternionPos(i->m_anim_fixed_dJointID,target_obj_quat_dQuaternionS,&mc.x);
	}
	(*(m_pPhysicsShell->Elements().begin()))->PhysicsRefObject()->XFORM().set(m_pPhysicsShell->mXFORM);
}