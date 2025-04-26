#pragma once

class CBlender_gasmask_dudv : public IBlender
{
public:
    virtual LPCSTR getComment() { return "Gasmask_dudv"; }

    virtual void Compile(CBlender_Compile& C);

    CBlender_gasmask_dudv();
    virtual ~CBlender_gasmask_dudv();
};
