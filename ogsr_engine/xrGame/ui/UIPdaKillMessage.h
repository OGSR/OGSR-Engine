// File:		UIPdaKillMessage.h
// Description:	HUD message about player death. Implementation of visual behavior
// Created:		10.03.2005
// Author:		Serge Vynnychenko
// Mail:		narrator@gsc-game.kiev.ua
// 
// Copyright 2005 GSC GameWorld

#include "KillMessageStruct.h"
#include "UIPdaMsgListItem.h"

class CUIPdaKillMessage : public CUIStatic {
typedef CUIPdaMsgListItem inherited;
public:
	CUIPdaKillMessage();
	virtual ~CUIPdaKillMessage();

			void Init(KillMessageStruct& msg);
	virtual void SetFont(CGameFont* pFont);
	virtual void SetTextColor(u32 color);
	virtual void SetColor(u32 color);

protected:
			float InitText(CUIStatic& refStatic, float x, PlayerInfo& info);
			float InitIcon(CUIStatic& refStatic, float x, IconInfo& info);

    CUIStatic	m_victim_name;
	CUIStatic	m_initiator;
	CUIStatic	m_killer_name;
	CUIStatic	m_ext_info;
};
