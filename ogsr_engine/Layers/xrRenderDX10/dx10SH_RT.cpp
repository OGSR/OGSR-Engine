#include "stdafx.h"


#include "../xrRender/ResourceManager.h"
#include "../xrRender/dxRenderDeviceRender.h"

CRT::~CRT()
{
    destroy();

    // release external reference
    DEV->_DeleteRT(this);
}

void CRT::create(LPCSTR Name, u32 w, u32 h, DXGI_FORMAT f, u32 SampleCount /*= 1*/, u32 slices_num /*=1*/, Flags32 flags /*= {}*/)
{
    if (pSurface)
        return;

    R_ASSERT(HW.pDevice && Name && Name[0] && w && h);
    _order = CPU::GetCLK(); // Device.GetTimerGlobal()->GetElapsed_clk();

    dwWidth = w;
    dwHeight = h;
    fmt = f;
    sampleCount = SampleCount;
    n_slices = slices_num;

    // Check width-and-height of render target surface
    if (w > D3D_REQ_TEXTURE2D_U_OR_V_DIMENSION)
        return;
    if (h > D3D_REQ_TEXTURE2D_U_OR_V_DIMENSION)
        return;

    // Select usage
    u32 usage{};
    if (DXGI_FORMAT_D24_UNORM_S8_UINT == fmt)
        usage = D3DUSAGE_DEPTHSTENCIL;
    else if (DXGI_FORMAT_R24_UNORM_X8_TYPELESS == fmt)
        usage = D3DUSAGE_DEPTHSTENCIL;
    else if (DXGI_FORMAT_D16_UNORM == fmt)
    {
        usage = D3DUSAGE_DEPTHSTENCIL;
        fmt = DXGI_FORMAT_R16_TYPELESS;
    }
    else if (DXGI_FORMAT_D32_FLOAT == fmt)
    {
        usage = D3DUSAGE_DEPTHSTENCIL;
        fmt = DXGI_FORMAT_R32_TYPELESS;
    }
    else if (DXGI_FORMAT_R24G8_TYPELESS == fmt)
        usage = D3DUSAGE_DEPTHSTENCIL;
    else
        usage = D3DUSAGE_RENDERTARGET;

    const bool bUseAsDepth = usage == D3DUSAGE_DEPTHSTENCIL;

    // Validate render-target usage
    u32 required = D3D11_FORMAT_SUPPORT_TEXTURE2D;

    if (bUseAsDepth)
        required |= D3D11_FORMAT_SUPPORT_DEPTH_STENCIL;
    else
        required |= D3D11_FORMAT_SUPPORT_RENDER_TARGET;

    if (!HW.CheckFormatSupport(f, required))
        return;

    if (flags.test(CreateBase))
    {
        dwFlags |= CreateBase;
        if (!bUseAsDepth)
        {
            /*u32 idx;
            char const* str = strrchr(Name, '_');
            sscanf(++str, "%u", &idx);*/
            R_CHK(HW.m_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pSurface)));
        }
    }

    // Create the render target texture
    D3D_TEXTURE2D_DESC desc{};
    if (pSurface)
    {
        pSurface->GetDesc(&desc);
    }
    else
    {
        desc.Width = dwWidth;
        desc.Height = dwHeight;
        desc.MipLevels = 1;
        desc.ArraySize = n_slices;
        desc.Format = f;
        desc.SampleDesc.Count = SampleCount;
        desc.Usage = D3D_USAGE_DEFAULT;
        desc.BindFlags = D3D_BIND_SHADER_RESOURCE | (bUseAsDepth ? D3D_BIND_DEPTH_STENCIL : D3D_BIND_RENDER_TARGET);

        if (SampleCount > 1)
        {
            // For feature level 10.0 and lower we can't have both SRV and DSV for a texture
            if (HW.FeatureLevel <= D3D_FEATURE_LEVEL_10_0 && bUseAsDepth)
                desc.BindFlags &= ~D3D_BIND_SHADER_RESOURCE; // remove SRV flag
        }

        if (flags.test(CreateUAV))
        {
            dwFlags |= CreateUAV;

            if (HW.FeatureLevel >= D3D_FEATURE_LEVEL_11_0 && !bUseAsDepth && SampleCount == 1 &&
                HW.CheckFormatSupport(f, D3D11_FORMAT_SUPPORT_TYPED_UNORDERED_ACCESS_VIEW))
            {
                desc.BindFlags |= D3D11_BIND_UNORDERED_ACCESS;
            }
        }

        CHK_DX(HW.pDevice->CreateTexture2D(&desc, NULL, &pSurface));
    }

    HW.stats_manager.increment_stats_rtarget(pSurface);
#ifdef DEBUG
        Msg("* created RT(%s), %dx%d, format = %d samples = %d", Name, w, h, dx10FMT, SampleCount);
#endif // DEBUG

    if (pSurface)
    {
        DXUT_SetDebugName(pSurface, cName.c_str());
    }

    // OK
    if (bUseAsDepth)
    {
        D3D_DEPTH_STENCIL_VIEW_DESC ViewDesc{};

        if (SampleCount <= 1)
        {
            ViewDesc.ViewDimension = n_slices > 1 ? D3D_DSV_DIMENSION_TEXTURE2DARRAY : D3D_DSV_DIMENSION_TEXTURE2D;
            if (n_slices > 1)
                ViewDesc.Texture2DArray.ArraySize = n_slices;
        }
        else
        {
            ViewDesc.ViewDimension = n_slices > 1 ? D3D_DSV_DIMENSION_TEXTURE2DMSARRAY : D3D_DSV_DIMENSION_TEXTURE2DMS;
            if (n_slices > 1)
                ViewDesc.Texture2DMSArray.ArraySize = n_slices;
        }

        switch (desc.Format)
        {
        case DXGI_FORMAT_R16_TYPELESS:
            ViewDesc.Format = DXGI_FORMAT_D16_UNORM;
            break;

        case DXGI_FORMAT_R24G8_TYPELESS:
            ViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
            break;

        case DXGI_FORMAT_R32_TYPELESS:
            ViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
            break;

        case DXGI_FORMAT_R32G8X24_TYPELESS:
            ViewDesc.Format = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
            break;

        default:
            ViewDesc.Format = desc.Format;
        }

        CHK_DX(HW.pDevice->CreateDepthStencilView(pSurface, &ViewDesc, &dsv_all));
        {
            char name[128];
            xr_sprintf(name, "%s:all", Name);
            DXUT_SetDebugName(dsv_all, name);
        }

        dsv_per_slice.resize(n_slices);
        for (u32 idx = 0; idx < n_slices; ++idx)
        {
            if (SampleCount <= 1)
            {
                ViewDesc.Texture2DArray.ArraySize = 1;
                ViewDesc.Texture2DArray.FirstArraySlice = idx;
            }
            else
            {
                ViewDesc.Texture2DMSArray.ArraySize = 1;
                ViewDesc.Texture2DMSArray.FirstArraySlice = idx;
            }
            CHK_DX(HW.pDevice->CreateDepthStencilView(pSurface, &ViewDesc, &dsv_per_slice[idx]));

            {
                char name[128];
                xr_sprintf(name, "%s:s%d", Name, idx);
                DXUT_SetDebugName(dsv_per_slice[idx], name);
            }
        }
        for (int id = 0; id < R__NUM_CONTEXTS; ++id)
        {
            set_slice_write(id, -1);
        }
    }
    else
        CHK_DX(HW.pDevice->CreateRenderTargetView(pSurface, 0, &pRT));

    if (desc.BindFlags & D3D11_BIND_UNORDERED_ACCESS)
    {
        D3D11_UNORDERED_ACCESS_VIEW_DESC UAVDesc{};
        UAVDesc.Format = f;
        UAVDesc.ViewDimension = n_slices > 1 ? D3D11_UAV_DIMENSION_TEXTURE2DARRAY : D3D11_UAV_DIMENSION_TEXTURE2D;
        if (n_slices > 1)
            UAVDesc.Texture2DArray.ArraySize = n_slices;
        CHK_DX(HW.pDevice->CreateUnorderedAccessView(pSurface, &UAVDesc, &pUAView));
    }

    if (!(desc.BindFlags & D3D_BIND_SHADER_RESOURCE))
    {
        // pTexture->surface_set(pSurface) creates shader resource view
        // which requires D3D_BIND_SHADER_RESOURCE flag to be set.
        // Usually, it isn't set swapchain's buffers.
        return;
    }

    pTexture = DEV->_CreateTexture(Name);
    pTexture->surface_set(pSurface);
}

void CRT::destroy()
{
    if (pTexture._get())
    {
        pTexture->surface_set(nullptr);
        pTexture.destroy();
        pTexture = nullptr;
    }
    _RELEASE(pRT);
    for (auto& dsv : dsv_per_slice)
    {
        _RELEASE(dsv);
    }
    _RELEASE(dsv_all);

    HW.stats_manager.decrement_stats_rtarget(pSurface);
    _RELEASE(pSurface);
    _RELEASE(pUAView);
}

void CRT::set_slice_read(int slice) const
{
    VERIFY(slice <= n_slices || slice == -1);
    pTexture->set_slice(slice);
}

void CRT::set_slice_write(u32 context_id, int slice)
{
    VERIFY(slice <= n_slices || slice == -1);
    pZRT[context_id] = (slice < 0) ? dsv_all : dsv_per_slice[slice];
}

void CRT::reset_begin() { destroy(); }
void CRT::reset_end() { create(*cName, dwWidth, dwHeight, fmt, sampleCount, n_slices, { dwFlags }); }

void resptrcode_crt::create(LPCSTR Name, u32 w, u32 h, DXGI_FORMAT f, u32 SampleCount /*= 1*/, u32 slices_num /*=1*/, Flags32 flags /*= 0*/)
{
    _set(DEV->_CreateRT(Name, w, h, f, SampleCount, slices_num, flags));
}
