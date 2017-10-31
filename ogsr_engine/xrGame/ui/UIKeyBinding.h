#pragma once
#include "UIWindow.h"
#include "UILabel.h"
#include "UIListWnd.h"
#include "UIFrameWindow.h"
#include "UIEditBox.h"

class CUIXml;
class CUIScrollView;

class CUIKeyBinding : public CUIWindow 
{
public:
					CUIKeyBinding			();
	void			InitFromXml				(CUIXml& xml_doc, LPCSTR path);
#ifdef DEBUG
	void			CheckStructure			(CUIXml& xml_doc);
	bool			IsActionExist			(LPCSTR action, CUIXml& xml_doc);
#endif
protected:
	void			FillUpList				(CUIXml& xml_doc, LPCSTR path);

#ifdef REMOVE_ALTERNATIVE_KEYBOARD_BINDING
	CUILabel		m_header[2];
#else
	CUILabel		m_header[3];
#endif
	CUIFrameWindow	m_frame;
	CUIScrollView*	m_scroll_wnd;
};