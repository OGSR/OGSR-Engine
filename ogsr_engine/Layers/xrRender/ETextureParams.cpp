#include "stdafx.h"
#pragma hdrstop

#include "ETextureParams.h"

xr_token tparam_token[] = {{"Advanced", STextureParams::kMIPFilterAdvanced},

                           {"Point", STextureParams::kMIPFilterPoint},
                           {"Box", STextureParams::kMIPFilterBox},
                           {"Triangle", STextureParams::kMIPFilterTriangle},
                           {"Quadratic", STextureParams::kMIPFilterQuadratic},
                           {"Cubic", STextureParams::kMIPFilterCubic},

                           {"Catrom", STextureParams::kMIPFilterCatrom},
                           {"Mitchell", STextureParams::kMIPFilterMitchell},

                           {"Gaussian", STextureParams::kMIPFilterGaussian},
                           {"Sinc", STextureParams::kMIPFilterSinc},
                           {"Bessel", STextureParams::kMIPFilterBessel},

                           {"Hanning", STextureParams::kMIPFilterHanning},
                           {"Hamming", STextureParams::kMIPFilterHamming},
                           {"Blackman", STextureParams::kMIPFilterBlackman},
                           {"Kaiser", STextureParams::kMIPFilterKaiser},
                           {0, 0}};

xr_token ttype_token[] = {{"2D Texture", STextureParams::ttImage},     {"Cube Map", STextureParams::ttCubeMap}, {"Bump Map", STextureParams::ttBumpMap},
                          {"Normal Map", STextureParams::ttNormalMap}, {"Terrain", STextureParams::ttTerrain},  {0, 0}};

xr_token tfmt_token[] = {{"DXT1", STextureParams::tfDXT1},
                         {"DXT1 Alpha", STextureParams::tfADXT1},
                         {"DXT3", STextureParams::tfDXT3},
                         {"DXT5", STextureParams::tfDXT5},
                         {"16 bit (1:5:5:5)", STextureParams::tf1555},
                         {"16 bit (5:6:5)", STextureParams::tf565},
                         {"32 bit (8:8:8:8)", STextureParams::tfRGBA},
                         {"8 bit (alpha)", STextureParams::tfA8},
                         {"8 bit (luminance)", STextureParams::tfL8},
                         {"16 bit (alpha:luminance)", STextureParams::tfA8L8},
                         {0, 0}};

xr_token tmtl_token[] = {{"OrenNayar <-> Blin", STextureParams::tmOrenNayar_Blin},
                         {"Blin <-> Phong", STextureParams::tmBlin_Phong},
                         {"Phong <-> Metal", STextureParams::tmPhong_Metal},
                         {"Metal <-> OrenNayar", STextureParams::tmMetal_OrenNayar},
                         {0, 0}};

xr_token tbmode_token[] = {{"None", STextureParams::tbmNone}, {"Use", STextureParams::tbmUse}, {"Use parallax", STextureParams::tbmUseParallax}, {0, 0}};

void STextureParams::Load(IReader& F, const char* dbg_name)
{
    R_ASSERT(F.find_chunk_thm(THM_CHUNK_TEXTUREPARAM, dbg_name));
    F.r(&fmt, sizeof(ETFormat));
    flags.assign(F.r_u32());
    border_color = F.r_u32();
    fade_color = F.r_u32();
    fade_amount = F.r_u32();
    mip_filter = F.r_u32();
    width = F.r_u32();
    height = F.r_u32();

    if (F.find_chunk_thm(THM_CHUNK_TEXTURE_TYPE, dbg_name))
    {
        type = (ETType)F.r_u32();
    }

    if (F.find_chunk_thm(THM_CHUNK_DETAIL_EXT, dbg_name))
    {
        F.r_stringZ(detail_name);
        detail_scale = F.r_float();
    }

    if (F.find_chunk_thm(THM_CHUNK_MATERIAL, dbg_name))
    {
        material = (ETMaterial)F.r_u32();
        material_weight = F.r_float();
    }

    if (F.find_chunk_thm(THM_CHUNK_BUMP, dbg_name))
    {
        bump_virtual_height = F.r_float();
        bump_mode = (ETBumpMode)F.r_u32();
        if (bump_mode < STextureParams::tbmNone)
        {
            bump_mode = STextureParams::tbmNone; //.. временно (до полного убирания Autogen)
        }
        F.r_stringZ(bump_name);
    }

    if (F.find_chunk_thm(THM_CHUNK_EXT_NORMALMAP, dbg_name))
        F.r_stringZ(ext_normal_map_name);

    if (F.find_chunk_thm(THM_CHUNK_FADE_DELAY, dbg_name))
        fade_delay = F.r_u8();
}

void STextureParams::Save(IWriter& F)
{
    F.open_chunk(THM_CHUNK_TEXTUREPARAM);
    F.w(&fmt, sizeof(ETFormat));
    F.w_u32(flags.get());
    F.w_u32(border_color);
    F.w_u32(fade_color);
    F.w_u32(fade_amount);
    F.w_u32(mip_filter);
    F.w_u32(width);
    F.w_u32(height);
    F.close_chunk();

    F.open_chunk(THM_CHUNK_TEXTURE_TYPE);
    F.w_u32(type);
    F.close_chunk();

    F.open_chunk(THM_CHUNK_DETAIL_EXT);
    F.w_stringZ(detail_name);
    F.w_float(detail_scale);
    F.close_chunk();

    F.open_chunk(THM_CHUNK_MATERIAL);
    F.w_u32(material);
    F.w_float(material_weight);
    F.close_chunk();

    F.open_chunk(THM_CHUNK_BUMP);
    F.w_float(bump_virtual_height);
    F.w_u32(bump_mode);
    F.w_stringZ(bump_name);
    F.close_chunk();

    F.open_chunk(THM_CHUNK_EXT_NORMALMAP);
    F.w_stringZ(ext_normal_map_name);
    F.close_chunk();

    F.open_chunk(THM_CHUNK_FADE_DELAY);
    F.w_u8(fade_delay);
    F.close_chunk();
}
