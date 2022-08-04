////////////////////////////////////////////////////////////////////////////
//	Module 		: restricted_object.cpp
//	Created 	: 18.08.2004
//  Modified 	: 23.08.2004
//	Author		: Dmitriy Iassenev
//	Description : Restricted object
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "restricted_object.h"
#include "space_restriction_manager.h"
#include "xrServer_Objects_ALife_Monsters.h"
#include "level.h"
#include "ai_space.h"
#include "level_graph.h"
#include "space_restriction.h"
#include "space_restriction_bridge.h"
#include "space_restriction_base.h"
#include "profiler.h"
#include "alife_simulator.h"
#include "alife_object_registry.h"
#include "game_graph.h"
#include "custommonster.h"

CRestrictedObject::~CRestrictedObject() {}

template <typename StrType>
static void construct_string(StrType& result, xr_vector<ALife::_OBJECT_ID>& restrictions, const CSE_ALifeMonsterAbstract* monster)
{
    u32 count = strlen(result) ? _GetItemCount(result) : 0;

    for (auto iter = restrictions.begin(); iter != restrictions.end();)
    {
        const auto* object = ai().alife().objects().object(*iter);
        if (object)
        {
            if (ai().game_graph().vertex(object->m_tGraphID)->level_id() == ai().level_graph().level_id())
            {
                if (count)
                    strcat_s(result, ",");

                strcat_s(result, object->name_replace());
                ++count;
            }

            ++iter;
        }
        else
        {
            Msg("~~[%s]: remove invalid restriction with ID[%u] from %s", __FUNCTION__, *iter, monster->name_replace());
            iter = restrictions.erase(iter);
        }
    }
}

BOOL CRestrictedObject::net_Spawn(CSE_Abstract* data)
{
    CSE_Abstract* abstract = (CSE_Abstract*)(data);
    CSE_ALifeMonsterAbstract* monster = smart_cast<CSE_ALifeMonsterAbstract*>(abstract);
    VERIFY(monster);
    m_applied = false;
    m_removed = true;

    string4096 temp0{}, temp1{};

    strcpy_s(temp0, *monster->m_out_space_restrictors);
    strcpy_s(temp1, *monster->m_in_space_restrictors);

    if (ai().get_alife())
    {
        construct_string(temp0, monster->m_dynamic_out_restrictions, monster);
        construct_string(temp1, monster->m_dynamic_in_restrictions, monster);
    }

    Level().space_restriction_manager().restrict(monster->ID, temp0, temp1);

    actual(true);

    return (TRUE);
}

void CRestrictedObject::net_Destroy() { Level().space_restriction_manager().unrestrict(m_object->ID()); }

u32 CRestrictedObject::accessible_nearest(const Fvector& position, Fvector& result) const
{
    START_PROFILE("Restricted Object/Accessible Nearest");
    VERIFY(!accessible(position));
    return (Level().space_restriction_manager().accessible_nearest(object().ID(), position, result));
    STOP_PROFILE;
}

bool CRestrictedObject::accessible(const Fvector& position) const
{
    START_PROFILE("Restricted Object/Accessible");
    return (accessible(position, EPS_L));
    STOP_PROFILE;
}

bool CRestrictedObject::accessible(const Fvector& position, float radius) const
{
    START_PROFILE("Restricted Object/Accessible");
    Fsphere sphere;
    sphere.P = position;
    sphere.R = radius;
    return (Level().space_restriction_manager().accessible(object().ID(), sphere));
    STOP_PROFILE;
}

bool CRestrictedObject::accessible(u32 level_vertex_id) const
{
    START_PROFILE("Restricted Object/Accessible");
    VERIFY(ai().level_graph().valid_vertex_id(level_vertex_id));
    return (accessible(level_vertex_id, EPS_L));
    STOP_PROFILE;
}

bool CRestrictedObject::accessible(u32 level_vertex_id, float radius) const
{
    START_PROFILE("Restricted Object/Accessible");
#ifdef CRASH_ON_INVALID_VERTEX_ID
    if (!ai().level_graph().valid_vertex_id(level_vertex_id))
        return false;
#endif
    VERIFY(ai().level_graph().valid_vertex_id(level_vertex_id));
    return (Level().space_restriction_manager().accessible(object().ID(), level_vertex_id, radius));
    STOP_PROFILE;
}

void CRestrictedObject::add_border(u32 start_vertex_id, float radius) const
{
    START_PROFILE("Restricted Object/Add Border");

    VERIFY(ai().level_graph().valid_vertex_id(start_vertex_id));
    VERIFY(!m_applied);
    VERIFY(m_removed);
    m_removed = false;
    if (accessible(start_vertex_id))
    {
        m_applied = true;
        Level().space_restriction_manager().add_border(object().ID(), start_vertex_id, radius);
    }

    STOP_PROFILE;
}

void CRestrictedObject::add_border(const Fvector& start_position, const Fvector& dest_position) const
{
    START_PROFILE("Restricted Object/Add Border");

    VERIFY(!m_applied);
    VERIFY(m_removed);
    m_removed = false;
    if (accessible(start_position))
    {
        m_applied = true;
        Level().space_restriction_manager().add_border(object().ID(), start_position, dest_position);
    }

    STOP_PROFILE;
}

void CRestrictedObject::add_border(u32 start_vertex_id, u32 dest_vertex_id) const
{
    START_PROFILE("Restricted Object/Add Border");
    VERIFY(ai().level_graph().valid_vertex_id(start_vertex_id));
    VERIFY(!m_applied);
    VERIFY(m_removed);
    m_removed = false;
    if (accessible(start_vertex_id))
    {
        m_applied = true;
        Level().space_restriction_manager().add_border(object().ID(), start_vertex_id, dest_vertex_id);
    }
    STOP_PROFILE;
}

void CRestrictedObject::remove_border() const
{
    START_PROFILE("Restricted Object/Remove Border");

    VERIFY(!m_removed);
    m_removed = true;
    if (m_applied)
        Level().space_restriction_manager().remove_border(object().ID());
    m_applied = false;

    STOP_PROFILE;
}

shared_str CRestrictedObject::in_restrictions() const
{
    START_PROFILE("Restricted Object/in_restrictions")
    return (Level().space_restriction_manager().in_restrictions(object().ID()));
    STOP_PROFILE
}

shared_str CRestrictedObject::out_restrictions() const
{
    START_PROFILE("Restricted Object/out_restrictions")
    return (Level().space_restriction_manager().out_restrictions(object().ID()));
    STOP_PROFILE
}

shared_str CRestrictedObject::base_in_restrictions() const
{
    START_PROFILE("Restricted Object/base_in_restrictions")
    return (Level().space_restriction_manager().base_in_restrictions(object().ID()));
    STOP_PROFILE
}

shared_str CRestrictedObject::base_out_restrictions() const
{
    START_PROFILE("Restricted Object/out_restrictions")
    return (Level().space_restriction_manager().base_out_restrictions(object().ID()));
    STOP_PROFILE
}

IC void CRestrictedObject::add_object_restriction(ALife::_OBJECT_ID id, const RestrictionSpace::ERestrictorTypes& restrictor_type)
{
    NET_Packet net_packet;
    object().u_EventGen(net_packet, GE_ADD_RESTRICTION, object().ID());
    net_packet.w(&id, sizeof(id));
    net_packet.w(&restrictor_type, sizeof(restrictor_type));
    Level().Send(net_packet, net_flags(TRUE, TRUE));
}

IC void CRestrictedObject::remove_object_restriction(ALife::_OBJECT_ID id, const RestrictionSpace::ERestrictorTypes& restrictor_type)
{
    NET_Packet net_packet;
    object().u_EventGen(net_packet, GE_REMOVE_RESTRICTION, object().ID());
    net_packet.w(&id, sizeof(id));
    net_packet.w(&restrictor_type, sizeof(restrictor_type));
    Level().Send(net_packet, net_flags(TRUE, TRUE));
}

template <typename P, const bool value, typename StrType>
static void construct_restriction_string(StrType& temp_restrictions, const xr_vector<ALife::_OBJECT_ID>& restrictions, const shared_str& current_restrictions, const P& p,
                                         CRestrictedObject* RObj)
{
    xr_vector<xr_string> cur_restrs;
    const int cnt = _GetItemCount(current_restrictions.c_str());
    for (int i = 0; i < cnt; ++i)
    {
        string256 tmp;
        _GetItem(current_restrictions.c_str(), i, tmp);
        cur_restrs.emplace_back(std::move(tmp));
    }

    xr_vector<xr_string> new_restrs;

    for (const auto& id : restrictions)
    {
        const auto* object = Level().Objects.net_Find(id);
        if (!object)
            continue;

        xr_string s{object->cName().c_str()};
        if ((std::find(cur_restrs.begin(), cur_restrs.end(), s) != cur_restrs.end()) == value)
            continue;

        if (std::find(new_restrs.begin(), new_restrs.end(), s) == new_restrs.end())
            new_restrs.emplace_back(std::move(s));

        p(RObj, object->ID());
    }

    if (!new_restrs.empty())
    {
        xr_string s;
        for (const auto& it : new_restrs)
        {
            if (!s.empty())
                s += ",";
            s += it;
        }
        ASSERT_FMT(s.length() < std::size(temp_restrictions), "!![%s]: resulted string too long: object[%s] temp_restrictions_size[%u] s.length[%u]", __FUNCTION__,
                   RObj->object().cName().c_str(), std::size(temp_restrictions), s.length());
        strcpy_s(temp_restrictions, s.c_str());
    }
}

template <bool add>
struct CRestrictionPredicate
{
    RestrictionSpace::ERestrictorTypes m_restrictor_type;

    IC CRestrictionPredicate(RestrictionSpace::ERestrictorTypes restrictor_type) { m_restrictor_type = restrictor_type; }

    IC void operator()(CRestrictedObject* object, ALife::_OBJECT_ID id) const
    {
        if (add)
            object->add_object_restriction(id, m_restrictor_type);
        else
            object->remove_object_restriction(id, m_restrictor_type);
    }
};

void CRestrictedObject::add_restrictions(const xr_vector<ALife::_OBJECT_ID>& out_restrictions, const xr_vector<ALife::_OBJECT_ID>& in_restrictions)
{
    if (out_restrictions.empty() && in_restrictions.empty())
        return;

    START_PROFILE("Restricted Object/Add Restrictions");

    string4096 temp_out_restrictions{}, temp_in_restrictions{};

    construct_restriction_string<CRestrictionPredicate<true>, true>(temp_out_restrictions, out_restrictions, this->out_restrictions(),
                                                                    CRestrictionPredicate<true>(RestrictionSpace::eRestrictorTypeOut), this);
    construct_restriction_string<CRestrictionPredicate<true>, true>(temp_in_restrictions, in_restrictions, this->in_restrictions(),
                                                                    CRestrictionPredicate<true>(RestrictionSpace::eRestrictorTypeIn), this);

    Level().space_restriction_manager().add_restrictions(object().ID(), temp_out_restrictions, temp_in_restrictions);

    actual(false);

    STOP_PROFILE;
}

void CRestrictedObject::remove_restrictions(const xr_vector<ALife::_OBJECT_ID>& out_restrictions, const xr_vector<ALife::_OBJECT_ID>& in_restrictions)
{
    if (out_restrictions.empty() && in_restrictions.empty())
        return;

    START_PROFILE("Restricted Object/Remove Restrictions");

    string4096 temp_out_restrictions{}, temp_in_restrictions{};

    construct_restriction_string<CRestrictionPredicate<false>, false>(temp_out_restrictions, out_restrictions, this->out_restrictions(),
                                                                      CRestrictionPredicate<false>(RestrictionSpace::eRestrictorTypeOut), this);
    construct_restriction_string<CRestrictionPredicate<false>, false>(temp_in_restrictions, in_restrictions, this->in_restrictions(),
                                                                      CRestrictionPredicate<false>(RestrictionSpace::eRestrictorTypeIn), this);

    Level().space_restriction_manager().remove_restrictions(object().ID(), temp_out_restrictions, temp_in_restrictions);

    actual(false);

    STOP_PROFILE;
}

void CRestrictedObject::add_restrictions(const shared_str& out_restrictions, const shared_str& in_restrictions)
{
    if (!out_restrictions.size() && !in_restrictions.size())
        return;

    START_PROFILE("Restricted Object/Add Restrictions");

    Level().space_restriction_manager().add_restrictions(object().ID(), *out_restrictions, *in_restrictions);

    actual(false);

    STOP_PROFILE;
}

void CRestrictedObject::remove_restrictions(const shared_str& out_restrictions, const shared_str& in_restrictions)
{
    if (!out_restrictions.size() && !in_restrictions.size())
        return;

    START_PROFILE("Restricted Object/Remove Restrictions");

    Level().space_restriction_manager().remove_restrictions(object().ID(), *out_restrictions, *in_restrictions);

    actual(false);

    STOP_PROFILE;
}

void CRestrictedObject::remove_all_restrictions(const RestrictionSpace::ERestrictorTypes& restrictor_type)
{
    NET_Packet net_packet;
    object().u_EventGen(net_packet, GE_REMOVE_ALL_RESTRICTIONS, object().ID());
    net_packet.w(&restrictor_type, sizeof(restrictor_type));
    Level().Send(net_packet, net_flags(TRUE, TRUE));
}

void CRestrictedObject::remove_all_restrictions()
{
    START_PROFILE("Restricted Object/Remove Restrictions");

    remove_all_restrictions(RestrictionSpace::eRestrictorTypeOut);
    remove_all_restrictions(RestrictionSpace::eRestrictorTypeIn);

    if (Level().space_restriction_manager().in_restrictions(object().ID()).size() || Level().space_restriction_manager().out_restrictions(object().ID()).size())
        actual(false);

    Level().space_restriction_manager().restrict(object().ID(), "", "");

    STOP_PROFILE;
}

void CRestrictedObject::actual(bool value)
{
    m_actual = value;
    if (!actual())
        m_object->on_restrictions_change();
}
