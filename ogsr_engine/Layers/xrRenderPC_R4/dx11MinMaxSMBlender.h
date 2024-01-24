#ifndef dx11MinMaxSMBlender_included
#define dx11MinMaxSMBlender_included

class CBlender_createminmax : public IBlender
{
public:
    virtual LPCSTR getComment() { return "INTERNAL: DX10 minmax sm blender"; }

    virtual void Compile(CBlender_Compile& C);
};

#endif