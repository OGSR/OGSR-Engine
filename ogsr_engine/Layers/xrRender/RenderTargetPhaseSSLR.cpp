#include "stdafx.h"

void CRenderTarget::phase_SSLR()
{
	u32 Offset = 0;
	constexpr float d_Z = EPS_S;
	constexpr float d_W = 1.0f;
	constexpr u32 C = color_rgba(0, 0, 0, 255);

	// Half-pixel offset (DX9 only)
#if defined(USE_DX10) || defined(USE_DX11)
	constexpr Fvector2 p0{ 0.0f, 0.0f }, p1{ 1.0f, 1.0f };
#else
	Fvector2 p0, p1;
	p0.set(0.5f / w, 0.5f / h);
	p1.set((w + 0.5f) / w, (h + 0.5f) / h);
#endif

	const u32 w = Device.dwWidth, h = Device.dwHeight;

	/////////////////////////////////////////////////////////////////////////////////////
	// phase SSLR
	u_setrt(rt_SSLR_0, nullptr, nullptr, HW.pBaseZB);

	RCache.set_CullMode(CULL_NONE);
	RCache.set_Stencil(FALSE);

	FVF::TL* pv = (FVF::TL*)RCache.Vertex.Lock(4, g_combine->vb_stride, Offset);
	pv->set(0, float(h), d_Z, d_W, C, p0.x, p1.y); pv++;
	pv->set(0, 0, d_Z, d_W, C, p0.x, p0.y); pv++;
	pv->set(float(w), float(h), d_Z, d_W, C, p1.x, p1.y); pv++;
	pv->set(float(w), 0, d_Z, d_W, C, p1.x, p0.y); pv++;
	RCache.Vertex.Unlock(4, g_combine->vb_stride);

	RCache.set_Element(s_SSLR->E[0]);

	Fmatrix m_inv_v;
	m_inv_v.invert(Device.mView);
	RCache.set_c("m_inv_v", m_inv_v);
	RCache.set_c("SSLR_params", ps_ext_SSLR_L, 1.f, 1.f, 1.f);

	RCache.set_Geometry(g_combine);
	RCache.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);

	if (fis_zero(ps_ext_SSLR_blur))
		return;

	/////////////////////////////////////////////////////////////////////////////////////
	// hblur
	u_setrt(rt_SSLR_1, nullptr, nullptr, HW.pBaseZB);

	RCache.set_CullMode(CULL_NONE);
	RCache.set_Stencil(FALSE);

	pv = (FVF::TL*)RCache.Vertex.Lock(4, g_combine->vb_stride, Offset);
	pv->set(0, float(h), d_Z, d_W, C, p0.x, p1.y); pv++;
	pv->set(0, 0, d_Z, d_W, C, p0.x, p0.y); pv++;
	pv->set(float(w), float(h), d_Z, d_W, C, p1.x, p1.y); pv++;
	pv->set(float(w), 0, d_Z, d_W, C, p1.x, p0.y); pv++;
	RCache.Vertex.Unlock(4, g_combine->vb_stride);

	RCache.set_Element(s_SSLR->E[1]);

	RCache.set_c("blur_params", ps_ext_SSLR_blur, 0.f, w, h);

	RCache.set_Geometry(g_combine);
	RCache.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);

	/////////////////////////////////////////////////////////////////////////////////////
	// vblur
	u_setrt(rt_SSLR_0, nullptr, nullptr, HW.pBaseZB);

	RCache.set_CullMode(CULL_NONE);
	RCache.set_Stencil(FALSE);

	pv = (FVF::TL*)RCache.Vertex.Lock(4, g_combine->vb_stride, Offset);
	pv->set(0, float(h), d_Z, d_W, C, p0.x, p1.y); pv++;
	pv->set(0, 0, d_Z, d_W, C, p0.x, p0.y); pv++;
	pv->set(float(w), float(h), d_Z, d_W, C, p1.x, p1.y); pv++;
	pv->set(float(w), 0, d_Z, d_W, C, p1.x, p0.y); pv++;
	RCache.Vertex.Unlock(4, g_combine->vb_stride);

	RCache.set_Element(s_SSLR->E[2]);

	RCache.set_c("blur_params", 0.f, ps_ext_SSLR_blur, w, h);

	RCache.set_Geometry(g_combine);
	RCache.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);
}
