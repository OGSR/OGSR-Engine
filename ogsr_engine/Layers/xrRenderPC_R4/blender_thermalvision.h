#pragma once


class CBlender_heatvision : public IBlender
{
public:
    virtual LPCSTR getComment() { return "thermalvision"; }

    virtual void Compile(CBlender_Compile& C);

    CBlender_heatvision();
    virtual ~CBlender_heatvision();
};
