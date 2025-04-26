#pragma once

class CBlender_accum_spot : public IBlender
{
public:
    virtual LPCSTR getComment() { return "INTERNAL: accumulate spot light"; }

    virtual void Compile(CBlender_Compile& C);

    CBlender_accum_spot();
    virtual ~CBlender_accum_spot();
};
