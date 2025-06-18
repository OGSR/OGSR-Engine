#include "stdafx.h"

constexpr u32 max_3dss_width{6880}, max_3dss_height{2880};

void CRenderTarget::ProcessSMAA(CBackend& cmd_list)
{
    PIX_EVENT(SMAA);

    RenderScreenQuad(cmd_list, Device.dwWidth, Device.dwHeight, rt_smaa_edgetex, s_pp_antialiasing->E[2]);
    RenderScreenQuad(cmd_list, Device.dwWidth, Device.dwHeight, rt_smaa_blendtex, s_pp_antialiasing->E[3]);
    RenderScreenQuad(cmd_list, Device.dwWidth, Device.dwHeight, rt_Generic_combine, s_pp_antialiasing->E[4]);

    HW.get_context(cmd_list.context_id)->CopyResource(rt_Generic_0->pSurface, rt_Generic_combine->pSurface);
}

void CRenderTarget::ProcessTAA(CBackend& cmd_list)
{
    PIX_EVENT(TAA);

    RenderScreenQuad(cmd_list, Device.dwWidth, Device.dwHeight, rt_Generic_combine, s_taa->E[0]);
    HW.get_context(cmd_list.context_id)->CopyResource(rt_Generic_0->pSurface, rt_Generic_combine->pSurface);

    HW.get_context(cmd_list.context_id)->CopyResource(rt_Generic_0_prev->pSurface, rt_Generic_combine->pSurface);
}

//*****************************************************************************************************
#include <..\NVIDIA_DLSS\include\nvsdk_ngx.h>
#include <..\NVIDIA_DLSS\include\nvsdk_ngx_helpers.h>

#ifdef _DEBUG
#if _ITERATOR_DEBUG_LEVEL == 0
#pragma comment(lib, "nvsdk_ngx_s_dbg_iterator0")
#else
#pragma comment(lib, "nvsdk_ngx_s_dbg")
#endif
#else
#pragma comment(lib, "nvsdk_ngx_d")
#endif

static class NGXWrapper
{
    NVSDK_NGX_Parameter* NgxParameters{};
    NVSDK_NGX_Handle* Handle{};
    bool DLSSCreated{}, DLSSInited{};
    ID3D11Resource* OutputRT{};

public:
    u32 saved_w{}, saved_h{};
    uint32_t dlssPreset{}, dlssQuality{};
    using Uvector2 = _vector2<u32>;

    bool Create(const u64 appid, const Uvector2& renderSize, const Uvector2& displaySize, ref_rt& out_rt, const u32 quality, u32& preset)
    {
        OutputRT = out_rt->pSurface;
        saved_w = out_rt->dwWidth;
        saved_h = out_rt->dwHeight;

        if (DLSSCreated)
        {
            Destroy();
        }

        if (HW.FeatureLevel < D3D_FEATURE_LEVEL_11_1)
        {
            Msg("!![%s] Low FeatureLevel: [%d]", __FUNCTION__, HW.FeatureLevel);
            // return false;
        }

        NVSDK_NGX_Result result{};
        if (!DLSSInited)
        {
            result = NVSDK_NGX_D3D11_Init(appid, L"", HW.pDevice);
            if (result != NVSDK_NGX_Result_Success)
            {
                Msg("!![%s] failed NVSDK_NGX_D3D11_Init. result: [%d]", __FUNCTION__, result);
                return false;
            }

            DLSSInited = true;
        }

        result = NVSDK_NGX_D3D11_GetCapabilityParameters(&NgxParameters);
        if (result != NVSDK_NGX_Result_Success)
        {
            Msg("!![%s] failed NVSDK_NGX_D3D11_GetCapabilityParameters. result: [%d]", __FUNCTION__, result);
            return false;
        }

        uint32_t needsUpdatedDriver{1};
        result = NgxParameters->Get(NVSDK_NGX_Parameter_SuperSampling_NeedsUpdatedDriver, &needsUpdatedDriver);
        if (needsUpdatedDriver)
        {
            Msg("!![%s] PLEASE UPDATE YOUR DRIVER", __FUNCTION__);
        }

        uint32_t dlssAvailable{};
        result = NgxParameters->Get(NVSDK_NGX_Parameter_SuperSampling_Available, &dlssAvailable);
        if (!dlssAvailable)
        {
            Msg("!![%s] DLSS NOT AVAILABLE", __FUNCTION__);
            NVSDK_NGX_D3D11_DestroyParameters(NgxParameters);
            return false;
        }

        const char* preset_name{};
        switch (quality)
        {
        case NVSDK_NGX_PerfQuality_Value_MaxPerf: preset_name = NVSDK_NGX_Parameter_DLSS_Hint_Render_Preset_Performance; break;
        case NVSDK_NGX_PerfQuality_Value_Balanced: preset_name = NVSDK_NGX_Parameter_DLSS_Hint_Render_Preset_Balanced; break;
        case NVSDK_NGX_PerfQuality_Value_MaxQuality: preset_name = NVSDK_NGX_Parameter_DLSS_Hint_Render_Preset_Quality; break;
        case NVSDK_NGX_PerfQuality_Value_UltraPerformance: preset_name = NVSDK_NGX_Parameter_DLSS_Hint_Render_Preset_UltraPerformance; break;
        case NVSDK_NGX_PerfQuality_Value_UltraQuality: preset_name = NVSDK_NGX_Parameter_DLSS_Hint_Render_Preset_UltraQuality; break;
        case NVSDK_NGX_PerfQuality_Value_DLAA: preset_name = NVSDK_NGX_Parameter_DLSS_Hint_Render_Preset_DLAA; break;
        }

        NVSDK_NGX_Parameter_SetUI(NgxParameters, preset_name, preset);

        result = NgxParameters->Get(preset_name, &dlssPreset);
        if (dlssPreset != preset)
        {
            Msg("!![%s] cannot change [%s] preset to: [%u], current: [%u]", __FUNCTION__, preset_name, preset, dlssPreset);
            preset = dlssPreset;
        }
        else
        {
            Msg("--[%s] [%s] current preset: [%u]", __FUNCTION__, preset_name, dlssPreset);
        }

        int32_t flags{};
        // Указывает, что вектор движения (Motion Vectors) представлен в низком разрешении. Это может снизить вычислительные затраты, но потенциально может снизить качество
        // итогового изображения. Когда вектор движения представлен в низком разрешении, это означает, что движение объектов отслеживается с меньшей детализацией. Например, вместо
        // того, чтобы иметь векторы движения для каждого пикселя, они могут быть рассчитаны для блоков пикселей (например, 4x4 или 8x8 пикселей).
        flags |= NVSDK_NGX_DLSS_Feature_Flags_MVLowRes;

        // Указывает, что сцена рендерится в HDR (High Dynamic Range). Это позволяет DLSS корректно работать с HDR-контентом, учитывая более широкий диапазон яркости и
        // контрастности. https://learn.microsoft.com/ru-ru/windows/win32/direct3darticles/high-dynamic-range
        flags |= NVSDK_NGX_DLSS_Feature_Flags_IsHDR;

        // Указывает на необходимость применения пост-обработки резкости к изображению после работы DLSS. Это улучшает четкость изображения, делая его визуально более
        // привлекательным. Использование флага NVSDK_NGX_DLSS_Feature_Flags_DoSharpening при инициализации и обновлении параметров DLSS является обязательным для активации функции
        // регулирования резкости через параметр InSharpness. Без этого флага значение sharpness не будет учитываться, и постобработка резкости не будет применена.
        /// flags |= NVSDK_NGX_DLSS_Feature_Flags_DoSharpening;

        // Указывает, что вектор движения подвергался дрожанию (Jittering). Это может использоваться для улучшения антиалиасинга путем случайного сдвига пикселей.
        // Флаг NVSDK_NGX_DLSS_Feature_Flags_MVJittered следует использовать, когда ваше приложение применяет джиттер в процессе рендеринга и вектора движения рассчитываются с
        // учетом этих смещений.
        /// flags |= NVSDK_NGX_DLSS_Feature_Flags_MVJittered;

        // Указывает, что нужно обрабатывать альфа-канал при масштабировании. Это важно для корректной работы с прозрачными объектами и элементами в сцене.
        // Этот флаг позволяет DLSS корректно обрабатывать прозрачные объекты, такие как дым, стекло, листву, волосы и другие элементы, которые имеют полупрозрачные пиксели.
        /// flags |= NVSDK_NGX_DLSS_Feature_Flags_AlphaUpscaling;

        NVSDK_NGX_DLSS_Create_Params dlssCreateParams{};
        dlssCreateParams.Feature.InWidth = renderSize.x;
        dlssCreateParams.Feature.InHeight = renderSize.y;
        // final resolution
        dlssCreateParams.Feature.InTargetWidth = displaySize.x;
        dlssCreateParams.Feature.InTargetHeight = displaySize.y;
        dlssCreateParams.Feature.InPerfQualityValue = static_cast<NVSDK_NGX_PerfQuality_Value>(quality);
        dlssQuality = dlssCreateParams.Feature.InPerfQualityValue;

        dlssCreateParams.InFeatureCreateFlags = flags;
        result = NGX_D3D11_CREATE_DLSS_EXT(HW.get_context(CHW::IMM_CTX_ID), &Handle, NgxParameters, &dlssCreateParams);
        if (result != NVSDK_NGX_Result_Success)
        {
            Msg("!![%s] failed NGX_D3D11_CREATE_DLSS_EXT. result: [%d]", __FUNCTION__, result);
            return false;
        }

        DLSSCreated = true;
        return true;
    }

    void Destroy()
    {
        if (Handle)
        {
            NVSDK_NGX_D3D11_ReleaseFeature(Handle);
            Handle = nullptr;
        }

        if (NgxParameters)
        {
            NVSDK_NGX_D3D11_DestroyParameters(NgxParameters);
            NgxParameters = nullptr;
        }

        if (DLSSInited)
        {
            NVSDK_NGX_D3D11_Shutdown1(nullptr);
            DLSSInited = false;
        }

        DLSSCreated = false;
    }

    bool Draw() const
    {
        if (!DLSSCreated)
        {
            Msg("! NGXWrapper not created!");
            return false;
        }

        NVSDK_NGX_D3D11_DLSS_Eval_Params dlssEvalParams{};

        // Исходный цветовой рендер-таргет, который будет обрабатываться DLSS. DXGI_FORMAT_R16G16B16A16_FLOAT
        dlssEvalParams.Feature.pInColor = RImplementation.Target->rt_Generic_0->pSurface;
        // Ресурс для конечного (обработанного) цвета. Должен соответствовать формату исходного рендер-таргета или быть подходящим для конечного вывода. Обычно это
        // DXGI_FORMAT_R16G16B16A16_FLOAT.
        dlssEvalParams.Feature.pInOutput = OutputRT;
        // Буфер глубины. Обычно используется DXGI_FORMAT_R32_FLOAT для высокой точности глубины или DXGI_FORMAT_D32_FLOAT для использования в качестве буфера глубины. У нас же
        // DXGI_FORMAT_D24_UNORM_S8_UINT
        dlssEvalParams.pInDepth = RImplementation.Target->rt_zbuffer->pSurface;
        // Ресурс, содержащий векторы движения. DXGI_FORMAT_R16G16_FLOAT
        dlssEvalParams.pInMotionVectors = RImplementation.Target->rt_Velocity->pSurface;

        dlssEvalParams.InRenderSubrectDimensions.Width = Device.dwWidth;
        dlssEvalParams.InRenderSubrectDimensions.Height = Device.dwHeight;

        dlssEvalParams.InJitterOffsetX = ps_r_taa_jitter_full.x;
        dlssEvalParams.InJitterOffsetY = ps_r_taa_jitter_full.y;

        dlssEvalParams.InMVScaleX = -static_cast<float>(Device.dwWidth) * 0.5f;
        dlssEvalParams.InMVScaleY = static_cast<float>(Device.dwHeight) * 0.5f;

        const NVSDK_NGX_Result result = NGX_D3D11_EVALUATE_DLSS_EXT(HW.get_context(CHW::IMM_CTX_ID), Handle, NgxParameters, &dlssEvalParams);
        if (result != NVSDK_NGX_Result_Success)
        {
            Msg("! NGX_D3D11_EVALUATE_DLSS_EXT not valid. result: [%d]", result);
            return false;
        }

        return true;
    }

    ~NGXWrapper() { Destroy(); }
} NGXWrapper, NGXWrapperScope;

static float saved_3dss_scale_factor{};
bool CRenderTarget::reset_3dss_rendertarget(const bool need_reset)
{
    if (!need_reset && saved_3dss_scale_factor == ps_r_dlss_3dss_scale_factor)
        return false;

    saved_3dss_scale_factor = ps_r_dlss_3dss_scale_factor;

    u32 saved_w{}, saved_h{};
    bool empty_rt = !rt_Generic_combine_scope;
    if (!empty_rt)
    {
        saved_w = rt_Generic_combine_scope->dwWidth;
        saved_h = rt_Generic_combine_scope->dwHeight;
        rt_Generic_combine_scope.destroy();
    }

    u32 w{}, h{};
    for (float i{ps_r_dlss_3dss_scale_factor}; i >= 1.f; i -= 0.1f)
    {
        w = static_cast<u32>(std::ceil(static_cast<float>(Device.dwWidth) * i));
        h = static_cast<u32>(std::ceil(static_cast<float>(Device.dwHeight) * i));
        if (w <= max_3dss_width && h <= max_3dss_height)
            break;
    }
    constexpr Flags32 flg{CRT::CreateUAV};
    rt_Generic_combine_scope.create(r2_RT_generic_combine_scope, w, h, DXGI_FORMAT_R16G16B16A16_FLOAT, 1, flg);

    Msg("--[%s] 3D Scope render size: [%u, %u]", __FUNCTION__, w, h);

    return empty_rt || saved_w != w || saved_h != h;
}

void CRenderTarget::InitDLSS()
{
    NGXWrapper.Destroy();
    NGXWrapperScope.Destroy();

    const NGXWrapper::Uvector2 RenderParams{Device.dwWidth, Device.dwHeight};
    if (!NGXWrapper.Create(20082024151405ull, RenderParams, RenderParams, rt_Generic_combine, NVSDK_NGX_PerfQuality_Value_DLAA, ps_r_dlss_preset))
    {
        if (ps_r_pp_aa_mode == DLSS)
            ps_r_pp_aa_mode = FSR2;
    }
    else
    {
        reset_3dss_rendertarget();

        const NGXWrapper::Uvector2 DisplayParams{rt_Generic_combine_scope->dwWidth, rt_Generic_combine_scope->dwHeight};
        R_ASSERT(NGXWrapperScope.Create(20082024849605ull, RenderParams, DisplayParams, rt_Generic_combine_scope, ps_r_dlss_3dss_quality, ps_r_dlss_3dss_preset));
    }
}

void CRenderTarget::DestroyDLSS()
{
    NGXWrapper.Destroy();
    NGXWrapperScope.Destroy();
}

bool CRenderTarget::ProcessDLSS()
{
    PIX_EVENT(DLSS);

    if (ps_r_dlss_preset != NGXWrapper.dlssPreset)
    {
        InitDLSS();
    }

    if (!NGXWrapper.Draw())
    {
        Msg("!![%s] FAILED DLSS DRAW!", __FUNCTION__);
        return false;
    }

    HW.get_context(CHW::IMM_CTX_ID)->CopyResource(rt_Generic_0->pSurface, rt_Generic_combine->pSurface);
    return true;
}

bool CRenderTarget::ProcessDLSS_3DSS(const bool need_reset)
{
    PIX_EVENT(3D_SCOPE_DLSS);

    if (need_reset || ps_r_dlss_3dss_preset != NGXWrapperScope.dlssPreset || ps_r_dlss_3dss_quality != NGXWrapperScope.dlssQuality)
    {
        InitDLSS();
    }

    if (!NGXWrapperScope.Draw())
    {
        Msg("!![%s] FAILED 3D SCOPE DLSS DRAW!", __FUNCTION__);
        return false;
    }

    return true;
}

//*****************************************************************************************************

void CRenderTarget::ProcessCAS(CBackend& cmd_list, ref_selement& sh, const bool force)
{
    if (!force && fis_zero(ps_r_cas))
        return;

    PIX_EVENT(CAS);

    const Fvector4 params{std::max(ps_r_cas, 0.01f), 0.f, 0.f, 0.f};
    RenderScreenQuad(cmd_list, Device.dwWidth, Device.dwHeight, rt_Generic_combine, sh, [&]() { cmd_list.set_c("f_cas_intensity", params); });
    HW.get_context(cmd_list.context_id)->CopyResource(rt_Generic_0->pSurface, rt_Generic_combine->pSurface);
}

//*****************************************************************************************************
#include <..\AMD_FSR2\build\native\include\ffx-fsr2-api\ffx_fsr2.h>
#include <..\AMD_FSR2\build\native\include\ffx-fsr2-api\dx11\ffx_fsr2_dx11.h>

#pragma comment(lib, "ffx_fsr2_api_x64")
#pragma comment(lib, "ffx_fsr2_api_dx11_x64")

static class Fsr2Wrapper
{
    bool fsr_created{};
    FfxFsr2Context m_context{};
    xr_vector<char> m_scratchBuffer;
    ID3D11Resource* OutputRT{};

public:
    u32 saved_w{}, saved_h{};

    bool Create(const FfxDimensions2D& maxRenderSize, const FfxDimensions2D& displaySize, ref_rt& out_rt)
    {
        OutputRT = out_rt->pSurface;
        saved_w = out_rt->dwWidth;
        saved_h = out_rt->dwHeight;

        if (fsr_created)
        {
            Destroy();
        }

        if (HW.FeatureLevel < D3D_FEATURE_LEVEL_11_0)
        {
            Msg("!![%s] Low FeatureLevel: [%d]", __FUNCTION__, HW.FeatureLevel);
        }

        // Setup DX11 interface.
        FfxFsr2ContextDescription m_contextDesc{};
        m_scratchBuffer.resize(ffxFsr2GetScratchMemorySizeDX11());
        const FfxErrorCode errorCode = ffxFsr2GetInterfaceDX11(&m_contextDesc.callbacks, HW.pDevice, m_scratchBuffer.data(), m_scratchBuffer.size());
        if (errorCode != FFX_OK)
        {
            Msg("!!Failed ffxFsr2GetInterfaceDX11! Error: [%d]", errorCode);
            return false;
        }

        // This adds a ref to the device.
        // The reference will get freed in ffxFsr2ContextDestroy
        m_contextDesc.device = ffxGetDeviceDX11(HW.pDevice);
        m_contextDesc.maxRenderSize = maxRenderSize;
        m_contextDesc.displaySize = displaySize;

        // You should config the flags you need based on your own project
        m_contextDesc.flags = FFX_FSR2_ENABLE_HIGH_DYNAMIC_RANGE;

        ffxFsr2ContextCreate(&m_context, &m_contextDesc);

        fsr_created = true;
        return true;
    }

    void Destroy()
    {
        if (!fsr_created)
            return;

        fsr_created = false;
        ffxFsr2ContextDestroy(&m_context);
    }

    bool Draw()
    {
        if (!fsr_created)
        {
            Msg("! Fsr2Wrapper not created!");
            return false;
        }

        FfxFsr2DispatchDescription dispatchParameters{};

        dispatchParameters.commandList = HW.get_context(CHW::IMM_CTX_ID);

        dispatchParameters.color = ffxGetResourceDX11(&m_context, RImplementation.Target->rt_Generic_0->pSurface, L"FSR2_InputColor");
        dispatchParameters.depth = ffxGetResourceDX11(&m_context, RImplementation.Target->rt_zbuffer->pSurface, L"FSR2_InputDepth");

        dispatchParameters.motionVectors = ffxGetResourceDX11(&m_context, RImplementation.Target->rt_Velocity->pSurface, L"FSR2_InputMotionVectors");
        dispatchParameters.exposure = ffxGetResourceDX11(&m_context, nullptr, L"FSR2_InputExposure");

        dispatchParameters.reactive = ffxGetResourceDX11(&m_context, nullptr, L"FSR2_InputReactiveMap");
        dispatchParameters.transparencyAndComposition = ffxGetResourceDX11(&m_context, nullptr, L"FSR2_TransparencyAndCompositionMap");

        dispatchParameters.output = ffxGetResourceDX11(&m_context, OutputRT, L"FSR2_OutputUpscaledColor", FFX_RESOURCE_STATE_UNORDERED_ACCESS);

        dispatchParameters.jitterOffset.x = ps_r_taa_jitter_full.x;
        dispatchParameters.jitterOffset.y = ps_r_taa_jitter_full.y;

        dispatchParameters.motionVectorScale.x = -static_cast<float>(Device.dwWidth) * 0.5f;
        dispatchParameters.motionVectorScale.y = static_cast<float>(Device.dwHeight) * 0.5f;

        dispatchParameters.reset = false;

        dispatchParameters.enableSharpening = false;
        dispatchParameters.sharpness = 0.f;

        dispatchParameters.frameTimeDelta = std::max(1.0f + EPS_L, Device.fTimeDeltaRealMS); // The time elapsed since the last frame (expressed in milliseconds).

        dispatchParameters.preExposure = 1.0f;

        dispatchParameters.renderSize.width = Device.dwWidth;
        dispatchParameters.renderSize.height = Device.dwHeight;

        dispatchParameters.cameraFar = g_pGamePersistent->Environment().CurrentEnv->far_plane;
        dispatchParameters.cameraNear = VIEWPORT_NEAR;

        dispatchParameters.cameraFovAngleVertical = deg2rad(Device.fFOV);

        dispatchParameters.viewSpaceToMetersFactor = 1.0f;
        dispatchParameters.autoTcThreshold = 0.1f;
        dispatchParameters.autoTcScale = 1.0f;
        dispatchParameters.autoReactiveScale = 5.0f;
        dispatchParameters.autoReactiveMax = 0.9f;

        const FfxErrorCode errorCode = ffxFsr2ContextDispatch(&m_context, &dispatchParameters);

        if (errorCode != FFX_OK)
        {
            Msg("! ffxFsr2ContextDispatch not valid. Error: [%d]", errorCode);
            return false;
        }

        return true;
    }

    ~Fsr2Wrapper() { Destroy(); }
} Fsr2Wrapper, Fsr2WrapperScope;

void CRenderTarget::InitFSR()
{
    Fsr2Wrapper.Destroy();
    Fsr2WrapperScope.Destroy();

    const FfxDimensions2D displaySize{Device.dwWidth, Device.dwHeight};

    if (!Fsr2Wrapper.Create(displaySize, displaySize, rt_Generic_combine))
    {
        if (ps_r_pp_aa_mode == FSR2)
            ps_r_pp_aa_mode = TAA;
    }
    else
    {
        reset_3dss_rendertarget();

        const FfxDimensions2D ScopeSize{rt_Generic_combine_scope->dwWidth, rt_Generic_combine_scope->dwHeight};
        R_ASSERT(Fsr2WrapperScope.Create(displaySize, ScopeSize, rt_Generic_combine_scope));
    }
}

void CRenderTarget::DestroyFSR()
{
    Fsr2Wrapper.Destroy();
    Fsr2WrapperScope.Destroy();
}

bool CRenderTarget::ProcessFSR() const
{
    PIX_EVENT(FSR);

    if (!Fsr2Wrapper.Draw())
    {
        Msg("!![%s] FAILED FSR DRAW!", __FUNCTION__);
        return false;
    }

    HW.get_context(CHW::IMM_CTX_ID)->CopyResource(rt_Generic_0->pSurface, rt_Generic_combine->pSurface);
    return true;
}

bool CRenderTarget::ProcessFSR_3DSS(const bool need_reset)
{
    PIX_EVENT(3D_SCOPE_FSR);

    if (need_reset)
    {
        InitFSR();
    }

    if (!Fsr2WrapperScope.Draw())
    {
        Msg("!![%s] FAILED 3D SCOPE FSR DRAW!", __FUNCTION__);
        return false;
    }

    return true;
}
//*****************************************************************************************************

void CRenderTarget::PhaseAA(CBackend& cmd_list)
{
    if (ps_pnv_mode > 1) // skip AA for heatvision
        return;

    switch (ps_r_pp_aa_mode)
    {
        case DLSS: {
            if (!ProcessDLSS())
                ps_r_pp_aa_mode = FSR2;
            break;
        }
        case FSR2: {
            u_setrt(cmd_list, get_width(cmd_list), get_height(cmd_list), nullptr, nullptr, nullptr, nullptr);
            RImplementation.rmNormal(cmd_list);

            if (!ProcessFSR())
                ps_r_pp_aa_mode = TAA;
            break;
        }
        case TAA: ProcessTAA(cmd_list); break;
        case SMAA: ProcessSMAA(cmd_list); break;
    }

    if (ps_r_pp_aa_mode != SMAA)
        ProcessCAS(cmd_list, s_cas->E[0]);
}

//*****************************************************************************************************

bool CRenderTarget::Phase3DSSUpscale(CBackend& cmd_list)
{
    if (ps_r_dlss_3dss_scale_factor <= 1.f)
        return false;

    bool need_reset = reset_3dss_rendertarget();
    bool rendered{};

    if (ps_r_pp_aa_mode == DLSS)
    {
        if (!need_reset)
            need_reset = (NGXWrapperScope.saved_w != rt_Generic_combine_scope->dwWidth || NGXWrapperScope.saved_h != rt_Generic_combine_scope->dwHeight);
        rendered = ProcessDLSS_3DSS(need_reset);
    }
    else if (ps_r_pp_aa_mode == FSR2)
    {
        if (!need_reset)
            need_reset = (Fsr2WrapperScope.saved_w != rt_Generic_combine_scope->dwWidth || Fsr2WrapperScope.saved_h != rt_Generic_combine_scope->dwHeight);
        rendered = ProcessFSR_3DSS(need_reset);
    }

    if (rendered)
    {
        if (!ps_r2_ls_flags_ext.test(R2FLAGEXT_DLSS_3DSS_USE_SECOND_PASS))
        {
            // вместо того чтобы два раза дергать dlss для прицела и для экрана, используем в cas и далее текстуру из rt_Generic_scope
            // это сэкономит немного фпс почти без потери качества
            ProcessCAS(cmd_list, s_cas->E[1], true);
        }
        return true;
    }

    return false;
}

//*****************************************************************************************************
