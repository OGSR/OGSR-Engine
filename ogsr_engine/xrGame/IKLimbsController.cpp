#include "stdafx.h"

#include "IKLimbsController.h"

#include "IK/IKLimb.h"
#include "physicsshellholder.h"

#include "ik_anim_state.h"
#include "../xr_3da/ennumerateVertices.h"
#ifdef DEBUG
#include "PHDebug.h"
#endif
#include "../Include/xrRender/RenderVisual.h"
#include "../Include/xrRender/Kinematics.h"
#include "../Include/xrRender/KinematicsAnimated.h"

CIKLimbsController::CIKLimbsController(): m_object(0), m_legs_blend(0)
{
	
}

void CIKLimbsController::Create( CGameObject* O )
{
	m_legs_blend	 = 0;
	
	IKinematics* K = smart_cast<IKinematics*>(O->Visual());
	m_object = O;
	VERIFY( K );
	{
		u16 bones[4]={K->LL_BoneID("bip01_l_upperarm"),K->LL_BoneID("bip01_l_forearm"),K->LL_BoneID("bip01_l_hand"),K->LL_BoneID("bip01_l_finger0")};
		LimbSetup(bones);
	}

	{
		u16 bones[4]={K->LL_BoneID("bip01_r_upperarm"),K->LL_BoneID("bip01_r_forearm"),K->LL_BoneID("bip01_r_hand"),K->LL_BoneID("bip01_r_finger0")};
		LimbSetup(bones);
	}

	{
		u16 bones[4]={K->LL_BoneID("bip01_l_thigh"),K->LL_BoneID("bip01_l_calf"),K->LL_BoneID("bip01_l_foot"),K->LL_BoneID("bip01_l_toe0")};
		LimbSetup(bones);
	}

	{
		u16 bones[4]={K->LL_BoneID("bip01_r_thigh"),K->LL_BoneID("bip01_r_calf"),K->LL_BoneID("bip01_r_foot"),K->LL_BoneID("bip01_r_toe0")};
		LimbSetup(bones);
	}
	O->add_visual_callback(IKVisualCallback);
}

struct envc :
public SEnumVerticesCallback
{
	Fvector &pos;
	Fvector start_pos;
	const Fmatrix &i_bind_transform;
	const Fvector &ax;
	envc(const envc&) = delete;
	void operator=(const envc&) = delete;
	envc( const Fmatrix &_i_bind_transform, const Fvector &_ax,  Fvector &_pos ): 
	SEnumVerticesCallback( ), i_bind_transform( _i_bind_transform ), ax( _ax ), pos( _pos ) { start_pos.set(0,0,0); }
	void operator () (const Fvector& p)
	{
		Fvector lpos;
		i_bind_transform.transform_tiny(lpos, p );
		//Fvector diff;diff.sub( lpos, pos );
		if( Fvector().sub(lpos,start_pos).dotproduct( ax ) > Fvector().sub(pos,start_pos).dotproduct( ax ) )
						pos.set( lpos );
	}
};

void get_toe(IKinematics *skeleton, Fvector & toe, const u16 bones[4])
{
	VERIFY( skeleton );
	xr_vector<Fmatrix> binds;
	skeleton->LL_GetBindTransform( binds );
	Fmatrix ibind; ibind.invert( binds[ bones[3] ] );
	Fvector ax; Fvector pos; pos.set( 0, 0, 0);
	ax.set(1, 0, -1 );
	envc pred( ibind, ax, pos );
	skeleton->EnumBoneVertices( pred, bones[3] );

	binds[ bones[3] ].transform_tiny( pos );
	Fmatrix( ).invert(  binds[ bones[2] ]  ).transform_tiny( pos );
	
	toe.set( pos );
	
	ibind.invert( binds[ bones[2] ] );
	ax.set(1, 0, 0);
	skeleton->EnumBoneVertices( pred, bones[2] );
	toe.x = _max( pos.x, toe.x );

}

void	CIKLimbsController::LimbSetup(  const u16 bones[4] )
{
	_bone_chains.push_back( CIKLimb( ) );
	IKinematics* skeleton_animated = m_object->Visual( )->dcast_PKinematics();
	VERIFY( skeleton_animated );
	Fvector toe;
	get_toe( skeleton_animated, toe, bones );
	_bone_chains.back( ).Create( ( u16 ) _bone_chains.size( )-1, skeleton_animated, bones, toe, true );//Fvector( ).set( 0.13143f, 0, 0.20f )
}



void	CIKLimbsController::LimbCalculate( SCalculateData &cd )
{
	cd.do_collide	= m_legs_blend && !cd.m_K->LL_GetMotionDef( m_legs_blend->motionID )->marks.empty() ;//m_legs_blend->;
	cd.m_limb.Calculate(cd);
}

void	CIKLimbsController::LimbUpdate( CIKLimb &L, u16 i )
{
	VERIFY( m_object->Visual()->dcast_PKinematicsAnimated() );
	L.Update( m_object, m_legs_blend, i );
}

IC void	update_blend (CBlend* &b)
{
	if(b && CBlend::eFREE_SLOT == b->blend_state())
		b = 0;
}
void CIKLimbsController::Calculate( )
{
	
	update_blend( m_legs_blend );
	IKinematicsAnimated *skeleton_animated = m_object->Visual()->dcast_PKinematicsAnimated( );
	const Fmatrix &obj = m_object->XFORM( );
	VERIFY( skeleton_animated );

	{
		SCalculateData cd(_bone_chains[left_leg],skeleton_animated,obj);//,m_anim_base,m_uneffected
		LimbCalculate(cd);
	}
	
	{
		SCalculateData cd(_bone_chains[right_leg],skeleton_animated,obj);//,m_anim_base,m_uneffected
		LimbCalculate(cd);
	}
/*
	{
		SCalculateData cd(m_left_arm,K,obj);//,m_anim_base,m_uneffected
		//cd.anim_goal = true;
		LimbCalculate(cd);
	}
	{
		SCalculateData cd(m_right_arm,K,obj);//,m_anim_base,m_uneffected
		//cd.anim_goal = true;
		LimbCalculate(cd);
	}
*/
}

void CIKLimbsController::Destroy(CGameObject* O)
{
#ifdef DEBUG
	CPhysicsShellHolder*	Sh = smart_cast<CPhysicsShellHolder*>(O);
	VERIFY(Sh);
	CIKLimbsController* ik = Sh->character_ik_controller();
	VERIFY(ik);
	VERIFY(ik==this);
#endif

	O->remove_visual_callback(IKVisualCallback);
	xr_vector<CIKLimb>::iterator	i = _bone_chains.begin(), e = _bone_chains.end();
	for(;e!=i;++i)
		i->Destroy();
	_bone_chains.clear();
}

void _stdcall CIKLimbsController:: IKVisualCallback( IKinematics* K )
{
#ifdef DEBUG
	if( ph_dbg_draw_mask1.test( phDbgIKOff ) )
		return;
#endif
	
	CGameObject* O=( ( CGameObject* )K->GetUpdateCallbackParam());
	CPhysicsShellHolder*	Sh = smart_cast<CPhysicsShellHolder*>( O );
	VERIFY( Sh );
	CIKLimbsController* ik = Sh->character_ik_controller( );
	VERIFY( ik );
	ik->Calculate( );
}
void CIKLimbsController::PlayLegs( CBlend *b )	
{
	m_legs_blend	= b;
#ifdef DEBUG
	IKinematicsAnimated *skeleton_animated = m_object->Visual( )->dcast_PKinematicsAnimated( );
	VERIFY( skeleton_animated );
	anim_name = skeleton_animated->LL_MotionDefName_dbg( b->motionID ).first;
	anim_set_name = skeleton_animated->LL_MotionDefName_dbg( b->motionID ).second;
#endif
}
void	CIKLimbsController:: Update						( )
{
	IKinematicsAnimated *skeleton_animated = m_object->Visual()->dcast_PKinematicsAnimated( );
	VERIFY( skeleton_animated );

	skeleton_animated->UpdateTracks();
	update_blend( m_legs_blend );

	LimbUpdate( _bone_chains[left_leg], 0 );
	LimbUpdate( _bone_chains[right_leg], 1 );
}
