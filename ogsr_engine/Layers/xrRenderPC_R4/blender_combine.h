#pragma once

class CBlender_combine : public IBlender
{
public:
    virtual LPCSTR getComment() { return "INTERNAL: combiner"; }

    virtual void Compile(CBlender_Compile& C);

    CBlender_combine();
    virtual ~CBlender_combine();
};

class CBlender_combine_msaa : public IBlender
{
public:
    virtual LPCSTR getComment() { return "INTERNAL: combiner"; }

    virtual void Compile(CBlender_Compile& C);

    CBlender_combine_msaa();
    virtual ~CBlender_combine_msaa();

    virtual void SetDefine(LPCSTR Name, LPCSTR Definition)
    {
        this->Name = Name;
        this->Definition = Definition;
    }

    LPCSTR Name;
    LPCSTR Definition;
};
