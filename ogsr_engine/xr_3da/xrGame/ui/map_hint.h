#pragma once
#include "UIWindow.h"

class CUIStatic;
class CUIFrameWindow;

class CUIMapHint :public CUIWindow
{
	typedef CUIWindow inherited;
	CUIStatic*			m_text;
	CUIFrameWindow*		m_border;
	CUIWindow*			m_owner;
public:
					CUIMapHint	():m_owner(NULL){};
	virtual 		~CUIMapHint	();
	void			Init		();
	void			SetText		(LPCSTR text);
	virtual void	Draw		(){return;};
			void	Draw_		();
	void			SetOwner	(CUIWindow* w)	{m_owner = w;}
	CUIWindow*		GetOwner	()				{return m_owner;}
};