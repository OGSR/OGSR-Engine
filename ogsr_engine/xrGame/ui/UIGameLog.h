//=============================================================================
//  Filename:   UIGameLog.h
//	Created by Vitaly 'Mad Max' Maximov, mad-max@gsc-game.kiev.ua
//	Copyright 2005. GSC Game World
//	---------------------------------------------------------------------------
//  Multiplayer game log window
//=============================================================================

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "UIDialogWnd.h"
//#include "UIListWnd.h"
#include "UIScrollView.h"

class CUIXml;
class CUIPdaMsgListItem;
class CUIStatic;

class CUIGameLog : public CUIScrollView
{
public:
    CUIGameLog();
    virtual ~CUIGameLog();
    CUIPdaMsgListItem* AddPdaMessage(LPCSTR msg, float delay);
    virtual void Update();

    void SetTextAtrib(CGameFont* pFont, u32 color);
    u32 GetTextColor();

private:
    // typedef xr_set<int, std::greater<int> > ToDelIndexes;
    // typedef ToDelIndexes::iterator			ToDelIndexes_it;
    xr_vector<CUIWindow*> toDelList;
    float kill_msg_height;
    u32 txt_color;
};
