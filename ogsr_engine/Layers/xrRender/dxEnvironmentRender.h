#pragma once

#include "../../Include/xrRender/EnvironmentRender.h"

#include "blenders/blender.h"
class CBlender_skybox : public IBlender
{
public:
    virtual LPCSTR getComment() { return "INTERNAL: combiner"; }

    virtual void Compile(CBlender_Compile& C)
    {
        C.r_Pass("sky2", "sky2", FALSE, TRUE, FALSE);
        C.r_dx10Texture("s_sky0", "$null");
        C.r_dx10Texture("s_sky1", "$null");
        C.r_dx10Sampler("smp_rtlinear");

        C.PassSET_ZB(FALSE, FALSE);
        C.r_End();
    }
};

class dxEnvDescriptorRender : public IEnvDescriptorRender
{
    friend class dxEnvDescriptorMixerRender;

public:
    virtual void OnDeviceCreate(CEnvDescriptor& owner);
    virtual void OnDeviceDestroy();

    virtual void OnPrepare(CEnvDescriptor& owner);
    virtual void OnUnload(CEnvDescriptor& owner);

    virtual void Copy(IEnvDescriptorRender& _in);

private:
    ref_texture sky_texture;
    ref_texture sky_texture_env;
    ref_texture clouds_texture;

    bool b_textures_loaded{};
};

class dxEnvDescriptorMixerRender : public IEnvDescriptorMixerRender
{
public:
    virtual void Copy(IEnvDescriptorMixerRender& _in);

    virtual void Destroy();
    virtual void Clear();
    virtual void lerp(IEnvDescriptorRender* inA, IEnvDescriptorRender* inB);
    // private:
public:
    STextureList sky_r_textures;
    STextureList sky_r_textures_env;
    STextureList clouds_r_textures;
};

class dxEnvironmentRender : public IEnvironmentRender
{
public:
    dxEnvironmentRender();
    virtual void Copy(IEnvironmentRender& _in);

    virtual void OnFrame(CEnvironment& env);
    virtual void RenderSky(CBackend& cmd_list, CEnvironment& env);
    virtual void RenderClouds(CBackend& cmd_list, CEnvironment& env);
    virtual void OnDeviceCreate();
    virtual void OnDeviceDestroy();

private:
    CBlender_skybox m_b_skybox;

    ref_shader sh_2sky;
    ref_geom sh_2geom;

    ref_shader clouds_sh;
    ref_geom clouds_geom;

    ref_texture tsky0, tsky1;
};
