#pragma once

#include "UIWindow.h"
#include "UIStatic.h"
#include "KillMessageStruct.h"

class CUIXml;
class CUIColorAnimatorWrapper;
class CUIGameLog;

class CUIMoneyIndicator: public CUIWindow {
public:
	CUIMoneyIndicator();
	virtual ~CUIMoneyIndicator();
	virtual void Update();
			void InitFromXML(CUIXml& xml_doc);
			void SetMoneyAmount(LPCSTR money);
			void SetMoneyChange(LPCSTR money);
			//void SetMoneyBonus(LPCSTR money);
			void AddBonusMoney(KillMessageStruct& msg);

protected:
	CUIStatic	m_back;
	CUIStatic	m_money_amount;
	CUIStatic	m_money_change;
//	CUIStatic	m_money_bonus;

	CUIColorAnimatorWrapper* m_pAnimChange;
//	CUIColorAnimatorWrapper* m_pAnimBonus;

	CUIGameLog*			m_pBonusMoney;
};