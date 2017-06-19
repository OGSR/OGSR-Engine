/////////////////////////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "phcharacter.h"
#include "Physics.h"
#include "ExtendedGeom.h"
#include "PHCapture.h"
#include "Entity.h"
#include "inventory_item.h"
#include "../skeletoncustom.h"
#include "Actor.h"
#include "Inventory.h"
extern	class CPHWorld	*ph_world;
///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
CPHCapture::CPHCapture	(CPHCharacter   *a_character, CPhysicsShellHolder	*a_taget_object)
{
	CPHUpdateObject::Activate();

	m_joint					=NULL;	
	m_ajoint				=NULL;
	m_body					=NULL;
	m_taget_object			=NULL;
	m_character				=NULL;
	b_failed				=false;
	b_disabled				=false;	
	b_character_feedback	=false;
	e_state					=cstPulling;
	
	if(!a_taget_object							||
	   !a_taget_object->m_pPhysicsShell			||
	   !a_taget_object->m_pPhysicsShell->isActive()||
	   smart_cast<CInventoryItem*>(a_taget_object)
	   ) 
	{
		m_taget_object=NULL;
		b_failed=true;
		return;
	}

	if(!a_character||!a_character->b_exist)
	{
		m_taget_object=NULL;
		b_failed=true;
		return;
	}
	m_taget_object			=a_taget_object;
	m_character				=a_character;

	CObject* capturer_object=smart_cast<CObject*>(m_character->PhysicsRefObject());

	if(!capturer_object)
	{
		m_taget_object=NULL;
		b_failed=true;
		return;
	}

	CKinematics* p_kinematics=smart_cast<CKinematics*>(capturer_object->Visual());

	if(!p_kinematics)
	{
		m_taget_object=NULL;
		b_failed=true;
		return;
	}

	CInifile* ini=p_kinematics->LL_UserData();

	if(!ini)
	{
		m_taget_object=NULL;
		b_failed=true;
		return;
	}

	if(!ini->section_exist("capture"))
	{
		m_taget_object=NULL;
		b_failed=true;
		return;
	}
	u16 capture_bone_id=p_kinematics->LL_BoneID(ini->r_string("capture","bone"));
	R_ASSERT2(capture_bone_id!=BI_NONE,"wrong capture bone");
	m_capture_bone=&p_kinematics->LL_GetBoneInstance(capture_bone_id);
		


	m_taget_element					=m_taget_object->m_pPhysicsShell->NearestToPoint(m_capture_bone->mTransform.c);

	Init(ini);

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
CPHCapture::CPHCapture(CPHCharacter   *a_character,CPhysicsShellHolder	*a_taget_object,u16 a_taget_element)
{

	CPHUpdateObject::Activate();
	m_joint					=NULL;	
	m_ajoint				=NULL;
	m_body					=NULL;
	b_failed				=false;
	b_disabled				=false;
	e_state					=cstPulling;
	b_character_feedback	=false;
	m_taget_object			=NULL;
	m_character				=NULL;
	if(!a_taget_object								||
	   !a_taget_object->m_pPhysicsShell				||
	   !a_taget_object->m_pPhysicsShell->isActive()	||
	   smart_cast<CInventoryItem*>(a_taget_object)
	   ) 
	{
		m_taget_object=NULL;
		b_failed=true;
		return;
	}

	if(!a_character||!a_character->b_exist)
	{
		m_taget_object=NULL;
		b_failed=true;
		return;
	}
	m_taget_object			=a_taget_object;
	m_character				=a_character;

	CObject* capturer_object=smart_cast<CObject*>(m_character->PhysicsRefObject());

	if(!capturer_object)
	{
		m_taget_object=NULL;
		b_failed=true;
		return;
	}

	CKinematics* p_kinematics=smart_cast<CKinematics*>(capturer_object->Visual());

	if(!p_kinematics)
	{
		m_taget_object=NULL;
		b_failed=true;
		return;
	}

	CInifile* ini=p_kinematics->LL_UserData();

	if(!ini)
	{
		m_taget_object=NULL;
		b_failed=true;
		return;
	}

	if(a_taget_element==BI_NONE)
	{
		m_taget_object=NULL;
		b_failed=true;
		return;
	}

	if(!ini->section_exist("capture"))
	{
		m_taget_object=NULL;
		b_failed=true;
		return;
	}
	
	u16 capture_bone_id=p_kinematics->LL_BoneID(ini->r_string("capture","bone"));
	R_ASSERT2(capture_bone_id!=BI_NONE,"wrong capture bone");
	m_capture_bone=&p_kinematics->LL_GetBoneInstance(capture_bone_id);
		


	IRender_Visual* V=m_taget_object->Visual();

	if(!V)
	{
		m_taget_object=NULL;
		b_failed=true;
		return;
	}

	CKinematics* K=	smart_cast<CKinematics*>(V);

	if(!K)
	{
		m_taget_object=NULL;
		b_failed=true;
		return;
	}

	CBoneInstance& tag_bone=K->LL_GetBoneInstance(a_taget_element);

	if(!tag_bone.Callback_Param)
	{
		m_taget_object=NULL;
		b_failed=true;
		return;
	}

	m_taget_element					=(CPhysicsElement*)tag_bone.Callback_Param;

	if(!m_taget_element)
	{
		m_taget_object=NULL;
		b_failed=true;
		return;
	}

	Init(ini);

}



void CPHCapture::Init(CInifile* ini)
{
	Fvector dir;
	Fvector capture_bone_position;
	capture_bone_position.set(m_capture_bone->mTransform.c);
	b_character_feedback=true;
	(m_character->PhysicsRefObject())->XFORM().transform_tiny(capture_bone_position);


	m_taget_element->GetGlobalPositionDynamic(&dir);
	dir.sub(capture_bone_position,dir);


	m_pull_distance=ini->r_float("capture","pull_distance");
	if(dir.magnitude()>m_pull_distance)
	{
		m_taget_object=NULL;
		b_failed=true;
		return;
	}

	float 					pool_force_factor=4.f;

	m_capture_distance		=ini->r_float("capture","distance");					//distance
	m_capture_force			=ini->r_float("capture","capture_force");				//capture force
	m_capture_time			=ini->r_u32("capture","time_limit")*1000;				//time;		
	m_time_start			=Device.dwTimeGlobal;
	float max_pull_force    =ini->r_float("capture","pull_force");					//pull force
	m_pull_force			=pool_force_factor*ph_world->Gravity()*m_taget_element->PhysicsShell()->getMass();
	if(m_pull_force>max_pull_force) m_pull_force=max_pull_force;



	float pulling_vel_scale =ini->r_float("capture","velocity_scale");				//
	m_taget_element->set_DynamicLimits(default_l_limit*pulling_vel_scale,default_w_limit*pulling_vel_scale);
	//m_taget_element->PhysicsShell()->set_ObjectContactCallback(object_contactCallbackFun);
	m_character->SetObjectContactCallback(object_contactCallbackFun);
	m_island.Init();
	CActor* A=smart_cast<CActor*>(m_character->PhysicsRefObject());
	if(A)
	{
		A->SetWeaponHideState(INV_STATE_BLOCK_ALL,true);
	}
}

void CPHCapture::Release()
{
	if(b_failed) return;
	if(e_state==cstReleased) return;
	if(m_joint) 
	{
		m_island.RemoveJoint(m_joint);

		dJointDestroy(m_joint);

	}
	m_joint=NULL;
	if(m_ajoint)
	{
		m_island.RemoveJoint(m_ajoint);
		dJointDestroy(m_ajoint);
	}
	m_ajoint=NULL;
	if(m_body) 
	{
		m_island.RemoveBody(m_body);
		dBodyDestroy(m_body);
	}
	m_body=NULL;

	if(e_state==cstPulling&&m_taget_element&&!m_taget_object->getDestroy()&&m_taget_object->PPhysicsShell()&&m_taget_object->PPhysicsShell()->isActive())
	{
		m_taget_element->set_DynamicLimits();
	}

	e_state=cstReleased;
	b_collide=true;
	CActor* A=smart_cast<CActor*>(m_character->PhysicsRefObject());
	if(A)
	{
		A->SetWeaponHideState(INV_STATE_BLOCK_ALL,false);
//.		A->inventory().setSlotsBlocked(false);
	}
}

void CPHCapture::Deactivate()
{
	Release();
	//if(m_taget_object&&m_taget_element&&!m_taget_object->getDestroy()&&m_taget_object->m_pPhysicsShell&&m_taget_object->m_pPhysicsShell->isActive())
	//{
	//	m_taget_element->set_ObjectContactCallback(0);

	//}
	if(m_character)m_character->SetObjectContactCallback(0);
	CPHUpdateObject::Deactivate();
	m_character		=NULL;
	m_taget_object	=NULL;
	m_taget_element	=NULL;
}