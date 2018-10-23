#pragma once

#include "uiwindow.h"
//#include "uipointergage.h"


class CUICarPanel : public CUIWindow
{
private:
	typedef CUIWindow inherited;

	CUIStatic			UIStaticCarHealth;
	CUIProgressBar		UICarHealthBar;
//	CUIPointerGage		UISpeedometer;
//	CUIPointerGage		UITachometer;
public: 

	// Установить 
	void				SetCarHealth	(float value);
	void				SetSpeed		(float speed);
	void				SetRPM			(float rmp);
	void				Init			(float x, float y, float width, float height);
};