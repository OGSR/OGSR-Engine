#pragma once

#include "RenderFactory.h"
#include "../../Include/xrAPI/xrAPI.h"

template <class T>
class FactoryPtr
{
public:
    FactoryPtr() { CreateObject(); }
    ~FactoryPtr() { DestroyObject(); }

    FactoryPtr(const FactoryPtr<T>& _in)
    {
        CreateObject();
        m_pObject->Copy(*_in.m_pObject);
    }

    FactoryPtr& operator=(const FactoryPtr& _in)
    {
        m_pObject->Copy(*_in.m_pObject);
        return *this;
    }

    T& operator*() const { return *m_pObject; }
    T* operator->() const { return m_pObject; }

    // unspecified bool type
    typedef T const* (FactoryPtr::*unspecified_bool_type)() const;
    operator unspecified_bool_type() const { return (!m_pObject ? 0 : &FactoryPtr::get); }
    bool operator!() const { return m_pObject == nullptr; }

private:
    void CreateObject();
    void DestroyObject();

    T const* get() const { return m_pObject; }

    T* m_pObject;
};

template <>
inline void FactoryPtr<IUISequenceVideoItem>::CreateObject()
{
    m_pObject = RenderFactory->CreateUISequenceVideoItem();
}
template <>
inline void FactoryPtr<IUISequenceVideoItem>::DestroyObject()
{
    RenderFactory->DestroyUISequenceVideoItem(m_pObject);
    m_pObject = nullptr;
}
template <>
inline void FactoryPtr<IUIShader>::CreateObject()
{
    m_pObject = RenderFactory->CreateUIShader();
}
template <>
inline void FactoryPtr<IUIShader>::DestroyObject()
{
    RenderFactory->DestroyUIShader(m_pObject);
    m_pObject = nullptr;
}
template <>
inline void FactoryPtr<IRenderDeviceRender>::CreateObject()
{
    m_pObject = RenderFactory->CreateRenderDeviceRender();
}
template <>
inline void FactoryPtr<IRenderDeviceRender>::DestroyObject()
{
    RenderFactory->DestroyRenderDeviceRender(m_pObject);
    m_pObject = nullptr;
}
template <>
inline void FactoryPtr<IWallMarkArray>::CreateObject()
{
    m_pObject = RenderFactory->CreateWallMarkArray();
}
template <>
inline void FactoryPtr<IWallMarkArray>::DestroyObject()
{
    RenderFactory->DestroyWallMarkArray(m_pObject);
    m_pObject = nullptr;
}
template <>
inline void FactoryPtr<IStatsRender>::CreateObject()
{
    m_pObject = RenderFactory->CreateStatsRender();
}
template <>
inline void FactoryPtr<IStatsRender>::DestroyObject()
{
    RenderFactory->DestroyStatsRender(m_pObject);
    m_pObject = nullptr;
};

template <>
inline void FactoryPtr<IFlareRender>::CreateObject()
{
    m_pObject = RenderFactory->CreateFlareRender();
}
template <>
inline void FactoryPtr<IFlareRender>::DestroyObject()
{
    RenderFactory->DestroyFlareRender(m_pObject);
    m_pObject = nullptr;
}
template <>
inline void FactoryPtr<IThunderboltRender>::CreateObject()
{
    m_pObject = RenderFactory->CreateThunderboltRender();
}
template <>
inline void FactoryPtr<IThunderboltRender>::DestroyObject()
{
    RenderFactory->DestroyThunderboltRender(m_pObject);
    m_pObject = nullptr;
}
template <>
inline void FactoryPtr<IThunderboltDescRender>::CreateObject()
{
    m_pObject = RenderFactory->CreateThunderboltDescRender();
}
template <>
inline void FactoryPtr<IThunderboltDescRender>::DestroyObject()
{
    RenderFactory->DestroyThunderboltDescRender(m_pObject);
    m_pObject = nullptr;
}
template <>
inline void FactoryPtr<ILensFlareRender>::CreateObject()
{
    m_pObject = RenderFactory->CreateLensFlareRender();
}
template <>
inline void FactoryPtr<ILensFlareRender>::DestroyObject()
{
    RenderFactory->DestroyLensFlareRender(m_pObject);
    m_pObject = nullptr;
}
template <>
inline void FactoryPtr<IRainRender>::CreateObject()
{
    m_pObject = RenderFactory->CreateRainRender();
}
template <>
inline void FactoryPtr<IRainRender>::DestroyObject()
{
    RenderFactory->DestroyRainRender(m_pObject);
    m_pObject = nullptr;
}
template <>
inline void FactoryPtr<IEnvironmentRender>::CreateObject()
{
    m_pObject = RenderFactory->CreateEnvironmentRender();
}
template <>
inline void FactoryPtr<IEnvironmentRender>::DestroyObject()
{
    RenderFactory->DestroyEnvironmentRender(m_pObject);
    m_pObject = nullptr;
}
template <>
inline void FactoryPtr<IEnvDescriptorRender>::CreateObject()
{
    m_pObject = RenderFactory->CreateEnvDescriptorRender();
}
template <>
inline void FactoryPtr<IEnvDescriptorRender>::DestroyObject()
{
    RenderFactory->DestroyEnvDescriptorRender(m_pObject);
    m_pObject = nullptr;
}
template <>
inline void FactoryPtr<IEnvDescriptorMixerRender>::CreateObject()
{
    m_pObject = RenderFactory->CreateEnvDescriptorMixerRender();
}
template <>
inline void FactoryPtr<IEnvDescriptorMixerRender>::DestroyObject()
{
    RenderFactory->DestroyEnvDescriptorMixerRender(m_pObject);
    m_pObject = nullptr;
}

template <>
inline void FactoryPtr<IFontRender>::CreateObject()
{
    m_pObject = RenderFactory->CreateFontRender();
}
template <>
inline void FactoryPtr<IFontRender>::DestroyObject()
{
    RenderFactory->DestroyFontRender(m_pObject);
    m_pObject = nullptr;
}
