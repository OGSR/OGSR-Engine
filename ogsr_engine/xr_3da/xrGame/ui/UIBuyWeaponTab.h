// File:		UIBuyWeaponTab.h
// Description:	Tab conorol for BuyWeaponWnd
// Created:		07.02.2005
// Author:		Serge Vynnychenko
// Mail:		narrator@gsc-game.kiev.ua

// Copyright 2005 GSC Game World

#pragma once

#include "UITabControl.h"
#include "UIStatic.h"

class CUIXml;

class CUIBuyWeaponTab : public CUITabControl 
{
public:
				CUIBuyWeaponTab				();
	virtual		~CUIBuyWeaponTab			();

	virtual void Init						(CUIXml* xml, char* path);
	virtual void OnTabChange				(int iCur, int iPrev);
			void SetActiveState				(bool bState = true);
private:
	bool		m_bActiveState;
	int			m_iStubIndex;
};
