#include "stdafx.h"
#include "PHSkeleton.h"
#include "PhysicsShellHolder.h"
#include "xrServer_Objects_ALife.h"
#include "Level.h"
#include "PHDefs.h"
#include "PhysicsShell.h"
#include "PHSynchronize.h"
#include "MathUtils.h"
#include "../skeletoncustom.h"
#include "PHObject.h"
#include "PHCollideValidator.h"
#include "ai_object_location.h"
#include "ai_space.h"
#include "game_graph.h"
#include "PHDestroyable.h"
#define F_MAX         3.402823466e+38F

u32 CPHSkeleton::existence_time=5000;

bool IC CheckObjectSize(CKinematics* K)
{
	u16 bcount=K->LL_BoneCount();
	for(u16 i=0;i<bcount;++i)
	{
		if(K->LL_GetBoneVisible(i))
		{
			Fobb obb=K->LL_GetBox(i);
			if(check_obb_sise(obb))return true;
		}
	}
	return false;
}

CPHSkeleton::CPHSkeleton()
{
		Init();
}

CPHSkeleton::~CPHSkeleton()
{
		ClearUnsplited();
}

void CPHSkeleton::RespawnInit()
{
	CKinematics*	K	=	smart_cast<CKinematics*>(PPhysicsShellHolder()->Visual());
	if(K)
	{
		K->LL_SetBoneRoot(0);
		K->LL_SetBonesVisibleAll();
		K->CalculateBones_Invalidate();
		K->CalculateBones();
	}
	Init();
	ClearUnsplited();
}

void CPHSkeleton::Init()
{
	m_remove_time = u32(-1);
	b_removing=false;
	m_startup_anim=NULL;
}

bool CPHSkeleton::Spawn(CSE_Abstract *D)
{
	
	CSE_PHSkeleton *po		= smart_cast<CSE_PHSkeleton*>(D);
	VERIFY					(po);

	m_flags					= po->_flags;
	CSE_Visual				*visual = smart_cast<CSE_Visual*>(D);
	VERIFY					(visual);
	m_startup_anim			= visual->startup_animation;

	if(po->_flags.test(CSE_PHSkeleton::flSpawnCopy))
	{
		CPHSkeleton* source=smart_cast<CPHSkeleton*>(Level().Objects.net_Find(po->source_id));
		R_ASSERT2(source,"no source");
		source->UnsplitSingle(this);
		m_flags.set				(CSE_PHSkeleton::flSpawnCopy,FALSE);
		po->_flags.set				(CSE_PHSkeleton::flSpawnCopy,FALSE);
		po->source_id				=BI_NONE;
		return true;
	}
	else 
	{
		CPhysicsShellHolder	*obj	=	PPhysicsShellHolder();
		CKinematics			*K		=	NULL;
		if (obj->Visual())
		{
			K= smart_cast<CKinematics*>(obj->Visual());
			if(K)
			{
				K->LL_SetBoneRoot(po->saved_bones.root_bone);
				K->LL_SetBonesVisible(po->saved_bones.bones_mask);
			}
		}
		SpawnInitPhysics(D);
		RestoreNetState(po);
		if(obj->PPhysicsShell()&&obj->PPhysicsShell()->isFullActive())
			obj->PPhysicsShell()->GetGlobalTransformDynamic(&obj->XFORM());
		
		CPHDestroyableNotificate::spawn_notificate(D);

		if(K)
		{
			CInifile* ini=K->LL_UserData();
			if(ini&&ini->section_exist("collide"))
			{
				if(ini->line_exist("collide","not_collide_parts"))
				{
					CGID gr= CPHCollideValidator::RegisterGroup();
					obj->PPhysicsShell()->RegisterToCLGroup(gr);
				}
			}
			if(ini&&ini->section_exist("collide_parts"))
			{
				if(ini->line_exist("collide_parts","small_object"))
				{
					obj->PPhysicsShell()->SetSmall();
				}
				if(ini->line_exist("collide_parts","ignore_small_objects"))
				{
					obj->PPhysicsShell()->SetIgnoreSmall();
				}
			}
		}
	}
	return false;
}

void CPHSkeleton::Load(LPCSTR section)
{
		existence_time= pSettings->r_u32(section,"remove_time")*1000;
}

void CPHSkeleton::Update(u32 dt)
{
	CPhysicsShellHolder* obj=PPhysicsShellHolder();
	CPhysicsShell* pPhysicsShell=obj->PPhysicsShell();
	if ( pPhysicsShell && pPhysicsShell->isFractured()) //!ai().get_alife() &&
	{
		PHSplit();
	}

	if(b_removing&&
		Device.dwTimeGlobal>m_remove_time&&
		//(Device.dwTimeGlobal-m_unsplit_time)*phTimefactor>remove_time&&
		m_unsplited_shels.empty()) 
	{
		if (obj->Local())	obj->DestroyObject	();
		b_removing=false;
	}

}
void CPHSkeleton::SaveNetState(NET_Packet& P)
{

	CPhysicsShellHolder* obj=PPhysicsShellHolder();
	CPhysicsShell* pPhysicsShell=obj->PPhysicsShell();
	CKinematics* K	=smart_cast<CKinematics*>(obj->Visual());
	if(pPhysicsShell&&pPhysicsShell->isActive())			m_flags.set(CSE_PHSkeleton::flActive,pPhysicsShell->isEnabled());

	P.w_u8 (m_flags.get());
	if(K)
	{
		VisMask _vm = K->LL_GetBonesVisible();
		P.w_u64(_vm._visimask.flags);
		P.w_u64(_vm._visimask_ex.flags);
//		P.w_u64(K->LL_GetBonesVisible());
		P.w_u16(K->LL_GetBoneRoot());
	}
	else
	{
		P.w_u64(u64(-1));
		P.w_u64(u64(-1));
		P.w_u16(0);
	}
	/////////////////////////////
	Fvector min,max;

	min.set(F_MAX,F_MAX,F_MAX);
	max.set(-F_MAX,-F_MAX,-F_MAX);
	/////////////////////////////////////

	u16 bones_number=obj->PHGetSyncItemsNumber();
	for(u16 i=0;i<bones_number;i++)
	{
		SPHNetState state;
		obj->PHGetSyncItem(i)->get_State(state);
		Fvector& p=state.position;
		if(p.x<min.x)min.x=p.x;
		if(p.y<min.y)min.y=p.y;
		if(p.z<min.z)min.z=p.z;

		if(p.x>max.x)max.x=p.x;
		if(p.y>max.y)max.y=p.y;
		if(p.z>max.z)max.z=p.z;
	}

	min.sub(2.f*EPS_L);
	max.add(2.f*EPS_L);

	P.w_vec3(min);
	P.w_vec3(max);

	P.w_u16(bones_number);

	for(u16 i=0;i<bones_number;i++)
	{
		SPHNetState state;
		obj->PHGetSyncItem(i)->get_State(state);
		state.net_Save(P,min,max);
	}
}

void CPHSkeleton::LoadNetState(NET_Packet& P)
{
	CPhysicsShellHolder* obj=PPhysicsShellHolder();
	CKinematics* K=smart_cast<CKinematics*>(obj->Visual());
	P.r_u8 (m_flags.flags);
	if(K)
	{
		u64 _low = P.r_u64();
		u64 _high = P.r_u64();
		VisMask _vm(_low, _high);
		K->LL_SetBonesVisible(_vm);
		K->LL_SetBoneRoot(P.r_u16());
	}

	u16 bones_number=P.r_u16();
	for(u16 i=0;i<bones_number;i++)
	{
		SPHNetState state;
		state.net_Load(P);
		obj->PHGetSyncItem(i)->set_State(state);
	}
}

void CPHSkeleton::RestoreNetState(CSE_PHSkeleton* po)
{
	if(!po->_flags.test(CSE_PHSkeleton::flSavedData))return;
	CPhysicsShellHolder* obj=PPhysicsShellHolder();
	PHNETSTATE_VECTOR& saved_bones=po->saved_bones.bones;
	PHNETSTATE_I i=saved_bones.begin(),e=saved_bones.end();
	if(obj->PPhysicsShell()&&obj->PPhysicsShell()->isActive())
	{
		obj->PPhysicsShell()->Disable();
	}
	for(u16 bone=0;e!=i;i++,bone++)
	{
		R_ASSERT(bone<obj->PHGetSyncItemsNumber());
		obj->PHGetSyncItem(bone)->set_State(*i);
	}
	saved_bones.clear();
	po->_flags.set(CSE_PHSkeleton::flSavedData,FALSE);
	m_flags.set(CSE_PHSkeleton::flSavedData,FALSE);
}


void CPHSkeleton::ClearUnsplited()
{
	SHELL_PAIR_I i=m_unsplited_shels.begin(),e=m_unsplited_shels.end();
	for(;i!=e;++i)
	{
		i->first->Deactivate();
		xr_delete(i->first);
	}
	m_unsplited_shels.clear();
}

void CPHSkeleton::SpawnCopy()
{
	if(PPhysicsShellHolder()->Local()) {
		CSE_Abstract*				D	= F_entity_Create("ph_skeleton_object");//*cNameSect()
		R_ASSERT					(D);
		/////////////////////////////////////////////////////////////////////////////////////////////
		CSE_ALifePHSkeletonObject	*l_tpALifePhysicObject = smart_cast<CSE_ALifePHSkeletonObject*>(D);
		R_ASSERT					(l_tpALifePhysicObject);
		l_tpALifePhysicObject->_flags.set	(CSE_PHSkeleton::flSpawnCopy,1);
		/////////////////////////////////////////////////////////////////////////////////////////////
		InitServerObject			(D);
		// Send
		NET_Packet			P;
		D->Spawn_Write		(P,TRUE);
		Level().Send		(P,net_flags(TRUE));
		// Destroy
		F_entity_Destroy	(D);
	}
}
PHSHELL_PAIR_VECTOR new_shells;
void CPHSkeleton::PHSplit()
{


	u16 spawned=u16(m_unsplited_shels.size());
	PPhysicsShellHolder()->PPhysicsShell()->SplitProcess(m_unsplited_shels);
	u16 i=u16(m_unsplited_shels.size())-spawned;
	//	Msg("%o,spawned,%d",this,i);
	for(;i;--i) SpawnCopy();


}




void CPHSkeleton::UnsplitSingle(CPHSkeleton* SO)
{
	//Msg("%o,received has %d,",this,m_unsplited_shels.size());
	if (0==m_unsplited_shels.size())	return;	//. hack
	CPhysicsShellHolder* obj = PPhysicsShellHolder();
	CPhysicsShellHolder* O =SO->PPhysicsShellHolder();
	VERIFY2(m_unsplited_shels.size(),"NO_SHELLS !!");
	VERIFY2(!O->m_pPhysicsShell,"this has shell already!!!");
	CPhysicsShell* newPhysicsShell=m_unsplited_shels.front().first;
	O->m_pPhysicsShell=newPhysicsShell;
	VERIFY(_valid(newPhysicsShell->mXFORM));
	CKinematics *newKinematics=smart_cast<CKinematics*>(O->Visual());
	CKinematics *pKinematics  =smart_cast<CKinematics*>(obj->Visual());

	VisMask mask0,mask1;
	u16 split_bone=m_unsplited_shels.front().second;
	mask1 = pKinematics->LL_GetBonesVisible();//source bones mask
	pKinematics->LL_SetBoneVisible(split_bone,FALSE,TRUE);

	pKinematics->CalculateBones_Invalidate	();
	pKinematics->CalculateBones				();

	mask0 = pKinematics->LL_GetBonesVisible();//first part mask
	VERIFY2(mask0,"mask0 -Zero");
	mask0.invert();
	mask1.and(mask0);//second part mask


	newKinematics->LL_SetBoneRoot		(split_bone);
	VERIFY2(mask1,"mask1 -Zero");
	newKinematics->LL_SetBonesVisible	(mask1);

	newKinematics->CalculateBones_Invalidate	();
	newKinematics->CalculateBones				();

	newPhysicsShell->set_Kinematics(newKinematics);
	VERIFY(_valid(newPhysicsShell->mXFORM));
	newPhysicsShell->ResetCallbacks(split_bone,mask1);
	VERIFY(_valid(newPhysicsShell->mXFORM));

	newPhysicsShell->ObjectInRoot().identity();

	if(!newPhysicsShell->isEnabled())O->processing_deactivate();
	newPhysicsShell->set_PhysicsRefObject(O);
	
	m_unsplited_shels.erase(m_unsplited_shels.begin());
	O->setVisible(TRUE);
	O->setEnabled(TRUE);


	SO->CopySpawnInit		();
	CopySpawnInit			();
	VERIFY3(CheckObjectSize(pKinematics),*(O->cNameVisual()),"Object unsplit whith no size");
	VERIFY3(CheckObjectSize(newKinematics),*(O->cNameVisual()),"Object unsplit whith no size");

}

void CPHSkeleton::CopySpawnInit()
{
	if(ReadyForRemove())
			SetAutoRemove();
}




void CPHSkeleton::SetAutoRemove(u32 time/*=CSE_PHSkeleton::existence_time*/)
{
	b_removing=true;
	m_remove_time=Device.dwTimeGlobal+iFloor(time/phTimefactor);
	SetNotNeedSave();
	PPhysicsShellHolder()->SheduleRegister();
}

static bool removable;//for RecursiveBonesCheck
void CPHSkeleton::RecursiveBonesCheck(u16 id)
{
	if(!removable) return;
	CPhysicsShellHolder* obj=PPhysicsShellHolder();
	CKinematics* K		= smart_cast<CKinematics*>(obj->Visual());
	CBoneData& BD		= K->LL_GetData(u16(id));
	//////////////////////////////////////////
	VisMask mask;
	mask = K->LL_GetBonesVisible();
	///////////////////////////////////////////
	if(
		mask.is(id)&& 
		!(BD.shape.flags.is(SBoneShape::sfRemoveAfterBreak))
		) {
			removable = false;
			return;
		}
		///////////////////////////////////////////////
		for (vecBonesIt it=BD.children.begin(); BD.children.end() != it; ++it){
			RecursiveBonesCheck		((*it)->GetSelfID());
		}
}
bool CPHSkeleton::ReadyForRemove()
{
	removable=true;
	CPhysicsShellHolder* obj=PPhysicsShellHolder();
	RecursiveBonesCheck(smart_cast<CKinematics*>(obj->Visual())->LL_GetBoneRoot());
	return removable;
}
void CPHSkeleton::InitServerObject(CSE_Abstract * D)
{

	CPhysicsShellHolder* obj=PPhysicsShellHolder();
	CSE_ALifeDynamicObject		*l_tpALifeDynamicObject = smart_cast<CSE_ALifeDynamicObject*>(D);
	R_ASSERT					(l_tpALifeDynamicObject);
	CSE_ALifePHSkeletonObject		*l_tpALifePhysicObject = smart_cast<CSE_ALifePHSkeletonObject*>(D);
	R_ASSERT					(l_tpALifePhysicObject);

	l_tpALifePhysicObject->m_tGraphID	=obj->ai_location().game_vertex_id();
	l_tpALifeDynamicObject->m_tNodeID	= obj->ai_location().level_vertex_id();
	l_tpALifePhysicObject->set_visual	(*obj->cNameVisual());

	l_tpALifePhysicObject->source_id	= u16(obj->ID());
	l_tpALifePhysicObject->startup_animation=m_startup_anim;
	D->s_name			= "ph_skeleton_object";//*cNameSect()
	D->set_name_replace	("");
	D->s_gameid			=	u8(GameID());
	D->s_RP				=	0xff;
	D->ID				=	0xffff;
	D->ID_Parent		=	0xffff;//u16(ID());//
	D->ID_Phantom		=	0xffff;
	D->o_Position		=	obj->Position();
	if (ai().get_alife())
		l_tpALifeDynamicObject->m_tGraphID = ai().game_graph().current_level_vertex();
	else
		l_tpALifeDynamicObject->m_tGraphID = 0xffff;
	obj->XFORM().getHPB	(D->o_Angle);
	D->s_flags.assign	(M_SPAWN_OBJECT_LOCAL);
	D->RespawnTime		=	0;
}

void	CPHSkeleton::SetNotNeedSave		()
{
	m_flags.set(CSE_PHSkeleton::flNotSave,TRUE);
}