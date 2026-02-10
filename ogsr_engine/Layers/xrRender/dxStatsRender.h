#pragma once

#include "../../Include/xrRender/StatsRender.h"

class dxStatsRender : public IStatsRender
{
public:
    virtual void Copy(IStatsRender& _in);

    void OutData(CGameFont& F) override;

    void GuardVerts(CGameFont& F) override;
    void GuardDrawCalls(CGameFont& F) override;

    void SetDrawParams(IRenderDeviceRender* pRender) override;

private:
};
