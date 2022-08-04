// File:		UIMessagesWindow.h
// Description:	Window with MP chat and Game Log ( with PDA messages in single and Kill Messages in MP)
// Created:		22.04.2005
// Author:		Serge Vynnychenko
// Mail:		narrator@gsc-game.kiev.ua
//
// Copyright 2005 GSC Game World

#pragma once

#include "UIWindow.h"
#include "../pda_space.h"
#include "../InfoPortionDefs.h"

class CUIGameLog;
class game_cl_GameState;
class CUIPdaMsgListItem;
class CUIProgressShape;

class CUIMessagesWindow : public CUIWindow
{
public:
    CUIMessagesWindow();
    virtual ~CUIMessagesWindow();

    void AddIconedPdaMessage(LPCSTR textureName, Frect originalRect, LPCSTR message, int iDelay);

    virtual void Update();

protected:
    virtual void Init(float x, float y, float width, float height);

    CUIGameLog* m_pGameLog;
    //	Frect				m_ListPos2;
};