#include "stdafx.h"

#include <wincodec.h>
#include <DirectXTex.h>

using namespace DirectX;

constexpr size_t GAMESAVE_SIZE{768};

extern bool use_reshade;

extern void get_screenshot_width_and_height(uint32_t* out_width, uint32_t* out_height);
extern void capture_screenshot(uint8_t* pixels);

void CRender::ScreenshotImpl(ScreenshotMode mode, LPCSTR name)
{
    ScratchImage SImage{};
    Blob SavedBlob{};

    // пример как снимать скриншоты через reshade
    // оно вроде б как должно делать скрины с эффектами но по факту не сохраняет 3д мир вообще

    //if (use_reshade)
    //{
    //    uint32_t width, height;

    //    get_screenshot_width_and_height(&width, &height);
    //    xr_vector<uint8_t> pixels(width * height * 4);

    //    capture_screenshot(pixels.data());

    //    SImage.Initialize2D(DXGI_FORMAT_R8G8B8A8_UNORM, width, height, 1, 1);

    //    const auto data = SImage.GetPixels();

    //    std::memcpy(data, pixels.data(), pixels.size());
    //}
    //else
    {
        Microsoft::WRL::ComPtr<ID3DResource> pSrcTexture;
        Target->get_base_rt()->GetResource(pSrcTexture.GetAddressOf());

        CHK_DX(CaptureTexture(HW.pDevice, HW.get_context(CHW::IMM_CTX_ID), pSrcTexture.Get(), SImage));
    }

    switch (mode)
    {
    case IRender_interface::SM_NORMAL: {

        static const bool UseTGA = !!strstr(Core.Params, "-ss_tga");
        static const bool UsePNG = !strstr(Core.Params, "-ss_jpg");

        string32 TimeBuf;
        using namespace std::chrono;
        const auto now = system_clock::now();
        const auto time = system_clock::to_time_t(now);
        std::strftime(TimeBuf, sizeof(TimeBuf), "%Y-%m-%d_%H-%M-%S", std::localtime(&time));

        string_path buf;
        sprintf_s(buf, sizeof(buf), "ss_%s_%s-(%s).%s", Core.UserName, TimeBuf, g_pGameLevel ? g_pGameLevel->name().c_str() : "mainmenu", UsePNG ? "png" : UseTGA ? "bmp" : "jpg");

        if (UseTGA)
        {
            SaveToTGAMemory(*SImage.GetImage(0, 0, 0), TGA_FLAGS_NONE, SavedBlob);
        }
        else if (UsePNG)
        {
            ScratchImage rgbImage{};
            Convert(*SImage.GetImage(0, 0, 0), DXGI_FORMAT_B8G8R8X8_UNORM, TEX_FILTER_DEFAULT, 0.0f, rgbImage);
            SaveToWICMemory(*rgbImage.GetImage(0, 0, 0), WIC_FLAGS_FORCE_SRGB, GUID_ContainerFormatPng, SavedBlob);
        }
        else
        {
            SaveToWICMemory(*SImage.GetImage(0, 0, 0), WIC_FLAGS_FORCE_SRGB, GUID_ContainerFormatJpeg, SavedBlob);
        }

        auto fs = FS.w_open(fsgame::screenshots, buf);
        R_ASSERT(fs);
        fs->w(SavedBlob.GetBufferPointer(), SavedBlob.GetBufferSize());
        FS.w_close(fs);
    }
    break;
    case IRender_interface::SM_FOR_GAMESAVE: {
        ScratchImage SImageSmall{};
        CHK_DX(Resize(*SImage.GetImage(0, 0, 0), GAMESAVE_SIZE, GAMESAVE_SIZE, TEX_FILTER_DEFAULT, SImageSmall));

        HRESULT hr;

        ScratchImage SImageCompressed{};
        hr = Compress(*SImageSmall.GetImage(0, 0, 0), DXGI_FORMAT_BC3_UNORM, TEX_COMPRESS_DEFAULT, TEX_THRESHOLD_DEFAULT, SImageCompressed);

        if (hr == D3D_OK)
        {
            hr = SaveToDDSMemory(*SImageCompressed.GetImage(0, 0, 0), DDS_FLAGS_NONE, SavedBlob);
            if (hr == D3D_OK)
            {
                auto fs = FS.w_open(name);
                if (fs)
                {
                    fs->w(SavedBlob.GetBufferPointer(), SavedBlob.GetBufferSize());
                    FS.w_close(fs);
                }
            }
        }

        SImageCompressed.Release();
        SImageSmall.Release();
    }
    break;
    case IRender_interface::SM_FOR_LEVELMAP:
    case IRender_interface::SM_FOR_CUBEMAP: {
        ScratchImage SImageSmall{};
        CHK_DX(Resize(*SImage.GetImage(0, 0, 0), Device.dwHeight, Device.dwHeight, TEX_FILTER_FLAGS::TEX_FILTER_LINEAR, SImageSmall));

        const auto hr = SaveToTGAMemory(*SImageSmall.GetImage(0, 0, 0), TGA_FLAGS::TGA_FLAGS_NONE, SavedBlob);
        if (hr == D3D_OK)
        {
            string_path buf;
            VERIFY(name);
            strconcat(sizeof(buf), buf, name, ".tga");
            auto fs = FS.w_open(fsgame::screenshots, buf);
            if (fs)
            {
                fs->w(SavedBlob.GetBufferPointer(), SavedBlob.GetBufferSize());
                FS.w_close(fs);
            }
        }

        SImageSmall.Release();
    }
    break;
    }

    SavedBlob.Release();
    SImage.Release();
}

void CRender::Screenshot(ScreenshotMode mode, LPCSTR name) { ScreenshotImpl(mode, name); }
