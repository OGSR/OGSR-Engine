#pragma once

// crookr
class CBlender_fakescope : public IBlender
{
public:
    virtual LPCSTR getComment() { return "fakescope"; }

    virtual void Compile(CBlender_Compile& C);

    CBlender_fakescope();
    virtual ~CBlender_fakescope();
};