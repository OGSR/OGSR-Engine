#pragma once

#include "../../xr_3da/render.h"
#include "../../xrcdb/ispatial.h"

#include "r__dsgraph_types.h"
#include "r__sector.h"
#include "SectorPortalStructure.h"

//////////////////////////////////////////////////////////////////////////
// feedback	for receiving visuals										//
//////////////////////////////////////////////////////////////////////////
class R_feedback
{
public:
    virtual void rfeedback_static(dxRender_Visual* V) = 0;
};

//////////////////////////////////////////////////////////////////////////
// common part of interface implementation for all D3D renderers		//
//////////////////////////////////////////////////////////////////////////
class R_dsgraph_structure
{
public:

    R_feedback* val_feedback{}; // feedback for geometry being rendered
    u32 val_feedback_breakp{}; // breakpoint

    u32 phase{};
    u32 marker{};

    bool pmask[2]; // deferred + forward
    bool pmask_wmark;

    bool main_pass{};

    Fvector render_position{};
    float max_render_distance{-1.f};

public:
    // Dynamic scene graph
    u32 mapNormalCount{};
    R_dsgraph::mapNormalPasses_T mapNormalPasses[2]; // 2==(priority/2)
    u32 mapMatrixCount{};
    R_dsgraph::mapMatrixPasses_T mapMatrixPasses[2];

    R_dsgraph::mapSortedLarge_T mapSorted;
    R_dsgraph::mapSortedLarge_T mapDistort;

    R_dsgraph::mapSorted_T mapHUD;
    R_dsgraph::mapSorted_T mapHUDSorted;
    R_dsgraph::mapSorted_T mapHUDEmissive;

    R_dsgraph::mapSorted_T mapScopeHUD;
    R_dsgraph::mapSorted_T mapScopeHUDSorted, mapScopeHUDSorted2;

    R_dsgraph::mapSorted_T mapWmark; // sorted
    R_dsgraph::mapSorted_T mapEmissive;

    // Runtime structures
    xr_vector<R_dsgraph::_LodItem> lstLODs;

    xr_vector<ISpatial*> lstRenderables;

    xr_vector<int> lstLODgroups;

    CBackend cmd_list{};

    SectorPortalStructure sector_portals_structure;
    
    CPortalTraverser PortalTraverser;

    u32 context_id{CHW::INVALID_CONTEXT_ID};

    u32 counter_S{};

public:
    void set_Feedback(R_feedback* V, u32 id)
    {
        val_feedback = V;
        val_feedback_breakp = id;
    }

    void get_Counters(u32& s) const
    {
        s = counter_S;
    }
    void clear_Counters() { counter_S = 0; }

public:
    R_dsgraph_structure()
    {
        val_feedback = nullptr;
        val_feedback_breakp = 0;
        marker = 0;
        r_pmask(true, true);
    }

    void reset();

    void r_pmask(const bool deferred, const bool forward, const bool wallmarks = false)
    {
        pmask[0] = deferred;
        pmask[1] = forward;
        pmask_wmark = wallmarks;
    }

    void load(const xr_vector<CSector::level_sector_data_t>& sectors, const xr_vector<CPortal::level_portal_data_t>& portals);
    void unload();

    void add_static(dxRender_Visual* pVisual, const CFrustum& view, u32 planes);
    void add_leafs_static(dxRender_Visual* pVisual); // if detected node's full visibility

    void add_leafs_dynamic(IRenderable* root, dxRender_Visual* pVisual, Fmatrix& xform); // if detected node's full visibility

    void r_dsgraph_insert_static(dxRender_Visual* pVisual);
    void r_dsgraph_insert_dynamic(IRenderable* root, dxRender_Visual* pVisual, Fmatrix& xform, Fvector& center);

    void r_dsgraph_render_graph_static(const u32 _priority);
    void r_dsgraph_render_graph_dynamic(u32 _priority);

    void r_dsgraph_render_graph(u32 _priority);
    void r_dsgraph_render_hud();
    void r_dsgraph_render_hud_scope_depth();
    void r_dsgraph_render_hud_ui();
    void r_dsgraph_render_lods();
    void r_dsgraph_render_sorted();
    void r_dsgraph_render_scope_sorted(const bool upscaled);
    void r_dsgraph_render_emissive(bool clear);
    void r_dsgraph_render_wmarks();
    void r_dsgraph_render_distort();

    void build_subspace(const IRender_Sector::sector_id_t& start_sector_id, CFrustum& frustum, const Fmatrix& xform, const Fvector& camera_position, BOOL add_dynamic);
};

extern float r_ssaDISCARD;
extern float r_ssaGLOD_start, r_ssaGLOD_end;

ICF float calcLOD(const float ssa /*fDistSq*/, float R) { return _sqrt(clampr((ssa - r_ssaGLOD_end) / (r_ssaGLOD_start - r_ssaGLOD_end), 0.01f, 1.f)); }
