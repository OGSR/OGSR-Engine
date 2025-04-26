#include "stdafx.h"

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
    uFlags = flags;

    Initialize(pSettings->r_string(section, "shader"), pSettings->r_string(section, "texture"), section);

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

CGameFont::CGameFont(LPCSTR shader, LPCSTR texture, const char* section, u32 flags)
{
    pFontRender = RenderFactory->CreateFontRender();
    fCurrentHeight = 0.0f;
    fXStep = 0.0f;
    uFlags = flags;

    Initialize(shader, texture, section);
}

void CGameFont::Initialize(LPCSTR cShader, LPCSTR cTextureName, const char* sect)
{
    string_path cTexture;

    LPCSTR _lang = pSettings->r_string("string_table", "font_prefix");
    const bool skip_prefix = READ_IF_EXISTS(pSettings, r_bool, sect, "skip_prefix", false) || strstr(cTextureName, "ui_font_hud_01") || strstr(cTextureName, "ui_font_hud_02") ||
        strstr(cTextureName, "ui_font_console_02");

    if (_lang && !skip_prefix)
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

    const bool is_mb = ini->section_exist("mb_symbol_coords");
    const u32 nNumChars = is_mb ? (std::numeric_limits<unsigned short>::max() + 1) : (std::numeric_limits<unsigned char>::max() + 1);
    TCMap = std::make_unique<Fvector[]>(nNumChars);

    // float fwc = READ_IF_EXISTS(ini, r_float, "width_correction", "value", 0.0f);
    // Это раньше не работало, теперь работает, и чтоб старые кривые конфиги не работали криво, имя
    // параметра переименовано.
    const float fwc = READ_IF_EXISTS(ini, r_float, "font_width_correction", "value", 0.0f);

    if (is_mb)
    {
        uFlags |= fsMultibyte;
        fHeight = ini->r_float("mb_symbol_coords", "height");

        fXStep = ceil(fHeight / 2.0f); // условная ширина пробела

        // Searching for the first valid character

        Fvector vFirstValid = {0, 0, 0};

        if (ini->line_exist("mb_symbol_coords", "09608"))
        {
            Fvector v = ini->r_fvector3("mb_symbol_coords", "09608");
            vFirstValid.set(v.x, v.y, v.z - v.x + fwc);
        }
        else
            for (u32 i = 0; i < nNumChars; i++)
            {
                xr_sprintf(buf, sizeof(buf), "%05d", i);
                if (ini->line_exist("mb_symbol_coords", buf))
                {
                    Fvector v = ini->r_fvector3("mb_symbol_coords", buf);
                    vFirstValid.set(v.x, v.y, v.z - v.x + fwc);
                    break;
                }
            }

        // Filling entire character table

        for (u32 i = 0; i < nNumChars; i++)
        {
            xr_sprintf(buf, sizeof(buf), "%05d", i);
            if (ini->line_exist("mb_symbol_coords", buf))
            {
                Fvector v = ini->r_fvector3("mb_symbol_coords", buf);
                TCMap[i].set(v.x, v.y, v.z - v.x + fwc);

                if (i == 0x0020)
                    fXStep = v.z - v.x + fwc; // реальная ширина пробела в шрифте
            }
            else
                TCMap[i] = vFirstValid; // "unassigned" unprintable characters
        }


        // Special case for space
        TCMap[0x0020].set(0, 0, 0);
        // Special case for ideographic space
        TCMap[0x3000].set(0, 0, 0);

        // костыль что б mb font можно было использовать без конвертации ресурсов в utf8
        static const bool enable_ansi_aupport_for_mb_fonts = READ_IF_EXISTS(pSettings, r_bool, "features", "enable_ansi_aupport_for_mb_fonts", true);
        if (enable_ansi_aupport_for_mb_fonts)
        {
            // add remap for local rus
            for (u16 i = 0; i < 256; i++)
                TCMap[0xC0 + i] = TCMap[0x410 + i];

            // Ёё
            TCMap[0xA8] = TCMap[0x401];
            TCMap[0xB8] = TCMap[0x451];

            // №—
            TCMap[0xB9] = TCMap[0x2116];
            TCMap[0x97] = TCMap[0x2014];

            // ҐґЄєІіЇї
            TCMap[0xA5] = TCMap[0x490];
            TCMap[0xB4] = TCMap[0x491];

            TCMap[0xAA] = TCMap[0x404];
            TCMap[0xBA] = TCMap[0x454];

            TCMap[0xB2] = TCMap[0x406];
            TCMap[0xB3] = TCMap[0x456];

            TCMap[0xAF] = TCMap[0x407];
            TCMap[0xBF] = TCMap[0x457];
        }
    }
    else if (ini->section_exist("symbol_coords"))
    {
        fHeight = ini->r_float("symbol_coords", "height");

        for (u32 i = 0; i < nNumChars; i++)
        {
            xr_sprintf(buf, sizeof(buf), "%03d", i);
            Fvector v = ini->r_fvector3("symbol_coords", buf);
            TCMap[i].set(v.x, v.y, v.z - v.x + fwc);
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
    // Shading
    RenderFactory->DestroyFontRender(pFontRender);
}

static inline float DI2PX(float x)
{
    auto& cmd_list = ::Render->get_imm_command_list();
    return float(iFloor((x + 1) * float(::Render->getTarget()->get_width(cmd_list)) * 0.5f));
}
static inline float DI2PY(float y)
{
    auto& cmd_list = ::Render->get_imm_command_list();
    return float(iFloor((y + 1) * float(::Render->getTarget()->get_height(cmd_list)) * 0.5f));
}

void CGameFont::OutSet(float x, float y)
{
    fCurrentX = x;
    fCurrentY = y;
}

void CGameFont::OutSetI(float x, float y) { OutSet(DI2PX(x), DI2PY(y)); }

u32 CGameFont::SmartLength(const char* S) { return (IsMultibyte() ? mbhMulti2Wide(NULL, NULL, 0, S) : xr_strlen(S)); }

void CGameFont::OnRender()
{
    pFontRender->OnRender(*this);
    strings.clear();
}

u16 CGameFont::GetCutLengthPos(float fTargetWidth, const char* pszText)
{
    VERIFY(pszText);

    float fCurWidth = 0.0f, fDelta = 0.0f;

    // vInterval.x  ???

    wide_char wsStr[MAX_MB_CHARS], wsPos[MAX_MB_CHARS];

    u16 len = mbhMulti2Wide(wsStr, wsPos, MAX_MB_CHARS, pszText);
    u16 i = 1;

    for (; i <= len; i++)
    {
        fDelta = (GetCharTC(wsStr[i]).z * GetWidthScale());

        if (IsSpaceCharacter(wsStr[i]))
            fDelta += GetfXStep() * GetWidthScale();

        if ((fCurWidth + fDelta) > fTargetWidth)
            break;
        else
            fCurWidth += fDelta;
    }

    return wsPos[i - 1];
}

u16 CGameFont::SplitByWidth(u16* puBuffer, u16 uBufferSize, float fTargetWidth, const char* pszText)
{
    VERIFY(puBuffer && uBufferSize && pszText);

    wide_char wsStr[MAX_MB_CHARS], wsPos[MAX_MB_CHARS];
    float fCurWidth = 0.0f, fDelta = 0.0f;

    u16 len = mbhMulti2Wide(wsStr, wsPos, MAX_MB_CHARS, pszText);
    u16 nLines = 0;

    // vInterval.x  ???

    for (u16 i = 1; i <= len; i++)
    {
        fDelta = GetCharTC(wsStr[i]).z * GetWidthScale();

        if (IsSpaceCharacter(wsStr[i]))
            fDelta += GetfXStep() * GetWidthScale();

        if (fCurWidth + fDelta > fTargetWidth && // overlength
            !IsBadStartCharacter(wsStr[i]) && // can start with this character
            (i < len) && // is not the last character
            (i > 1 && (!IsBadEndCharacter(wsStr[i - 1]))) // && // do not stop the string on a "bad" character
        )
        {
            fCurWidth = fDelta;

            VERIFY(nLines < uBufferSize);

            puBuffer[nLines++] = wsPos[i - 1];
        }
        else
            fCurWidth += fDelta;
    }

    return nLines;
}

void CGameFont::MasterOut(BOOL bCheckDevice, BOOL bUseCoords, BOOL bScaleCoords, BOOL bUseSkip, float _x, float _y, float _skip, LPCSTR fmt, va_list p)
{
    if (bCheckDevice && (!Device.b_is_Active))
        return;

    String rs;

    rs.x = (bUseCoords ? (bScaleCoords ? (DI2PX(_x)) : _x) : fCurrentX);
    rs.y = (bUseCoords ? (bScaleCoords ? (DI2PY(_y)) : _y) : fCurrentY);
    rs.color = dwCurrentColor;
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

float CGameFont::SizeOf_(const char cChar)
{
    float x = GetCharTC((u16)(u8)(cChar)).z;
    if (IsMultibyte() && IsSpaceCharacter((u16)(u8)(cChar)))
        x += GetfXStep();
    return x * vInterval.x * GetWidthScale();
}

float CGameFont::SizeOf_(const u16 cChar)
{
    float x = GetCharTC(cChar).z;
    if (IsMultibyte() && IsSpaceCharacter(cChar))
        x += GetfXStep();
    return x * vInterval.x * GetWidthScale();
}

float CGameFont::SizeOf_(LPCSTR s)
{
    if (!(s && s[0]))
        return 0;

    if (IsMultibyte())
    {
        wide_char wsStr[MAX_MB_CHARS];

        mbhMulti2Wide(wsStr, NULL, MAX_MB_CHARS, s);

        return SizeOf_(wsStr);
    }

    int len = xr_strlen(s);
    float X = 0;
    if (len)
        for (int j = 0; j < len; j++)
            X += GetCharTC((u16)(u8)s[j]).z;

    return (X * vInterval.x) * GetWidthScale();
}

float CGameFont::SizeOf_(const wide_char* wsStr)
{
    if (!(wsStr && wsStr[0]))
        return 0;

    unsigned int len = wsStr[0];
    float X = 0.0f, fDelta = 0.0f;

    if (len)
        for (unsigned int j = 1; j <= len; j++)
        {
            fDelta = GetCharTC(wsStr[j]).z;

            if (IsSpaceCharacter(wsStr[j]))
                fDelta += GetfXStep();
            X += fDelta;
        }

    return (X * vInterval.x) * GetWidthScale();
}

float CGameFont::CurrentHeight_() { return fCurrentHeight * vInterval.y * GetHeightScale(); }

void CGameFont::SetHeightI(float S)
{
    VERIFY(uFlags & fsDeviceIndependent);
    fCurrentHeight = S * Device.dwHeight;
};

void CGameFont::SetHeight(float S)
{
    VERIFY(!uFlags & fsDeviceIndependent);
    fCurrentHeight = S;
};

float CGameFont::GetWidthScale() const
{
    if (uFlags & fsDeviceIndependent)
        return 1.f;

    return g_fontWidthScale * (!fis_zero(fXScale) ? fXScale : 1);
}
float CGameFont::GetHeightScale() const
{
    if (uFlags & fsDeviceIndependent)
        return 1.f;

    return g_fontHeightScale * (!fis_zero(fYScale) ? fYScale : 1);
}