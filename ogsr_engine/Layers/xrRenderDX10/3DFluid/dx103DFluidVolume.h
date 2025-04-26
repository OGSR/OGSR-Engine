#pragma once

#ifdef DX10_FLUID_ENABLE

#include "dx103DFluidData.h"
#include "../../xrRender/FBasicVisual.h"

class dx103DFluidVolume : public dxRender_Visual
{
public:
    dx103DFluidVolume();
    virtual ~dx103DFluidVolume();

    virtual void Load(LPCSTR N, IReader* data, u32 dwFlags);
    virtual void Render(CBackend& cmd_list, float lod, bool use_fast_geo); // LOD - Level Of Detail  [0.0f - min, 1.0f - max], Ignored ?
    virtual void Copy(dxRender_Visual* pFrom);
    virtual void Release();

    shared_str getProfileName() { return m_FluidData.dbg_name; }

private:
    //	For debug purpose only
    ref_geom m_Geom;

    dx103DFluidData m_FluidData;
};

#endif //	dx103DFluidVolume_included
