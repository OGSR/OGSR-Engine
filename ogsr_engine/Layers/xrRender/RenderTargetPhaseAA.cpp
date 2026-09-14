#include "stdafx.h"
#include "FidelityFX/host/ffx_fsr3.h"
#include "FidelityFX/host/backends/dx11/ffx_dx11.h"

void CRenderTarget::ProcessSMAA(CBackend& cmd_list)
{
    PIX_EVENT(SMAA);

    RenderScreenTriangle(cmd_list, rt_smaa_edgetex, s_pp_antialiasing->E[2]);
    RenderScreenTriangle(cmd_list, rt_smaa_blendtex, s_pp_antialiasing->E[3]);
    RenderScreenTriangle(cmd_list, rt_Generic_scene_scratch, s_pp_antialiasing->E[4]);

    HW.get_context(cmd_list.context_id)->CopyResource(rt_Generic_0->pSurface, rt_Generic_scene_scratch->pSurface);
}

void CRenderTarget::ProcessTAA(CBackend& cmd_list)
{
    PIX_EVENT(TAA);

    if (m_resetTemporalHistory)
    {
        HW.get_context(cmd_list.context_id)->CopyResource(rt_Generic_0_prev->pSurface, rt_Generic_0->pSurface);
        m_resetTemporalHistory = false;
    }

    RenderScreenTriangle(cmd_list, rt_Generic_scene_scratch, s_taa->E[0]);
    HW.get_context(cmd_list.context_id)->CopyResource(rt_Generic_0->pSurface, rt_Generic_scene_scratch->pSurface);

    HW.get_context(cmd_list.context_id)->CopyResource(rt_Generic_0_prev->pSurface, rt_Generic_scene_scratch->pSurface);
}

//*****************************************************************************************************
#include <..\NVIDIA_DLSS\DLSS\include\nvsdk_ngx.h>
#include <..\NVIDIA_DLSS\DLSS\include\nvsdk_ngx_helpers.h>
#include <winver.h>

#ifdef _DEBUG
#if _ITERATOR_DEBUG_LEVEL == 0
#pragma comment(lib, "nvsdk_ngx_s_dbg_iterator0")
#else
#pragma comment(lib, "nvsdk_ngx_s_dbg")
#endif
#else
#ifdef _DLL
#pragma comment(lib, "nvsdk_ngx_d")
#else
#pragma comment(lib, "nvsdk_ngx_s")
#endif
#endif

#pragma comment(lib, "version.lib")

char ps_r_dlss_dll_version[64]{};

struct DlssDllFileVersion
{
    u32 major{};
    u32 minor{};
    u32 patch{};
    u32 revision{};
    bool valid{};
};

static DlssDllFileVersion g_dlss_dll_file_version{};

static bool DlssDllVersionAtLeast(const u32 major, const u32 minor, const u32 patch)
{
    if (!g_dlss_dll_file_version.valid)
        return false;
    if (g_dlss_dll_file_version.major != major)
        return g_dlss_dll_file_version.major > major;
    if (g_dlss_dll_file_version.minor != minor)
        return g_dlss_dll_file_version.minor > minor;
    return g_dlss_dll_file_version.patch >= patch;
}

static void QueryLoadedDlssDllVersion()
{
    if (g_dlss_dll_file_version.valid)
        return;

    const HMODULE mod = GetModuleHandleW(L"nvngx_dlss.dll");
    if (!mod)
        return;

    wchar_t path[MAX_PATH]{};
    if (!GetModuleFileNameW(mod, path, MAX_PATH))
        return;

    DWORD dummy{};
    const DWORD size = GetFileVersionInfoSizeW(path, &dummy);
    if (!size)
        return;

    xr_vector<u8> data(size);
    if (!GetFileVersionInfoW(path, 0, size, data.data()))
        return;

    VS_FIXEDFILEINFO* info{};
    UINT info_size{};
    if (!VerQueryValueW(data.data(), L"\\", reinterpret_cast<LPVOID*>(&info), &info_size) || !info)
        return;

    g_dlss_dll_file_version.major = HIWORD(info->dwFileVersionMS);
    g_dlss_dll_file_version.minor = LOWORD(info->dwFileVersionMS);
    g_dlss_dll_file_version.patch = HIWORD(info->dwFileVersionLS);
    g_dlss_dll_file_version.revision = LOWORD(info->dwFileVersionLS);
    g_dlss_dll_file_version.valid = true;

    if (g_dlss_dll_file_version.revision == 0)
    {
        xr_sprintf(ps_r_dlss_dll_version, "%u.%u.%u", g_dlss_dll_file_version.major, g_dlss_dll_file_version.minor, g_dlss_dll_file_version.patch);
    }
    else
    {
        xr_sprintf(ps_r_dlss_dll_version, "%u.%u.%u.%u", g_dlss_dll_file_version.major, g_dlss_dll_file_version.minor, g_dlss_dll_file_version.patch,
            g_dlss_dll_file_version.revision);
    }

    Msg("--[DLSS] loaded DLL version: [%s]", ps_r_dlss_dll_version);
}

struct DlssResolutionInfo
{
    u32 renderWidth{}, renderHeight{};
    u32 minRenderWidth{}, minRenderHeight{};
    u32 maxRenderWidth{}, maxRenderHeight{};
    float sharpness{};
};

static NVSDK_NGX_PerfQuality_Value GetRequestedDlssQuality()
{
    switch (ps_r_dlss_quality)
    {
    case DLSS_QUALITY_QUALITY: return NVSDK_NGX_PerfQuality_Value_MaxQuality;
    case DLSS_QUALITY_BALANCED: return NVSDK_NGX_PerfQuality_Value_Balanced;
    case DLSS_QUALITY_PERFORMANCE: return NVSDK_NGX_PerfQuality_Value_MaxPerf;
    case DLSS_QUALITY_ULTRA_PERFORMANCE: return NVSDK_NGX_PerfQuality_Value_UltraPerformance;
    default: return NVSDK_NGX_PerfQuality_Value_DLAA;
    }
}

static const char* GetDlssPresetParameterName(const NVSDK_NGX_PerfQuality_Value quality)
{
    switch (quality)
    {
    case NVSDK_NGX_PerfQuality_Value_MaxPerf: return NVSDK_NGX_Parameter_DLSS_Hint_Render_Preset_Performance;
    case NVSDK_NGX_PerfQuality_Value_Balanced: return NVSDK_NGX_Parameter_DLSS_Hint_Render_Preset_Balanced;
    case NVSDK_NGX_PerfQuality_Value_MaxQuality: return NVSDK_NGX_Parameter_DLSS_Hint_Render_Preset_Quality;
    case NVSDK_NGX_PerfQuality_Value_UltraPerformance: return NVSDK_NGX_Parameter_DLSS_Hint_Render_Preset_UltraPerformance;
    case NVSDK_NGX_PerfQuality_Value_UltraQuality: return NVSDK_NGX_Parameter_DLSS_Hint_Render_Preset_UltraQuality;
    case NVSDK_NGX_PerfQuality_Value_DLAA: return NVSDK_NGX_Parameter_DLSS_Hint_Render_Preset_DLAA;
    default: return NVSDK_NGX_Parameter_DLSS_Hint_Render_Preset_DLAA;
    }
}

static class NGXWrapper
{
    NVSDK_NGX_Parameter* NgxParameters{};
    NVSDK_NGX_Handle* Handle{};
    bool DLSSCreated{}, DLSSInited{};
    ID3D11Resource* OutputRT{};
    NVSDK_NGX_Dimensions saved_renderSize{};
    bool resetHistory{true};
    bool availablePresetsResolved{};

public:
    u32 saved_w{}, saved_h{};
    uint32_t dlssPreset{}, dlssQuality{}, requestedQuality{};
    u32 availablePresetMask{~0u};

    bool Initialize(const u64 appid)
    {
        if (!HW.pDevice)
            return false;

        if (HW.FeatureLevel < D3D_FEATURE_LEVEL_11_1)
            Msg("!![%s] Low FeatureLevel: [%d]", __FUNCTION__, HW.FeatureLevel);

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

        if (!NgxParameters)
        {
            result = NVSDK_NGX_D3D11_GetCapabilityParameters(&NgxParameters);
            if (result != NVSDK_NGX_Result_Success)
            {
                Msg("!![%s] failed NVSDK_NGX_D3D11_GetCapabilityParameters. result: [%d]", __FUNCTION__, result);
                return false;
            }

            uint32_t needsUpdatedDriver{1};
            NgxParameters->Get(NVSDK_NGX_Parameter_SuperSampling_NeedsUpdatedDriver, &needsUpdatedDriver);
            if (needsUpdatedDriver)
                Msg("!![%s] PLEASE UPDATE YOUR DRIVER", __FUNCTION__);

            uint32_t dlssAvailable{};
            NgxParameters->Get(NVSDK_NGX_Parameter_SuperSampling_Available, &dlssAvailable);
            if (!dlssAvailable)
            {
                Msg("!![%s] DLSS NOT AVAILABLE", __FUNCTION__);
                NVSDK_NGX_D3D11_DestroyParameters(NgxParameters);
                NgxParameters = nullptr;
                return false;
            }
        }

        QueryLoadedDlssDllVersion();
        return true;
    }

    bool IsPresetAvailable(const u32 preset) const
    {
        if (preset > 31)
            return false;
        if (availablePresetMask == ~0u)
            return true;
        return (availablePresetMask & (1u << preset)) != 0;
    }

    void RefreshAvailablePresets()
    {
        if (!HW.pDevice)
            return;

        if (!Initialize(20082024151405ull) || !NgxParameters)
            return;

        QueryLoadedDlssDllVersion();
        if (availablePresetsResolved)
            return;

        // NGX parameter Set/Get does not report whether a preset exists in the
        // loaded DLL; it just stores the hint. NVIDIA added L/M in SDK 310.5.0.
        u32 mask = 1u << NVSDK_NGX_DLSS_Hint_Render_Preset_Default;
        mask |= 1u << NVSDK_NGX_DLSS_Hint_Render_Preset_F;
        mask |= 1u << NVSDK_NGX_DLSS_Hint_Render_Preset_J;
        mask |= 1u << NVSDK_NGX_DLSS_Hint_Render_Preset_K;
        if (DlssDllVersionAtLeast(310, 5, 0))
        {
            mask |= 1u << NVSDK_NGX_DLSS_Hint_Render_Preset_L;
            mask |= 1u << NVSDK_NGX_DLSS_Hint_Render_Preset_M;
        }

        availablePresetMask = mask;
        if (g_dlss_dll_file_version.valid)
            availablePresetsResolved = true;

        Msg("--[DLSS] available presets for DLL [%s]: Default/F/J/K%s", ps_r_dlss_dll_version[0] ? ps_r_dlss_dll_version : "unknown",
            DlssDllVersionAtLeast(310, 5, 0) ? "/L/M" : "");

        if (!IsPresetAvailable(ps_r_dlss_preset))
        {
            Msg("!![%s] DLSS preset [%u] is unavailable, falling back to Default", __FUNCTION__, ps_r_dlss_preset);
            ps_r_dlss_preset = NVSDK_NGX_DLSS_Hint_Render_Preset_Default;
        }
    }

    bool QueryOptimalSettings(const u32 displayWidth, const u32 displayHeight, const NVSDK_NGX_PerfQuality_Value quality, DlssResolutionInfo& out) const
    {
        if (!NgxParameters)
            return false;

        const NVSDK_NGX_Result result = NGX_DLSS_GET_OPTIMAL_SETTINGS(NgxParameters, displayWidth, displayHeight, quality, &out.renderWidth, &out.renderHeight,
            &out.maxRenderWidth, &out.maxRenderHeight, &out.minRenderWidth, &out.minRenderHeight, &out.sharpness);
        if (result != NVSDK_NGX_Result_Success)
        {
            Msg("!![%s] failed for quality [%d]. result: [%d]", __FUNCTION__, quality, result);
            return false;
        }

        Msg("--[DLSS] quality: [%d], output: [%ux%u], recommended render: [%ux%u], range: [%ux%u]-[%ux%u], sharpness: [%.3f]", quality,
            displayWidth, displayHeight, out.renderWidth, out.renderHeight, out.minRenderWidth, out.minRenderHeight, out.maxRenderWidth, out.maxRenderHeight, out.sharpness);
        return true;
    }

    bool Create(const u64 appid, const NVSDK_NGX_Dimensions& renderSize, const NVSDK_NGX_Dimensions& displaySize, ref_rt& out_rt, const u32 quality, u32& preset,
        const NVSDK_NGX_PerfQuality_Value requested_quality)
    {
        DestroyFeature();
        if (!Initialize(appid))
        {
            Msg("!![%s] failed Initialize()!", __FUNCTION__);
            return false;
        }

        OutputRT = out_rt->pSurface;
        saved_w = out_rt->dwWidth;
        saved_h = out_rt->dwHeight;
        saved_renderSize = renderSize;

        NVSDK_NGX_Result result{};

        requestedQuality = requested_quality;
        const char* preset_name = GetDlssPresetParameterName(static_cast<NVSDK_NGX_PerfQuality_Value>(quality));

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
        dlssCreateParams.Feature.InWidth = renderSize.Width;
        dlssCreateParams.Feature.InHeight = renderSize.Height;
        // final resolution
        dlssCreateParams.Feature.InTargetWidth = displaySize.Width;
        dlssCreateParams.Feature.InTargetHeight = displaySize.Height;
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
        resetHistory = true;
        QueryLoadedDlssDllVersion();
        RefreshAvailablePresets();
        return true;
    }

    void Destroy()
    {
        DestroyFeature();

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
    }

    void DestroyFeature()
    {
        if (Handle)
        {
            NVSDK_NGX_D3D11_ReleaseFeature(Handle);
            Handle = nullptr;
        }
        DLSSCreated = false;
        OutputRT = nullptr;
    }

    bool Draw()
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

        dlssEvalParams.InRenderSubrectDimensions = saved_renderSize;

        dlssEvalParams.InJitterOffsetX = ps_r_taa_jitter_full.x;
        dlssEvalParams.InJitterOffsetY = ps_r_taa_jitter_full.y;

        dlssEvalParams.InMVScaleX = -static_cast<float>(saved_renderSize.Width) * 0.5f;
        dlssEvalParams.InMVScaleY = static_cast<float>(saved_renderSize.Height) * 0.5f;
        dlssEvalParams.InReset = resetHistory ? 1 : 0;

        const NVSDK_NGX_Result result = NGX_D3D11_EVALUATE_DLSS_EXT(HW.get_context(CHW::IMM_CTX_ID), Handle, NgxParameters, &dlssEvalParams);
        if (result != NVSDK_NGX_Result_Success)
        {
            Msg("! NGX_D3D11_EVALUATE_DLSS_EXT not valid. result: [%d]", result);
            return false;
        }

        resetHistory = false;
        return true;
    }

    void RequestHistoryReset() { resetHistory = true; }

    ~NGXWrapper() { Destroy(); }
} NGXWrapper;

bool R_dlss_is_preset_available(const u32 preset) { return NGXWrapper.IsPresetAvailable(preset); }

void R_dlss_refresh_available_presets() { NGXWrapper.RefreshAvailablePresets(); }

static FfxFsr3UpscalerQualityMode GetRequestedFsr3Quality()
{
    return static_cast<FfxFsr3UpscalerQualityMode>(
        std::clamp(ps_r_fsr3_quality, static_cast<u32>(FSR3_QUALITY_NATIVE_AA), static_cast<u32>(FSR3_QUALITY_ULTRA_PERFORMANCE)));
}

void CRenderTarget::ConfigureTemporalRenderSize()
{
    SetTemporalRenderSize(Device.dwWidth, Device.dwHeight, Device.dwWidth, Device.dwHeight);

    if (ps_r_pp_aa_mode == DLSS)
    {
        const NVSDK_NGX_PerfQuality_Value quality = GetRequestedDlssQuality();
        if (!NGXWrapper.Initialize(20082024151405ull))
        {
            ps_r_pp_aa_mode = FSR3;
        }
        else
        {
            NGXWrapper.RefreshAvailablePresets();
            DlssResolutionInfo resolutionInfo{};
            if (NGXWrapper.QueryOptimalSettings(Device.dwWidth, Device.dwHeight, quality, resolutionInfo))
            {
                SetTemporalRenderSize(resolutionInfo.renderWidth, resolutionInfo.renderHeight, Device.dwWidth, Device.dwHeight);
            }
            else
            {
                // Do not create an upscaling feature against native-sized
                // resources after a failed quality query.
                ps_r_dlss_quality = DLSS_QUALITY_DLAA;
                Msg("!![DLSS] falling back to DLAA because optimal settings could not be queried");
            }

            Msg("--[DLSS] physical render domain: [%ux%u] -> [%ux%u]", GetRenderWidth(), GetRenderHeight(), GetDisplayWidth(), GetDisplayHeight());
            return;
        }
    }

    if (ps_r_pp_aa_mode != FSR3)
        return;

    u32 renderWidth{};
    u32 renderHeight{};
    const FfxFsr3UpscalerQualityMode quality = GetRequestedFsr3Quality();
    const FfxErrorCode result =
        ffxFsr3UpscalerGetRenderResolutionFromQualityMode(&renderWidth, &renderHeight, Device.dwWidth, Device.dwHeight, quality);
    if (result != FFX_OK)
    {
        Msg("!![FSR3] failed to calculate render resolution for quality [%u]. Error: [%d]", ps_r_fsr3_quality, result);
        ps_r_fsr3_quality = FSR3_QUALITY_NATIVE_AA;
        return;
    }

    SetTemporalRenderSize(renderWidth, renderHeight, Device.dwWidth, Device.dwHeight);
    Msg("--[FSR3] quality: [%u], physical render domain: [%ux%u] -> [%ux%u]", ps_r_fsr3_quality, GetRenderWidth(), GetRenderHeight(), GetDisplayWidth(),
        GetDisplayHeight());
}

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
        if (w < D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION && h < D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION)
            break;
    }
    constexpr Flags32 flg{CRT::CreateUAV};
    rt_Generic_combine_scope.create(r2_RT_generic_combine_scope, w, h, DXGI_FORMAT_R16G16B16A16_FLOAT, 1, flg);

    Msg("--[%s] 3D Scope render size: [%u, %u]", __FUNCTION__, w, h);

    return empty_rt || saved_w != w || saved_h != h;
}

void CRenderTarget::InitDLSS()
{
    if (ps_r_pp_aa_mode != DLSS)
        return;

    const NVSDK_NGX_Dimensions renderSize{GetRenderWidth(), GetRenderHeight()};
    const NVSDK_NGX_Dimensions displaySize{GetDisplayWidth(), GetDisplayHeight()};
    const NVSDK_NGX_PerfQuality_Value requestedQuality = GetRequestedDlssQuality();

    if (!NGXWrapper.Create(20082024151405ull, renderSize, displaySize, rt_Generic_combine, requestedQuality, ps_r_dlss_preset, requestedQuality))
    {
        if (ps_r_pp_aa_mode == DLSS)
            ps_r_pp_aa_mode = FSR3;
    }
}

void CRenderTarget::DestroyDLSS()
{
    NGXWrapper.Destroy();
}

bool CRenderTarget::ProcessDLSS()
{
    PIX_EVENT(DLSS);

    const NVSDK_NGX_PerfQuality_Value requestedQuality = GetRequestedDlssQuality();

    const bool qualityMatchesPhysicalTargets = static_cast<u32>(requestedQuality) == NGXWrapper.requestedQuality;
    if (!qualityMatchesPhysicalTargets)
    {
        static u32 lastReportedQuality = u32(-1);
        if (lastReportedQuality != static_cast<u32>(requestedQuality))
        {
            Msg("--[DLSS] quality changes resize physical render targets; apply video settings or run vid_restart");
            lastReportedQuality = static_cast<u32>(requestedQuality);
        }
    }

    if (qualityMatchesPhysicalTargets && (ps_r_dlss_preset != NGXWrapper.dlssPreset || NGXWrapper.saved_w != Device.dwWidth ||
        NGXWrapper.saved_h != Device.dwHeight))
    {
        InitDLSS();
    }

    if (!NGXWrapper.Draw())
    {
        Msg("!![%s] FAILED DLSS DRAW!", __FUNCTION__);
        return false;
    }

    return true;
}

void CRenderTarget::BeginPostprocess(CBackend& cmd_list, const bool temporalOutput, const bool skip_temporal_copy)
{
    m_pp_pingponged = false;
    m_pp_current_is_combine = false;

    // The last scene target is physically render-sized. Switch both the cached
    // target dimensions and the D3D viewport before any display-sized pass.
    u_setrt(cmd_list, GetDisplayWidth(), GetDisplayHeight(), nullptr, nullptr, nullptr, nullptr);
    RImplementation.rmNormal(cmd_list);

    if (temporalOutput)
    {
        // DLSS/FSR already wrote combine. CAS (if any) samples that and writes postprocess0.
        if (skip_temporal_copy)
        {
            m_pp_current_is_combine = true;
            m_pp_remap_enabled = true;
            return;
        }

        PIX_EVENT(copy_pp_after_upscale);
        HW.get_context(cmd_list.context_id)->CopyResource(rt_Postprocess_0->pSurface, rt_Generic_combine->pSurface);
    }
    else if (GetRenderWidth() == GetDisplayWidth() && GetRenderHeight() == GetDisplayHeight())
    {
        PIX_EVENT(copy_pp_from_generic0);
        HW.get_context(cmd_list.context_id)->CopyResource(rt_Postprocess_0->pSurface, rt_Generic_0->pSurface);
    }
    else
    {
        // CopyResource cannot scale. If a temporal upscaler is unavailable,
        // stretch the render-sized scene so failure remains full-screen.
        RenderScreenTriangle(cmd_list, rt_Postprocess_0, s_temporal_resolve->E[0]);
    }

    m_pp_remap_enabled = true;
}

//*****************************************************************************************************

void CRenderTarget::ProcessCAS(CBackend& cmd_list, const bool read_combine)
{
    if (fis_zero(ps_r_cas))
        return;

    PIX_EVENT(CAS);

    const Fvector4 params{std::max(ps_r_cas, 0.01f), 0.f, 0.f, 0.f};
    // After skip_temporal_copy, current is combine (DLSS/FSR). Element 1 samples that.
    // After a combine→postprocess0 copy, current is postprocess0. Element 0 samples that.
    RenderScreenTriangle(cmd_list, pp_dst(), read_combine ? s_cas->E[1] : s_cas->E[0], [&]() { cmd_list.set_c("f_cas_intensity", params); });
    pp_flip();
}

//*****************************************************************************************************
static DXGI_FORMAT GetDxgiFormat(const FfxSurfaceFormat format)
{
    switch (format)
    {
    case FFX_SURFACE_FORMAT_R32G32B32A32_TYPELESS: return DXGI_FORMAT_R32G32B32A32_TYPELESS;
    case FFX_SURFACE_FORMAT_R32G32B32A32_UINT: return DXGI_FORMAT_R32G32B32A32_UINT;
    case FFX_SURFACE_FORMAT_R32G32B32A32_FLOAT: return DXGI_FORMAT_R32G32B32A32_FLOAT;
    case FFX_SURFACE_FORMAT_R16G16B16A16_FLOAT: return DXGI_FORMAT_R16G16B16A16_FLOAT;
    case FFX_SURFACE_FORMAT_R32G32_FLOAT: return DXGI_FORMAT_R32G32_FLOAT;
    case FFX_SURFACE_FORMAT_R8_UINT: return DXGI_FORMAT_R8_UINT;
    case FFX_SURFACE_FORMAT_R32_UINT: return DXGI_FORMAT_R32_UINT;
    case FFX_SURFACE_FORMAT_R10G10B10A2_UNORM: return DXGI_FORMAT_R10G10B10A2_UNORM;
    case FFX_SURFACE_FORMAT_R8G8B8A8_TYPELESS: return DXGI_FORMAT_R8G8B8A8_TYPELESS;
    case FFX_SURFACE_FORMAT_R8G8B8A8_UNORM: return DXGI_FORMAT_R8G8B8A8_UNORM;
    case FFX_SURFACE_FORMAT_R8G8B8A8_SNORM: return DXGI_FORMAT_R8G8B8A8_SNORM;
    case FFX_SURFACE_FORMAT_R8G8B8A8_SRGB: return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    case FFX_SURFACE_FORMAT_R11G11B10_FLOAT: return DXGI_FORMAT_R11G11B10_FLOAT;
    case FFX_SURFACE_FORMAT_R16G16_FLOAT: return DXGI_FORMAT_R16G16_FLOAT;
    case FFX_SURFACE_FORMAT_R16G16_UINT: return DXGI_FORMAT_R16G16_UINT;
    case FFX_SURFACE_FORMAT_R16G16_SINT: return DXGI_FORMAT_R16G16_SINT;
    case FFX_SURFACE_FORMAT_R16_FLOAT: return DXGI_FORMAT_R16_FLOAT;
    case FFX_SURFACE_FORMAT_R16_UINT: return DXGI_FORMAT_R16_UINT;
    case FFX_SURFACE_FORMAT_R16_UNORM: return DXGI_FORMAT_R16_UNORM;
    case FFX_SURFACE_FORMAT_R16_SNORM: return DXGI_FORMAT_R16_SNORM;
    case FFX_SURFACE_FORMAT_R8_UNORM: return DXGI_FORMAT_R8_UNORM;
    case FFX_SURFACE_FORMAT_R8G8_UNORM: return DXGI_FORMAT_R8G8_UNORM;
    case FFX_SURFACE_FORMAT_R8G8_UINT: return DXGI_FORMAT_R8G8_UINT;
    case FFX_SURFACE_FORMAT_R32_FLOAT: return DXGI_FORMAT_R32_FLOAT;
    default: return DXGI_FORMAT_UNKNOWN;
    }
}

static HRESULT CreateFsrSharedTexture(const FfxCreateResourceDescription& createDescription, ID3D11Texture2D** texture)
{
    const FfxResourceDescription& resource = createDescription.resourceDescription;
    if (resource.type != FFX_RESOURCE_TYPE_TEXTURE2D)
        return E_INVALIDARG;

    D3D11_TEXTURE2D_DESC description{};
    description.Width = resource.width;
    description.Height = resource.height;
    description.MipLevels = resource.mipCount;
    description.ArraySize = _max(1u, resource.depth);
    description.Format = GetDxgiFormat(resource.format);
    description.SampleDesc.Count = 1;
    description.Usage = D3D11_USAGE_DEFAULT;
    description.BindFlags = D3D11_BIND_SHADER_RESOURCE;

    if (resource.usage & FFX_RESOURCE_USAGE_UAV)
        description.BindFlags |= D3D11_BIND_UNORDERED_ACCESS;
    if (resource.usage & FFX_RESOURCE_USAGE_RENDERTARGET)
        description.BindFlags |= D3D11_BIND_RENDER_TARGET;
    if (resource.usage & FFX_RESOURCE_USAGE_DEPTHTARGET)
        description.BindFlags |= D3D11_BIND_DEPTH_STENCIL;

    if (description.Format == DXGI_FORMAT_UNKNOWN || !description.Width || !description.Height || !description.MipLevels)
        return E_INVALIDARG;

    return HW.pDevice->CreateTexture2D(&description, nullptr, texture);
}

static class Fsr3Wrapper
{
    bool fsr_created{};
    bool resetHistory{true};
    FfxFsr3UpscalerContext m_UpscalerContext{};
    xr_vector<char> m_scratchBuffer;
    ID3D11Resource* OutputRT{};
    FfxDimensions2D saved_maxRenderSize{}, saved_displaySize{};

    ID3D11Texture2D* dilatedDepth{};
    ID3D11Texture2D* dilatedMotionVectors{};
    ID3D11Texture2D* reconstructedPrevNearestDepth{};
public:
    u32 saved_w{}, saved_h{}, requestedQuality{};

    bool Create(const FfxDimensions2D& maxRenderSize, const FfxDimensions2D& displaySize, ref_rt& out_rt, const u32 quality)
    {
        OutputRT = out_rt->pSurface;
        saved_w = out_rt->dwWidth;
        saved_h = out_rt->dwHeight;
        saved_maxRenderSize = maxRenderSize;
        saved_displaySize = displaySize;
        requestedQuality = quality;

        if (fsr_created)
        {
            Destroy();
        }

        if (HW.FeatureLevel < D3D_FEATURE_LEVEL_11_0)
        {
            Msg("!![%s] Low FeatureLevel: [%d]", __FUNCTION__, HW.FeatureLevel);
        }

        m_scratchBuffer.resize(ffxGetScratchMemorySizeDX11(FFX_FSR3UPSCALER_CONTEXT_COUNT));

        FfxInterface fsrInterface{};
        auto fsrDevice = ffxGetDeviceDX11(HW.pDevice);
        FfxErrorCode errorCode = ffxGetInterfaceDX11(&fsrInterface, fsrDevice, m_scratchBuffer.data(), m_scratchBuffer.size(), FFX_FSR3UPSCALER_CONTEXT_COUNT);
        if (errorCode != FFX_OK)
        {
            Msg("!!Failed ffxGetInterfaceDX11! Error: [%d]", errorCode);
            Destroy();
            return false;
        }

        FfxFsr3UpscalerContextDescription m_UpscalercontextDesc{};
        m_UpscalercontextDesc.backendInterface = fsrInterface;
        m_UpscalercontextDesc.maxRenderSize = maxRenderSize;
        m_UpscalercontextDesc.maxUpscaleSize = displaySize;
        m_UpscalercontextDesc.flags = FFX_FSR3UPSCALER_ENABLE_HIGH_DYNAMIC_RANGE;

        errorCode = ffxFsr3UpscalerContextCreate(&m_UpscalerContext, &m_UpscalercontextDesc);
        if (errorCode != FFX_OK)
        {
            Msg("!!Failed ffxFsr3UpscalerContextCreate! Error: [%d]", errorCode);
            Destroy();
            return false;
        }

        fsr_created = true;
        resetHistory = true;

        FfxFsr3UpscalerSharedResourceDescriptions sharedDescriptions{};
        errorCode = ffxFsr3UpscalerGetSharedResourceDescriptions(&m_UpscalerContext, &sharedDescriptions);
        if (errorCode != FFX_OK)
        {
            Msg("!!Failed ffxFsr3UpscalerGetSharedResourceDescriptions! Error: [%d]", errorCode);
            Destroy();
            return false;
        }

        const HRESULT dilatedDepthResult = CreateFsrSharedTexture(sharedDescriptions.dilatedDepth, &dilatedDepth);
        const HRESULT dilatedMotionResult = CreateFsrSharedTexture(sharedDescriptions.dilatedMotionVectors, &dilatedMotionVectors);
        const HRESULT reconstructedDepthResult =
            CreateFsrSharedTexture(sharedDescriptions.reconstructedPrevNearestDepth, &reconstructedPrevNearestDepth);

        if (FAILED(dilatedDepthResult) || FAILED(dilatedMotionResult) || FAILED(reconstructedDepthResult))
        {
            Msg("!!Failed to create FSR3 shared resources: [0x%08x, 0x%08x, 0x%08x]", dilatedDepthResult, dilatedMotionResult, reconstructedDepthResult);
            Destroy();
            return false;
        }

        return true;
    }

    void Destroy()
    {
        if (fsr_created)
        {
            ffxFsr3UpscalerContextDestroy(&m_UpscalerContext);
            fsr_created = false;
        }

        _RELEASE(dilatedDepth);
        _RELEASE(dilatedMotionVectors);
        _RELEASE(reconstructedPrevNearestDepth);
        resetHistory = true;
    }

    bool Draw()
    {
        if (!fsr_created)
        {
            Msg("! Fsr3Wrapper not created!");
            return false;
        }

        FfxFsr3UpscalerDispatchDescription dispatchParameters{};

        dispatchParameters.commandList = ffxGetCommandListDX11(HW.get_context(CHW::IMM_CTX_ID));

        dispatchParameters.color = ffxGetResourceDX11(RImplementation.Target->rt_Generic_0->pSurface, GetFfxResourceDescriptionDX11(RImplementation.Target->rt_Generic_0->pSurface), L"FSR3_InputColor");
        dispatchParameters.depth = ffxGetResourceDX11(RImplementation.Target->rt_zbuffer->pSurface, GetFfxResourceDescriptionDX11(RImplementation.Target->rt_zbuffer->pSurface), L"FSR3_InputDepth");

        dispatchParameters.motionVectors = ffxGetResourceDX11(RImplementation.Target->rt_Velocity->pSurface, GetFfxResourceDescriptionDX11(RImplementation.Target->rt_Velocity->pSurface), L"FSR3_InputMotionVectors");
        dispatchParameters.exposure = ffxGetResourceDX11(nullptr, {}, L"FSR3_InputExposure");

        dispatchParameters.reactive = ffxGetResourceDX11(nullptr, {}, L"FSR3_InputReactiveMap");
        dispatchParameters.transparencyAndComposition = ffxGetResourceDX11(nullptr, {}, L"FSR3_TransparencyAndCompositionMap");

        dispatchParameters.dilatedDepth = ffxGetResourceDX11(dilatedDepth, GetFfxResourceDescriptionDX11(dilatedDepth), L"FSR3_dilatedDepth", FFX_RESOURCE_STATE_UNORDERED_ACCESS);
        dispatchParameters.dilatedMotionVectors =
            ffxGetResourceDX11(dilatedMotionVectors, GetFfxResourceDescriptionDX11(dilatedMotionVectors), L"FSR3_DilatedMotion", FFX_RESOURCE_STATE_UNORDERED_ACCESS);
        dispatchParameters.reconstructedPrevNearestDepth = ffxGetResourceDX11(reconstructedPrevNearestDepth, GetFfxResourceDescriptionDX11(reconstructedPrevNearestDepth),
                                                                               L"FSR3_reconstructedPrevNearestDepth", FFX_RESOURCE_STATE_UNORDERED_ACCESS);

        dispatchParameters.output = ffxGetResourceDX11(OutputRT, GetFfxResourceDescriptionDX11(OutputRT), L"FSR3_OutputUpscaledColor", FFX_RESOURCE_STATE_UNORDERED_ACCESS);

        dispatchParameters.jitterOffset.x = ps_r_taa_jitter_full.x;
        dispatchParameters.jitterOffset.y = ps_r_taa_jitter_full.y;

        dispatchParameters.motionVectorScale.x = -static_cast<float>(saved_maxRenderSize.width) * 0.5f;
        dispatchParameters.motionVectorScale.y = static_cast<float>(saved_maxRenderSize.height) * 0.5f;

        dispatchParameters.frameTimeDelta = std::max(1.0f + EPS_L, Device.fTimeDeltaRealMS); // The time elapsed since the last frame (expressed in milliseconds).

        dispatchParameters.preExposure = 1.0f;

        dispatchParameters.renderSize = saved_maxRenderSize;
        dispatchParameters.upscaleSize = saved_displaySize;

        dispatchParameters.cameraFar = g_pGamePersistent->Environment().CurrentEnv->far_plane;
        dispatchParameters.cameraNear = VIEWPORT_NEAR;

        dispatchParameters.cameraFovAngleVertical = deg2rad(Device.fFOV);

        dispatchParameters.viewSpaceToMetersFactor = 1.0f;
        dispatchParameters.reset = resetHistory;

        const FfxErrorCode errorCode = ffxFsr3UpscalerContextDispatch(&m_UpscalerContext, &dispatchParameters);

        if (errorCode != FFX_OK)
        {
            Msg("! ffxFsr3UpscalerContextDispatch not valid. Error: [%d]", errorCode);
            return false;
        }

        resetHistory = false;
        return true;
    }

    void RequestHistoryReset() { resetHistory = true; }

    ~Fsr3Wrapper() { Destroy(); }
} Fsr3Wrapper , Fsr3WrapperScope;

void CRenderTarget::InitFSR()
{
    Fsr3Wrapper.Destroy();
    Fsr3WrapperScope.Destroy();

    const FfxDimensions2D renderSize{GetRenderWidth(), GetRenderHeight()};
    const FfxDimensions2D displaySize{GetDisplayWidth(), GetDisplayHeight()};

    const u32 quality = static_cast<u32>(GetRequestedFsr3Quality());
    if (!Fsr3Wrapper.Create(renderSize, displaySize, rt_Generic_combine, quality))
    {
        if (ps_r_pp_aa_mode == FSR3)
            ps_r_pp_aa_mode = TAA;
    }
    else
    {
        reset_3dss_rendertarget();

        const FfxDimensions2D ScopeSize{rt_Generic_combine_scope->dwWidth, rt_Generic_combine_scope->dwHeight};
        R_ASSERT(Fsr3WrapperScope.Create(renderSize, ScopeSize, rt_Generic_combine_scope, FSR3_QUALITY_NATIVE_AA));
    }
}

void CRenderTarget::DestroyFSR()
{
    Fsr3Wrapper.Destroy();
    Fsr3WrapperScope.Destroy();
}

void CRenderTarget::ResetTemporalHistory()
{
    m_resetTemporalHistory = true;
    NGXWrapper.RequestHistoryReset();
    Fsr3Wrapper.RequestHistoryReset();
    Fsr3WrapperScope.RequestHistoryReset();
}

bool CRenderTarget::ProcessFSR()
{
    PIX_EVENT(FSR);

    if (ps_r_pp_aa_mode == FSR3 && ps_r_fsr3_quality != Fsr3Wrapper.requestedQuality)
    {
        static u32 lastReportedQuality = u32(-1);
        if (lastReportedQuality != ps_r_fsr3_quality)
        {
            Msg("--[FSR3] quality changes resize physical render targets; apply video settings or run vid_restart");
            lastReportedQuality = ps_r_fsr3_quality;
        }
    }

    if (!Fsr3Wrapper.Draw())
    {
        Msg("!![%s] FAILED FSR DRAW!", __FUNCTION__);
        return false;
    }

    return true;
}

bool CRenderTarget::ProcessFSR_3DSS(const bool need_reset)
{
    PIX_EVENT(3D_SCOPE_FSR);

    if (need_reset)
    {
        InitFSR();
    }

    if (!Fsr3WrapperScope.Draw())
    {
        Msg("!![%s] FAILED 3D SCOPE FSR DRAW!", __FUNCTION__);
        return false;
    }

    return true;
}
//*****************************************************************************************************

void CRenderTarget::PhaseAA(CBackend& cmd_list)
{
    bool temporalOutput = false;

    switch (ps_r_pp_aa_mode)
    {
        case DLSS: {
            temporalOutput = ProcessDLSS();
            if (!temporalOutput)
            {
                ps_r_pp_aa_mode = FSR3;
                temporalOutput = ProcessFSR();
                if (!temporalOutput)
                    ps_r_pp_aa_mode = TAA;
            }
            break;
        }
        case FSR3: {
            u_setrt(cmd_list, get_width(cmd_list), get_height(cmd_list), nullptr, nullptr, nullptr, nullptr);
            RImplementation.rmNormal(cmd_list);

            temporalOutput = ProcessFSR();
            if (!temporalOutput)
                ps_r_pp_aa_mode = TAA;
            break;
        }
        case TAA: ProcessTAA(cmd_list); break;
        case SMAA: ProcessSMAA(cmd_list); break;
    }

    EndTemporalUpscaleInput();
    RImplementation.rmNormal(cmd_list);

    const bool cas = !fis_zero(ps_r_cas) && ps_r_pp_aa_mode != SMAA;
    const bool cas_from_combine = temporalOutput && cas && s_cas->E[1];

    BeginPostprocess(cmd_list, temporalOutput, cas_from_combine);

    if (ps_r_pp_aa_mode != SMAA)
        ProcessCAS(cmd_list, cas_from_combine);
}

//*****************************************************************************************************

bool CRenderTarget::Phase3DSSUpscale(CBackend& cmd_list)
{
    if (ps_r_dlss_3dss_scale_factor <= 1.f)
        return false;

    // Проверки на сглаживание для того, что для апскейлинга нам нужен taa джиттер
    if (ps_r_pp_aa_mode != DLSS && ps_r_pp_aa_mode != FSR3 && ps_r_pp_aa_mode != TAA)
        return false;

    bool need_reset = reset_3dss_rendertarget();
    if (!need_reset)
        need_reset = (Fsr3WrapperScope.saved_w != rt_Generic_combine_scope->dwWidth || Fsr3WrapperScope.saved_h != rt_Generic_combine_scope->dwHeight);

    return ProcessFSR_3DSS(need_reset);
}

//*****************************************************************************************************
