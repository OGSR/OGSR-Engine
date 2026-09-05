#include "stdafx.h"

#include <DirectXTex.h>

void fix_texture_name(const char* fn)
{
    char* _ext = strext(fn);
    if (_ext && (0 == _stricmp(_ext, ".tga") || 0 == _stricmp(_ext, ".dds") || 0 == _stricmp(_ext, ".bmp") || 0 == _stricmp(_ext, ".ogm")))
        *_ext = 0;
}

ID3DBaseTexture* CRender::texture_load(LPCSTR fRName, u32& ret_msize)
{
    // validation
    R_ASSERT(fRName && fRName[0]);

    // make file name
    string_path fname, fn;
    xr_strcpy(fname, fRName);
    fix_texture_name(fname);

    if (strstr(fname, "_bump") && !FS.exist(fn, "$game_textures$", fname, ".dds"))
    {
        Msg("! Fallback to default bump map: [%s]", fname);

        if (strstr(fname, "_bump#"))
            R_ASSERT(FS.exist(fn, "$game_textures$", "ed\\ed_dummy_bump#", ".dds"), "ed_dummy_bump#");
        else
            R_ASSERT(FS.exist(fn, "$game_textures$", "ed\\ed_dummy_bump", ".dds"), "ed_dummy_bump");
    }
    else if (!FS.exist(fn, "$level$", fname, ".dds") && !FS.exist(fn, "$game_saves$", fname, ".dds") && !FS.exist(fn, "$game_textures$", fname, ".dds"))
    {
        Msg("! Can't find texture [%s]", fname);

        R_ASSERT(FS.exist(fn, "$game_textures$", "ed\\ed_not_existing_texture", ".dds"));
    }

    // Load and get header
    IReader* File = FS.r_open(fn);
    R_ASSERT(File);
#ifdef DEBUG
    Msg("* Loaded: %s[%zu]", fn, File->length());
#endif
    ID3DBaseTexture* pTexture2D{};
    DirectX::TexMetadata IMG{};
    DirectX::DDS_FLAGS dds_flags{DirectX::DDS_FLAGS_PERMISSIVE};
    bool allowFallback = true;

    do
    {
        DirectX::ScratchImage texture{};
        if (const auto hr = LoadFromDDSMemory(reinterpret_cast<const uint8_t*>(File->pointer()), File->length(), dds_flags, &IMG, texture); FAILED(hr))
        {
            Msg("! Failed to load DDS texture from memory: [%s], hr: [%d]", fn, hr);
            break;
        }

        // DirectX requires compressed texture size to be
        // a multiple of 4. Make sure to meet this requirement.
        if (DirectX::IsCompressed(IMG.format))
        {
            IMG.width = (IMG.width + 3u) & ~0x3u;
            IMG.height = (IMG.height + 3u) & ~0x3u;
        }

        const auto hr = CreateTextureEx(HW.pDevice, texture.GetImages(), texture.GetImageCount(), IMG, D3D_USAGE_IMMUTABLE, D3D_BIND_SHADER_RESOURCE, 0, IMG.miscFlags,
                                        DirectX::CREATETEX_DEFAULT, &pTexture2D);

        if (SUCCEEDED(hr))
        {
            // Получилось. Считаем сколько весит текстура и сваливаем.
            ret_msize = texture.GetImages()[0].slicePitch * IMG.arraySize * IMG.depth;
            break;
        }

        if (!allowFallback)
        {
            Msg("! Failed CreateTextureEx: [%s], hr: [%d]", fn, hr);
            break; // Уже была вторая попытка, прекращаем.
        }

        // Помянем, не получилось загрузить текстуру...
        // Давай заново, с конвертацией текстур. Может помочь.
        dds_flags |= DirectX::DDS_FLAGS::DDS_FLAGS_NO_16BPP | DirectX::DDS_FLAGS_FORCE_RGB;
        allowFallback = false;
    } while (true);

    FS.r_close(File);

    return pTexture2D;
}