// CameraManager.cpp: implementation of the CCameraManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "igame_level.h"
#include "igame_persistent.h"

#include "Environment.h"
#include "CameraBase.h"
#include "CameraManager.h"
#include "Effector.h"
#include "EffectorPP.h"

#include "x_ray.h"
#include "gamefont.h"
#include "render.h"

float	psCamInert		= 0.7f;
float	psCamSlideInert	= 0.25f;

SPPInfo		pp_identity;
SPPInfo		pp_zero;

void SPPInfo::normalize() 
{
	/*
	noise.intensity = _max(_min(noise.intensity, 1.f), 0.f);
	noise.color.r = _max(_min(noise.color.r, 1.f), 0.f);
	noise.color.g = _max(_min(noise.color.g, 1.f), 0.f);
	noise.color.b = _max(_min(noise.color.b, 1.f), 0.f);
	noise.color.a = _max(_min(noise.color.a, 1.f), 0.f);
	blend_color.r = _max(_min(blend_color.r, 1.f), 0.f);
	blend_color.g = _max(_min(blend_color.g, 1.f), 0.f);
	blend_color.b = _max(_min(blend_color.b, 1.f), 0.f);
	blend_color.a = _max(_min(blend_color.a, 1.f), 0.f);
	*/
}

void SPPInfo::validate(LPCSTR str)
{
	VERIFY2(_valid(duality.h),str);
	VERIFY2(_valid(duality.v),str);
	VERIFY2(_valid(blur),str);
	VERIFY2(_valid(gray),str);
	VERIFY2(_valid(noise.intensity),str);
	VERIFY2(_valid(noise.grain),str);
	VERIFY2(_valid(noise.fps),str);
	VERIFY2(_valid(color_base.r),str);
	VERIFY2(_valid(color_base.g),str);
	VERIFY2(_valid(color_base.b),str);
	VERIFY2(_valid(color_gray.r),str);
	VERIFY2(_valid(color_gray.g),str);
	VERIFY2(_valid(color_gray.b),str);
	VERIFY2(_valid(color_add.r),str);
	VERIFY2(_valid(color_add.g),str);
	VERIFY2(_valid(color_add.b),str);
}

SPPInfo& SPPInfo::lerp(const SPPInfo& def, const SPPInfo& to, float factor)
{
	VERIFY(_valid(factor));
	SPPInfo& pp = *this;
	clamp(factor, 0.0f, 1.0f);

	pp.duality.h		+= def.duality.h		+ (to.duality.h			- def.duality.h)		* factor; 			
	pp.duality.v		+= def.duality.v		+ (to.duality.v			- def.duality.v)		* factor;
	pp.gray				+= def.gray				+ (to.gray				- def.gray)				* factor;
	pp.blur				+= def.blur				+ (to.blur				- def.blur)				* factor;
	pp.noise.intensity	= to.noise.intensity;//	+ (to.noise.intensity	- def.noise.intensity)	* factor;
	pp.noise.grain		= to.noise.grain;//		+ (to.noise.grain		- def.noise.grain)		* factor;
	pp.noise.fps		= to.noise.fps; //		+ (to.noise.fps			- def.noise.fps)		* factor;	

	pp.color_base.set	(
		def.color_base.r	+ (to.color_base.r - def.color_base.r) * factor, 
		def.color_base.g	+ (to.color_base.g - def.color_base.g) * factor, 
		def.color_base.b	+ (to.color_base.b - def.color_base.b) * factor
		);

	pp.color_gray.set	(
		def.color_gray.r	+ (to.color_gray.r - def.color_gray.r) * factor, 
		def.color_gray.g	+ (to.color_gray.g - def.color_gray.g) * factor, 
		def.color_gray.b	+ (to.color_gray.b - def.color_gray.b) * factor
		);

	pp.color_add.set	(
		def.color_add.r	+ (to.color_add.r - def.color_add.r) * factor, 
		def.color_add.g	+ (to.color_add.g - def.color_add.g) * factor, 
		def.color_add.b	+ (to.color_add.b - def.color_add.b) * factor
		);
	return *this;
}

CCameraManager::CCameraManager(bool bApplyOnUpdate)
{
#ifdef DEBUG
	dbg_upd_frame					= 0;
#endif

	m_bAutoApply					= bApplyOnUpdate;
	vPosition.set					(0,0,0);
	vDirection.set					(0,0,1);
	vNormal.set						(0,1,0);

	fFov							= 90;
	fFar							= 100;
	fAspect							= 1.f;

	pp_identity.blur				= 0;
	pp_identity.gray				= 0;
	pp_identity.duality.h			= 0; 
	pp_identity.duality.v			= 0;
	pp_identity.noise.intensity		= 0;	
	pp_identity.noise.grain			= 1.0f;	
	pp_identity.noise.fps			= 30;
	pp_identity.color_base.set		(.5f,	.5f,	.5f);
	pp_identity.color_gray.set		(.333f,	.333f,	.333f);
	pp_identity.color_add.set		(0,		0,		0);

	pp_zero.blur = pp_zero.gray		= pp_zero.duality.h = pp_zero.duality.v = 0.0f;
	pp_zero.noise.intensity			=0;
	pp_zero.noise.grain				= 0.0f;	
	pp_zero.noise.fps				= 0.0f;
	pp_zero.color_base.set			(0,0,0);
	pp_zero.color_gray.set			(0,0,0);
	pp_zero.color_add.set			(0,0,0);

	pp_affected						= pp_identity;
}

CCameraManager::~CCameraManager()
{
	for (EffectorCamIt it=m_EffectorsCam.begin(); it!=m_EffectorsCam.end(); it++ )
		xr_delete(*it);
	for (EffectorPPIt it=m_EffectorsPP.begin(); it!=m_EffectorsPP.end(); it++ )
		xr_delete(*it);
}

CEffectorCam* CCameraManager::GetCamEffector(ECamEffectorType type)	
{ 
	for (EffectorCamIt it=m_EffectorsCam.begin(); it!=m_EffectorsCam.end(); it++ )
		if ((*it)->eType==type) return *it;
	return 0;
}

CEffectorCam* CCameraManager::AddCamEffector(CEffectorCam* ef)
{
	m_EffectorsCam_added_deffered.push_back(ef);
	return m_EffectorsCam_added_deffered.back();
}

void CCameraManager::UpdateDeffered()
{
	EffectorCamIt it		= m_EffectorsCam_added_deffered.begin();
	EffectorCamIt it_e		= m_EffectorsCam_added_deffered.end();
	for (; it!=it_e; ++it)
	{
		RemoveCamEffector			( (*it)->eType );
		m_EffectorsCam.push_back	(*it);
	}
	m_EffectorsCam_added_deffered.clear	();
}

void CCameraManager::RemoveCamEffector(ECamEffectorType type)
{
	for (EffectorCamIt it=m_EffectorsCam.begin(); it!=m_EffectorsCam.end(); it++ )
		if ((*it)->eType==type)
		{ 
			xr_delete(*it);
			m_EffectorsCam.erase(it);
			return;
		}
}

CEffectorPP* CCameraManager::GetPPEffector(EEffectorPPType type)	
{ 
	for (EffectorPPIt it=m_EffectorsPP.begin(); it!=m_EffectorsPP.end(); it++ )
		if ((*it)->Type()==type) return *it;
	return 0;
}

CEffectorPP* CCameraManager::AddPPEffector(CEffectorPP* ef) 
{
	RemovePPEffector				(ef->Type());
	m_EffectorsPP.push_back			(ef);
	return m_EffectorsPP.back		();
}

void CCameraManager::RemovePPEffector(EEffectorPPType type)
{
	for (EffectorPPIt it=m_EffectorsPP.begin(); it!=m_EffectorsPP.end(); it++ )
		if ((*it)->Type()==type){ 
			if ((*it)->FreeOnRemove())	xr_delete(*it);
			m_EffectorsPP.erase			(it);
			return;
		}
}

void CCameraManager::Update(const CCameraBase* C)
{	
	Update(C->vPosition,C->vDirection,C->vNormal, C->f_fov, C->f_aspect, g_pGamePersistent->Environment().CurrentEnv.far_plane, C->m_Flags.flags); 
}
void CCameraManager::Update(const Fvector& P, const Fvector& D, const Fvector& N, float fFOV_Dest, float fASPECT_Dest, float fFAR_Dest, u32 flags)
{
#ifdef DEBUG
	VERIFY(dbg_upd_frame!=Device.dwFrame);// already updated !!!
	dbg_upd_frame			= Device.dwFrame;
#endif
	// camera
	if (flags&CCameraBase::flPositionRigid)
		vPosition.set		(P);
	else
		vPosition.inertion	(P,	psCamInert);
	if (flags&CCameraBase::flDirectionRigid){
		vDirection.set		(D);
		vNormal.set			(N);
	}else{
		vDirection.inertion	(D,	psCamInert);
		vNormal.inertion	(N,	psCamInert);
	}
	
	// Normalize
	vDirection.normalize	();
	vNormal.normalize		();
	vRight.crossproduct		(vNormal,vDirection);
	vNormal.crossproduct	(vDirection,vRight);

	float aspect				= Device.fHeight_2/Device.fWidth_2;
	float src					= 10*Device.fTimeDelta;	clamp(src,0.f,1.f);
	float dst					= 1-src;
	fFov						= fFov*dst		+ fFOV_Dest*src;
	fFar						= fFar*dst		+ fFAR_Dest*src;
	fAspect						= fAspect*dst	+ (fASPECT_Dest*aspect)*src;

	// Effector
	BOOL bOverlapped			= FALSE;
	if (m_EffectorsCam.size())
	{
		for (int i=m_EffectorsCam.size()-1; i>=0; i--)
		{
			CEffectorCam* eff		= m_EffectorsCam[i];
			if(eff->Valid() && eff->Process(vPosition,vDirection,vNormal,fFov,fFar,fAspect))
			{
				bOverlapped		|= eff->Overlapped();
			}else
			{
				if(eff->AllowProcessingIfInvalid())
				{
					eff->ProcessIfInvalid(vPosition,vDirection,vNormal,fFov,fFar,fAspect);
					bOverlapped		|= eff->Overlapped();
				}

				m_EffectorsCam.erase(m_EffectorsCam.begin()+i);
				xr_delete(eff);
			}
		}
		
		// Normalize
		vDirection.normalize	();
		vNormal.normalize		();
		vRight.crossproduct		(vNormal,vDirection);
		vNormal.crossproduct	(vDirection,vRight);
	}

	pp_affected.validate		("before applying pp");
	// EffectorPP
	int		_count	= 0;
	if(m_EffectorsPP.size()) {
		pp_affected = pp_identity;
		for(int i = m_EffectorsPP.size()-1; i >= 0; i--) {
			CEffectorPP* eff	= m_EffectorsPP[i];
			SPPInfo l_PPInf		= pp_zero;
			if((eff->Valid())&&eff->Process(l_PPInf)) 
			{
				++_count;
				pp_affected			+= l_PPInf;
				pp_affected			-= pp_identity;
				pp_affected.validate("in cycle");
			}else 
				RemovePPEffector(eff->Type());
		}
		if (0==_count)	pp_affected				= pp_identity;
		else			pp_affected.normalize	();
	} else {
		pp_affected		=	pp_identity;
	}

	if( !positive(pp_affected.noise.grain) ) pp_affected.noise.grain = pp_identity.noise.grain;
	
	pp_affected.validate		("after applying pp");
	if (FALSE==bOverlapped && m_bAutoApply)
			ApplyDevice		(VIEWPORT_NEAR);

	UpdateDeffered			();
}

void CCameraManager::ApplyDevice (float _viewport_near)
{
	// Device params
	Device.mView.build_camera_dir(vPosition, vDirection, vNormal);

	Device.vCameraPosition.set	( vPosition		);
	Device.vCameraDirection.set	( vDirection	);
	Device.vCameraTop.set		( vNormal		);
	Device.vCameraRight.set		( vRight		);

	// projection
	Device.fFOV					= fFov;
	Device.fASPECT				= fAspect;
	Device.mProject.build_projection(deg2rad(fFov/**fAspect*/), fAspect, _viewport_near, fFar);

	if( g_pGamePersistent && g_pGamePersistent->m_pMainMenu->IsActive() )
		ResetPP					();
	else
	{
		pp_affected.validate		("apply device");
		// postprocess
		IRender_Target*		T		= ::Render->getTarget();
		T->set_duality_h			(pp_affected.duality.h);
		T->set_duality_v			(pp_affected.duality.v);
		T->set_blur					(pp_affected.blur);
		T->set_gray					(pp_affected.gray);
		T->set_noise				(pp_affected.noise.intensity);

		clamp						(pp_affected.noise.grain,EPS_L,1000.0f);

		T->set_noise_scale			(pp_affected.noise.grain);

		T->set_noise_fps			(pp_affected.noise.fps);
		T->set_color_base			(pp_affected.color_base);
		T->set_color_gray			(pp_affected.color_gray);
		T->set_color_add			(pp_affected.color_add);
	}
}

void CCameraManager::ResetPP()
{
	IRender_Target*		T	= ::Render->getTarget();
	T->set_duality_h		(pp_identity.duality.h);
	T->set_duality_v		(pp_identity.duality.v);
	T->set_blur				(pp_identity.blur);
	T->set_gray				(pp_identity.gray);
	T->set_noise			(pp_identity.noise.intensity);
	T->set_noise_scale		(pp_identity.noise.grain);
	T->set_noise_fps		(pp_identity.noise.fps);
	T->set_color_base		(pp_identity.color_base);
	T->set_color_gray		(pp_identity.color_gray);
	T->set_color_add		(pp_identity.color_add);
}

void CCameraManager::Dump()
{
	Fmatrix mInvCamera;
	Fvector _R,_U,_T,_P;
	
	mInvCamera.invert(Device.mView);
	_R.set( mInvCamera._11, mInvCamera._12, mInvCamera._13 );
	_U.set( mInvCamera._21, mInvCamera._22, mInvCamera._23 );
	_T.set( mInvCamera._31, mInvCamera._32, mInvCamera._33 );
	_P.set( mInvCamera._41, mInvCamera._42, mInvCamera._43 );
	Log("CCameraManager::Dump::vPosition  = ",_P);
	Log("CCameraManager::Dump::vDirection = ",_T);
	Log("CCameraManager::Dump::vNormal    = ",_U);
	Log("CCameraManager::Dump::vRight     = ",_R);
}
