#ifndef dx10MSAABlender_included
#define dx10MSAABlender_included

class CBlender_msaa : public IBlender
{
public:
    virtual LPCSTR getComment() { return "INTERNAL: DX10 msaa blender"; }

    virtual void Compile(CBlender_Compile& C);
};

#endif //	dx10RainBlender_included