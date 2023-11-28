////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_anomalous_zone.cpp
//	Created 	: 27.10.2005
//  Modified 	: 27.10.2005
//	Author		: Dmitriy Iassenev
//	Description : ALife anomalous zone class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "xrServer_Objects_ALife_Monsters.h"
#include "ai_space.h"
#include "alife_simulator.h"
#include "alife_time_manager.h"
#include "alife_spawn_registry.h"
#include "alife_graph_registry.h"

CSE_ALifeItemWeapon* CSE_ALifeAnomalousZone::tpfGetBestWeapon(ALife::EHitType& tHitType, float& fHitPower)
{
    m_tpCurrentBestWeapon = 0;
    m_tTimeID = ai().alife().time_manager().game_time();
    fHitPower = 1.0f; // m_maxPower;
    tHitType = m_tHitType;
    return (m_tpCurrentBestWeapon);
}

ALife::EMeetActionType CSE_ALifeAnomalousZone::tfGetActionType(CSE_ALifeSchedulable* tpALifeSchedulable, int iGroupIndex, bool bMutualDetection)
{
    return (ALife::eMeetActionTypeAttack);
}

bool CSE_ALifeAnomalousZone::bfActive() { return !interactive(); }

CSE_ALifeDynamicObject* CSE_ALifeAnomalousZone::tpfGetBestDetector()
{
    VERIFY2(false, "This function shouldn't be called");
    NODEFAULT;
#ifdef DEBUG
    return (0);
#endif
}

void CSE_ALifeAnomalousZone::spawn_artefacts()
{
    static const bool enabled = READ_IF_EXISTS(pSettings, r_bool, "features", "enable_spawn_artefacts", false);
    if (!enabled)
        return;

    VERIFY2(!m_bOnline, "Cannot spawn artefacts in online!");

    //float m_min_start_power = pSettings->r_float(name(), "min_start_power");
    //float m_max_start_power = pSettings->r_float(name(), "max_start_power");

    u32 m_min_artefact_count = pSettings->r_u32(name(), "min_artefact_count");
    u32 m_max_artefact_count = pSettings->r_u32(name(), "max_artefact_count");
    
    u32 m_artefact_count;

    if (m_min_artefact_count == m_max_artefact_count)
        m_artefact_count = m_min_artefact_count;
    else
        m_artefact_count = ::Random.randI(m_min_artefact_count, m_max_artefact_count);

    if (m_artefact_count == 0)
        return;

    //if (m_min_start_power == m_max_start_power)
    //    m_maxPower = m_min_start_power;
    //else
    //    m_maxPower = randF(m_min_start_power, m_max_start_power);

    LPCSTR artefacts = pSettings->r_string(name(), "artefacts");
    if (artefacts == nullptr)
        return;


    if (::Random.randF(0.f, 1.f) > READ_IF_EXISTS(pSettings, r_float, name(), "artefact_offline_spawn_probability", 0.5f))
        return;


    u16 itemCount = (u16)_GetItemCount(artefacts);
    VERIFY2(!(n % 2), "Invalid parameters count in line artefacts for anomalous zone");
    itemCount >>= 1;

    typedef std::pair<shared_str, float> artefact_pair;

    xr_vector<artefact_pair> m_weights;

    float total_probability = 0.f;

    m_weights.resize(itemCount);
    for (u16 i = 0; i < itemCount; ++i)
    {
        string256 temp0, temp1;
        _GetItem(artefacts, 2 * i, temp0);
        _GetItem(artefacts, 2 * i + 1, temp1);

        artefact_pair& artefact_spawn = m_weights[i];
        artefact_spawn.first = temp0;
        artefact_spawn.second = (float)atof(temp1);

        total_probability += artefact_spawn.second;
    }

    // нормализировать вероятности
    for (u32 i = 0; i < m_weights.size(); ++i)
    {
        m_weights[i].second = m_weights[i].second / total_probability;
    }

    for (u32 i = 0; i < m_artefact_count; ++i)
    {
        const float fProbability = ::Random.randF(1.f);
        float fSum = 0.f;

        u16 item = 0;
        for (; item < itemCount; ++item)
        {
            fSum += m_weights[item].second;
            if (fProbability < fSum)
                break;
        }

        if (item < itemCount)
        {
            Msg("~ [%s] id=%d offline spawn art [%s]", name(), ID, * m_weights[item].first);

            Fvector art_pos = position();

            art_pos.y = art_pos.y + 1;

            art_pos.x = art_pos.x + ::Random.randF(-0.5, 0.5);
            art_pos.z = art_pos.z + ::Random.randF(-0.5, 0.5);

            CSE_Abstract* l_tpSE_Abstract = alife().spawn_item(*m_weights[item].first, art_pos, m_tNodeID, m_tGraphID, 0xffff);
            R_ASSERT3(l_tpSE_Abstract, "Can't spawn artefact ", *m_weights[item].first);

            CSE_ALifeDynamicObject* i = smart_cast<CSE_ALifeDynamicObject*>(l_tpSE_Abstract);
            R_ASSERT2(i, "Non-ALife object in the 'game.spawn'");

            i->m_tSpawnID = m_tSpawnID;
            i->m_bALifeControl = true;

            //ai().alife().spawns().assign_artefact_position(this, i);

            //Fvector t = i->o_Position;
            //u32 p = i->m_tNodeID;
            //float q = i->m_fDistance;

            //alife().graph().change(i, m_tGraphID, i->m_tGraphID);

            //i->o_Position = t;
            //i->m_tNodeID = p;
            //i->m_fDistance = q;

            CSE_ALifeItemArtefact* l_tpALifeItemArtefact = smart_cast<CSE_ALifeItemArtefact*>(i);
            R_ASSERT2(l_tpALifeItemArtefact, "Anomalous zone can't generate non-artefact objects since they don't have an 'anomaly property'!");
            l_tpALifeItemArtefact->m_fAnomalyValue = /*m_maxPower * */ (1.f - i->o_Position.distance_to(o_Position) / m_offline_interactive_radius);
        }
    }
}

void CSE_ALifeAnomalousZone::on_spawn()
{
    inherited::on_spawn();
    spawn_artefacts();
}
