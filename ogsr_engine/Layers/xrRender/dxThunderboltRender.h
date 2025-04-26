#pragma once

#include "../../Include/xrRender/ThunderboltRender.h"

class dxThunderboltRender : public IThunderboltRender
{
public:
    dxThunderboltRender();
    virtual ~dxThunderboltRender();

    virtual void Copy(IThunderboltRender& _in);

    virtual void Render(CBackend& cmd_list, CEffect_Thunderbolt& owner);

private:
    ref_geom hGeom_model;
    ref_geom hGeom_gradient;
};
