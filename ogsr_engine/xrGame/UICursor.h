#pragma once

#include "ui_base.h"
#include "UIStaticItem.h"

class CUIStatic;

class CUICursor:public pureRender
{
	bool			bVisible;
	Fvector2		vPos;
	Fvector2		vPrevPos;

	CUIStatic*		m_static;
	void			InitInternal	();
public:
					CUICursor		();
	virtual			~CUICursor		();
	virtual void	OnRender		();
	
	Fvector2		GetCursorPositionDelta();

	Fvector2		GetCursorPosition		();
	void			SetUICursorPosition		(Fvector2 pos);
	void			UpdateCursorPosition	();

	bool			IsVisible		() {return bVisible;}
	void			Show			() {bVisible = true;}
	void			Hide			() {bVisible = false;}
};
