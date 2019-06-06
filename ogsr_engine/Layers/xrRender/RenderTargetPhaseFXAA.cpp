#include "stdafx.h"

void CRenderTarget::phase_fxaa()
{
	u32 Offset = 0;
	const float _w = float(Device.dwWidth);
	const float _h = float(Device.dwHeight);
	const float du = ps_r1_pps_u, dv = ps_r1_pps_v;

#if defined(USE_DX10) || defined(USE_DX11)
	ref_rt dest_rt = RImplementation.o.dx10_msaa ? rt_Generic : rt_Color;
	u_setrt(dest_rt, nullptr, nullptr, HW.pBaseZB);

	FVF::V* pv = (FVF::V*)RCache.Vertex.Lock(4, g_fxaa->vb_stride, Offset);
	pv->set(du + 0, dv + float(_h), 0, 0, 1);
	pv++;
	pv->set(du + 0, dv + 0, 0, 0, 0);
	pv++;
	pv->set(du + float(_w), dv + float(_h), 0, 1, 1);
	pv++;
	pv->set(du + float(_w), dv + 0, 0, 1, 0);
	pv++;
#else
	u_setrt(rt_Generic_0, nullptr, nullptr, HW.pBaseZB);
	RCache.set_CullMode(CULL_NONE);
	RCache.set_Stencil(FALSE);

	FVF::V* pv = (FVF::V*)RCache.Vertex.Lock(4, g_fxaa->vb_stride, Offset);
	pv->set(du - 0.5, dv + float(_h) - 0.5, 0, 0, 1);
	pv++;
	pv->set(du - 0.5, dv - 0.5, 0, 0, 0);
	pv++;
	pv->set(du + float(_w) - 0.5, dv + float(_h) - 0.5, 0, 1, 1);
	pv++;
	pv->set(du + float(_w) - 0.5, dv - 0.5, 0, 1, 0);
	pv++;
#endif
	RCache.Vertex.Unlock(4, g_fxaa->vb_stride);

	RCache.set_Element(s_fxaa->E[0]);
	RCache.set_Geometry(g_fxaa);
	RCache.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);

#if defined(USE_DX10) || defined(USE_DX11)
	HW.pContext->CopyResource(rt_Generic_0->pTexture->surface_get(), dest_rt->pTexture->surface_get());
#endif
}
