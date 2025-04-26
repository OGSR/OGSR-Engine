#include "stdafx.h"
#include "dx10StateManager.h"

#include "../dx10StateUtils.h"
#include "dx10StateCache.h"

//	DX10: TODO: Implement alpha referense control

dx10StateManager::dx10StateManager(CBackend& cmd_list_in) : cmd_list(cmd_list_in)
{
    //  If dx11StateManager would ever own any object
    //  implement correct state manager
    Reset();
}

// dx10StateManager::~dx10StateManager()
//{
//	Don't own any object so no release is needed
//}

//	Set all states to default
void dx10StateManager::Reset()
{
    UnmapConstants();

    m_pRState = nullptr;
    m_pDepthStencilState = nullptr;
    m_pBlendState = nullptr;

    m_uiStencilRef = 0;
    m_uiAlphaRef = 0;

    m_bRSNeedApply = true;
    m_bDSSNeedApply = true;
    m_bBSNeedApply = true;

    m_bRSChanged = false;
    m_bDSSChanged = false;
    m_bBSChanged = false;

    m_bRDInvalid = false;
    m_bDSDInvalid = false;
    m_bBDInvalid = false;

    dx10StateUtils::ResetDescription(m_RDesc);
    dx10StateUtils::ResetDescription(m_DSDesc);
    dx10StateUtils::ResetDescription(m_BDesc);

    m_bOverrideScissoring = false;
    m_bOverrideScissoringValue = FALSE;
    m_uiSampleMask = 0xffffffff;
}

void dx10StateManager::UnmapConstants() { m_cAlphaRef = nullptr; }

void dx10StateManager::SetRasterizerState(ID3DRasterizerState* pRState)
{
    m_bRSChanged = false;
    m_bRDInvalid = true;

    if (pRState != m_pRState)
    {
        m_pRState = pRState;
        m_bRSNeedApply = true;
    }

    if (m_bOverrideScissoring)
        EnableScissoring(m_bOverrideScissoringValue);
}

void dx10StateManager::SetDepthStencilState(ID3DDepthStencilState* pDSState)
{
    m_bDSSChanged = false;
    m_bDSDInvalid = true;

    if (pDSState != m_pDepthStencilState)
    {
        m_pDepthStencilState = pDSState;
        m_bDSSNeedApply = true;
    }
}

void dx10StateManager::SetBlendState(ID3DBlendState* pBlendState)
{
    m_bBSChanged = false;
    m_bBDInvalid = true;

    if (pBlendState != m_pBlendState)
    {
        m_pBlendState = pBlendState;
        m_bBSNeedApply = true;
    }
}

void dx10StateManager::SetStencilRef(UINT uiStencilRef)
{
    if (m_uiStencilRef != uiStencilRef)
    {
        m_uiStencilRef = uiStencilRef;
        m_bDSSNeedApply = true;
    }
}

void dx10StateManager::SetAlphaRef(UINT uiAlphaRef)
{
    if (m_uiAlphaRef != uiAlphaRef)
    {
        m_uiAlphaRef = uiAlphaRef;
        if (m_cAlphaRef)
            cmd_list.set_c(m_cAlphaRef, (float)m_uiAlphaRef / 255.0f);
    }
}

void dx10StateManager::BindAlphaRef(R_constant* C)
{
    m_cAlphaRef = C;
    if (m_cAlphaRef)
        cmd_list.set_c(m_cAlphaRef, (float)m_uiAlphaRef / 255.0f);
}

void dx10StateManager::ValidateRDesc()
{
    if (m_bRDInvalid)
    {
        if (m_pRState)
            m_pRState->GetDesc(&m_RDesc);
        else
            dx10StateUtils::ResetDescription(m_RDesc);

        m_bRDInvalid = false;
    }
}

void dx10StateManager::ValidateDSDesc()
{
    if (m_bDSDInvalid)
    {
        if (m_pDepthStencilState)
            m_pDepthStencilState->GetDesc(&m_DSDesc);
        else
            dx10StateUtils::ResetDescription(m_DSDesc);

        m_bDSDInvalid = false;
    }
}

void dx10StateManager::ValidateBDesc()
{
    if (m_bBDInvalid)
    {
        if (m_pBlendState)
            m_pBlendState->GetDesc(&m_BDesc);
        else
            dx10StateUtils::ResetDescription(m_BDesc);

        m_bBDInvalid = false;
    }
}

//	Sends states to DX10 runtime, creates new state objects if nessessary
void dx10StateManager::Apply()
{
    auto* d3d_context = HW.get_context(cmd_list.context_id);

    //	Apply rasterizer state
    if (m_bRSNeedApply || m_bRSChanged)
    {
        if (m_bRSChanged)
        {
            m_pRState = RSManager.GetState(m_RDesc);
            m_bRSChanged = false;
        }

        d3d_context->RSSetState(m_pRState);
        m_bRSNeedApply = false;
    }

    //	Apply depth stencil state
    if (m_bDSSNeedApply || m_bDSSChanged)
    {
        if (m_bDSSChanged)
        {
            m_pDepthStencilState = DSSManager.GetState(m_DSDesc);
            m_bDSSChanged = false;
        }

        d3d_context->OMSetDepthStencilState(m_pDepthStencilState, m_uiStencilRef);
        m_bDSSNeedApply = false;
    }

    //	Apply blend state
    if (m_bBSNeedApply || m_bBSChanged)
    {
        if (m_bBSChanged)
        {
            m_pBlendState = BSManager.GetState(m_BDesc);
            m_bBSChanged = false;
        }

        constexpr FLOAT BlendFactor[4]{};

        d3d_context->OMSetBlendState(m_pBlendState, BlendFactor, m_uiSampleMask);
        m_bBSNeedApply = false;
    }
}

void dx10StateManager::SetStencil(u32 Enable, u32 Func, u32 Ref, u32 Mask, u32 WriteMask, u32 Fail, u32 Pass, u32 ZFail)
{
    ValidateDSDesc();

    // Simple filter
    const BOOL BEnable = (BOOL)Enable;
    if (m_DSDesc.StencilEnable != BEnable)
    {
        m_bDSSChanged = true;
        m_DSDesc.StencilEnable = BEnable;
    }

    if (!m_DSDesc.StencilEnable)
        return;

    const D3D_COMPARISON_FUNC SFunc = dx10StateUtils::ConvertCmpFunction(D3DCMPFUNC(Func));

    if ((m_DSDesc.FrontFace.StencilFunc != SFunc) || (m_DSDesc.BackFace.StencilFunc != SFunc))
    {
        m_bDSSChanged = true;
        m_DSDesc.FrontFace.StencilFunc = SFunc;
        m_DSDesc.BackFace.StencilFunc = SFunc;
    }

    SetStencilRef(Ref);

    UINT8 SMask = (UINT8)Mask;
    if (m_DSDesc.StencilReadMask != SMask)
    {
        m_bDSSChanged = true;
        m_DSDesc.StencilReadMask = SMask;
    }

    SMask = (UINT8)WriteMask;
    if (m_DSDesc.StencilWriteMask != SMask)
    {
        m_bDSSChanged = true;
        m_DSDesc.StencilWriteMask = SMask;
    }

    D3D_STENCIL_OP SOp = dx10StateUtils::ConvertStencilOp(D3DSTENCILOP(Fail));
    if ((m_DSDesc.FrontFace.StencilFailOp != SOp) || (m_DSDesc.BackFace.StencilFailOp != SOp))
    {
        m_bDSSChanged = true;
        m_DSDesc.FrontFace.StencilFailOp = SOp;
        m_DSDesc.BackFace.StencilFailOp = SOp;
    }

    SOp = dx10StateUtils::ConvertStencilOp(D3DSTENCILOP(Pass));
    if ((m_DSDesc.FrontFace.StencilPassOp != SOp) || (m_DSDesc.BackFace.StencilPassOp != SOp))
    {
        m_bDSSChanged = true;
        m_DSDesc.FrontFace.StencilPassOp = SOp;
        m_DSDesc.BackFace.StencilPassOp = SOp;
    }

    SOp = dx10StateUtils::ConvertStencilOp(D3DSTENCILOP(ZFail));
    if ((m_DSDesc.FrontFace.StencilDepthFailOp != SOp) || (m_DSDesc.BackFace.StencilDepthFailOp != SOp))
    {
        m_bDSSChanged = true;
        m_DSDesc.FrontFace.StencilDepthFailOp = SOp;
        m_DSDesc.BackFace.StencilDepthFailOp = SOp;
    }
}

void dx10StateManager::SetDepthFunc(u32 Func)
{
    ValidateDSDesc();

    // if (z_func!=_func)
    //{
    //	z_func = _func;
    //	CHK_DX(HW.pDevice->SetRenderState( D3DRS_ZFUNC, _func));
    // }

    const D3D_COMPARISON_FUNC DFunc = dx10StateUtils::ConvertCmpFunction(D3DCMPFUNC(Func));
    if (m_DSDesc.DepthFunc != DFunc)
    {
        m_bDSSChanged = true;
        m_DSDesc.DepthFunc = DFunc;
    }
}

void dx10StateManager::SetDepthEnable(u32 Enable)
{
    ValidateDSDesc();

    // if (z_enable != _enable)
    //{
    //	z_enable=_enable;
    //	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_ZENABLE, _enable ));
    // }

    const BOOL BEnable = (BOOL)Enable;
    if (m_DSDesc.DepthEnable != BEnable)
    {
        m_bDSSChanged = true;
        m_DSDesc.DepthEnable = BEnable;
    }
}

void dx10StateManager::SetColorWriteEnable(u32 WriteMask)
{
    ValidateBDesc();

    // if (colorwrite_mask		!= _mask)		{
    //	colorwrite_mask=_mask;
    //	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_COLORWRITEENABLE,	_mask	));
    //	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_COLORWRITEENABLE1,	_mask	));
    //	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_COLORWRITEENABLE2,	_mask	));
    //	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_COLORWRITEENABLE3,	_mask	));
    // }

    const UINT8 WMask = (UINT8)WriteMask;

    bool bNeedUpdate = false;
    for (int i = 0; i < 4; ++i)
    {
        if (m_BDesc.RenderTarget[i].RenderTargetWriteMask != WMask)
            bNeedUpdate = true;
    }

    if (bNeedUpdate)
    {
        m_bBSChanged = true;
        for (int i = 0; i < 4; ++i)
            m_BDesc.RenderTarget[i].RenderTargetWriteMask = WMask;
    }
}

void dx10StateManager::SetSampleMask(u32 SampleMask)
{
    if (m_uiSampleMask != SampleMask)
    {
        m_uiSampleMask = SampleMask;
        m_bBSNeedApply = true;
    }
}

void dx10StateManager::SetCullMode(u32 Mode)
{
    ValidateRDesc();

    const D3D_CULL_MODE CMode = dx10StateUtils::ConvertCullMode((D3DCULL)Mode);
    if (m_RDesc.CullMode != CMode)
    {
        m_bRSChanged = true;
        m_RDesc.CullMode = CMode;
    }
}

void dx10StateManager::SetFillMode(u32 Mode)
{
    ValidateRDesc();

    const D3D_FILL_MODE CMode = dx10StateUtils::ConvertFillMode((D3DFILLMODE)Mode);
    if (m_RDesc.FillMode != CMode)
    {
        m_bRSChanged = true;
        m_RDesc.FillMode = CMode;
    }
}

void dx10StateManager::SetMultisample(u32 Enable)
{
    ValidateRDesc();

    if (m_RDesc.MultisampleEnable != BOOL(Enable))
    {
        m_bRSChanged = true;
        m_RDesc.MultisampleEnable = BOOL(Enable);
    }
}

void dx10StateManager::EnableScissoring(BOOL bEnable)
{
    ValidateRDesc();

    if (m_RDesc.ScissorEnable != bEnable)
    {
        m_bRSChanged = true;
        m_RDesc.ScissorEnable = bEnable;
    }
}

void dx10StateManager::OverrideScissoring(bool bOverride, BOOL bValue)
{
    m_bOverrideScissoring = bOverride;
    m_bOverrideScissoringValue = bValue;

    if (m_bOverrideScissoring)
        EnableScissoring(m_bOverrideScissoringValue);
    else
    {
        if (m_bRSChanged)
        {
            D3D_RASTERIZER_DESC tmpDesc;

            if (m_pRState)
                m_pRState->GetDesc(&tmpDesc);
            else
                dx10StateUtils::ResetDescription(tmpDesc);

            m_RDesc.ScissorEnable = tmpDesc.ScissorEnable;
        }
    }
}
