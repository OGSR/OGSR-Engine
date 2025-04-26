#pragma once

class CEnvironment;
class CEnvDescriptor;

class IEnvDescriptorRender
{
public:
    virtual ~IEnvDescriptorRender() { ; }
    virtual void Copy(IEnvDescriptorRender& _in) = 0;

    virtual void OnDeviceCreate(CEnvDescriptor& owner) = 0;
    virtual void OnDeviceDestroy() = 0;

    virtual void OnPrepare(CEnvDescriptor& owner) = 0;
    virtual void OnUnload(CEnvDescriptor& owner) = 0;
};

class IEnvDescriptorMixerRender
{
public:
    virtual ~IEnvDescriptorMixerRender() { ; }
    virtual void Copy(IEnvDescriptorMixerRender& _in) = 0;

    virtual void Destroy() = 0;
    virtual void Clear() = 0;
    virtual void lerp(IEnvDescriptorRender* inA, IEnvDescriptorRender* inB) = 0;
};

class CBackend;

class IEnvironmentRender
{
public:
    virtual ~IEnvironmentRender() { ; }
    virtual void Copy(IEnvironmentRender& _in) = 0;
    virtual void OnFrame(CEnvironment& env) = 0;
    virtual void RenderSky(CBackend& cmd_list, CEnvironment& env) = 0;
    virtual void RenderClouds(CBackend& cmd_list, CEnvironment& env) = 0;
    virtual void OnDeviceCreate() = 0;
    virtual void OnDeviceDestroy() = 0;
};
