#include "stdafx.h"


#include "Environment.h"
#include "render.h"
#include "xr_efflensflare.h"
#include "rain.h"
#include "thunderbolt.h"
#include "igame_level.h"

//-----------------------------------------------------------------------------
// Environment render
//-----------------------------------------------------------------------------
// BOOL bNeed_re_create_env = FALSE;
void CEnvironment::RenderSky(CBackend& cmd_list)
{
    if (nullptr == g_pGameLevel)
        return;

    m_pRender->RenderSky(cmd_list , * this);
}

void CEnvironment::RenderClouds(CBackend& cmd_list)
{
    if (nullptr == g_pGameLevel)
        return;

    // draw clouds
    if (fis_zero(CurrentEnv->clouds_color.w, EPS_L))
        return;

    m_pRender->RenderClouds(cmd_list, *this);
}

void CEnvironment::RenderFlares(CBackend& cmd_list)
{
    if (nullptr == g_pGameLevel)
        return;
    // 1
    eff_LensFlare->Render(cmd_list, FALSE, TRUE, TRUE);
}

void CEnvironment::RenderLast(CBackend& cmd_list)
{
    if (nullptr == g_pGameLevel)
        return;

    ZoneScoped;

    if (async_started)
    {
        if (awaiter.valid())
        {
            awaiter.wait();
        }

        async_started = false;
    }
    else
    {
        eff_Rain->Calculate();
    }

    // 2
    eff_Rain->Render(cmd_list);
    eff_Thunderbolt->Render(cmd_list);
}

void CEnvironment::OnDeviceCreate()
{
    m_pRender->OnDeviceCreate();

    // weathers
    {
        EnvsMapIt _I, _E;
        _I = WeatherCycles.begin();
        _E = WeatherCycles.end();
        for (; _I != _E; ++_I)
            for (auto& it : _I->second)
                it->on_device_create();
    }
    // effects
    {
        EnvsMapIt _I, _E;
        _I = WeatherFXs.begin();
        _E = WeatherFXs.end();
        for (; _I != _E; ++_I)
            for (auto& it : _I->second)
                it->on_device_create();
    }

    Invalidate();
    OnFrame();
}

void CEnvironment::OnDeviceDestroy()
{
    m_pRender->OnDeviceDestroy();

    // weathers
    {
        EnvsMapIt _I, _E;
        _I = WeatherCycles.begin();
        _E = WeatherCycles.end();
        for (; _I != _E; ++_I)
            for (auto& it : _I->second)
                it->on_device_destroy();
    }
    // effects
    {
        EnvsMapIt _I, _E;
        _I = WeatherFXs.begin();
        _E = WeatherFXs.end();
        for (; _I != _E; ++_I)
            for (auto& it : _I->second)
                it->on_device_destroy();
    }

    CurrentEnv->destroy();
}

void CEnvironment::StartCalculateAsync()
{
    if (nullptr == g_pGameLevel)
        return;

    awaiter = TTAPI->submit([this]() { eff_Rain->Calculate(); });

    async_started = true;
}
