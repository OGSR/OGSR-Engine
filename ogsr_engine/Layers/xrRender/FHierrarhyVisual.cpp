// FHierrarhyVisual.cpp: implementation of the FHierrarhyVisual class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"


#include "FHierrarhyVisual.h"
#include "../../xr_3da/Fmesh.h"

#include "../../xr_3da/render.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

FHierrarhyVisual::FHierrarhyVisual() : dxRender_Visual() { bDontDelete = FALSE; }

FHierrarhyVisual::~FHierrarhyVisual()
{
    if (!bDontDelete)
    {
        for (auto& i : children)
            RImplementation.model_Delete((IRenderVisual*&)i);
    }
    children.clear();
}

void FHierrarhyVisual::Release()
{
    if (!bDontDelete)
    {
        for (const auto& i : children)
            i->Release();
    }
}

void FHierrarhyVisual::Load(const char* N, IReader* data, u32 dwFlags)
{
    dxRender_Visual::Load(N, data, dwFlags);
    if (data->find_chunk(OGF_CHILDREN_L))
    {
        // From Link
        const u32 cnt = data->r_u32();
        children.resize(cnt);
        for (u32 i = 0; i < cnt; i++)
        {
            const u32 ID = data->r_u32();
            children[i] = smart_cast<dxRender_Visual*>(RImplementation.getVisual(ID));
        }
        bDontDelete = TRUE;
    }
    else
    {
        if (data->find_chunk(OGF_CHILDREN))
        {
            // From stream
            IReader* OBJ = data->open_chunk(OGF_CHILDREN);
            if (OBJ)
            {
                IReader* O = OBJ->open_chunk(0);
                for (int count = 1; O; count++)
                {
                    string_path name_load, short_name, num;
                    xr_strcpy(short_name, N);
                    if (strext(short_name))
                        *strext(short_name) = 0;
                    strconcat(sizeof(name_load), name_load, short_name, ":", itoa(count, num, 10));
                    children.push_back(smart_cast<dxRender_Visual*>(RImplementation.model_CreateChild(name_load, O)));
                    O->close();
                    O = OBJ->open_chunk(count);
                }
                OBJ->close();
            }
            bDontDelete = FALSE;
        }
        else
        {
            FATAL("Invalid visual");
        }
    }
}

void FHierrarhyVisual::Copy(dxRender_Visual* pSrc)
{
    dxRender_Visual::Copy(pSrc);

    const FHierrarhyVisual* pFrom = smart_cast<FHierrarhyVisual*>(pSrc);

    children.clear();
    children.reserve(pFrom->children.size());
    for (const auto& i : pFrom->children)
    {
        dxRender_Visual* p = smart_cast<dxRender_Visual*>(RImplementation.model_Duplicate(i));
        children.push_back(p);
    }
    bDontDelete = FALSE;
}

void FHierrarhyVisual::MarkAsHot(bool is_hot)
{
    dxRender_Visual::MarkAsHot(is_hot);
    for (const auto& i : children)
        i->MarkAsHot(is_hot);
}