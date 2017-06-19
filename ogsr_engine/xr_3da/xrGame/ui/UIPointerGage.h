//////////////////////////////////////////////////////////////////////////
// UIPointerGage.h:			цифровой прибор
//////////////////////////////////////////////////////////////////////////


#pragma once


#include "UIStatic.h"

class CUIPointerGage: public CUIStatic
{
private:
	typedef CUIStatic inherited;
public:

	CUIPointerGage	();
	virtual ~CUIPointerGage	();


	virtual void Init		(LPCSTR tex_name, 
							float x, float y, float width, float height);

	virtual void InitPointer(LPCSTR arrow_tex_name,  float arrow_offset_x, float arrow_offset_y,
		                     float angle_min, float angle_max);
	
	virtual void Update		();
	virtual void Draw		();

	//value - от 0 до 1
	virtual void SetValue	(float value);

protected:
	float m_fAngleMin, m_fAngleMax;
	float m_fAngle;

	//текстура стрелки
    CUIStaticItem	m_ArrowPointer;
	float m_iArrowOffsetX, m_iArrowOffsetY;
};