#pragma once

class IFlareRender
{
public:
    virtual ~IFlareRender() { ; }
    virtual void Copy(IFlareRender& _in) = 0;

    virtual void CreateShader(LPCSTR sh_name, LPCSTR tex_name) = 0;
    virtual void DestroyShader() = 0;
};

class CLensFlare;
class CBackend;

class ILensFlareRender
{
public:
    virtual ~ILensFlareRender() { ; }
    virtual void Copy(ILensFlareRender& _in) = 0;

    virtual void Render(CBackend& cmd_list, CLensFlare& owner, BOOL bSun, BOOL bFlares, BOOL bGradient) = 0;
    virtual void OnDeviceCreate() = 0;
    virtual void OnDeviceDestroy() = 0;
};
