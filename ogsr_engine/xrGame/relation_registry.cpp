//////////////////////////////////////////////////////////////////////////
// relation_registry.cpp:	реестр для хранения данных об отношении персонажа к
//							другим персонажам
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "relation_registry.h"
#include "alife_registry_wrappers.h"

#include "character_community.h"
#include "character_reputation.h"
#include "character_rank.h"

//////////////////////////////////////////////////////////////////////////

SRelation::SRelation() { m_iGoodwill = NEUTRAL_GOODWILL; }

//////////////////////////////////////////////////////////////////////////

void RELATION_DATA::clear()
{
    personal.clear();
    reverse_personal.clear();
    communities.clear();
}

void RELATION_DATA::load(IReader& stream)
{
    load_data(personal, stream);
    load_data(communities, stream);
}

void RELATION_DATA::save(IWriter& stream)
{
    save_data(personal, stream);
    save_data(communities, stream);
}

//////////////////////////////////////////////////////////////////////////

RELATION_REGISTRY::RELATION_MAP_SPOTS::RELATION_MAP_SPOTS()
{
    spot_names[ALife::eRelationTypeFriend] = "friend_location";
    spot_names[ALife::eRelationTypeNeutral] = "neutral_location";
    spot_names[ALife::eRelationTypeEnemy] = "enemy_location";
    spot_names[ALife::eRelationTypeWorstEnemy] = "enemy_location";
    // spot_names[ALife::eRelationTypeWorstEnemy]	= "enemy_location";
    spot_names[ALife::eRelationTypeLast] = "neutral_location";
}
//////////////////////////////////////////////////////////////////////////

CRelationRegistryWrapper* RELATION_REGISTRY::m_relation_registry = NULL;
RELATION_REGISTRY::FIGHT_VECTOR* RELATION_REGISTRY::m_fight_registry = NULL;
RELATION_REGISTRY::RELATION_MAP_SPOTS* RELATION_REGISTRY::m_spot_names = NULL;

//////////////////////////////////////////////////////////////////////////

RELATION_REGISTRY::RELATION_REGISTRY() {}

RELATION_REGISTRY::~RELATION_REGISTRY() {}

//////////////////////////////////////////////////////////////////////////

extern void load_attack_goodwill();
CRelationRegistryWrapper& RELATION_REGISTRY::relation_registry()
{
    if (!m_relation_registry)
    {
        m_relation_registry = xr_new<CRelationRegistryWrapper>();
        load_attack_goodwill();
    }

    return *m_relation_registry;
}

void RELATION_REGISTRY::build_reverse_personal()
{
    for (const auto& it : relation_registry().registry().get_registry_objects())
    {
        if (it.first)
        { // skip actor (0)
            const auto& relation_data = relation_registry().registry().objects(it.first);
            for (const auto& it2 : relation_data.personal)
            {
                if (it2.first)
                { // skip actor (0)
                    auto& relation_data2 = relation_registry().registry().objects(it2.first);
                    const auto it3 = std::find(relation_data2.reverse_personal.begin(), relation_data2.reverse_personal.end(), it.first);
                    ASSERT_FMT(it3 == relation_data2.reverse_personal.end(), "[%s]: %u already exists in revers_personal of %u", __FUNCTION__, it.first, it2.first);
                    relation_data2.reverse_personal.push_back(it.first);
                }
            }
        }
    }
}

RELATION_REGISTRY::FIGHT_VECTOR& RELATION_REGISTRY::fight_registry()
{
    if (!m_fight_registry)
        m_fight_registry = xr_new<FIGHT_VECTOR>();

    return *m_fight_registry;
}

void RELATION_REGISTRY::clear_relation_registry()
{
    xr_delete(m_relation_registry);
    xr_delete(m_fight_registry);
    xr_delete(m_spot_names);
}

const shared_str& RELATION_REGISTRY::GetSpotName(ALife::ERelationType& type)
{
    if (!m_spot_names)
        m_spot_names = xr_new<RELATION_MAP_SPOTS>();
    return m_spot_names->GetSpotName(type);
}

//////////////////////////////////////////////////////////////////////////

void RELATION_REGISTRY::ClearRelations(u16 person_id)
{
    if (relation_registry().registry().objects_ptr(person_id))
    {
        auto& relation_data = relation_registry().registry().objects(person_id);
        for (const auto& it : relation_data.personal)
        {
            if (it.first)
            { // skip actor (0)
                clear_reverse_personal(person_id, it.first);
            }
        }
        for (const auto to : relation_data.reverse_personal)
        {
            ASSERT_FMT(relation_registry().registry().objects_ptr(to), "[%s]: %u not found clearing %u", __FUNCTION__, to, person_id);
            auto& relation_data2 = relation_registry().registry().objects(to);
            const auto it = relation_data2.personal.find(person_id);
            ASSERT_FMT(it != relation_data2.personal.end(), "[%s]: %u not found in personal of %u", __FUNCTION__, person_id, to);
            relation_data2.personal.erase(it);
        }
        auto& objects = relation_registry().registry().get_registry_objects();
        auto it = objects.find(person_id);
        ASSERT_FMT(it != objects.end(), "[%s]: %u not found", __FUNCTION__, person_id);
        objects.erase(it);
    }
}

void RELATION_REGISTRY::clear_reverse_personal(u16 from, u16 to)
{
    ASSERT_FMT(to, "[%s]: actor detected clearing %u", __FUNCTION__, from);
    ASSERT_FMT(relation_registry().registry().objects_ptr(to), "[%s]: %u not found clearing %u", __FUNCTION__, to, from);
    auto& relation_data = relation_registry().registry().objects(to);
    const auto it = std::find(relation_data.reverse_personal.begin(), relation_data.reverse_personal.end(), from);
    ASSERT_FMT(it != relation_data.reverse_personal.end(), "[%s]: %u not found in reverse_personal of %u", __FUNCTION__, from, to);
    relation_data.reverse_personal.erase(it);
}

//////////////////////////////////////////////////////////////////////////
CHARACTER_GOODWILL RELATION_REGISTRY::GetGoodwill(u16 from, u16 to) const
{
    const RELATION_DATA* relation_data = relation_registry().registry().objects_ptr(from);

    if (relation_data)
    {
        PERSONAL_RELATION_MAP::const_iterator it = relation_data->personal.find(to);
        if (relation_data->personal.end() != it)
        {
            const SRelation& relation = (*it).second;
            return relation.Goodwill();
        }
    }
    //если отношение еще не задано, то возвращаем нейтральное
    return NEUTRAL_GOODWILL;
}

#include "game_object_space.h"
#include "script_callback_ex.h"
#include "script_game_object.h"
#include "Actor.h"

void RELATION_REGISTRY::SetGoodwill(u16 from, u16 to, CHARACTER_GOODWILL goodwill)
{
    RELATION_DATA& relation_data = relation_registry().registry().objects(from);

    static Ivector2 gw_limits = pSettings->r_ivector2(ACTIONS_POINTS_SECT, "personal_goodwill_limits");
    clamp(goodwill, gw_limits.x, gw_limits.y);

    relation_data.personal[to].SetGoodwill(goodwill);

    if (to)
    { // skip actor (0)
        auto& relation_data2 = relation_registry().registry().objects(to);
        auto it = std::find(relation_data2.reverse_personal.begin(), relation_data2.reverse_personal.end(), from);
        if (it == relation_data2.reverse_personal.end())
        {
            relation_data2.reverse_personal.push_back(from);
        }
    }

    if (g_actor)
        g_actor->callback(GameObject::eOnGoodwillChange)(from, to);
}

void RELATION_REGISTRY::ChangeGoodwill(u16 from, u16 to, CHARACTER_GOODWILL delta_goodwill)
{
    CHARACTER_GOODWILL new_goodwill = GetGoodwill(from, to) + delta_goodwill;
    SetGoodwill(from, to, new_goodwill);
}

//////////////////////////////////////////////////////////////////////////
CHARACTER_GOODWILL RELATION_REGISTRY::GetCommunityGoodwill(CHARACTER_COMMUNITY_INDEX from_community, u16 to_character) const
{
    const RELATION_DATA* relation_data = relation_registry().registry().objects_ptr(to_character);

    if (relation_data)
    {
        COMMUNITY_RELATION_MAP::const_iterator it = relation_data->communities.find(from_community);
        if (relation_data->communities.end() != it)
        {
            const SRelation& relation = (*it).second;
            return relation.Goodwill();
        }
    }
    //если отношение еще не задано, то возвращаем нейтральное
    return NEUTRAL_GOODWILL;
}

void RELATION_REGISTRY::SetCommunityGoodwill(CHARACTER_COMMUNITY_INDEX from_community, u16 to_character, CHARACTER_GOODWILL goodwill)
{
    static Ivector2 gw_limits = pSettings->r_ivector2(ACTIONS_POINTS_SECT, "community_goodwill_limits");
    clamp(goodwill, gw_limits.x, gw_limits.y);
    RELATION_DATA& relation_data = relation_registry().registry().objects(to_character);

    relation_data.communities[from_community].SetGoodwill(goodwill);
}

void RELATION_REGISTRY::ChangeCommunityGoodwill(CHARACTER_COMMUNITY_INDEX from_community, u16 to_character, CHARACTER_GOODWILL delta_goodwill)
{
    CHARACTER_GOODWILL gw = GetCommunityGoodwill(from_community, to_character) + delta_goodwill;
    SetCommunityGoodwill(from_community, to_character, gw);
}
//////////////////////////////////////////////////////////////////////////

CHARACTER_GOODWILL RELATION_REGISTRY::GetCommunityRelation(CHARACTER_COMMUNITY_INDEX index1, CHARACTER_COMMUNITY_INDEX index2) const
{
    return CHARACTER_COMMUNITY::relation(index1, index2);
}

CHARACTER_GOODWILL RELATION_REGISTRY::GetRankRelation(CHARACTER_RANK_VALUE rank1, CHARACTER_RANK_VALUE rank2) const
{
    CHARACTER_RANK rank_from, rank_to;
    rank_from.set(rank1);
    rank_to.set(rank2);
    return CHARACTER_RANK::relation(rank_from.index(), rank_to.index());
}

CHARACTER_GOODWILL RELATION_REGISTRY::GetReputationRelation(CHARACTER_REPUTATION_VALUE rep1, CHARACTER_REPUTATION_VALUE rep2) const
{
    CHARACTER_REPUTATION rep_from, rep_to;
    rep_from.set(rep1);
    rep_to.set(rep2);
    return CHARACTER_REPUTATION::relation(rep_from.index(), rep_to.index());
}

//////////////////////////////////////////////////////////////////////////

void RELATION_REGISTRY::ClearGoodwill(u16 from, u16 to)
{
    if (relation_registry().registry().objects_ptr(from))
    {
        auto& relation_data = relation_registry().registry().objects(from);
        relation_data.personal.erase(to);
        if (to)
        { // skip actor (0)
            clear_reverse_personal(from, to);
        }
    }
}
