#include "stdafx.h"
#include "script_game_object.h"
#include "GameObject.h"
#include "..\xrcdb\xr_collide_defs.h"
#include "..\xr_3da\GameMtlLib.h"
#include "..\Include/xrRender/Kinematics.h"
#include "Level.h"
#include "script_rq_result.h"

void script_rq_result::set_result(collide::rq_result _res)
{
    range = _res.range;
    element = _res.element;
    result = true;
    if (_res.O)
    {
        CGameObject* obj = smart_cast<CGameObject*>(_res.O);
        if (obj)
            object = obj->lua_game_object();

        const auto pK = smart_cast<IKinematics*>(_res.O->Visual());
        if (pK)
        {
            const auto& bone_data = pK->LL_GetData((u16)_res.element);
            if (bone_data.game_mtl_idx < GMLib.CountMaterial())
            {
                mtl = GMLib.GetMaterialByIdx(bone_data.game_mtl_idx);
            }
        }
    }
    else
    {
        CDB::TRI* T = Level().ObjectSpace.GetStaticTris() + element;
        if (T->material < GMLib.CountMaterial())
        {
            mtl = GMLib.GetMaterialByIdx(T->material);
        }
    }
}

LPCSTR get_name(const SGameMtl* self) { return (*self->m_Name); }

LPCSTR get_desc(const SGameMtl* self) { return (*self->m_Desc); }

using namespace luabind;

#pragma optimize("s", on)

void script_rq_result::script_register(lua_State* L)
{
    module(
        L)[class_<script_rq_result>("rq_result")
               .def_readonly("range", &script_rq_result::range)
               .def_readonly("object", &script_rq_result::object)
               .def_readonly("element", &script_rq_result::element)
               .def_readonly("result", &script_rq_result::result)
               .def_readonly("mtl", &script_rq_result::mtl),

           class_<enum_exporter<SGameMtl>>("SGameMtlFlags")
               .enum_("SGameMtlFlags")[value("flActorObstacle", int(SGameMtl::flActorObstacle)), value("flBloodmark", int(SGameMtl::flBloodmark)),
                                       value("flBounceable", int(SGameMtl::flBounceable)), value("flClimable", int(SGameMtl::flClimable)),
                                       value("flDynamic", int(SGameMtl::flDynamic)), value("flInjurious", int(SGameMtl::flInjurious)), value("flLiquid", int(SGameMtl::flLiquid)),
                                       value("flPassable", int(SGameMtl::flPassable)), value("flSlowDown", int(SGameMtl::flSlowDown)),
                                       value("flSuppressShadows", int(SGameMtl::flSuppressShadows)), value("flSuppressWallmarks", int(SGameMtl::flSuppressWallmarks))],

           class_<SGameMtl>("SGameMtl")
               .property("m_Name", &get_name)
               .property("m_Desc", &get_desc)
               .def_readonly("Flags", &SGameMtl::Flags)
               .def_readonly("fPHFriction", &SGameMtl::fPHFriction)
               .def_readonly("fPHDamping", &SGameMtl::fPHDamping)
               .def_readonly("fPHSpring", &SGameMtl::fPHSpring)
               .def_readonly("fPHBounceStartVelocity", &SGameMtl::fPHBounceStartVelocity)
               .def_readonly("fPHBouncing", &SGameMtl::fPHBouncing)
               .def_readonly("fFlotationFactor", &SGameMtl::fFlotationFactor)
               .def_readonly("fShootFactor", &SGameMtl::fShootFactor)
               .def_readonly("fBounceDamageFactor", &SGameMtl::fBounceDamageFactor)
               .def_readonly("fInjuriousSpeed", &SGameMtl::fInjuriousSpeed)
               .def_readonly("fVisTransparencyFactor", &SGameMtl::fVisTransparencyFactor)
               .def_readonly("fSndOcclusionFactor", &SGameMtl::fSndOcclusionFactor)];
}
