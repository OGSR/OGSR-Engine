#include "stdafx.h"
#include "flod.h"

#include "../../xr_3da/igame_persistent.h"
#include "../../xr_3da/environment.h"

extern float r_ssaLOD_A;
extern float r_ssaLOD_B;

ICF bool static pred_dot(const std::pair<float, u32>& _1, const std::pair<float, u32>& _2) { return _1.first < _2.first; }

void R_dsgraph_structure::r_dsgraph_render_lods()
{
    ZoneScoped;

    PIX_EVENT_CTX(cmd_list, dsgraph_render_lods);

    if (lstLODs.empty())
        return;

    // *** Fill VB and generate groups
    const u32 shid = SE_R1_LMODELS;
    FLOD* firstV = smart_cast<FLOD*>(lstLODs[0].pVisual);
    ref_selement cur_S = firstV->shader->E[shid];
    float ssaRange = r_ssaLOD_A - r_ssaLOD_B;
    if (ssaRange < EPS_S)
        ssaRange = EPS_S;

    const u32 uiVertexPerImposter = 4;
    const u32 uiImpostersFit = RImplementation.Vertex.GetSize() / (firstV->geom->vb_stride * uiVertexPerImposter);

    // Msg("dbg_lods: shid[%d],firstV[%X]",shid,u32((void*)firstV));
    // Msg("dbg_lods: shader[%X]",u32((void*)firstV->shader._get()));
    // Msg("dbg_lods: shader_E[%X]",u32((void*)cur_S._get()));

    for (u32 i = 0; i < lstLODs.size(); i++)
    {
        const u32 iBatchSize = _min(u32(lstLODs.size()) - i, uiImpostersFit);
        int cur_count = 0;
        u32 vOffset;
        FLOD::_hw* V = (FLOD::_hw*)RImplementation.Vertex.Lock(iBatchSize * uiVertexPerImposter, firstV->geom->vb_stride, vOffset);

        for (u32 j = 0; j < iBatchSize; ++j, ++i)
        {
            // sort out redundancy
            const R_dsgraph::_LodItem& P = lstLODs[i];
            if (!P.pVisual)
                continue;

            if (P.pVisual->shader->E[shid] == cur_S)
            {
                cur_count++;
            }
            else
            {
                lstLODgroups.push_back(cur_count);
                cur_S = P.pVisual->shader->E[shid];
                cur_count = 1;
            }

            // calculate alpha
            const float ssaDiff = P.ssa - r_ssaLOD_B;
            const float scale = ssaDiff / ssaRange;
            int iA = iFloor((1 - scale) * 255.f);
            const u32 uA = u32(clampr(iA, 0, 255));

            // calculate direction and shift
            FLOD* lodV = smart_cast<FLOD*>(P.pVisual);
            Fvector Ldir, shift;
            Ldir.sub(lodV->getVisData().sphere.P, Device.vCameraPosition).normalize();
            shift.mul(Ldir, -.5f * lodV->getVisData().sphere.R);

            // gen geometry
            FLOD::_face* facets = lodV->facets;
            svector<std::pair<float, u32>, 8> selector;
            for (u32 s = 0; s < 8; s++)
                selector.push_back(mk_pair(Ldir.dotproduct(facets[s].N), s));
            std::sort(selector.begin(), selector.end(), pred_dot);

            const float dot_best = selector[selector.size() - 1].first;
            const float dot_next = selector[selector.size() - 2].first;
            const float dot_next_2 = selector[selector.size() - 3].first;
            const u32 id_best = selector[selector.size() - 1].second;
            const u32 id_next = selector[selector.size() - 2].second;

            // Now we have two "best" planes, calculate factor, and approx normal
            float fA = dot_best, fB = dot_next, fC = dot_next_2;
            const float alpha = 0.5f + 0.5f * (1 - (fB - fC) / (fA - fC));
            int iF = iFloor(alpha * 255.5f);
            const u32 uF = u32(clampr(iF, 0, 255));

            // Fill VB
            FLOD::_face& FA = facets[id_best];
            FLOD::_face& FB = facets[id_next];
            constexpr int vid[4] = {3, 0, 2, 1};
            for (int id : vid)
            {
                V->p0.add(FB.v[id].v, shift);
                V->p1.add(FA.v[id].v, shift);
                V->n0 = FB.N;
                V->n1 = FA.N;
                V->sun_af = color_rgba(FB.v[id].c_sun, FA.v[id].c_sun, uA, uF);
                V->t0 = FB.v[id].t;
                V->t1 = FA.v[id].t;
                V->rgbh0 = FB.v[id].c_rgb_hemi;
                V->rgbh1 = FA.v[id].c_rgb_hemi;
                V++;
            }
        }
        lstLODgroups.push_back(cur_count);
        RImplementation.Vertex.Unlock(iBatchSize * uiVertexPerImposter, firstV->geom->vb_stride);

        // *** Render
        cmd_list.set_xform_world(Fidentity);
        for (u32 uiPass = 0; uiPass < SHADER_PASSES_MAX; ++uiPass)
        {
            int current = 0;
            u32 vCurOffset = vOffset;

            for (const int p_count : lstLODgroups)
            {
                const u32 uiNumPasses = lstLODs[current].pVisual->shader->E[shid]->passes.size();
                if (uiPass < uiNumPasses)
                {
                    cmd_list.set_Element(lstLODs[current].pVisual->shader->E[shid], uiPass);
                    cmd_list.set_Geometry(firstV->geom);
                    cmd_list.Render(D3DPT_TRIANGLELIST, vCurOffset, 0, 4 * p_count, 0, 2 * p_count);
                    cmd_list.stat.r.s_flora_lods.add(4 * p_count);
                }
                current += p_count;
                vCurOffset += 4 * p_count;
            }
        }

        lstLODgroups.clear();
    }

    lstLODs.clear();
}
