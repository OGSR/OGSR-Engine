#pragma once

#include "../../Include/xrRender/EnvironmentRender.h"

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
    ref_shader skybox_sh;
    ref_geom skybox_geom;

    ref_shader clouds_sh;
    ref_geom clouds_geom;

    ref_texture tsky0, tsky1;
};
