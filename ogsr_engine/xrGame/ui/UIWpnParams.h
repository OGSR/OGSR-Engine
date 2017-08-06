#pragma once
#include "UIWindow.h"

#include "UIProgressBar.h"

class CUIXml;

#include "../script_export_space.h"

struct SLuaWpnParams;
class CPhysicsShellHolder;

class CUIWpnParams : public CUIWindow 
{
public:
								CUIWpnParams			();
	virtual						~CUIWpnParams			();

	void 						InitFromXml				(CUIXml& xml_doc);
	void 						SetInfo					(const shared_str& wnp_section);
	bool 						Check					(CPhysicsShellHolder &obj/*const shared_str& wpn_section*/);

protected:
	CUIProgressBar				m_progressAccuracy;
	CUIProgressBar				m_progressHandling;
	CUIProgressBar				m_progressDamage;
	CUIProgressBar				m_progressRPM;

	CUIStatic					m_textAccuracy;
	CUIStatic					m_textHandling;
	CUIStatic					m_textDamage;
	CUIStatic					m_textRPM;
};