#include "stdafx.h"

#include <wincodec.h>
#include <DirectXTex.h>

using namespace DirectX;

constexpr size_t GAMESAVE_SIZE{768};

void CRender::ScreenshotImpl(ScreenshotMode mode, LPCSTR name, CMemoryWriter* memory_writer)
{
    Microsoft::WRL::ComPtr<ID3DResource> pSrcTexture;
    HW.pBaseRT->GetResource(pSrcTexture.GetAddressOf());

    ScratchImage SImage{};
    CHK_DX(CaptureTexture(HW.pDevice, HW.pContext, pSrcTexture.Get(), SImage));
    Blob SavedBlob{};

    switch (mode)
    {
    case IRender_interface::SM_NORMAL: {
        string64 t_stemp;
        string_path buf;

        static const bool UseTGA = !!strstr(Core.Params, "-ss_tga");
        static const bool UsePNG = !strstr(Core.Params, "-ss_jpg");

        xr_sprintf(buf, sizeof(buf), "ss_%s_%s_(%s).%s", Core.UserName, timestamp(t_stemp), (g_pGameLevel) ? g_pGameLevel->name().c_str() : "mainmenu",
                   UseTGA ? "tga" : (UsePNG ? "png" : "jpg"));

        if (UseTGA)
            SaveToTGAMemory(*SImage.GetImage(0, 0, 0), TGA_FLAGS::TGA_FLAGS_NONE, SavedBlob);
        else
            SaveToWICMemory(*SImage.GetImage(0, 0, 0), WIC_FLAGS::WIC_FLAGS_FORCE_SRGB, UsePNG ? GUID_ContainerFormatPng : GUID_ContainerFormatJpeg, SavedBlob);

        auto fs = FS.w_open("$screenshots$", buf);
        R_ASSERT(fs);
        fs->w(SavedBlob.GetBufferPointer(), SavedBlob.GetBufferSize());
        FS.w_close(fs);
    }
    break;
    case IRender_interface::SM_FOR_GAMESAVE: {
        ScratchImage SImageSmall{};
        Resize(*SImage.GetImage(0, 0, 0), GAMESAVE_SIZE, GAMESAVE_SIZE, TEX_FILTER_FLAGS::TEX_FILTER_DEFAULT, SImageSmall);

        auto hr = SaveToDDSMemory(*SImageSmall.GetImage(0, 0, 0), DDS_FLAGS::DDS_FLAGS_NONE, SavedBlob);
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
    break;
    case IRender_interface::SM_FOR_LEVELMAP:
    case IRender_interface::SM_FOR_CUBEMAP: {
        ScratchImage SImageSmall{};
        CHK_DX(Resize(*SImage.GetImage(0, 0, 0), Device.dwHeight, Device.dwHeight, TEX_FILTER_FLAGS::TEX_FILTER_LINEAR, SImageSmall));

        auto hr = SaveToTGAMemory(*SImageSmall.GetImage(0, 0, 0), TGA_FLAGS::TGA_FLAGS_NONE, SavedBlob);

        if (hr == D3D_OK)
        {
            string_path buf;
            VERIFY(name);
            strconcat(sizeof(buf), buf, name, ".tga");
            auto fs = FS.w_open("$screenshots$", buf);
            if (fs)
            {
                fs->w(SavedBlob.GetBufferPointer(), SavedBlob.GetBufferSize());
                FS.w_close(fs);
            }
        }
    }
    break;
    }
}

void CRender::Screenshot(ScreenshotMode mode, LPCSTR name) { ScreenshotImpl(mode, name, nullptr); }

void DoAsyncScreenshot() { RImplementation.Target->DoAsyncScreenshot(); }
