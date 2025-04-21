//////////////////////////////////////////////////////////////////////
// UIRadioButton.h: класс кнопки, имеющей 2 состояния
// и работающей в группе с такими же кнопками
//////////////////////////////////////////////////////////////////////

#pragma once

#include "UITabButton.h"

class CUIRadioButton : public CUITabButton
{
public:
    CUIRadioButton(void);
    virtual ~CUIRadioButton(void);

    virtual void Init(float x, float y, float width, float height);
    virtual void InitTexture(LPCSTR tex_name);
    virtual void SetTextX(float x)
    { /*do nothing*/
    }
};
