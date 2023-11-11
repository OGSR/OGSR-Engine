// File:		UILine.cpp
// Description:	Single text line
// Created:		05.04.2005
// Author:		Serge Vynnycheko
// Mail:		narrator@gsc-game.kiev.ua
//
// Copyright 2005 GSC Game World

#include "StdAfx.h"
#include "UILine.h"
#include "uilinestd.h"

//#define LOG_ALL_LINES
#ifdef LOG_ALL_LINES
int ListLinesCount = 0;
struct DBGList
{
    CUILine* wnd;
    int num;
};
xr_vector<DBGList> dbg_list_lines;
void dump_list_lines()
{
    Msg("------Total  Lines %d", dbg_list_lines.size());
    xr_vector<DBGList>::iterator _it = dbg_list_lines.begin();
    for (; _it != dbg_list_lines.end(); ++_it)
        Msg("--leak detected ---- Line = %d", (*_it).num);
}
#else
void dump_list_lines() {}
#endif

CUILine::CUILine()
{
    m_tmpLine = nullptr;

#ifdef LOG_ALL_LINES
    ListLinesCount++;
    dbg_list_lines.push_back(DBGList());
    dbg_list_lines.back().wnd = this;
    dbg_list_lines.back().num = ListLinesCount;
#endif
}

CUILine::~CUILine()
{
    xr_delete(m_tmpLine);

#ifdef LOG_ALL_LINES
    xr_vector<DBGList>::iterator _it = dbg_list_lines.begin();
    bool bOK = false;
    for (; _it != dbg_list_lines.end(); ++_it)
    {
        if ((*_it).wnd == this)
        {
            bOK = true;
            dbg_list_lines.erase(_it);
            break;
        }
    }
    if (!bOK)
        Msg("CUILine::~CUILine()!!!!!!!!!!!!!!!!!!!!!!! cannot find window in list");
#endif
}

CUILine::CUILine(const CUILine& other)
{
    m_subLines = other.m_subLines;
    m_tmpLine = NULL;
#ifdef LOG_ALL_LINES
    ListLinesCount++;
    dbg_list_lines.push_back(DBGList());
    dbg_list_lines.back().wnd = this;
    dbg_list_lines.back().num = ListLinesCount;
#endif
}

CUILine& CUILine::operator=(const CUILine& other)
{
    m_subLines = other.m_subLines;
    xr_delete(m_tmpLine);
    return (*this);
}

void CUILine::AddSubLine(const xr_string& str, u32 color)
{
    auto& sline = m_subLines.emplace_back();
    sline.m_color = color;
    sline.m_text = str;
}

void CUILine::AddSubLine(xr_string&& str, u32 color)
{
    auto& sline = m_subLines.emplace_back();
    sline.m_color = color;
    sline.m_text = std::move(str);
}

void CUILine::AddSubLine(const CUISubLine* subLine) { m_subLines.push_back(*subLine); }

void CUILine::Clear() { m_subLines.clear(); }

void CUILine::ProcessNewLines()
{
    // ("SATAN->SATAN: sometimes working badly (leaves empty lines)")
    for (u32 i = 0; i < m_subLines.size(); i++)
    {
        StrSize pos = m_subLines[i].m_text.find("\\n");
        if (pos != npos)
        {
            CUISubLine sbLine;
            if (pos)
            {
                sbLine = *m_subLines[i].Cut2Pos((int)pos - 1);
            }
            sbLine.m_last_in_line = true;

            m_subLines.insert(m_subLines.begin() + i, sbLine);
            m_subLines[i + 1].m_text.erase(0, 2);

            if (m_subLines[i + 1].m_text.empty())
            {
                m_subLines.erase(m_subLines.begin() + i + 1);
            }
        }
    }
}

void CUILine::ProcessSpaces()
{
    for (u32 i = 0; i < m_subLines.size(); i++)
    {
        const StrSize pos = m_subLines[i].m_text.find(' ');
        if (pos != npos)
        {
            CUISubLine sbLine;
            if (pos)
            {
                sbLine = *m_subLines[i].Cut2Pos((int)pos - 1);
            }

            m_subLines.insert(m_subLines.begin() + i, sbLine);
            m_subLines[i + 1].m_text.erase(0, 1); // only one char

            if (m_subLines[i + 1].m_text.empty())
            {
                m_subLines.erase(m_subLines.begin() + i + 1);
            }
        }
    }
}

void CUILine::Draw(CGameFont* pFont, float x, float y, float max_w) const
{
    const int size = m_subLines.size();

    float total_w = 0.f;
    
    int last_nonEmpty = 0;
    int nonEmptyCount = 0;

    for (int i = 0; i < size; i++)
    {
        float w = pFont->SizeOf_(m_subLines[i].m_text.c_str());

        UI()->ClientToScreenScaledWidth(w);

        total_w += w;
        if (w > 0.f)
        {
            nonEmptyCount++;
            last_nonEmpty = i;
        }
    }

    float def_add_w = 0.f; 
    bool use_def_add_w = false;

    float add_w = 0.f;

    if (pFont->GetAligment() == CGameFont::alJustified)
    {
        def_add_w = pFont->SizeOf_(' ');

        UI()->ClientToScreenScaledWidth(def_add_w);

        const float space_w = max_w - total_w - def_add_w * (size - nonEmptyCount);

        add_w = space_w / (float)(nonEmptyCount - 1);

        if (total_w < max_w * 3/4)
        {
            add_w = def_add_w;
            use_def_add_w = true;
        }
    }

    float length = 0;

    for (int i = 0; i < size; i++)
    {
        float w = pFont->SizeOf_(m_subLines[i].m_text.c_str());
        UI()->ClientToScreenScaledWidth(w);

        float d = length;

        if (pFont->GetAligment() == CGameFont::alCenter)
        {
            // center align
            d += (w / 2 - total_w / 2);
        }
        else if (pFont->GetAligment() == CGameFont::alRight)
        {
            // right align
            d += (w - total_w);
        }
        else if (pFont->GetAligment() == CGameFont::alJustified)
        {
            if (i == last_nonEmpty && !use_def_add_w)
            {
                d += max_w - length - w;
            }
        }

        m_subLines[i].Draw(pFont, x + d, y);

        length += w + (w > 0.f ? add_w : def_add_w);
    }
}

int CUILine::GetSize()
{
    int sz = 0;
    int size = m_subLines.size();
    for (int i = 0; i < size; i++)
        sz += (int)m_subLines[i].m_text.size();

    return sz;
}

const CUILine* CUILine::GetEmptyLine()
{
    xr_delete(m_tmpLine);
    m_tmpLine = xr_new<CUILine>();
    return m_tmpLine;
}
