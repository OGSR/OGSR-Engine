#include "stdafx.h"
#include "artifact.h"
#include "MercuryBall.h"
#include "GraviArtifact.h"
#include "BlackDrops.h"
#include "Needles.h"
#include "BastArtifact.h"
#include "BlackGraviArtifact.h"
#include "DummyArtifact.h"
#include "ZudaArtifact.h"
#include "ThornArtifact.h"
#include "FadedBall.h"
#include "ElectricBall.h"
#include "RustyHairArtifact.h"
#include "GalantineArtifact.h"

using namespace luabind;

#pragma optimize("s", on)
void CArtefact::script_register(lua_State* L)
{
    module(L)[class_<CArtefact, CInventoryItemObject>("CArtefact")
                  .def_readwrite("can_spawn_zone", &CArtefact::m_bCanSpawnZone)

                  .def_readwrite("health_restore_speed", &CArtefact::m_fHealthRestoreSpeed)
                  .def_readwrite("psy_health_restore_speed", &CArtefact::m_fPsyHealthRestoreSpeed)
                  .def_readwrite("radiation_restore_speed", &CArtefact::m_fRadiationRestoreSpeed)
                  .def_readwrite("satiety_restore_speed", &CArtefact::m_fSatietyRestoreSpeed)
                  .def_readwrite("power_restore_speed", &CArtefact::m_fPowerRestoreSpeed)
                  .def_readwrite("bleeding_restore_speed", &CArtefact::m_fBleedingRestoreSpeed)
                  .def_readwrite("thirst_restore_speed", &CArtefact::m_fThirstRestoreSpeed)

                  .def_readwrite("additional_inventory_weight", &CArtefact::m_additional_weight)
                  .def_readwrite("additional_inventory_weight2", &CArtefact::m_additional_weight2),

              class_<CMercuryBall, CGameObject>("CMercuryBall").def(constructor<>()), class_<CBlackDrops, CGameObject>("CBlackDrops").def(constructor<>()),
              class_<CBlackGraviArtefact, CGameObject>("CBlackGraviArtefact").def(constructor<>()), class_<CBastArtefact, CGameObject>("CBastArtefact").def(constructor<>()),
              class_<CDummyArtefact, CGameObject>("CDummyArtefact").def(constructor<>()), class_<CZudaArtefact, CGameObject>("CZudaArtefact").def(constructor<>()),
              class_<CThornArtefact, CGameObject>("CThornArtefact").def(constructor<>()), class_<CFadedBall, CGameObject>("CFadedBall").def(constructor<>()),
              class_<CElectricBall, CGameObject>("CElectricBall").def(constructor<>()), class_<CRustyHairArtefact, CGameObject>("CRustyHairArtefact").def(constructor<>()),
              class_<CGalantineArtefact, CGameObject>("CGalantineArtefact").def(constructor<>()), class_<CGraviArtefact, CGameObject>("CGraviArtefact").def(constructor<>())];
}
