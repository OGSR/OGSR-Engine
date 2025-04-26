#include "stdafx.h"

#include "../Include/xrRender/DrawUtils.h"
#include "render.h"

void CRenderDevice::_SetupStates()
{
    // General Render States
    mView.identity();
    mProject.identity();
    mFullTransform.identity();
    vCameraPosition.set(0, 0, 0);
    vCameraDirection.set(0, 0, 1);
    vCameraTop.set(0, 1, 0);
    vCameraRight.set(1, 0, 0);

    m_pRender->SetupStates();
}

void CRenderDevice::_Create()
{
    Memory.mem_compact();

    // after creation
    b_is_Ready = TRUE;

    _SetupStates();

    m_pRender->OnDeviceCreate();

    dwFrame = 0;
}

void CRenderDevice::ConnectToRender()
{
    if (!m_pRender)
        m_pRender = RenderFactory->CreateRenderDeviceRender();
}

void CRenderDevice::Create()
{
    if (b_is_Ready)
        return; // prevent double call

    ZoneScoped;

    Statistic = xr_new<CStats>();

    cdb_clRAY = &Statistic->clRAY; // total: ray-testing
    cdb_clBOX = &Statistic->clBOX; // total: box query
    cdb_clFRUSTUM = &Statistic->clFRUSTUM; // total: frustum query

    if (!m_pRender)
        m_pRender = RenderFactory->CreateRenderDeviceRender();

    Log("Starting RENDER device...");

    fFOV = 90.f;
    fASPECT = 1.f;
    m_pRender->Create(m_hWnd, dwWidth, dwHeight, fWidth_2, fHeight_2);
 
    _Create();

    PreCache(0, false, false);
}
