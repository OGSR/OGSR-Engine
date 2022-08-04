#include "stdafx.h"
#include "pda.h"
#include "hudmanager.h"
#include "PhysicsShell.h"
#include "Entity.h"
#include "actor.h"

#include "xrserver.h"
#include "xrServer_Objects_ALife_Items.h"
#include "level.h"

#include "specific_character.h"
#include "alife_registry_wrappers.h"
#include "UIGameSP.h"
#include "ui/UIPDAWnd.h"
#include "player_hud.h"
#include "ai_sounds.h"
#include "inventory.h"

static CUIPdaWnd* GetPdaWindow()
{
    if (auto pGameSP = smart_cast<CUIGameSP*>(HUD().GetUI()->UIGame()))
        return pGameSP->PdaMenu;
    else
        return nullptr;
}

CPda::CPda()
{
    SetSlot(PDA_SLOT);
    m_flags.set(Fruck, TRUE);

    m_idOriginalOwner = u16(-1);
    m_SpecificChracterOwner = nullptr;
    TurnOff();
}

BOOL CPda::net_Spawn(CSE_Abstract* DC)
{
    inherited::net_Spawn(DC);
    auto pda = smart_cast<CSE_ALifeItemPDA*>(DC);
    R_ASSERT(pda);
    m_idOriginalOwner = pda->m_original_owner;
    m_SpecificChracterOwner = pda->m_specific_character;

    return TRUE;
}

void CPda::net_Destroy()
{
    if (this_is_3d_pda)
        inherited::net_Destroy();
    else
        CInventoryItemObject::net_Destroy();

    TurnOff();
    feel_touch.clear();
}

void CPda::Load(LPCSTR section)
{
    this_is_3d_pda = pSettings->line_exist(section, "hud");
    if (this_is_3d_pda)
        inherited::Load(section);
    else
        CInventoryItemObject::Load(section);

    m_fRadius = pSettings->r_float(section, "radius");

    if (!this_is_3d_pda)
        return;

    m_joystick_bone = READ_IF_EXISTS(pSettings, r_string, section, "joystick_bone", nullptr);
    HUD_SOUND::LoadSound(section, "snd_draw", sndShow, SOUND_TYPE_ITEM_TAKING);
    HUD_SOUND::LoadSound(section, "snd_holster", sndHide, SOUND_TYPE_ITEM_HIDING);

    HUD_SOUND::LoadSound(section, "snd_btn_press", sndBtnPress, SOUND_TYPE_ITEM_USING);
    HUD_SOUND::LoadSound(section, "snd_btn_release", sndBtnRelease, SOUND_TYPE_ITEM_USING);

    m_thumb_rot[0] = READ_IF_EXISTS(pSettings, r_float, section, "thumb_rot_x", 0.f);
    m_thumb_rot[1] = READ_IF_EXISTS(pSettings, r_float, section, "thumb_rot_y", 0.f);

    m_fZoomRotateTime = READ_IF_EXISTS(pSettings, r_float, hud_sect, "zoom_rotate_time", 0.25f);
}

void CPda::shedule_Update(u32 dt)
{
    inherited::shedule_Update(dt);

    if (!H_Parent())
        return;
    Position().set(H_Parent()->Position());

    if (IsOn() && Level().CurrentEntity() && Level().CurrentEntity()->ID() == H_Parent()->ID())
    {
        CEntityAlive* EA = smart_cast<CEntityAlive*>(H_Parent());
        if (!EA || !EA->g_Alive())
        {
            TurnOff();
            return;
        }

        m_changed = false;
        feel_touch_update(Position(), m_fRadius);
        UpdateActiveContacts();

        if (m_changed)
        {
            if (HUD().GetUI())
            {
                CUIGameSP* pGameSP = smart_cast<CUIGameSP*>(HUD().GetUI()->UIGame());
                if (pGameSP)
                    pGameSP->PdaMenu->PdaContentsChanged(pda_section::contacts);
            }
            m_changed = false;
        }
    }
}

void CPda::UpdateActiveContacts()
{
    if (!m_changed)
    {
        for (auto& it : m_active_contacts)
        {
            CEntityAlive* pEA = smart_cast<CEntityAlive*>(it);
            if (!pEA->g_Alive())
            {
                m_changed = true;
                break;
            }
        }
    }

    m_active_contacts.clear();
    auto it = feel_touch.begin();
    for (; it != feel_touch.end(); ++it)
    {
        CEntityAlive* pEA = smart_cast<CEntityAlive*>(*it);
        if (!!pEA->g_Alive())
            m_active_contacts.push_back(*it);
    }
}

void CPda::feel_touch_new(CObject* O)
{
    CInventoryOwner* pNewContactInvOwner = smart_cast<CInventoryOwner*>(O);
    CInventoryOwner* pOwner = smart_cast<CInventoryOwner*>(H_Parent());
    VERIFY(pOwner);

    pOwner->NewPdaContact(pNewContactInvOwner);
    m_changed = true;
}

void CPda::feel_touch_delete(CObject* O)
{
    if (!H_Parent())
        return;
    CInventoryOwner* pLostContactInvOwner = smart_cast<CInventoryOwner*>(O);
    CInventoryOwner* pOwner = smart_cast<CInventoryOwner*>(H_Parent());
    VERIFY(pOwner);

    pOwner->LostPdaContact(pLostContactInvOwner);
    m_changed = true;
}

BOOL CPda::feel_touch_contact(CObject* O)
{
    CInventoryOwner* pInvOwner = smart_cast<CInventoryOwner*>(O);
    if (pInvOwner)
    {
        if (this != pInvOwner->GetPDA())
        {
            CEntityAlive* pEntityAlive = smart_cast<CEntityAlive*>(O);
            if (pEntityAlive && !pEntityAlive->cast_base_monster())
                return TRUE;
        }
        else
            return FALSE;
    }

    return FALSE;
}

void CPda::OnH_A_Chield()
{
    VERIFY(IsOff());

    //включить PDA только если оно находится у первого владельца
    if (H_Parent()->ID() == m_idOriginalOwner)
    {
        TurnOn();
        if (m_sFullName.empty())
        {
            m_sFullName.assign(inherited::Name());
            m_sFullName += " ";
            m_sFullName += (smart_cast<CInventoryOwner*>(H_Parent()))->Name();
        }
    }
    if (this_is_3d_pda)
        inherited::OnH_A_Chield();
    else
        CInventoryItemObject::OnH_A_Chield();
}

void CPda::OnH_B_Independent(bool just_before_destroy)
{
    if (this_is_3d_pda)
        inherited::OnH_B_Independent(just_before_destroy);
    else
        CInventoryItemObject::OnH_B_Independent(just_before_destroy);

    //выключить
    TurnOff();

    if (!this_is_3d_pda || !smart_cast<CActor*>(H_Parent()))
        return;

    HUD_SOUND::PlaySound(sndHide, Position(), H_Root(), !!GetHUDmode(), false, false);

    SwitchState(eHidden);
    SetPending(FALSE);
    m_bZoomed = false;

    CUIPdaWnd* pda = GetPdaWindow();
    // KRodin: TODO: А тут надо скрывать окно пда?

    g_player_hud->reset_thumb(true);
    pda->ResetJoystick(true);

    if (joystick != BI_NONE && HudItemData())
        HudItemData()->m_model->LL_GetBoneInstance(joystick).reset_callback();

    g_player_hud->detach_item(this);
}

CInventoryOwner* CPda::GetOriginalOwner() const
{
    CObject* pObject = Level().Objects.net_Find(GetOriginalOwnerID());
    CInventoryOwner* pInvOwner = smart_cast<CInventoryOwner*>(pObject);

    return pInvOwner;
}

xr_map<u16, CPda*> CPda::ActivePDAContacts()
{
    xr_map<u16, CPda*> res;

    for (auto* Obj : m_active_contacts)
        if (CPda* p = GetPdaFromOwner(Obj))
            res.emplace(Obj->ID(), p);

    return res;
}

void CPda::save(NET_Packet& output_packet)
{
    inherited::save(output_packet);
    save_data(m_sFullName, output_packet);
}

void CPda::load(IReader& input_packet)
{
    inherited::load(input_packet);
    load_data(m_sFullName, input_packet);
}

LPCSTR CPda::Name()
{
    if (!m_SpecificChracterOwner.size())
        return inherited::Name();

    if (m_sFullName.empty())
    {
        m_sFullName.assign(inherited::Name());

        CSpecificCharacter spec_char;
        spec_char.Load(m_SpecificChracterOwner);
        m_sFullName += " ";
        m_sFullName += xr_string(spec_char.Name());
    }

    return m_sFullName.c_str();
}

CPda* CPda::GetPdaFromOwner(CObject* owner) { return smart_cast<CInventoryOwner*>(owner)->GetPDA(); }

void CPda::TurnOn()
{
    m_bTurnedOff = false;
    m_changed = true;
}

void CPda::TurnOff()
{
    m_bTurnedOff = true;
    m_active_contacts.clear();
}

void CPda::net_Relcase(CObject* O)
{
    inherited::net_Relcase(O);
    if (m_active_contacts.size() && !Level().is_removing_objects())
    {
        const auto I = std::find(m_active_contacts.begin(), m_active_contacts.end(), O);
        if (I != m_active_contacts.end())
            m_active_contacts.erase(I);
    }
}

void CPda::OnStateSwitch(u32 S, u32 oldState)
{
    if (!this_is_3d_pda)
        return;

    inherited::OnStateSwitch(S, oldState);

    if (!smart_cast<CActor*>(H_Parent()))
        return;

    switch (S)
    {
    case eShowing: {
        g_player_hud->attach_item(this);

        HUD_SOUND::PlaySound(sndShow, Position(), H_Root(), !!GetHUDmode(), false, false);

        PlayHUDMotion("anm_show", false, GetState());

        SetPending(TRUE);
    }
    break;
    case eHiding: {
        if (oldState != eHiding)
        {
            HUD_SOUND::PlaySound(sndHide, Position(), H_Root(), !!GetHUDmode(), false, false);
            PlayHUDMotion("anm_hide", true, GetState());
            SetPending(TRUE);
            m_bZoomed = false;
            auto pdawnd = GetPdaWindow();
            g_player_hud->reset_thumb(false);
            if (pdawnd)
                pdawnd->ResetJoystick(false);
            if (joystick != BI_NONE && HudItemData())
                HudItemData()->m_model->LL_GetBoneInstance(joystick).reset_callback();
        }
    }
    break;
    case eHidden: {
        if (oldState != eHidden)
        {
            m_bZoomed = false;
            CUIPdaWnd* pda = GetPdaWindow();
            // KRodin: TODO: А тут надо скрывать окно пда?
            g_player_hud->reset_thumb(true);
            pda->ResetJoystick(true);
        }
        SetPending(FALSE);
    }
    break;
    case eIdle: {
        PlayAnimIdle();

        if (m_joystick_bone && joystick == BI_NONE && HudItemData())
            joystick = HudItemData()->m_model->LL_BoneID(m_joystick_bone);

        if (joystick != BI_NONE && HudItemData())
        {
            CBoneInstance* bi = &HudItemData()->m_model->LL_GetBoneInstance(joystick);
            if (bi)
                bi->set_callback(bctCustom, JoystickCallback, this);
        }
    }
    break;
    }
}

void CPda::OnAnimationEnd(u32 state)
{
    if (!this_is_3d_pda)
        return;

    switch (state)
    {
    case eShowing: {
        SetPending(FALSE);
        SwitchState(eIdle);
    }
    break;
    case eHiding: {
        SetPending(FALSE);
        SwitchState(eHidden);
        g_player_hud->detach_item(this);
    }
    break;
    default: inherited::OnAnimationEnd(state);
    }
}

// inertion
constexpr float _inertion(const float _val_cur, const float _val_trgt, const float _friction)
{
    const float friction_i = 1.f - _friction;
    return _val_cur * _friction + _val_trgt * friction_i;
}

void CPda::JoystickCallback(CBoneInstance* B)
{
    CPda* Pda = static_cast<CPda*>(B->callback_param());
    CUIPdaWnd* pda = GetPdaWindow();

    static float fAvgTimeDelta = Device.fTimeDelta;
    fAvgTimeDelta = _inertion(fAvgTimeDelta, Device.fTimeDelta, 0.8f);

    Fvector& target = pda->target_joystickrot;
    Fvector& current = pda->joystickrot;
    float& target_press = pda->target_buttonpress;
    float& press = pda->buttonpress;

    if (!target.similar(current, .0001f))
    {
        Fvector diff;
        diff = target;
        diff.sub(current);
        diff.mul(fAvgTimeDelta / .1f);
        current.add(diff);
    }
    else
        current.set(target);

    if (!fsimilar(target_press, press, .0001f))
    {
        float prev_press = press;

        float diff = target_press;
        diff -= press;
        diff *= (fAvgTimeDelta / .1f);
        press += diff;

        if (prev_press == 0.f && press < 0.f)
            HUD_SOUND::PlaySound(Pda->sndBtnPress, B->mTransform.c, Pda->H_Root(), !!Pda->GetHUDmode(), false, false);
        else if (prev_press < -.001f && press >= -.001f)
            HUD_SOUND::PlaySound(Pda->sndBtnRelease, B->mTransform.c, Pda->H_Root(), !!Pda->GetHUDmode(), false, false);
    }
    else
        press = target_press;

    Fmatrix rotation;
    rotation.identity();
    rotation.rotateX(current.x);

    Fmatrix rotation_y;
    rotation_y.identity();
    rotation_y.rotateY(current.y);
    rotation.mulA_43(rotation_y);

    rotation_y.identity();
    rotation_y.rotateZ(current.z);
    rotation.mulA_43(rotation_y);

    rotation.translate_over(0.f, press, 0.f);

    B->mTransform.mulB_43(rotation);
}

void CPda::OnMoveToRuck(EItemPlace prevPlace)
{
    inherited::OnMoveToRuck(prevPlace);

    if (!this_is_3d_pda || !smart_cast<CActor*>(H_Parent()))
        return;

    if (prevPlace == eItemPlaceSlot)
    {
        SwitchState(eHidden);
        if (joystick != BI_NONE && HudItemData())
            HudItemData()->m_model->LL_GetBoneInstance(joystick).reset_callback();
        g_player_hud->detach_item(this);
    }

    // KRodin: TODO: У нас нету слота пда, поэтому наверно не нужно тут делать скрытие UI. Сделать, если будет надо.

    StopCurrentAnimWithoutCallback();
    SetPending(FALSE);
}

void CPda::UpdateCL()
{
    if (this_is_3d_pda)
        inherited::UpdateCL();
    else
        CInventoryItemObject::UpdateCL();
}

void CPda::UpdateXForm() { CInventoryItem::UpdateXForm(); }

void CPda::OnActiveItem()
{
    if (!this_is_3d_pda || !smart_cast<CActor*>(H_Parent()))
        return;

    SwitchState(eShowing);

    if (!psActorFlags.test(AF_3D_PDA))
        return;

    auto pGameSP = smart_cast<CUIGameSP*>(HUD().GetUI()->UIGame());
    pGameSP->ShowHidePda(true);

    m_fZoomRotationFactor = 0.f;
}

void CPda::OnHiddenItem()
{
    if (!this_is_3d_pda || !smart_cast<CActor*>(H_Parent()))
        return;

    SwitchState(eHiding);

    if (!psActorFlags.test(AF_3D_PDA))
        return;

    auto pGameSP = smart_cast<CUIGameSP*>(HUD().GetUI()->UIGame());
    pGameSP->ShowHidePda(false);
}

CPda::~CPda()
{
    if (!this_is_3d_pda)
        return;

    HUD_SOUND::DestroySound(sndShow);
    HUD_SOUND::DestroySound(sndHide);
    HUD_SOUND::DestroySound(sndBtnPress);
    HUD_SOUND::DestroySound(sndBtnRelease);
}
