#include "stdafx.h"
#include "..\xrRender\xrRender_console.h"
#include "r2_fog_volumes.h"

bool fv_pred(u32 a, u32 b) 
{
	float d_a = FV->m_volumes[a].m_xform.c.distance_to(Device.vCameraPosition);
	float d_b = FV->m_volumes[b].m_xform.c.distance_to(Device.vCameraPosition);
	return d_a > d_b;
}

void CRenderTarget::phase_fog_volumes()
{
	u32 Offset;

	// check visibility of fog volumes
	xr_vector<u32> visible_volumes;
	visible_volumes.reserve(FV->m_volumes.size());

	for (u16 i = 0; i < FV->m_volumes.size(); ++i)
	{
		u32 mask = 0xff;
		u32 res				= RImplementation.ViewBase.testAABB (FV->m_volumes[i].m_bbox.data(), mask);
		if (fcvNone==res) continue;
		visible_volumes.push_back(i);
	};

	if (visible_volumes.empty()) return;

	Fvector2 p0, p1;

	struct v_simple	{
		Fvector4	p;
		Fvector2	uv0;
	};

	float	_w					= float(Device.dwWidth);
	float	_h					= float(Device.dwHeight);
	p0.set						(.5f/_w, .5f/_h);
	p1.set						((_w+.5f)/_w, (_h+.5f)/_h );

	// sorting far to near
	std::sort(visible_volumes.begin(), visible_volumes.end(), fv_pred);
	for (u16 i = 0; i < visible_volumes.size(); ++i)
	{
		// render back faces
		u_setrt(FV->rt_FV_faces, 0, 0, HW.pBaseZB);
		CHK_DX	( HW.pDevice->Clear	( 0L, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 0.0f, 0L) );
		RCache.set_Stencil			( FALSE)	;
		RCache.set_CullMode			(CULL_CW);

		RCache.set_Element			(FV->s_fvolumes->E[0]);

		CFogVolume *vol = &FV->m_volumes[visible_volumes[i]];

		Fvector inv_cam_pos;
		vol->m_inv_xform.transform_tiny(inv_cam_pos, Device.vCameraPosition);

		RCache.set_xform_world		(vol->m_xform);
		RCache.set_Geometry			(vol->hGeom);
		RCache.Render				(D3DPT_TRIANGLELIST,0,0,vol->num_vertices,0,vol->num_indices/3);

		// render front faces
		u_setrt(FV->rt_FV_faces, 0, 0, HW.pBaseZB);
		RCache.set_CullMode			(CULL_CCW);

		RCache.set_Element			(FV->s_fvolumes->E[1]);

		RCache.Render				(D3DPT_TRIANGLELIST,0,0,vol->num_vertices,0,vol->num_indices/3);

		// draw volume
		RCache.set_CullMode			(CULL_NONE);

		// Set RT's
		u_setrt(rt_Generic_0,0,0,HW.pBaseZB);

		// Fill vertex buffer
		v_simple* pv					= (v_simple*) RCache.Vertex.Lock	(4,g_KD->vb_stride,Offset);
			pv->p.set(EPS,			float(_h+EPS),	EPS,1.f); pv->uv0.set(p0.x, p1.y);pv++;
			pv->p.set(EPS,			EPS,			EPS,1.f); pv->uv0.set(p0.x, p0.y);pv++;
			pv->p.set(float(_w+EPS),float(_h+EPS),	EPS,1.f); pv->uv0.set(p1.x, p1.y);pv++;
			pv->p.set(float(_w+EPS),EPS,			EPS,1.f); pv->uv0.set(p1.x, p0.y);pv++;
		RCache.Vertex.Unlock		(4,g_KD->vb_stride);

		// Draw COLOR
		RCache.set_Element		(FV->s_fvolumes->E[2]);
		RCache.set_Geometry		(g_KD);
		RCache.set_c			("c_color", vol->m_color.r, vol->m_color.g, vol->m_color.b, vol->m_color.a);
		RCache.set_c			("c_params", vol->fMaxDensity, inv_cam_pos.x, inv_cam_pos.y, inv_cam_pos.z);
		RCache.Render			(D3DPT_TRIANGLELIST,Offset,0,4,0,2);
		RCache.set_Stencil		(FALSE);
	}
}