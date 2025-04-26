// FVisual.h: interface for the FVisual class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include "fbasicvisual.h"

class Fvisual : public dxRender_Visual, public IRender_Mesh
{
public:
    IRender_Mesh* m_fast;

public:
    void Render(CBackend& cmd_list, float lod, bool use_fast_geo) override; // LOD - Level Of Detail  [0.0f - min, 1.0f - max], Ignored ?
    virtual void Load(const char* N, IReader* data, u32 dwFlags);
    virtual void Copy(dxRender_Visual* pFrom);
    virtual void Release();

    Fvisual();
    virtual ~Fvisual();
};
