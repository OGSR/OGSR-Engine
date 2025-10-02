#include "stdafx.h"

#include "../../xrCDB/frustum.h"

#include "../xrRenderDX10/StateManager/dx10StateManager.h"
#include "../xrRenderDX10/StateManager/dx10ShaderResourceStateCache.h"

void CBackend::OnFrameEnd()
{
    // if (context_id == CHW::IMM_CTX_ID)
        HW.get_context(context_id)->ClearState();

    Invalidate();
}

void CBackend::OnFrameBegin()
{
    PGO(Msg("PGO:*****frame[%d]*****", Device.dwFrame));
    Invalidate();
    //	DX9 sets base rt nd base zb by default
    RImplementation.rmNormal(*this);
    set_RT(RImplementation.Target->get_base_rt());
    set_ZB(RImplementation.Target->rt_Base_Depth->pZRT[context_id]);
    set_Stencil(FALSE);

    set_CullMode(CULL_CW); // ???
    set_CullMode(CULL_CCW);

    Memory.mem_fill(&stat, 0, sizeof(stat));
}

void CBackend::Invalidate()
{
    pRT[0] = nullptr;
    pRT[1] = nullptr;
    pRT[2] = nullptr;
    pRT[3] = nullptr;

    pZB = nullptr;

    decl = nullptr;
    vb = nullptr;
    ib = nullptr;
    vb_stride = 0;

    state = nullptr;
    ps = nullptr;
    vs = nullptr;
    gs = nullptr;

    hs = nullptr;
    ds = nullptr;
    cs = nullptr;

    ctable = nullptr;

    T = nullptr;
    M = nullptr;
    C = nullptr;

    //	Since constant buffers are unmapped (for DirecX 10)
    //	transform setting handlers should be unmapped too.
    xforms.unmap();

    m_pInputLayout = nullptr;
    m_PrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
    m_bChangedRTorZB = false;
    m_pInputSignature = nullptr;
    for (int i = 0; i < MaxCBuffers; ++i)
    {
        m_aPixelConstants[i] = nullptr;
        m_aVertexConstants[i] = nullptr;
        m_aGeometryConstants[i] = nullptr;
        m_aHullConstants[i] = nullptr;
        m_aDomainConstants[i] = nullptr;
        m_aComputeConstants[i] = nullptr;
    }
    StateManager.Reset();
    //	Redundant call. Just no note that we need to unmap const
    //	if we create dedicated class.
    StateManager.UnmapConstants();
    SRVSManager.ResetDeviceState();

    for (u32 gs_it = 0; gs_it < CTexture::mtMaxGeometryShaderTextures;)
        textures_gs[gs_it++] = nullptr;
    for (u32 hs_it = 0; hs_it < CTexture::mtMaxHullShaderTextures;)
        textures_hs[hs_it++] = nullptr;
    for (u32 ds_it = 0; ds_it < CTexture::mtMaxDomainShaderTextures;)
        textures_ds[ds_it++] = nullptr;
    for (u32 cs_it = 0; cs_it < CTexture::mtMaxComputeShaderTextures;)
        textures_cs[cs_it++] = nullptr;
    for (u32 ps_it = 0; ps_it < CTexture::mtMaxPixelShaderTextures;)
        textures_ps[ps_it++] = nullptr;
    for (u32 vs_it = 0; vs_it < CTexture::mtMaxVertexShaderTextures;)
        textures_vs[vs_it++] = nullptr;
}

void CBackend::set_Textures(STextureList* _T)
{
    // TODO: expose T invalidation method

    if (T == _T)
    {
        if (_T)
        {
            bool changed = false;

            STextureList::iterator _it = _T->begin();
            const STextureList::iterator _end = _T->end();

            for (; _it != _end; ++_it)
            {
                std::pair<u32, ref_texture>& loader = *_it;
                const u32 load_id = loader.first;
                CTexture* load_surf = &*loader.second;

                if (load_id < CTexture::rstVertex)
                {
                    if ((load_surf && (load_surf->last_slice != load_surf->curr_slice)))
                    {
                        changed = true;
                        break;
                    }
                }
            }

            if (!changed)
                return;
        }
        else
            return;
    }

    T = _T;

    //	If resources weren't set at all we should clear from resource #0.
    int _last_ps = -1;
    int _last_vs = -1;
    int _last_gs = -1;
    int _last_hs = -1;
    int _last_ds = -1;
    int _last_cs = -1;

    STextureList::iterator _it = _T->begin();
    const STextureList::iterator _end = _T->end();

    for (; _it != _end; ++_it)
    {
        std::pair<u32, ref_texture>& loader = *_it;
        const u32 load_id = loader.first;
        CTexture* load_surf = &*loader.second;

        if (load_id < CTexture::rstVertex)
        {
            //	Set up pixel shader resources
            R_ASSERT(load_id < CTexture::mtMaxPixelShaderTextures);
            // ordinary pixel surface
            if ((int)load_id > _last_ps)
                _last_ps = load_id;
            if (textures_ps[load_id] != load_surf || (load_surf && (load_surf->last_slice != load_surf->curr_slice)))
            {
                textures_ps[load_id] = load_surf;
                stat.textures++;
                if (load_surf)
                {
                    PGO(Msg("PGO:tex%d:%s", load_id, load_surf->cName.c_str()));
                    load_surf->bind(*this, load_id);
                    load_surf->last_slice = load_surf->curr_slice;
                }
            }
        }
        else if (load_id < CTexture::rstGeometry)
        {
            //	Set up pixel shader resources
            R_ASSERT(load_id < CTexture::rstVertex + CTexture::mtMaxVertexShaderTextures);

            // vertex only //d-map or vertex
            const u32 load_id_remapped = load_id - CTexture::rstVertex;
            if ((int)load_id_remapped > _last_vs)
                _last_vs = load_id_remapped;
            if (textures_vs[load_id_remapped] != load_surf)
            {
                textures_vs[load_id_remapped] = load_surf;
                stat.textures++;
                if (load_surf)
                {
                    PGO(Msg("PGO:tex%d:%s", load_id, load_surf->cName.c_str()));
                    load_surf->bind(*this, load_id);
                }
            }
        }
        else if (load_id < CTexture::rstHull)
        {
            //	Set up pixel shader resources
            R_ASSERT(load_id < CTexture::rstGeometry + CTexture::mtMaxGeometryShaderTextures);

            // vertex only //d-map or vertex
            const u32 load_id_remapped = load_id - CTexture::rstGeometry;
            if ((int)load_id_remapped > _last_gs)
                _last_gs = load_id_remapped;
            if (textures_gs[load_id_remapped] != load_surf)
            {
                textures_gs[load_id_remapped] = load_surf;
                stat.textures++;
                if (load_surf)
                {
                    PGO(Msg("PGO:tex%d:%s", load_id, load_surf->cName.c_str()));
                    load_surf->bind(*this, load_id);
                }
            }
        }
        else if (load_id < CTexture::rstDomain)
        {
            //	Set up pixel shader resources
            R_ASSERT(load_id < CTexture::rstHull + CTexture::mtMaxHullShaderTextures);

            // vertex only //d-map or vertex
            const u32 load_id_remapped = load_id - CTexture::rstHull;
            if ((int)load_id_remapped > _last_hs)
                _last_hs = load_id_remapped;
            if (textures_hs[load_id_remapped] != load_surf)
            {
                textures_hs[load_id_remapped] = load_surf;
                stat.textures++;
                if (load_surf)
                {
                    PGO(Msg("PGO:tex%d:%s", load_id, load_surf->cName.c_str()));
                    load_surf->bind(*this, load_id);
                }
            }
        }
        else if (load_id < CTexture::rstCompute)
        {
            //	Set up pixel shader resources
            R_ASSERT(load_id < CTexture::rstDomain + CTexture::mtMaxDomainShaderTextures);

            // vertex only //d-map or vertex
            const u32 load_id_remapped = load_id - CTexture::rstDomain;
            if ((int)load_id_remapped > _last_ds)
                _last_ds = load_id_remapped;
            if (textures_ds[load_id_remapped] != load_surf)
            {
                textures_ds[load_id_remapped] = load_surf;
                stat.textures++;
                if (load_surf)
                {
                    PGO(Msg("PGO:tex%d:%s", load_id, load_surf->cName.c_str()));
                    load_surf->bind(*this, load_id);
                }
            }
        }
        else if (load_id < CTexture::rstInvalid)
        {
            //	Set up pixel shader resources
            R_ASSERT(load_id < CTexture::rstCompute + CTexture::mtMaxComputeShaderTextures);

            // vertex only //d-map or vertex
            const u32 load_id_remapped = load_id - CTexture::rstCompute;
            if ((int)load_id_remapped > _last_cs)
                _last_cs = load_id_remapped;
            if (textures_cs[load_id_remapped] != load_surf)
            {
                textures_cs[load_id_remapped] = load_surf;
                stat.textures++;
                if (load_surf)
                {
                    PGO(Msg("PGO:tex%d:%s", load_id, load_surf->cName.c_str()));
                    load_surf->bind(*this, load_id);
                }
            }
        }
        else
            VERIFY("Invalid enum");
    }

    // clear remaining stages (PS)
    for (++_last_ps; _last_ps < CTexture::mtMaxPixelShaderTextures; _last_ps++)
    {
        if (!textures_ps[_last_ps])
            continue;

        textures_ps[_last_ps] = nullptr;
        //	TODO: DX10: Optimise: set all resources at once
        ID3DShaderResourceView* pRes = nullptr;
        SRVSManager.SetPSResource(_last_ps, pRes);
    }
    // clear remaining stages (VS)
    for (++_last_vs; _last_vs < CTexture::mtMaxVertexShaderTextures; _last_vs++)
    {
        if (!textures_vs[_last_vs])
            continue;

        textures_vs[_last_vs] = nullptr;
        //	TODO: DX10: Optimise: set all resources at once
        ID3DShaderResourceView* pRes = nullptr;
        SRVSManager.SetVSResource(_last_vs, pRes);
    }

    // clear remaining stages (VS)
    for (++_last_gs; _last_gs < CTexture::mtMaxGeometryShaderTextures; _last_gs++)
    {
        if (!textures_gs[_last_gs])
            continue;

        textures_gs[_last_gs] = nullptr;

        //	TODO: DX10: Optimise: set all resources at once
        ID3DShaderResourceView* pRes = nullptr;
        SRVSManager.SetGSResource(_last_gs, pRes);
    }
    for (++_last_hs; _last_hs < CTexture::mtMaxHullShaderTextures; _last_hs++)
    {
        if (!textures_hs[_last_hs])
            continue;

        textures_hs[_last_hs] = nullptr;

        //	TODO: DX10: Optimise: set all resources at once
        ID3DShaderResourceView* pRes = nullptr;
        SRVSManager.SetHSResource(_last_hs, pRes);
    }
    for (++_last_ds; _last_ds < CTexture::mtMaxDomainShaderTextures; _last_ds++)
    {
        if (!textures_ds[_last_ds])
            continue;

        textures_ds[_last_ds] = nullptr;

        //	TODO: DX10: Optimise: set all resources at once
        ID3DShaderResourceView* pRes = nullptr;
        SRVSManager.SetDSResource(_last_ds, pRes);
    }
    for (++_last_cs; _last_cs < CTexture::mtMaxComputeShaderTextures; _last_cs++)
    {
        if (!textures_cs[_last_cs])
            continue;

        textures_cs[_last_cs] = nullptr;

        //	TODO: DX10: Optimise: set all resources at once
        ID3DShaderResourceView* pRes = nullptr;
        SRVSManager.SetCSResource(_last_cs, pRes);
    }
}

extern float r__dtex_range;

void CBackend::apply_lmaterial()
{
    ZoneScoped;

    const R_constant* C = get_c(c_sbase)._get(); // get sampler
    if (nullptr == C)
        return;

    VERIFY(RC_dest_sampler == C->destination);
    VERIFY(RC_dx10texture == C->type);
    const CTexture* T = get_ActiveTexture(u32(C->samp.index));
    if (!T)
        return;

    float mtl = T->m_material;
#ifdef DEBUG
    if (ps_r2_ls_flags.test(R2FLAG_GLOBALMATERIAL))
        mtl = ps_r2_gmaterial;
#endif
    if (!T->m_is_hot)
        hemi.set_hotness(0.f, 0.f, 0.f, 0.f);

    hemi.set_material(o_hemi, o_sun, 0, (mtl + .5f) / 4.f);
    hemi.set_pos_faces(o_hemi_cube[CROS_impl::CUBE_FACE_POS_X], o_hemi_cube[CROS_impl::CUBE_FACE_POS_Y], o_hemi_cube[CROS_impl::CUBE_FACE_POS_Z]);
    hemi.set_neg_faces(o_hemi_cube[CROS_impl::CUBE_FACE_NEG_X], o_hemi_cube[CROS_impl::CUBE_FACE_NEG_Y], o_hemi_cube[CROS_impl::CUBE_FACE_NEG_Z]);

    const float scale = T->m_detail_scale;
    hemi.set_scale(scale, scale, scale, 1 / r__dtex_range);
}
