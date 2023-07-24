// File:		UILine.h
// Description:	Single text line
// Created:		11.03.2005
// Author:		Serge Vynnycheko
// Mail:		narrator@gsc-game.kiev.ua
//
// Copyright 2005 GSC Game World

#pragma once

#include "UISubLine.h"
#include "../HUDManager.h"
#include "UIColorAnimatorWrapper.h"

// Attention! Destructor is not virtual.
// if you want to inherit this class then make _coresponding_ changes
class CUILine
{
    friend class CUILines;

public:
    CUILine();
    CUILine(const CUILine& other);

    ~CUILine();
    
    CUILine& operator=(const CUILine& other);

    void AddSubLine(const xr_string& str, u32 color);
    void AddSubLine(xr_string&& str, u32 color);
    void AddSubLine(const CUISubLine* subLine);

    void Clear();

    void ProcessNewLines();
    void ProcessSpaces();

    void Draw(CGameFont* pFont, float x, float y, float max_w) const;
    bool IsEmpty() const { return m_subLines.empty(); }

protected:
    int GetSize();

    const CUILine* GetEmptyLine();
    
    xr_vector<CUISubLine> m_subLines;

    CUILine* m_tmpLine;
};
