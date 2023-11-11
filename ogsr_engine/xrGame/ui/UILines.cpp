// File:		UILines.cpp
// Description:	Multi-line Text Control
// Created:		12.03.2005
// Author:		Serge Vynnycheko
// Mail:		narrator@gsc-game.kiev.ua
//
// Copyright 2005 GSC Game World

#include "StdAfx.h"

#include "UILines.h"
#include "../HUDmanager.h"
#include "UIXmlInit.h"
#include "uilinestd.h"

CUILines::CUILines()
{
    m_pFont = NULL;
    m_interval = 0.0f;
    m_eTextAlign = CGameFont::alLeft;
    m_eVTextAlign = valTop;
    m_dwTextColor = 0xffffffff;
    m_dwCursorColor = 0xAAFFFF00;

    m_bShowMe = true;

    uFlags.zero();
    uFlags.set(flNeedReparse, FALSE);
    uFlags.set(flComplexMode, FALSE);
    uFlags.set(flPasswordMode, FALSE);
    uFlags.set(flColoringMode, TRUE);
    uFlags.set(flRecognizeNewLine, TRUE);

    m_pFont = UI()->Font()->pFontLetterica16Russian;
    m_cursor_pos.set(0, 0);
    m_iCursorPos = 0;
    m_bDrawCursor = false;
    m_wndSize.set(0.f, 0.f);
    m_wndPos.set(0.f, 0.f);
}

CUILines::~CUILines() {}

void CUILines::SetTextComplexMode(bool mode)
{
    uFlags.set(flComplexMode, mode);
    if (mode)
        uFlags.set(flPasswordMode, FALSE);
}

bool CUILines::GetTextComplexMode() const { return uFlags.test(flComplexMode) ? true : false; }

void CUILines::SetPasswordMode(bool mode)
{
    uFlags.set(flPasswordMode, mode);
    if (mode)
        uFlags.set(flComplexMode, false);
}

void CUILines::SetColoringMode(bool mode) { uFlags.set(flColoringMode, mode); }

void CUILines::SetUseNewLineMode(bool mode) { uFlags.set(flRecognizeNewLine, mode); }

void CUILines::Init(float x, float y, float width, float heigt) { CUISimpleWindow::Init(x, y, width, heigt); }

void CUILines::SetText(LPCSTR text)
{
    if (!m_pFont)
        m_pFont = UI()->Font()->pFontLetterica16Russian;

    if (text && text[0] != 0)
    {
        if (m_text == text)
            return;

        m_text = text;
        m_text_mask.clear();
        for (auto it = m_text.cbegin(), it_end = m_text.cend(); it != it_end; ++it)
        {
            if (m_pFont->IsMultibyte() && *reinterpret_cast<const unsigned char*>(&*it) > std::numeric_limits<char>::max() &&
                (it + 1) != it_end) // Костыльный поиск юникодных символов. Но для хрея пойдет, тут в рендере юникодного текста и так костыль на костыле
            {
                m_text_mask.emplace_back(true);
                m_text_mask.emplace_back(true);
                ++it;
            }
            else
            {
                m_text_mask.emplace_back(false);
            }
        }

        uFlags.set(flNeedReparse, TRUE);
    }
    else
    {
        m_text = "";
        m_text_mask.clear();

        Reset();
    }
    MoveCursorToEnd();
}

void CUILines::AddCharAtCursor(const u16 ch)
{
    if (m_pFont->IsMultibyte() && ch > std::numeric_limits<char>::max())
    {
        auto byte = reinterpret_cast<const char*>(&ch);
        m_text_mask.insert(m_text_mask.begin() + m_iCursorPos, true);
        m_text.insert(m_text.begin() + (m_iCursorPos++), *byte);
        m_text_mask.insert(m_text_mask.begin() + m_iCursorPos, true);
        m_text.insert(m_text.begin() + (m_iCursorPos++), *(++byte));
    }
    else
    {
        m_text_mask.insert(m_text_mask.begin() + m_iCursorPos, false);
        m_text.insert(m_text.begin() + (m_iCursorPos++), ch);
    }

    uFlags.set(flNeedReparse, TRUE);
}

void CUILines::MoveCursorToEnd() { m_iCursorPos = (int)m_text.size(); }

void CUILines::DelLeftChar()
{
    if (m_text.empty() || m_iCursorPos <= 0)
        return;

    if (m_text_mask.at(m_iCursorPos - 1))
    {
        m_text.erase(m_text.begin() + m_iCursorPos - 2, m_text.begin() + m_iCursorPos);
        m_text_mask.erase(m_text_mask.begin() + m_iCursorPos - 2, m_text_mask.begin() + m_iCursorPos);
        m_iCursorPos -= 2;
    }
    else
    {
        m_text.erase(m_text.begin() + m_iCursorPos - 1);
        m_text_mask.erase(m_text_mask.begin() + m_iCursorPos - 1);
        m_iCursorPos -= 1;
    }

    uFlags.set(flNeedReparse, TRUE);
}

void CUILines::DelCurrentChar()
{
    if (m_text.empty() || m_iCursorPos >= m_text.size())
        return;

    if (m_text_mask.at(m_iCursorPos))
    {
        m_text.erase(m_text.begin() + m_iCursorPos, m_text.begin() + m_iCursorPos + 2);
        m_text_mask.erase(m_text_mask.begin() + m_iCursorPos, m_text_mask.begin() + m_iCursorPos + 2);
    }
    else
    {
        m_text.erase(m_text.begin() + m_iCursorPos);
        m_text_mask.erase(m_text_mask.begin() + m_iCursorPos);
    }

    uFlags.set(flNeedReparse, TRUE);
}

LPCSTR CUILines::GetText() { return m_text.c_str(); }

void CUILines::Reset() { m_lines.clear(); }

void CUILines::ParseText()
{
    if (!fsimilar(m_oldWidth, m_wndSize.x))
    {
        m_oldWidth = m_wndSize.x;

        uFlags.set(flNeedReparse, TRUE);
    }

    if (!uFlags.test(flComplexMode) || !uFlags.test(flNeedReparse))
        return;

    if (!m_pFont)
        return;

    Reset();

    if (!m_text.empty() && !m_pFont)
        R_ASSERT2(false, "can't parse text without font");

    CUILine* line{};

    if (uFlags.test(flColoringMode))
    {
        line = ParseTextToColoredLine(m_text);
    }
    else
    {
        line = xr_new<CUILine>();
        line->AddSubLine(m_text, GetTextColor());
    }

    bool bNewLines{};
    static const bool use_new_parse_text = READ_IF_EXISTS(pSettings, r_bool, "features", "use_new_parse_text", true);

    if (uFlags.test(flRecognizeNewLine))
    {
        if (!use_new_parse_text && m_pFont->IsMultibyte())
        {
            CUILine* ptmp_line = xr_new<CUILine>();

            for (const auto& subline : line->m_subLines)
            {
                const u32 tcolor = subline.m_color;
                xr_string szTempLine = subline.m_text;
                char* pszSearch = szTempLine.data();
                while (char* pszTemp = strstr(pszSearch, "\\n"))
                {
                    bNewLines = true;
                    *pszTemp = '\0';
                    ptmp_line->AddSubLine(xr_string{pszSearch}, tcolor);
                    pszSearch = pszTemp + 2;
                }
                ptmp_line->AddSubLine(xr_string{pszSearch}, tcolor);
            }
            xr_delete(line);
            line = ptmp_line;
        }
        else
        {
            line->ProcessNewLines(); // process "\n"
        }
    }

    if (!use_new_parse_text && m_pFont->IsMultibyte())
    {
        float fTargetWidth = 1.0f;
        UI()->ClientToScreenScaledWidth(fTargetWidth);
        VERIFY((m_wndSize.x > 0) && (fTargetWidth > 0));
        fTargetWidth = m_wndSize.x / fTargetWidth;

        if (line->m_subLines.size() > 1 && !bNewLines)
        {
            // only colored line, pizdets
            auto& tmp_line = m_lines.emplace_back();
            for (const auto& subline : line->m_subLines)
            {
                xr_string pszText = subline.m_text;
                const u32 tcolor = subline.m_color;
                tmp_line.AddSubLine(std::move(pszText), tcolor);
            }
        }
        else
        {
            for (const auto& subline : line->m_subLines)
            {
                const char* pszText = subline.m_text.c_str();
                VERIFY(pszText);
                const u32 tcolor = subline.m_color;

                u16 aMarkers[1000]{};
                u16 uFrom{}, uPartLen{};
                const u16 nMarkers = m_pFont->SplitByWidth(aMarkers, std::size(aMarkers), fTargetWidth, pszText);
                for (u16 j = 0; j < nMarkers; j++)
                {
                    uPartLen = aMarkers[j] - uFrom;
                    VERIFY(uPartLen > 0);
                    xr_string szTempLine{pszText + uFrom, uPartLen};
                    auto& tmp_line = m_lines.emplace_back();
                    tmp_line.AddSubLine(std::move(szTempLine), tcolor);
                    uFrom += uPartLen;
                }

                xr_string szTempLine{pszText + uFrom};
                auto& tmp_line = m_lines.emplace_back();
                tmp_line.AddSubLine(std::move(szTempLine), tcolor);
            }
        }
    }
    else
    {
        constexpr float __eps = 5.f;

        const float max_width = m_wndSize.x;
        float curr_width{};

        CUILine tmp_line;

        const size_t sbl_cnt = line->m_subLines.size();
        for (size_t sbl_idx{}; sbl_idx < sbl_cnt; ++sbl_idx)
        {
            const bool b_last_subl = sbl_idx == sbl_cnt - 1;
            CUISubLine& sbl = line->m_subLines[sbl_idx];
            const size_t sub_len = sbl.m_text.length();

            size_t curr_w_pos{}, last_space_idx{};

            for (size_t idx{}; idx < sub_len;)
            {
                bool is_wide_char =
                    m_pFont->IsMultibyte() && *reinterpret_cast<const unsigned char*>(&sbl.m_text[idx]) > std::numeric_limits<char>::max() && (idx + 1) < sub_len;

                auto get_str_width = [](CGameFont* pFont, const char ch) {
                    float fDelta = pFont->SizeOf_(ch);
                    return fDelta;
                };

                auto get_wstr_width = [&](CGameFont* pFont, const char* ch) {
                    u16 wchar{};
                    const int wchars_num = MultiByteToWideChar(CP_UTF8, 0, ch, 2, reinterpret_cast<LPWSTR>(&wchar), 1);

                    // Msg("--MultiByteToWideChar returned symbol [%u]", wchar);

                    if (wchars_num < 1) // Такое бывает на неподдерживаемых языках кроме rus/eng, мб ещё в каких то редких случаях
                    {
                        // Msg("!!--MultiByteToWideChar returned 0 symbols. Something strange! String: [%s]", ch);
                        is_wide_char = false;
                        return get_str_width(pFont, *ch);
                    }

                    float fDelta = pFont->SizeOf_(wchar);
                    return fDelta;
                };

                /*
                if (is_wide_char)
                    Msg("--Size of W symbol [%d] is [%f], curr_width: [%f], max_width: [%f]", sbl.m_text[idx], get_Wstr_width(m_pFont, &sbl.m_text[idx]), curr_width, max_width);
                else
                    Msg("~~Size of A symbol [%d] is [%f], curr_width: [%f], max_width: [%f]", sbl.m_text[idx], get_str_width(m_pFont, sbl.m_text[idx]), curr_width, max_width);
                */

                float char_width = is_wide_char ? get_wstr_width(m_pFont, &sbl.m_text[idx])
                                                : get_str_width(m_pFont, sbl.m_text[idx]);

                UI()->ClientToScreenScaledWidth(char_width);

                if (!is_wide_char && iswspace(sbl.m_text[idx]))
                {
                    last_space_idx = idx;
                }

                const bool bOver = curr_width + char_width + __eps > max_width;
                const bool b_last_ch = idx == sub_len - (is_wide_char ? 2 : 1);

                if (bOver || b_last_ch)
                {
                    if (last_space_idx && !b_last_ch)
                    {
                        idx = last_space_idx;
                        is_wide_char = false;
                        last_space_idx = 0;
                    }

                    xr_string buff{sbl.m_text.c_str() + curr_w_pos, idx - curr_w_pos + (is_wide_char ? 2 : 1)};
                    tmp_line.AddSubLine(std::move(buff), sbl.m_color);
                    curr_w_pos = idx + (is_wide_char ? 2 : 1);
                }
                else
                {
                    curr_width += char_width;
                }

                if (bOver || b_last_ch && sbl.m_last_in_line)
                {
                    m_lines.push_back(tmp_line);
                    tmp_line.Clear();
                    curr_width = 0.0f;
                }

                if (is_wide_char)
                {
                    ++idx;
                    ++idx;
                }
                else
                {
                    ++idx;
                }
            }

            if (b_last_subl && !tmp_line.IsEmpty())
            {
                m_lines.push_back(tmp_line);
                tmp_line.Clear();
                curr_width = 0.0f;
            }
        }
    }

    if (m_eTextAlign == CGameFont::alJustified)
    {
        for (auto& m_line : m_lines)
        {
            m_line.ProcessSpaces();
        }
    }

    xr_delete(line);
    uFlags.set(flNeedReparse, FALSE);
}

float CUILines::GetVisibleHeight()
{
    float _curr_h = m_pFont->CurrentHeight_();
    UI()->ClientToScreenScaledHeight(_curr_h);

    if (uFlags.test(flComplexMode))
    {
        if (uFlags.test(flNeedReparse))
            ParseText();

        return (_curr_h + m_interval) * m_lines.size() - m_interval;
    }
    else
        return _curr_h;
}

void CUILines::SetTextColor(u32 color)
{
    if (color == m_dwTextColor)
        return;
    uFlags.set(flNeedReparse, true);
    m_dwTextColor = color;
}

void CUILines::SetFont(CGameFont* pFont)
{
    if (pFont == m_pFont)
        return;
    uFlags.set(flNeedReparse, true);
    m_pFont = pFont;
}

void CUILines::Draw(float x, float y)
{
    static string256 passText;

    /* движок вроде нормально справляется с отображением пустой
       строки. Отдельная обработка для этого случая не нужна. А вот
       проблема с этой обработкой замечена. При некоторых положениях
       строки ввода, курсор не отображается в пустой строке.

        if (m_text.empty())
        {
            if (m_bDrawCursor)
            {
                UpdateCursor();

                DrawCursor(x + GetIndentByAlign(), y + GetVIndentByAlign());
            }

            return;
        }
    */

    R_ASSERT(m_pFont);
    m_pFont->SetColor(m_dwTextColor);

    if (!uFlags.is(flComplexMode))
    {
        Fvector2 text_pos;
        text_pos.set(0, 0);

        text_pos.x = x + GetIndentByAlign();
        text_pos.y = y + GetVIndentByAlign();

        UI()->ClientToScreenScaled(text_pos);

        if (uFlags.test(flPasswordMode))
        {
            int sz = (int)m_text.size();
            for (int i = 0; i < sz; i++)
                passText[i] = '*';

            passText[sz] = 0;

            m_pFont->SetAligment((CGameFont::EAligment)m_eTextAlign);
            m_pFont->Out(text_pos.x, text_pos.y, "%s", passText);
        }
        else
        {
            m_pFont->SetAligment((CGameFont::EAligment)m_eTextAlign);
            m_pFont->Out(text_pos.x, text_pos.y, "%s", m_text.c_str());
        }
    }
    else
    {
        ParseText();

        const float max_width = m_wndSize.x;

        Fvector2 pos;

        // get vertical indent
        pos.x = x + GetIndentByAlign();
        pos.y = y + GetVIndentByAlign();

        float height = m_pFont->CurrentHeight_();
        UI()->ClientToScreenScaledHeight(height);

        u32 size = m_lines.size();

        m_pFont->SetAligment((CGameFont::EAligment)m_eTextAlign);
        for (int i = 0; i < (int)size; i++)
        {
            m_lines[i].Draw(m_pFont, pos.x, pos.y, max_width);

            pos.y += height + m_interval;
        }
    }

    if (m_bDrawCursor)
    {
        UpdateCursor();

        DrawCursor(x + GetIndentByAlign(), y + GetVIndentByAlign());
    }

    m_pFont->OnRender();
}

void CUILines::DrawCursor(float x, float y)
{
    float _h = m_pFont->CurrentHeight_();
    UI()->ClientToScreenScaledHeight(_h);

    float _w_tmp = 0;

    if (!m_text.empty())
    {
        string4096 buff;
        if (!uFlags.is(flComplexMode))
        {
            strncpy_s(buff, m_text.c_str(), m_cursor_pos.x);
        }
        else
        {
            strcpy_s(buff, "");

            CUILine line = m_lines[m_cursor_pos.y];

            int sz = 0;
            int size = line.m_subLines.size();
            for (int i = 0; i < size; i++)
            {
                const int line_size = (int)line.m_subLines[i].m_text.size();
                const char* line_text = line.m_subLines[i].m_text.c_str();

                if (sz + line_size < m_cursor_pos.x)
                {
                    strcat_s(buff, line_text);
                    sz += line_size;
                }
                else
                {
                    strncat_s(buff, line_text, m_cursor_pos.x - sz);
                    break;
                }
            }
        }

        buff[m_cursor_pos.x] = 0;

        _w_tmp = m_pFont->SizeOf_(buff);
    }

    UI()->ClientToScreenScaledWidth(_w_tmp);

    Fvector2 outXY;
    outXY.x = x + _w_tmp;
    outXY.y = y + (_h + m_interval) * m_cursor_pos.y;

    UI()->ClientToScreenScaled(outXY);

    m_pFont->Out(outXY.x, outXY.y, "_");
}

void CUILines::Draw()
{
    Fvector2 p = GetWndPos();
    Draw(p.x, p.y);
}

void CUILines::Update() {}

void CUILines::OnDeviceReset() { uFlags.set(flNeedReparse, TRUE); }

float CUILines::GetIndentByAlign() const
{
    switch (m_eTextAlign)
    {
    case CGameFont::alCenter: {
        return (m_wndSize.x) / 2;
    }
    break;
    case CGameFont::alLeft: {
        return 0;
    }
    break;
    case CGameFont::alRight: {
        return (m_wndSize.x);
    }
    break;
    case CGameFont::alJustified: {
        return 0;
    }
    break;
    default: NODEFAULT;
    }
#ifdef DEBUG
    return 0;
#endif
}

float CUILines::GetVIndentByAlign()
{
    float r = 0;

    switch (m_eVTextAlign)
    {
    case valTop: {
        float delta_h = m_pFont->ScaleHeightDelta();
        UI()->ClientToScreenScaledHeight(delta_h);
        return delta_h * -1;
    }
    case valCenter: r = (m_wndSize.y - GetVisibleHeight()) / 2; break;
    case valBotton: r = m_wndSize.y - GetVisibleHeight(); break;
    default: NODEFAULT;
    }

    // UI()->ClientToScreenScaledHeight(r);

    return r;

#ifdef DEBUG
    return 0;
#endif
}

// %c[255,255,255,255] or %c[255,255,255]
u32 CUILines::GetColorFromText(const xr_string& str) const
{
    //	typedef xr_string::size_type size;

    StrSize begin, end, comma1_pos, comma2_pos, comma3_pos;

    begin = str.find(BEGIN);
    end = str.find(END, begin);
    R_ASSERT2(npos != begin, "CUISubLine::GetColorFromText -- can't find beginning tag %c[");
    R_ASSERT2(npos != end, "CUISubLine::GetColorFromText -- can't find ending tag ]");

    // try default color
    if (npos != str.find("%c[default]", begin, end - begin))
        return m_dwTextColor;

    // Try predefined in XML colors
    //	CUIXmlInit xml;
    for (CUIXmlInit::ColorDefs::const_iterator it = CUIXmlInit::GetColorDefs()->begin(); it != CUIXmlInit::GetColorDefs()->end(); ++it)
    {
        int cmp = str.compare(begin + 3, end - begin - 3, *it->first);
        if (cmp == 0)
            return it->second;
    }

    // try parse values separated by commas
    comma1_pos = str.find(',', begin);
    comma2_pos = str.find(',', comma1_pos + 1);
    comma3_pos = str.find(',', comma2_pos + 1);

    // R_ASSERT2(npos != comma1_pos, "CUISubLine::GetColorFromText -- can't find first comma");
    // R_ASSERT2(npos != comma2_pos, "CUISubLine::GetColorFromText -- can't find second comma");
    // R_ASSERT2(npos != comma3_pos, "CUISubLine::GetColorFromText -- can't find third comma");
    //  commented by Zander

    u32 a, r, g, b;
    xr_string single_color;

    begin += 3;

    // Этот код воспринимает цвет и в ARGB, и в RGB формате. При не-валидном коде цвета устанавливается ARGB 255,200,200,200.
    if (npos != comma1_pos && npos != comma2_pos)
    { // have 2+ comma`s
        if (npos != comma3_pos)
        { // ARGB code found
            single_color = str.substr(begin, comma1_pos - 1);
            a = atoi(single_color.c_str());
            single_color = str.substr(comma1_pos + 1, comma2_pos - 1);
            r = atoi(single_color.c_str());
            single_color = str.substr(comma2_pos + 1, comma3_pos - 1);
            g = atoi(single_color.c_str());
            single_color = str.substr(comma3_pos + 1, end - 1);
            b = atoi(single_color.c_str());
        }
        else
        { // RGB code found
            a = 255;
            single_color = str.substr(begin, comma1_pos - 1);
            r = atoi(single_color.c_str());
            single_color = str.substr(comma1_pos + 1, comma2_pos - 1);
            g = atoi(single_color.c_str());
            single_color = str.substr(comma2_pos + 1, end - 1);
            b = atoi(single_color.c_str());
        }
    }
    else
    { // necessary comma not contains
        Msg("!not valid text-color code detected. [%s]", str.c_str());
        a = 255;
        r = 200;
        g = 200;
        b = 200;
    }

    return color_argb(a, r, g, b);
}

CUILine* CUILines::ParseTextToColoredLine(const xr_string& str)
{
    CUILine* line = xr_new<CUILine>();
    xr_string tmp(str);

    u32 color;

    do
    {
        xr_string entry;
        CutFirstColoredTextEntry(entry, color, tmp);
        line->AddSubLine(std::move(entry), subst_alpha(color, color_get_A(GetTextColor())));
    } while (!tmp.empty());

    return line;
}

void CUILines::CutFirstColoredTextEntry(xr_string& entry, u32& color, xr_string& text) const
{
    entry.clear();

    StrSize begin = text.find(BEGIN);
    StrSize end = text.find(END, begin);
    if (xr_string::npos == end)
        begin = end;
    StrSize begin2 = text.find(BEGIN, end);
    StrSize end2 = text.find(END, begin2);
    if (xr_string::npos == end2)
        begin2 = end2;

    // if we do not have any color entry or it is single with 0 position
    if (xr_string::npos == begin)
    {
        entry = text;
        color = m_dwTextColor;
        text.clear();
    }
    else if (0 == begin && xr_string::npos == begin2)
    {
        entry = text;
        color = GetColorFromText(entry);
        entry.replace(begin, end - begin + 1, "");
        text.clear();
    }
    // if we have color entry not at begin
    else if (0 != begin)
    {
        entry = text.substr(0, begin);
        color = m_dwTextColor;
        text.replace(0, begin, "");
    }
    // if we have two color entries. and first has 0 position
    else if (0 == begin && xr_string::npos != begin2)
    {
        entry = text.substr(0, begin2);
        color = GetColorFromText(entry);
        entry.replace(begin, end - begin + 1, "");
        text.replace(0, begin2, "");
    }
}

void CUILines::SetWndSize_inline(const Fvector2& wnd_size) { m_wndSize = wnd_size; }

void CUILines::IncCursorPos()
{
    const int txt_len = (int)m_text.size();

    if (0 == txt_len || m_iCursorPos >= txt_len)
        return;

    if (m_text_mask.at(m_iCursorPos))
        m_iCursorPos += 2;
    else
        m_iCursorPos++;
}

void CUILines::DecCursorPos()
{
    if (m_text.empty() || m_iCursorPos <= 0)
        return;

    if (m_text_mask.at(m_iCursorPos - 1))
        m_iCursorPos -= 2;
    else
        m_iCursorPos--;
}

void CUILines::UpdateCursor()
{
    if (uFlags.test(flComplexMode) && !m_text.empty())
    {
        ParseText();
        const int sz = (int)m_lines.size();
        int len = 0;
        for (int i = 0; i < sz; i++)
        {
            int curlen = m_lines[i].GetSize();
            if (m_iCursorPos <= len + curlen)
            {
                m_cursor_pos.y = i;
                m_cursor_pos.x = m_iCursorPos - len;
                return;
            }
            len += curlen;
        }
        R_ASSERT(false);
    }
    else
    {
        m_cursor_pos.y = 0;
        m_cursor_pos.x = m_iCursorPos;
    }
}
