// FHierrarhyVisual.h: interface for the FHierrarhyVisual class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "fbasicvisual.h"

class FHierrarhyVisual : public dxRender_Visual
{
    BOOL bDontDelete;

public:
    xr_vector<dxRender_Visual*> children;

    FHierrarhyVisual();
    virtual ~FHierrarhyVisual();

    virtual void Load(const char* N, IReader* data, u32 dwFlags);
    virtual void Copy(dxRender_Visual* pFrom);
    virtual void Release();
    virtual void MarkAsHot(bool is_hot);
};
