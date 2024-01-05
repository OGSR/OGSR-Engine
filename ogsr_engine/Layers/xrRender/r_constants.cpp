#include "stdafx.h"

#include "ResourceManager.h"

#include "../../xrCore/xrPool.h"
#include "r_constants.h"

#include "../xrRender/dxRenderDeviceRender.h"

// pool
//.static	poolSS<R_constant,512>			g_constant_allocator;

// R_constant_table::~R_constant_table	()	{	dxRenderDeviceRender::Instance().Resources->_DeleteConstantTable(this);	}

R_constant_table::~R_constant_table()
{
    // dxRenderDeviceRender::Instance().Resources->_DeleteConstantTable(this);
    DEV->_DeleteConstantTable(this);
}

void R_constant_table::fatal(LPCSTR S) { FATAL(S); }

// predicates
IC bool p_search(ref_constant C, LPCSTR S) { return xr_strcmp(*C->name, S) < 0; }
IC bool p_sort(ref_constant C1, ref_constant C2) { return xr_strcmp(C1->name, C2->name) < 0; }

ref_constant R_constant_table::get(LPCSTR S)
{
    // assumption - sorted by name
    c_table::iterator I = std::lower_bound(table.begin(), table.end(), S, p_search);
    if (I == table.end() || (0 != xr_strcmp(*(*I)->name, S)))
        return 0;
    else
        return *I;
}
ref_constant R_constant_table::get(shared_str& S)
{
    // linear search, but only ptr-compare
    c_table::iterator I = table.begin();
    c_table::iterator E = table.end();
    for (; I != E; ++I)
    {
        ref_constant C = *I;
        if (C->name.equal(S))
            return C;
    }
    return 0;
}

/// !!!!!!!!FIX THIS FOR DX11!!!!!!!!!
void R_constant_table::merge(R_constant_table* T)
{
    if (0 == T)
        return;

    // Real merge
    for (u32 it = 0; it < T->table.size(); it++)
    {
        ref_constant src = T->table[it];
        ref_constant C = get(*src->name);
        if (!C)
        {
            C = xr_new<R_constant>(); //.g_constant_allocator.create();
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
            table.push_back(C);
        }
        else
        {
            VERIFY2(!(C->destination & src->destination & RC_dest_sampler), "Can't have samplers or textures with the same name for PS, VS and GS.");
            C->destination |= src->destination;
            VERIFY(C->type == src->type);
            R_constant_load& sL = src->get_load(src->destination);
            R_constant_load& dL = C->get_load(src->destination);
            dL.index = sL.index;
            dL.cls = sL.cls;
        }
    }

    // Sort
    std::sort(table.begin(), table.end(), p_sort);

    //	TODO:	DX10:	Implement merge with validity check
    m_CBTable.reserve(m_CBTable.size() + T->m_CBTable.size());
    for (u32 i = 0; i < T->m_CBTable.size(); ++i)
        m_CBTable.push_back(T->m_CBTable[i]);
}

void R_constant_table::clear()
{
    //.
    for (u32 it = 0; it < table.size(); it++)
        table[it] = 0; //.g_constant_allocator.destroy(table[it]);
    table.clear();
    m_CBTable.clear();
}

BOOL R_constant_table::equal(R_constant_table& C)
{
    if (table.size() != C.table.size())
        return FALSE;
    u32 size = table.size();
    for (u32 it = 0; it < size; it++)
    {
        if (!table[it]->equal(&*C.table[it]))
            return FALSE;
    }

    return TRUE;
}
