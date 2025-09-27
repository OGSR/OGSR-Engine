#include "stdafx.h"

#include "dxDebugRender.h"
#include "dxUIShader.h"
#include "dxRenderDeviceRender.h"
#include "HUDInitializer.h"

dxDebugRender DebugRenderImpl;

extern ENGINE_API float psHUD_FOV;

void dxDebugRender::Render()
{
    if (!m_line_vertices.empty())
    {
        for (auto& [color, vert_vec] : m_line_vertices)
        {
            auto& ind_vec = m_line_indices.at(color);

            RCache.set_xform_world(Fidentity);
            RCache.set_Shader(dxRenderDeviceRender::Instance().m_WireShader);
            RCache.set_c("tfactor", float(color_get_R(color)) / 255.f, float(color_get_G(color)) / 255.f, float(color_get_B(color)) / 255.f, float(color_get_A(color)) / 255.f);
            RCache.dbg_Draw(D3DPT_LINELIST, &vert_vec.front(), static_cast<int>(vert_vec.size()), &ind_vec.front(), static_cast<int>(ind_vec.size() / 2));
        }

        m_line_vertices.clear();
        m_line_indices.clear();
    }

    if (!m_line_vertices_hud.empty())
    {
        CHUDTransformHelper initializer(RCache, true);

        for (auto& [color, vert_vec] : m_line_vertices_hud)
        {
            auto& ind_vec = m_line_indices_hud.at(color);

            RCache.set_xform_world(Fidentity);
            RCache.set_Shader(dxRenderDeviceRender::Instance().m_WireShader);
            RCache.set_c("tfactor", float(color_get_R(color)) / 255.f, float(color_get_G(color)) / 255.f, float(color_get_B(color)) / 255.f, float(color_get_A(color)) / 255.f);
            RCache.dbg_Draw_Near(D3DPT_LINELIST, &vert_vec.front(), static_cast<int>(vert_vec.size()), &ind_vec.front(), static_cast<int>(ind_vec.size() / 2));
        }

        m_line_vertices_hud.clear();
        m_line_indices_hud.clear();
    }
}

void dxDebugRender::add_lines(Fvector const* vertices, u32 const& vertex_count, u16 const* pairs, u32 const& pair_count, u32 const& color, bool hud_mode)
{
    auto& line_vertices = hud_mode ? m_line_vertices_hud : m_line_vertices;
    auto& line_indices = hud_mode ? m_line_indices_hud : m_line_indices;

    size_t all_verts_count{}, all_inds_count{};
    for (const auto& [color, vert_vec] : line_vertices)
    {
        all_verts_count += vert_vec.size();
        all_inds_count += line_indices.at(color).size();
    }

    //Лимиты превышать нельзя ни в коем случае - убавить лимит если будут краши в R_DStreams.cpp
    if ((all_verts_count + vertex_count) >= u16(-1))
    {
        // Msg("~~[%s.1] Rendered [%u] verts and [%u] inds", __FUNCTION__, all_verts_count, all_inds_count);
        Render();
    }
    else if ((all_inds_count + 2 * pair_count) >= u16(-1))
    {
        // Msg("~~[%s.2] Rendered [%u] verts and [%u] inds", __FUNCTION__, all_verts_count, all_inds_count);
        Render();
    }

    //////////////////////////////////////////////////////////////////

    auto& vert_vec = line_vertices[color];
    auto& ind_vec = line_indices[color];

    const auto vertices_size = vert_vec.size(), indices_size = ind_vec.size();

    ind_vec.resize(indices_size + 2 * pair_count);
    auto I = ind_vec.begin() + indices_size, E = ind_vec.end();
    const u16* J = pairs;
    for (; I != E; ++I, ++J)
        *I = static_cast<u16>(vertices_size + *J);

    vert_vec.resize(vertices_size + vertex_count);
    auto i = vert_vec.begin() + vertices_size, e = vert_vec.end();
    Fvector const* j = vertices;
    for (; i != e; ++i, ++j)
    {
        i->color = color;
        i->p = *j;
    }
}

void dxDebugRender::OnFrameEnd() { RCache.OnFrameEnd(); }

void dxDebugRender::SetShader(const debug_shader& shader) { RCache.set_Shader(smart_cast<dxUIShader*>(&*shader)->hShader); }

void dxDebugRender::CacheSetXformWorld(const Fmatrix& M) { RCache.set_xform_world(M); }

void dxDebugRender::CacheSetCullMode(CullMode m) { RCache.set_CullMode(CULL_NONE + m); }

void dxDebugRender::SetDebugShader(dbgShaderHandle shdHandle)
{
    R_ASSERT(shdHandle < dbgShaderCount);

    constexpr LPCSTR dbgShaderParams[][2] = {
        {"hud\\default", "ui\\ui_pop_up_active_back"}, // dbgShaderWindow
    };

    if (!m_dbgShaders[shdHandle])
        m_dbgShaders[shdHandle].create(dbgShaderParams[shdHandle][0], dbgShaderParams[shdHandle][1]);

    RCache.set_Shader(m_dbgShaders[shdHandle]);
}

void dxDebugRender::DestroyDebugShader(dbgShaderHandle shdHandle)
{
    R_ASSERT(shdHandle < dbgShaderCount);

    m_dbgShaders[shdHandle].destroy();
}

void dxDebugRender::dbg_DrawTRI(Fmatrix& T, Fvector& p1, Fvector& p2, Fvector& p3, u32 C) { RCache.dbg_DrawTRI(T, p1, p2, p3, C); }

#ifdef DEBUG

struct RDebugRender : public dxDebugRender, public pureRender
{
    RDebugRender() { Device.seqRender.Add(this, REG_PRIORITY_LOW - 100); }

    virtual ~RDebugRender() { Device.seqRender.Remove(this); }

    void OnRender() { Render(); }
    virtual void add_lines(Fvector const* vertices, u32 const& vertex_count, u16 const* pairs, u32 const& pair_count, u32 const& color)
    {
        dxDebugRender::add_lines(vertices, vertex_count, pairs, pair_count, color);
    }
} rdebug_render_impl;
dxDebugRender* rdebug_render = &rdebug_render_impl;

#endif
