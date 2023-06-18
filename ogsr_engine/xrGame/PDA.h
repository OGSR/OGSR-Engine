#pragma once

#include "..\xr_3da\feel_touch.h"
#include "hud_item_object.h"
#include "InfoPortionDefs.h"
#include "character_info_defs.h"
#include "PdaMsg.h"
#include "HudSound.h"

class CPda : public CHudItemObject, public Feel::Touch
{
    using inherited = CHudItemObject;

public:
    CPda();
    virtual ~CPda();

    virtual BOOL net_Spawn(CSE_Abstract* DC) override;
    virtual void Load(LPCSTR section) override;
    virtual void net_Destroy() override;
    virtual void net_Relcase(CObject* O) override;

    virtual void OnH_A_Chield() override;
    virtual void OnH_B_Independent(bool just_before_destroy) override;

    virtual void shedule_Update(u32 dt) override;

    virtual void feel_touch_new(CObject* O) override;
    virtual void feel_touch_delete(CObject* O) override;
    virtual BOOL feel_touch_contact(CObject* O) override;

    u16 GetOriginalOwnerID() const { return m_idOriginalOwner; }
    CInventoryOwner* GetOriginalOwner() const;

    void TurnOn();
    void TurnOff();

    bool IsActive() const { return IsOn(); }
    bool IsOn() const { return !m_bTurnedOff; }
    bool IsOff() const { return m_bTurnedOff; }

    xr_map<u16, CPda*> ActivePDAContacts();
    CPda* GetPdaFromOwner(CObject* owner);
    u32 ActiveContactsNum() const { return m_active_contacts.size(); }

    virtual void save(NET_Packet& output_packet) override;
    virtual void load(IReader& input_packet) override;

    virtual LPCSTR Name() override;

private:
    void UpdateActiveContacts();

    xr_vector<CObject*> m_active_contacts;
    float m_fRadius;
    bool m_changed;

    u16 m_idOriginalOwner;
    shared_str m_SpecificChracterOwner;
    xr_string m_sFullName;

    bool m_bTurnedOff;

    const char* m_joystick_bone{};
    u16 joystick{BI_NONE};
    static void JoystickCallback(CBoneInstance* B);

    HUD_SOUND sndShow, sndHide, sndBtnPress, sndBtnRelease;

    bool this_is_3d_pda{};

public:
    bool Is3DPDA() const { return this_is_3d_pda; }
    virtual void OnStateSwitch(u32 S, u32 oldState) override;
    virtual void OnAnimationEnd(u32 state) override;

    virtual void PlayAnimIdle() override;
    bool ThumbAnimsAllowed() const { return joystick == BI_NONE; }

    virtual void OnMoveToRuck(EItemPlace prevPlace) override;
    virtual void UpdateCL() override;
    virtual void UpdateXForm() override;
    virtual void OnActiveItem() override;
    virtual void OnHiddenItem() override;

    bool m_bZoomed{};
    float m_thumb_rot[2]{};
    xr_string thumb_anim_name;

    u8 GetCurrentHudOffsetIdx() const override { return (m_bZoomed || m_fZoomRotationFactor != 0.f) ? 1 : 0; }
    bool IsZoomed() const override { return m_bZoomed; }
};
