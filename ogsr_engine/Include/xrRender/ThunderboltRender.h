#pragma once

class CEffect_Thunderbolt;
class CBackend;

class IThunderboltRender
{
public:
    virtual ~IThunderboltRender() { ; }
    virtual void Copy(IThunderboltRender& _in) = 0;

    virtual void Render(CBackend& cmd_list, CEffect_Thunderbolt& owner) = 0;
};