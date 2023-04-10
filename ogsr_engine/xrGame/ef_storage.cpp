////////////////////////////////////////////////////////////////////////////
//	Module 		: ef_storage.cpp
//	Created 	: 25.03.2002
//  Modified 	: 11.10.2002
//	Author		: Dmitriy Iassenev
//	Description : Evaluation functions storage class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ef_storage.h"
#include "ef_primary.h"
#include "ef_pattern.h"

CEF_Storage::CEF_Storage()
{
    ZeroMemory(m_fpaBaseFunctions, sizeof(m_fpaBaseFunctions));

    m_fpaBaseFunctions[0] = xr_new<CDistanceFunction>(this);
    m_fpaBaseFunctions[1] = xr_new<CGraphPointType0>(this);
    m_fpaBaseFunctions[2] = m_pfEquipmentType = xr_new<CEquipmentType>(this);
    m_fpaBaseFunctions[3] = xr_new<CItemDeterioration>(this);
    m_fpaBaseFunctions[4] = xr_new<CEquipmentPreference>(this);
    m_fpaBaseFunctions[5] = m_pfMainWeaponType = xr_new<CMainWeaponType>(this);
    m_fpaBaseFunctions[6] = xr_new<CMainWeaponPreference>(this);
    m_fpaBaseFunctions[7] = m_pfItemValue = xr_new<CItemValue>(this);
    m_fpaBaseFunctions[8] = xr_new<CWeaponAmmoCount>(this);
    m_fpaBaseFunctions[9] = xr_new<CDetectorType>(this);

    m_fpaBaseFunctions[21] = xr_new<CPersonalHealthFunction>(this);
    m_fpaBaseFunctions[22] = xr_new<CPersonalMoraleFunction>(this);
    m_fpaBaseFunctions[23] = xr_new<CPersonalCreatureTypeFunction>(this);
    m_fpaBaseFunctions[24] = m_pfPersonalWeaponType = xr_new<CPersonalWeaponTypeFunction>(this);
    m_fpaBaseFunctions[25] = xr_new<CPersonalAccuracyFunction>(this);
    m_fpaBaseFunctions[26] = xr_new<CPersonalIntelligenceFunction>(this);
    m_fpaBaseFunctions[27] = xr_new<CPersonalRelationFunction>(this);
    m_fpaBaseFunctions[28] = xr_new<CPersonalGreedFunction>(this);
    m_fpaBaseFunctions[29] = xr_new<CPersonalAggressivenessFunction>(this);
    m_fpaBaseFunctions[30] = xr_new<CPersonalEyeRange>(this);
    m_fpaBaseFunctions[31] = xr_new<CPersonalMaxHealth>(this);

    m_fpaBaseFunctions[41] = xr_new<CEnemyHealthFunction>(this);
    m_fpaBaseFunctions[42] = xr_new<CEnemyCreatureTypeFunction>(this);
    m_fpaBaseFunctions[43] = xr_new<CEnemyWeaponTypeFunction>(this);
    m_fpaBaseFunctions[44] = xr_new<CEnemyEquipmentCostFunction>(this);
    m_fpaBaseFunctions[45] = xr_new<CEnemyRukzakWeightFunction>(this);
    m_fpaBaseFunctions[46] = xr_new<CEnemyAnomalityFunction>(this);
    m_fpaBaseFunctions[47] = xr_new<CEnemyEyeRange>(this);
    m_fpaBaseFunctions[48] = xr_new<CEnemyMaxHealth>(this);
    m_fpaBaseFunctions[49] = xr_new<CEnemyAnomalyType>(this);
    m_fpaBaseFunctions[50] = xr_new<CEnemyDistanceToGraphPoint>(this);

    m_pfWeaponEffectiveness = xr_new<CPatternFunction>("common\\WeaponEffectiveness.efd", this);
    xr_new<CPatternFunction>("common\\CreatureEffectiveness.efd", this);
    xr_new<CPatternFunction>("common\\IntCreatureEffectiveness.efd", this);
    xr_new<CPatternFunction>("common\\AccWeaponEffectiveness.efd", this);
    xr_new<CPatternFunction>("common\\FinCreatureEffectiveness.efd", this);
    m_pfVictoryProbability = xr_new<CPatternFunction>("common\\VictoryProbability.efd", this);
    xr_new<CPatternFunction>("common\\EntityCost.efd", this);
    m_pfExpediency = xr_new<CPatternFunction>("common\\Expediency.efd", this);
    xr_new<CPatternFunction>("common\\SurgeDeathProbability.efd", this);
    xr_new<CPatternFunction>("common\\EquipmentValue.efd", this);
    m_pfMainWeaponValue = xr_new<CPatternFunction>("common\\MainWeaponValue.efd", this);
    m_pfSmallWeaponValue = xr_new<CPatternFunction>("common\\SmallWeaponValue.efd", this);
    xr_new<CPatternFunction>("alife\\TerrainType.efd", this);
    xr_new<CPatternFunction>("alife\\WeaponAttackTimes.efd", this);
    xr_new<CPatternFunction>("alife\\WeaponSuccessProbability.efd", this);
    xr_new<CPatternFunction>("alife\\EnemyDetectability.efd", this);
    xr_new<CPatternFunction>("alife\\EnemyDetectProbability.efd", this);
    xr_new<CPatternFunction>("alife\\EnemyRetreatProbability.efd", this);
    xr_new<CPatternFunction>("alife\\AnomalyDetectProbability.efd", this);
    xr_new<CPatternFunction>("alife\\AnomalyInteractProbability.efd", this);
    xr_new<CPatternFunction>("alife\\AnomalyRetreatProbability.efd", this);
    xr_new<CPatternFunction>("alife\\BirthPercentage.efd", this);
    xr_new<CPatternFunction>("alife\\BirthProbability.efd", this);
    xr_new<CPatternFunction>("alife\\BirthSpeed.efd", this);
}

CEF_Storage::~CEF_Storage()
{
    for (int i = 0; i < AI_MAX_EVALUATION_FUNCTION_COUNT; ++i)
        xr_delete(m_fpaBaseFunctions[i]);
}

//CBaseFunction* CEF_Storage::function(LPCSTR function) const
//{
//    for (int i = 0; i < AI_MAX_EVALUATION_FUNCTION_COUNT; ++i)
//    {
//        if (!m_fpaBaseFunctions[i])
//            continue;
//        if (!xr_strcmp(function, m_fpaBaseFunctions[i]->Name()))
//            return (m_fpaBaseFunctions[i]);
//    }
//    return (0);
//}
