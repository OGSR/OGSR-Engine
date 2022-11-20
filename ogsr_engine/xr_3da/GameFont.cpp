#include "stdafx.h"
#pragma hdrstop

#include "GameFont.h"
#include "Render.h"

extern ENGINE_API BOOL g_bRendering;

ENGINE_API Fvector2 g_current_font_scale = {1.f, 1.f};

ENGINE_API float g_fontWidthScale = 1.f;
ENGINE_API float g_fontHeightScale = 1.f;

#include "../Include/xrAPI/xrAPI.h"
#include "../Include/xrRender/RenderFactory.h"
#include "../Include/xrRender/FontRender.h"

CGameFont::CGameFont(LPCSTR section, u32 flags)
{
    pFontRender = RenderFactory->CreateFontRender();
    fCurrentHeight = 0.0f;
    fXStep = 0.0f;
    fYStep = 0.0f;
    uFlags = flags;
    nNumChars = 0x100;
    TCMap = NULL;

    Initialize(pSettings->r_string(section, "shader"), pSettings->r_string(section, "texture"));

    if (pSettings->line_exist(section, "size"))
    {
        float sz = pSettings->r_float(section, "size");
        if (uFlags & fsDeviceIndependent)
            SetHeightI(sz);
        else
            SetHeight(sz);
    }
    if (pSettings->line_exist(section, "interval"))
        SetInterval(pSettings->r_fvector2(section, "interval"));
}

CGameFont::CGameFont(LPCSTR shader, LPCSTR texture, u32 flags)
{
    pFontRender = RenderFactory->CreateFontRender();
    fCurrentHeight = 0.0f;
    fXStep = 0.0f;
    fYStep = 0.0f;
    uFlags = flags;
    nNumChars = 0x100;
    TCMap = NULL;

    Initialize(shader, texture);
}

void CGameFont::Initialize(LPCSTR cShader, LPCSTR cTextureName)
{
    string_path cTexture;

    LPCSTR _lang = pSettings->r_string("string_table", "font_prefix");
    bool is_di = strstr(cTextureName, "ui_font_hud_01") || strstr(cTextureName, "ui_font_hud_02") || strstr(cTextureName, "ui_font_console_02");
    if (_lang && !is_di)
        strconcat(sizeof(cTexture), cTexture, cTextureName, _lang);
    else
        xr_strcpy(cTexture, sizeof(cTexture), cTextureName);

    uFlags &= ~fsValid;
    vTS.set(1.f, 1.f); // обязательно !!!

    eCurrentAlignment = alLeft;
    vInterval.set(1.f, 1.f);

    strings.reserve(128);

    // check ini exist
    string_path fn, buf;
    xr_strcpy(buf, cTexture);
    if (strext(buf))
        *strext(buf) = 0;
    R_ASSERT2(FS.exist(fn, "$game_textures$", buf, ".ini"), fn);

    CInifile _ini{fn, TRUE};
    CInifile* ini = &_ini;

    nNumChars = 0x100;
    TCMap = (Fvector*)xr_realloc((void*)TCMap, nNumChars * sizeof(Fvector));

    if (ini->section_exist("symbol_coords"))
    {
        //float d = READ_IF_EXISTS(ini, r_float, "width_correction", "value", 0.0f); // Это раньше не работало, теперь работает, и чтоб старые кривые конфиги не работали криво, имя параметра переименовано.
        float d = READ_IF_EXISTS(ini, r_float, "font_width_correction", "value", 0.0f);

        fHeight = ini->r_float("symbol_coords", "height");

        for (u32 i = 0; i < nNumChars; i++)
        {
            xr_sprintf(buf, sizeof(buf), "%03d", i);
            Fvector v = ini->r_fvector3("symbol_coords", buf);
            TCMap[i].set(v.x, v.y, v.z - v.x + d);
        }
    }
    else
    {
        if (ini->section_exist("char widths"))
        {
            fHeight = ini->r_float("char widths", "height");

            const int cpl = 16;

            for (u32 i = 0; i < nNumChars; i++)
            {
                xr_sprintf(buf, sizeof(buf), "%d", i);
                float w = ini->r_float("char widths", buf); // ширина одного знака

                TCMap[i].set((i % cpl) * fHeight, (i / cpl) * fHeight, w);
            }
        }
        else
        {
            R_ASSERT(ini->section_exist("font_size"));

            fHeight = ini->r_float("font_size", "height");

            float width = ini->r_float("font_size", "width");
            const int cpl = ini->r_s32("font_size", "cpl");

            for (u32 i = 0; i < nNumChars; i++)
            {
                TCMap[i].set((i % cpl) * width, (i / cpl) * fHeight, width);
            }
        }
    }

    fCurrentHeight = fHeight;

    // Shading
    pFontRender->Initialize(cShader, cTexture);
}

CGameFont::~CGameFont()
{
    if (TCMap)
        xr_free(TCMap);

    // Shading
    RenderFactory->DestroyFontRender(pFontRender);
}

#define DI2PX(x) float(((x + 1.f) * float(::Render->getTarget()->get_width()) * 0.5f))
#define DI2PY(y) float(((y + 1.f) * float(::Render->getTarget()->get_height()) * 0.5f))

void CGameFont::OutSet(float x, float y)
{
    fCurrentX = x;
    fCurrentY = y;
}

void CGameFont::OutSetI(float x, float y) { OutSet(DI2PX(x), DI2PY(y)); }

void CGameFont::OnRender()
{
    pFontRender->OnRender(*this);
    strings.clear();
}

void CGameFont::MasterOut(BOOL bCheckDevice, BOOL bUseCoords, BOOL bScaleCoords, BOOL bUseSkip, float _x, float _y, float _skip, LPCSTR fmt, va_list p)
{
    if (bCheckDevice && (!RDEVICE.b_is_Active))
        return;

    String rs;

    rs.x = (bUseCoords ? (bScaleCoords ? (DI2PX(_x)) : _x) : fCurrentX);
    rs.y = (bUseCoords ? (bScaleCoords ? (DI2PY(_y)) : _y) : fCurrentY);
    rs.c = dwCurrentColor;
    rs.height = fCurrentHeight;
    rs.align = eCurrentAlignment;

    int vs_sz = vsprintf_s(rs.string, fmt, p);
    // VERIFY( ( vs_sz != -1 ) && ( rs.string[ vs_sz ] == '\0' ) );

    rs.string[sizeof(rs.string) - 1] = 0;
    if (vs_sz == -1)
    {
        return;
    }

    if (vs_sz)
        strings.push_back(rs);

    if (bUseSkip)
        OutSkip(_skip);
}

#define MASTER_OUT(CHECK_DEVICE, USE_COORDS, SCALE_COORDS, USE_SKIP, X, Y, SKIP, FMT) \
    { \
        va_list p; \
        va_start(p, fmt); \
        MasterOut(CHECK_DEVICE, USE_COORDS, SCALE_COORDS, USE_SKIP, X, Y, SKIP, FMT, p); \
        va_end(p); \
    }

void __cdecl CGameFont::OutI(float _x, float _y, LPCSTR fmt, ...) { MASTER_OUT(FALSE, TRUE, TRUE, FALSE, _x, _y, 0.0f, fmt); };

void __cdecl CGameFont::Out(float _x, float _y, LPCSTR fmt, ...) { MASTER_OUT(TRUE, TRUE, FALSE, FALSE, _x, _y, 0.0f, fmt); };

void __cdecl CGameFont::OutNext(LPCSTR fmt, ...) { MASTER_OUT(TRUE, FALSE, FALSE, TRUE, 0.0f, 0.0f, 1.0f, fmt); };

void CGameFont::OutSkip(float val) { fCurrentY += val * CurrentHeight_(); }

float CGameFont::SizeOf_(const char cChar) { return (GetCharTC((u16)(u8)(cChar)).z * vInterval.x * GetWidthScale()); }

float CGameFont::SizeOf_(LPCSTR s)
{
    if (!(s && s[0]))
        return 0;

    int len = xr_strlen(s);
    float X = 0;
    if (len)
        for (int j = 0; j < len; j++)
            X += GetCharTC((u16)(u8)s[j]).z;

    return (X * vInterval.x) * GetWidthScale();
}

float CGameFont::CurrentHeight_() { return fCurrentHeight * vInterval.y * GetHeightScale(); }

void CGameFont::SetHeightI(float S)
{
    VERIFY(uFlags & fsDeviceIndependent);
    fCurrentHeight = S * RDEVICE.dwHeight;
};

void CGameFont::SetHeight(float S)
{
    VERIFY(!uFlags & fsDeviceIndependent);
    fCurrentHeight = S;
};

float CGameFont::GetWidthScale()
{
    if (uFlags & fsDeviceIndependent)
        return 1.f;

    return g_fontWidthScale * (!fis_zero(fXScale) ? fXScale : 1);
}
float CGameFont::GetHeightScale()
{
    if (uFlags & fsDeviceIndependent)
        return 1.f;

    return g_fontHeightScale * (!fis_zero(fYScale) ? fYScale : 1);
}