//=============================================================================
//  Filename:   UIGameLog.h
//	Created by Vitaly 'Mad Max' Maximov, mad-max@gsc-game.kiev.ua
//	Copyright 2005. GSC Game World
//	---------------------------------------------------------------------------
//  Multiplayer game log window
//=============================================================================

#ifndef UI_GAME_LOG_H_
#define UI_GAME_LOG_H_

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "UIDialogWnd.h"
//#include "UIListWnd.h"
#include "UIScrollView.h"
#include "KillMessageStruct.h"

class CUIXml;
class CUIPdaKillMessage;
class CUIPdaMsgListItem;
class CUIStatic;

//////////////////////////////////////////////////////////////////////////

class CUIGameLog: public CUIScrollView
{
public:
	CUIGameLog();
	virtual ~CUIGameLog();
	CUIStatic*				AddLogMessage	(LPCSTR msg);
	CUIPdaKillMessage*		AddLogMessage	(KillMessageStruct& msg);
	CUIPdaMsgListItem*		AddPdaMessage	(LPCSTR msg, float delay);
	void					AddChatMessage	(LPCSTR msg, LPCSTR author);
	virtual void			Update			();

	void					SetTextAtrib	(CGameFont* pFont, u32 color);
	u32						GetTextColor	();

private:

	//typedef xr_set<int, std::greater<int> > ToDelIndexes;
	//typedef ToDelIndexes::iterator			ToDelIndexes_it;
	xr_vector<CUIWindow*>		toDelList;
	float						kill_msg_height;
	u32							txt_color;
};

//////////////////////////////////////////////////////////////////////////

#endif