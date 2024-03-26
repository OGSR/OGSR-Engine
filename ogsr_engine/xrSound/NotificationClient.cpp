#include "stdafx.h"
/*

#include "NotificationClient.h"

#include <codecvt>
#include <locale>

#include "Functiondiscoverykeys_devpkey.h"

NotificationClient::NotificationClient()
{
    // Create the device enumerator
    CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, IID_PPV_ARGS(m_pEnumerator.GetAddressOf()));
}

bool NotificationClient::Start()
{
    if (m_pEnumerator)
    {
        // Register for device change notifications
        HRESULT hr = m_pEnumerator->RegisterEndpointNotificationCallback(this);

        if (SUCCEEDED(hr))
        {
            Msg("- SOUND: NotificationClient registered!");

            return true;
        }

        Msg("! RegisterEndpointNotificationCallback failed!");
    }

    return false;
}

void NotificationClient::Stop()
{
    // Unregister the device enumerator
    if (m_pEnumerator)
    {
        m_pEnumerator->UnregisterEndpointNotificationCallback(this);
        m_pEnumerator->Release();
    }
}

static std::string ws2s(const std::wstring& wstr)
{
    // костыль, видимо не будет работать везде

    using convert_typeX = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<convert_typeX> converterX;

    return converterX.to_bytes(wstr);
}

std::string NotificationClient::GetDefaultDeviceName() const
{
    std::string result;

    if (!m_pEnumerator)
    {
        return result;
    }

    Microsoft::WRL::ComPtr<IMMDevice> pDevice{};

    HRESULT hr = m_pEnumerator->GetDefaultAudioEndpoint(eRender, eMultimedia, pDevice.GetAddressOf());

    if (SUCCEEDED(hr))
    {
        Microsoft::WRL::ComPtr<IPropertyStore> pStore{};
        hr = pDevice->OpenPropertyStore(STGM_READ, pStore.GetAddressOf());
        if (SUCCEEDED(hr))
        {
            PROPVARIANT friendlyName{};
            PropVariantInit(&friendlyName);
            hr = pStore->GetValue(PKEY_Device_FriendlyName, &friendlyName);
            if (SUCCEEDED(hr))
            {
                std::wstring wstr{friendlyName.pwszVal};
                result = ws2s(wstr);

                Msg("~~[%s] System Default sound output device is [%s], device name size: [%u]", __FUNCTION__, result.c_str(), result.size());

                PropVariantClear(&friendlyName);
            }
        }
    }

    return result;
}

*/