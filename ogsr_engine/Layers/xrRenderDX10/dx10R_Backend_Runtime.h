#pragma once

#include "StateManager/dx10StateManager.h"
#include "StateManager/dx10ShaderResourceStateCache.h"

IC void CBackend::set_pass_targets(const ref_rt& _1, const ref_rt& _2, const ref_rt& _3, const ref_rt& _4, const ref_rt& zb)
{
    if (_1)
    {
        curr_rt_width = _1->dwWidth;
        curr_rt_height = _1->dwHeight;
    }
    else
    {
        VERIFY(zb);
        curr_rt_width = zb->dwWidth;
        curr_rt_height = zb->dwHeight;
    }

    set_RT(_1 ? _1->pRT : nullptr, 0);
    set_RT(_2 ? _2->pRT : nullptr, 1);
    set_RT(_3 ? _3->pRT : nullptr, 2);
    set_RT(_4 ? _4->pRT : nullptr, 3);

    set_ZB(zb ? zb->pZRT[context_id] : nullptr);

    const D3D_VIEWPORT viewport = {0, 0, curr_rt_width, curr_rt_height, 0.f, 1.f};
    SetViewport(viewport);
}

IC void CBackend::set_RT(ID3DRenderTargetView* RT, u32 ID)
{
    if (RT != pRT[ID])
    {
        PGO(Msg("PGO:setRT"));
        stat.target_rt++;
        pRT[ID] = RT;

        //	Mark RT array dirty
        // HW.pDevice->OMSetRenderTargets(sizeof(pRT)/sizeof(pRT[0]), pRT, 0);
        // HW.pDevice->OMSetRenderTargets(sizeof(pRT)/sizeof(pRT[0]), pRT, pZB);
        //	Reset all RT's here to allow RT to be bounded as input

        if (!m_bChangedRTorZB)
            HW.get_context(context_id)->OMSetRenderTargets(0, nullptr, nullptr);

        m_bChangedRTorZB = true;
    }
}

IC void CBackend::set_ZB(ID3DDepthStencilView* ZB)
{
    if (ZB != pZB)
    {
        PGO(Msg("PGO:setZB"));
        stat.target_zb++;
        pZB = ZB;
        // HW.pDevice->OMSetRenderTargets(0, 0, pZB);
        // HW.pDevice->OMSetRenderTargets(sizeof(pRT)/sizeof(pRT[0]), pRT, pZB);
        //	Reset all RT's here to allow RT to be bounded as input
        if (!m_bChangedRTorZB)
            HW.get_context(context_id)->OMSetRenderTargets(0, nullptr, nullptr);

        m_bChangedRTorZB = true;
    }
}

IC void CBackend::ClearRT(ID3DRenderTargetView* rt, const Fcolor& color) const
{
    HW.get_context(context_id)->ClearRenderTargetView(rt, reinterpret_cast<const FLOAT*>(&color));
}

IC void CBackend::ClearZB(ID3DDepthStencilView* zb, float depth)
{
    HW.get_context(context_id)->ClearDepthStencilView(zb, D3D_CLEAR_DEPTH, depth, 0);
}

IC void CBackend::ClearZB(ID3DDepthStencilView* zb, float depth, u8 stencil)
{
    HW.get_context(context_id)->ClearDepthStencilView(zb, D3D_CLEAR_DEPTH | D3D_CLEAR_STENCIL, depth, stencil);
}

ICF void CBackend::set_Format(SDeclaration* _decl)
{
    if (decl != _decl)
    {
        PGO(Msg("PGO:v_format:%x", _decl));
        stat.decl++;
        decl = _decl;
    }
}

ICF void CBackend::set_PS(ID3DPixelShader* _ps, LPCSTR _n)
{
    if (ps != _ps)
    {
        PGO(Msg("PGO:Pshader:%x", _ps));
        stat.ps++;
        ps = _ps;
        HW.get_context(context_id)->PSSetShader(ps, nullptr, 0);


#ifdef DEBUG
        ps_name = _n;
#endif
    }
}

ICF void CBackend::set_GS(ID3DGeometryShader* _gs, LPCSTR _n)
{
    if (gs != _gs)
    {
        PGO(Msg("PGO:Gshader:%x", _ps));
        stat.gs++;
        gs = _gs;
        HW.get_context(context_id)->GSSetShader(gs, nullptr, 0);


#ifdef DEBUG
        gs_name = _n;
#endif
    }
}

ICF void CBackend::set_HS(ID3D11HullShader* _hs, LPCSTR _n)
{
    if (hs != _hs)
    {
        PGO(Msg("PGO:Hshader:%x", _ps));
        stat.hs++;
        hs = _hs;
        HW.get_context(context_id)->HSSetShader(hs, nullptr, 0);

#ifdef DEBUG
        hs_name = _n;
#endif
    }
}

ICF void CBackend::set_DS(ID3D11DomainShader* _ds, LPCSTR _n)
{
    if (ds != _ds)
    {
        PGO(Msg("PGO:Dshader:%x", _ps));
        stat.ds++;
        ds = _ds;
        HW.get_context(context_id)->DSSetShader(ds, nullptr, 0);

#ifdef DEBUG
        ds_name = _n;
#endif
    }
}

ICF void CBackend::set_CS(ID3D11ComputeShader* _cs, LPCSTR _n)
{
    if (cs != _cs)
    {
        PGO(Msg("PGO:Cshader:%x", _ps));
        stat.cs	++;
        cs = _cs;
        HW.get_context(context_id)->CSSetShader(cs, nullptr, 0);

#ifdef DEBUG
        cs_name = _n;
#endif
    }
}

ICF void CBackend::set_VS(ID3DVertexShader* _vs, LPCSTR _n)
{
    if (vs != _vs)
    {
        PGO(Msg("PGO:Vshader:%x", _vs));
        stat.vs++;
        vs = _vs;

        HW.get_context(context_id)->VSSetShader(vs, nullptr, 0);

#ifdef DEBUG
        vs_name = _n;
#endif
    }
}

ICF void CBackend::set_VS(const ref_vs& _vs)
{
    m_pInputSignature = _vs->signature->signature;
    set_VS(_vs->sh, _vs->cName.c_str());
}

ICF bool CBackend::is_TessEnabled() const { return HW.FeatureLevel >= D3D_FEATURE_LEVEL_11_0 && (ds != nullptr || hs != nullptr); }

ICF void CBackend::set_Vertices(ID3DVertexBuffer* _vb, u32 _vb_stride)
{
    if (need_reset_vertbuf || (vb != _vb) || (vb_stride != _vb_stride))
    {
        PGO(Msg("PGO:VB:%x,%d", _vb, _vb_stride));
        stat.vb++;
        vb = _vb;
        vb_stride = _vb_stride;
        // CHK_DX			(HW.pDevice->SetStreamSource(0,vb,0,vb_stride));
        // UINT StreamNumber,
        // IDirect3DVertexBuffer9 * pStreamData,
        // UINT OffsetInBytes,
        // UINT Stride

        // UINT StartSlot,
        // UINT NumBuffers,
        // ID3DxxBuffer *const *ppVertexBuffers,
        // const UINT *pStrides,
        // const UINT *pOffsets
        const u32 iOffset = 0;
        HW.get_context(context_id)->IASetVertexBuffers(0, 1, &vb, &_vb_stride, &iOffset);
        need_reset_vertbuf = false;
    }
}

ICF void CBackend::set_Vertices_Forced(const u32 count, ID3DVertexBuffer* const* _vb, const u32* _vb_stride, const u32* iOffset)
{
    stat.vb++;

    HW.get_context(context_id)->IASetVertexBuffers(0, count, _vb, _vb_stride, iOffset);
    need_reset_vertbuf = true;
}

ICF void CBackend::set_Indices(ID3DIndexBuffer* _ib)
{
    if (ib != _ib)
    {
        PGO(Msg("PGO:IB:%x", _ib));
        stat.ib++;
        ib = _ib;
        HW.get_context(context_id)->IASetIndexBuffer(ib, DXGI_FORMAT_R16_UINT, 0);
    }
}

IC D3D_PRIMITIVE_TOPOLOGY TranslateTopology(D3DPRIMITIVETYPE T)
{
    constexpr D3D_PRIMITIVE_TOPOLOGY translateTable[]{
        D3D_PRIMITIVE_TOPOLOGY_UNDEFINED, //	None
        D3D_PRIMITIVE_TOPOLOGY_POINTLIST, //	D3DPT_POINTLIST = 1,
        D3D_PRIMITIVE_TOPOLOGY_LINELIST, //	D3DPT_LINELIST = 2,
        D3D_PRIMITIVE_TOPOLOGY_LINESTRIP, //	D3DPT_LINESTRIP = 3,
        D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, //	D3DPT_TRIANGLELIST = 4,
        D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP, //	D3DPT_TRIANGLESTRIP = 5,
        D3D_PRIMITIVE_TOPOLOGY_UNDEFINED, //	D3DPT_TRIANGLEFAN = 6,
    };

    VERIFY(T < sizeof(translateTable) / sizeof(translateTable[0]));
    VERIFY(T >= 0);

    const D3D_PRIMITIVE_TOPOLOGY result = translateTable[T];

    VERIFY(result != D3D_PRIMITIVE_TOPOLOGY_UNDEFINED);

    return result;
}

IC u32 GetIndexCount(D3DPRIMITIVETYPE T, u32 iPrimitiveCount)
{
    switch (T)
    {
    case D3DPT_POINTLIST: return iPrimitiveCount;
    case D3DPT_LINELIST: return iPrimitiveCount * 2;
    case D3DPT_LINESTRIP: return iPrimitiveCount + 1;
    case D3DPT_TRIANGLELIST: return iPrimitiveCount * 3;
    case D3DPT_TRIANGLESTRIP: return iPrimitiveCount + 2;
    default: NODEFAULT;
#ifdef DEBUG
        return 0;
#endif // #ifdef DEBUG
    }
}

IC void CBackend::ApplyPrimitieTopology(D3D_PRIMITIVE_TOPOLOGY Topology)
{
    if (m_PrimitiveTopology != Topology)
    {
        m_PrimitiveTopology = Topology;
        HW.get_context(context_id)->IASetPrimitiveTopology(m_PrimitiveTopology);
    }
}

//IC void CBackend::Compute(UINT ThreadGroupCountX, UINT ThreadGroupCountY, UINT ThreadGroupCountZ)
//{
//    stat.calls++;
//
//    SRVSManager.Apply(context_id);
//    StateManager.Apply();
//    //	State manager may alter constants
//    constants.flush();
//    HW.get_context(context_id)->Dispatch(ThreadGroupCountX, ThreadGroupCountY, ThreadGroupCountZ);
//}

IC void CBackend::Render(D3DPRIMITIVETYPE T, u32 baseV, u32 startV, u32 countV, u32 startI, u32 PC, u32 inst_cnt)
{
    D3D_PRIMITIVE_TOPOLOGY Topology = TranslateTopology(T);
    const u32 iIndexCount = GetIndexCount(T, PC);

    //!!! HACK !!!
    if (hs != nullptr || ds != nullptr)
    {
        R_ASSERT(Topology == D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        Topology = D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST;
    }

    stat.calls++;
    stat.verts += countV;
    stat.polys += PC;

    ApplyPrimitieTopology(Topology);

    SRVSManager.Apply(context_id);
    ApplyRTandZB();
    ApplyVertexLayout();
    StateManager.Apply();
    constants.flush();

    if (inst_cnt > 0)
        HW.get_context(context_id)->DrawIndexedInstanced(iIndexCount, inst_cnt, startI, baseV, 0);
    else
        HW.get_context(context_id)->DrawIndexed(iIndexCount, startI, baseV);

    PGO(Msg("PGO:DIP:%dv/%df", countV, PC));
}

IC void CBackend::Render(D3DPRIMITIVETYPE T, u32 startV, u32 PC)
{
    //	TODO: DX10: Remove triangle fan usage from the engine
    if (T == D3DPT_TRIANGLEFAN)
        return;

    const D3D_PRIMITIVE_TOPOLOGY Topology = TranslateTopology(T);
    const u32 iVertexCount = GetIndexCount(T, PC);

    stat.calls++;
    stat.verts += 3 * PC;
    stat.polys += PC;

    ApplyPrimitieTopology(Topology);

    SRVSManager.Apply(context_id);
    ApplyRTandZB();
    ApplyVertexLayout();
    StateManager.Apply();
    constants.flush();

    HW.get_context(context_id)->Draw(iVertexCount, startV);

    PGO(Msg("PGO:DIP:%dv/%df", 3 * PC, PC));
}

IC void CBackend::Render(const u32 Vcount)
{
    stat.calls++;
    stat.verts += Vcount;

    SRVSManager.Apply(context_id);
    ApplyRTandZB();

    // Unbind IA (VB, IB)
    ApplyPrimitieTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    m_pInputLayout = nullptr;
    HW.get_context(context_id)->IASetInputLayout(m_pInputLayout);

    StateManager.Apply();

    // State manager may alter constants
    constants.flush();

    HW.get_context(context_id)->Draw(Vcount, 0);
}

IC void CBackend::set_Geometry(SGeometry* _geom)
{
    set_Format(&*_geom->dcl);

    set_Vertices(_geom->vb, _geom->vb_stride);
    set_Indices(_geom->ib);
}

IC void CBackend::set_Scissor(Irect* R)
{
    if (R)
    {
        // CHK_DX		(HW.pDevice->SetRenderState(D3DRS_SCISSORTESTENABLE,TRUE));
        StateManager.EnableScissoring();
        const RECT* clip = (RECT*)R;
        HW.get_context(context_id)->RSSetScissorRects(1, clip);
    }
    else
    {
        // CHK_DX		(HW.pDevice->SetRenderState(D3DRS_SCISSORTESTENABLE,FALSE));
        StateManager.EnableScissoring(FALSE);
        HW.get_context(context_id)->RSSetScissorRects(0, nullptr);
    }
}

IC void CBackend::SetViewport(const D3D_VIEWPORT& viewport) const
{
    HW.get_context(context_id)->RSSetViewports(1, &viewport);
}

IC void CBackend::set_Stencil(u32 _enable, u32 _func, u32 _ref, u32 _mask, u32 _writemask, u32 _fail, u32 _pass, u32 _zfail)
{
    StateManager.SetStencil(_enable, _func, _ref, _mask, _writemask, _fail, _pass, _zfail);
    // Simple filter
    // if (stencil_enable		!= _enable)		{ stencil_enable=_enable;		CHK_DX(HW.pDevice->SetRenderState	( D3DRS_STENCILENABLE,		_enable				)); }
    // if (!stencil_enable)					return;
    // if (stencil_func		!= _func)		{ stencil_func=_func;			CHK_DX(HW.pDevice->SetRenderState	( D3DRS_STENCILFUNC,		_func				)); }
    // if (stencil_ref			!= _ref)		{ stencil_ref=_ref;				CHK_DX(HW.pDevice->SetRenderState	( D3DRS_STENCILREF,			_ref				)); }
    // if (stencil_mask		!= _mask)		{ stencil_mask=_mask;			CHK_DX(HW.pDevice->SetRenderState	( D3DRS_STENCILMASK,		_mask				)); }
    // if (stencil_writemask	!= _writemask)	{ stencil_writemask=_writemask;	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_STENCILWRITEMASK,	_writemask			)); }
    // if (stencil_fail		!= _fail)		{ stencil_fail=_fail;			CHK_DX(HW.pDevice->SetRenderState	( D3DRS_STENCILFAIL,		_fail				)); }
    // if (stencil_pass		!= _pass)		{ stencil_pass=_pass;			CHK_DX(HW.pDevice->SetRenderState	( D3DRS_STENCILPASS,		_pass				)); }
    // if (stencil_zfail		!= _zfail)		{ stencil_zfail=_zfail;			CHK_DX(HW.pDevice->SetRenderState	( D3DRS_STENCILZFAIL,		_zfail				)); }
}

IC void CBackend::set_Z(u32 _enable)
{
    StateManager.SetDepthEnable(_enable);
    // if (z_enable != _enable)
    //{
    //	z_enable=_enable;
    //	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_ZENABLE, _enable ));
    // }
}

IC void CBackend::set_ZFunc(u32 _func)
{
    StateManager.SetDepthFunc(_func);
    // if (z_func!=_func)
    //{
    //	z_func = _func;
    //	CHK_DX(HW.pDevice->SetRenderState( D3DRS_ZFUNC, _func));
    // }
}

IC void CBackend::set_AlphaRef(u32 _value)
{
    //	TODO: DX10: Implement rasterizer state update to support alpha ref
    VERIFY(!"Not implemented.");
    // if (alpha_ref != _value)
    //{
    //	alpha_ref = _value;
    //	CHK_DX(HW.pDevice->SetRenderState(D3DRS_ALPHAREF,_value));
    // }
}

IC void CBackend::set_ColorWriteEnable(u32 _mask)
{
    StateManager.SetColorWriteEnable(_mask);
    // if (colorwrite_mask		!= _mask)		{
    //	colorwrite_mask=_mask;
    //	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_COLORWRITEENABLE,	_mask	));
    //	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_COLORWRITEENABLE1,	_mask	));
    //	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_COLORWRITEENABLE2,	_mask	));
    //	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_COLORWRITEENABLE3,	_mask	));
    // }
}

ICF void CBackend::set_CullMode(u32 _mode) { StateManager.SetCullMode(_mode); }

ICF void CBackend::set_FillMode(u32 _mode) { StateManager.SetFillMode(_mode); }

IC void CBackend::ApplyVertexLayout()
{
    VERIFY(vs);
    VERIFY(decl);
    VERIFY(m_pInputSignature);

    auto it = decl->vs_to_layout.find(m_pInputSignature);
    if (it == decl->vs_to_layout.end())
    {
        ID3DInputLayout* pLayout;

        CHK_DX(HW.pDevice->CreateInputLayout(&decl->dx10_dcl_code[0], 
            decl->dx10_dcl_code.size() - 1, 
            m_pInputSignature->GetBufferPointer(), 
            m_pInputSignature->GetBufferSize(),
            &pLayout));

        it = decl->vs_to_layout.emplace(m_pInputSignature, pLayout).first;
    }

    if (m_pInputLayout != it->second)
    {
        m_pInputLayout = it->second;
        HW.get_context(context_id)->IASetInputLayout(m_pInputLayout);
    }
}

IC bool CBackend::CBuffersNeedUpdate(ref_cbuffer buf1[MaxCBuffers], ref_cbuffer buf2[MaxCBuffers], u32& uiMin, u32& uiMax)
{
    bool bRes = false;
    int i = 0;
    while ((i < MaxCBuffers) && (buf1[i] == buf2[i]))
        ++i;

    uiMin = i;

    for (; i < MaxCBuffers; ++i)
    {
        if (buf1[i] != buf2[i])
        {
            bRes = true;
            uiMax = i;
        }
    }

    return bRes;
}

IC void CBackend::set_Constants(R_constant_table* C)
{
    // caching
    if (ctable == C)
        return;
    ctable = C;

    xforms.unmap();
    hemi.unmap();
    lod.unmap();

    StateManager.UnmapConstants();

    if (nullptr == C)
        return;

    PGO(Msg("PGO:c-table"));

    bool was_updated = false;

    //	Setup constant tables
    {
        ref_cbuffer aPixelConstants[MaxCBuffers];
        ref_cbuffer aVertexConstants[MaxCBuffers];
        ref_cbuffer aGeometryConstants[MaxCBuffers];
        ref_cbuffer aHullConstants[MaxCBuffers];
        ref_cbuffer aDomainConstants[MaxCBuffers];
        ref_cbuffer aComputeConstants[MaxCBuffers];

        for (int i = 0; i < MaxCBuffers; ++i)
        {
            aPixelConstants[i] = m_aPixelConstants[i];
            aVertexConstants[i] = m_aVertexConstants[i];
            aGeometryConstants[i] = m_aGeometryConstants[i];

            aHullConstants[i] = m_aHullConstants[i];
            aDomainConstants[i] = m_aDomainConstants[i];
            aComputeConstants[i] = m_aComputeConstants[i];

            m_aPixelConstants[i] = nullptr;
            m_aVertexConstants[i] = nullptr;
            m_aGeometryConstants[i] = nullptr;

            m_aHullConstants[i] = nullptr;
            m_aDomainConstants[i] = nullptr;
            m_aComputeConstants[i] = nullptr;
        }

        R_constant_table::cb_table::iterator it = C->m_CBTable[context_id].begin();
        const R_constant_table::cb_table::iterator end = C->m_CBTable[context_id].end();

        for (; it != end; ++it)
        {
            // ID3DxxBuffer*	pBuffer = (it->second)->GetBuffer();
            const u32 uiBufferIndex = it->first;

            if ((uiBufferIndex & CB_BufferTypeMask) == CB_BufferPixelShader)
            {
                R_ASSERT((uiBufferIndex & CB_BufferIndexMask) < MaxCBuffers);
                m_aPixelConstants[uiBufferIndex & CB_BufferIndexMask] = it->second;
            }
            else if ((uiBufferIndex & CB_BufferTypeMask) == CB_BufferVertexShader)
            {
                R_ASSERT((uiBufferIndex & CB_BufferIndexMask) < MaxCBuffers);
                m_aVertexConstants[uiBufferIndex & CB_BufferIndexMask] = it->second;
            }
            else if ((uiBufferIndex & CB_BufferTypeMask) == CB_BufferGeometryShader)
            {
                R_ASSERT((uiBufferIndex & CB_BufferIndexMask) < MaxCBuffers);
                m_aGeometryConstants[uiBufferIndex & CB_BufferIndexMask] = it->second;
            }
            else if ((uiBufferIndex & CB_BufferTypeMask) == CB_BufferHullShader)
            {
                R_ASSERT((uiBufferIndex & CB_BufferIndexMask) < MaxCBuffers);
                m_aHullConstants[uiBufferIndex & CB_BufferIndexMask] = it->second;
            }
            else if ((uiBufferIndex & CB_BufferTypeMask) == CB_BufferDomainShader)
            {
                R_ASSERT((uiBufferIndex & CB_BufferIndexMask) < MaxCBuffers);
                m_aDomainConstants[uiBufferIndex & CB_BufferIndexMask] = it->second;
            }
            else if ((uiBufferIndex & CB_BufferTypeMask) == CB_BufferComputeShader)
            {
                R_ASSERT((uiBufferIndex & CB_BufferIndexMask) < MaxCBuffers);
                m_aComputeConstants[uiBufferIndex & CB_BufferIndexMask] = it->second;
            }
            else
                FATAL("Invalid enumeration");
        }

        ID3DBuffer* tempBuffer[MaxCBuffers];

        u32 uiMin;
        u32 uiMax;

        if (CBuffersNeedUpdate(m_aPixelConstants, aPixelConstants, uiMin, uiMax))
        {
            ++uiMax;

            for (u32 i = uiMin; i < uiMax; ++i)
            {
                if (m_aPixelConstants[i])
                    tempBuffer[i] = m_aPixelConstants[i]->GetBuffer();
                else
                    tempBuffer[i] = nullptr;
            }
            HW.get_context(context_id)->PSSetConstantBuffers(uiMin, uiMax - uiMin, &tempBuffer[uiMin]);
            was_updated = true;
        }

        if (CBuffersNeedUpdate(m_aVertexConstants, aVertexConstants, uiMin, uiMax))
        {
            ++uiMax;

            for (u32 i = uiMin; i < uiMax; ++i)
            {
                if (m_aVertexConstants[i])
                    tempBuffer[i] = m_aVertexConstants[i]->GetBuffer();
                else
                    tempBuffer[i] = nullptr;
            }
            HW.get_context(context_id)->VSSetConstantBuffers(uiMin, uiMax - uiMin, &tempBuffer[uiMin]);
            was_updated = true;
        }

        if (CBuffersNeedUpdate(m_aGeometryConstants, aGeometryConstants, uiMin, uiMax))
        {
            ++uiMax;

            for (u32 i = uiMin; i < uiMax; ++i)
            {
                if (m_aGeometryConstants[i])
                    tempBuffer[i] = m_aGeometryConstants[i]->GetBuffer();
                else
                    tempBuffer[i] = nullptr;
            }
            HW.get_context(context_id)->GSSetConstantBuffers(uiMin, uiMax - uiMin, &tempBuffer[uiMin]);
            was_updated = true;
        }

        if (CBuffersNeedUpdate(m_aHullConstants, aHullConstants, uiMin, uiMax))
        {
            ++uiMax;

            for (u32 i = uiMin; i < uiMax; ++i)
            {
                if (m_aHullConstants[i])
                    tempBuffer[i] = m_aHullConstants[i]->GetBuffer();
                else
                    tempBuffer[i] = nullptr;
            }
            HW.get_context(context_id)->HSSetConstantBuffers(uiMin, uiMax - uiMin, &tempBuffer[uiMin]);
            was_updated = true;
        }

        if (CBuffersNeedUpdate(m_aDomainConstants, aDomainConstants, uiMin, uiMax))
        {
            ++uiMax;

            for (u32 i = uiMin; i < uiMax; ++i)
            {
                if (m_aDomainConstants[i])
                    tempBuffer[i] = m_aDomainConstants[i]->GetBuffer();
                else
                    tempBuffer[i] = nullptr;
            }
            HW.get_context(context_id)->DSSetConstantBuffers(uiMin, uiMax - uiMin, &tempBuffer[uiMin]);
            was_updated = true;
        }

        if (CBuffersNeedUpdate(m_aComputeConstants, aComputeConstants, uiMin, uiMax))
        {
            ++uiMax;

            for (u32 i = uiMin; i < uiMax; ++i)
            {
                if (m_aComputeConstants[i])
                    tempBuffer[i] = m_aComputeConstants[i]->GetBuffer();
                else
                    tempBuffer[i] = nullptr;
            }
            HW.get_context(context_id)->CSSetConstantBuffers(uiMin, uiMax - uiMin, &tempBuffer[uiMin]);
            was_updated = true;
        }
    }

    if (!was_updated)
    {
        //Msg("can skip const set !!");
    }

    //if (was_updated)
    {
        // process constant-loaders
        R_constant_table::c_table::iterator it = C->table.begin();
        const R_constant_table::c_table::iterator end = C->table.end();
        for (; it != end; ++it)
        {
            R_constant* constant = &**it;
            VERIFY(constant);
            if (constant && constant->handler)
            {
                if (was_updated || constant->handler->bCapture) // for future optimization
                    constant->handler->setup(*this, constant);
            }
        }
    }
}

ICF void CBackend::ApplyRTandZB()
{
    if (m_bChangedRTorZB)
    {
        m_bChangedRTorZB = false;
        HW.get_context(context_id)->OMSetRenderTargets(std::size(pRT), pRT, pZB);
    }
}

IC void CBackend::get_ConstantDirect(const shared_str& n, u32 DataSize, void** pVData, void** pGData, void** pPData, bool assert)
{
    const ref_constant C = get_c(n);

    if (assert && !C)
    {
        if (ctable)
            ctable->dbg_dump(context_id);
        else
            Msg("! ERROR: ctable is null!");

        ASSERT_FMT(FALSE, "! ERROR: failed to find shader constant '%s'", n.c_str());
    }

    if (C)
    {
        constants.access_direct(&*C, DataSize, pVData, pGData, pPData);
    }
    else
    {
        if (pVData)
            *pVData = nullptr;
        if (pGData)
            *pGData = nullptr;
        if (pPData)
            *pPData = nullptr;
    }
}
