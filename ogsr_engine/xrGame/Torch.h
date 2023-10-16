#pragma once

#include "inventory_item_object.h"
#include "hudsound.h"
#include "script_export_space.h"

class CLAItem;
class CMonsterEffector;

class CTorch : public CInventoryItemObject
{
private:
    typedef CInventoryItemObject inherited;

protected:
    float fBrightness;
    CLAItem* lanim;
    float time2hide;

    u16 guid_bone{};
    shared_str light_trace_bone;

    float m_delta_h;
    Fvector2 m_prev_hp;
    bool m_switched_on;
    ref_light light_render;
    ref_light light_omni;
    ref_glow glow_render;
    Fvector m_focus;
    Fcolor m_color;

private:
    bool useVolumetric{}, useVolumetricForActor{};

public:
    CTorch(void);
    virtual ~CTorch(void);

    virtual void Load(LPCSTR section);
    virtual BOOL net_Spawn(CSE_Abstract* DC);
    virtual void net_Destroy();
    virtual void net_Export(CSE_Abstract* E);

    virtual void OnH_A_Chield();
    virtual void OnH_B_Independent(bool just_before_destroy);

    virtual void UpdateCL();

    void Switch();
    void Switch(bool light_on);
    bool torch_active() const;

    virtual bool can_be_attached() const;
    void calc_m_delta_h(float);
    float get_range() const;

public:
    void SwitchNightVision();
    void SwitchNightVision(bool light_on);
    void UpdateSwitchNightVision();

protected:
    bool m_bNightVisionEnabled{};
    bool m_bNightVisionOn{};

    HUD_SOUND m_NightVisionOnSnd;
    HUD_SOUND m_NightVisionOffSnd;
    HUD_SOUND m_NightVisionIdleSnd;
    HUD_SOUND m_NightVisionBrokenSnd;

    enum EStats
    {
        eTorchActive = (1 << 0),
        eNightVisionActive = (1 << 1),
        eAttached = (1 << 2)
    };

public:
    virtual bool use_parent_ai_locations() const { return (!H_Parent()); }
    virtual void create_physic_shell();
    virtual void activate_physic_shell();
    virtual void setup_physic_shell();

    virtual void afterDetach();
    virtual void renderable_Render();

    // alpet: управление светом фонаря
    IRender_Light* GetLight(int target = 0) const;

    void SetAnimation(LPCSTR name);
    void SetBrightness(float brightness);
    void SetColor(const Fcolor& color, int target = 0);
    void SetRGB(float r, float g, float b, int target = 0);
    void SetAngle(float angle, int target = 0);
    void SetRange(float range, int target = 0);
    void SetTexture(LPCSTR texture, int target = 0);
    void SetVirtualSize(float size, int target = 0);

    DECLARE_SCRIPT_REGISTER_FUNCTION
};

add_to_type_list(CTorch)
#undef script_type_list
#define script_type_list save_type_list(CTorch)
