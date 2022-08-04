#include "stdafx.h"
#include "WeaponBinoculars.h"

#include "xr_level_controller.h"

#include "level.h"
#include "ui\UIFrameWindow.h"
#include "WeaponBinocularsVision.h"
#include "object_broker.h"
#include "hudmanager.h"
CWeaponBinoculars::CWeaponBinoculars() : CWeaponCustomPistol("BINOCULARS") {}

CWeaponBinoculars::~CWeaponBinoculars()
{
    HUD_SOUND::DestroySound(sndZoomIn);
    HUD_SOUND::DestroySound(sndZoomOut);
}

void CWeaponBinoculars::Load(LPCSTR section)
{
    inherited::Load(section);

    // Sounds
    HUD_SOUND::LoadSound(section, "snd_zoomin", sndZoomIn, SOUND_TYPE_ITEM_USING);
    HUD_SOUND::LoadSound(section, "snd_zoomout", sndZoomOut, SOUND_TYPE_ITEM_USING);
}

bool CWeaponBinoculars::Action(s32 cmd, u32 flags)
{
    if (!Core.Features.test(xrCore::Feature::binoc_firing))
    {
        switch (cmd)
        {
        case kWPN_FIRE: return inherited::Action(kWPN_ZOOM, flags);
        }
    }

    return inherited::Action(cmd, flags);
}

void CWeaponBinoculars::OnZoomIn()
{
    if (H_Parent() && !IsZoomed())
    {
        HUD_SOUND::StopSound(sndZoomOut);
        bool b_hud_mode = (Level().CurrentEntity() == H_Parent());
        HUD_SOUND::PlaySound(sndZoomIn, H_Parent()->Position(), H_Parent(), b_hud_mode);
    }

    inherited::OnZoomIn();
}

void CWeaponBinoculars::OnZoomOut()
{
    if (H_Parent() && IsZoomed() && !IsRotatingToZoom())
    {
        HUD_SOUND::StopSound(sndZoomIn);
        bool b_hud_mode = (Level().CurrentEntity() == H_Parent());
        HUD_SOUND::PlaySound(sndZoomOut, H_Parent()->Position(), H_Parent(), b_hud_mode);
    }

    inherited::OnZoomOut();
}

BOOL CWeaponBinoculars::net_Spawn(CSE_Abstract* DC)
{
    inherited::net_Spawn(DC);
    return TRUE;
}

void CWeaponBinoculars::net_Destroy() { inherited::net_Destroy(); }

void CWeaponBinoculars::UpdateCL() { inherited::UpdateCL(); }

void CWeaponBinoculars::OnDrawUI() { inherited::OnDrawUI(); }

void CWeaponBinoculars::save(NET_Packet& output_packet)
{
    inherited::save(output_packet);
    save_data(m_fRTZoomFactor, output_packet);
}

void CWeaponBinoculars::load(IReader& input_packet)
{
    inherited::load(input_packet);
    load_data(m_fRTZoomFactor, input_packet);
}

void CWeaponBinoculars::GetBriefInfo(xr_string& str_name, xr_string& icon_sect_name, xr_string& str_count)
{
    if (Core.Features.test(xrCore::Feature::binoc_firing))
    {
        inherited::GetBriefInfo(str_name, icon_sect_name, str_count);
    }
    else
    {
        str_name = NameShort();
        str_count = "";
        icon_sect_name = *cNameSect();
    }
}

void CWeaponBinoculars::net_Relcase(CObject* object) { inherited::net_Relcase(object); }
