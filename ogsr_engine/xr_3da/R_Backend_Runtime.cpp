#include "stdafx.h"
#pragma hdrstop

#pragma warning(push)
#pragma warning(disable:4995)
#include <d3dx9.h>
#pragma warning(pop)

#include "frustum.h"

void CBackend::OnFrameEnd	()
{
#ifndef DEDICATED_SERVER
	for (u32 stage=0; stage<HW.Caps.raster.dwStages; stage++)
		CHK_DX(HW.pDevice->SetTexture(0,0));
	CHK_DX				(HW.pDevice->SetStreamSource	(0,0,0,0));
	CHK_DX				(HW.pDevice->SetIndices			(0));
	CHK_DX				(HW.pDevice->SetVertexShader	(0));
	CHK_DX				(HW.pDevice->SetPixelShader		(0));
	Invalidate			();
#endif
}

void CBackend::OnFrameBegin	()
{
#ifndef DEDICATED_SERVER
	PGO					(Msg("PGO:*****frame[%d]*****",Device.dwFrame));
	Memory.mem_fill		(&stat,0,sizeof(stat));
	Vertex.Flush		();
	Index.Flush			();
	set_Stencil			(FALSE);
#endif
}

void CBackend::Invalidate	()
{
	pRT[0]						= NULL;
	pRT[1]						= NULL;
	pRT[2]						= NULL;
	pRT[3]						= NULL;
	pZB							= NULL;

	decl						= NULL;
	vb							= NULL;
	ib							= NULL;
	vb_stride					= 0;

	state						= NULL;
	ps							= NULL;
	vs							= NULL;
	ctable						= NULL;

	T							= NULL;
	M							= NULL;
	C							= NULL;

	colorwrite_mask				= u32(-1);

	for (u32 ps_it =0; ps_it<16;)	textures_ps	[ps_it++]	= 0;
	for (u32 vs_it =0; vs_it< 5;)	textures_vs	[vs_it++]	= 0;
#ifdef _EDITOR
	for (u32 m_it =0; m_it< 8;)		matrices	[m_it++]	= 0;
#endif
}

void	CBackend::set_ClipPlanes	(u32 _enable, Fplane*	_planes /*=NULL */, u32 count/* =0*/)
{
	if (0==HW.Caps.geometry.dwClipPlanes)	return;
	if (!_enable)	{
		CHK_DX	(HW.pDevice->SetRenderState(D3DRS_CLIPPLANEENABLE,FALSE));
		return;
	}

	// Enable and setup planes
	VERIFY	(_planes && count);
	if		(count>HW.Caps.geometry.dwClipPlanes)	count=HW.Caps.geometry.dwClipPlanes;

	D3DXMATRIX			worldToClipMatrixIT;
	D3DXMatrixInverse	(&worldToClipMatrixIT,NULL,(D3DXMATRIX*)&Device.mFullTransform);
	D3DXMatrixTranspose	(&worldToClipMatrixIT,&worldToClipMatrixIT);
	for		(u32 it=0; it<count; it++)		{
		Fplane&		P			= _planes	[it];
		D3DXPLANE	planeWorld	(-P.n.x,-P.n.y,-P.n.z,-P.d), planeClip;
		D3DXPlaneNormalize		(&planeWorld,	&planeWorld);
		D3DXPlaneTransform		(&planeClip,	&planeWorld, &worldToClipMatrixIT);
		CHK_DX					(HW.pDevice->SetClipPlane(it,planeClip));
	}

	// Enable them
	u32		e_mask	= (1<<count)-1;
	CHK_DX	(HW.pDevice->SetRenderState(D3DRS_CLIPPLANEENABLE,e_mask));
}

#ifndef DEDICATED_SREVER
void	CBackend::set_ClipPlanes	(u32 _enable, Fmatrix*	_xform  /*=NULL */, u32 fmask/* =0xff */)
{
	if (0==HW.Caps.geometry.dwClipPlanes)	return;
	if (!_enable)	{
		CHK_DX	(HW.pDevice->SetRenderState(D3DRS_CLIPPLANEENABLE,FALSE));
		return;
	}
	VERIFY		(_xform && fmask);
	CFrustum	F;
	F.CreateFromMatrix	(*_xform,fmask);
	set_ClipPlanes		(_enable,F.planes,F.p_count);
}

void CBackend::set_Textures			(STextureList* _T)
{
	if (T == _T)	return;
	T				= _T;
	u32 _last_ps	= 0;
	u32 _last_vs	= 0;
	STextureList::iterator	_it		= _T->begin	();
	STextureList::iterator	_end	= _T->end	();
	for (; _it!=_end; _it++)
	{
		std::pair<u32,ref_texture>&		loader	=	*_it;
		u32			load_id		= loader.first		;
		CTexture*	load_surf	= &*loader.second	;
		if (load_id<256)		{
			// ordinary pixel surface
			if (load_id>_last_ps)		_last_ps	=	load_id;
			if (textures_ps[load_id]!=load_surf)	{
				textures_ps[load_id]	= load_surf			;
#ifdef DEBUG
				stat.textures			++;
#endif
				if (load_surf)			{
					PGO					(Msg("PGO:tex%d:%s",load_id,load_surf->cName.c_str()));
					load_surf->bind		(load_id);
//					load_surf->Apply	(load_id);
				}
			}
		} else {
			// d-map or vertex	
			u32		load_id_remapped	= load_id-256;
			if (load_id_remapped>_last_vs)	_last_vs	=	load_id_remapped;
			if (textures_vs[load_id_remapped]!=load_surf)	{
				textures_vs[load_id_remapped]	= load_surf			;
#ifdef DEBUG
				stat.textures	++;
#endif
				if (load_surf)	{
					PGO					(Msg("PGO:tex%d:%s",load_id,load_surf->cName.c_str()));
					load_surf->bind		(load_id);
//					load_surf->Apply	(load_id);
				}
			}
		}
	}

	// clear remaining stages (PS)
	for (++_last_ps; _last_ps<16 && textures_ps[_last_ps]; _last_ps++)	{
		textures_ps[_last_ps]			= 0;
		CHK_DX							(HW.pDevice->SetTexture(_last_ps,NULL));
	}
	// clear remaining stages (VS)
	for (++_last_vs; _last_vs<5 && textures_vs[_last_vs]; _last_vs++)		{
		textures_vs[_last_vs]			= 0;
		CHK_DX							(HW.pDevice->SetTexture(_last_vs+256,NULL));
	}
}
#else

void	CBackend::set_ClipPlanes	(u32 _enable, Fmatrix*	_xform  /*=NULL */, u32 fmask/* =0xff */) {}
void CBackend::set_Textures			(STextureList* _T) {}

#endif