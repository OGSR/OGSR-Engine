#include "StdAfx.h"
#include "animation_movement_controller.h"
#include "../SkeletonAnimated.h" 
#include "game_object_space.h"

animation_movement_controller::animation_movement_controller( Fmatrix *_pObjXForm, CKinematics* _pKinematicsC, CBlend* b ):
m_startObjXForm( *_pObjXForm ), 
m_pObjXForm( *_pObjXForm ),
m_pKinematicsC( _pKinematicsC ),
m_control_blend( b )
{
	VERIFY( _pKinematicsC );
	VERIFY( _pObjXForm );
	VERIFY( b );
	CBoneInstance& B=m_pKinematicsC->LL_GetBoneInstance( m_pKinematicsC->LL_GetBoneRoot( ) );
	VERIFY( !B.Callback && !B.Callback_Param );
	B.set_callback( bctCustom, RootBoneCallback, this );
	m_startRootXform.set(B.mTransform);
}

animation_movement_controller::~animation_movement_controller( )
{
	if(isActive())
		deinitialize();
}
void	animation_movement_controller::	deinitialize					()
{
	CBoneInstance& B=m_pKinematicsC->LL_GetBoneInstance( m_pKinematicsC->LL_GetBoneRoot( ) );
	VERIFY( B.Callback == RootBoneCallback );
	VERIFY( B.Callback_Param == (void*)this );
	B.reset_callback( );
	m_control_blend =  0 ;
}
void animation_movement_controller::OnFrame( )
{
	m_pKinematicsC->CalculateBones( );
	
	if(CBlend::eFREE_SLOT == m_control_blend->blend)
	{
		deinitialize();
		return;
	}
	if( m_control_blend->blend == CBlend::eAccrue && m_control_blend->blendPower - EPS_L > m_control_blend->blendAmount )
			m_control_blend->timeCurrent =0;
}

void animation_movement_controller::RootBoneCallback( CBoneInstance* B )
{
	VERIFY( B );
	VERIFY( B->Callback_Param );
	
	animation_movement_controller* O=( animation_movement_controller* )( B->Callback_Param );

	if(O->m_control_blend->playing)
	{
		Fmatrix m;m.mul_43(  B->mTransform, Fmatrix( ).invert( O->m_startRootXform ) );
		O->m_pObjXForm.mul_43( O->m_startObjXForm, m );
	} 
	B->mTransform.set(O->m_startRootXform);
}

bool	animation_movement_controller::isActive() const
{
	return !!m_control_blend ;
}