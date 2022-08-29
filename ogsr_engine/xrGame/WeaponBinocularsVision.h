#pragma once
#include "ui\uistatic.h"
class CObject;
class CWeaponBinoculars;

enum
{
    flVisObjNotValid = (1 << 0),
    flTargetLocked = (1 << 1),
};
struct SBinocVisibleObj
{
    SBinocVisibleObj(){};
    CObject* m_object;
    CUIStatic m_lt;
    CUIStatic m_lb;
    CUIStatic m_rt;
    CUIStatic m_rb;
    Frect cur_rect;

    u32 m_visible_time;
    float m_upd_speed;
    Flags8 m_flags;
    void create_default(u32 color);
    void Draw();
    void Update();
};

class CWeaponMagazined;
class CBinocularsVision
{
    xr_vector<std::unique_ptr<SBinocVisibleObj>> m_active_objects;

public:
    CBinocularsVision(CWeaponMagazined* parent);
    ~CBinocularsVision();
    void Update();
    void Draw();
    void remove_links(CObject* object);

protected:
    CWeaponMagazined* m_parent;
    Fcolor m_frame_color;
    float m_rotating_speed;
    void Load(const shared_str& section);
    ref_sound m_snd_found;

    u32 m_min_visible_time;
    float m_transparency_threshold;
};
