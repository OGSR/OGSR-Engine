#pragma once

class CGameFont;
class IRenderDeviceRender;

class IStatsRender
{
public:
    virtual ~IStatsRender() { ; }
    virtual void Copy(IStatsRender& _in) = 0;

    virtual void OutData(CGameFont& F) = 0;

    virtual void GuardVerts(CGameFont& F) = 0;
    virtual void GuardDrawCalls(CGameFont& F) = 0;

    virtual void SetDrawParams(IRenderDeviceRender* pRender) = 0;
};