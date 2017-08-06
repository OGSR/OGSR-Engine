// file:		UISpinNum.cpp
// description:	Spin Button with numerical data (unlike text data)
// created:		15.06.2005
// author:		Serge Vynnychenko
//

#include "StdAfx.h"
#include "UISpinNum.h"
#include "UILines.h"

CUISpinNum::CUISpinNum()
:	m_iVal(0),
	m_iMin(0),
	m_iMax(100),
	m_iStep(1)
{

}

void CUISpinNum::SetCurrentValue()
{
	GetOptIntegerValue(m_iVal, m_iMin, m_iMax);
	SetValue();
}

void CUISpinNum::SaveValue()
{
	CUIOptionsItem::SaveValue();
	SaveOptIntegerValue(m_iVal);
}

bool CUISpinNum::IsChanged()
{
	int val, min, max;
	GetOptIntegerValue(val, min, max);
    return m_iVal != val;
}

void CUISpinNum::Init(float x, float y, float width, float height)
{
	CUICustomSpin::Init(x,y,width,height);
	SetValue();
}

void CUISpinNum::IncVal()
{
	if (CanPressUp())
		m_iVal += m_iStep;

	SetValue();
}

void CUISpinNum::DecVal()
{
	if (CanPressDown())
		m_iVal -= m_iStep;

	SetValue();
}

void CUISpinNum::OnBtnUpClick()
{
	IncVal						();
	CUICustomSpin::OnBtnUpClick	();
}

void CUISpinNum::OnBtnDownClick()
{
	DecVal						();
	CUICustomSpin::OnBtnDownClick();
}

void CUISpinNum::SetValue()
{
	string16	buff;
	m_pLines->SetText(itoa(m_iVal, buff, 10)); 
}

bool CUISpinNum::CanPressUp()
{
	return m_iVal + m_iStep <= m_iMax;

}

bool CUISpinNum::CanPressDown()
{
	return m_iVal - m_iStep >= m_iMin;
}

CUISpinFlt::CUISpinFlt()
:	m_fVal(0),
	m_fMin(0),
	m_fMax(100),
	m_fStep(0.1f)
{}


void CUISpinFlt::SetCurrentValue()
{
	GetOptFloatValue(m_fVal, m_fMin, m_fMax);
	SetValue();
}

void CUISpinFlt::SaveValue()
{
	CUIOptionsItem::SaveValue();
	SaveOptFloatValue(m_fVal);
}

bool CUISpinFlt::IsChanged()
{
	float val, min, max;
	GetOptFloatValue(val, min, max);
    return !fsimilar		(m_fVal,val);
}

void CUISpinFlt::Init(float x, float y, float width, float height)
{
	CUICustomSpin::Init(x,y,width,height);
	SetValue();
}

void CUISpinFlt::IncVal()
{
	m_fVal += m_fStep;
	SetValue();
}

void CUISpinFlt::DecVal()
{
	m_fVal -= m_fStep;
	SetValue();
}

void CUISpinFlt::OnBtnUpClick()
{
	IncVal		();

	CUICustomSpin::OnBtnUpClick();
}

void CUISpinFlt::OnBtnDownClick()
{
	DecVal			();

	CUICustomSpin::OnBtnDownClick();
}

void CUISpinFlt::SetValue()
{
	clamp					(m_fVal, m_fMin, m_fMax);
	string16				buff;
	sprintf_s					(buff,"%.1f",m_fVal);
	m_pLines->SetText		(buff); 
}

bool CUISpinFlt::CanPressUp()
{
	return m_fVal + m_fStep <= m_fMax;

}

bool CUISpinFlt::CanPressDown()
{
	return (m_fVal - m_fStep > m_fMin) || fsimilar(m_fVal-m_fStep, m_fMin);
}
