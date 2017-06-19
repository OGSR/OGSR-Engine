#pragma once
#include "UITabButton.h"

class CUITabButtonMP : public CUITabButton 
{
	typedef CUITabButton inherited;
public:
					CUITabButtonMP			();
	virtual void 	Draw					();
	virtual void 	Update					();
	virtual void 	UpdateTextAlign			();	
	virtual void 	DrawHighlightedText		()	{}
	virtual void 	SendMessage				(CUIWindow* pWnd, s16 msg, void* pData);

	virtual bool 	IsEnabled				()					{ return true; }
			void 	SetOrientation			(bool bVert)		{m_orientationVertical = bVert;};
	u32				m_temp_index;
		Fvector2	m_text_ident_pushed;
		Fvector2	m_text_ident_cursor_over;
private:
    bool			m_orientationVertical;

};