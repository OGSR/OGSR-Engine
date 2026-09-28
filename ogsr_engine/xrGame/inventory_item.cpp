////////////////////////////////////////////////////////////////////////////
//	Module 		: inventory_item.cpp
//	Created 	: 24.03.2003
//  Modified 	: 29.01.2004
//	Author		: Victor Reutsky, Yuri Dobronravin
//	Description : Inventory item
////////////////////////////////////////////////////////////////////////////

// #include "stdafx.h"
#include "stdafx.h"
#include "inventory_item.h"
#include "inventory_item_impl.h"
#include "Inventory.h"
// #include "Physics.h"
#include "PhysicsShellHolder.h"
#include "entity_alive.h"
#include "Level.h"
#include "game_cl_base.h"
#include "Actor.h"
#include "string_table.h"
#include "../Include/xrRender/Kinematics.h"
#include "ai_object_location.h"
#include "object_broker.h"
#include "../xr_3da/IGame_Persistent.h"

#ifdef DEBUG
#include "debug_renderer.h"
#endif

#define ITEM_REMOVE_TIME 30000

net_updateInvData* CInventoryItem::NetSync()
{
    if (!m_net_updateData)
        m_net_updateData = xr_new<net_updateInvData>();
    return m_net_updateData;
}

CInventoryItem::CInventoryItem()
{
    m_net_updateData = nullptr;
    m_flags.set(Fbelt, FALSE);
    m_flags.set(Fruck, TRUE);
    m_flags.set(FRuckDefault, TRUE);
    m_pInventory = nullptr;

    SetDropManual(FALSE);

    m_flags.set(FCanTake, TRUE);
    m_can_trade = TRUE;
    m_flags.set(FCanTrade, m_can_trade);
    m_flags.set(FUsingCondition, FALSE);
    m_fCondition = 1.0f;

    m_name = m_nameShort = nullptr;

    m_ItemCurrPlace.value = 0;
    m_ItemCurrPlace.type = eItemPlaceUndefined;
    m_ItemCurrPlace.base_slot_id = NO_ACTIVE_SLOT;
    m_ItemCurrPlace.slot_id = NO_ACTIVE_SLOT;

    m_Description = "";
    m_section_id = 0;
    m_flags.set(FIsHelperItem, FALSE);
}

CInventoryItem::~CInventoryItem()
{
    delete_data(m_net_updateData);

#ifndef MASTER_GOLD
    bool B_GOOD = (!m_pInventory || (std::find(m_pInventory->m_all.begin(), m_pInventory->m_all.end(), this) == m_pInventory->m_all.end()));
    if (!B_GOOD)
    {
        CObject* p = object().H_Parent();
        Msg("inventory ptr is [%s]", m_pInventory ? "not-null" : "null");
        if (p)
            Msg("parent name is [%s]", p->cName().c_str());

        Msg("! ERROR item_id[%d] H_Parent=[%s][%d] [%d]", object().ID(), p ? p->cName().c_str() : "none", p ? p->ID() : -1, Device.dwFrame);
    }
#endif // #ifndef MASTER_GOLD
}

void CInventoryItem::Load(LPCSTR section)
{
    CHitImmunity::LoadImmunities(pSettings->r_string(section, "immunities_sect"), pSettings);

    ISpatial* self = smart_cast<ISpatial*>(this);
    if (self)
        self->spatial.type |= STYPE_VISIBLEFORAI;

    m_section_id._set(section);
    m_name = CStringTable().translate(pSettings->r_string(section, "inv_name"));
    m_nameShort = CStringTable().translate(pSettings->r_string(section, "inv_name_short"));

    m_weight = pSettings->r_float(section, "inv_weight");
    R_ASSERT(m_weight >= 0.f);

    m_cost = pSettings->r_u32(section, "cost");
    u32 sl = pSettings->r_u32(section, "slot");
    m_ItemCurrPlace.base_slot_id = (sl == -1) ? 0 : (sl + 1);

    m_Description = CStringTable().translate(pSettings->r_string(section, "description"));

    m_flags.set(Fbelt, READ_IF_EXISTS(pSettings, r_bool, section, "belt", FALSE));
    m_can_trade = READ_IF_EXISTS(pSettings, r_bool, section, "can_trade", TRUE);
    m_flags.set(FCanTake, READ_IF_EXISTS(pSettings, r_bool, section, "can_take", TRUE));
    m_flags.set(FCanTrade, m_can_trade);
    m_flags.set(FIsQuestItem, READ_IF_EXISTS(pSettings, r_bool, section, "quest_item", FALSE));

    m_flags.set(FUsingCondition, READ_IF_EXISTS(pSettings, r_bool, section, "use_condition", false));

    if (BaseSlot() != NO_ACTIVE_SLOT || Belt())
    {
        m_flags.set(FRuckDefault, pSettings->r_bool(section, "default_to_ruck"));
        m_flags.set(FAllowSprint, pSettings->r_bool(section, "sprint_allowed"));
        m_fControlInertionFactor = pSettings->r_float(section, "control_inertion_factor");
    }

    m_icon_name = READ_IF_EXISTS(pSettings, r_string, section, "icon_name", nullptr);
}

void CInventoryItem::ReloadNames()
{
    m_name = CStringTable().translate(pSettings->r_string(m_object->cNameSect(), "inv_name"));
    m_nameShort = CStringTable().translate(pSettings->r_string(m_object->cNameSect(), "inv_name_short"));

    if (pSettings->line_exist(m_object->cNameSect(), "description"))
        m_Description = CStringTable().translate(pSettings->r_string(m_object->cNameSect(), "description"));
}

void CInventoryItem::ChangeCondition(float fDeltaCondition)
{
    m_fCondition += fDeltaCondition;
    clamp(m_fCondition, 0.f, 1.f);
}

void CInventoryItem::Hit(SHit* pHDS)
{
    if (!IsUsingCondition())
        return;

    float hit_power = pHDS->damage();
    hit_power *= GetHitImmunity(pHDS->hit_type);

    ChangeCondition(-hit_power);
}

LPCSTR CInventoryItem::NameItem() { return m_name.c_str(); }

LPCSTR CInventoryItem::NameShort() { return m_nameShort.c_str(); }

bool CInventoryItem::Useful() const { return CanTake(); }

bool CInventoryItem::ActivateItem() { return false; }

void CInventoryItem::DeactivateItem() {}

void CInventoryItem::OnH_B_Independent(bool just_before_destroy)
{
    UpdateXForm();
    m_ItemCurrPlace.type = eItemPlaceUndefined;
}

void CInventoryItem::OnH_A_Independent()
{
    m_dwItemIndependencyTime = Level().timeServer();
    m_ItemCurrPlace.type = eItemPlaceUndefined;
    inherited::OnH_A_Independent();
}

void CInventoryItem::OnH_B_Chield() { Level().RemoveObject_From_4CrPr(m_object); }

void CInventoryItem::OnH_A_Chield() { inherited::OnH_A_Chield(); }
#ifdef DEBUG
extern Flags32 dbg_net_Draw_Flags;
#endif

void CInventoryItem::UpdateCL()
{
#ifdef DEBUG
    if (bDebug)
    {
        if (dbg_net_Draw_Flags.test(dbg_draw_invitem))
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
    switch (type)
    {
    case GE_ADDON_ATTACH: {
        u16 ItemID;
        P.r_u16(ItemID);
        CInventoryItem* ItemToAttach = smart_cast<CInventoryItem*>(Level().Objects.net_Find(ItemID));
        if (!ItemToAttach)
            break;
        Attach(ItemToAttach, true);
    }
    break;
    case GE_ADDON_DETACH: {
        string64 i_name;
        P.r_stringZ(i_name);
        Detach(i_name, true);
    }
    break;
    case GE_CHANGE_POS: {
        Fvector p;
        P.r_vec3(p);
        CPHSynchronize* pSyncObj = nullptr;
        pSyncObj = object().PHGetSyncItem(0);
        if (!pSyncObj)
            return;
        SPHNetState state;
        pSyncObj->get_State(state);
        state.position = p;
        state.previous_position = p;
        pSyncObj->set_State(state);
    }
    break;
    }
}

// процесс отсоединения вещи заключается в спауне новой вещи
// в инвентаре и установке соответствующих флагов в родительском
// объекте, поэтому функция должна быть переопределена
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
        //.		D->s_gameid			=	u8(GameID());
        D->s_RP = 0xff;
        D->ID = 0xffff;
        if (GameID() == eGameIDSingle)
        {
            D->ID_Parent = u16(object().H_Parent()->ID());
        }
        else // i'm not sure this is right
        { // but it is simpliest way to avoid exception in MP BuyWnd... [Satan]
            if (object().H_Parent())
                D->ID_Parent = u16(object().H_Parent()->ID());
            else
                D->ID_Parent = NULL;
        }
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
    VERIFY(!m_pInventory);

    m_flags.set(FInInterpolation, FALSE);
    m_flags.set(FInInterpolate, FALSE);
    //	m_bInInterpolation				= false;
    //	m_bInterpolate					= false;

    m_flags.set(Fuseful_for_NPC, TRUE);
    CSE_Abstract* e = (CSE_Abstract*)(DC);
    CSE_ALifeObject* alife_object = smart_cast<CSE_ALifeObject*>(e);
    if (alife_object)
    {
        m_flags.set(Fuseful_for_NPC, alife_object->m_flags.test(CSE_ALifeObject::flUsefulForAI));
    }

    CSE_ALifeInventoryItem* pSE_InventoryItem = smart_cast<CSE_ALifeInventoryItem*>(e);
    if (!pSE_InventoryItem)
        return TRUE;

    //!!!
    m_fCondition = pSE_InventoryItem->m_fCondition;

    net_Spawn_install_upgrades(pSE_InventoryItem->m_upgrades);

    m_dwItemIndependencyTime = 0;

    m_just_after_spawn = true;
    m_activated = false;
    return TRUE;
}

void CInventoryItem::net_Destroy()
{
    if (m_pInventory)
    {
        VERIFY(std::find(m_pInventory->m_all.begin(), m_pInventory->m_all.end(), this) == m_pInventory->m_all.end());
    }

    // инвентарь которому мы принадлежали
    //.	m_pInventory = nullptr;
}

void CInventoryItem::save(NET_Packet& packet)
{
    packet.w_u16(m_ItemCurrPlace.value);
    packet.w_float(m_fCondition);
    //--	save_data				(m_upgrades, packet);

    if (object().H_Parent())
    {
        packet.w_u8(0);
        return;
    }

    u8 _num_items = (u8)object().PHGetSyncItemsNumber();
    packet.w_u8(_num_items);
    object().PHSaveState(packet);
}

void CInventoryItem::net_Import(NET_Packet& P)
{
    // copy from CPhysicObject
    // Msg("Inventory item [%d][%s] net_Import...", object().ID(), object().cName().c_str());
    u8 NumItems = 0;
    NumItems = P.r_u8();
    if (!NumItems)
        return;

    mask_inv_num_items num_items;
    num_items.common = NumItems;
    NumItems = num_items.num_items;

    /*if (num_items.mask & CSE_ALifeObjectPhysic::animated)
    {
        net_Import_Anim_Params(P);
    }*/

    net_update_IItem N;
    N.dwTimeStamp = Device.dwTimeGlobal;

    net_Import_PH_Params(P, N, num_items);
    ////////////////////////////////////////////
    P.r_u8(); // active (not freezed ot not)

    if (this->cast_game_object()->Local())
    {
        return;
    }

    net_updateInvData* p = NetSync();

    //	if (!p->NET_IItem.empty() && (p->NET_IItem.back().dwTimeStamp>=N.dwTimeStamp))
    //		return;

    // if (!p->NET_IItem.empty())
    // m_flags.set							(FInInterpolate, TRUE);

    Level().AddObject_To_Objects4CrPr(m_object);
    // this->CrPr_SetActivated				(false);
    // this->CrPr_SetActivationStep			(0);

    p->NET_IItem.push_back(N);

    while (p->NET_IItem.size() > 2)
    {
        p->NET_IItem.pop_front();
    }
    if (!m_activated)
    {
#ifdef DEBUG
        Msg("Activating object [%d] before interpolation starts", object().ID());
#endif // #ifdef DEBUG
        object().processing_activate();
        m_activated = true;
    }

    /*u8							NumItems = 0;
    NumItems					= P.r_u8();
    if (!NumItems)
        return;

    net_update_IItem			N;
    N.State.force.set			(0.f,0.f,0.f);
    N.State.torque.set			(0.f,0.f,0.f);

    P.r_vec3					(N.State.position);

    N.State.quaternion.x		= P.r_float_q8(-1.f, 1.f);
    N.State.quaternion.y		= P.r_float_q8(-1.f, 1.f);
    N.State.quaternion.z		= P.r_float_q8(-1.f, 1.f);
    N.State.quaternion.w		= P.r_float_q8(-1.f, 1.f);

    mask_num_items				num_items;
    num_items.common			= NumItems;
    NumItems					= num_items.num_items;

    N.State.enabled				= num_items.mask & CSE_ALifeInventoryItem::inventory_item_state_enabled;
    if (!(num_items.mask & CSE_ALifeInventoryItem::inventory_item_angular_null)) {
        N.State.angular_vel.x	= P.r_float_q8(0.f,10.f*PI_MUL_2);
        N.State.angular_vel.y	= P.r_float_q8(0.f,10.f*PI_MUL_2);
        N.State.angular_vel.z	= P.r_float_q8(0.f,10.f*PI_MUL_2);
    }
    else
        N.State.angular_vel.set	(0.f,0.f,0.f);

    if (!(num_items.mask & CSE_ALifeInventoryItem::inventory_item_linear_null)) {
        N.State.linear_vel.x	= P.r_float_q8(-32.f,32.f);
        N.State.linear_vel.y	= P.r_float_q8(-32.f,32.f);
        N.State.linear_vel.z	= P.r_float_q8(-32.f,32.f);
    }
    else
        N.State.linear_vel.set	(0.f,0.f,0.f);
    ////////////////////////////////////////////

    N.State.previous_position	= N.State.position;
    N.State.previous_quaternion	= N.State.quaternion;

    net_updateData				*p = NetSync();

    if (!p->NET_IItem.empty())
    {
        //if (p->NET_IItem.back().dwTimeStamp>=N.dwTimeStamp)
        //{
        //	return;
        //}
        //m_flags.set				(FInInterpolate, TRUE);
    }

    Level().AddObject_To_Objects4CrPr		(m_object);
    object().CrPr_SetActivated				(false);
    object().CrPr_SetActivationStep			(0);

    p->NET_IItem.push_back					(N);
    while (p->NET_IItem.size() > 2)
    {
        p->NET_IItem.pop_front				();
    };

    P.r_u8();	//enabled or not*/
};

void CInventoryItem::net_Import_PH_Params(NET_Packet& P, net_update_IItem& N, mask_inv_num_items& num_items)
{
    // N.State.force.set			(0.f,0.f,0.f);
    // N.State.torque.set			(0.f,0.f,0.f);
    // UI().Font().pFontStat->OutSet(100.0f,100.0f);
    P.r_vec3(N.State.force);
    // Msg("Import N.State.force.y:%4.6f",N.State.force.y);
    P.r_vec3(N.State.torque);

    P.r_vec3(N.State.position);
    // Msg("Import N.State.position.y:%4.6f",N.State.position.y);

    P.r_float(N.State.quaternion.x);
    P.r_float(N.State.quaternion.y);
    P.r_float(N.State.quaternion.z);
    P.r_float(N.State.quaternion.w);

    N.State.enabled = num_items.mask & CSE_ALifeInventoryItem::inventory_item_state_enabled;
    // UI().Font().pFontStat->OutNext("Import N.State.enabled:%i",int(N.State.enabled));
    if (!(num_items.mask & CSE_ALifeInventoryItem::inventory_item_angular_null))
    {
        N.State.angular_vel.x = P.r_float();
        N.State.angular_vel.y = P.r_float();
        N.State.angular_vel.z = P.r_float();
    }
    else
        N.State.angular_vel.set(0.f, 0.f, 0.f);

    if (!(num_items.mask & CSE_ALifeInventoryItem::inventory_item_linear_null))
    {
        N.State.linear_vel.x = P.r_float();
        N.State.linear_vel.y = P.r_float();
        N.State.linear_vel.z = P.r_float();
    }
    else
        N.State.linear_vel.set(0.f, 0.f, 0.f);
    // Msg("Import N.State.linear_vel.y:%4.6f",N.State.linear_vel.y);

    N.State.previous_position = N.State.position;
    N.State.previous_quaternion = N.State.quaternion;
}

void CInventoryItem::net_Export_PH_Params(NET_Packet& P, SPHNetState& State, mask_inv_num_items& num_items)
{
    // UI().Font().pFontStat->OutSet(100.0f,100.0f);
    P.w_vec3(State.force);
    // Msg("Export State.force.y:%4.6f",State.force.y);
    P.w_vec3(State.torque);
    // UI().Font().pFontStat->OutNext("Export State.torque:%4.6f",State.torque.magnitude());
    P.w_vec3(State.position);
    // Msg("Export State.position.y:%4.6f",State.position.y);
    // Msg("Export State.enabled:%i",int(State.enabled));

    float magnitude = _sqrt(State.quaternion.magnitude());
    if (fis_zero(magnitude))
    {
        magnitude = 1;
        State.quaternion.x = 0.f;
        State.quaternion.y = 0.f;
        State.quaternion.z = 1.f;
        State.quaternion.w = 0.f;
    }
    else
    {
        /*		float				invert_magnitude = 1.f/magnitude;

        State.quaternion.x	*= invert_magnitude;
        State.quaternion.y	*= invert_magnitude;
        State.quaternion.z	*= invert_magnitude;
        State.quaternion.w	*= invert_magnitude;

        clamp				(State.quaternion.x,-1.f,1.f);
        clamp				(State.quaternion.y,-1.f,1.f);
        clamp				(State.quaternion.z,-1.f,1.f);
        clamp				(State.quaternion.w,-1.f,1.f);*/
    }

    P.w_float(State.quaternion.x);
    P.w_float(State.quaternion.y);
    P.w_float(State.quaternion.z);
    P.w_float(State.quaternion.w);

    if (!(num_items.mask & CSE_ALifeInventoryItem::inventory_item_angular_null))
    {
        /*	clamp				(State.angular_vel.x,-10.f*PI_MUL_2,10.f*PI_MUL_2);
        clamp				(State.angular_vel.y,-10.f*PI_MUL_2,10.f*PI_MUL_2);
        clamp				(State.angular_vel.z,-10.f*PI_MUL_2,10.f*PI_MUL_2);*/

        P.w_float(State.angular_vel.x);
        P.w_float(State.angular_vel.y);
        P.w_float(State.angular_vel.z);
    }

    if (!(num_items.mask & CSE_ALifeInventoryItem::inventory_item_linear_null))
    {
        /*clamp				(State.linear_vel.x,-32.f,32.f);
        clamp				(State.linear_vel.y,-32.f,32.f);
        clamp				(State.linear_vel.z,-32.f,32.f);*/

        P.w_float(State.linear_vel.x);
        P.w_float(State.linear_vel.y);
        P.w_float(State.linear_vel.z);
        // Msg("Export State.linear_vel.y:%4.6f",State.linear_vel.y);
    }
    else
    {
        // Msg("Export State.linear_vel.y:%4.6f",0.0f);
    }
}

void CInventoryItem::net_Export(NET_Packet& P) { P.w_u8(0); };

void CInventoryItem::load(IReader& packet)
{
    m_ItemCurrPlace.value = packet.r_u16();
    m_fCondition = packet.r_float();

    //--	load_data( m_upgrades, packet );
    //--	install_loaded_upgrades();

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

///////////////////////////////////////////////
void CInventoryItem::PH_B_CrPr() {
    /*net_updateData* p		= NetSync();
    //just set last update data for now
    if (object().CrPr_IsActivated()) return;
    if (object().CrPr_GetActivationStep() > ph_world->m_steps_num) return;
    object().CrPr_SetActivated(true);

    ///////////////////////////////////////////////
    CPHSynchronize* pSyncObj				= nullptr;
    pSyncObj = object().PHGetSyncItem		(0);
    if (!pSyncObj)							return;
    ///////////////////////////////////////////////
    pSyncObj->get_State						(p->LastState);
    ///////////////////////////////////////////////
    net_update_IItem N_I	= p->NET_IItem.back();

    pSyncObj->set_State						(N_I.State);

    object().PHUnFreeze						();
    ///////////////////////////////////////////////
    if (Level().InterpolationDisabled())
    {
        m_flags.set			(FInInterpolation, FALSE);
//		m_bInInterpolation = false;
    };*/
    ///////////////////////////////////////////////
};

void CInventoryItem::PH_I_CrPr() // actions & operations between two phisic prediction steps
{
    /*net_updateData* p					= NetSync();
    //store recalculated data, then we able to restore it after small future prediction
    if (!object().CrPr_IsActivated())	return;
    ////////////////////////////////////
    CPHSynchronize* pSyncObj			= nullptr;
    pSyncObj = object().PHGetSyncItem	(0);
    if (!pSyncObj)						return;
    ////////////////////////////////////
    pSyncObj->get_State					(p->RecalculatedState);
    ///////////////////////////////////////////////
    Fmatrix xformX;
    pSyncObj->cv2obj_Xfrom(p->RecalculatedState.quaternion, p->RecalculatedState.position, xformX);

    VERIFY2								(_valid(xformX),*object().cName());
    pSyncObj->cv2obj_Xfrom				(p->RecalculatedState.quaternion, p->RecalculatedState.position, xformX);

    p->IRecRot.set(xformX);
    p->IRecPos.set(xformX.c);
    VERIFY2								(_valid(p->IRecPos),*object().cName());*/
};

#ifdef DEBUG
void CInventoryItem::PH_Ch_CrPr() {
    /*net_updateData* p					= NetSync();
    //restore recalculated data and get data for interpolation
    if (!object().CrPr_IsActivated())	return;
    ////////////////////////////////////
    CPHSynchronize* pSyncObj			= nullptr;
    pSyncObj = object().PHGetSyncItem	(0);
    if (!pSyncObj)						return;
    ////////////////////////////////////
    pSyncObj->get_State					(p->CheckState);

    if (!object().H_Parent() && object().getVisible())
    {
        if (p->CheckState.enabled == false && p->RecalculatedState.enabled == true)
        {
            ///////////////////////////////////////////////////////////////////
            pSyncObj->set_State			(p->LastState);
            pSyncObj->set_State			(p->RecalculatedState);//, N_A.State.enabled);

            object().PHUnFreeze			();
            ///////////////////////////////////////////////////////////////////
            ph_world->Step				();
            ///////////////////////////////////////////////////////////////////
            PH_Ch_CrPr					();
            ////////////////////////////////////
        };
    };*/
};
#endif

void CInventoryItem::PH_A_CrPr()
{
    if (m_just_after_spawn)
    {
        VERIFY(object().Visual());
        IKinematics* K = object().Visual()->dcast_PKinematics();
        VERIFY(K);
        if (!object().PPhysicsShell())
        {
            Msg("! ERROR: PhysicsShell is nullptr, object [%s][%d]", object().cName().c_str(), object().ID());
            VERIFY2(0, "physical shell is nullptr");
            return;
        }
        if (!object().PPhysicsShell()->isFullActive())
        {
            K->CalculateBones_Invalidate();
            K->CalculateBones(TRUE);
        }
        object().PPhysicsShell()->GetGlobalTransformDynamic(&object().XFORM());
        K->CalculateBones_Invalidate();
        K->CalculateBones(TRUE);

        object().spatial_move();
        m_just_after_spawn = false;

        object().PPhysicsShell()->get_ElementByStoreOrder(0)->Fix();
        object().PPhysicsShell()->SetIgnoreStatic();
    }
};

void CInventoryItem::Interpolate() { return; }

float CInventoryItem::interpolate_states(net_update_IItem const& first, net_update_IItem const& last, SPHNetState& current)
{
    float ret_val = 0.f;
    u32 CurTime = Device.dwTimeGlobal;

    if (CurTime == last.dwTimeStamp)
        return 0.f;

    float factor = float(CurTime - last.dwTimeStamp) / float(last.dwTimeStamp - first.dwTimeStamp);

    ret_val = factor;
    if (factor > 1.f)
    {
        factor = 1.f;
    }
    else if (factor < 0.f)
    {
        factor = 0.f;
    }

    current.position.x = first.State.position.x + (factor * (last.State.position.x - first.State.position.x));
    current.position.y = first.State.position.y + (factor * (last.State.position.y - first.State.position.y));
    current.position.z = first.State.position.z + (factor * (last.State.position.z - first.State.position.z));
    current.previous_position = current.position;

    current.quaternion.slerp(first.State.quaternion, last.State.quaternion, factor);
    current.previous_quaternion = current.quaternion;
    return ret_val;
}

void CInventoryItem::reload(LPCSTR section)
{
    inherited::reload(section);
    m_holder_range_modifier = READ_IF_EXISTS(pSettings, r_float, section, "holder_range_modifier", 1.f);
    m_holder_fov_modifier = READ_IF_EXISTS(pSettings, r_float, section, "holder_fov_modifier", 1.f);
}

void CInventoryItem::reinit()
{
    m_pInventory = nullptr;
    m_ItemCurrPlace.type = eItemPlaceUndefined;
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
    int boneL = -1, boneR = -1, boneR2 = -1;
    E->g_WeaponBones(boneL, boneR, boneR2);
    if (boneR == -1)
        return;
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
        if (!(dbg_net_Draw_Flags.is_any(dbg_draw_invitem)))
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

bool CInventoryItem::NeedToDestroyObject() const
{
    if (GameID() == eGameIDSingle)
        return false;

    if (object().Remote())
        return false;
    if (TimePassedAfterIndependant() > ITEM_REMOVE_TIME)
        return true;

    return false;
}

ALife::_TIME_ID CInventoryItem::TimePassedAfterIndependant() const
{
    if (!object().H_Parent() && m_dwItemIndependencyTime != 0)
        return Level().timeServer() - m_dwItemIndependencyTime;
    else
        return 0;
}

bool CInventoryItem::CanTrade() const
{
    bool res = true;
#pragma todo("Dima to Andy : why CInventoryItem::CanTrade can be called for the item, which doesn't have owner?")
    if (m_pInventory)
        res = inventory_owner().AllowItemToTrade(this, m_ItemCurrPlace);

    return (res && m_flags.test(FCanTrade) && !IsQuestItem());
}

Frect CInventoryItem::GetKillMsgRect() const
{
    float x, y, w, h;

    x = READ_IF_EXISTS(pSettings, r_float, m_object->cNameSect(), "kill_msg_x", 0.0f);
    y = READ_IF_EXISTS(pSettings, r_float, m_object->cNameSect(), "kill_msg_y", 0.0f);
    w = READ_IF_EXISTS(pSettings, r_float, m_object->cNameSect(), "kill_msg_width", 0.0f);
    h = READ_IF_EXISTS(pSettings, r_float, m_object->cNameSect(), "kill_msg_height", 0.0f);

    return Frect().set(x, y, w, h);
}

Irect CInventoryItem::GetInvGridRect() const
{
    u32 x, y, w, h;

    x = pSettings->r_u32(m_object->cNameSect(), "inv_grid_x");
    y = pSettings->r_u32(m_object->cNameSect(), "inv_grid_y");
    w = pSettings->r_u32(m_object->cNameSect(), "inv_grid_width");
    h = pSettings->r_u32(m_object->cNameSect(), "inv_grid_height");

    return Irect().set(x, y, w, h);
}

Irect CInventoryItem::GetUpgrIconRect() const
{
    u32 x, y, w, h;

    x = READ_IF_EXISTS(pSettings, r_u32, m_object->cNameSect(), "upgr_icon_x", 0);
    y = READ_IF_EXISTS(pSettings, r_u32, m_object->cNameSect(), "upgr_icon_y", 0);
    w = READ_IF_EXISTS(pSettings, r_u32, m_object->cNameSect(), "upgr_icon_width", 0);
    h = READ_IF_EXISTS(pSettings, r_u32, m_object->cNameSect(), "upgr_icon_height", 0);

    return Irect().set(x, y, w, h);
}

bool CInventoryItem::IsNecessaryItem(CInventoryItem* item) { return IsNecessaryItem(item->object().cNameSect()); };

BOOL CInventoryItem::IsInvalid() const { return object().getDestroy() || GetDropManual(); }

u16 CInventoryItem::object_id() const { return object().ID(); }

u16 CInventoryItem::parent_id() const { return (object().H_Parent()) ? object().H_Parent()->ID() : u16(-1); }

void CInventoryItem::SetDropManual(BOOL val) { m_flags.set(FdropManual, val); }

bool CInventoryItem::has_network_synchronization() const { return false; }