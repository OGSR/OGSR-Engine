#pragma once

#include "../../Include/xrRender/DebugRender.h"

class dxDebugRender : public IDebugRender
{
public:
    dxDebugRender() = default;

    virtual void Render();
    virtual void add_lines(Fvector const* vertices, u32 const& vertex_count, u16 const* pairs, u32 const& pair_count, u32 const& color, bool hud_mode = false);

    virtual void ZEnable(bool bEnable);
    virtual void OnFrameEnd();
    virtual void SetShader(const debug_shader& shader);
    virtual void CacheSetXformWorld(const Fmatrix& M);
    virtual void CacheSetCullMode(CullMode);
    virtual void SetAmbient(u32 colour);

    // Shaders
    virtual void SetDebugShader(dbgShaderHandle shdHandle);
    virtual void DestroyDebugShader(dbgShaderHandle shdHandle);

    virtual void dbg_DrawTRI(Fmatrix& T, Fvector& p1, Fvector& p2, Fvector& p3, u32 C);

private:
    xr_unordered_map<u32, xr_vector<FVF::L>> m_line_vertices;
    xr_unordered_map<u32, xr_vector<u16>> m_line_indices;

    xr_unordered_map<u32, xr_vector<FVF::L>> m_line_vertices_hud;
    xr_unordered_map<u32, xr_vector<u16>> m_line_indices_hud;

    ref_shader m_dbgShaders[dbgShaderCount];
};

extern dxDebugRender DebugRenderImpl;

#ifdef DEBUG
extern dxDebugRender* rdebug_render;
#endif
