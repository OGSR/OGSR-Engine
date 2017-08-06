#pragma once
#include "uibutton.h"

class CUIScrollBox :public CUIStatic
{
	typedef	CUIStatic			inherited;
public:
								CUIScrollBox			();

			void 				SetHorizontal			();
			void 				SetVertical				();

	virtual bool				OnMouse					(float x, float y, EUIMessages mouse_action);
	virtual void				Draw					();
						
protected:
	bool						m_bIsHorizontal;
};
