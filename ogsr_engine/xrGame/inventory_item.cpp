////////////////////////////////////////////////////////////////////////////
//	Module 		: inventory_item.cpp
//	Created 	: 24.03.2003
//  Modified 	: 29.01.2004
//	Author		: Victor Reutsky, Yuri Dobronravin
//	Description : Inventory item
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "inventory_item.h"
#include "inventory_item_impl.h"
#include "inventory.h"
#include "Physics.h"
#include "xrserver_objects_alife.h"
#include "xrserver_objects_alife_items.h"
#include "entity_alive.h"
#include "Level.h"
#include "game_cl_base.h"
#include "Actor.h"
#include "string_table.h"
#include "../Include/xrRender/Kinematics.h"
#include "ai_object_location.h"
#include "object_broker.h"
#include "..\xr_3da\IGame_Persistent.h"
#include "alife_registry_wrappers.h"
#include "alife_simulator_header.h"
#include "grenade.h"

#ifdef DEBUG
#include "debug_renderer.h"
#endif

CInventoryItem::CInventoryItem()
{
    SetSlot(NO_ACTIVE_SLOT);
    m_flags.zero();
    m_flags.set(Fbelt, FALSE);
    m_flags.set(Fruck, TRUE);
    m_flags.set(FRuckDefault, TRUE);
    m_pCurrentInventory = NULL;

    SetDropManual(FALSE);

    m_flags.set(FCanTake, TRUE);
    m_flags.set(FCanTrade, TRUE);
    m_flags.set(FUsingCondition, FALSE);
    m_fCondition = 1.0f;

    m_name = m_nameShort = NULL;

    m_eItemPlace = eItemPlaceUndefined;
    m_Description = "";
    m_cell_item = NULL;

    m_fPsyHealthRestoreSpeed = 0.f;
    m_fRadiationRestoreSpeed = 0.f;

    loaded_belt_index = (u8)(-1);
    m_highlight_equipped = false;
    m_always_ungroupable = false;
}

CInventoryItem::~CInventoryItem()
{
    ASSERT_FMT((int)m_slots.size() >= 0, "m_slots.size() returned negative value inside destructor!"); // alpet: для детекта повреждения объекта

    bool B_GOOD = (!m_pCurrentInventory || (std::find(m_pCurrentInventory->m_all.begin(), m_pCurrentInventory->m_all.end(), this) == m_pCurrentInventory->m_all.end()));
    if (!B_GOOD)
    {
        CObject* p = object().H_Parent();
        Msg("inventory ptr is [%s]", m_pCurrentInventory ? "not-null" : "null");
        if (p)
            Msg("parent name is [%s]", p->cName().c_str());

        Msg("! ERROR item_id[%d] H_Parent=[%s][%d] [%d]", object().ID(), p ? p->cName().c_str() : "none", p ? p->ID() : -1, Device.dwFrame);
    }
}

void CInventoryItem::Load(LPCSTR section)
{
    CHitImmunity::LoadImmunities(pSettings->r_string(section, "immunities_sect"), pSettings);
    m_icon_params.Load(section);

    ISpatial* self = smart_cast<ISpatial*>(this);
    if (self)
        self->spatial.type |= STYPE_VISIBLEFORAI;

    m_name = CStringTable().translate(pSettings->r_string(section, "inv_name"));
    m_nameShort = CStringTable().translate(pSettings->r_string(section, "inv_name_short"));

    //.	NameComplex			();
    m_weight = pSettings->r_float(section, "inv_weight");
    R_ASSERT(m_weight >= 0.f);

    m_cost = pSettings->r_u32(section, "cost");

    m_slots_sect = READ_IF_EXISTS(pSettings, r_string, section, "slot", "");
    {
        char buf[16];
        const int count = _GetItemCount(m_slots_sect);
        if (count)
            m_slots.clear(); // full override!
        for (int i = 0; i < count; ++i)
        {
            u8 slot = u8(atoi(_GetItem(m_slots_sect, i, buf)));
            // вместо std::find(m_slots.begin(), m_slots.end(), slot) == m_slots.end() используется !IsPlaceable
            if (slot < SLOTS_TOTAL && !IsPlaceable(slot, slot))
                m_slots.push_back(slot);
        }
        if (count)
            SetSlot(m_slots[0]);
    }

    if (Core.Features.test(xrCore::Feature::forcibly_equivalent_slots))
    {
        // В OGSR, первый и второй оружейные слоты принудительно
        // равнозначны. Что бы сохранить совместимость с этим, если
        // для предмета указан только один слот и это оружейный слот,
        // добавим к нему соотв. второй оружейный слот.
        if (GetSlotsCount() == 1)
        {
            if (m_slots[0] == FIRST_WEAPON_SLOT)
                m_slots.push_back(SECOND_WEAPON_SLOT);
            else if (m_slots[0] == SECOND_WEAPON_SLOT)
                m_slots.push_back(FIRST_WEAPON_SLOT);
        }
    }

    // Description
    if (pSettings->line_exist(section, "description"))
        m_Description = CStringTable().translate(pSettings->r_string(section, "description"));

    m_flags.set(Fbelt, READ_IF_EXISTS(pSettings, r_bool, section, "belt", FALSE));
    m_flags.set(FRuckDefault, READ_IF_EXISTS(pSettings, r_bool, section, "default_to_ruck", TRUE));
    m_flags.set(FCanTake, READ_IF_EXISTS(pSettings, r_bool, section, "can_take", TRUE));
    m_flags.set(FCanTrade, READ_IF_EXISTS(pSettings, r_bool, section, "can_trade", TRUE));
    m_flags.set(FIsQuestItem, READ_IF_EXISTS(pSettings, r_bool, section, "quest_item", FALSE));

    m_flags.set(FAllowSprint, READ_IF_EXISTS(pSettings, r_bool, section, "sprint_allowed", TRUE));
    m_fControlInertionFactor = READ_IF_EXISTS(pSettings, r_float, section, "control_inertion_factor", 1.0f);
    m_icon_name = READ_IF_EXISTS(pSettings, r_string, section, "icon_name", NULL);

    m_fPsyHealthRestoreSpeed = READ_IF_EXISTS(pSettings, r_float, section, "psy_health_restore_speed", 0.f);
    m_fRadiationRestoreSpeed = READ_IF_EXISTS(pSettings, r_float, section, "radiation_restore_speed", 0.f);
    m_always_ungroupable = READ_IF_EXISTS(pSettings, r_bool, section, "always_ungroupable", false);

    m_need_brief_info = READ_IF_EXISTS(pSettings, r_bool, section, "show_brief_info", true);
}

void CInventoryItem::ChangeCondition(float fDeltaCondition)
{
    m_fCondition += fDeltaCondition;
    clamp(m_fCondition, 0.f, 1.f);
    auto se_obj = object().alife_object();
    if (se_obj)
    {
        CSE_ALifeInventoryItem* itm = smart_cast<CSE_ALifeInventoryItem*>(se_obj);
        if (itm)
            itm->m_fCondition = m_fCondition;
    }
}

void CInventoryItem::SetSlot(u8 slot)
{
    if (GetSlotsCount() == 0 && slot < (u8)NO_ACTIVE_SLOT)
        m_slots.push_back(slot); // in-constructor initialization

    for (u32 i = 0; i < GetSlotsCount(); i++)
        if (m_slots[i] == slot)
        {
            selected_slot = u8(i);
            return;
        }

    if (slot >= (u8)NO_ACTIVE_SLOT) // u8 used for code compatibility
        selected_slot = NO_ACTIVE_SLOT;
    else
    {
        Msg("!#ERROR: slot %d not acceptable for object %s (%s) with slots {%s}", slot, object().Name_script(), Name(), m_slots_sect);
        return;
    }
}

u8 CInventoryItem::GetSlot() const
{
    if (GetSlotsCount() < 1 || selected_slot >= GetSlotsCount())
    {
        return NO_ACTIVE_SLOT;
    }
    else
        return (u8)m_slots[selected_slot];
}

bool CInventoryItem::IsPlaceable(u8 min_slot, u8 max_slot)
{
    for (u32 i = 0; i < GetSlotsCount(); i++)
    {
        u8 s = m_slots[i];
        if (min_slot <= s && s <= max_slot)
            return true;
    }
    return false;
}

void CInventoryItem::Hit(SHit* pHDS)
{
    if (!m_flags.test(FUsingCondition))
        return;

    float hit_power = pHDS->damage();
    hit_power *= m_HitTypeK[pHDS->hit_type];

    ChangeCondition(-hit_power);
}

const char* CInventoryItem::Name() { return *m_name; }

const char* CInventoryItem::NameShort() { return *m_nameShort; }

bool CInventoryItem::Useful() const { return CanTake(); }

bool CInventoryItem::Activate(bool now) { return false; }

void CInventoryItem::Deactivate(bool now) {}

void CInventoryItem::OnH_B_Independent(bool just_before_destroy)
{
    UpdateXForm();
    m_eItemPlace = eItemPlaceUndefined;
}

void CInventoryItem::OnH_A_Independent()
{
    m_eItemPlace = eItemPlaceUndefined;
    inherited::OnH_A_Independent();
}

void CInventoryItem::OnH_B_Chield() {}

void CInventoryItem::OnH_A_Chield() { inherited::OnH_A_Chield(); }
#ifdef DEBUG
extern Flags32 dbg_net_Draw_Flags;
#endif

void CInventoryItem::UpdateCL()
{
#ifdef DEBUG
    if (bDebug)
    {
        if (dbg_net_Draw_Flags.test(1 << 4))
        {
            Device.seqRender.Remove(this);
            Device.seqRender.Add(this);
        }
        else
        {
            Device.seqRender.Remove(this);
        }
    }

#endif
}

void CInventoryItem::OnEvent(NET_Packet& P, u16 type)
{
    if (type == GE_CHANGE_POS) {
        Fvector p;
        P.r_vec3(p);
        CPHSynchronize* pSyncObj = NULL;
        pSyncObj = object().PHGetSyncItem(0);
        if (!pSyncObj)
            return;
        SPHNetState state;
        pSyncObj->get_State(state);
        state.position = p;
        state.previous_position = p;
        pSyncObj->set_State(state);
    }
}

//процесс отсоединения вещи заключается в спауне новой вещи
//в инвентаре и установке соответствующих флагов в родительском
//объекте, поэтому функция должна быть переопределена
bool CInventoryItem::Detach(const char* item_section_name, bool b_spawn_item)
{
    if (b_spawn_item)
    {
        CSE_Abstract* D = F_entity_Create(item_section_name);
        R_ASSERT(D);
        CSE_ALifeDynamicObject* l_tpALifeDynamicObject = smart_cast<CSE_ALifeDynamicObject*>(D);
        R_ASSERT(l_tpALifeDynamicObject);

        l_tpALifeDynamicObject->m_tNodeID = object().ai_location().level_vertex_id();

        // Fill
        D->s_name = item_section_name;
        D->set_name_replace("");
        D->s_gameid = u8(GameID());
        D->s_RP = 0xff;
        D->ID = 0xffff;
        D->ID_Parent = object().H_Parent()->ID();
        D->ID_Phantom = 0xffff;
        D->o_Position = object().Position();
        D->s_flags.assign(M_SPAWN_OBJECT_LOCAL);
        D->RespawnTime = 0;
        // Send
        NET_Packet P;
        D->Spawn_Write(P, TRUE);
        Level().Send(P, net_flags(TRUE));
        // Destroy
        F_entity_Destroy(D);
    }
    return true;
}

/////////// network ///////////////////////////////
BOOL CInventoryItem::net_Spawn(CSE_Abstract* DC)
{
    m_flags.set(FInInterpolation, FALSE);
    m_flags.set(FInInterpolate, FALSE);

    m_flags.set(Fuseful_for_NPC, TRUE);
    CSE_Abstract* e = (CSE_Abstract*)(DC);
    CSE_ALifeObject* alife_object = smart_cast<CSE_ALifeObject*>(e);
    if (alife_object)
    {
        m_flags.set(Fuseful_for_NPC, alife_object->m_flags.test(CSE_ALifeObject::flUsefulForAI));
    }

    auto se_obj = object().alife_object();
    if (se_obj)
    {
        CSE_ALifeInventoryItem* itm = smart_cast<CSE_ALifeInventoryItem*>(se_obj);
        if (itm)
        {
            m_fCondition = itm->m_fCondition;
        }
    }

    CSE_ALifeInventoryItem* pSE_InventoryItem = smart_cast<CSE_ALifeInventoryItem*>(e);
    if (!pSE_InventoryItem)
        return TRUE;

    return TRUE;
}

void CInventoryItem::net_Destroy()
{
    //инвентарь которому мы принадлежали
    //.	m_pCurrentInventory = NULL;
}

void CInventoryItem::save(NET_Packet& packet)
{
    if (m_eItemPlace == eItemPlaceBelt && smart_cast<CActor*>(object().H_Parent()))
    {
        packet.w_u8((u8)eItemPlaceBeltActor);
        packet.w_u8((u8)m_pCurrentInventory->GetIndexOnBelt(this));
    }
    else
        packet.w_u8((u8)m_eItemPlace);
    packet.w_float(m_fCondition);
    if (m_eItemPlace == eItemPlaceSlot)
        packet.w_u8((u8)GetSlot());

    if (object().H_Parent())
    {
        packet.w_u8(0);
        return;
    }

    u8 _num_items = (u8)object().PHGetSyncItemsNumber();
    packet.w_u8(_num_items);
    object().PHSaveState(packet);
}

void CInventoryItem::net_Export(CSE_Abstract* E)
{
    CSE_ALifeInventoryItem* item = smart_cast<CSE_ALifeInventoryItem*>(E);
    item->m_u8NumItems = 0;
};

void CInventoryItem::load(IReader& packet)
{
    m_eItemPlace = (EItemPlace)packet.r_u8();
    if (m_eItemPlace == eItemPlaceBeltActor)
    {
        if (Belt())
            SetLoadedBeltIndex(packet.r_u8());
        else
        {
            packet.r_u8();
            Msg("! [%s]: move %s from belt, because belt = false", __FUNCTION__, object().cName().c_str());
            m_eItemPlace = eItemPlaceRuck;
        }
    }
    m_fCondition = packet.r_float();
    if (m_eItemPlace == eItemPlaceSlot)
        if (ai().get_alife()->header().version() < 4)
        {
            auto slots = GetSlots();
            SetSlot(slots.size() ? slots[0] : NO_ACTIVE_SLOT);
        }
        else
            SetSlot(packet.r_u8());

    u8 tmp = packet.r_u8();
    if (!tmp)
        return;

    if (!object().PPhysicsShell())
    {
        object().setup_physic_shell();
        object().PPhysicsShell()->Disable();
    }

    object().PHLoadState(packet);
    object().PPhysicsShell()->Disable();
}

void CInventoryItem::reload(LPCSTR section)
{
    inherited::reload(section);
    m_holder_range_modifier = READ_IF_EXISTS(pSettings, r_float, section, "holder_range_modifier", 1.f);
    m_holder_fov_modifier = READ_IF_EXISTS(pSettings, r_float, section, "holder_fov_modifier", 1.f);
}

void CInventoryItem::reinit()
{
    m_pCurrentInventory = NULL;
    m_eItemPlace = eItemPlaceUndefined;
}

bool CInventoryItem::can_kill() const { return (false); }

CInventoryItem* CInventoryItem::can_kill(CInventory* inventory) const { return (0); }

const CInventoryItem* CInventoryItem::can_kill(const xr_vector<const CGameObject*>& items) const { return (0); }

CInventoryItem* CInventoryItem::can_make_killing(const CInventory* inventory) const { return (0); }

bool CInventoryItem::ready_to_kill() const { return (false); }

void CInventoryItem::activate_physic_shell()
{
    CEntityAlive* E = smart_cast<CEntityAlive*>(object().H_Parent());
    if (!E)
    {
        on_activate_physic_shell();
        return;
    };

    UpdateXForm();

    object().CPhysicsShellHolder::activate_physic_shell();
}

void CInventoryItem::UpdateXForm()
{
    if (0 == object().H_Parent())
        return;

    // Get access to entity and its visual
    CEntityAlive* E = smart_cast<CEntityAlive*>(object().H_Parent());
    if (!E)
        return;

    if (E->cast_base_monster())
        return;

    const CInventoryOwner* parent = smart_cast<const CInventoryOwner*>(E);
    if (parent && parent->use_simplified_visual())
        return;

    if (parent->attached(this))
        return;

    R_ASSERT(E);
    IKinematics* V = smart_cast<IKinematics*>(E->Visual());
    VERIFY(V);

    // Get matrices
    int boneL, boneR, boneR2;
    E->g_WeaponBones(boneL, boneR, boneR2);
    //	if ((HandDependence() == hd1Hand) || (STATE == eReload) || (!E->g_Alive()))
    //		boneL = boneR2;
#pragma todo("TO ALL: serious performance problem")
    V->CalculateBones();
    Fmatrix& mL = V->LL_GetTransform(u16(boneL));
    Fmatrix& mR = V->LL_GetTransform(u16(boneR));
    // Calculate
    Fmatrix mRes;
    Fvector R, D, N;
    D.sub(mL.c, mR.c);
    D.normalize_safe();

    if (fis_zero(D.magnitude()))
    {
        mRes.set(E->XFORM());
        mRes.c.set(mR.c);
    }
    else
    {
        D.normalize();
        R.crossproduct(mR.j, D);

        N.crossproduct(D, R);
        N.normalize();

        mRes.set(R, N, D, mR.c);
        mRes.mulA_43(E->XFORM());
    }

    //	UpdatePosition	(mRes);
    object().Position().set(mRes.c);
}

#ifdef DEBUG

void CInventoryItem::OnRender()
{
    if (bDebug && object().Visual())
    {
        if (!(dbg_net_Draw_Flags.is_any((1 << 4))))
            return;

        Fvector bc, bd;
        object().Visual()->getVisData().box.get_CD(bc, bd);
        Fmatrix M = object().XFORM();
        M.c.add(bc);
        Level().debug_renderer().draw_obb(M, bd, color_rgba(0, 0, 255, 255));
    };
}
#endif

DLL_Pure* CInventoryItem::_construct()
{
    m_object = smart_cast<CPhysicsShellHolder*>(this);
    VERIFY(m_object);
    return (inherited::_construct());
}

void CInventoryItem::modify_holder_params(float& range, float& fov) const
{
    range *= m_holder_range_modifier;
    fov *= m_holder_fov_modifier;
}

bool CInventoryItem::CanTrade() const
{
    bool res = true;
#pragma todo("Dima to Andy : why CInventoryItem::CanTrade can be called for the item, which doesn't have owner?")
    if (m_pCurrentInventory)
        res = inventory_owner().AllowItemToTrade(this, m_eItemPlace);

    return (res && m_flags.test(FCanTrade) && !IsQuestItem());
}

int CInventoryItem::GetGridWidth() const { return (int)m_icon_params.grid_width; }

int CInventoryItem::GetGridHeight() const { return (int)m_icon_params.grid_height; }

int CInventoryItem::GetIconIndex() const { return m_icon_params.icon_group; }

int CInventoryItem::GetXPos() const { return (int)m_icon_params.grid_x; }
int CInventoryItem::GetYPos() const { return (int)m_icon_params.grid_y; }

bool CInventoryItem::IsNecessaryItem(CInventoryItem* item) { return IsNecessaryItem(item->object().cNameSect()); };

BOOL CInventoryItem::IsInvalid() const { return object().getDestroy() || GetDropManual(); }

bool CInventoryItem::GetInvShowCondition() const { return m_icon_params.show_condition; }

void CInventoryItem::SetLoadedBeltIndex(u8 pos)
{
    loaded_belt_index = pos;
    m_eItemPlace = eItemPlaceBelt;
}

void CInventoryItem::OnMoveToSlot()
{
    if (smart_cast<CActor*>(object().H_Parent()) /* && !smart_cast<CGrenade*>( this )*/)
    {
        if (Core.Features.test(xrCore::Feature::equipped_untradable))
        {
            m_flags.set(FIAlwaysUntradable, TRUE);
            m_flags.set(FIUngroupable, TRUE);
            if (Core.Features.test(xrCore::Feature::highlight_equipped))
                m_highlight_equipped = true;
        }
        else if (Core.Features.test(xrCore::Feature::highlight_equipped))
        {
            m_flags.set(FIUngroupable, TRUE);
            m_highlight_equipped = true;
        }
    }
};

void CInventoryItem::OnMoveToBelt()
{
    if (smart_cast<CActor*>(object().H_Parent()))
    {
        if (Core.Features.test(xrCore::Feature::equipped_untradable))
        {
            m_flags.set(FIAlwaysUntradable, TRUE);
            m_flags.set(FIUngroupable, TRUE);
            if (Core.Features.test(xrCore::Feature::highlight_equipped))
                m_highlight_equipped = true;
        }
        else if (Core.Features.test(xrCore::Feature::highlight_equipped))
        {
            m_flags.set(FIUngroupable, TRUE);
            m_highlight_equipped = true;
        }
    }
};

void CInventoryItem::OnMoveToRuck(EItemPlace prevPlace)
{
    if (smart_cast<CActor*>(object().H_Parent()))
    {
        if (Core.Features.test(xrCore::Feature::equipped_untradable))
        {
            m_flags.set(FIAlwaysUntradable, FALSE);
            m_flags.set(FIUngroupable, FALSE);
            if (Core.Features.test(xrCore::Feature::highlight_equipped))
                m_highlight_equipped = false;
        }
        else if (Core.Features.test(xrCore::Feature::highlight_equipped))
        {
            m_flags.set(FIUngroupable, FALSE);
            m_highlight_equipped = false;
        }
    }
};
