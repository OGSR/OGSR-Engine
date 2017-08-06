//////////////////////////////////////////////////////////////////////
// UIRadioButton.h: класс кнопки, имеющей 2 состояния
// и работающей в группе с такими же кнопками
//////////////////////////////////////////////////////////////////////

#ifndef _UI_RADIO_BUTTON_H_
#define _UI_RADIO_BUTTON_H_


#pragma once
#include "UITabButton.h"

class CUIRadioButton : public CUITabButton
{
public:
	virtual void Init(float x, float y, float width, float height);
	virtual void InitTexture(LPCSTR tex_name);
	virtual void SetTextX(float x)	{/*do nothing*/}
};


#endif