#ifndef dx10RainBlender_included
#define dx10RainBlender_included

class CBlender_rain : public IBlender
{
public:
    virtual LPCSTR getComment() { return "INTERNAL: DX10 rain blender"; }

    virtual void Compile(CBlender_Compile& C);
};

#endif //	dx10RainBlender_included