////////////////////////////////////////////////////////////////////////////
//	Module 		: purchase_list.cpp
//	Created 	: 12.01.2006
//  Modified 	: 12.01.2006
//	Author		: Dmitriy Iassenev
//	Description : purchase list class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "purchase_list.h"
#include "inventoryowner.h"
#include "gameobject.h"
#include "ai_object_location.h"
#include "level.h"
#include "xrServer_Object_Base.h"
#include "xrServer_Objects_ALife.h"

static float min_deficit_factor = .3f;

void CPurchaseList::process(CInifile& ini_file, LPCSTR section, CInventoryOwner& owner)
{
    owner.sell_useless_items();

    m_deficits.clear();

    luabind::functor<void> lua_function;

    if (pSettings->line_exist("engine_callbacks", "trade_purchase_item_process"))
    {
        const char* callback = pSettings->r_string("engine_callbacks", "trade_purchase_item_process");
        if (!ai().script_engine().functor(callback, lua_function))
        {
            Msg("Cannot get engine callback %s!", pSettings->r_string("engine_callbacks", "trade_purchase_item_process"));
        }
    }

    const CGameObject& game_object = smart_cast<const CGameObject&>(owner);
    CInifile::Sect& S = ini_file.r_section(section);
    for (const auto& I : S.Data)
    {
        ASSERT_FMT(I.second.size(), "!![%s] : cannot handle lines in section without values: [%s], first param: [%s]", __FUNCTION__, section, I.first.c_str());
        ASSERT_FMT(_GetItemCount(I.second.c_str()) > 1, "!![%s] : Invalid second parameter in section [%s], first param: [%s]", __FUNCTION__, section, I.first.c_str());

        string256 temp0, temp1;
        process(game_object, I.first, atoi(_GetItem(I.second.c_str(), 0, temp0)), (float)atof(_GetItem(I.second.c_str(), 1, temp1)), lua_function);
    }
}

#include "script_game_object.h"

void CPurchaseList::process(const CGameObject& owner, const shared_str& name, const u32& count, const float& probability, luabind::functor<void>& lua_function)
{
    VERIFY3(count, "Invalid count for section in the purchase list", *name);
    VERIFY3(!fis_zero(probability, EPS_S), "Invalid probability for section in the purchase list", *name);

    const Fvector& position = owner.Position();
    const u32& level_vertex_id = owner.ai_location().level_vertex_id();
    const ALife::_OBJECT_ID& id = owner.ID();

    u32 j = 0;
    for (u32 i = 0; i < count; ++i)
    {
        if (::Random.randF(0.f, 1.f) > probability)
            continue;

        ++j;

        CSE_Abstract* _abstract = Level().spawn_item(*name, position, level_vertex_id, id, true);

        if (lua_function)
        {
            lua_function(owner.lua_game_object(), smart_cast<CSE_ALifeObject*>(_abstract));
        }

        NET_Packet P;
        _abstract->Spawn_Write(P, TRUE);
        Level().Send(P, net_flags(TRUE));
        F_entity_Destroy(_abstract);
    }

    VERIFY3(m_deficits.find(name) == m_deficits.end(), "Duplicate section in the purchase list", *name);
    m_deficits.insert(std::make_pair(name, (float)count * probability / _max((float)j, min_deficit_factor)));
}
