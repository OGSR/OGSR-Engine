#pragma once

#include "render.h"

//////////////////////////////////////////////////////////////////////////
class ENGINE_API IRenderable
{
public:
    struct
    {
        Fmatrix xform;
        IRenderVisual* visual;
        IRender_ObjectSpecific* pROS;
        BOOL pROS_Allowed;
        bool invisible{}; // object should be invisible on the scene graph
        bool hud{}; // At the current moment, object is being rendered on HUD
    } renderable;

public:
    IRenderable();
    virtual ~IRenderable();
    IRender_ObjectSpecific* renderable_ROS();

    virtual void renderable_Render(u32 context_id, IRenderable* root) = 0;
    virtual BOOL renderable_ShadowReceive() { return FALSE; };

    bool renderable_Invisible() const { return renderable.invisible; }
    void renderable_Invisible(bool value) { renderable.invisible = value; }
    bool renderable_HUD() const  { return renderable.hud; }
    void renderable_HUD(bool value) { renderable.hud = value; }

    virtual float GetHotness() { return 0.0; }
    virtual float GetTransparency() { return 0.0; }
};
