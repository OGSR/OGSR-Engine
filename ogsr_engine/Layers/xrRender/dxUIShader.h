#pragma once

#include "../../Include/xrRender/UIShader.h"

class dxUIShader : public IUIShader
{
    friend class dxUIRender;
    friend class dxDebugRender;
    friend class dxWallMarkArray;
    friend class CRender;

public:
    virtual ~dxUIShader() { ; }
    virtual void Copy(IUIShader& _in);
    virtual void create(LPCSTR sh, LPCSTR tex = nullptr, bool no_cache = false);
    virtual bool inited() { return hShader; }
    //virtual void destroy();

private:
    ref_shader hShader;
};
