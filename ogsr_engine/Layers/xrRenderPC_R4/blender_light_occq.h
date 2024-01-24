#pragma once

class CBlender_light_occq : public IBlender
{
public:
    virtual LPCSTR getComment() { return "INTERNAL: occlusion testing"; }

    virtual void Compile(CBlender_Compile& C);

    CBlender_light_occq();
    virtual ~CBlender_light_occq();
};
