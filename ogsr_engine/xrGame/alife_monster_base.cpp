////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_monster_base.cpp
//	Created 	: 07.02.2007
//  Modified 	: 07.02.2007
//	Author		: Dmitriy Iassenev
//	Description : ALife mnster base class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "xrServer_Objects_ALife_Monsters.h"
#include "alife_simulator.h"
#include "xrServer.h"
#include "alife_monster_brain.h"

void CSE_ALifeMonsterBase::on_spawn()
{
    inherited1::on_spawn();

    if (!pSettings->line_exist(s_name, "Spawn_Inventory_Item_Section"))
        return;

    LPCSTR item_section = pSettings->r_string(s_name, "Spawn_Inventory_Item_Section");
    float spawn_probability = READ_IF_EXISTS(pSettings, r_float, s_name, "Spawn_Inventory_Item_Probability", 0.f);
    if (item_section && item_section[0])
    {
        string128 item;
        int count = _GetItemCount(item_section);
        for (int i = 0; i < count; i += 2)
        {
            _GetItem(item_section, i, item);
            float spawn_prob = spawn_probability;
            if (i + 1 < count)
            {
                string128 tmp;
                spawn_prob = atof(_GetItem(item_section, i + 1, tmp));
            }
            float probability = ::Random.randF();
            if (probability < spawn_prob || fsimilar(spawn_prob, 1.f))
            {
                alife().spawn_item(item, o_Position, m_tNodeID, m_tGraphID, ID)->ID_Parent = ID;
                break;
            }
        }
    }
}

extern void add_online_impl(CSE_ALifeDynamicObject* object, const bool& update_registries);

void CSE_ALifeMonsterBase::add_online(const bool& update_registries)
{
    add_online_impl(this, update_registries);
    brain().on_switch_online();
}

extern void add_offline_impl(CSE_ALifeDynamicObject* object, const xr_vector<ALife::_OBJECT_ID>& saved_children, const bool& update_registries);

void CSE_ALifeMonsterBase::add_offline(const xr_vector<ALife::_OBJECT_ID>& saved_children, const bool& update_registries)
{
    add_offline_impl(this, saved_children, update_registries);
    brain().on_switch_offline();
}
