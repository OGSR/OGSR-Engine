#pragma once

/*

#include <mmdeviceapi.h>
#include <wrl.h>

class NotificationClient : public IMMNotificationClient
{
public:
    NotificationClient();
    virtual ~NotificationClient() { Stop(); }

    bool Start();
    void Stop();

    std::string GetDefaultDeviceName() const;

    // IUnknown methods
    STDMETHOD(QueryInterface)(REFIID riid, void** ppvObject)
    {
        if (riid == IID_IUnknown || riid == __uuidof(IMMNotificationClient))
        {
            *ppvObject = static_cast<IMMNotificationClient*>(this);
            AddRef();
            return S_OK;
        }
        return E_NOINTERFACE;
    }

    ULONG STDMETHODCALLTYPE AddRef() { return InterlockedIncrement(&m_cRef); }

    ULONG STDMETHODCALLTYPE Release() { return InterlockedDecrement(&m_cRef); }

    // IMMNotificationClient methods
    STDMETHOD(OnDefaultDeviceChanged)(EDataFlow flow, ERole role, LPCWSTR pwstrDeviceId)
    {
        // Default audio device has been changed.
        Msg("~ NotificationClient OnDefaultDeviceChanged dev_id=%s", pwstrDeviceId);
        Sound->_restart();
        return S_OK;
    }

    STDMETHOD(OnDeviceAdded)(LPCWSTR pwstrDeviceId)
    {
        // A new audio device has been added.
        Msg("~ NotificationClient OnDeviceAdded dev_id=%s", pwstrDeviceId);
        Sound->_restart();
        return S_OK;
    }

    STDMETHOD(OnDeviceRemoved)(LPCWSTR pwstrDeviceId)
    {
        // An audio device has been removed.
        Msg("~ NotificationClient OnDeviceRemoved dev_id=%s", pwstrDeviceId);
        Sound->_restart();
        return S_OK;
    }

    STDMETHOD(OnDeviceStateChanged)(LPCWSTR pwstrDeviceId, DWORD dwNewState)
    {
        // The state of an audio device has changed.
        return S_OK;
    }

    STDMETHOD(OnPropertyValueChanged)(LPCWSTR pwstrDeviceId, const PROPERTYKEY key)
    {
        // A property value of an audio device has changed.
        return S_OK;
    }

private:
    LONG m_cRef{};
    Microsoft::WRL::ComPtr<IMMDeviceEnumerator> m_pEnumerator{};
};

*/