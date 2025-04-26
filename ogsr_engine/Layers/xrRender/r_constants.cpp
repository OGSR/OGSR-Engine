#include "stdafx.h"

#include "ResourceManager.h"
#include "r_constants.h"
#include "../xrRender/dxRenderDeviceRender.h"

R_constant_table::~R_constant_table()
{
    // dxRenderDeviceRender::Instance().Resources->_DeleteConstantTable(this);
    DEV->_DeleteConstantTable(this);
}

void R_constant_table::fatal(LPCSTR S) { FATAL(S); }

ref_constant R_constant_table::get(LPCSTR S) const
{
    // assumption - sorted by name
    const c_table::const_iterator it = std::lower_bound(table.cbegin(), table.cend(), S, [](const ref_constant& C, LPCSTR S) { return xr_strcmp(*C->name, S) < 0; });
    if (it == table.cend() || (0 != xr_strcmp((*it)->name.c_str(), S)))
        return nullptr;
    return *it;
}

ref_constant R_constant_table::get(const shared_str& S) const
{
    // linear search, but only ptr-compare
    for (const ref_constant& C : table)
    {
        if (C->name.equal(S))
            return C;
    }
    return nullptr;
}

void R_constant_table::dbg_dump(u32 context_id) const
{
    Msg("Constant table:");
    for (const ref_constant& C : table)
    {
        Msg("    %s", *C->name);
    }
    Msg("Constant buffers:");
    Msg("    Context %d:", context_id);
    for (const auto& cb : m_CBTable[context_id])
    {
        Msg("%d:", cb.first);

        cb.second->dbg_dump();
    }
}

/// !!!!!!!!FIX THIS FOR DX11!!!!!!!!!
void R_constant_table::merge(const R_constant_table* T)
{
    if (nullptr == T)
        return;

    // Real merge
    xr_vector<ref_constant> table_tmp;
    table_tmp.reserve(table.size());
    for (const auto& src : T->table)
    {
        ref_constant C = get(*src->name);
        if (!C)
        {
            C = xr_new<R_constant>();
            C->name = src->name;
            C->destination = src->destination;
            C->type = src->type;
            C->ps = src->ps;
            C->vs = src->vs;
            C->gs = src->gs;
            C->hs = src->hs;
            C->ds = src->ds;
            C->cs = src->cs;
            C->samp = src->samp;
            C->handler = src->handler;
            table_tmp.push_back(C);
        }
        else
        {
            C->destination |= src->destination;
            VERIFY(C->type == src->type);
            const R_constant_load& sL = src->get_load(src->destination);
            R_constant_load& dL = C->get_load(src->destination);
            dL.index = sL.index;
            dL.cls = sL.cls;
        }
    }

    if (!table_tmp.empty())
    {
        // Append
        std::move(table_tmp.begin(), table_tmp.end(), std::back_inserter(table));

        // Sort
        std::sort(table.begin(), table.end(), [](const ref_constant& C1, const ref_constant& C2) { return xr_strcmp(C1->name, C2->name) < 0; });
    }

    //	TODO:	DX10:	Implement merge with validity check
    for (u32 id = 0; id < R__NUM_CONTEXTS; ++id)
    {
        m_CBTable[id].reserve(m_CBTable[id].size() + T->m_CBTable[id].size());
        for (u32 i = 0; i < T->m_CBTable[id].size(); ++i)
            m_CBTable[id].push_back((T->m_CBTable[id])[i]);
    }
}

void R_constant_table::clear()
{
    for (auto& it : table)
        it = nullptr;

    table.clear();

    for (auto& id : m_CBTable)
    {
        id.clear();
    }
}

BOOL R_constant_table::equal(const R_constant_table& C) const
{
    if (table.size() != C.table.size())
        return FALSE;
    const u32 size = table.size();
    for (u32 it = 0; it < size; it++)
    {
        if (!table[it]->equal(&*C.table[it]))
            return FALSE;
    }

    return TRUE;
}
