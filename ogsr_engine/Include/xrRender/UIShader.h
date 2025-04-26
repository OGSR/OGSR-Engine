#pragma once

class IUIShader
{
public:
    virtual ~IUIShader() { ; }
    virtual void Copy(IUIShader& _in) = 0;
    virtual void create(LPCSTR sh, LPCSTR tex = nullptr, bool no_cache = false) = 0;
    virtual bool inited() = 0;
    //virtual void destroy() = 0;
};