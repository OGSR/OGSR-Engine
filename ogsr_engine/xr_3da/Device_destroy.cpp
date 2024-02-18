#include "stdafx.h"

#include "../Include/xrRender/DrawUtils.h"
#include "render.h"
#include "IGame_Persistent.h"
#include "xr_IOConsole.h"
#include "xr_input.h"

void CRenderDevice::_Destroy(BOOL bKeepTextures)
{
    DU->OnDeviceDestroy();

    // before destroy
    b_is_Ready = FALSE;
    Statistic->OnDeviceDestroy();
    ::Render->destroy();
    m_pRender->OnDeviceDestroy(bKeepTextures);
    // Resources->OnDeviceDestroy	(bKeepTextures);
    // RCache.OnDeviceDestroy		();

    Memory.mem_compact();
}

void CRenderDevice::Destroy(void)
{
    if (!b_is_Ready)
        return;

    Log("Destroying Direct3D...");

    pInput->clip_cursor(false);

    _Destroy(FALSE);

    // real destroy
    m_pRender->DestroyHW();

    // xr_delete					(Resources);
    // HW.DestroyDevice			();

    seqRender.R.clear();
    seqAppActivate.R.clear();
    seqAppDeactivate.R.clear();
    seqAppStart.R.clear();
    seqAppEnd.R.clear();
    seqFrame.R.clear();
    seqFrameMT.R.clear();
    seqDeviceReset.R.clear();
    seqParallel.clear();

    RenderFactory->DestroyRenderDeviceRender(m_pRender);
    m_pRender = 0;
    xr_delete(Statistic);
}

void CRenderDevice::Reset(bool precache)
{
    u32 dwWidth_before = dwWidth;
    u32 dwHeight_before = dwHeight;

    pInput->clip_cursor(false);

    u32 tm_start = TimerAsync();

    m_pRender->Reset(m_hWnd, dwWidth, dwHeight, fWidth_2, fHeight_2);

    if (g_pGamePersistent)
    {
        g_pGamePersistent->Environment().bNeed_re_create_env = TRUE;
    }

    _SetupStates();

    if (precache)
        PreCache(20, false, false);

    u32 tm_end = TimerAsync();

    Msg("*** RESET [%d ms]", tm_end - tm_start);

    //	TODO: Remove this! It may hide crash
#pragma todo("KRodin: ??? Remove this! It may hide crash ???")
    Memory.mem_compact();

    pInput->clip_cursor(true);

    seqDeviceReset.Process(rp_DeviceReset);

    if (dwWidth_before != dwWidth || dwHeight_before != dwHeight)
    {
        seqResolutionChanged.Process(rp_ScreenResolutionChanged);
    }
}
