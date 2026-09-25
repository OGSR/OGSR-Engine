#include "stdafx.h"

#include "ZoneCampfire.h"
#include "ParticlesObject.h"
#include "GamePersistent.h"
#include "../xr_3da/LightAnimLibrary.h"

CZoneCampfire::CZoneCampfire() : m_pDisabledParticles(nullptr), m_pEnablingParticles(nullptr), m_turned_on(true), m_turn_time(0) {}
CZoneCampfire::~CZoneCampfire()
{
    CParticlesObject::Destroy(m_pDisabledParticles);
    CParticlesObject::Destroy(m_pEnablingParticles);

    m_disabled_sound.destroy();
}

void CZoneCampfire::Load(const char* section) { inherited::Load(section); }

void CZoneCampfire::GoEnabledState()
{
    inherited::GoEnabledState();

    if (m_pDisabledParticles)
    {
        // Msg("* [%s]: 1 stop disabled particles: %s", __FUNCTION__, cNameSect().c_str());
        m_pDisabledParticles->Stop(false);
        CParticlesObject::Destroy(m_pDisabledParticles);
    }

    m_disabled_sound.stop();
    m_disabled_sound.destroy();

    if (pSettings->line_exist(cNameSect(), "enabling_particles"))
    {
        const char* str = pSettings->r_string(cNameSect(), "enabling_particles");
        m_pEnablingParticles = CParticlesObject::Create(str, false);
        m_pEnablingParticles->UpdateParent(XFORM(), {});
        m_pEnablingParticles->Play(false);
        // Msg("* [%s]: start enabling particles: %s", __FUNCTION__, cNameSect().c_str());
    }
}

void CZoneCampfire::GoDisabledState()
{
    inherited::GoDisabledState();

    if (pSettings->line_exist(cNameSect(), "disabled_particles"))
    {
        R_ASSERT(!m_pDisabledParticles);
        const char* str = pSettings->r_string(cNameSect(), "disabled_particles");
        m_pDisabledParticles = CParticlesObject::Create(str, false);
        m_pDisabledParticles->UpdateParent(XFORM(), {});
        m_pDisabledParticles->Play(false);
        // Msg("* [%s]: start disabled particles: %s", __FUNCTION__, cNameSect().c_str());
    }

    const char* str = pSettings->r_string(cNameSect(), "disabled_sound");
    m_disabled_sound.create(str, st_Effect, sg_SourceType);
    m_disabled_sound.play_at_pos(nullptr, Position());
}

#define OVL_TIME 3000

void CZoneCampfire::turn_on_script()
{
    if (ZoneState() != eZoneStateIdle)
    {
        m_turn_time = Device.dwTimeGlobal + OVL_TIME;
        m_turned_on = true;
        GoEnabledState();
    }
}

void CZoneCampfire::turn_off_script()
{
    constexpr bool force = false;
    if (ZoneState() != eZoneStateDisabled)
    {
        if (force)
        {
            if (m_pDisabledParticles)
            {
                // Msg("* [%s]: 2 stop disabled particles: %s", __FUNCTION__, cNameSect().c_str());
                m_pDisabledParticles->Stop(false);
                CParticlesObject::Destroy(m_pDisabledParticles);
            }
            if (m_pEnablingParticles)
            {
                // Msg("* [%s]: 2 stop enabling particles: %s", __FUNCTION__, cNameSect().c_str());
                m_pEnablingParticles->Stop(false);
                CParticlesObject::Destroy(m_pEnablingParticles);
            }

            m_turn_time = 0;
            m_turned_on = false;
            inherited::GoDisabledState();
        }
        else
        {
            m_turn_time = Device.dwTimeGlobal + OVL_TIME;
            m_turned_on = false;
            GoDisabledState();
        }
    }
}

bool CZoneCampfire::is_on() const { return m_turned_on; }

void CZoneCampfire::shedule_Update(u32 dt)
{
    if (!IsEnabled() && m_turn_time)
    {
        UpdateWorkload(dt);
    }

    if (m_pIdleParticles)
    {
        Fvector vel;
        vel.mul(GamePersistent().Environment().wind_blast_direction, GamePersistent().Environment().wind_strength_factor);
        m_pIdleParticles->UpdateParent(XFORM(), vel);
    }
    inherited::shedule_Update(dt);
}

void CZoneCampfire::PlayIdleParticles(bool bIdleLight)
{
    // if (m_turn_time == 0 || m_turn_time - Device.dwTimeGlobal < (OVL_TIME - 2000))
    {
        inherited::PlayIdleParticles(bIdleLight);
    }
}

void CZoneCampfire::StopIdleParticles(bool bIdleLight)
{
    // if (m_turn_time == 0 || m_turn_time - Device.dwTimeGlobal < (OVL_TIME - 500))
    {
        inherited::StopIdleParticles(bIdleLight);
    }
}

BOOL CZoneCampfire::AlwaysTheCrow()
{
    if (m_turn_time)
        return TRUE;

    return inherited::AlwaysTheCrow();
}

void CZoneCampfire::UpdateWorkload(u32 dt)
{
    inherited::UpdateWorkload(dt);

    if (m_turn_time == 0 || m_turn_time - Device.dwTimeGlobal < OVL_TIME - 2000)
    {
        if (m_pEnablingParticles)
        {
            // Msg("* [%s]: 2 stop enabling particles: %s", __FUNCTION__, cNameSect().c_str());
            m_pEnablingParticles->Stop(false);
            CParticlesObject::Destroy(m_pEnablingParticles);
        }
    }

    if (m_turn_time > Device.dwTimeGlobal)
    {
        float k = static_cast<float>(m_turn_time - Device.dwTimeGlobal) / static_cast<float>(OVL_TIME);

        if (m_turned_on)
        {
            k = 1.0f - k;
        }
        else
        {
            StartIdleLight();
        }

        if (m_pIdleLight && m_pIdleLight->get_active())
        {
            R_ASSERT(m_pIdleLAnim);

            int frame{};
            u32 clr = m_pIdleLAnim->CalculateRGB(Device.fTimeGlobal, frame);
            Fcolor fclr;
            fclr.set(static_cast<float>(color_get_R(clr)) / 255.f * k, static_cast<float>(color_get_G(clr)) / 255.f * k, static_cast<float>(color_get_B(clr)) / 255.f * k, 1.f);

            float range = m_fIdleLightRange + 0.25f * ::Random.randF(-1.f, 1.f);
            range *= k;

            m_pIdleLight->set_range(range);
            m_pIdleLight->set_color(fclr);
        }
    }
    else if (m_turn_time)
    {
        m_turn_time = 0;

        if (m_pDisabledParticles)
        {
            // Msg("* [%s]: 2 stop disabled particles: %s", __FUNCTION__, cNameSect().c_str());
            m_pDisabledParticles->Stop(false);
            CParticlesObject::Destroy(m_pDisabledParticles);
        }
    }
}
