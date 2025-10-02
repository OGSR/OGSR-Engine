#include "stdafx.h"


#include "ResourceManager.h"
#include "../../xr_3da/Render.h"


void CResourceManager::reset_begin()
{
    // destroy everything, renderer may use
    RImplementation.reset_begin();

    // destroy state-blocks
    for (u32 _it = 0; _it < v_states.size(); _it++)
        _RELEASE(v_states[_it]->state);

    // destroy RTs
    for (map_RTIt rt_it = m_rtargets.begin(); rt_it != m_rtargets.end(); ++rt_it)
        rt_it->second->reset_begin();

    // destroy DStreams
    RImplementation.old_QuadIB = RImplementation.QuadIB;
    HW.stats_manager.decrement_stats_ib(RImplementation.QuadIB);
    _RELEASE(RImplementation.QuadIB);

    RImplementation.Index.reset_begin();
    RImplementation.Vertex.reset_begin();
}

bool cmp_rt(const CRT* A, const CRT* B) { return A->_order < B->_order; }

void CResourceManager::reset_end()
{
    // create RDStreams
    RImplementation.Vertex.reset_end();
    RImplementation.Index.reset_end();

    RImplementation.CreateQuadIB();

    // remark geom's which point to dynamic VB/IB
    {
        for (u32 _it = 0; _it < v_geoms.size(); _it++)
        {
            SGeometry* _G = v_geoms[_it];
            if (_G->vb == RImplementation.Vertex.old_pVB)
                _G->vb = RImplementation.Vertex.Buffer();

            // Here we may recover the buffer using one of
            // cmd_list's index buffers.
            // Do not remove else.
            if (_G->ib == RImplementation.Index.old_pIB)
            {
                _G->ib = RImplementation.Index.Buffer();
            }
            else if (_G->ib == RImplementation.old_QuadIB)
            {
                _G->ib = RImplementation.QuadIB;
            }
        }
    }

    // create RTs in the same order as them was first created
    {
        // RT
        xr_vector<CRT*> rt;
        for (map_RTIt rt_it = m_rtargets.begin(); rt_it != m_rtargets.end(); ++rt_it)
            rt.push_back(rt_it->second);
        std::sort(rt.begin(), rt.end(), cmp_rt);
        for (u32 _it = 0; _it < rt.size(); _it++)
            rt[_it]->reset_end();
    }

    // create state-blocks
    {
        for (u32 _it = 0; _it < v_states.size(); _it++)
            v_states[_it]->state = ID3DState::Create(v_states[_it]->state_code);
    }

    // create everything, renderer may use
    RImplementation.reset_end();

    Dump(true);
}

template <class C>
void mdump(C c)
{
    if (0 == c.size())
        return;
    for (auto I = c.begin(); I != c.end(); ++I)
        Msg("*        : %3d: %s", I->second->ref_count.load(), I->second->cName.c_str());
}

extern string_unordered_map<std::string, ref_shader> g_ModelShadersCache;
extern string_unordered_map<std::string, ref_shader> g_ShadersCache;

CResourceManager::~CResourceManager()
{
    for (auto& it : g_ShadersCache)
        it.second.destroy();

    g_ShadersCache.clear();

    for (auto& it : g_ModelShadersCache)
        it.second.destroy();

    g_ModelShadersCache.clear();

    Dump(false);
}

void CResourceManager::Dump(bool bBrief) const
{
    Msg("* RM_Dump: textures  : %d", m_textures.size());
    if (!bBrief)
        mdump(m_textures);

    Msg("* RM_Dump: rtargets  : %d", m_rtargets.size());
    if (!bBrief)
        mdump(m_rtargets);

    Msg("* RM_Dump: vs        : %d", m_vs.size());
    if (!bBrief)
        mdump(m_vs);

    Msg("* RM_Dump: ps        : %d", m_ps.size());
    if (!bBrief)
        mdump(m_ps);

    Msg("* RM_Dump: dcl       : %d", v_declarations.size());
    Msg("* RM_Dump: states    : %d", v_states.size());
    Msg("* RM_Dump: tex_list  : %d", lst_textures.size());
    Msg("* RM_Dump: matrices  : %d", lst_matrices.size());
    Msg("* RM_Dump: lst_constants: %d", lst_constants.size());
    Msg("* RM_Dump: v_passes  : %d", v_passes.size());
    Msg("* RM_Dump: v_elements: %d", v_elements.size());
    Msg("* RM_Dump: v_shaders : %d", v_shaders.size());
}
