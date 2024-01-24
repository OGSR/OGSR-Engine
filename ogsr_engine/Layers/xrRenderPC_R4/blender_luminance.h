#pragma once

class CBlender_luminance : public IBlender
{
public:
    virtual LPCSTR getComment() { return "INTERNAL: luminance estimate"; }

    virtual void Compile(CBlender_Compile& C);

    CBlender_luminance();
    virtual ~CBlender_luminance();
};
