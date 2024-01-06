#include "stdafx.h"

void CRenderTarget::phase_lut()
{
    ref_rt& dest_rt = RImplementation.o.dx10_msaa ? rt_Generic : rt_Color;
    RenderScreenQuad(Device.dwWidth, Device.dwHeight, dest_rt, s_lut->E[0]);
    HW.pContext->CopyResource(rt_Generic_0->pTexture->surface_get(), dest_rt->pTexture->surface_get());
}
