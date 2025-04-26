#include "stdafx.h"

#include "fhierrarhyvisual.h"
#include "SkeletonCustom.h"
#include "../../xr_3da/fmesh.h"
#include "../../xr_3da/irenderable.h"
#include "../../xr_3da/XR_IOConsole.h"
#include "../../xr_3da/xr_ioc_cmd.h"

#include "flod.h"
#include "particlegroup.h"
#include "FTreeVisual.h"

using namespace R_dsgraph;

//u32 mapNormalItems::instance_cnt = 0;
//u32 mapMatrixItems::instance_cnt = 0;

// Static geometry optimization
#define O_S_L1_S_LOW 10.f // geometry 3d volume size
#define O_S_L1_D_LOW 150.f // distance, after which it is not rendered
#define O_S_L2_S_LOW 100.f
#define O_S_L2_D_LOW 200.f
#define O_S_L3_S_LOW 500.f
#define O_S_L3_D_LOW 250.f
#define O_S_L4_S_LOW 2500.f
#define O_S_L4_D_LOW 350.f
#define O_S_L5_S_LOW 7000.f
#define O_S_L5_D_LOW 400.f

#define O_S_L1_S_MED 25.f
#define O_S_L1_D_MED 50.f
#define O_S_L2_S_MED 200.f
#define O_S_L2_D_MED 150.f
#define O_S_L3_S_MED 1000.f
#define O_S_L3_D_MED 200.f
#define O_S_L4_S_MED 2500.f
#define O_S_L4_D_MED 300.f
#define O_S_L5_S_MED 7000.f
#define O_S_L5_D_MED 400.f

#define O_S_L1_S_HII 50.f
#define O_S_L1_D_HII 50.f
#define O_S_L2_S_HII 400.f
#define O_S_L2_D_HII 150.f
#define O_S_L3_S_HII 1500.f
#define O_S_L3_D_HII 200.f
#define O_S_L4_S_HII 5000.f
#define O_S_L4_D_HII 300.f
#define O_S_L5_S_HII 20000.f
#define O_S_L5_D_HII 350.f

#define O_S_L1_S_ULT 50.f
#define O_S_L1_D_ULT 35.f
#define O_S_L2_S_ULT 500.f
#define O_S_L2_D_ULT 125.f
#define O_S_L3_S_ULT 1750.f
#define O_S_L3_D_ULT 175.f
#define O_S_L4_S_ULT 5250.f
#define O_S_L4_D_ULT 250.f
#define O_S_L5_S_ULT 25000.f
#define O_S_L5_D_ULT 300.f


constexpr Fvector4 o_optimize_static_l1_dist{O_S_L1_D_LOW, O_S_L1_D_MED, O_S_L1_D_HII, O_S_L1_D_ULT};
constexpr Fvector4 o_optimize_static_l1_size{O_S_L1_S_LOW, O_S_L1_S_MED, O_S_L1_S_HII, O_S_L1_S_ULT};
constexpr Fvector4 o_optimize_static_l2_dist{O_S_L2_D_LOW, O_S_L2_D_MED, O_S_L2_D_HII, O_S_L2_D_ULT};
constexpr Fvector4 o_optimize_static_l2_size{O_S_L2_S_LOW, O_S_L2_S_MED, O_S_L2_S_HII, O_S_L2_S_ULT};
constexpr Fvector4 o_optimize_static_l3_dist{O_S_L3_D_LOW, O_S_L3_D_MED, O_S_L3_D_HII, O_S_L3_D_ULT};
constexpr Fvector4 o_optimize_static_l3_size{O_S_L3_S_LOW, O_S_L3_S_MED, O_S_L3_S_HII, O_S_L3_S_ULT};
constexpr Fvector4 o_optimize_static_l4_dist{O_S_L4_D_LOW, O_S_L4_D_MED, O_S_L4_D_HII, O_S_L4_D_ULT};
constexpr Fvector4 o_optimize_static_l4_size{O_S_L4_S_LOW, O_S_L4_S_MED, O_S_L4_S_HII, O_S_L4_S_ULT};
constexpr Fvector4 o_optimize_static_l5_dist{O_S_L5_D_LOW, O_S_L5_D_MED, O_S_L5_D_HII, O_S_L5_D_ULT};
constexpr Fvector4 o_optimize_static_l5_size{O_S_L5_S_LOW, O_S_L5_S_MED, O_S_L5_S_HII, O_S_L5_S_ULT};

#define BASE_FOV 67.f

extern float ps_r__LOD_k;

// Aproximate, adjusted by fov, distance from camera to position (For right work when looking though binoculars and scopes)

IC float GetDistFromCamera(const Fvector& from_position)
{
    const float distance = Device.vCameraPosition.distance_to(from_position);
    const float fov_K = BASE_FOV / Device.fFOV;
    const float adjusted_distane = distance / fov_K;

    return adjusted_distane;
}

IC bool IsValuableToRender(dxRender_Visual* pVisual, bool sm, bool ignore_optimize = false)
{
    if (ignore_optimize)
        return true;

    if (opt_static_geom >= 1)
    {
        const float sphere_volume = pVisual->getVisData().sphere.volume();

        const float adjusted_distance = GetDistFromCamera(pVisual->getVisData().sphere.P);

        if (sm && opt_shadow_geom) // Highest cut off for shadow map
        {
            if (sphere_volume < 50000.f && adjusted_distance > ps_ssfx_shadow_cascades.z)
                // don't need geometry behind the farest sun shadow cascade
                return false;

            if ((sphere_volume < o_optimize_static_l1_size.z) && (adjusted_distance > o_optimize_static_l1_dist.z))
                return false;
            else if ((sphere_volume < o_optimize_static_l2_size.z) && (adjusted_distance > o_optimize_static_l2_dist.z))
                return false;
            else if ((sphere_volume < o_optimize_static_l3_size.z) && (adjusted_distance > o_optimize_static_l3_dist.z))
                return false;
            else if ((sphere_volume < o_optimize_static_l4_size.z) && (adjusted_distance > o_optimize_static_l4_dist.z))
                return false;
            else if ((sphere_volume < o_optimize_static_l5_size.z) && (adjusted_distance > o_optimize_static_l5_dist.z))
                return false;

            return true;
        }

        {
            if (pVisual->Type == MT_LOD || pVisual->Type == MT_TREE_PM || pVisual->Type == MT_TREE_ST)
                return true;

            if (opt_static_geom == 2)
            {
                if ((sphere_volume < o_optimize_static_l1_size.y) && (adjusted_distance > o_optimize_static_l1_dist.y))
                    return false;
                else if ((sphere_volume < o_optimize_static_l2_size.y) && (adjusted_distance > o_optimize_static_l2_dist.y))
                    return false;
                else if ((sphere_volume < o_optimize_static_l3_size.y) && (adjusted_distance > o_optimize_static_l3_dist.y))
                    return false;
                else if ((sphere_volume < o_optimize_static_l4_size.y) && (adjusted_distance > o_optimize_static_l4_dist.y))
                    return false;
                else if ((sphere_volume < o_optimize_static_l5_size.y) && (adjusted_distance > o_optimize_static_l5_dist.y))
                    return false;
            }
            else if (opt_static_geom == 3)
            {
                if ((sphere_volume < o_optimize_static_l1_size.z) && (adjusted_distance > o_optimize_static_l1_dist.z))
                    return false;
                else if ((sphere_volume < o_optimize_static_l2_size.z) && (adjusted_distance > o_optimize_static_l2_dist.z))
                    return false;
                else if ((sphere_volume < o_optimize_static_l3_size.z) && (adjusted_distance > o_optimize_static_l3_dist.z))
                    return false;
                else if ((sphere_volume < o_optimize_static_l4_size.z) && (adjusted_distance > o_optimize_static_l4_dist.z))
                    return false;
                else if ((sphere_volume < o_optimize_static_l5_size.z) && (adjusted_distance > o_optimize_static_l5_dist.z))
                    return false;
            }
            else if (opt_static_geom == 4)
            {
                if ((sphere_volume < o_optimize_static_l1_size.w) && (adjusted_distance > o_optimize_static_l1_dist.w))
                    return false;
                else if ((sphere_volume < o_optimize_static_l2_size.w) && (adjusted_distance > o_optimize_static_l2_dist.w))
                    return false;
                else if ((sphere_volume < o_optimize_static_l3_size.w) && (adjusted_distance > o_optimize_static_l3_dist.w))
                    return false;
                else if ((sphere_volume < o_optimize_static_l4_size.w) && (adjusted_distance > o_optimize_static_l4_dist.w))
                    return false;
                else if ((sphere_volume < o_optimize_static_l5_size.w) && (adjusted_distance > o_optimize_static_l5_dist.w))
                    return false;
            }
            else
            {
                if ((sphere_volume < o_optimize_static_l1_size.x) && (adjusted_distance > o_optimize_static_l1_dist.x))
                    return false;
                else if ((sphere_volume < o_optimize_static_l2_size.x) && (adjusted_distance > o_optimize_static_l2_dist.x))
                    return false;
                else if ((sphere_volume < o_optimize_static_l3_size.x) && (adjusted_distance > o_optimize_static_l3_dist.x))
                    return false;
                else if ((sphere_volume < o_optimize_static_l4_size.x) && (adjusted_distance > o_optimize_static_l4_dist.x))
                    return false;
                else if ((sphere_volume < o_optimize_static_l5_size.x) && (adjusted_distance > o_optimize_static_l5_dist.x))
                    return false;
            }
        }
    }

    return true;
}

float r_ssaDISCARD;
float r_ssaLOD_A, r_ssaLOD_B;
float r_ssaGLOD_start, r_ssaGLOD_end;

ICF float CalcSSA(float& distSQ, Fvector& C, dxRender_Visual* V)
{
    const float R = V->getVisData().sphere.R + 0;
    distSQ = Device.vCameraPosition.distance_to_sqr(C) + EPS;
    return R / distSQ;
}
ICF float CalcSSA(float& distSQ, Fvector& C, float R)
{
    distSQ = Device.vCameraPosition.distance_to_sqr(C) + EPS;
    return R / distSQ;
}
ICF float CalcHudSSA(float& distSQ, Fvector& C, dxRender_Visual* V)
{
    const float R = V->getVisData().sphere.R + 0;
    distSQ = Fvector().set(0.f, 0.f, 0.f).distance_to_sqr(C) + EPS;
    return R / distSQ;
}

void R_dsgraph_structure::r_dsgraph_insert_dynamic(IRenderable* root, dxRender_Visual* pVisual, Fmatrix& xform, Fvector& center)
{
    if (pVisual->getVisData().marker[context_id] == marker)
        return;
    pVisual->getVisData().marker[context_id] = marker;

    ZoneScoped;

    float distSQ;
    float SSA;
    if (root && root->renderable_HUD())
        SSA = CalcHudSSA(distSQ, center, pVisual);
    else
        SSA = CalcSSA(distSQ, center, pVisual);

    if (SSA <= r_ssaDISCARD)
        return;

    // Distortive geometry should be marked and R2 special-cases it
    // a) Allow to optimize RT order
    // b) Should be rendered to special distort buffer in another pass
    VERIFY(pVisual->shader._get());
    ShaderElement* sh_d = &*pVisual->shader->E[4];
    if (RImplementation.o.distortion && sh_d && sh_d->flags.bDistort && pmask[sh_d->flags.iPriority / 2])
    {
        mapDistort.insert_anyway(distSQ, _MatrixItemS({SSA, root, pVisual, xform, sh_d}));
    }

    // Select shader
    ShaderElement* sh = RImplementation.rimp_select_sh_dynamic(pVisual, distSQ, root ? root->renderable_HUD() : false, phase);
    if (nullptr == sh)
        return;
    if (!pmask[sh->flags.iPriority / 2])
        return;

    // HUD rendering
    if (root && root->renderable_HUD())
    {
        extern bool Is3dssZoomed;
        if (sh->flags.iScopeLense)
        {
            if (Is3dssZoomed)
            {
                mapHUD.insert_anyway(distSQ, _MatrixItemS({SSA, root, pVisual, xform, &*pVisual->shader->E[0]}));
                mapScopeHUD.insert_anyway(distSQ, _MatrixItemS({SSA, root, pVisual, xform, &*pVisual->shader->E[1]}));
                if (ps_r_pp_aa_mode == DLSS || ps_r_pp_aa_mode == FSR2 || ps_r_pp_aa_mode == TAA || ps_r2_ls_flags.test(R2FLAG_DBG_TAA_JITTER_ENABLE))
                {
                    mapScopeHUDSorted.insert_anyway(distSQ, _MatrixItemS({SSA, root, pVisual, xform, &*pVisual->shader->E[0]}));
                    mapScopeHUDSorted.insert_anyway(distSQ, _MatrixItemS({SSA, root, pVisual, xform, &*pVisual->shader->E[1]}));
                    mapScopeHUDSorted2.insert_anyway(distSQ, _MatrixItemS({SSA, root, pVisual, xform, &*pVisual->shader->E[0]}));
                    mapScopeHUDSorted2.insert_anyway(distSQ, _MatrixItemS({SSA, root, pVisual, xform, &*pVisual->shader->E[1]}));
                }
                mapScopeHUDSorted.insert_anyway(distSQ, _MatrixItemS({SSA, root, pVisual, xform, &*pVisual->shader->E[2]}));
                mapScopeHUDSorted2.insert_anyway(distSQ, _MatrixItemS({SSA, root, pVisual, xform, &*pVisual->shader->E[4]}));
            }
            else
            {
                mapHUD.insert_anyway(distSQ, _MatrixItemS({SSA, root, pVisual, xform, &*pVisual->shader->E[3]}));
            }
            return;
        }

        if (sh->flags.bStrictB2F && !Is3dssZoomed)
        {
            mapHUDSorted.insert_anyway(distSQ, _MatrixItemS({SSA, root, pVisual, xform, sh}));
            return;
        }

        mapHUD.insert_anyway(distSQ, _MatrixItemS({SSA, root, pVisual, xform, sh}));

        if (sh->flags.bEmissive && sh_d && !Is3dssZoomed)
        {
            mapHUDEmissive.insert_anyway(distSQ, _MatrixItemS({SSA, root, pVisual, xform, sh_d}));
        }
        return;
    }

    // Shadows registering

    if (root && root->renderable_Invisible())
        return;

    // strict-sorting selection
    if (sh->flags.bStrictB2F)
    {
        mapSorted.insert_anyway(distSQ, _MatrixItemS({SSA, root, pVisual, xform, sh}));
        return;
    }

    // Emissive geometry should be marked and R2 special-cases it
    // a) Allow to skeep already lit pixels
    // b) Allow to make them 100% lit and really bright
    // c) Should not cast shadows
    // d) Should be rendered to accumulation buffer in the second pass
    if (sh->flags.bEmissive)
    {
        mapEmissive.insert_anyway(distSQ, _MatrixItemS({SSA, root, pVisual, xform, sh_d}));
    }
    if (sh->flags.bWmark && pmask_wmark)
    {
        mapWmark.insert_anyway(distSQ, _MatrixItemS({SSA, root, pVisual, xform, sh}));
        return;
    }

    ASSERT_FMT(sh->passes.size() <= SHADER_PASSES_MAX, "max pass should be <= 2! shader [%s]", pVisual->shader->dbg_shader_name.c_str());

    for (u32 iPass = 0; iPass < sh->passes.size(); ++iPass)
    {
        SPass* pass = sh->passes[iPass]._get();
        mapMatrix_T& map = mapMatrixPasses[sh->flags.iPriority / 2][iPass];
        mapMatrixItems& matrixItems = map[pass];

        matrixItems.items->emplace_back(_MatrixItem{SSA, root, pVisual, xform});

        // Need to sort for HZB efficient use
        if (SSA > matrixItems.ssa)
        {
            matrixItems.ssa = SSA;
        }
    }
}

void R_dsgraph_structure::r_dsgraph_insert_static(dxRender_Visual* pVisual)
{
    if (pVisual->getVisData().marker[context_id] == marker)
        return;
    pVisual->getVisData().marker[context_id] = marker;

    ZoneScoped;

    float distSQ;
    const float SSA = CalcSSA(distSQ, pVisual->getVisData().sphere.P, pVisual);
    if (SSA <= r_ssaDISCARD)
        return;

    // Distortive geometry should be marked and R2 special-cases it
    // a) Allow to optimize RT order
    // b) Should be rendered to special distort buffer in another pass
    VERIFY(pVisual->shader._get());
    const Shader* vis_sh = pVisual->shader._get();
    ShaderElement* sh_d = vis_sh ? vis_sh->E[4]._get() : nullptr; // 4=L_special
    if (RImplementation.o.distortion && sh_d && sh_d->flags.bDistort && pmask[sh_d->flags.iPriority / 2])
    {
        mapDistort.insert_anyway(distSQ, _MatrixItemS({SSA, nullptr, pVisual, Fidentity, sh_d}));
    }

    // Select shader
    ShaderElement* sh = RImplementation.rimp_select_sh_static(pVisual, distSQ, phase);
    if (nullptr == sh)
        return;
    if (!pmask[sh->flags.iPriority / 2])
        return;

    // strict-sorting selection
    if (sh->flags.bStrictB2F)
    {
        mapSorted.insert_anyway(distSQ, _MatrixItemS({/*0*/ SSA, nullptr, pVisual, Fidentity, sh}));
        return;
    }

    // Emissive geometry should be marked and R2 special-cases it
    // a) Allow to skeep already lit pixels
    // b) Allow to make them 100% lit and really bright
    // c) Should not cast shadows
    // d) Should be rendered to accumulation buffer in the second pass
    if (sh->flags.bEmissive)
    {
        mapEmissive.insert_anyway(distSQ, _MatrixItemS({SSA, nullptr, pVisual, Fidentity, sh_d})); // sh_d -> L_special
    }
    if (sh->flags.bWmark && pmask_wmark)
    {
        mapWmark.insert_anyway(distSQ, _MatrixItemS({SSA, nullptr, pVisual, Fidentity, sh}));
        return;
    }

    if (val_feedback && counter_S == val_feedback_breakp)
        val_feedback->rfeedback_static(pVisual);

    counter_S++;

    {
        ZoneScopedN("mapNormalPasses");

        ASSERT_FMT(sh->passes.size() <= SHADER_PASSES_MAX, "max pass should be <= 2! shader [%s]", pVisual->shader->dbg_shader_name.c_str());

        for (u32 iPass = 0; iPass < sh->passes.size(); ++iPass)
        {
            SPass* pass = sh->passes[iPass]._get();
            mapNormal_T& map = mapNormalPasses[sh->flags.iPriority / 2][iPass];
            mapNormalItems& normalItems = map[pass];

            if (pVisual->base_crc)
            {
                const float lod = calcLOD(SSA, pVisual->getVisData().sphere.R);
                pVisual->select_lod_id(clampr(1.f - (1.f - lod) * ps_r__LOD_k, 0.01f, 1.f), context_id);

                if (auto it = normalItems.trees->find(pVisual->crc[context_id]); it != normalItems.trees->end())
                {
                    it->second.data.push_back(&pVisual->tree_data);
                }
                else
                {
                    normalItems.trees->emplace(pVisual->crc[context_id], _TreeItem{pVisual, {&pVisual->tree_data}});
                }
            }
            else
            {
                normalItems.items->emplace_back(_NormalItem{SSA, pVisual});
            }

            // Need to sort for HZB efficient use
            if (SSA > normalItems.ssa)
            {
                normalItems.ssa = SSA;
            }
        }
    }
}

void R_dsgraph_structure::add_leafs_dynamic(IRenderable* root, dxRender_Visual* pVisual, Fmatrix& xform)
{
    if (!pVisual)
        return;

    ZoneScoped;

    switch (pVisual->Type)
    {
    case MT_PARTICLE_GROUP: {
        if (phase == CRender::PHASE_NORMAL)
        {
            // Add all children, doesn't perform any tests
            const PS::CParticleGroup* pG = (PS::CParticleGroup*)pVisual;
            for (auto& I : pG->items)
            {
                if (I._effect)
                    add_leafs_dynamic(root, I._effect, xform);
                for (const auto& pit : I._children_related)
                    add_leafs_dynamic(root, pit, xform);
                for (const auto& pit : I._children_free)
                    add_leafs_dynamic(root, pit, xform);
            }
        }
    }
        return;
    case MT_HIERRARHY: {
        for (dxRender_Visual* Vis : dynamic_cast<FHierrarhyVisual*>(pVisual)->children)
            if (Vis->getRZFlag())
                add_leafs_dynamic(root, Vis, xform);
    }
        return;
    case MT_SKELETON_ANIM:
    case MT_SKELETON_RIGID: {
        const auto pV = dynamic_cast<CKinematics*>(pVisual);
        BOOL _use_lod = FALSE;
        if (pV->m_lod)
        {
            Fvector Tpos;
            float D;
            xform.transform_tiny(Tpos, pV->getVisData().sphere.P);
            const float ssa = CalcSSA(D, Tpos, pV->getVisData().sphere.R / 2.f); // assume dynamics never consume full sphere
            if (ssa < r_ssaLOD_A)
                _use_lod = TRUE;
        }
        if (_use_lod)
        {
            add_leafs_dynamic(root, pV->m_lod, xform);
        }
        else
        {
            if (phase == CRender::PHASE_NORMAL)
            {
                pV->CalculateBones(TRUE);
                pV->CalculateWallmarks(root ? root->renderable_HUD() : false); //. bug?
            }

            for (dxRender_Visual* Vis : pV->children)
                if (Vis->getRZFlag())
                    add_leafs_dynamic(root, Vis, xform);
        }
    }
        return;
    default: {
        // General type of visual
        // Calculate distance to it's center
        Fvector Tpos;
        xform.transform_tiny(Tpos, pVisual->getVisData().sphere.P);
        r_dsgraph_insert_dynamic(root, pVisual, xform, Tpos);
    }
        return;
    }
}

void R_dsgraph_structure::add_leafs_static(dxRender_Visual* pVisual)
{
    ZoneScoped;

    if (!RImplementation.HOM.visible(pVisual->getVisData()))
    {
        //Msg("add_leafs_static skip static model");
        return;
    }

    if (!pVisual->ignore_optimization && !IsValuableToRender(pVisual, phase == CRender::PHASE_SMAP))
        return;

    switch (pVisual->Type)
    {
    case MT_PARTICLE_GROUP: {
        // Xottab_DUTY: for dynamic objects we need matrix,
        // which is nullptr, when we use add_leafs_static
        Log("Dynamic particles added via static procedure.");
        NODEFAULT;
    }
        return;
    case MT_HIERRARHY: {
        for (dxRender_Visual* Vis : dynamic_cast<FHierrarhyVisual*>(pVisual)->children)
            if (Vis->getRZFlag())
                add_leafs_static(Vis);
    }
        return;
    case MT_SKELETON_ANIM:
    case MT_SKELETON_RIGID: {
        R_ASSERT(false, "SKELETON in static !");
    }
        return;
    case MT_LOD: {
        if (ps_r2_ls_flags_ext.test(R2FLAGEXT_DISABLE_STATIC_LOD))
        {
            return;
        }

        const auto pV = dynamic_cast<FLOD*>(pVisual);
        float D;
        float ssa = CalcSSA(D, pV->getVisData().sphere.P, pV) * pV->lod_factor;

        if (ssa < r_ssaLOD_A && phase != CRender::PHASE_SMAP)
        {
            mapLOD.insert_anyway(D, _LodItem({ssa, pVisual}));
        }

        //if (ssa < r_ssaDISCARD)
        //    return;

        if (ssa > r_ssaLOD_B || phase == CRender::PHASE_SMAP)
        {
            // Add all children, doesn't perform any tests
            for (dxRender_Visual* Vis : pV->children)
                add_leafs_static(Vis);
        }
    }
        return;
    default: {
        if (max_render_distance > 0.f) // check used only for light render
        {
            if (render_position.distance_to(pVisual->getVisData().sphere.P) - pVisual->getVisData().sphere.R / 2 > max_render_distance)
                return;
        }

        bool is_normal = pVisual->Type == MT_NORMAL;
        bool is_tree = pVisual->Type == MT_TREE_ST;
        bool is_tree_progressive = pVisual->Type == MT_TREE_PM;
        bool is_progressive = pVisual->Type == MT_PROGRESSIVE;

        if (is_normal && ps_r2_ls_flags_ext.test(R2FLAGEXT_DISABLE_STATIC_NORMAL))
        {
            return;
        }

        if (is_tree && ps_r2_ls_flags_ext.test(R2FLAGEXT_DISABLE_STATIC_TREE))
        {
            return;
        }

        if (is_tree_progressive && ps_r2_ls_flags_ext.test(R2FLAGEXT_DISABLE_STATIC_TREE_PROGRESSIVE))
        {
            return;
        }

        if (is_progressive && ps_r2_ls_flags_ext.test(R2FLAGEXT_DISABLE_STATIC_PROGRESSIVE))
        {
            return;
        }

        // General type of visual
        r_dsgraph_insert_static(pVisual);
    }
    }
}

void R_dsgraph_structure::add_static(dxRender_Visual* pVisual, const CFrustum& view, u32 planes)
{
    ZoneScoped;

    vis_data& vis = pVisual->getVisData();

    // Check frustum visibility and calculate distance to visual's center
    const EFC_Visible VIS = view.testSAABB(vis.sphere.P, vis.sphere.R, vis.box.data(), planes);
    if (fcvNone == VIS)
        return;

    // If we get here visual is visible or partially visible

    switch (pVisual->Type)
    {
    case MT_HIERRARHY: {
        for (dxRender_Visual* Vis : dynamic_cast<FHierrarhyVisual*>(pVisual)->children)
        {
            if (Vis->getRZFlag())
            {
                if (fcvPartial == VIS)
                {
                    add_static(Vis, view, planes);
                }
                else
                {
                    add_leafs_static(Vis);
                }
            }
        }
    }
    break;
    default: {
        add_leafs_static(pVisual);
    }
    break;
    }
}

void R_dsgraph_structure::load(const xr_vector<CSector::level_sector_data_t>& sectors_data, const xr_vector<CPortal::level_portal_data_t>& portals_data)
{
    const auto portals_count = portals_data.size();
    const auto sectors_count = sectors_data.size();

    Sectors.resize(sectors_count);
    Portals.resize(portals_count);

    for (int idx = 0; idx < portals_count; ++idx)
    {
        auto* portal = xr_new<CPortal>();
        Portals[idx] = portal;
    }

    for (int idx = 0; idx < sectors_count; ++idx)
    {
        auto* sector = xr_new<CSector>();

        sector->unique_id = static_cast<IRender_Sector::sector_id_t>(idx);
        sector->setup(sectors_data[idx], Portals);
        Sectors[idx] = sector;
    }

    for (int idx = 0; idx < portals_count; ++idx)
    {
        auto* portal = static_cast<CPortal*>(Portals[idx]);
        portal->setup(portals_data[idx], Sectors);
    }
}

void R_dsgraph_structure::unload()
{
    for (auto* sector : Sectors)
        xr_delete(sector);
    Sectors.clear();

    for (auto* portal : Portals)
        xr_delete(portal);
    Portals.clear();
}

IRender_Sector::sector_id_t R_dsgraph_structure::detect_sector(const Fvector& P)
{
    Fvector dir{0, -1, 0};
    auto sector = detect_sector(P, dir);
    if (sector == IRender_Sector::INVALID_SECTOR_ID)
    {
        dir = {0, 1, 0};
        sector = detect_sector(P, dir);
    }
    return sector;
}

#pragma optimize("", off)

IRender_Sector::sector_id_t R_dsgraph_structure::detect_sector(const Fvector& P, Fvector& dir)
{
    // Portals model
    int id1 = -1;
    float range1 = 500.f;
    if (RImplementation.rmPortals)
    {
        Sectors_xrc.ray_query(CDB::OPT_ONLYNEAREST, RImplementation.rmPortals, P, dir, range1);
        if (Sectors_xrc.r_count())
        {
            const CDB::RESULT* RP1 = Sectors_xrc.r_begin();
            id1 = RP1->id;
            range1 = RP1->range;
        }
    }

    // Geometry model
    int id2 = -1;
    float range2 = range1;
    Sectors_xrc.ray_query(CDB::OPT_ONLYNEAREST, g_pGameLevel->ObjectSpace.GetStaticModel(), P, dir, range2);
    if (Sectors_xrc.r_count())
    {
        const CDB::RESULT* RP2 = Sectors_xrc.r_begin();
        id2 = RP2->id;
        range2 = RP2->range;
    }

    // Select ID
    int ID{};
    if (id1 >= 0)
    {
        if (id2 >= 0)
            ID = (range1 <= range2 + EPS_L) ? id1 : id2; // both was found
        else
            ID = id1; // only id1 found
    }
    else if (id2 >= 0)
        ID = id2; // only id2 found
    else
        return IRender_Sector::INVALID_SECTOR_ID;

    if (ID == id1)
    {
        __try
        {
            // Take sector, facing to our point from portal
            const CDB::TRI* pTri = RImplementation.rmPortals->get_tris() + ID;
            if (!pTri)
            {
                Msg("!![%s] nullptr pTri detected! tris ID: [%d]", __FUNCTION__, ID);
                return IRender_Sector::INVALID_SECTOR_ID;
            }

            const CPortal* pPortal = Portals.at(pTri->dummy);
            if (!pPortal)
            {
                Msg("!![%s] nullptr pPortal detected! tris ID: [%d], pTri->dummy: [%u]", __FUNCTION__, ID, pTri->dummy);
                return IRender_Sector::INVALID_SECTOR_ID;
            }

            return pPortal->getSectorFacing(P)->unique_id;
        }
        __except (ExceptStackTrace("Exception catched in " __FUNCTION__))
        {
            Msg("!![%s] possible bad tris ID: [%d]", __FUNCTION__, ID);

            { // Отключаем сектора до перезапуска двига без сохранения этого отключения в юзере
                auto it = Console->Commands.find("r2_disable_sectors");
                if (it != Console->Commands.end())
                {
                    auto* cmd = it->second;
                    cmd->SetCanSave(false);
                    cmd->Execute("on");
                }
            }

            return IRender_Sector::INVALID_SECTOR_ID;
        }
    }

    // Take triangle at ID and use it's Sector
    const CDB::TRI* pTri = g_pGameLevel->ObjectSpace.GetStaticTris() + ID;
    return pTri->sector;
}

void R_dsgraph_structure::update_sector(ISpatial* S)
{
    if (S->spatial.type & STYPEFLAG_INVALIDSECTOR)
    {
        const auto entity_pos = S->spatial_sector_point();
        const auto sector_id = detect_sector(entity_pos);
        S->spatial_updatesector(sector_id);
    }
}

#pragma optimize("", on)
