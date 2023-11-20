#pragma once

class CEffect_Rain;

#include "../../xrCore/_sphere.h"

class IRainRender
{
public:
    virtual ~IRainRender() { ; }

    virtual void Render(CEffect_Rain& owner) = 0;
    virtual void Calculate(CEffect_Rain& owner) = 0;

    virtual const Fsphere& GetDropBounds() const = 0;
};
